# CubeMX 配置信息

> 本文档根据工程根目录的 `SlaveTestRemove.ioc`、CubeMX 生成代码以及当前 BSP 实现整理，用于后续代码剥离、硬件接口迁移和重新生成代码时对照。
>
> **适用工程**：`SlaveTestRemove`  
> **整理日期**：2026-08-27  
> **配置来源**：STM32CubeMX 6.17.0 / STM32Cube FW_F4 V1.28.3

---

## 1. 工程概览

| 项目 | 当前配置 |
|---|---|
| MCU | STM32F407ZET6 |
| CubeMX 识别型号 | STM32F407Z(E-G)Tx / STM32F407ZETx |
| MCU 系列 | STM32F4 / STM32F407/417 |
| 封装 | LQFP144 |
| 板卡类型 | `custom` |
| HAL 固件包 | STM32Cube FW_F4 V1.28.3 |
| CubeMX 版本 | 6.17.0 |
| CubeMX 数据库版本 | DB.6.0.170 |
| 目标工具链 | MDK-ARM V5.32 |
| 工程文件 | `SlaveTestRemove.ioc` |
| 代码生成目录 | `Core/Src`、`Core/Inc` |
| 工程保留用户代码 | 是（`ProjectManager.KeepUserCode=true`） |
| 堆 / 栈大小 | `0x200` / `0x400` |

### 1.1 相关目录

```text
SlaveTestRemove/
├── SlaveTestRemove.ioc       # CubeMX 主配置文件
├── Core/
│   ├── Inc/                  # CubeMX 生成的头文件
│   └── Src/                  # CubeMX 生成的初始化与中断文件
├── Drivers/                  # STM32 HAL 与 CMSIS
├── Inc/                      # 项目自定义头文件、BSP 头文件
├── Src/                      # EtherCAT 应用与自定义 BSP 实现
├── Ethercat/                 # EtherCAT 从站协议栈相关代码
└── MDK-ARM/                  # Keil 工程文件
```

> 注意：当前仓库同时存在 `Core/` 与旧版 `Src/` 初始化代码。本文档以根目录 `SlaveTestRemove.ioc` 对应的 `Core/` 配置为准；`Src/` 中的旧 BSP 代码仍被 EtherCAT/LEN9252 相关代码使用，剥离时需要明确两套初始化代码的边界，避免重复定义或重复初始化。

---

## 2. CubeMX 启用的外设

根目录 `.ioc` 中启用的 IP 只有以下 6 项：

| IP | 用途 | 当前状态 |
|---|---|---|
| NVIC | 中断优先级与使能 | 已启用 |
| RCC | 时钟树和外部高速时钟 | 已启用 |
| SPI1 | 外部 ESC / SPI 设备访问 | 主机、双线、8 bit |
| SYS | SysTick、调试接口 | SysTick + Serial Wire |
| TIM2 | 通用定时基准 | 内部时钟，当前生成代码未启用 ISR 回调 |
| UART4 | 异步串口 | 115200-8-N-1 |

未在 `.ioc` 中启用 ETH、LwIP、FreeRTOS、DMA、USB、CAN、ADC、I2C 或 RTC。虽然 `stm32f4xx_hal_conf.h` 保留了部分通用 HAL 模块配置，不能据此认为对应外设已由 CubeMX 配置并可直接使用。

---

## 3. 时钟配置

### 3.1 时钟源和 PLL

| 参数 | 值 |
|---|---:|
| HSE | 8 MHz，外部晶振，`HSEState = RCC_HSE_ON` |
| HSI | 16 MHz（芯片内部） |
| PLL 输入源 | HSE |
| PLLM | 4 |
| PLLN | 168 |
| PLLP | 2（`RCC_PLLP_DIV2`） |
| PLLQ | 4 |
| SYSCLK 来源 | PLLCLK |
| CSS | 已启用 |
| Flash Latency | 5 WS |

计算关系：

```text
PLL 输入 VCO = 8 MHz / 4 = 2 MHz
PLL VCO      = 2 MHz × 168 = 336 MHz
SYSCLK       = 336 MHz / 2 = 168 MHz
```

### 3.2 总线时钟

| 时钟 | 配置 / 频率 |
|---|---:|
| SYSCLK | 168 MHz |
| HCLK / AHB | 168 MHz，AHB 分频 `DIV1` |
| PCLK1 / APB1 | 42 MHz，APB1 分频 `DIV4` |
| PCLK2 / APB2 | 84 MHz，APB2 分频 `DIV2` |
| APB1 定时器时钟 | 84 MHz（APB1 分频不为 1 时，定时器时钟加倍） |
| APB2 定时器时钟 | 168 MHz |
| Cortex 时钟 | 168 MHz |
| Ethernet 时钟字段 | 168 MHz；当前未启用 ETH 外设 |

### 3.3 代码位置

- CubeMX 配置值：`SlaveTestRemove.ioc` 的 `RCC.*` 项。
- 运行时配置：`Core/Src/main.c` 的 `SystemClock_Config()`。
- HAL 常量：`Inc/stm32f4xx_hal_conf.h` 中 `HSE_VALUE=8000000U`、`HSI_VALUE=16000000U`。

### 3.4 重要核对项

`Core/Src/system_stm32f4xx.c` 的默认 `HSE_VALUE` 兜底值为 25 MHz，但工程 HAL 配置文件将 `HSE_VALUE` 设置为 8 MHz。编译时应确保使用工程的 `Inc/stm32f4xx_hal_conf.h`，并确认没有其他头文件或编译宏覆盖该值；实际外部晶振也必须为 8 MHz，否则 `SystemCoreClock` 计算和基于时钟的定时均会偏差。

---

## 4. 引脚分配

### 4.1 汇总

| 引脚 | CubeMX 信号 / 模式 | 电气配置 | 当前用途或备注 |
|---|---|---|---|
| PH0 | `RCC_OSC_IN` | HSE 外部振荡器 | 外部高速晶振输入 |
| PH1 | `RCC_OSC_OUT` | HSE 外部振荡器 | 外部高速晶振输出 |
| PA5 | `SPI1_SCK` | SPI1 全双工主机 | SPI 时钟 |
| PA6 | `SPI1_MISO` | SPI1 全双工主机 | SPI 主入 |
| PA7 | `SPI1_MOSI` | SPI1 全双工主机 | SPI 主出 |
| PA8 | `GPIO_Output` | 推挽、上拉、低速 | 通用输出；与旧 BSP 的实际 CS 定义一致 |
| PA0-WKUP | `GPIO_Output` | 推挽、上拉、低速 | 通用输出 |
| PA3 | `GPIO_Output` | 推挽、上拉、低速 | 通用输出 |
| PF8 | `GPIO_Output` | 推挽、上拉、低速 | ESC RESET；旧 BSP 也使用 PF8 |
| PE2 | `GPIO_Input` | 上拉 | 通用输入 |
| PE3 | `GPIO_Input` | 上拉 | 通用输入 |
| PE4 | `GPIO_Input` | 上拉 | 通用输入 |
| PE5 | `GPIO_Input` | 上拉 | 通用输入 |
| PC0 | `GPIO_EXTI0` | 上拉，上升沿 | 外部中断线 0 |
| PC1 | `GPIO_EXTI1` | 上拉，上升沿 | 外部中断线 1 |
| PC3 | `GPIO_EXTI3` | 上拉，上升沿 | 外部中断线 3 |
| PB6 | `GPIO_Output` | 推挽、上拉、低速 | 通用输出；旧 BSP / 应用可作为 LED 或业务输出 |
| PB7 | `GPIO_Output` | 推挽、上拉、低速 | 通用输出；旧 BSP / 应用可作为 LED 或业务输出 |
| PB10 | `GPIO_Output` | 推挽、上拉、低速 | 通用输出 |
| PB11 | `GPIO_Output` | 推挽、上拉、低速 | 通用输出；旧 BSP 定义为 LED1 |
| PB12 | `GPIO_Output` | 推挽、上拉、低速 | 通用输出；旧 BSP 定义为 LED2 |
| PB13 | `GPIO_Output` | 推挽、上拉、低速 | 通用输出 |
| PB14 | `GPIO_Output` | 推挽、上拉、低速 | 通用输出 |
| PB15 | `GPIO_Output` | 推挽、上拉、低速 | 通用输出 |
| PA1 | `UART4_RX` | 复用推挽、上拉 | UART4 接收 |
| PC10 | `UART4_TX` | 复用推挽、上拉 | UART4 发送 |
| PC11 | `USART3_RX` | 复用推挽、无上下拉、极高速 | 仅由 GPIO 初始化配置；`.ioc` 未启用 USART3 IP |
| PA13 | `SYS_JTMS-SWDIO` | Serial Wire | SWD 数据 |
| PA14 | `SYS_JTCK-SWCLK` | Serial Wire | SWD 时钟 |

### 4.2 GPIO 默认输出电平

`Core/Src/gpio.c` 在配置输出模式前将下列引脚置为低电平：

- PF8
- PA0、PA3、PA8
- PB6、PB7、PB10～PB15

因此，启动阶段这些输出在 CubeMX 生成代码中的初始状态为 `GPIO_PIN_RESET`。对于复位、片选、使能等有效电平敏感信号，硬件原理图和外设时序必须与该默认状态一起核对。

### 4.3 外部中断配置差异

根目录 `.ioc` 和 `Core/Src/gpio.c` 将 PC0、PC1、PC3 配置为：

```text
GPIO_MODE_IT_RISING
GPIO_PULLUP
```

而旧版 `Src/bsp/gpio/bsp_gpio.c` 中的 `EXTI0_Configuration()`、`EXTI1_Configuration()`、`EXTI3_Configuration()` 使用的是下降沿触发：

```text
GPIO_MODE_IT_FALLING
GPIO_PULLUP
```

这不是简单的代码目录差异，而是会改变 IRQ 触发时机。剥离或统一工程时，应根据 LEN9252/ESC 的 IRQ、SYNC0、SYNC1 信号有效电平确认最终选择，并同步修改 `.ioc` 与实际初始化代码。

---

## 5. SPI1 配置

### 5.1 CubeMX 参数

| 参数 | 值 |
|---|---|
| 模式 | `SPI_MODE_MASTER` |
| 方向 | `SPI_DIRECTION_2LINES` |
| 数据帧 | 8 bit |
| 时钟极性 | `SPI_POLARITY_LOW`（CPOL=0） |
| 时钟相位 | `SPI_PHASE_1EDGE`（CPHA=0） |
| NSS | 软件管理 `SPI_NSS_SOFT` |
| 首位 | MSB first |
| 波特率分频 | `SPI_BAUDRATEPRESCALER_16` |
| CubeMX 计算速率 | 5.25 Mbit/s |
| TI 模式 | 禁用 |
| CRC | 禁用 |
| CRC Polynomial | 10（CRC 未启用时无实际作用） |

SPI1 引脚为 PA5/PA6/PA7，复用功能 `GPIO_AF5_SPI1`，速度为 `GPIO_SPEED_FREQ_VERY_HIGH`，无上下拉。

### 5.2 片选 CS

由于 NSS 为软件管理，CS 不由 SPI 外设自动产生。当前工程使用：

```text
CS 端口：GPIOA
CS 引脚：PA8
有效电平：低
释放电平：高
```

对应定义位于 `Inc/bsp/spiflash/bsp_spiflash.h`：

- `SELECT_SPI`：PA8 输出低电平
- `DESELECT_SPI`：PA8 输出高电平
- `CSLOW()` / `CSHIGH()`：对上述操作的封装

`Core/Src/spi.c` 的 `HAL_SPI_MspInit()` 会先将 PA8 置高，再配置为推挽输出。旧版 `Src/bsp/spiflash/bsp_spiflash.c` 中的 SPI GPIO MSP 初始化代码被注释，实际初始化依赖 `Core/Src/spi.c` 的 HAL 回调；剥离时必须保留其中一套，不要同时启用两套初始化。

### 5.3 与 LEN9252 / ESC 的关系

`Src/bsp/spiflash/bsp_spiflash.c` 通过 SPI1 访问 LEN9252/ESC 类寄存器，常见事务顺序是：

1. PA8 拉低；
2. 发送读写命令；
3. 发送 16 bit 地址（高字节在前）；
4. 读写数据；
5. PA8 拉高。

相关命令和 CSR 地址定义见 `Inc/bsp/spiflash/bsp_spiflash.h`，底层字节读写使用 `HAL_SPI_Transmit()` / `HAL_SPI_Receive()`。SPI 访问前需要确认 SPI 句柄只有一个有效实例：CubeMX 生成的 `hspi1` 与旧 BSP 中自定义的 `hspix` 当前并存，旧 BSP 的 `MX_SPIFlash_Init()` 使用 `hspix`。

---

## 6. TIM2 配置

### 6.1 CubeMX 生成配置

| 参数 | 值 |
|---|---:|
| 定时器 | TIM2 |
| 时钟源 | 内部时钟 |
| 计数方向 | 向上计数 |
| 预分频值 | `42 - 1` |
| 自动重装值 | `1000 - 1` |
| 时钟分频 | DIV1 |
| 自动重装预装载 | 禁用 |
| 主模式触发 | `TIM_TRGO_RESET` |
| 主从模式 | 禁用 |
| 更新中断优先级 | 1, 0（`.ioc` 中启用） |

在 APB1 定时器时钟为 84 MHz 的前提下：

```text
计数频率 = 84 MHz / 42 = 2 MHz
更新周期 = 1000 / 2 MHz = 500 us
更新频率 = 2 kHz
```

### 6.2 当前代码状态

- `Core/Src/tim.c` 生成了 `htim2` 和 `MX_TIM2_Init()`，但 `HAL_TIM_Base_MspInit()`、`TIM2_IRQHandler()` 等部分代码目前被注释。
- `Core/Src/stm32f4xx_it.c` 中的 `TIM2_IRQHandler()` 也被注释，因此仅调用 `MX_TIM2_Init()` 不会形成完整的定时器中断链路。
- 旧版 `Src/bsp/GeneralTIM/bsp_GeneralTIM.c` 使用自定义句柄 `htimx`，并通过 `TIM_Configuration(uint8_t period)` 配置 TIM2；其参数为预分频 41、周期 `period*200`，与 CubeMX 生成的 `42-1` / `1000-1` 表达的是同一个预分频值，但周期取决于传入 `period`。
- 旧版 BSP 是否启动中断还受 `ECAT_TIMER_INT` 控制；且旧代码的 NVIC 使能调用并非始终开启。

### 6.3 剥离建议

后续只保留一套定时器抽象：

1. 若使用 CubeMX 生成路径，恢复并维护 `HAL_TIM_Base_MspInit()`、`TIM2_IRQHandler()`，并由 `HAL_TIM_PeriodElapsedCallback()` 接入 EtherCAT 周期处理。
2. 若保留旧 BSP 路径，明确 `htimx` 的所有权，并将 CubeMX 的 `htim2` 初始化代码移除或不再编译。
3. 不要让 `htim2`、`htimx` 同时代表 TIM2 后被不同模块交叉调用，否则会造成中断、清标志和启动状态不一致。

---

## 7. UART4 配置

### 7.1 CubeMX 参数

| 参数 | 值 |
|---|---|
| 外设 | UART4 |
| 模式 | 异步，TX/RX |
| 波特率 | 115200 |
| 字长 | 8 bit |
| 停止位 | 1 |
| 校验 | None |
| 硬件流控 | None |
| 过采样 | 16 |
| RX | PA1，`GPIO_AF8_UART4` |
| TX | PC10，`GPIO_AF8_UART4` |

`Core/Src/usart.c` 生成的句柄为 `huart4`。但该文件中的 `HAL_UART_MspInit()` / `HAL_UART_MspDeInit()` 当前被注释，因此 `MX_UART4_Init()` 单独调用时不会自动完成 UART4 时钟和引脚初始化。

### 7.2 旧版调试串口路径

`Src/bsp/usart/bsp_debug_usart.h` 和 `Src/bsp/usart/bsp_debug_usart.c` 也将 UART4 作为调试串口：

- TX：PC10
- RX：PC11
- 复用：`GPIO_AF8_UART4`
- 波特率：115200
- 句柄：`husart_debug`

这与 CubeMX `.ioc` 的 RX 配置（PA1）不一致；同时，CubeMX 将 PC11 标记为 `USART3_RX`，但没有启用 USART3 IP。当前 `printf` 重定向在旧 BSP 中通过 `husart_debug` 完成，而不是通过 CubeMX 的 `huart4`。

因此，在剥离代码时必须先决定调试串口引脚方案：

| 方案 | RX | 需要同步修改 |
|---|---|---|
| 采用 `.ioc` | PA1 | 恢复 CubeMX UART4 MSP，统一使用 `huart4` |
| 采用旧 BSP | PC11 | 修改 `.ioc` 的 UART4_RX，保留旧 BSP 的 `husart_debug` 初始化 |

不要同时将 PA1 和 PC11 配置为 UART4 RX，也不要让两个句柄都对 UART4 做初始化。

---

## 8. NVIC 与中断

### 8.1 `.ioc` 中配置的中断

| IRQ | 优先级 | 状态 |
|---|---:|---|
| EXTI0_IRQn | 1, 0 | 启用 |
| EXTI1_IRQn | 1, 0 | 启用 |
| EXTI3_IRQn | 1, 0 | 启用 |
| TIM2_IRQn | 1, 0 | 启用（但生成的部分 MSP/IRQ 代码被注释） |
| SysTick_IRQn | 15, 0 | 系统滴答 |
| Fault/SVC/PendSV 等 | 默认 | 按 Cortex-M 异常表保留 |

优先级分组为 `NVIC_PRIORITYGROUP_4`，即 4 位抢占优先级、无子优先级（具体 HAL 表达仍以芯片和库版本为准）。

### 8.2 当前 EXTI ISR

`Core/Src/stm32f4xx_it.c` 提供：

```text
EXTI0_IRQHandler -> HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0)
EXTI1_IRQHandler -> HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1)
EXTI3_IRQHandler -> HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3)
```

外部中断最终需要通过 `HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)` 或项目自定义回调接入 EtherCAT/ESC 处理。当前文件中未见完整的业务回调实现时，应继续检查 `Src/`、`Ethercat/` 和 BSP 文件。

### 8.3 全局中断和业务中断

`Src/bsp_len9252.c` 中的 LEN9252 抽象层提供全局中断开关、TIM2 NVIC 开关和 EXTI NVIC 开关。其函数通过 `__enable_irq()`、`__disable_irq()`、`NVIC_EnableIRQ()`、`NVIC_DisableIRQ()` 操作硬件。后续剥离时，建议将这些操作集中到一个平台适配文件，避免协议栈直接依赖 CubeMX 生成文件名或句柄名。

---

## 9. SYS 与调试配置

| 项目 | 配置 |
|---|---|
| 调试接口 | Serial Wire |
| SWDIO | PA13 |
| SWCLK | PA14 |
| SysTick | `VP_SYS_VS_Systick` |
| SysTick 优先级（HAL 配置） | 15 |
| RTOS | 未启用，`USE_RTOS=0` |
| FPU / Cache | 按 STM32F4 HAL/CMSIS 默认配置启用相应支持 |
| 电源电压 | 3.3 V |

`Core/Src/main.c` 当前 `main()` 主体被整体注释，仅保留 `SystemClock_Config()` 和 `Error_Handler()`。因此 `.ioc` 中的初始化顺序不会自动在当前固件启动流程中执行，除非后续恢复或另行实现入口函数。

推荐的 CubeMX 初始化顺序为：

```c
HAL_Init();
SystemClock_Config();
MX_GPIO_Init();
MX_SPI1_Init();
MX_TIM2_Init();
MX_UART4_Init();
```

实际接入 LEN9252/EtherCAT 时，还需要在该顺序之后完成协议栈/BSP 实例绑定，并确保 SPI CS、ESC RESET、EXTI 和定时器初始化只执行一次。

---

## 10. HAL 模块与工程边界

`Inc/stm32f4xx_hal_conf.h` 当前显式启用：

- GPIO / EXTI
- DMA
- RCC / FLASH / PWR / CORTEX
- SPI
- TIM
- UART

但 `.ioc` 没有配置 DMA 通道，SPI、TIM、UART 的 CubeMX 生成代码中也没有形成完整 DMA 链路。ETH 模块在 HAL 配置模板中未启用，且工程没有启用 LwIP；EtherCAT 数据链路在当前设计中通过外部 ESC 的 SPI 接口实现，而不是 STM32 内置 Ethernet MAC。

---

## 11. 代码剥离时的配置基线

建议将下面的内容作为“平台初始化层”的最小保留集合：

### 必须保留

- `SystemClock_Config()` 及 8 MHz HSE / 168 MHz SYSCLK 配置；
- `MX_GPIO_Init()` 中实际使用的 PA8 CS、PF8 RESET、PA5/6/7 SPI 和 PC0/1/3 EXTI；
- SPI1 主机参数：CPOL=0、CPHA=0、8 bit、MSB、软件 CS、分频 16；
- 与最终硬件方案一致的 UART4 调试串口配置；
- 最终确定的一套 TIM2 句柄、MSP 初始化、IRQ 入口和 EtherCAT 周期回调；
- SWD 引脚 PA13/PA14，便于调试和下载。

### 可以按功能移除

- 与当前应用无关的 PB6/PB7/PB10～PB15 通用输出；
- 未使用的 PE2～PE5 输入；
- 未使用的 UART4（若剥离后不再保留 `printf` 调试）；
- TIM2（若协议栈不需要周期中断）；
- 旧版 `Src/` 中与新 `Core/` 重复的初始化封装。

### 剥离前必须确认的冲突

1. `Core/Src/spi.c` 使用 `hspi1`，旧 BSP 使用 `hspix`；
2. CubeMX CS 为 PA8，旧 BSP 宏也为 PA8，但 CubeMX 与旧 BSP 的初始化入口不同；
3. CubeMX EXTI 为上升沿，旧 BSP EXTI 配置为下降沿；
4. CubeMX UART4 RX 为 PA1，旧调试 BSP 使用 PC11；
5. CubeMX 定时器句柄为 `htim2`，旧 BSP 使用 `htimx`；
6. `Core/Src/main.c`、`Core/Src/tim.c`、`Core/Src/usart.c` 的部分生成入口被注释，而旧 BSP 在 `Src/` 中提供了另一套初始化实现。

---

## 12. 参考文件

- [SlaveTestRemove.ioc](../SlaveTestRemove.ioc)
- [Core/Src/main.c](../Core/Src/main.c)
- [Core/Src/gpio.c](../Core/Src/gpio.c)
- [Core/Src/spi.c](../Core/Src/spi.c)
- [Core/Src/tim.c](../Core/Src/tim.c)
- [Core/Src/usart.c](../Core/Src/usart.c)
- [Core/Src/stm32f4xx_it.c](../Core/Src/stm32f4xx_it.c)
- [Core/Src/system_stm32f4xx.c](../Core/Src/system_stm32f4xx.c)
- [Inc/stm32f4xx_hal_conf.h](../Inc/stm32f4xx_hal_conf.h)
- [Inc/bsp/spiflash/bsp_spiflash.h](../Inc/bsp/spiflash/bsp_spiflash.h)
- [Inc/bsp/usart/bsp_debug_usart.h](../Inc/bsp/usart/bsp_debug_usart.h)
- [Inc/bsp/GeneralTIM/bsp_GeneralTIM.h](../Inc/bsp/GeneralTIM/bsp_GeneralTIM.h)
- [Src/bsp/spiflash/bsp_spiflash.c](../Src/bsp/spiflash/bsp_spiflash.c)
- [Src/bsp/gpio/bsp_gpio.c](../Src/bsp/gpio/bsp_gpio.c)
- [Src/bsp/GeneralTIM/bsp_GeneralTIM.c](../Src/bsp/GeneralTIM/bsp_GeneralTIM.c)
- [Src/bsp/usart/bsp_debug_usart.c](../Src/bsp/usart/bsp_debug_usart.c)
