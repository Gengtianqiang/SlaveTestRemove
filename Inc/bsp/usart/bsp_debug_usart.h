
#ifndef __BSP_DEBUG_USART_H__
#define __BSP_DEBUG_USART_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdio.h>

/* 类型定义 ------------------------------------------------------------------*/

/* 宏定义 --------------------------------------------------------------------*/
#define DEBUG_USARTx                                 UART4
#define DEBUG_USARTx_BAUDRATE                        115200
#define DEBUG_USART_RCC_CLK_ENABLE()                 __HAL_RCC_UART4_CLK_ENABLE()
#define DEBUG_USART_RCC_CLK_DISABLE()                __HAL_RCC_UART4_CLK_DISABLE()

#define DEBUG_USARTx_GPIO_ClK_ENABLE()               __HAL_RCC_GPIOC_CLK_ENABLE()
#define DEBUG_USARTx_Tx_GPIO_PIN                     GPIO_PIN_10
#define DEBUG_USARTx_Tx_GPIO                         GPIOC
#define DEBUG_USARTx_Rx_GPIO_PIN                     GPIO_PIN_11
#define DEBUG_USARTx_Rx_GPIO                         GPIOC

#define DEBUG_USARTx_AFx                             GPIO_AF8_UART4

#define DEBUG_USART_IRQn                             UART4_IRQn

/* 扩展变量 ------------------------------------------------------------------*/
extern UART_HandleTypeDef husart_debug;

/* 函数声明 ------------------------------------------------------------------*/
void MX_DEBUG_USART_Init(void);


#endif  /* __BSP_DEBUG_USART_H__ */


