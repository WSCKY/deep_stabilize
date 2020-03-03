/*
 * ctrl_task.c
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#include "main_task.h"

#include "servo.h"
#include <math.h>

#if CONFIG_LOG_ENABLE
//static const char* TAG = "CTRL";
#endif /* CONFIG_LOG_ENABLE */

//float exp_angle = 0.0f;
extern AngleInfo_t AngleInfo;

/* extern rtu_handle_t rtu_rs485_1;

static const servo_handle_t motor_pitch = {
  .addr = 0x01,
  .hrtu = &rtu_rs485_1,
}; */

void ctrl_task(void const *arg)
{
  uint32_t task_period_div = 0;
  pwm16_init();
  pwm17_init();
  delay(1000); // wait motor driver ready.

  if(tim7_init() != status_ok) {
    vTaskDelete(NULL);
  }

  for(;;) {
    if(tim7_check_update(100) == status_ok) {
      task_period_div ++;
      if(task_period_div == 4) { // 20ms
        task_period_div = 0;

        pwm16_period((int)(1200 * fabs((AngleInfo.AngleVal - 0))));
        if(AngleInfo.AngleVal < 0) {
        	output_port_clear(IO_OUTPUT2);
        } else {
        	output_port_set(IO_OUTPUT2);
        }
//        ky_info(TAG, "loop");
//      (AngleInfo.AngleVal - exp_angle) * 50 * 200 / 360
      }
    }
  }
}
