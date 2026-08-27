# bsp_len9252 软件架构说明书

> **版本**: V1.0 | **作者**: Yharim | **日期**: 2026-05-22

---

## 1. 概述

`bsp_len9252` 是 LEN9252 EtherCAT 从站控制器的 HAL 层驱动模块。它封装了 MCU 底层硬件（SPI、GPIO、定时器、外部中断）与上层 EtherCAT 协议栈之间的通信接口。

### 1.1 设计目标

- **硬件解耦**: 通过函数指针表抽象底层硬件，上层代码不直接依赖 STM32 HAL
- **依赖注入**: 所有外部依赖通过 `len9252_inst()` 接口注入，便于单元测试和平台移植
- **弱函数覆盖**: 默认实现使用 `__weak` 修饰，用户可根据实际硬件覆盖任意接口

### 1.2 核心特性

| 特性 | 实现方式 |
|------|---------|
| 硬件抽象 | 6 个函数指针接口结构体 |
| 实例管理 | 全局句柄 `len9252_handle` (LEN9252_t) |
| 状态机 | `LEN9252_UNINIT` → `LEN9252_INITED` |
| 错误码 | 统一状态码 `len9252_tatus_t` (8 种状态) |
| 调试支持 | `EC_DEBUG` 宏控制 printf 输出 |

---

## 2. 架构层次

```
┌──────────────────────────────────────────┐
│         EtherCAT 协议栈 (上层)             │
│         ecatslv / coeappl / ...           │
├──────────────────────────────────────────┤
│         North Interface                   │  ← len9252_North_t
│         (预留, 未来扩展)                    │
├──────────────────────────────────────────┤
│         LEN9252 Core Handle               │
│         len9252_handle (LEN9252_t)        │  ← 核心句柄
│         - len9252_inst()  实例化           │
│         - len9252_init()  初始化           │
│         - len9252_all_unit_test() 自测     │
├──────────────────────────────────────────┤
│  South Interface (MCU 底层抽象)            │
│  ┌─────────┬────────┬─────────┬────────┐ │
│  │ SPI API │GPIO API│Timer API│EXTI API│ │
│  └─────────┴────────┴─────────┴────────┘ │
├──────────────────────────────────────────┤
│  Time API    │ ISR API  │ Debug API       │  ← 辅助接口
├──────────────────────────────────────────┤
│         STM32F4xx HAL 层                   │
│         (SPI / GPIO / TIM / EXTI / NVIC)  │
├──────────────────────────────────────────┤
│         STM32F407 硬件                     │
└──────────────────────────────────────────┘
```

---

## 3. 核心数据结构

### 3.1 主句柄: LEN9252_t

```c
typedef struct LEN9252 {
    len9252_init_t      init_status;     // 初始化状态机
    len9252_tatus_t     (*pf_sys_init)(void);  // 系统初始化函数指针
    len9252_Time_t*     time_api;        // 时间接口
    len9252_isr_t*      isr_api;         // 全局中断接口
    len9252_debug_t*    debug_api;       // 调试接口
    len9252_South_t*    south_api;       // 南向接口 (MCU 底层)
    len9252_North_t*    north_api;       // 北向接口 (EtherCAT 上层)
} LEN9252_t;
```

### 3.2 南向接口 (South API): len9252_South_t

汇集所有 MCU 底层驱动接口:

```c
typedef struct {
    len9252_spi_t*      spi_api;         // SPI 读写 + CS 控制
    len9252_gpio_t*     gpio_api;        // ESC 复位控制
    len9252_timer_t*    timer_api;       // 定时中断管理
    len9252_exti_t*     exti_api;        // 外部中断管理
} len9252_South_t;
```

### 3.3 各子接口概览

| 接口结构体 | 函数指针数 | 职责 |
|-----------|-----------|------|
| `len9252_spi_t` | 6 | SPI 字节/双字读写、CS 高低电平控制 |
| `len9252_gpio_t` | 3 | ESC 复位初始化、RESET 拉低/拉高 |
| `len9252_timer_t` | 5 | 定时器初始化、启动/停止、清标志、ISR |
| `len9252_exti_t` | 11 | 3 路外部中断 (EXTI0/1/3) 的初始化/清除/开关 |
| `len9252_Time_t` | 2 | 获取系统 tick、毫秒延时 |
| `len9252_isr_t` | 2 | 全局中断 使能/禁止 |

---

## 4. 接口定义详解

### 4.1 SPI 接口 (len9252_spi_t)

| 函数指针 | 签名 | 功能 |
|---------|------|------|
| `pf_spi_write` | `(uint8_t data) -> len9252_tatus_t` | 写 1 字节到 SPI |
| `pf_spi_read` | `(void) -> uint8_t` | 从 SPI 读 1 字节 |
| `pf_gpio_high` | `(void) -> len9252_tatus_t` | CS 拉高 (片选释放) |
| `pf_gpio_low` | `(void) -> len9252_tatus_t` | CS 拉低 (片选使能) |
| `pf_spi_write_dword` | `(uint16_t Addr, uint32_t Val) -> len9252_tatus_t` | 向指定地址写 32 位 |
| `pf_spi_read_dword` | `(uint16_t Addr, uint32_t* data) -> len9252_tatus_t` | 从指定地址读 32 位 |

### 4.2 GPIO 接口 (len9252_gpio_t)

| 函数指针 | 功能 |
|---------|------|
| `pf_rst_esc` | ESC 复位 GPIO 初始化 |
| `pf_rst_reset` | RESET 引脚拉低 (PF8) |
| `pf_rst_set` | RESET 引脚拉高 (PF8) |

### 4.3 定时器接口 (len9252_timer_t)

| 函数指针 | 功能 |
|---------|------|
| `pf_it_init` | 初始化 TIM2，周期 10×100μs |
| `pf_clear_it` | 清除 TIM2 更新中断标志 |
| `pf_it_isr` | 定时器 ISR 入口 (映射到 `TIM2_IRQHandler`) |
| `pf_timer_start` | 使能 TIM2 NVIC 中断 |
| `pf_timer_stop` | 禁止 TIM2 NVIC 中断 |

### 4.4 外部中断接口 (len9252_exti_t)

| 函数指针 | 功能 |
|---------|------|
| `pf_exti0_init/clear` | EXTI0 初始化和清标志 |
| `pf_exti1_init/clear/it_open/it_close` | EXTI1 完整管理 |
| `pf_exti3_init/clear/it_open/it_close` | EXTI3 完整管理 |
| `pf_exti1_isr` | EXTI1 ISR 入口 (映射到 `EXTI0_IRQHandler`) |

---

## 5. 初始化流程

```
main()
  │
  └─ len9252_inst(&len9252_handle,
                   &len9252_south_api,   // 传入配置好的南向接口
                   &len9252_north_api,
                   &len9252_time_api,
                   &len9252_isr_api,
                   &len9252_debug_api)
       │
       ├─ [1] 校验所有参数非 NULL
       ├─ [2] 检查 init_status == LEN9252_UNINIT (防重复初始化)
       ├─ [3] 绑定所有接口指针到句柄
       ├─ [4] 绑定 pf_sys_init = my_sys_init
       ├─ [5] 调用 len9252_init(self) 内部初始化
       ├─ [6] 失败则回滚 (所有指针置 NULL)
       └─ [7] 成功则设置 init_status = LEN9252_INITED
```

---

## 6. 调用链路

以一次 SPI 读写为例:

```
EtherCAT 协议栈
  │
  └─ len9252_handle.south_api->spi_api->pf_spi_write(0xAB)
       │                                    │
       │    LEN9252_t (句柄)                  └─ 实际指向 spi_api.pf_spi_write
       │    .south_api ──────────────→ len9252_south_api
       │    .south_api->spi_api ────→ spi_api
       │    .south_api->spi_api->pf_spi_write → my_spi_write()
       │                                           │
       │                                           └─ SPIWrite(data)  [实际 HAL 操作]
       │
       └─ 返回 len9252_tatus_t
```

**关键**: `bsp_spiflash.h` 中提供了便捷宏:
```c
#define SPIWriteByte    len9252_handle.south_api->spi_api->pf_spi_write
#define SPIReadByte()   len9252_handle.south_api->spi_api->pf_spi_read()
```

注意 `len9252_handle` 是结构体值（非指针），使用 `.` 而非 `->`。

---

## 7. 错误码定义

| 枚举值 | 数值 | 含义 |
|--------|-----|------|
| `EC_OK` | 0 | 操作成功 |
| `EC_ERROR` | 1 | 运行时错误（无匹配场景） |
| `EC_ERRORTIMEOUT` | 2 | 操作超时 |
| `EC_ERRORRESOURCE` | 3 | 资源不可用（如重复初始化） |
| `EC_ERRORPARAMETER` | 4 | 参数错误（如 NULL 指针） |
| `EC_ERRORNOMEMORY` | 5 | 内存不足 |
| `EC_ERRORISR` | 6 | 不允许在 ISR 中执行 |
| `EC_RESERVED` | 7 | 预留扩展 |

---

## 8. 全局实例清单

| 变量名 | 类型 | 说明 |
|--------|------|------|
| `len9252_handle` | `LEN9252_t` | 驱动核心句柄 |
| `len9252_south_api` | `len9252_South_t` | 南向接口实例 |
| `len9252_north_api` | `len9252_North_t` | 北向接口实例 |
| `len9252_time_api` | `len9252_Time_t` | 时间接口实例 |
| `len9252_isr_api` | `len9252_isr_t` | 全局中断接口实例 |
| `len9252_debug_api` | `len9252_debug_t` | 调试接口实例 |
| `spi_api` | `len9252_spi_t` | SPI 接口实例 |
| `gpio_api` | `len9252_gpio_t` | GPIO 接口实例 |
| `timer_api` | `len9252_timer_t` | 定时器接口实例 |
| `exti_api` | `len9252_exti_t` | 外部中断接口实例 |

---

## 9. 设计模式

### 9.1 依赖注入 (Dependency Injection)

所有硬件依赖通过 `len9252_inst()` 的外部参数注入，驱动内部不直接调用任何 HAL 函数。这使得:
- 单元测试可替换为 Mock 函数
- 移植到其他 MCU 只需替换接口实现

### 9.2 策略模式 (Strategy Pattern)

每个子接口（SPI、GPIO、Timer、EXTI）是一组可替换的策略。用户可通过 `__weak` 覆盖任一函数实现。

### 9.3 外观模式 (Facade Pattern)

`LEN9252_t` 句柄作为统一入口，屏蔽了底层 6 个子接口的复杂性。上层只需持有句柄指针即可访问所有功能。
