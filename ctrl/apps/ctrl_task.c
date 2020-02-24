/*
 * ctrl_task.c
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#include "main_task.h"

//float exp_angle = 0.0f;
//extern AngleInfo_t AngleInfo;

void ctrl_task(void const *arg)
{
  uint32_t task_period_div = 0;
  if(tim7_init() != status_ok) {
    vTaskDelete(NULL);
  }
  for(;;) {
    if(tim7_check_update(100) == status_ok) {
      task_period_div ++;
      if(task_period_div == 4) {
        task_period_div = 0;

//      (AngleInfo.AngleVal - exp_angle) * 50 * 200 / 360
      }
    }
  }
}
