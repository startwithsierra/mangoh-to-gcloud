// main.cc ---
//
// Author: Majdi Toumi
// Created: Fri Feb 10 17:13:24 2017 (+0100)
// Last-Updated: Thu Feb 16 14:28:15 2017 (+0100)
//           By: Majdi Toumi
// Version:
//
// THE TEA-WARE LICENSE:
// Majdi Toumi wrote this file
// As long you retain this notice, you can do whatever
// you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a cup of tea in return.
//
// Let's Rock!
//

#include "legato.h"
#include "sensorToGoogleCloud.hh"

// Application entry point --> Let's Rock!
COMPONENT_INIT
{
  LE_INFO("Hi from s2cgcloud app! ");

  // instantiate our application as a singleton
  SensorToGoogleCloud* s = SensorToGoogleCloud::getInstance();

  s->start();
}
