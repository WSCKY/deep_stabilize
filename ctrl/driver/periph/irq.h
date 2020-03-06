/**
  ******************************************************************************
  * @file    irq.h 
  * @author  kyChu
  * @version V1.0.0
  * @date    17-April-2018
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IRQ_H
#define __IRQ_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "SysConfig.h"

#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "uart4.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void irq_initialize(void);

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_4_IRQHandler(void);
void DMA1_Channel2_3_IRQHandler(void);
void DMA1_Channel4_5_6_7_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __IRQ_H */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
