/*
 * parameter.h
 *
 *  Created on: Mar 7, 2020
 *      Author: kychu
 */

#ifndef APPS_PARAMETER_H_
#define APPS_PARAMETER_H_

#include "SysConfig.h"

#define ENCODER_NUMBER         2

typedef struct {
  float AngleRateVal;
  float AngleVal;
} AngleInfo_t;

#define IO_INPUT_1_BIT         0x00000001
#define IO_INPUT_2_BIT         0x00000002
#define IO_INPUT_3_BIT         0x00000004
#define IO_INPUT_4_BIT         0x00000008
#define IO_INPUT_5_BIT         0x00000010
#define IO_INPUT_6_BIT         0x00000020
#define IO_INPUT_7_BIT         0x00000040
#define IO_INPUT_8_BIT         0x00000080

#define IO_OUTPUT_1_BIT        0x00010000
#define IO_OUTPUT_2_BIT        0x00020000
#define IO_OUTPUT_3_BIT        0x00040000
#define IO_OUTPUT_4_BIT        0x00080000
#define IO_OUTPUT_5_BIT        0x00100000
#define IO_OUTPUT_6_BIT        0x00200000
#define IO_OUTPUT_7_BIT        0x00400000
#define IO_OUTPUT_8_BIT        0x00800000

status_t param_init(void);

void param_set_anginfo(AngleInfo_t *info);
void param_get_anginfo(AngleInfo_t *info);

#endif /* APPS_PARAMETER_H_ */
