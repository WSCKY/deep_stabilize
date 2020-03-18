/*
 * AttEst.h
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#ifndef __ATTEST_H
#define __ATTEST_H

#include "icm20602.h"

#include "maths.h"

#if BOARD_IMU_ENABLE

Quat_T get_est_q(void);
IMU_UNIT get_imu_unit(void);

#endif /* BOARD_IMU_ENABLE */

#endif /* __ATTEST_H */
