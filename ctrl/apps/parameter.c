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

static Params_t* params;
static osMutexId paramMutex;

osMutexDef(PARAMMutex);

#define PARAMETER_SAVE_FLASH_ADDR                0x0801F800

#define PARAMETER_SAVE_FLASH_FLAG                0x5AA5A55A

static void param_req_grant(void);
static void param_rel_grant(void);

status_t param_init(void)
{
  uint32_t temp, addr;
  /* create the MUTEX object */
  paramMutex = osMutexCreate(osMutex(PARAMMutex));
  if(paramMutex == NULL) return status_error;
  params = kmm_alloc(sizeof(Params_t));
  if(params == NULL) return status_nomem;
  memset(params, 0, sizeof(Params_t));
  addr = PARAMETER_SAVE_FLASH_ADDR;
  temp = *(__IO uint32_t *)addr;
  if(temp != PARAMETER_SAVE_FLASH_FLAG) {
    params->flags |= ENCODER_CAL_BIT; // need calibrate.
  }

  return status_ok;
}

void param_get_param(Params_t *param)
{
  param_req_grant();
  memcpy(param, params, sizeof(Params_t));
  param_rel_grant();
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
  if(id < ENCODER_NUMBER) {
    params->encoder[id] = val;
//    params->flags &= ~(0x1 << (ENCODER_ERROR_BIT_OFF + id));
  }
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

bool param_get_flag_bit(uint32_t bit)
{
  bool ret = false;
  param_req_grant();
  if((params->flags & bit) != 0) ret = true;
  param_rel_grant();
  return ret;
}

void param_cfg_flag_bits(uint32_t mask, uint32_t bits)
{
  param_req_grant();
  params->flags &= mask;
  params->flags |= (~mask & bits);
  param_rel_grant();
}

void param_clr_flash(void)
{
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
  FLASH_ErasePage(PARAMETER_SAVE_FLASH_ADDR);
  FLASH_Lock();
}

void param_sav_flash(void)
{
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
  if(*(__IO uint32_t *)PARAMETER_SAVE_FLASH_ADDR != 0xFFFFFFFF)
    FLASH_ErasePage(PARAMETER_SAVE_FLASH_ADDR);
  FLASH_ProgramWord(PARAMETER_SAVE_FLASH_ADDR, PARAMETER_SAVE_FLASH_FLAG);
  FLASH_Lock();
}

static void param_req_grant(void)
{
  osMutexWait(paramMutex, osWaitForever);
}

static void param_rel_grant(void)
{
  osMutexRelease(paramMutex);
}
