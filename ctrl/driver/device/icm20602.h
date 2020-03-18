/*
 * icm20602.h
 *
 *  Created on: Feb 27, 2019
 *      Author: kychu
 */

#ifndef __ICM20602_H
#define __ICM20602_H

#include "ifconfig.h"

#if CONFIG_USE_BOARD_IMU

#define imu_delay                      delay

#if !FREERTOS_ENABLED
#define MPU_DATA_UPDATE_HOOK_ENABLE    (1)
#endif /* !FREERTOS_ENABLED */

#define imuif_init                     spi1_init
#define imuif_int_init                 intio_init
#define imuif_int_setcallback          intio_set_irq_handler
#define imuif_config_rate              spi1_configrate
#define imuif_txrx_bytes               spi1_rx_tx
#define imuif_txrx_bytes_dma           spi1_rx_tx_dma

#define IMU_SPI_SLOW_RATE              SPI_BaudRatePrescaler_128 /* 48 / 64 = 0.75MHz */
#define IMU_SPI_FAST_RATE              SPI_BaudRatePrescaler_8   /* 48 / 8 = 6MHz */

//#define MPU_DATA_DEPTH            (1 << 1)
//#define MPU_BUFF_MASK             (MPU_DATA_DEPTH - 1)

__PACK_BEGIN typedef struct {
  int16_t X;
  int16_t Y;
  int16_t Z;
} __PACK_END _3AxisRaw;

__PACK_BEGIN typedef struct {
  float X;
  float Y;
  float Z;
} __PACK_END _3AxisUnit;

__PACK_BEGIN typedef struct {
  _3AxisRaw Acc;
  _3AxisRaw Gyr;
  int16_t Temp;
  uint32_t TS;
} __PACK_END IMU_RAW_6DOF;

__PACK_BEGIN typedef struct {
  _3AxisUnit Acc;
  _3AxisUnit Gyr;
  float Temp;
  uint32_t TS;
} __PACK_END IMU_UNIT_6DOF;

#define ICM20602_WHOAMI_ID        (0x12)

status_t icm20602_init(void);
status_t icm20602_read(IMU_RAW_6DOF *raw, IMU_UNIT_6DOF *unit, uint32_t timeout);

#endif /* CONFIG_USE_BOARD_IMU */

#endif /* __ICM20602_H */
