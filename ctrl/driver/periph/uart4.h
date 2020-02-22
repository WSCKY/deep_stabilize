/*
 * uart4.h
 *
 *  Created on: Feb 20, 2020
 *      Author: kychu
 */

#ifndef __UART4_H_
#define __UART4_H_

#include "SysConfig.h"

#define UART4                               USART4
#define UART4_GPIO                          GPIOA
#define UART4_AF                            GPIO_AF_4
#define UART4_GPIO_CLK                      RCC_AHBPeriph_GPIOA
#define UART4_CLK                           RCC_APB1Periph_USART4
#define UART4_CLK_CMD                       RCC_APB1PeriphClockCmd
#define UART4_RxPin                         GPIO_Pin_1
#define UART4_TxPin                         GPIO_Pin_0
#define UART4_RxPin_AF                      GPIO_PinSource1
#define UART4_TxPin_AF                      GPIO_PinSource0
/* #define UART4_IRQn                          USART3_4_IRQn */

#define UART4_RX_CACHE_SIZE                 88

status_t uart4_init(void);

void uart4_TxByte(uint8_t c);
void uart4_TxBytes(uint8_t *p, uint32_t l);

uint8_t uart4_pullByte(uint8_t *p);
uint32_t uart4_pullBytes(uint8_t *p, uint32_t l);

status_t uart4_waitBytes(uint32_t size, uint32_t timeout);

void uart4_irq_handler(void);

#endif /* __UART4_H_ */
