/*
 * boardconfig.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef BOARDCONFIG_H_
#define BOARDCONFIG_H_

#define SYS_ARCH_STR                        "ATT CTL M0"

#define SYS_XCLK_FREQUENCY                  (8000000)

#define SYS_HEAP_SIZE                       0
#define SYS_STACK_SIZE                      256

#define SYS_BOOT_ORIGIN                     0x08000000
#define SYS_BOOT_LENGTH                     8K
#define SYS_TEXT_ORIGIN                     0x08002000
#define SYS_TEXT_LENGTH                     120K
#define SYS_SRAM_ORIGIN                     0x20000000
#define SYS_SRAM_LENGTH                     6K

/* reserve 48 word starting in RAM @ 0x20000000 */
#define SYS_VECTOR_SIZE                     192

/*  Total size of IN buffer:  Total size of USB IN buffer: APP_RX_DATA_SIZE*8/MAX_BAUDARATE*1000 should be > CDC_IN_FRAME_INTERVAL */
#define APP_RX_DATA_SIZE                    256

#define UART1_RX_CACHE_SIZE                 80
#define UART2_RX_CACHE_SIZE                 88
#define UART3_RX_CACHE_SIZE                 88
#define UART4_RX_CACHE_SIZE                 88

#define SYS_MAIN_FLASH                      FLASH_TEXT
#define SYS_MAIN_MEMORY                     SRAM

#endif /* BOARDCONFIG_H_ */
