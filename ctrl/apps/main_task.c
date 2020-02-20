/*
 * main_task.c
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#include "main_task.h"

USB_CORE_HANDLE USB_Device_dev;

/**
  * @brief  Start Thread
  * @param  argument not used
  * @retval None
  */
void StartThread(void const * arg)
{
  irq_initialize();
  board_gpio_init();
/*  pwm_init(); */
  uart1_init();
  uart2_init();
  uart3_init();
  uart4_init();
  delay(50);
  /* The Application layer has only to call USBD_Init to
     initialize the USB low level driver, the USB device library, the USB clock,
     pins and interrupt service routine (BSP) to start the Library*/
  USBD_Init(&USB_Device_dev, &USR_desc, &USBD_CDC_cb, &USR_cb);
  delay(50);

/*  mpu9250_init();
  _delay_ms(10); */
  for(;;) {
    delay(1000);
    uart1_TxBytes((uint8_t *)"1234567890\n", 11);
    uart4_TxBytes((uint8_t *)"1234567890\n", 11);
    uart2_TxBytesDMA((uint8_t *)"1234567890\n", 11);
    uart3_TxBytesDMA((uint8_t *)"1234567890\n", 11);
//    com_task();
//    uart2_TxBytesDMA((uint8_t *)"kyChu\n", 6);
//    if(USBD_isEnabled()) {
//      USB_CDC_SendBufferFast((uint8_t *)"kyChu\n", 6);
//    }
//    _delay_ms(200);
  }
}
