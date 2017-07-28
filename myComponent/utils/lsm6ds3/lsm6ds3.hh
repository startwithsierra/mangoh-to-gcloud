// lsm6d3.hh ---
//
// Author: Majdi Toumi
// Created: Wed Feb  8 16:55:37 2017 (+0100)
// Last-Updated: Wed Feb 15 19:57:33 2017 (+0100)
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

#ifndef __LSM6DS3_HH__
# define __LSM6DS3_HH__

#include "i2c.hh"
#include "imu.hh"
#include "lsm6ds3_registers.hh"
#include "lsm6ds3_types.hh"

#define LSM6DS3_DEFAULT_I2C_ADDRESS			0x6A

/**
 * @class Lsm6ds3
 * @brief a simple LSM6DS3 module interface
 */
class Lsm6ds3
{
  //
  // METHODS
  //

public:

  /**
   * @method Lsm6ds3
   * @brief  constructor
   */
  Lsm6ds3();

  /**
   * @method begin
   * @brief  configuration section based on sensor settings attributes
   *
   * @return [status_t] inertial measurement unit value
   */
  status_t begin();

  /**
   * @method i2cInitialization
   * @brief  just configure I2C serial interface access
   *
   * @return nothing
   */
  void i2cInitialization(int i2c_address = LSM6DS3_DEFAULT_I2C_ADDRESS);

  /**
   * @method writeRegister
   * @brief  write data to a specific I2C
   *
   * @param [uint8_t] offset
   * @param [uint8_t] data to write
   *
   * @return nothing
   *
   * Example usage:
   * @code
   * writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dtw);
   */
  status_t writeRegister(uint8_t offset, uint8_t data_to_write);

  /**
   * @method readRegister
   * @brief  read data from a specific I2C
   *
   * @param [uint8_t*] output pointer
   * @param [uint8_t] offset
   *
   * @return nothing
   *
   * Example usage:
   * @code
   * readRegister(&dtw, LSM6DS3_ACC_GYRO_CTRL4_C);
   */
  status_t readRegister(uint8_t* ouput_ptr, uint8_t offset);

  /**
   * @method readRegisterInt16
   * @brief  read int16 data from a specific I2C
   *
   * @param [int16_t*] output pointer
   * @param [uint8_t] offset
   *
   * @return nothing
   *
   * Example usage:
   * @code
   * readRegisterInt16(&dtw, LSM6DS3_ACC_GYRO_OUT_TEMP_L);
   */
  status_t readRegisterInt16(int16_t* output_ptr, uint8_t offset);

  /**
   * @method calculateAcceleration
   * @brief  calculate the acceleration from sensor settings
   *
   * @param [int16_t] input
   *
   * @return [float]
   *
   * Example usage:
   * @code
   *
   */
  float	 calculateAcceleration(int16_t input);

  /**
   * @method readRawAccelerationByAxis
   * @brief  read raw acceleration data depending on axis
   *
   * @param [char] axis
   *
   * @return [int16_t]
   *
   * Example usage:
   * @code
   * readRawAccelerationByAxis('X');
   */
  int16_t readRawAccelerationByAxis(char axis);

  /**
   * @method readAccelerationByAxis
   * @brief  read float acceleration data depending on axis
   *
   * @param [char] axis
   *
   * @return [float]
   *
   * Example usage:
   * @code
   * readAccelerationByAxis('X');
   */
  float readAccelerationByAxis(char axis);

  /**
   * @method calculateGyroscope
   * @brief  calculate the gyroscope range from sensor settings
   *
   * @param [int16_t] input
   *
   * @return [float]
   *
   * Example usage:
   * @code
   *
   */
  float calculateGyroscope(int16_t input);

  /**
   * @method readRawGyroscopeByAxis
   * @brief  read raw gyroscope range depending on axis
   *
   * @param [char] axis
   *
   * @return [int16_t]
   *
   * Example usage:
   * @code
   * readRawGyroscopeByAxis('X');
   */
  int16_t readRawGyroscopeByAxis(char axis);

  /**
   * @method readGyroscopeByAxis
   * @brief  read float gyroscope range depending on axis
   *
   * @param [char] axis
   *
   * @return [float]
   *
   * Example usage:
   * @code
   * readGyroscopeByAxis('X');
   */
  float readGyroscopeByAxis(char axis);

  /**
   * @method readRawTemperature
   * @brief  read raw temperature from register
   *
   * @return [int16_t]
   */
  int16_t readRawTemperature();

  /**
   * @method readTemperatureByUnity
   * @brief  read floattemperature depending on conversion unity
   *
   * @return [float]
   *
   * Example usage:
   * @code
   * readTemperatureByUnity('C');
   */
  float readTemperatureByUnity(char unity);

private:

  // I2C serial interface standard output
  I2c	_i2c;

  // native sensor driver settings
  struct SensorSetting {

    // about gyroscope
    struct {
      uint8_t	enabled;
      uint16_t	range;
      uint16_t	sample_rate;
      uint16_t	bandwidth;

      uint8_t 	fifo_enabled;
      uint8_t	fifo_decimation;
    } gyroscope;

    // about accelerometer
    struct {
      uint8_t	enabled;
      uint8_t	odr_off;
      uint16_t	range;
      uint16_t	sample_rate;
      uint16_t	bandwidth;

      uint8_t	fifo_enabled;
      uint8_t	fifo_decimation;
    } acceleration;

    // about temperature

    uint8_t	temperature_enabled;

    // about non-basic mode settings

    uint8_t	comm_mode;

    // about FIFO control data
    struct {
      uint16_t	threshold;
      int16_t	sample_rate;
      uint8_t	mode_word;
    } fifo;

  } _sensor_settings;

  // metric error for received all ones report
  uint16_t	_err_all_ones_counter;

  // metric error for all failed access
  uint16_t	_err_non_success_counter;
};

#endif // !__LSM6DS3_HH__
