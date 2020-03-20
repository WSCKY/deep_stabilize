/*
 * main_task.c
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#include "main_task.h"

#include "parameter.h"

#if CONFIG_USB_IF_ENABLE
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_cdc_vcp.h"
#endif /* CONFIG_USB_IF_ENABLE */

#define STR1(R) #R
#define STR2(R) STR1(R)

static const char SystemInfo[] =
"\n  .--,       .--,"
"\n ( (  \\.---./  ) )"
"\n  '.__/o   o\\__.'"
"\n     {=  ^  =}"               "\t\t\t"   "Fire-fighting Robot Sprinkler Controller"
"\n      >  -  <"                "\t\t\t"   "Author:  kyChu<kychu@qq.com>"
"\n     /       \\"              "\t\t\t"   "Version: " STR2(__VERSION_STR__)
"\n    //       \\\\"            "\t\t\t"   "Date:    " __DATE__
"\n   //|   .   |\\\\"             "\t\t"   "Time:    " __TIME__
"\n   \"'\\       /'\"_.-~^`'-."     "\t"   "Board:   Deepblue DRV CTR Board(2020 V1)"
"\n      \\  _  /--'         `"      "\t"   "ALL RIGHTS RESERVED BY kyChu<kychu@qq.com>"
"\n    ___)( )(___"
"\n   (((__) (__)))"
"\n"
;

#if CONFIG_LOG_ENABLE
static const char* TAG = "MAIN";
#endif /* CONFIG_LOG_ENABLE */

static void error_handler(int code);

/**
  * @brief  Start Thread
  * @param  argument not used
  * @retval None
  */
void StartThread(void const * arg)
{
#if CONFIG_USB_IF_ENABLE
  USB_CORE_HANDLE *USB_Device_dev;
#endif /* CONFIG_USB_IF_ENABLE */

  irq_initialize();
  board_gpio_init();
/*  pwm_init(); */

  if(LOG_PORT_INIT() != status_ok) {
    error_handler(1);
  }

  LOG_PORT_OUTPUT("!!!KERNEL START!!!\n");
  LOG_PORT_OUTPUT(SystemInfo);

#if CONFIG_LOG_ENABLE
  if(log_init(LOG_PORT_OUTPUT) != status_ok) {
    error_handler(1);
  }
#endif /* CONFIG_LOG_ENABLE */

#if CONFIG_USB_IF_ENABLE
  /* The Application layer has only to call USBD_Init to
     initialize the USB low level driver, the USB device library, the USB clock,
     pins and interrupt service routine (BSP) to start the Library*/
  USB_Device_dev = kmm_alloc(sizeof(USB_CORE_HANDLE));
  if(USB_Device_dev == NULL) {
#if CONFIG_LOG_ENABLE
    ky_err(TAG, "usb init failed.");
#endif /* CONFIG_LOG_ENABLE */
    error_handler(5);
  }
  USBD_Init(USB_Device_dev, &USR_desc, &USBD_CDC_cb, &USR_cb);
  delay(50);
#endif /* CONFIG_USB_IF_ENABLE */

  if(param_init() != status_ok) {
    error_handler(1);
  }

#if SINS_TASK_MODULE_ENABLE
  osThreadDef(T_SINS, sins_task, osPriorityNormal, 0, 256); // 8% usage
  if(osThreadCreate (osThread(T_SINS), NULL) == NULL) {
#if CONFIG_LOG_ENABLE
    ky_err(TAG, "sins task create failed.");
#endif /* CONFIG_LOG_ENABLE */
    error_handler(2);
  }
#endif /* SINS_TASK_MODULE_ENABLE */

  osThreadDef(T_CTRL, ctrl_task, osPriorityNormal, 0, 256); // 50% usage
  if(osThreadCreate (osThread(T_CTRL), NULL) == NULL) {
#if CONFIG_LOG_ENABLE
    ky_err(TAG, "ctrl task create failed.");
#endif /* CONFIG_LOG_ENABLE */
    error_handler(3);
  }

  osThreadDef(T_STAT, stat_task, osPriorityNormal, 0, 256); // 2% usage
  if(osThreadCreate (osThread(T_STAT), NULL) == NULL) {
#if CONFIG_LOG_ENABLE
    ky_err(TAG, "stat task create failed.");
#endif /* CONFIG_LOG_ENABLE */
    error_handler(5);
  }

  osThreadDef(T_COM, com_task, osPriorityNormal, 0, 256); // 2% usage
  if(osThreadCreate (osThread(T_COM), NULL) == NULL) {
#if CONFIG_LOG_ENABLE
    ky_err(TAG, "mesg task create failed.");
#endif /* CONFIG_LOG_ENABLE */
    error_handler(4);
  }

#if CONFIG_LOG_ENABLE
  ky_info(TAG, "application started!");
#endif /* CONFIG_LOG_ENABLE */
//vTaskDelete(NULL);
/*  mpu9250_init();
  _delay_ms(10); */
  for(;;) {
    delay(200);
    USER_LED_TOG();
    ky_info(TAG, "usage %u%%", osGetCPUUsage());
  }
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName )
{
#if CONFIG_LOG_ENABLE
  ky_err("RTOS", "stack overflow: %s", pcTaskName);
#endif /* CONFIG_LOG_ENABLE */
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
