// imu.hh ---
//
// Author: Majdi Toumi
// Created: Sat Feb 11 15:43:52 2017 (+0100)
// Last-Updated: Sun Feb 12 22:08:04 2017 (+0100)
//           By: Majdi Toumi
// Version: 1.0.0
//
// THE TEA-WARE LICENSE:
// Majdi Toumi wrote this file
// As long you retain this notice, you can do whatever
// you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a cup of tea in return.
//
// Let's Rock!
//

#ifndef __IMU_STATUS_HH__
# define __IMU_STATUS_HH__

typedef enum {
  IMU_SUCCESS = 0,
  IMU_HW_ERROR,
  IMU_NOT_SUPPORTED,
  IMU_GENERIC_ERROR,
  IMU_OUT_OF_BOUNDS,
  IMU_ALL_ONES_WARNING,
} status_t;

#endif // !__IMU_STATUS_HH__
