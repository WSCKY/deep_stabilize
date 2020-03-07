/*
 * parameter.c
 *
 *  Created on: Mar 7, 2020
 *      Author: kychu
 */

#include <string.h>

#include "parameter.h"

#if FREERTOS_ENABLED
#include "cmsis_os.h"
#endif /* FREERTOS_ENABLED */

typedef struct {
	AngleInfo_t angInfo;
	uint16_t encoder[ENCODER_NUMBER];
	float exp_pitch;
	float exp_yaw;
	uint32_t flags;
} Params_t;

static Params_t* params;
static osMutexId paramMutex;

osMutexDef(PARAMMutex);

static void param_req_grant(void);
static void param_rel_grant(void);

status_t param_init(void)
{
  /* create the MUTEX object */
  paramMutex = osMutexCreate(osMutex(PARAMMutex));
  if(paramMutex == NULL) return status_error;
  params = kmm_alloc(sizeof(Params_t));
  if(params == NULL) return status_nomem;
  memset(params, 0, sizeof(Params_t));

  return status_ok;
}

void param_set_anginfo(AngleInfo_t *info)
{
  param_req_grant();
  params->angInfo = *info;
  param_rel_grant();
}

void param_get_anginfo(AngleInfo_t *info)
{
  param_req_grant();
  *info = params->angInfo;
  param_rel_grant();
}

void param_set_exppit(float ang)
{
  param_req_grant();
  params->exp_pitch = ang;
  param_rel_grant();
}

void param_get_exppit(float *ang)
{
  param_req_grant();
  *ang = params->exp_pitch;
  param_rel_grant();
}

void param_set_expyaw(float ang)
{
  param_req_grant();
  params->exp_yaw = ang;
  param_rel_grant();
}

void param_get_expyaw(float *ang)
{
  param_req_grant();
  *ang = params->exp_yaw;
  param_rel_grant();
}

void param_set_encval(uint16_t val, uint8_t id)
{
  param_req_grant();
  if(id < ENCODER_NUMBER)
    params->encoder[id] = val;
  param_rel_grant();
}

void param_get_encval(uint16_t *val, uint8_t id)
{
  param_req_grant();
  if(id < ENCODER_NUMBER)
    *val = params->encoder[id];
  param_rel_grant();
}

void param_set_flag(uint32_t flag)
{
  param_req_grant();
  params->flags = flag;
  param_rel_grant();
}

void param_get_flag(uint32_t *flag)
{
  param_req_grant();
  *flag = params->flags;
  param_rel_grant();
}

void param_set_flag_bit(uint32_t bit)
{
  param_req_grant();
  params->flags |= bit;
  param_rel_grant();
}

void param_clr_flag_bit(uint32_t bit)
{
  param_req_grant();
  params->flags &= ~bit;
  param_rel_grant();
}

static void param_req_grant(void)
{
  osMutexWait(paramMutex, osWaitForever);
}

static void param_rel_grant(void)
{
  osMutexRelease(paramMutex);
}
