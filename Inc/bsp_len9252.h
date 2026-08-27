/******************************************************************************
 * Copyright (C) 2024 gengshuaige, Inc.(Gmbh) or its affiliates.
 *
 * All Rights Reserved.
 *
 * @file bsp_len9252.h
 *
 * @par dependencies
 * - stm32f4xx_hal.h
 * - stm32f4xx_it.h
 * - el9800hw.h
 *
 * @author Yharim
 *
 * @brief HAL level LEN9252 driver header | HAL层LEN9252驱动头文件
 * @details This header file defines all structures & interfaces for LEN9252 driver
 *          此头文件定义LEN9252驱动的所有结构体与接口
 * @note 1 tab == 4 spaces!
 *
 * @version V1.0 2026-5-22
 *
 *****************************************************************************/

#ifndef __BSP_LEN9252_H
#define __BSP_LEN9252_H

//******************************** Includes *********************************//
#include <stdio.h>
#include <stdint.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"


//******************************** Includes *********************************//
#include "el9800hw.h"


//******************************** Defines **********************************//
#define EC_DEBUG
#define EC_DEBUG_OUT(X,...)  printf(X, ##__VA_ARGS__)  /* Debug output interface | 调试输出接口 */

#define EC_EXTI1_ISR         EXTI0_IRQHandler          /* EXTI1 interrupt handler alias | EXTI1中断处理函数别名 */
#define EC_IT_ISR            TIM2_IRQHandler           /* Timer interrupt handler alias | 定时器中断处理函数别名 */
//******************************** Defines **********************************//

/**
 * @brief LEN9252 driver status enumeration | LEN9252驱动状态枚举
 */
typedef enum {
    EC_OK             = 0,      /* Operation completed successfully | 操作执行成功 */
    EC_ERROR          = 1,      /* Run-time error without case matched | 运行时错误，无匹配场景 */
    EC_ERRORTIMEOUT   = 2,      /* Operation failed with timeout | 操作超时失败 */
    EC_ERRORRESOURCE  = 3,      /* Resource not available | 资源不可用 */
    EC_ERRORPARAMETER = 4,      /* Parameter error | 参数错误 */
    EC_ERRORNOMEMORY  = 5,      /* Out of memory | 内存不足 */
    EC_ERRORISR       = 6,      /* Not allowed in ISR context | 不允许在中断上下文执行 */
    EC_RESERVED       = 7       /* Reserved for future extension | 预留值，用于未来扩展 */
} len9252_tatus_t;

/**
 * @brief LEN9252 initialization status enumeration | LEN9252初始化状态枚举
 */
typedef enum {
    LEN9252_UNINIT    = 0,         /* Not initialized | 未初始化 */
    LEN9252_INITED    = 1             /* Initialized | 已初始化 */
} len9252_init_t;

//******************************** North Interface ********************************//
/**
 * @brief LEN9252 North interface structure (EtherCAT upper layer)
 *        LEN9252北向接口结构体（EtherCAT上层）
 */
typedef struct {
    int test;                   /* Reserved test variable | 保留测试变量 */
} len9252_North_t;

//******************************** SPI Interface ********************************//
/**
 * @brief LEN9252 SPI interface structure | LEN9252 SPI接口结构体
 */
typedef struct {
    len9252_tatus_t                          (*pf_spi_write)(uint8_t data);                /* SPI write byte | SPI写字节 */
    uint8_t                                           (*pf_spi_read)(void);                /* SPI read byte | SPI读字节 */
    len9252_tatus_t                                  (*pf_gpio_high)(void);                        /* GPIO set high | GPIO拉高 */
    len9252_tatus_t                                   (*pf_gpio_low)(void);                         /* GPIO set low | GPIO拉低 */
    len9252_tatus_t  (*pf_spi_write_dword)(uint16_t Address, uint32_t Val); /* SPI write dword | SPI写双字 */
    len9252_tatus_t (*pf_spi_read_dword)(uint16_t Address, uint32_t* data);  /* SPI read dword | SPI读双字 */
} len9252_spi_t;

//******************************** GPIO Interface ********************************//
/**
 * @brief LEN9252 GPIO interface structure | LEN9252 GPIO接口结构体
 */
typedef struct {
    len9252_tatus_t            (*pf_rst_esc)(void);                          /* ESC reset GPIO init | ESC复位GPIO初始化 */
    len9252_tatus_t          (*pf_rst_reset)(void);                        /* RESET pin low | RESET引脚拉低 */
    len9252_tatus_t            (*pf_rst_set)(void);                          /* RESET pin high | RESET引脚拉高 */
} len9252_gpio_t;

//******************************** Timer Interface ********************************//
/**
 * @brief LEN9252 Timer interface structure | LEN9252定时器接口结构体
 */
typedef struct {
    len9252_tatus_t     (*pf_it_init)(void);                          /* Timer interrupt init | 定时器中断初始化 */
    len9252_tatus_t    (*pf_clear_it)(void);                         /* Clear timer interrupt flag | 清除定时器中断标志 */
    void                 (*pf_it_isr)(void);                                      /* Timer interrupt service | 定时器中断服务函数 */
    len9252_tatus_t (*pf_timer_start)(void);                      /* Start timer interrupt | 启动定时器中断 */
    len9252_tatus_t  (*pf_timer_stop)(void);                       /* Stop timer interrupt | 关闭定时器中断 */
} len9252_timer_t;

//******************************** EXTI Interface ********************************//
/**
 * @brief LEN9252 EXTI interface structure (3 external interrupts)
 *        LEN9252外部中断接口结构体（3个外部中断）
 */
typedef struct {
    len9252_tatus_t     (*pf_exti0_init)(void);                       /* EXTI0 initialization | 外部中断0初始化 */
    len9252_tatus_t     (*pf_exti1_init)(void);                       /* EXTI1 initialization | 外部中断1初始化 */
    len9252_tatus_t     (*pf_exti3_init)(void);                       /* EXTI3 initialization | 外部中断3初始化 */

    len9252_tatus_t    (*pf_exti0_clear)(void);                       /* Clear EXTI0 flag | 清除外部中断0标志 */
    void (*pf_exti1_isr)                (void);                                   /* EXTI1 interrupt service | 外部中断1服务函数 */

    len9252_tatus_t    (*pf_exti1_clear)(void);                      /* Clear EXTI1 flag | 清除外部中断1标志 */
    len9252_tatus_t  (*pf_exti1_it_open)(void);                    /* Enable EXTI1 interrupt | 使能外部中断1 */
    len9252_tatus_t (*pf_exti1_it_close)(void);                   /* Disable EXTI1 interrupt | 关闭外部中断1 */

    len9252_tatus_t    (*pf_exti3_clear)(void);                      /* Clear EXTI3 flag | 清除外部中断3标志 */
    len9252_tatus_t  (*pf_exti3_it_open)(void);                    /* Enable EXTI3 interrupt | 使能外部中断3 */
    len9252_tatus_t (*pf_exti3_it_close)(void);                   /* Disable EXTI3 interrupt | 关闭外部中断3 */
} len9252_exti_t;

//******************************** Time Interface ********************************//
/**
 * @brief LEN9252 Time interface structure | LEN9252时间接口结构体
 */
typedef struct {
    uint32_t (*pf_get_time)(uint32_t* const);                     /* Get system tick | 获取系统滴答 */
    len9252_tatus_t (*pf_delay)(uint32_t ms);                     /* Millisecond delay | 毫秒延时 */
} len9252_Time_t;

//******************************** South Interface ********************************//
/**
 * @brief LEN9252 South interface structure (MCU bottom layer)
 *        LEN9252南向接口结构体（MCU底层）
 */
typedef struct {
    len9252_spi_t*      spi_api;                                    /* SPI interface pointer | SPI接口指针 */
    len9252_gpio_t*    gpio_api;                                   /* GPIO interface pointer | GPIO接口指针 */
    len9252_timer_t*  timer_api;                                  /* Timer interface pointer | 定时器接口指针 */
    len9252_exti_t*    exti_api;                                   /* EXTI interface pointer | 外部中断接口指针 */
} len9252_South_t;

//******************************** ISR Interface ********************************//
/**
 * @brief LEN9252 global interrupt interface structure
 *        LEN9252全局中断接口结构体
 */
typedef struct {
    len9252_tatus_t  (*pf_enable_global_int)(void);                /* Enable global interrupt | 使能全局中断 */
    len9252_tatus_t (*pf_disable_global_int)(void);               /* Disable global interrupt | 关闭全局中断 */
} len9252_isr_t;

//******************************** Debug Interface ********************************//
/**
 * @brief LEN9252 debug interface structure | LEN9252调试接口结构体
 */
typedef struct {
    int test;                                                   /* Reserved test variable | 保留测试变量 */
} len9252_debug_t;

//******************************** Core Handle ********************************//
/**
 * @brief LEN9252 driver core handle | LEN9252驱动核心句柄
 */
typedef struct LEN9252 {
    len9252_init_t           init_status;                              /* Driver initialization status | 驱动初始化状态 */
    len9252_tatus_t (*pf_sys_init)(void);                        /* System initialization function | 系统初始化函数 */

    len9252_Time_t*     time_api;                                 /* Time interface pointer | 时间接口指针 */
    len9252_isr_t*       isr_api;                                  /* Interrupt interface pointer | 中断接口指针 */
    len9252_debug_t*   debug_api;                                /* Debug interface pointer | 调试接口指针 */
    len9252_South_t*   south_api;                                /* South interface pointer | 南向接口指针 */
    len9252_North_t*   north_api;                                /* North interface pointer | 北向接口指针 */
} LEN9252_t;

//******************************** Function Declarations ********************************//
/**
 * @brief LEN9252 driver instance initialization (dependency injection)
 *        LEN9252驱动实例初始化（依赖注入）
 * @param pf_len9252    : Driver handle pointer
 * @param pf_south_api  : South API pointer
 * @param pf_north_api  : North API pointer
 * @param pf_time_api   : Time API pointer
 * @param pf_isr_api    : ISR API pointer
 * @param pf_debug_api  : Debug API pointer
 * @return len9252_tatus_t
 */
len9252_tatus_t len9252_inst(LEN9252_t*         pf_len9252,
                             len9252_South_t* pf_south_api,
                             len9252_North_t* pf_north_api,
                             len9252_Time_t*   pf_time_api,
                             len9252_isr_t*     pf_isr_api,
                             len9252_debug_t* pf_debug_api
                            );

/**
 * @brief LEN9252 driver internal parameter initialization
 *        LEN9252驱动内部参数初始化
 * @param pf_len9252 : Driver handle pointer
 * @return len9252_tatus_t
 */
len9252_tatus_t len9252_init(LEN9252_t* pf_len9252);

/**
 * @brief LEN9252 all interface unit test
 *        LEN9252全接口单元测试
 * @param self : Driver handle pointer
 * @return none
 */
void len9252_all_unit_test(LEN9252_t* self);

//******************************** External Declarations ********************************//
extern TIM_HandleTypeDef  htimx;

extern LEN9252_t              len9252_handle;
extern len9252_South_t     len9252_south_api;
extern len9252_North_t     len9252_north_api;
extern len9252_Time_t       len9252_time_api;
extern len9252_isr_t         len9252_isr_api;
extern len9252_debug_t     len9252_debug_api;

extern void EXTI0_IRQHandler(void);
extern void  TIM2_IRQHandler(void);

#endif /* __BSP_LEN9252_H */