#ifndef __MATHS_H
#define __MATHS_H

#include <math.h>
#include <stdint.h>
#include "SysConfig.h"

//#define MATH_FUNCTION_VERSION_STR      "Maths Func V0.1.0"

#define CRC_INIT                       (0x66)

#define LIMIT_MAX(x, y)                (((x) > (y)) ? (y) : (x))
#define LIMIT_MIN(x, y)                (((x) < (y)) ? (y) : (x))
#define LIMIT_RANGE(x, max, min)       (LIMIT_MAX(LIMIT_MIN((x), (min)), (max)))

#define ABS(x)                         (((x) > 0) ? (x) : (-(x)))
#define MIN(x, y)                      (((x) < (y)) ? (x) : (y))
#define MAX(x, y)                      (((x) > (y)) ? (x) : (y))
#define LIMIT(x, max, min)             (MAX((min), MIN((x), (max))))

#define DEG_TO_RAD 0.017453292519943295769236907684886f
#define RAD_TO_DEG 57.295779513082320876798154814105f

//typedef union {
//  uint8_t cData[4];
//  float fData;
//} __packed FloatUnion;

//uint8_t* GetVersionString(void);

typedef struct
{
  float kp, ki, kd, preErr, Output, I_max, I_sum, dt, D_max;
} PID;

void pid_loop(PID* pid, float expect, float measure);

float apply_limit(float in, float min, float max);
float apply_deadband(float value, float deadband);
void step_change(float *in, float target, float step, float deadBand);
uint8_t ComputeCRC8(uint8_t *pchMessage, uint32_t dwLength, uint8_t ucCRC8);

void fusionQ_6dot(IMU_UNIT *unit, Quat_T *q, float prop_gain, float intg_gain, float dt);
void Quat2Euler(Quat_T* q, Euler_T* eur);

#endif /* __MATHS_H */
