/*
 * rtu.h
 *
 *  Created on: Feb 20, 2020
 *      Author: kychu
 */

#ifndef __RTU_H_
#define __RTU_H_

#include "SysConfig.h"

#define RTU_CMD_READ_MULTI     0x03
#define RTU_CMD_READ_SINGLE    0x04
#define RTU_CMD_WRITE_MULTI    0x10
#define RTU_CMD_WRITE_SINGLE   0x06

typedef status_t (*rtuTxBytesFunc)(uint8_t *, uint32_t);

typedef struct {
  uint8_t *tx_cache;
  uint32_t cache_size;
  rtuTxBytesFunc tx_bytes;
} rtu_handle_t;

#endif /* __RTU_H_ */
