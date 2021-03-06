/*
 * config.h
 *
 *  Created on: Feb 25, 2020
 *      Author: kychu
 */

#ifndef SYSCONFIG_CONFIG_H_
#define SYSCONFIG_CONFIG_H_

#define SYS_ARCH_STR                        "ATT CTL M0"

#define SYS_XCLK_FREQUENCY                  (8000000)

#define SYS_BOOT_ORIGIN                     0x08000000
#define SYS_BOOT_LENGTH                     8K
#define SYS_TEXT_ORIGIN                     0x08002000
#define SYS_TEXT_LENGTH                     120K
#define SYS_SRAM_ORIGIN                     0x20000000
#define SYS_SRAM_LENGTH                     16K

#define SYS_MAIN_FLASH                      FLASH_TEXT
#define SYS_MAIN_MEMORY                     SRAM

#define SYS_HEAP_SIZE                       0
#define SYS_STACK_SIZE                      512

/* reserve 48 word starting in RAM @ 0x20000000 */
#define SYS_VECTOR_SIZE                     192

#define FREERTOS_ENABLED                    (1)

#define CONFIG_LOG_ENABLE                   (1)
#define CONFIG_USE_BOARD_IMU                (1)
#define CONFIG_USB_IF_ENABLE                (0)
#define CONFIG_RS485_2_ENABLE               (0)

#if FREERTOS_ENABLED
  #define SYSTICK_ENABLE                    (1)
#else
  #define SYSTICK_ENABLE                    (0)
#endif /* FREERTOS_ENABLED */

#if FREERTOS_ENABLED
#if CONFIG_USB_IF_ENABLE
  #define FREERTOS_HEAP_SIZE                (8 * 1024)
#else
  #define FREERTOS_HEAP_SIZE                (12 * 1024)
#endif /* CONFIG_USB_IF_ENABLE */

#define START_TASK_STACK_SIZE               (256)
#endif /* FREERTOS_ENABLED */

/* Interrupt Priority Table */
#define SYSTEM_TIMER_INT_PRIORITY           (0)

#if CONFIG_USB_IF_ENABLE
  #define USB_DEVICE_INT_PRIORITY           (3)
#endif /* CONFIG_USB_IF_ENABLE */
#if CONFIG_USE_BOARD_IMU
  #define EXTI4_15_INT_PRIORITY             (0)
  #define DMA_CHAN2_3_INT_PRIORITY          (0)
  #define DMA_CHAN4_5_6_7_INT_PRIORITY      (2)
  #define USART3_4_RX_INT_PRIORITY          (3)
#else
  #define DMA_CHAN2_3_INT_PRIORITY          (2)
  #define DMA_CHAN4_5_6_7_INT_PRIORITY      (0)
  #define USART3_4_RX_INT_PRIORITY          (0)
#endif /* CONFIG_USE_BOARD_IMU */
#define USART1_RX_INT_PRIORITY              (1)
#define USART2_RX_INT_PRIORITY              (2)
#define CONTROL_TIMER_INT_PRIORITY          (1)
#if SYSTICK_ENABLE
#define SYSTICK_INT_PRIORITY                (3)
#endif /* SYSTICK_ENABLE */

#if CONFIG_USB_IF_ENABLE
/*  Total size of IN buffer:  Total size of USB IN buffer: APP_RX_DATA_SIZE*8/MAX_BAUDARATE*1000 should be > CDC_IN_FRAME_INTERVAL */
#define APP_RX_DATA_SIZE                    256
#endif /* CONFIG_USB_IF_ENABLE */

#if CONFIG_LOG_ENABLE
#define CONFIG_LOG_CACHE_SIZE               (80)
#endif /* CONFIG_LOG_ENABLE */

#endif /* SYSCONFIG_CONFIG_H_ */
