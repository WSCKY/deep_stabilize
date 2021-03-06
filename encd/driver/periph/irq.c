/**
  ******************************************************************************
  * @file    irq.c
  * @author  kyChu
  * @date    17-April-2018
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "irq.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void irq_initialize(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = USART1_RX_INT_PRIORITY;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = USART2_RX_INT_PRIORITY;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the USART3 and USART4 Interrupts */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = USART3_4_RX_INT_PRIORITY;
  NVIC_Init(&NVIC_InitStructure);

#if CONFIG_USE_BOARD_IMU
  /* Enable the EXTI Line 4 to 15 Interrupts */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = EXTI4_15_INT_PRIORITY;
  NVIC_Init(&NVIC_InitStructure);
#endif /* CONFIG_USE_BOARD_IMU */

  /* Enable the DMA1 Channel 2, 3 Interrupts */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = DMA_CHAN2_3_INT_PRIORITY;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the DMA1 Channel 4, 5, 6, 7 Interrupts */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_5_6_7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = DMA_CHAN4_5_6_7_INT_PRIORITY;
  NVIC_Init(&NVIC_InitStructure);

  /* NVIC configuration *******************************************************/
  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = CONTROL_TIMER_INT_PRIORITY;
  NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
//void HardFault_Handler(void)
//{
//  /* Go to infinite loop when Hard Fault exception occurs */
//  while (1)
//  {
//  }
//}

#if !FREERTOS_ENABLED

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}
#endif /* FREERTOS_ENABLED */

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles USART1 interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
  uart1_irq_handler();
}

/**
  * @brief  This function handles USART2 interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
  uart2_irq_handler();
}

/**
  * @brief  This function handles USART3 & USART4 interrupt request.
  * @param  None
  * @retval None
  */
void USART3_4_IRQHandler(void)
{
#if !CONFIG_USE_BOARD_IMU
  uart3_irq_handler();
#endif /* !CONFIG_USE_BOARD_IMU */
  uart4_irq_handler();
}

#if CONFIG_USE_BOARD_IMU
/**
  * @brief  This function handles EXTI Line 4 to 15 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
  intio_irq_handler();
}
#endif /* CONFIG_USE_BOARD_IMU */

/**
  * @brief  This function handles DMA Channel 2, 3 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel2_3_IRQHandler(void)
{
#if CONFIG_USE_BOARD_IMU
  spi1_dma_irq_handler();
#else
  uart1_dma_irq_handler();
#endif /* CONFIG_USE_BOARD_IMU */
}

/**
  * @brief  This function handles DMA Channel 4, 5, 6, 7 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel4_5_6_7_IRQHandler(void)
{
#if CONFIG_USE_BOARD_IMU
  uart1_dma_irq_handler();
  uart2_dma_irq_handler();
#else
  uart3_dma_irq_handler();
  uart2_dma_irq_handler();
#endif /* CONFIG_USE_BOARD_IMU */
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
