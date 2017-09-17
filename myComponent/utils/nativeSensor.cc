// nativeSensor.cc ---
//
// Author: Majdi Toumi
// Created: Sat Sep 17 10:58:58 2017 (+0100)
// Last-Updated: Sat Sep 17 11:42:50 2017 (+0100)
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

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include "legato.h"
#include "interfaces.h"
#include "nativeSensor.hh"

static const char FormatStr[] = "/sys/devices/i2c-0/0-0068/iio:device0/in_%s_%s";
static const char AccType[]   = "accel";
static const char GyroType[]  = "anglvel";
static const char CompX[]     = "x_raw";
static const char CompY[]     = "y_raw";
static const char CompZ[]     = "z_raw";
static const char CompScale[] = "scale";

static const char TemperatureFile[] = "/sys/devices/i2c-0/0-0076/iio:device1/in_temp_input";

le_result_t NativeSensor::readAccelerometer(double *xAcc, double *yAcc, double *zAcc)
{
  le_result_t r;
  char path[256];

  double scaling = 0.0;
  unsigned int pathLen = snprintf(path, sizeof(path), FormatStr, AccType, CompScale);
  LE_ASSERT(pathLen < sizeof(path));
  r = this->readDoubleFromFile(path, &scaling);
  if (r != LE_OK) {
    return r;
  }

  pathLen = snprintf(path, sizeof(path), FormatStr, AccType, CompX);
  LE_ASSERT(pathLen < sizeof(path));
  r = this->readDoubleFromFile(path, xAcc);
  if (r != LE_OK) {
    return r;
  }
  *xAcc *= scaling;

  pathLen = snprintf(path, sizeof(path), FormatStr, AccType, CompY);
  LE_ASSERT(pathLen < sizeof(path));
  r = this->readDoubleFromFile(path, yAcc);
  if (r != LE_OK) {
    return r;
  }
  *yAcc *= scaling;

  pathLen = snprintf(path, sizeof(path), FormatStr, AccType, CompZ);
  LE_ASSERT(pathLen < sizeof(path));
  r = this->readDoubleFromFile(path, zAcc);
  *zAcc *= scaling;

  return r;
}

le_result_t NativeSensor::readGyro(double *x, double *y, double *z)
{
  le_result_t r;
  char path[256];

  double scaling = 0.0;
  unsigned int pathLen = snprintf(path, sizeof(path), FormatStr, GyroType, CompScale);
  LE_ASSERT(pathLen < sizeof(path));
  r = this->readDoubleFromFile(path, &scaling);
  if (r != LE_OK) {
    return r;
  }

  pathLen = snprintf(path, sizeof(path), FormatStr, GyroType, CompX);
  LE_ASSERT(pathLen < sizeof(path));
  r = this->readDoubleFromFile(path, x);
  if (r != LE_OK) {
    return r;
  }
  *x *= scaling;

  pathLen = snprintf(path, sizeof(path), FormatStr, GyroType, CompY);
  LE_ASSERT(pathLen < sizeof(path));
  r = this->readDoubleFromFile(path, y);
  if (r != LE_OK) {
    return r;
  }
  *y *= scaling;

  pathLen = snprintf(path, sizeof(path), FormatStr, GyroType, CompZ);
  LE_ASSERT(pathLen < sizeof(path));
  r = this->readDoubleFromFile(path, z);
  *z *= scaling;

  return r;
}

le_result_t NativeSensor::readTemperature(double *reading)
{
  int temp;
  le_result_t r = this->readIntFromFile(TemperatureFile, &temp);
  if (r != LE_OK) {
    return r;
  }

  // The divider is 1000 based on the comments in the kernel driver on bmp280_compensate_temp()
  // which is called by bmp280_read_temp()
  *reading = ((double)temp) / 1000.0;
  return LE_OK;
}

le_result_t NativeSensor::readIntFromFile(const char *filePath, int *value)
{
    le_result_t r = LE_OK;
    FILE *f = fopen(filePath, "r");
    if (f == NULL) {
      LE_WARN("Couldn't open '%s' - %m", filePath);
      r = LE_IO_ERROR;
      return r;
    }

    int numScanned = fscanf(f, "%d", value);
    if (numScanned != 1) {
      r = LE_FORMAT_ERROR;
    }

    fclose(f);
    return r;
}

le_result_t NativeSensor::readDoubleFromFile(const char *filePath, double *value)
{
    le_result_t r = LE_OK;
    FILE *f = fopen(filePath, "r");
    if (f == NULL) {
      LE_WARN("Couldn't open '%s' - %m", filePath);
      r = LE_IO_ERROR;
      return r;
    }

    int numScanned = fscanf(f, "%lf", value);
    if (numScanned != 1){
      r = LE_FORMAT_ERROR;
    }

    fclose(f);
    return r;
}
