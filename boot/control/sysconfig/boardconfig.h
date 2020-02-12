/*
 * boardconfig.h
 *
 *  Created on: Feb 10, 2020
 *      Author: kychu
 */

#ifndef BOARDCONFIG_H_
#define BOARDCONFIG_H_

#define SYS_ARCH_STR                        "ATT CTL M0"

#define SYS_XCLK_FREQUENCY                  (8000000)

#define SYS_HEAP_SIZE                       0x400
#define SYS_STACK_SIZE                      0x400

#define SYS_BOOT_ORIGIN                     0x08000000
#define SYS_BOOT_LENGTH                     8K
#define SYS_TEXT_ORIGIN                     0x08002000
#define SYS_TEXT_LENGTH                     120K
#define SYS_SRAM_ORIGIN                     0x20000000
#define SYS_SRAM_LENGTH                     6K

#define SYS_MAIN_FLASH                      FLASH_BOOT
#define SYS_MAIN_MEMORY                     SRAM

#define BOOT_CACHE_SIZE                     (2000)

#endif /* BOARDCONFIG_H_ */
