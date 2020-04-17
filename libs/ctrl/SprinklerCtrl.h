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

typedef enum {
  sprinkler_adj_done = 0,
  pitch_encoder_error = 1,
  yaw_encoder_error = 2,
  pitch_lost_control = 3,
  yaw_lost_control = 4,
  invalid_error_occured = 0xF,
} event_type_t;

/*
 * input argument: reserved.
 */
typedef void (*SprinklerCtrl_event_callback)(event_type_t);

/*
 * @brief  communication start.
 * @param  dev: device node, e.g. "/dev/ttyUSB0".
 * @param  callback: event callback handler.
 * @retval 0 is OK, or negative value means error.
 */
int SprinklerCtrl_start(const char *dev, SprinklerCtrl_event_callback callback);

/*
 * @brief  enable or disable stability augmentation control.
 * @param  e: 0 to disable, 1 to enable.
 * @retval 0 is OK, or negative value means error.
 */
int SprinklerCtrl_enable_stabilize(int e);

/*
 * @brief  power on/off external device.
 * @param  id: device id, from 0 to 1.
 * @param  cmd: 0 to power-off, 1 to power-on.
 * @retval operation state
 */
int SprinklerCtrl_device_ctrl(int id, int cmd);

/*
 * @brief  set pitch angle. (based on vehicle level)
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
 * @brief  get pitch angle. (based on vehicle level)
 * @param  none
 * @retval angle of pitch. (unit: deg)
 */
float SprinklerCtrl_get_pitch(void);

/*
 * @brief  get yaw angle. (based on vehicle heading)
 * @param  none
 * @retval angle of yaw. (unit: deg)
 */
float SprinklerCtrl_get_yaw(void);

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

int SprinklerCtrl_set_angle(float pitch, float yaw);
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
