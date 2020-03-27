/*
 * intio.h
 *
 *  Created on: Feb 27, 2019
 *      Author: kychu
 */

#ifndef DRIVER_PERIPH_INTIO_H_
#define DRIVER_PERIPH_INTIO_H_

#include "SysConfig.h"

#define INTx_PIN                            GPIO_Pin_13                  /* PC.13 */
#define INTx_GPIO_PORT                      GPIOC
#define INTx_GPIO_CLK                       RCC_AHBPeriph_GPIOC
#define INTx_GPIO_PortSource                EXTI_PortSourceGPIOC
#define INTx_GPIO_PinSource                 EXTI_PinSource13
#define INTx_EXTI_LINE                      EXTI_Line13
#define INTx_EXTI_IRQn                      EXTI4_15_IRQn
/* #define INTx_EXTI_IRQHandler                EXTI4_15_IRQHandler */

typedef void (*INTIO_IRQCallback)(void);

void intio_init(void);
void intio_set_irq_handler(INTIO_IRQCallback p);

void intio_irq_handler(void);

#endif /* DRIVER_PERIPH_INTIO_H_ */
