/*
 * uart2.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef __UART3_H_
#define __UART3_H_

#include "SysConfig.h"

#define UART3_DMA_ENABLE                    (1)

#define UART3                               USART3
#define UART3_GPIO                          GPIOB
#define UART3_AF                            GPIO_AF_4
#define UART3_GPIO_CLK                      RCC_AHBPeriph_GPIOB
#define UART3_CLK                           RCC_APB1Periph_USART3
#define UART3_CLK_CMD                       RCC_APB1PeriphClockCmd
#define UART3_RxPin                         GPIO_Pin_11
#define UART3_TxPin                         GPIO_Pin_10
#define UART3_RxPin_AF                      GPIO_PinSource11
#define UART3_TxPin_AF                      GPIO_PinSource10
/* #define UART3_IRQn                          USART3_4_IRQn */

#if UART3_DMA_ENABLE
#define UART3_TX_DMA                        DMA1_Channel7
#define UART3_RX_DMA                        DMA1_Channel6
#define UART3_DMA_CLK                       RCC_AHBPeriph_DMA1
#define UART3_DMA_CLK_CMD                   RCC_AHBPeriphClockCmd
#define UART3_TX_DMA_IT_TC_FLAG             DMA1_IT_TC7
#define UART3_RX_DMA_IT_HT_FLAG             DMA1_IT_HT6
#define UART3_RX_DMA_IT_TC_FLAG             DMA1_IT_TC6
/* #define UART3_DMA_IRQn                      DMA1_Channel4_5_6_7_IRQn */ /*!< DMA1 Channel 6, Channel 7 Interrupts */
#endif /* UART3_DMA_ENABLE */

#define UART3_RX_CACHE_SIZE                 88

status_t uart3_init(
#if UART3_DMA_ENABLE
		void
#else
		PortRecvByteCallback p
#endif
		);

void uart3_TxByte(uint8_t c);
void uart3_TxBytes(uint8_t *p, uint32_t l);
#if UART3_DMA_ENABLE
status_t uart3_TxBytesDMA(uint8_t *p, uint32_t l);
uint8_t uart3_pullByte(uint8_t *p);
uint32_t uart3_pullBytes(uint8_t *p, uint32_t l);
#else
#define uart3_TxBytesDMA uart3_TxBytes
void uart3_set_callback(PortRecvByteCallback p);
#endif /* UART3_DMA_ENABLE */

/* interrupt handler */
void uart3_irq_handler(void);
void uart3_dma_irq_handler(void);

#endif /* __UART3_H_ */
