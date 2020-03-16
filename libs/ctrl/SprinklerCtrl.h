/**
 * @file    SprinklerCtrl.h
 * @author  kyChu<kychu@qq.com>
 * @date    Mar 9, 2020
 * @version V1.0.0
 * @brief   API for Sprinkler Controller.
 */

#ifndef _SPRINKLERCTRL_H_
#define _SPRINKLERCTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * input argument: reserved.
 */
typedef void (*SprinklerCtrl_adj_done_callback)(unsigned int);

/*
 * @brief  communication start.
 * @param  dev: device node, e.g. "/dev/ttyUSB0".
 * @retval 0 is OK, or negative value means error.
 */
int SprinklerCtrl_start(const char *dev);

/*
 * @brief  set controller event callback.
 * @param  callback: callback handler.
 * @retval 0 is OK, or -1 is error.
 */
int SprinklerCtrl_config_callback(SprinklerCtrl_adj_done_callback callback);

/*
 * @brief  enable or disable stability augmentation control.
 * @param  e: 0 to disable, 1 to enable.
 * @retval 0 is OK, or negative value means error.
 */
int SprinklerCtrl_enable_stabilize(int e);
/*
 * @brief  set pitch angle. (based on water level)
 * @param  pitch: angle value (unit: deg)
 * @retval 0 is OK, or negative value means error.
 */
int SprinklerCtrl_set_pitch(float pitch);

/*
 * @brief  set yaw angle. (based on vehicle heading)
 * @param  yaw: angle value
 * @retval 0 is OK, or negative value means error.
 */
int SprinklerCtrl_set_yaw(float yaw);

/*
 * @brief  get pitch angle. (based on water level)
 * @param  none
 * @retval angle of pitch. (unit: deg)
 */
float SprinklerCtrl_get_pitch(void);

/*
 * @brief  get pitch angle rate.
 * @param  none
 * @retval angle rate of pitch. (unit: deg/s)
 */
float SprinklerCtrl_get_pitchrate(void);

/*
 * @brief  get the measurements of encoder which mounted on pitch axis.
 * @param  none
 * @retval measured value.
 */
int SprinklerCtrl_get_pitchencoder(void);

/*
 * @brief  get the measurements of encoder which mounted on yaw axis.
 * @param  none
 * @retval measured value.
 */
int SprinklerCtrl_get_yawencoder(void);

int SprinklerCtrl_set_exp(float pitch, float yaw);
int SprinklerCtrl_get_angle(float *pitch, float *yaw);

/*
 * @brief  exit safely.
 * @param  none
 * @retval none
 */
int SprinklerCtrl_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* _SPRINKLERCTRL_H_ */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
