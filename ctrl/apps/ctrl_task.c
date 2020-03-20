/*
 * ctrl_task.c
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#include "main_task.h"
#include "parameter.h"
#include "encoder.h"
#include "maths.h"
#include "servo.h"
#include <math.h>

#if CONFIG_LOG_ENABLE
static const char* TAG = "CTRL";
#endif /* CONFIG_LOG_ENABLE */

/*
 * keep ctrl_task task handle
 */
static osThreadId ctrlThread;

/*
 * task notify callback
 */
static void ctrl_task_notify(void);

/* pitch angle limitation */
#define PITCH_ANGLE_LIMIT_MIN      (-15.0f)      /* down: -15deg */
#define PITCH_ANGLE_LIMIT_MAX      (30.0f)       /* up: 30deg */
/* yaw angle limitation */
#define YAW_ANGLE_LIMIT_MIN        (-90.0f)      /* right: -90deg */
#define YAW_ANGLE_LIMIT_MAX        (90.0f)       /* left: 90deg */
/* encoder ID */
#define PITCH_MOTOR_ENCODER_ID     (0)           /* encoder 0 */
#define YAW_MOTOR_ENCODER_ID       (1)           /* encoder 1 */
/* motor output limitation */
#define PITCH_MOTOR_OUTPUT_LIMIT   (8000)        /* 8KHz */
#define YAW_MOTOR_OUTPUT_LIMIT     (8000)        /* 8KHz */

#define CTRL_STABILIZE_MODE_ENABLE (0)           /* enable(1) / disable(0) stabilize control mode */

#if CTRL_STABILIZE_MODE_ENABLE

#define CTRL_LOOP_PERIOD_MS        (2)           /* 2ms */

#define PITCH_ADJ_ANGLE_RATE       (10)          /* 10deg/s */
#define PITCH_ADJ_ANGLE_DEADBAND   (0.5f)        /* +/- 0.5deg */

#define YAW_ADJ_ANGLE_RATE         (60)          /* 60deg/s */

#define PITCH_ADJ_STEP_DEG         (PITCH_ADJ_ANGLE_RATE * CTRL_LOOP_PERIOD_MS * 0.001f)
#define YAW_ADJ_STEP_DEG           (YAW_ADJ_ANGLE_RATE * CTRL_LOOP_PERIOD_MS * 0.001f)

void ctrl_task(void const *arg)
{
  PID *pid;
  Params_t *params;
  float angle, exp_angle = 0;
  float yaw, exp_yaw = 0;

  ctrlThread = xTaskGetCurrentTaskHandle();
#if CONFIG_LOG_ENABLE
  ky_info(TAG, "ctrl task id: 0x%lx", (uint32_t)ctrlThread);
#endif /* CONFIG_LOG_ENABLE */

  pid = kmm_alloc(sizeof(PID));
  params = kmm_alloc(sizeof(Params_t));
  if(pid == NULL || params == NULL) {
    kmm_free(pid);
    kmm_free(params);
#if CONFIG_LOG_ENABLE
    ky_err(TAG, "no enough memory");
#endif /* CONFIG_LOG_ENABLE */
    vTaskDelete(NULL);
  }

  /* initialize task sync timer */
  tim7_init(CTRL_LOOP_PERIOD_MS, ctrl_task_notify);

  pwm16_init();
  pwm17_init();
  delay(1000); // wait motor driver ready.

  pid->kp = 100;
  pid->ki = 000;
  pid->kd = 0;
  pid->I_max = 3000;
  pid->I_sum = 0;
  pid->dt = CTRL_LOOP_PERIOD_MS * 0.001f;
  pid->D_max = 100;

  for(;;) {
    if(xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, 100) == pdTRUE) {
      // update control parameter
      param_get_param(params);

      // pitch axis control
      if((params->flags & CTRL_LOOP_ENABLE_BIT) != 0) { // stabilize enable
        // limit angle rate
        step_change(&exp_angle, params->exp_pitch, PITCH_ADJ_STEP_DEG, PITCH_ADJ_STEP_DEG);
        // limit expect angle
        exp_angle = LIMIT(exp_angle, PITCH_ANGLE_LIMIT_MAX, PITCH_ANGLE_LIMIT_MIN);
        // apply deadband
        angle = apply_deadband(params->angInfo.AngleVal, PITCH_ADJ_ANGLE_DEADBAND);
//        pid->kp = 500 + ABS(exp_angle - angle) * 200;
        // PID control
        pid_loop(pid, exp_angle, angle);
//        pid->Output += ((0 - angle) * 300 - AngleInfo.AngleRateVal) * 1.0f;
//        pid->Output = pid->Output + params->angInfo.AngleRateVal * 10.6667f;
      } else {
        // reset PID controller
        pid->I_sum = 0;
        pid->preErr = 0;
        // limit angle rate
        step_change(&exp_angle, params->exp_pitch, PITCH_ADJ_STEP_DEG, PITCH_ADJ_STEP_DEG);
        angle = ENCODER_INT2DEG(params->encoder[PITCH_MOTOR_ENCODER_ID]);
        if(angle > 180) angle -= 360; // from -180 to 180
//        (ENCODER_DEG2INT(params->exp_pitch) - params->encoder[PITCH_MOTOR_ENCODER_ID])
        pid->Output = (exp_angle - angle) * 1000; // err * kp
      }
      // limit PID output
      pid->Output = LIMIT(pid->Output, PITCH_MOTOR_OUTPUT_LIMIT, -PITCH_MOTOR_OUTPUT_LIMIT);
      // drive pitch motor
      pwm16_period((uint32_t)ABS(pid->Output));
      // set direction
      if(pid->Output < 0) {
        output_port_clear(IO_OUTPUT1);
      } else {
        output_port_set(IO_OUTPUT1);
      }

      // yaw axis control
      // limit angle rate
      step_change(&exp_yaw, params->exp_yaw, YAW_ADJ_STEP_DEG, YAW_ADJ_STEP_DEG);
      yaw = ENCODER_INT2DEG(params->encoder[YAW_MOTOR_ENCODER_ID]);
      if(yaw > 180) yaw -= 360;
      // simple PID controller
      pid->Output = (exp_yaw - yaw) * 100; // err * kp
      // limit PID output
      pid->Output = LIMIT(pid->Output, YAW_MOTOR_OUTPUT_LIMIT, -YAW_MOTOR_OUTPUT_LIMIT);
      //drive yaw motor
      pwm17_period((uint32_t)ABS(pid->Output));
      // set direction
      if(pid->Output < 0) {
        output_port_set(IO_OUTPUT2);
      } else {
        output_port_clear(IO_OUTPUT2);
      }
    } else {
#if CONFIG_LOG_ENABLE
      ky_err(TAG, "wait notify error");
#endif /* CONFIG_LOG_ENABLE */
    }
  }
}
#else

#define CTRL_LOOP_PERIOD_MS        (20)           /* 20ms */

void ctrl_task(void const *arg)
{
  Params_t *params;
  params = kmm_alloc(sizeof(Params_t));
  if(params == NULL) {
    kmm_free(params);
#if CONFIG_LOG_ENABLE
    ky_err(TAG, "no enough memory");
#endif /* CONFIG_LOG_ENABLE */
    vTaskDelete(NULL);
  }
  ctrlThread = xTaskGetCurrentTaskHandle();

  /* initialize task sync timer */
  tim7_init(CTRL_LOOP_PERIOD_MS, ctrl_task_notify);

  pwm16_init();
  pwm17_init();
  delay(1000); // wait motor driver ready.

  for(;;) {
    if(xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, 100) == pdTRUE) {
      // ...
    }
  }
}
#endif /* CTRL_STABILIZE_MODE_ENABLE */

static void ctrl_task_notify(void)
{
  osSignalSet(ctrlThread, 0x00000001);
}
