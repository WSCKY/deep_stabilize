/**
 * @file    SprinklerCtrl.h
 * @author  kyChu
 * @date    Mar 9, 2020
 * @version V1.0.0
 * @brief   API for Sprinkler Controller.
 */

#ifndef SPRINKLERCTRL_H_
#define SPRINKLERCTRL_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int SprinklerCtrl_start(const char *dev, const char *baudrate);

int SprinklerCtrl_enable_stabilize(int e);
int SprinklerCtrl_set_pitch(float pitch);
int SprinklerCtrl_set_yaw(float yaw);

float SprinklerCtrl_get_pitch(void);
float SprinklerCtrl_get_pitchrate(void);

int SprinklerCtrl_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* SPRINKLERCTRL_H_ */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
