
/* 包含头文件 ----------------------------------------------------------------*/
#include "gpio/bsp_gpio.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

void RST_Configuration(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStruct;
	
  __HAL_RCC_GPIOF_CLK_ENABLE();	
    /**  GPIO Configuration    
    PD2     ------> RST
    */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;	
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct); 
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);  

}

void EXTI0_Configuration(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStruct;
	
  __HAL_RCC_GPIOB_CLK_ENABLE();	
    /**  GPIO Configuration    
    PB0     ------> IRQ
    */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);   
	
  HAL_NVIC_SetPriority(EXTI0_IRQn,1, 0);
//  HAL_NVIC_EnableIRQ(EXTI4_IRQn);	
}

void EXTI2_Configuration(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStruct;	
	
  __HAL_RCC_GPIOB_CLK_ENABLE();	
    /**  GPIO Configuration    
    PB2      ------> SYNC0
    */	
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);   
		
  HAL_NVIC_SetPriority(EXTI2_IRQn, 1, 1);
 // HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
}

void EXTI1_Configuration(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStruct;	
	
  __HAL_RCC_GPIOB_CLK_ENABLE();	
    /**  GPIO Configuration    
    PB1      ------> SYNC1
    */	
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);   
		
  HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 1);
 // HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
}


