/*
 * icm20602.c
 *
 *  Created on: Feb 27, 2019
 *      Author: kychu
 */

#include "icm20602.h"
#include <string.h>

#if CONFIG_USE_BOARD_IMU

#define IMU_CACHE_SIZE                           15

static uint32_t icm20602_init_flag = 0;

static uint32_t icm20602_timestamp = 0;

#if FREERTOS_ENABLED
static osSemaphoreId imu_semaphore;
static uint8_t *imu_tx_buffer = NULL;
static uint8_t *imu_rx_buffer = NULL;
#else
static uint32_t imu_data_ready = 0;
static uint8_t imu_tx_buffer[IMU_CACHE_SIZE] = {0};
static uint8_t imu_rx_buffer[IMU_CACHE_SIZE] = {0};
#endif /* FREERTOS_ENABLED */

static void imuif_int_callback(void);
static status_t imu_read_reg(uint8_t reg, uint8_t num);
static status_t imu_write_reg(uint8_t reg, uint8_t val);

/*
 * configure the icm20602 registers.
 */
status_t icm20602_init(void)
{
  if(icm20602_init_flag == 1) return status_ok;

  /* initialize SPI bus. */
  imuif_init();
  /* initialize INT pin */
  imuif_int_init();
  imuif_int_setcallback(imuif_int_callback);
  /* Communication with all registers of the device is performed using SPI at 1MHz */
  imuif_config_rate(IMU_SPI_SLOW_RATE);

#if FREERTOS_ENABLED
  imu_tx_buffer = kmm_alloc(IMU_CACHE_SIZE); if(imu_tx_buffer == NULL) return status_nomem;
  imu_rx_buffer = kmm_alloc(IMU_CACHE_SIZE); if(imu_rx_buffer == NULL) return status_nomem;

  /* Define used semaphore */
  osSemaphoreDef(IMU_SEM);
  /* Create the semaphore */
  imu_semaphore = osSemaphoreCreate(osSemaphore(IMU_SEM) , 1);
  if(imu_semaphore == NULL) return status_error;
#else
  imu_data_ready = 0;
#endif /* FREERTOS_ENABLED */

  /* Reset the internal registers and restores the default settings. the bit will auto clear. */
  imu_write_reg(0x6B, 0x80);
  imu_delay(10);
  /* Reset accel & temp digital signal path */
  imu_write_reg(0x68, 0x03);
  /*
    1, Disable FIFO access from serial interface. (bit6 -> 0)
    2, Reset FIFO module. Reset is asynchronous. This bit auto clears after one clock cycle. (bit2 - > 1)
    3, Reset all gyro digital signal path, accel digital signal path, and temp digital signal path. This bit also clears all the sensor registers. (bit0 -> 1)
  */
  imu_write_reg(0x6A, 0x05);
  /* use the Internal 20MHz oscillator */
  imu_write_reg(0x6B, 0x00);
  imu_delay(10);

  /* Set sample rate to 500Hz */
  imu_write_reg(0x19, 0x01);
  /*
    1, The logic level for INT pin is active high. (bit7 -> 0)
    2, INT pin is configured as push-pull. (bit6 -> 0)
    3, INT pin indicates interrupt pulse's is width 50us. (bit5 -> 0)
    4, Interrupt status is cleared if any read operation is performed. (bit4 -> 1)
    5, The logic level for the FSYNC pin as an interrupt is active high. (bit3 -> 0)
    6, This disables the FSYNC pin from causing an interrupt. (bit2 -> 0)
  */
  imu_write_reg(0x37, 0x10);
  /* Enable Raw Sensor Data Ready interrupt to propagate to interrupt pin. */
  imu_write_reg(0x38, 0x01);
  /*
    1, when the fifo is full, additional writes will be written to the fifo, replacing the oldest data. (bit6 -> 0)
    2, disable FSYNC function.
    3, Gyroscope & Temperature DLPF. (Gyr: BW:41Hz, Delay: 5.9ms; Temp: BW:42Hz, Delay: 4.8ms)
  */
  imu_write_reg(0x1A, 0x03);
  /* Accel bypass DLPF configuration: BW:44.8Hz, Delay:4.88ms, Noise Density:300ug/rtHz */
  imu_write_reg(0x1D, 0x07);
  /* Accel Full Scale Select: +/-8g */
  imu_write_reg(0x1C, 0x10);
  /* Gyro Full Scale Select: +/-2000dps */
  imu_write_reg(0x1B, 0x18);
  imu_delay(1);
  /* Enable gyroscope & accelerometer(1 is disabled, 0 is on) */
  imu_write_reg(0x6C, 0x00);
  imu_delay(1);

  /* For applications requiring faster communications, the sensor and interrupt registers may be read using SPI at 10MHz. */
  imuif_config_rate(IMU_SPI_FAST_RATE);

  /* This register is used to verify the identity of the device. The contents of WHO_AM_I is an 8-bit device ID. The default value of the register is 0x71. */
  imu_read_reg(0x75, 1);
  if(imu_rx_buffer[1] != ICM20602_WHOAMI_ID) {
    return status_error;
  }

  imu_delay(1);

  memset(imu_tx_buffer, 0, IMU_CACHE_SIZE);
  memset(imu_rx_buffer, 0, IMU_CACHE_SIZE);

  icm20602_init_flag = 1;
  return status_ok;
}

status_t icm20602_read(IMU_RAW_6DOF *raw, IMU_UNIT_6DOF *unit, uint32_t timeout)
{
#if FREERTOS_ENABLED
  if(osSemaphoreWait(imu_semaphore , timeout) == osOK) {
#else
  uint32_t st = imu_ticks();
  do {
    if(timeout == 0) {
      break;
    } else if(timeout != HAL_MAX_DELAY) {
      if((imu_ticks() - st) > timeout)
        return status_timeout;
    }
  } while(imu_data_ready == 0);
  if(imu_data_ready != 0) {
    imu_data_ready = 0;
#endif /* FREERTOS_ENABLED */

    if(raw != NULL) {
      /* organize the received data */
      ((int8_t *)&raw->Acc.X)[0] = imu_rx_buffer[2];
      ((int8_t *)&raw->Acc.X)[1] = imu_rx_buffer[1];

      ((int8_t *)&raw->Acc.Y)[0] = imu_rx_buffer[4];
      ((int8_t *)&raw->Acc.Y)[1] = imu_rx_buffer[3];

      ((int8_t *)&raw->Acc.Z)[0] = imu_rx_buffer[6];
      ((int8_t *)&raw->Acc.Z)[1] = imu_rx_buffer[5];

      ((int8_t *)&raw->Temp)[0] = imu_rx_buffer[8];
      ((int8_t *)&raw->Temp)[1] = imu_rx_buffer[7];

      ((int8_t *)&raw->Gyr.X)[0] = imu_rx_buffer[10];
      ((int8_t *)&raw->Gyr.X)[1] = imu_rx_buffer[9];

      ((int8_t *)&raw->Gyr.Y)[0] = imu_rx_buffer[12];
      ((int8_t *)&raw->Gyr.Y)[1] = imu_rx_buffer[11];

      ((int8_t *)&raw->Gyr.Z)[0] = imu_rx_buffer[14];
      ((int8_t *)&raw->Gyr.Z)[1] = imu_rx_buffer[13];

      raw->TS = icm20602_timestamp;
    }

    imu_tx_buffer[0] = 0x3B | 0x80;
    imuif_txrx_bytes_dma(imu_tx_buffer, imu_rx_buffer, 14 + 1);

    if(unit != NULL && raw != NULL) {
      unit->Acc.X = raw->Acc.X * 0.002392578125f;
      unit->Acc.Y = raw->Acc.Y * 0.002392578125f;
      unit->Acc.Z = raw->Acc.Z * 0.002392578125f;

      unit->Gyr.X = raw->Gyr.X * 0.06103515625f;
      unit->Gyr.Y = raw->Gyr.Y * 0.06103515625f;
      unit->Gyr.Z = raw->Gyr.Z * 0.06103515625f;

      unit->Temp = raw->Temp / 338.87f + 21.0f;
      unit->TS = raw->TS;
    }

    return status_ok;
  }

  return status_timeout;
}

static void imuif_int_callback(void)
{
  if(icm20602_init_flag != 0) {
    icm20602_timestamp ++;
#if FREERTOS_ENABLED
    osSemaphoreRelease(imu_semaphore);
#else
    imu_data_ready = 1;
#endif /* FREERTOS_ENABLED */
  }
}

static status_t imu_read_reg(uint8_t reg, uint8_t num)
{
  if(num > IMU_CACHE_SIZE - 1) return status_error;
  imu_tx_buffer[0] = reg | 0x80;
  imuif_txrx_bytes(imu_tx_buffer, imu_rx_buffer, num +1);
  return status_ok;
}

static status_t imu_write_reg(uint8_t reg, uint8_t val)
{
  imu_tx_buffer[0] = reg;
  imu_tx_buffer[1] = val;
  imuif_txrx_bytes(imu_tx_buffer, imu_rx_buffer, 2);
  return status_ok;
}

#endif /* CONFIG_USE_BOARD_IMU */
