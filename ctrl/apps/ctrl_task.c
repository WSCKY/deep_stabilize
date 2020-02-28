/*
 * ctrl_task.c
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#include "main_task.h"

#include "servo.h"

static const char* TAG = "CTRL";

//float exp_angle = 0.0f;
//extern AngleInfo_t AngleInfo;

extern rtu_handle_t rtu_rs485_2;

static const servo_handle_t motor_pitch = {
  .addr = 0x01,
  .hrtu = &rtu_rs485_2,
};

void ctrl_task(void const *arg)
{
//  uint32_t task_period_div = 0;

  for(;;) {
  delay(1000);
//  servo_set_current(&motor_pitch, 2);
//  delay(1000);
#if CONFIG_LOG_ENABLE
    ky_info(TAG, "servo run 5000 pulse.");
#endif /* CONFIG_LOG_ENABLE */
  servo_run_pulse(&motor_pitch, 5000);
//  delay(1000);
  }

//  if(tim7_init() != status_ok) {
//    vTaskDelete(NULL);
//  }
//
//  for(;;) {
//    if(tim7_check_update(100) == status_ok) {
//      task_period_div ++;
//      if(task_period_div == 4) {
//        task_period_div = 0;
//
////      (AngleInfo.AngleVal - exp_angle) * 50 * 200 / 360
//      }
//    }
//  }
}
