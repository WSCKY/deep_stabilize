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
/* angle rate limitation */
#define PITCH_ADJ_ANGLE_RATE       (30)          /* 30deg/s */
#define YAW_ADJ_ANGLE_RATE         (20)          /* 20deg/s */
/* deadband for controller */
#define PITCH_ADJ_ANGLE_DEADBAND   (0.5f)        /* +/- 0.5deg */
#define YAW_ADJ_ANGLE_DEADBAND     (1.0f)        /* +/- 1.0deg */
/* adjustment step in degree */
#define PITCH_ADJ_STEP_DEG         (PITCH_ADJ_ANGLE_RATE * CTRL_LOOP_PERIOD_MS * 0.001f)
#define YAW_ADJ_STEP_DEG           (YAW_ADJ_ANGLE_RATE * CTRL_LOOP_PERIOD_MS * 0.001f)
/* encoder ID */
#define PITCH_MOTOR_ENCODER_ID     (0)           /* encoder 0 */
#define YAW_MOTOR_ENCODER_ID       (1)           /* encoder 1 */
/* motor output limitation */
#define PITCH_MOTOR_OUTPUT_LIMIT   (8000)        /* 8KHz */
#define YAW_MOTOR_OUTPUT_LIMIT     (8000)        /* 8KHz */

#define CTRL_STABILIZE_MODE_ENABLE (0)           /* enable(1) / disable(0) stabilize control mode */

#if CTRL_STABILIZE_MODE_ENABLE

#define CTRL_LOOP_PERIOD_MS        (2)           /* 2ms */

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

#define CTRL_LOOP_PERIOD_MS        (10)           /* 10ms */
//
//#define CTRL_LOST_ERROR_LEVEL      (15)           /* 15deg */

void ctrl_task(void const *arg)
{
  PID *pid;
  Params_t *params;
  int16_t m2_vel, m2_exp = 0;
//  float cur_pitch, exp_pitch = 0;
  float control_output = 0;
  uint32_t time_now, time_ts = 0;

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
  ctrlThread = xTaskGetCurrentTaskHandle();

  pid->kp = 0.022;
  pid->ki = 0.002;
  pid->kd = 0;
  pid->I_max = 10;
  pid->I_sum = 0;
  pid->dt = CTRL_LOOP_PERIOD_MS * 0.001f;
  pid->D_max = 100;

  pwm16_init();
  pwm17_init();
  delay(1000); // wait motor driver ready.

  /* initialize expect angle as current angle to avoid controller over-current */
  // update control parameter
//  param_get_param(params);
//  cur_pitch = ENCODER_INT2DEG(params->encoder[PITCH_MOTOR_ENCODER_ID]);
//  if(cur_pitch > 180) cur_pitch -= 360; // (-180, 180]
//  cur_pitch = -cur_pitch;
//  exp_pitch = cur_pitch;
//  cur_yaw = ENCODER_INT2DEG(params->encoder[YAW_MOTOR_ENCODER_ID]);
//  if(cur_yaw > 180) cur_yaw -= 360; // (-180, 180]
//  exp_yaw = cur_yaw;

  encoder3_init();
  /* initialize task sync timer */
  tim7_init(CTRL_LOOP_PERIOD_MS, ctrl_task_notify);

  for(;;) {
    if(xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, 100) == pdTRUE) {
      // update control parameter
//      param_get_param(params);

      m2_vel = encoder3_read();
      param_set_encval(m2_vel, 1);
      param_set_expval(m2_exp, 1);

      time_now = xTaskGetTickCountFromISR();
      if((time_now - time_ts) >= 3000) {
        time_ts = time_now;
        if(m2_exp < 2500)
          m2_exp = 3000;
        else
          m2_exp = 600;
      }

      // PID control
      pid_loop(pid, m2_exp, m2_vel);

      if(ABS(m2_exp) < 1) m2_exp = 0;
      if(m2_exp == 0) {
        control_output = 0;
        pid->I_sum = 0;
        pid->preErr = 0;
      } else {
        control_output += pid->Output;
        if(control_output < 500) control_output = 500;
      }
      control_output = m2_exp;
      param_set_ctrval(ABS(control_output), 1);

      pwm17_period(ABS(control_output));
      if(control_output < 0) output_port_set(IO_OUTPUT2);
      else output_port_clear(IO_OUTPUT2);
      // pitch axis control
//      if((params->flags & (1 << (PITCH_MOTOR_ENCODER_ID + ENCODER_ERROR_BIT_OFF))) == 0) {
//        if(ABS(exp_pitch - cur_pitch) > CTRL_LOST_ERROR_LEVEL) {
//          // lost control ...
//          pwm16_period(0);
//          output_port_clear(IO_OUTPUT1);
//          param_set_flag_bit(CTRL_LOST_PIT_BIT); // report we lost control.
//        } else {
//          // limit angle rate
//          step_change(&exp_pitch, params->exp_pitch, PITCH_ADJ_STEP_DEG, PITCH_ADJ_STEP_DEG);
//          cur_pitch = ENCODER_INT2DEG(params->encoder[PITCH_MOTOR_ENCODER_ID]);
//          if(cur_pitch > 180) cur_pitch -= 360; // (-180, 180]
//          cur_pitch = -cur_pitch;
//          // simple PID controller
//          if(ABS(params->exp_pitch - cur_pitch) > PITCH_ADJ_ANGLE_DEADBAND)
//            control_output = (exp_pitch - cur_pitch) * 300; // err * kp
//          else
//            control_output = 0; // we got the position
//          // limit PID output
//          control_output = LIMIT(control_output, PITCH_MOTOR_OUTPUT_LIMIT, -PITCH_MOTOR_OUTPUT_LIMIT);
//          //drive pitch motor
//          pwm16_period((uint32_t)ABS(control_output));
//          // set direction
//          if(control_output < 0) {
//            output_port_clear(IO_OUTPUT1);
//          } else {
//            output_port_set(IO_OUTPUT1);
//          }
//        }
//      } else {
//        // encoder error ...
//        pwm16_period(0);
//        output_port_clear(IO_OUTPUT1);
//      }
    }
  }
}

//void ctrl_task(void const *arg)
//{
//  int dir = 5;
//  int vel = 0;
//
//  pwm17_init();
//  delay(1000); // wait motor driver ready.
//  for(;;) {
//    delay(20);
//    vel += dir;
//    if(vel >= 3000) dir = -5;
//    if(vel <= -3000) dir = 5;
////    if(vel > 1000) vel = 1000;
//    pwm17_period(ABS(vel));
//    if(vel < 0)
//      output_port_set(IO_OUTPUT2);
//    else
//      output_port_clear(IO_OUTPUT2);
//  }
//}

#endif /* CTRL_STABILIZE_MODE_ENABLE */

static void ctrl_task_notify(void)
{
  osSignalSet(ctrlThread, 0x00000001);
}
