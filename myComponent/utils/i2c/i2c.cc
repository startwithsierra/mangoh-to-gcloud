// i2c.cc ---
//
// Author: Majdi Toumi
// Created: Wed Feb  8 16:58:58 2017 (+0100)
// Last-Updated: Sun Feb 12 21:51:20 2017 (+0100)
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

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include "legato.h"
#include "i2c.hh"

I2c::I2c() {}


void I2c::open(std::string filename, bool is_non_block) throw(const char*)
{
  int oflags;

  _filename = filename;

  LE_INFO("FILENAME: %s", _filename.c_str());

  oflags = O_RDWR;
  if (is_non_block == true) {
    LE_INFO("FLAGS");
    oflags |= O_NONBLOCK;
  }

  _fd = ::open(_filename.c_str(), oflags);

  if(_fd == -1) {
    LE_INFO("Failed to open the bus. File name %s Error %d\n", _filename.c_str(), _fd);
    throw("I2c::open - open");
  }

  return;
}

void I2c::setAddress(int address) throw(const char*)
{
  // Change slave address of the device we want to access
  // The address is passed in the 7 lower bits of the argument
  // @note: except for 10 bit addresses, passed in the 10 lower bits in this case
  if (-1 == ::ioctl(_fd, I2C_SLAVE, address)) {
    throw("I2c::setAddress - ioctl");
  }
  return;
}

int I2c::readByteData(uint8_t register_address, uint8_t* value) throw(const char*)
{
  if (-1 == ::write(_fd, &register_address, 1)) {
    throw("I2c::readByteData - write");
  }

  return read(_fd, value, 1);
}

int I2c::readUint16Data(uint8_t register_address, int16_t* value) throw(const char*)
{
  if (-1 == ::write(_fd, &register_address, 1)) {
    throw("I2c::readByteData - write");
  }

  return read(_fd, value, 2);
}

void I2c::writeByteData(uint8_t register_address, uint8_t value) throw(const char*)
{
  uint8_t	data[2];

  data[0] = register_address;
  data[1] = value;

  if (-1 == ::write(_fd, &data, 2)) {
    throw("I2c::writeByteData - write");
  };

  return;
}

int I2c::close()
{
  return ::close(_fd);
}
