# bsp_len9252 使用方案

> **版本**: V1.0 | **作者**: Yharim | **日期**: 2026-05-22

---

## 1. 快速开始

### 1.1 文件清单

```
工程根目录/
├── Inc/
│   └── bsp_len9252.h          # 驱动头文件（类型定义 + API 声明）
├── Src/
│   └── bsp_len9252.c          # 驱动实现（默认接口 + 实例化逻辑）
├── Inc/bsp/spiflash/
│   └── bsp_spiflash.h         # SPI Flash BSP（含 SPIWriteByte 便捷宏）
└── Src/
    └── el9800appl.c           # 应用层调用示例
```

### 1.2 最小初始化代码

```c
#include "bsp_len9252.h"

int main(void)
{
    // [1] 系统启动
    HAL_Init();
    SystemClock_Config();

    // [2] 硬件初始化（GPIO、SPI、UART、定时器、外部中断）
    GPIO_Config();
    MX_SPIFlash_Init();
    MX_DEBUG_USART_Init();
    RST_Configuration();
    EXTI0_Configuration();
    EXTI1_Configuration();
    EXTI3_Configuration();
    TIM_Configuration(10);

    // [3] 依赖注入 + 驱动实例化
    len9252_inst(&len9252_handle,
                 &len9252_south_api,
                 &len9252_north_api,
                 &len9252_time_api,
                 &len9252_isr_api,
                 &len9252_debug_api);

    // [4] 运行自测（开发阶段）
    // len9252_all_unit_test(&len9252_handle);

    // [5] 进入主循环
    while (1) { }
}
```

---

## 2. 自定义底层接口

默认接口均以 `__weak` 修饰，用户可在任意 `.c` 文件中重新定义来覆盖。

### 2.1 覆盖 SPI 写函数

```c
// 覆盖默认的 my_spi_write
len9252_tatus_t my_spi_write(uint8_t data)
{
    // 等待 SPI 发送完成
    while (__HAL_SPI_GET_FLAG(&hspix, SPI_FLAG_TXE) == RESET);
    HAL_SPI_Transmit(&hspix, &data, 1, 100);
    return EC_OK;
}
```

### 2.2 覆盖复位函数

```c
len9252_tatus_t my_rst_esc(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOF_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    return EC_OK;
}

len9252_tatus_t my_rst_reset(void)
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);
    return EC_OK;
}

len9252_tatus_t my_rst_set(void)
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_SET);
    return EC_OK;
}
```

### 2.3 覆盖毫秒延时函数

```c
len9252_tatus_t my_delay(uint32_t ms)
{
    HAL_Delay(ms);
    return EC_OK;
}
```

---

## 3. API 调用参考

### 3.1 通过句柄直接调用

```c
LEN9252_t* self = &len9252_handle;

// --- 系统级 ---
self->pf_sys_init();                                     // 系统初始化

// --- 时间 ---
uint32_t tick;
self->time_api->pf_get_time(&tick);                      // 获取系统 tick
self->time_api->pf_delay(500);                           // 延时 500ms

// --- 全局中断 ---
self->isr_api->pf_disable_global_int();                  // 关全局中断
// ... 临界区代码 ...
self->isr_api->pf_enable_global_int();                   // 开全局中断

// --- SPI ---
self->south_api->spi_api->pf_gpio_low();                 // CS 拉低
self->south_api->spi_api->pf_spi_write(0x80);            // 写命令字节
uint8_t val = self->south_api->spi_api->pf_spi_read();   // 读数据字节
self->south_api->spi_api->pf_gpio_high();                // CS 拉高

// --- SPI 双字操作 ---
self->south_api->spi_api->pf_spi_write_dword(0x0123, 0xDEADBEEF);
uint32_t data32;
self->south_api->spi_api->pf_spi_read_dword(0x0123, &data32);

// --- GPIO (复位控制) ---
self->south_api->gpio_api->pf_rst_esc();                 // 复位 GPIO 初始化
self->south_api->gpio_api->pf_rst_reset();               // RESET 拉低
self->south_api->gpio_api->pf_rst_set();                 // RESET 拉高

// --- 定时器 ---
self->south_api->timer_api->pf_it_init();                // 初始化 TIM2
self->south_api->timer_api->pf_timer_start();            // 启动定时中断
// ...
self->south_api->timer_api->pf_timer_stop();             // 停止定时中断
self->south_api->timer_api->pf_clear_it();               // 清除中断标志

// --- 外部中断 ---
self->south_api->exti_api->pf_exti1_init();              // 初始化 EXTI1
self->south_api->exti_api->pf_exti1_it_open();           // 使能 EXTI1 中断
self->south_api->exti_api->pf_exti1_it_close();          // 禁止 EXTI1 中断
self->south_api->exti_api->pf_exti1_clear();             // 清除 EXTI1 标志
```

### 3.2 通过便捷宏调用 (SPI Flash 场景)

`bsp_spiflash.h` 中定义的宏:

```c
#define SPIWriteByte    len9252_handle.south_api->spi_api->pf_spi_write
#define SPIReadByte()   len9252_handle.south_api->spi_api->pf_spi_read()
#define CSLOW()         SELECT_SPI    // CS 拉低
#define CSHIGH()        DESELECT_SPI  // CS 拉高
```

使用示例 (读 SPI Flash ID):

```c
uint32_t SPIReadDWord(uint16_t Address)
{
    UINT32_VAL dwResult;
    UINT16_VAL wAddr;
    wAddr.Val = Address;

    CSLOW();
    SPIWriteByte(CMD_FAST_READ);        // 发读命令
    SPIWriteByte(wAddr.byte.HB);        // 发地址高字节
    SPIWriteByte(wAddr.byte.LB);        // 发地址低字节
    SPIWriteByte(CMD_FAST_READ_DUMMY);  // Dummy byte

    dwResult.byte.LB = SPIReadByte();
    dwResult.byte.HB = SPIReadByte();
    dwResult.byte.UB = SPIReadByte();
    dwResult.byte.MB = SPIReadByte();
    CSHIGH();

    return dwResult.Val;
}
```

---

## 4. 中断服务函数集成

### 4.1 定时器中断

在 `stm32f4xx_it.c` 中:

```c
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htimx);
    // LEN9252 的 ISR 会通过 pf_it_isr 自动调用
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) {
        // EtherCAT 周期处理
    }
}
```

### 4.2 外部中断

```c
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin) {
        case GPIO_PIN_0: /* SYNC0 中断 */ break;
        case GPIO_PIN_1: /* SYNC1 中断 */ break;
        case GPIO_PIN_3: /* 其他中断 */ break;
    }
}
```

---

## 5. 完整硬件初始化清单

`my_sys_init()` (在 `bsp_len9252.c` 中, 可覆盖) 包含了推荐的初始化顺序:

| 步骤 | 函数 | 说明 |
|------|------|------|
| 1 | `HAL_Init()` | HAL 库初始化 |
| 2 | `SystemClock_Config()` | 系统时钟配置 |
| 3 | `GPIO_Config()` | 通用 GPIO 初始化 |
| 4 | `MX_SPIFlash_Init()` | SPI1 Flash 接口初始化 |
| 5 | `MX_DEBUG_USART_Init()` | 调试串口初始化 |
| 6 | `RST_Configuration()` | 复位引脚初始化 |
| 7 | `EXTI0/1/3_Configuration()` | 外部中断配置 (用户补全) |
| 8 | `TIM_Configuration(10)` | 定时器初始化 |

---

## 6. 调试开关

在 `bsp_len9252.h` 中:

```c
#define EC_DEBUG                          // 注释此行关闭调试输出
#define EC_DEBUG_OUT(X,...)  printf(X, ##__VA_ARGS__)
```

调试输出会打印在串口上:
- 参数校验失败
- 驱动初始化状态
- 各接口函数被调用时

---

## 7. 常见问题

### Q1: `len9252_handle->xxx` 编译报错 "expression must have pointer type"

`len9252_handle` 是 `LEN9252_t` 结构体值（不是指针），访问成员要用 `.` 而不是 `->`:

```c
// 正确
len9252_handle.south_api->spi_api->pf_spi_write(0xAB);

// 错误
len9252_handle->south_api->spi_api->pf_spi_write(0xAB);
```

### Q2: 如何添加新的硬件接口？

1. 在 `bsp_len9252.h` 中定义新的接口结构体（如 `len9252_i2c_t`）
2. 将其指针加入 `len9252_South_t` 或 `LEN9252_t`
3. 在 `bsp_len9252.c` 中创建默认实例并绑定

### Q3: 多个 SPI 设备如何管理？

当前设计使用单一 `spi_api` 实例，多 SPI 设备需扩展 `len9252_spi_t` 增加 `pf_spi_select(uint8_t dev_id)` 接口，通过 CS 引脚切换设备。

### Q4: 如何移植到其他 MCU (非 STM32)？

只需替换 `bsp_len9252.c` 中所有 `my_*` 函数的实现，保持函数签名不变。头文件 `bsp_len9252.h` 无需修改。
