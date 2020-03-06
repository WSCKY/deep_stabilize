/*
 * uart1.h
 *
 *  Created on: Feb 20, 2020
 *      Author: kychu
 */

#ifndef __UART1_H_
#define __UART1_H_

#include "SysConfig.h"

#define UART1_DMA_ENABLE                    (1)

#define UART1                               USART1
#define UART1_GPIO                          GPIOB
#define UART1_AF                            GPIO_AF_0
#define UART1_GPIO_CLK                      RCC_AHBPeriph_GPIOB
#define UART1_CLK                           RCC_APB2Periph_USART1
#define UART1_CLK_CMD                       RCC_APB2PeriphClockCmd
#define UART1_RxPin                         GPIO_Pin_7
#define UART1_TxPin                         GPIO_Pin_6
#define UART1_RxPin_AF                      GPIO_PinSource7
#define UART1_TxPin_AF                      GPIO_PinSource6
/* #define UART1_IRQn                          USART1_IRQn */

#if UART1_DMA_ENABLE
#define UART1_TX_DMA                        DMA1_Channel2
#define UART1_RX_DMA                        DMA1_Channel3
#define UART1_TDR_ADDRESS                   0x40013828
#define UART1_RDR_ADDRESS                   0x40013824
#define UART1_DMA_CLK                       RCC_AHBPeriph_DMA1
#define UART1_DMA_CLK_CMD                   RCC_AHBPeriphClockCmd
#define UART1_TX_DMA_IT_TC_FLAG             DMA1_IT_TC2
#define UART1_RX_DMA_IT_HT_FLAG             DMA1_IT_HT3
#define UART1_RX_DMA_IT_TC_FLAG             DMA1_IT_TC3
/* #define UART1_DMA_IRQn                      DMA1_Channel2_3_IRQn */ /*!< DMA1 Channel 2, Channel 3 Interrupts */
#endif /* UART1_DMA_ENABLE */

#define UART1_RX_CACHE_SIZE                 80

status_t uart1_init(uint32_t baudrate);

void uart1_TxByte(uint8_t c);
void uart1_TxBytes(uint8_t *p, uint32_t l);
#if UART1_DMA_ENABLE
status_t uart1_TxBytesDMA(uint8_t *p, uint32_t l);
#if FREERTOS_ENABLED
status_t uart1_TxBytesDMA_Block(uint8_t *p, uint32_t l, uint32_t timeout);
#endif /* FREERTOS_ENABLED */
#endif /* UART1_DMA_ENABLE */
uint8_t uart1_pullByte(uint8_t *p);
uint32_t uart1_pullBytes(uint8_t *p, uint32_t l);

status_t uart1_waitBytes(uint32_t size, uint32_t timeout);
#if FREERTOS_ENABLED
status_t uart1_read_block(uint8_t *p, uint32_t l, uint32_t timeout);
#endif /* FREERTOS_ENABLED */

/* interrupt handler */
void uart1_irq_handler(void);
void uart1_dma_irq_handler(void);

#endif /* __UART1_H_ */
