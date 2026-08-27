/******************************************************************************
 * Copyright (C) 2024 gengshuaige, Inc.(Gmbh) or its affiliates.
 *
 * All Rights Reserved.
 *
 * @file bsp_len9252.c
 *
 * @par dependencies
 * - bsp_len9252.h
 *
 * @author Yharim
 *
 * @brief HAL level LEN9252 driver implementation | HAL层LEN9252驱动实现
 * @details This source file implements all interfaces of LEN9252 module driver | 此源文件实现LEN9252模块驱动的所有接口
 * @note 1 tab == 4 spaces! This is the core source for LEN9252 driver instance management | 这是LEN9252驱动实例管理的核心源文件
 *
 * @version V1.0 2026-5-22
 *
 *****************************************************************************/

//******************************** Includes *********************************//
#include "bsp_len9252.h"
//******************************** Includes *********************************//

//******************************** Defines **********************************//

LEN9252_t len9252_handle = {0};       /* LEN9252 driver instance | LEN9252驱动实例 */

/**
 * @brief LEN9252 user system initialization function | LEN9252用户系统初始化函数
 * @details Default implementation of system initialization | 系统初始化的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
__weak len9252_tatus_t my_sys_init(void)
{
    printf("LEN9252 System Init\n");
        /* 复位所有外设，初始化Flash接口和系统滴答定时器 */
    HAL_Init();
  
  /* 配置系统时钟 */
    SystemClock_Config();
	
	GPIO_Config();
		/* initialize the SSP registers for the ESC SPI */
	MX_SPIFlash_Init();	
	
	MX_DEBUG_USART_Init();
	
	RST_Configuration();
	
    // 用户需在实例化时提供具体的系统初始化函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 get system time function | LEN9252获取系统时间函数
 * @details Default implementation of get system time | 获取系统时间的默认实现
 * @param[in] titk : Pointer to system tick count | 指向系统滴答计数的指针
 * @return uint32_t : Current system time | 当前系统时间
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
uint32_t my_get_time(uint32_t* const titk)
{
    printf("LEN9252 Get Time\n");

    // 用户需在实例化时提供具体的获取系统时间函数实现
    return 0;
}

/**
 * @brief LEN9252 delay function | LEN9252延时函数
 * @details Default implementation of delay | 延时的默认实现
 * @param[in] ms : Delay time in milliseconds | 延时时间（毫秒）
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_delay(uint32_t ms)
{
    HAL_Delay(ms);
    // 用户需在实例化时提供具体的延时函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 enable global interrupt function | LEN9252使能全局中断函数
 * @details Default implementation of enable global interrupt | 使能全局中断的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_enable_global_int(void)
{
    __enable_irq();
    return EC_OK;
}

/**
 * @brief LEN9252 disable global interrupt function | LEN9252关闭全局中断函数
 * @details Default implementation of disable global interrupt | 关闭全局中断的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_disable_global_int(void)
{
    __disable_irq();
    // 用户需在实例化时提供具体的禁止全局中断函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 ESC reset function | LEN9252 ESC复位函数
 * @details Default implementation of ESC reset | ESC复位的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_rst_esc(void)
{
    printf("LEN9252 Reset ESC\n");
    // 用户需在实例化时提供具体的ESC复位函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 RESET pin low function | LEN9252 RESET引脚拉低函数
 * @details Default implementation of RESET pin low | RESET引脚拉低的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_rst_reset(void)
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);
    // 用户需在实例化时提供具体的ESC RESET函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 RESET pin high function | LEN9252 RESET引脚拉高函数
 * @details Default implementation of RESET pin high | RESET引脚拉高的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_rst_set(void)
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_SET);
    // 用户需在实例化时提供具体的ESC SET函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 timer start NVIC function | LEN9252定时器启动中断函数
 * @details Default implementation of timer start NVIC | 定时器启动中断的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_timer_start_nvic(void)
{
    NVIC_EnableIRQ(TIM2_IRQn);
    // 用户需在实例化时提供具体的定时器启动函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 timer stop NVIC function | LEN9252定时器关闭中断函数
 * @details Default implementation of timer stop NVIC | 定时器关闭中断的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_timer_stop_nvic(void)
{
    NVIC_DisableIRQ(TIM2_IRQn);
    // 用户需在实例化时提供具体的定时器停止函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 timer interrupt init function | LEN9252定时器中断初始化函数
 * @details Default implementation of timer interrupt init | 定时器中断初始化的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_timer_it_init(void)
{
    TIM_Configuration(10);
    // 用户需在实例化时提供具体的定时器中断初始化函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 timer clear interrupt function | LEN9252定时器清除中断标志函数
 * @details Default implementation of timer clear interrupt | 定时器清除中断标志的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_timer_clear_it(void)
{
    __HAL_TIM_CLEAR_IT(&htimx, TIM_IT_UPDATE);
    // 用户需在实例化时提供具体的定时器中断清除函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 EXTI0 init function | LEN9252外部中断0初始化函数
 * @details Default implementation of EXTI0 init | 外部中断0初始化的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_exti0_init(void)
{
    EXTI0_Configuration();
    // 用户需在实例化时提供具体的外部中断初始化函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 EXTI1 init function | LEN9252外部中断1初始化函数
 * @details Default implementation of EXTI1 init | 外部中断1初始化的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_exti1_init(void)
{
    EXTI1_Configuration();
    // 用户需在实例化时提供具体的外部中断初始化函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 EXTI3 init function | LEN9252外部中断3初始化函数
 * @details Default implementation of EXTI3 init | 外部中断3初始化的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_exti3_init(void)
{
    EXTI3_Configuration();
    // 用户需在实例化时提供具体的外部中断初始化函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 EXTI1 interrupt open function | LEN9252外部中断1使能函数
 * @details Default implementation of EXTI1 interrupt open | 外部中断1使能的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_exti1_open(void)
{
    NVIC_EnableIRQ(EXTI1_IRQn);
    // 用户需在实例化时提供具体的外部中断初始化函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 EXTI1 interrupt close function | LEN9252外部中断1关闭函数
 * @details Default implementation of EXTI1 interrupt close | 外部中断1关闭的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_exti1_close(void)
{
    NVIC_DisableIRQ(EXTI1_IRQn);
    // 用户需在实例化时提供具体的外部中断初始化函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 EXTI3 interrupt open function | LEN9252外部中断3使能函数
 * @details Default implementation of EXTI3 interrupt open | 外部中断3使能的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_exti3_open(void)
{
    NVIC_EnableIRQ(EXTI3_IRQn);
    // 用户需在实例化时提供具体的外部中断初始化函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 EXTI3 interrupt close function | LEN9252外部中断3关闭函数
 * @details Default implementation of EXTI3 interrupt close | 外部中断3关闭的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_exti3_close(void)
{
    NVIC_DisableIRQ(EXTI3_IRQn);
    // 用户需在实例化时提供具体的外部中断初始化函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 EXTI0 clear flag function | LEN9252外部中断0清除标志函数
 * @details Default implementation of EXTI0 clear flag | 外部中断0清除标志的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_exti0_clear(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
    return EC_OK;
}

/**
 * @brief LEN9252 EXTI1 clear flag function | LEN9252外部中断1清除标志函数
 * @details Default implementation of EXTI1 clear flag | 外部中断1清除标志的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_exti1_clear(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
    // 用户需在实例化时提供具体的exti1清除函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 EXTI3 clear flag function | LEN9252外部中断3清除标志函数
 * @details Default implementation of EXTI3 clear flag | 外部中断3清除标志的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_exti3_clear(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
    // 用户需在实例化时提供具体的exti3清除函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 SPI write byte function | LEN9252 SPI写字节函数
 * @details Default implementation of SPI write byte | SPI写字节的默认实现
 * @param[in] data : SPI write data | SPI写入数据
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_spi_write(uint8_t data)
{
    
    SPIWrite(data);
    // 用户需在实例化时提供具体的SPI写函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 SPI read byte function | LEN9252 SPI读字节函数
 * @details Default implementation of SPI read byte | SPI读字节的默认实现
 * @param[in] data : Pointer to SPI read data | 指向SPI读取数据的指针
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
uint8_t my_spi_read(void)
{
    
    // 用户需在实例化时提供具体的SPI读函数实现
    return SPIRead();
}

/**
 * @brief LEN9252 GPIO set high function | LEN9252 GPIO拉高函数
 * @details Default implementation of GPIO set high | GPIO拉高的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_gpio_high(void)
{
    printf("LEN9252 GPIO High\n");
    // 用户需在实例化时提供具体的GPIO拉高函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 GPIO set low function | LEN9252 GPIO拉低函数
 * @details Default implementation of GPIO set low | GPIO拉低的默认实现
 * @param[in] none
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_gpio_low(void)
{
    printf("LEN9252 GPIO Low\n");
    // 用户需在实例化时提供具体的GPIO拉低函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 SPI write dword function | LEN9252 SPI写双字函数
 * @details Default implementation of SPI write dword | SPI写双字的默认实现
 * @param[in] Address : SPI register address | SPI寄存器地址
 * @param[in] Val     : SPI write data | SPI写入数据
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_spi_write_dword(uint16_t Address, uint32_t Val)
{
    printf("LEN9252 SPI Write DWord: Address=0x%04X, Value=0x%08X\n", Address, Val);
    // 用户需在实例化时提供具体的SPI写Word函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 SPI read dword function | LEN9252 SPI读双字函数
 * @details Default implementation of SPI read dword | SPI读双字的默认实现
 * @param[in]  Address : SPI register address | SPI寄存器地址
 * @param[out] data    : Pointer to SPI read data | 指向SPI读取数据的指针
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note User can override this function with custom implementation | 用户可通过自定义实现覆盖此函数
 */
len9252_tatus_t my_spi_read_dword(uint16_t Address, uint32_t* data)
{
    printf("LEN9252 SPI Read DWord: Address=0x%04X\n", Address);
    // 用户需在实例化时提供具体的SPI读Word函数实现
    return EC_OK;
}

/**
 * @brief LEN9252 North interface configuration | LEN9252北向接口配置
 * @details Default configuration of North interface | 北向接口的默认配置
 */
len9252_North_t len9252_north_api = {
    .test = 0,    /* Reserved test variable | 保留测试变量 */
};

/**
 * @brief LEN9252 time interface configuration | LEN9252时间接口配置
 * @details Default configuration of time interface | 时间接口的默认配置
 */
len9252_Time_t len9252_time_api = {
    .pf_get_time = my_get_time,    /* Get system time function | 获取系统时间函数 */
    .pf_delay    = my_delay,       /* Delay function | 延时函数 */
};

/**
 * @brief LEN9252 interrupt interface configuration | LEN9252中断接口配置
 * @details Default configuration of interrupt interface | 中断接口的默认配置
 */
len9252_isr_t len9252_isr_api = {
    .pf_enable_global_int  = my_enable_global_int,    /* Enable global interrupt function | 使能全局中断函数 */
    .pf_disable_global_int = my_disable_global_int,   /* Disable global interrupt function | 关闭全局中断函数 */
};

/**
 * @brief LEN9252 debug interface configuration | LEN9252调试接口配置
 * @details Default configuration of debug interface | 调试接口的默认配置
 */
len9252_debug_t len9252_debug_api = {
    .test = 0,    /* Reserved test variable | 保留测试变量 */
};

/**
 * @brief LEN9252 GPIO interface configuration | LEN9252 GPIO接口配置
 * @details Default configuration of GPIO interface | GPIO接口的默认配置
 */
len9252_gpio_t gpio_api = {
    .pf_rst_esc   = my_rst_esc,      /* ESC reset function | ESC复位函数 */
    .pf_rst_reset = my_rst_reset,    /* RESET pin low function | RESET引脚拉低函数 */
    .pf_rst_set   = my_rst_set,      /* RESET pin high function | RESET引脚拉高函数 */
};

/**
 * @brief LEN9252 timer interface configuration | LEN9252定时器接口配置
 * @details Default configuration of timer interface | 定时器接口的默认配置
 */
len9252_timer_t timer_api = {
    .pf_timer_start = my_timer_start_nvic,    /* Timer start function | 定时器启动函数 */
    .pf_timer_stop  = my_timer_stop_nvic,     /* Timer stop function | 定时器关闭函数 */
    .pf_it_isr      = EC_IT_ISR,              /* Timer interrupt service function | 定时器中断服务函数 */
    .pf_it_init     = my_timer_it_init,       /* Timer interrupt init function | 定时器中断初始化函数 */
    .pf_clear_it    = my_timer_clear_it,      /* Timer clear interrupt function | 定时器清除中断函数 */
};

/**
 * @brief LEN9252 EXTI interface configuration | LEN9252外部中断接口配置
 * @details Default configuration of EXTI interface | 外部中断接口的默认配置
 */
len9252_exti_t exti_api = {
    .pf_exti0_clear    = my_exti0_clear,      /* EXTI0 clear flag function | 外部中断0清除标志函数 */
    .pf_exti1_clear    = my_exti1_clear,      /* EXTI1 clear flag function | 外部中断1清除标志函数 */
    .pf_exti3_clear    = my_exti3_clear,      /* EXTI3 clear flag function | 外部中断3清除标志函数 */
    .pf_exti0_init     = my_exti0_init,       /* EXTI0 init function | 外部中断0初始化函数 */
    .pf_exti1_init     = my_exti1_init,       /* EXTI1 init function | 外部中断1初始化函数 */
    .pf_exti3_init     = my_exti3_init,       /* EXTI3 init function | 外部中断3初始化函数 */
    .pf_exti1_isr      = EC_EXTI1_ISR,        /* EXTI1 interrupt service function | 外部中断1服务函数 */
    .pf_exti1_it_open  = my_exti1_open,       /* EXTI1 interrupt open function | 外部中断1使能函数 */
    .pf_exti1_it_close = my_exti1_close,      /* EXTI1 interrupt close function | 外部中断1关闭函数 */
    .pf_exti3_it_open  = my_exti3_open,       /* EXTI3 interrupt open function | 外部中断3使能函数 */
    .pf_exti3_it_close = my_exti3_close,      /* EXTI3 interrupt close function | 外部中断3关闭函数 */
};

/**
 * @brief LEN9252 SPI interface configuration | LEN9252 SPI接口配置
 * @details Default configuration of SPI interface | SPI接口的默认配置
 */
len9252_spi_t spi_api = {
    .pf_spi_write       = my_spi_write,        /* SPI write byte function | SPI写字节函数 */
    .pf_spi_read        = my_spi_read,         /* SPI read byte function | SPI读字节函数 */
    .pf_gpio_high       = my_gpio_high,        /* GPIO set high function | GPIO拉高函数 */
    .pf_gpio_low        = my_gpio_low,         /* GPIO set low function | GPIO拉低函数 */
    .pf_spi_write_dword = my_spi_write_dword,  /* SPI write dword function | SPI写双字函数 */
    .pf_spi_read_dword  = my_spi_read_dword,   /* SPI read dword function | SPI读双字函数 */
};

/**
 * @brief LEN9252 South interface configuration | LEN9252南向接口配置
 * @details Default configuration of South interface | 南向接口的默认配置
 */
len9252_South_t len9252_south_api = {
    .spi_api   = &spi_api,      /* SPI interface pointer | SPI接口指针 */
    .gpio_api  = &gpio_api,     /* GPIO interface pointer | GPIO接口指针 */
    .timer_api = &timer_api,    /* Timer interface pointer | 定时器接口指针 */
    .exti_api  = &exti_api,     /* EXTI interface pointer | 外部中断接口指针 */
};

/**
 * @brief LEN9252 driver initialization function | LEN9252驱动初始化函数
 * @details Internal initialization function for LEN9252 driver instance | LEN9252驱动实例的内部初始化函数
 * @param[in] self : Pointer to LEN9252 driver instance | 指向LEN9252驱动实例的指针
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 * @note Called by len9252_inst() automatically | 由len9252_inst()自动调用
 */
len9252_tatus_t len9252_init(LEN9252_t* self)
{
    len9252_tatus_t res = EC_OK;

    /*************1. Checking the input parameters**************/
    if (NULL == self)
    {
#ifdef EC_DEBUG
        EC_DEBUG_OUT("Error: Invalid input parameter for LEN9252 initialization.\n");
#endif
        res = EC_ERRORPARAMETER;
        return res;
    }
    /*************1. Checking the input parameters**************/

    /*************2. Parameter assignment**************/
    self->north_api->test = 0;
    /*************2. Parameter assignment**************/

#ifdef EC_DEBUG
    EC_DEBUG_OUT("LEN9252 driver initialization completed.\n");
#endif

    return res;
}

/**
 * @brief LEN9252 driver instance initialization function | LEN9252驱动实例初始化函数
 * @details Dependency injection interface for LEN9252 driver instance | LEN9252驱动实例的依赖注入接口
 * @param[in] self         : Pointer to LEN9252 driver instance | 指向LEN9252驱动实例的指针
 * @param[in] pf_south_api : Pointer to South interface structure | 指向南向接口结构体的指针
 * @param[in] pf_north_api : Pointer to North interface structure | 指向北向接口结构体的指针
 * @param[in] pf_time_api  : Pointer to time interface structure | 指向时间接口结构体的指针
 * @param[in] pf_isr_api   : Pointer to interrupt interface structure | 指向中断接口结构体的指针
 * @param[in] pf_debug_api : Pointer to debug interface structure | 指向调试接口结构体的指针
 * @return len9252_tatus_t : Operation status (EC_OK for success) | 操作状态（EC_OK表示成功）
 */
len9252_tatus_t len9252_inst(LEN9252_t* self,
                             len9252_South_t* pf_south_api,
                             len9252_North_t* pf_north_api,
                             len9252_Time_t* pf_time_api,
                             len9252_isr_t* pf_isr_api,
                             len9252_debug_t* pf_debug_api)
{
    len9252_tatus_t res = EC_OK;

    /*************1. Checking the input parameters**************/
    if (NULL == self         ||
        NULL == pf_south_api ||
        NULL == pf_north_api ||
        NULL == pf_time_api  ||
        NULL == pf_isr_api   ||
        NULL == pf_debug_api)
    {
#ifdef EC_DEBUG
        EC_DEBUG_OUT("Error: Invalid input parameters for LEN9252 driver instantiation.\n");
#endif
        res = EC_ERRORPARAMETER;
        return res;
    }
    /*************1. Checking the input parameters**************/

    /*************2. Checking the initialization status**************/
    if (LEN9252_UNINIT != self->init_status)
    {
        res = EC_ERRORRESOURCE;
        return res;
    }
    /*************2. Checking the initialization status**************/

    /*************3. Binding the interfaces**************/
    self->south_api = pf_south_api;
    self->north_api = pf_north_api;
    self->time_api  = pf_time_api;
    self->isr_api   = pf_isr_api;
    self->debug_api = pf_debug_api;
    self->pf_sys_init = my_sys_init;
    /*************3. Binding the interfaces**************/

    /*************4. Initializing the driver**************/
    res = len9252_init(self);
    /*************4. Initializing the driver**************/

    /*************5. Checking the initialization result**************/
    if (EC_OK != res)
    {
        self->south_api = NULL;
        self->north_api = NULL;
        self->time_api  = NULL;
        self->isr_api   = NULL;
        self->debug_api = NULL;
        self->pf_sys_init = NULL;

#ifdef EC_DEBUG
        EC_DEBUG_OUT("Error: LEN9252 driver initialization failed.\n");
#endif
        return res;
    }
    /*************5. Checking the initialization result**************/

#ifdef EC_DEBUG
    EC_DEBUG_OUT("LEN9252 driver instantiation completed.\n");
#endif

    self->init_status = LEN9252_INITED;
    return res;
}

/**
 * @brief LEN9252 all unit test function | LEN9252全功能单元测试函数
 * @details Test all interfaces of LEN9252 driver | 测试LEN9252驱动的所有接口
 * @param[in] self : Pointer to LEN9252 driver instance | 指向LEN9252驱动实例的指针
 * @return none
 */
void len9252_all_unit_test(LEN9252_t* self)
{
    // Test code
    self->pf_sys_init();
    self->time_api->pf_get_time(NULL);
    self->time_api->pf_delay(1000);
    self->isr_api->pf_enable_global_int();
    self->isr_api->pf_disable_global_int();
    self->debug_api->test = 123;

    self->south_api->spi_api->pf_spi_write(0xAB);

    self->south_api->spi_api->pf_spi_read();
    self->south_api->spi_api->pf_gpio_high();
    self->south_api->spi_api->pf_gpio_low();
    self->south_api->spi_api->pf_spi_write_dword(0x1234, 0xDEADBEEF);
    uint32_t data32;
    self->south_api->spi_api->pf_spi_read_dword(0x1234, &data32);

    self->south_api->gpio_api->pf_rst_esc();
    self->south_api->gpio_api->pf_rst_reset();
    self->south_api->gpio_api->pf_rst_set();

    self->south_api->timer_api->pf_timer_start();
    self->south_api->timer_api->pf_timer_stop();
    self->south_api->timer_api->pf_it_init();
    self->south_api->timer_api->pf_clear_it();

    self->south_api->exti_api->pf_exti0_clear();
    self->south_api->exti_api->pf_exti1_clear();
    self->south_api->exti_api->pf_exti3_clear();
    self->south_api->exti_api->pf_exti0_init();
    self->south_api->exti_api->pf_exti1_init();
    self->south_api->exti_api->pf_exti3_init();
    self->south_api->exti_api->pf_exti1_it_open();
    self->south_api->exti_api->pf_exti1_it_close();
    self->south_api->exti_api->pf_exti3_it_open();
    self->south_api->exti_api->pf_exti3_it_close();
}

//******************************** Defines **********************************//