// lsm6ds3.cc ---
//
// Author: Majdi Toumi
// Created: Wed Feb  8 16:58:58 2017 (+0100)
// Last-Updated: Sun Feb 12 22:08:41 2017 (+0100)
//           By: Majdi Toumi
// Version: 1.0.0
//
// THE AIOWTEA-WARE LICENSE
// Majdi Toumi wrote this file
// As long you retain this notice, you can do whatever
// you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a cup of tea in return.
//
// Let's Rock!
//

#include "legato.h"
#include "lsm6ds3.hh"

Lsm6ds3::Lsm6ds3()
{
  // (1) gyroscope control data

  // can be 0 or 1
  _sensor_settings.gyroscope.enabled = 1;

  // max deg/s .- can be: 125, 245, 500, 1000, 2000
  _sensor_settings.gyroscope.range = 2000;

  // [Hz] .-  can be: 13, 26, 52, 104, 208, 416, 833, 1666
  _sensor_settings.gyroscope.sample_rate = 416;

  // [Hz] .-  can be: 50, 100, 200, 400
  _sensor_settings.gyroscope.bandwidth = 400;

  // set to include gyroscope in FIFO
  _sensor_settings.gyroscope.fifo_enabled = 1;

  // set 1 for on
  _sensor_settings.gyroscope.fifo_decimation = 1;

  // (2) acceleration control data

  _sensor_settings.acceleration.enabled = 1;
  _sensor_settings.acceleration.odr_off = 1;

  // max G-force readable .- can be: 2, 4, 8, 16
  _sensor_settings.acceleration.range = 16;

  // [Hz] .- can be: 13, 26, 52, 104, 208, 416, 833, 1666, 3332, 6664, 13330
  _sensor_settings.acceleration.sample_rate = 416;

  // [Hz] .- can be: 50, 100, 200, 400;
  _sensor_settings.acceleration.bandwidth = 100;

  // set to include accelerometer in the FIFO
  _sensor_settings.acceleration.fifo_enabled = 1;

  // set 1 for on
  _sensor_settings.acceleration.fifo_decimation = 1;

  // (3) temperature control data
  _sensor_settings.temperature_enabled = 1;

  // (4) select interface mode .- can be modes: 1, 2 or 3
  _sensor_settings.comm_mode = 1;

  // (5) FIFO control data

  // can be 0 to 4096 (16 bit bytes)
  _sensor_settings.fifo.threshold = 3000;

  // default 10Hz
  _sensor_settings.fifo.sample_rate = 10;

  // default off
  _sensor_settings.fifo.mode_word = 0;

  _err_all_ones_counter = 0;
  _err_non_success_counter = 0;

  // let's initialize I2cC
  this->i2cInitialization();
}


void Lsm6ds3::i2cInitialization(int i2c_address)
{
  uint8_t byte[16];

  // opening the serial interface
  _i2c.open();

  // set a specific address
  _i2c.setAddress(i2c_address);

  // read byte data from the WHO_AM_I register
  _i2c.readByteData(LSM6DS3_ACC_GYRO_WHO_AM_I_REG, &byte[0]);
}

status_t Lsm6ds3::writeRegister(uint8_t offset, uint8_t data_to_write)
{
  // write to the serial interface
  _i2c.writeByteData(offset, data_to_write);
  return IMU_SUCCESS;
}

status_t Lsm6ds3::readRegister(uint8_t* output_ptr, uint8_t offset)
{
  // read from the serial interface
  _i2c.readByteData(offset, output_ptr);
  return IMU_SUCCESS;
}

status_t Lsm6ds3::readRegisterInt16(int16_t* output_ptr, uint8_t offset)
{
  // read uint16 from our serial interface
  _i2c.readUint16Data(offset, output_ptr);
  return IMU_SUCCESS;
}

status_t Lsm6ds3::begin()
{
  uint8_t data_to_write = 0;

  if (_sensor_settings.acceleration.enabled == 1) {

    // Build config reg
    // First patch in filter bandwidth
    switch (_sensor_settings.acceleration.bandwidth) {
    case 50:
      data_to_write |= LSM6DS3_ACC_GYRO_BW_XL_50Hz;
      break;
    case 100:
      data_to_write |= LSM6DS3_ACC_GYRO_BW_XL_100Hz;
      break;
    case 200:
      data_to_write |= LSM6DS3_ACC_GYRO_BW_XL_200Hz;
      break;
    default:  //set default case to max passthrough
    case 400:
      data_to_write |= LSM6DS3_ACC_GYRO_BW_XL_400Hz;
      break;
    }

    //Next, patch in full scale
    switch (_sensor_settings.acceleration.range) {
    case 2:
      data_to_write |= LSM6DS3_ACC_GYRO_FS_XL_2g;
      break;
    case 4:
      data_to_write |= LSM6DS3_ACC_GYRO_FS_XL_4g;
      break;
    case 8:
      data_to_write |= LSM6DS3_ACC_GYRO_FS_XL_8g;
      break;
    default:  //set default case to 16(max)
    case 16:
      data_to_write |= LSM6DS3_ACC_GYRO_FS_XL_16g;
      break;
    }

    //Lastly, patch in accelerometer ODR
    switch (_sensor_settings.acceleration.sample_rate) {
    case 13:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_XL_13Hz;
      break;
    case 26:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_XL_26Hz;
      break;
    case 52:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_XL_52Hz;
      break;
    default:  //Set default to 104
    case 104:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_XL_104Hz;
      break;
    case 208:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_XL_208Hz;
      break;
    case 416:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_XL_416Hz;
      break;
    case 833:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_XL_833Hz;
      break;
    case 1660:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_XL_1660Hz;
      break;
    case 3330:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_XL_3330Hz;
      break;
    case 6660:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_XL_6660Hz;
      break;
    case 13330:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_XL_13330Hz;
      break;
    }
  }

  // Now, write the patched together data
  this->writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, data_to_write);

  // Set the ODR bit
  this->readRegister(&data_to_write, LSM6DS3_ACC_GYRO_CTRL4_C);

  data_to_write &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);

  if (_sensor_settings.acceleration.odr_off == 1) {
    data_to_write |= LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED;
  }
  this->writeRegister(LSM6DS3_ACC_GYRO_CTRL4_C, data_to_write);

  // Setup the gyroscope

  data_to_write = 0; //Start Fresh!
  if (_sensor_settings.gyroscope.enabled == 1) {

    // Build config reg
    // First, patch in full scale
    switch (_sensor_settings.gyroscope.range) {
    case 125:
      data_to_write |= LSM6DS3_ACC_GYRO_FS_125_ENABLED;
      break;
    case 245:
      data_to_write |= LSM6DS3_ACC_GYRO_FS_G_245dps;
      break;
    case 500:
      data_to_write |= LSM6DS3_ACC_GYRO_FS_G_500dps;
      break;
    case 1000:
      data_to_write |= LSM6DS3_ACC_GYRO_FS_G_1000dps;
      break;
    default:  //Default to full 2000DPS range
    case 2000:
      data_to_write |= LSM6DS3_ACC_GYRO_FS_G_2000dps;
      break;
    }

    //Lastly, patch in gyro ODR
    switch (_sensor_settings.gyroscope.sample_rate) {
    case 13:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_G_13Hz;
      break;
    case 26:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_G_26Hz;
      break;
    case 52:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_G_52Hz;
      break;
    default:  //Set default to 104
    case 104:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_G_104Hz;
      break;
    case 208:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_G_208Hz;
      break;
    case 416:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_G_416Hz;
      break;
    case 833:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_G_833Hz;
      break;
    case 1660:
      data_to_write |= LSM6DS3_ACC_GYRO_ODR_G_1660Hz;
      break;
    }
  }

  // Write the byte
  this->writeRegister(LSM6DS3_ACC_GYRO_CTRL2_G, data_to_write);

  // Return WHO AM I reg  //Not no mo!
  uint8_t result;
  this->readRegister(&result, LSM6DS3_ACC_GYRO_WHO_AM_I_REG);

  return IMU_SUCCESS;
}

float Lsm6ds3::calculateAcceleration(int16_t input)
{
  // depending our settings and sensitivity to linear cceleration
  return (float)(input) * 0.061 * (_sensor_settings.acceleration.range >> 1) / 1000;
}

// ACCELEROMETER

int16_t Lsm6ds3::readRawAccelerationByAxis(char axis)
{
  int16_t	output;
  status_t	ret;
  uint8_t	offset;

  switch(axis) {
  case 'X':
    offset = LSM6DS3_ACC_GYRO_OUTX_L_XL;
    break;
  case 'Y':
    offset = LSM6DS3_ACC_GYRO_OUTY_L_XL;
    break;
  default:
  case 'Z':
    offset = LSM6DS3_ACC_GYRO_OUTZ_L_XL;
    break;
  }

  ret = this->readRegisterInt16(&output, offset);

  if (ret != IMU_SUCCESS) {
    if (ret == IMU_ALL_ONES_WARNING) {
      _err_all_ones_counter++;
    } else {
      _err_non_success_counter++;
    }
  }
  return output;
}

float Lsm6ds3::readAccelerationByAxis(char axis)
{
  return this->calculateAcceleration(this->readRawAccelerationByAxis(axis));
}

// Gyroscope

float Lsm6ds3::calculateGyroscope(int16_t input)
{
  uint8_t gyroscope_range_divisor = _sensor_settings.gyroscope.range == 245 ? 2 : _sensor_settings.gyroscope.range / 125;

  return (float)(input) * 4.375 * (gyroscope_range_divisor) / 1000;
}

int16_t Lsm6ds3::readRawGyroscopeByAxis(char axis)
{
  int16_t	output;
  status_t	ret;
  uint8_t	offset;

  switch(axis) {
  case 'X':
    offset = LSM6DS3_ACC_GYRO_OUTX_L_G;
    break;
  case 'Y':
    offset = LSM6DS3_ACC_GYRO_OUTY_L_G;
    break;
  default:
  case 'Z':
    offset = LSM6DS3_ACC_GYRO_OUTZ_L_G;
    break;
  }

  ret = this->readRegisterInt16(&output, offset);

  if (ret != IMU_SUCCESS) {
    if (ret == IMU_ALL_ONES_WARNING) {
      _err_all_ones_counter++;
    }
    else {
      _err_non_success_counter++;
    }
  }

  return output;
}

float Lsm6ds3::readGyroscopeByAxis(char axis)
{
  return this->calculateGyroscope(this->readRawGyroscopeByAxis(axis));
}

// Temperature

int16_t Lsm6ds3::readRawTemperature()
{
  int16_t output;

  this->readRegisterInt16(&output, LSM6DS3_ACC_GYRO_OUT_TEMP_L);
  return output;
}

float Lsm6ds3::readTemperatureByUnity(char unity)
{
  // divide by 16 to scale
  float output = (float)this->readRawTemperature() / 16;

  // add 25 degrees to remove offset
  output += 25;

    if (unity == 'F') {
    output = (output * 9) / 5 + 32;
  }

  return output;
}
