# SWS :: MangOH to Google Cloud IoT

[![](https://img.shields.io/badge/legato-17.06-blue.svg)](http://legato.io/) [![](https://img.shields.io/badge/mangoh-green-green.svg)](http://mangoh.io/)
[![](https://img.shields.io/badge/mangoh-red-red.svg)](http://mangoh.io/)

> :book: This application is about building a **device to cloud** application with **Google Cloud IoT**

## <a name='TOC'>Summary</a>

- [Overview](#overview)
- [Sources code](#sources)
- [Credits](#credits)

## <a name='note'>Overview</a>

This application illustrates how to send data to a **Google Cloud IoT** platform from a **Legato**
application running on a **AirPrime WP** module on **mangOH Green** or **mangOH Red** open source
hardware platforms.<br />

The tutorial details the source files of the Legato application and shows how to build, install and
run the Legato application that sends mangOH native sensor data [ temperature + gyroscope ] to
the cloud using **MQTT** protocol through **Google Cloud IoT** platform.<br />

We'll build, install and run the application with the open source Linux-based embedded platform
**Legato** toolchain and installation command line.

> You must first install and run the `utils-legato-mqtt` application available
[here](https://github.com/startwithsierra/utils-legato-mqtt) which is the MQTT client
allowing our device to cloud application (`MQTT` publisher) to communicate.

## <a name='sources'>Sources code</a>

Select a specific board to access to the right source code:
- [**MangOH Green**](https://github.com/startwithsierra/mangoh-to-gcloud/tree/green)
- [**MangOH Red**](https://github.com/startwithsierra/mangoh-to-gcloud/tree/red)

### Credits

Develop with :heart: by [**Majdi Toumi**](http://majditoumi.com)|[**Mhirba**](http://mhirba.com) in **Paris** for [**Sierra Wireless**](https://www.sierrawireless.com/).

[](https://www.newconstructs.com/wp-content/uploads/2014/10/sierra-wireless-inc-logo.jpg)
