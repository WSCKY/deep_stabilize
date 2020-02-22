/*
 * sins_task.c
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#include "main_task.h"

void sins_task(void const *arg)
{
  uart3_init();
  for(;;) {
    delay(5);
//    uart3_pullBytes();
  }
}
