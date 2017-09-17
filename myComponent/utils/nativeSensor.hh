// nativeSensor.hh ---
//
// Author: Majdi Toumi
// Created: Sat Sep 17 10:58:58 2017 (+0100)
// Last-Updated: Sat Sep 17 11:42:50 2017 (+0100)n
//           By: Majdi Toumi
// Version: 1.0.0
//
// THE AIEOWTEA-WARE LICENSE
// Majdi Toumi wrote this file
// As long you retain this notice, you can do whatever
// you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a cup of tea in return.
//
// Let's Rock!
//

#ifndef __NATIVE_SENSOR_HH__
# define __NATIVE_SENSOR_HH__

#include <fstream>
#include <string>
#include "legato.h"

/**
 * @class NativeSensor
 * @brief Native sensor bus wrapper
 */
class NativeSensor
{
  //
  // METHODS
  //

public:

  // contructor
  NativeSensor();

  /**
   * @method readAccelerometer
   * @brief  Reports the x, y and z accelerometer readings in meters per second squared.
   *
   * @return LE_OK if success OR the specific error
   *
   * Example Usage:
   * @code
   * readAccelerometer(&xAcc, &yAcc, &zAcc);
   */
  le_result_t readAccelerometer(double *xAcc, double *yAcc, double *zAcc);

  /**
   * @method readGyro
   * @brief  Reports the x, y and z gyro readings in radians per second.
   *
   * @return LE_OK if success OR the specific error
   *
   * Example Usage:
   * @code
   * readGyro(&x, &y, &z);
   */
  le_result_t readGyro(double *x, double *y, double *z);

  /**
   * @method readTemperature
   * @brief  Reports the temperature in degrees celcius.
   *
   * @return LE_OK if success OR the specific error
   *
   * Example Usage:
   * @code
   * readTemperature(&temp);
   */
  le_result_t readTemperature(double *reading);

private:
  le_result_t readIntFromFile(const char *filePath, int *value);
  le_result_t readDoubleFromFile(const char *filePath, double *value);

};

#endif // !__NATIVE_SENSOR_HH__
