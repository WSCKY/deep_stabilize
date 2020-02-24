/*
 * main_task.c
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#include "main_task.h"

USB_CORE_HANDLE USB_Device_dev;

void led_task(void const *arg);
static void error_handler(int code);

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
  uart2_init();

  if(rs485_init() != status_ok) {
    error_handler(1);
  }
  delay(50);
  /* The Application layer has only to call USBD_Init to
     initialize the USB low level driver, the USB device library, the USB clock,
     pins and interrupt service routine (BSP) to start the Library*/
  USBD_Init(&USB_Device_dev, &USR_desc, &USBD_CDC_cb, &USR_cb);
  delay(50);

  osThreadDef(T_SINS, sins_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  if(osThreadCreate (osThread(T_SINS), NULL) == NULL) {
    error_handler(2);
  }

  osThreadDef(T_CTRL, ctrl_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  if(osThreadCreate (osThread(T_CTRL), NULL) == NULL) {
    error_handler(3);
  }

  osThreadDef(T_LED, led_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  if(osThreadCreate (osThread(T_LED), NULL) == NULL) {
    error_handler(5);
  }

/*  mpu9250_init();
  _delay_ms(10); */
  for(;;) {
    delay(1000);
//    uart1_TxBytes((uint8_t *)"uart1\n", 6);
//    uart4_TxBytes((uint8_t *)"uart4\n", 6);
//    uart2_TxBytesDMA((uint8_t *)"uart2\n", 6);
//    uart3_TxBytesDMA((uint8_t *)"uart3\n", 6);
//    com_task();
//    uart2_TxBytesDMA((uint8_t *)"kyChu\n", 6);
//    if(USBD_isEnabled()) {
//      USB_CDC_SendBufferFast((uint8_t *)"kyChu\n", 6);
//    }
//    _delay_ms(200);
  }
}

void led_task(void const *arg)
{
  for(;;) {
    delay(200);
    USER_LED_TOG();
  }
}

static void error_handler(int code)
{
  int i;
  USER_LED_OFF();
  delay(200);
  for(;;) {
    for(i = 0; i < code; i ++) {
      USER_LED_ON();
      delay(100);
      USER_LED_OFF();
      delay(400);
    }
    delay(500);
  }
}
