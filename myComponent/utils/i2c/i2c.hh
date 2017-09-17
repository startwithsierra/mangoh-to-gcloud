// i2c.hh ---
//
// Author: Majdi Toumi
// Created: Wed Feb  8 16:58:58 2017 (+0100)
// Last-Updated: Wed Feb 15 19:51:09 2017 (+0100)
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

#ifndef __I2C_HH__
# define __I2C_HH__

#include <fstream>
#include <string>

/**
 * @CLASS I2c
 * @brief I2C-bus wrapper
 */
class I2c
{
  //
  // METHODS
  //

public:

  // contructor
  I2c();

  /**
   * @method open
   * @brief  open a bus (block|non_block mode)
   *
   * @return nothing
   *
   * Example Usage:
   * @code
   * open("/dev/i2c-0", true);
   */
  void	open(std::string filename = "/dev/i2c-0", bool is_non_block = false) throw(const char*);

  /**
   * @method setAddress
   * @brief  configure device slave address
   *
   * @return [int] address
   *
   * Example Usage:
   * @code
   * setAddress(0x6A);
   */
  void	setAddress(int address) throw(const char*);

  /**
   * @method readByteData
   * @brief  read uint8 data from a specific address
   *
   * @return [uint8_t] register address
   * @return [uint8_t] buffer
   *
   * Example Usage:
   * @code
   * readByteData(LSM6DS3_ACC_GYRO_WHO_AM_I_REG, &v);
   */
  int	readByteData(uint8_t register_address, uint8_t* value) throw(const char*);

  /**
   * @method readUint16Data
   * @brief  read uint16 data from a specific address
   *
   * @return [uint8_t] register address
   * @return [uint16_t] buffer
   *
   * Example Usage:
   * @code
   * readUint16Data(offset, &v);
   */
  int	readUint16Data(uint8_t register_address, int16_t* value) throw(const char*);

  /**
   * @method writedByteData
   * @brief  write uint8 data to a specific address
   *
   * @return [uint8_t] register address
   * @return [uint8_t] value
   *
   * Example Usage:
   * @code
   * writeByData(offset, &v);
   */
  void	writeByteData(uint8_t register_address, uint8_t value) throw(const char*);

  /**
   * @method close
   * @brief  close the file descriptor
   *
   * Example Usage:
   * @code
   * close();
   */
  int	close();

  //
  // ATTRIBUTES
  //

private:

  // current file descriptor
  int		_fd;

  // bus filename
  std::string	_filename;
};

#endif // !__I2C_HH__
