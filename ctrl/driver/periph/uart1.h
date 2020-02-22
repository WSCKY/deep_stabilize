/*
 * uart1.h
 *
 *  Created on: Feb 20, 2020
 *      Author: kychu
 */

#ifndef __UART1_H_
#define __UART1_H_

#include "SysConfig.h"

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

#define UART1_RX_CACHE_SIZE                 80

status_t uart1_init(void);

void uart1_TxByte(uint8_t c);
void uart1_TxBytes(uint8_t *p, uint32_t l);

uint8_t uart1_pullByte(uint8_t *p);
uint32_t uart1_pullBytes(uint8_t *p, uint32_t l);

status_t uart1_waitBytes(uint32_t size, uint32_t timeout);

void uart1_irq_handler(void);

#endif /* __UART1_H_ */
