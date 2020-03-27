/*
 * uart2.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef __UART2_H_
#define __UART2_H_

#include "SysConfig.h"

#define UART2_DMA_ENABLE                    (1)

#define UART2                               USART2
#define UART2_GPIO                          GPIOA
#define UART2_AF                            GPIO_AF_1
#define UART2_GPIO_CLK                      RCC_AHBPeriph_GPIOA
#define UART2_CLK                           RCC_APB1Periph_USART2
#define UART2_CLK_CMD                       RCC_APB1PeriphClockCmd
#define UART2_RxPin                         GPIO_Pin_3
#define UART2_TxPin                         GPIO_Pin_2
#define UART2_RxPin_AF                      GPIO_PinSource3
#define UART2_TxPin_AF                      GPIO_PinSource2
/* #define UART2_IRQn                          USART2_IRQn */

#if UART2_DMA_ENABLE
#define UART2_TDR_ADDRESS                   0x40004428
#define UART2_RDR_ADDRESS                   0x40004424
#if CONFIG_USE_BOARD_IMU
#define UART2_TX_DMA                        DMA1_Channel7
#define UART2_RX_DMA                        DMA1_Channel6
#define UART2_DMA_CLK                       RCC_AHBPeriph_DMA1
#define UART2_DMA_CLK_CMD                   RCC_AHBPeriphClockCmd
#define UART2_TX_DMA_IT_TC_FLAG             DMA1_IT_TC7
#define UART2_RX_DMA_IT_HT_FLAG             DMA1_IT_HT6
#define UART2_RX_DMA_IT_TC_FLAG             DMA1_IT_TC6
/* #define UART2_DMA_IRQn                      DMA1_Channel4_5_6_7_IRQn */ /*!< DMA1 Channel 6, Channel 7 Interrupts */
#else
#define UART2_TX_DMA                        DMA1_Channel4
#define UART2_RX_DMA                        DMA1_Channel5
#define UART2_DMA_CLK                       RCC_AHBPeriph_DMA1
#define UART2_DMA_CLK_CMD                   RCC_AHBPeriphClockCmd
#define UART2_TX_DMA_IT_TC_FLAG             DMA1_IT_TC4
#define UART2_RX_DMA_IT_HT_FLAG             DMA1_IT_HT5
#define UART2_RX_DMA_IT_TC_FLAG             DMA1_IT_TC5
/* #define UART2_DMA_IRQn                      DMA1_Channel4_5_6_7_IRQn */ /*!< DMA1 Channel 4, Channel 5 Interrupts */
#endif /* CONFIG_USE_BOARD_IMU */
#endif /* UART2_DMA_ENABLE */

#define UART2_RX_CACHE_SIZE                 88

status_t uart2_init(
#if UART2_DMA_ENABLE
		void
#else
		PortRecvByteCallback p
#endif
		);

void uart2_TxByte(uint8_t c);
void uart2_TxBytes(uint8_t *p, uint32_t l);
status_t uart2_TxString(const char *p);
#if UART2_DMA_ENABLE
status_t uart2_TxBytesDMA(uint8_t *p, uint32_t l);
uint8_t uart2_pullByte(uint8_t *p);
uint32_t uart2_pullBytes(uint8_t *p, uint32_t l);
#else
#define uart2_TxBytesDMA uart2_TxBytes
void uart2_set_callback(PortRecvByteCallback p);
#endif /* UART2_DMA_ENABLE */

/* interrupt handler */
void uart2_irq_handler(void);
void uart2_dma_irq_handler(void);

#endif /* __UART2_H_ */
