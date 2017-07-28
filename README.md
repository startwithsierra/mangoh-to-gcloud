# SWS :: MangOH to Google Cloud IoT

[![](https://img.shields.io/badge/legato-17.06-blue.svg)](http://legato.io/) [![](https://img.shields.io/badge/mangoh-WP8548-green.svg)](http://legato.io/)

> :book: This application is about building a **device to cloud** application with **Google Cloud IoT**

## <a name='TOC'>Summary</a>

- [Overview](#overview)
- [Good to know](#good2know)
- [Specifications](#specifications)
- [Application Skeleton](#application-skeleton)
- [Application definition file](#application-definition-file)
- [Component](#component)
- [Steps](#steps)
- [Credits](#credits)

## <a name='note'>Overview</a>

This application is around building a **device to cloud** application which is
going to send MangOH native sensor data [ temperature + gyroscope ] to the cloud
using **MQTT** protocol through **Google Cloud IoT** platform.<br />

We'll build, install and run the application with the open source Linux-based embedded
platform **Legato** toolchain + installation command line.

> You must first install and run the `utils-legato-mqtt` application available
[here](https://github.com/startwithsierra/utils-legato-mqtt) which is the MQTT client
allowing our device to cloud application (`MQTT` publisher) to communicate.

### Setting up your Google IoT Cloud account

You must follow Google Cloud IoT [documentation](https://cloud.google.com/iot/docs/device_manager_guide?authuser=1)
to setup your gcloud account and create your project, registry, devices and channels.<br />
You must also generate all public/private key pair registered in your Google Cloud IoT project.

## <a name='good2know'>Good to know</a>

Theses are interested links around this usecase:

- [About I2c device interface](https://www.kernel.org/doc/Documentation/i2c/dev-interface)
- [I2c specification](https://dl.dropboxusercontent.com/u/28401099/mangoh.rocks/docs/I2c-specification.pdf)
- [LSM6DS3 specification](https://dl.dropboxusercontent.com/u/28401099/mangoh.rocks/docs/lsm6ds3-specification.pdf)

## <a name='specifications'>Specifications</a>

The `mangOH` board is using the `LSM6DS3` module which is an accelerometer + gyroscope sensor.<br />
This module provide a **8kb buffer** and an embedded processing interrupt functions specifically targed for the smart market.

> Thanks to the full open hardware philosophy, you can find all design resources in the official website.

Theses are somes features available from the `LSM6DS3`:
- **Accelerometer** for super accurate movement sensing
- **Gyroscope** data
- **Temperature**
- Built-in **FIFO buffer**
- **Pedometer**
- **Shocks detection**
- **Drive interrupt** pins by embedded functions or by FIFO low-capacity/overflow warning.

The `LSM6DS3` module is using the `I2C` (Inter-Integrated Circuit) which is a multi-{master,slave},
single-ended and serial computer bus.

## <a name='application-skeleton'>Application skeleton</a>

```sh
~/mhirba/sws/mangoh-to-gcloud ❯❯❯ tree .
.
├── Makefile
├── ReadMe.md
├── mangohToGCloud.adef
├── mqtt.api
└── myComponent
    ├── Component.cdef
    ├── config
    │   ├── ec_private.pem
    │   ├── ec_public.pem
    │   ├── rsa_cert.pem
    │   └── rsa_private.pem
    ├── main.cc
    ├── sensorToGoogleCloud.cc
    ├── sensorToGoogleCloud.hh
    └── utils
        ├── i2c
        │   ├── i2c.cc
        │   └── i2c.hh
        └── lsm6ds3
            ├── imu.hh
            ├── lsm6ds3.cc
            ├── lsm6ds3.hh
            ├── lsm6ds3_registers.hh
            └── lsm6ds3_types.hh

5 directories, 19 files
```

There is a main `myComponent` with the entry point, our main class and some
utils (`i2c`, `lsm6ds3`) that you can use for others usecase.

> IMPORTANT: the config directory must contain your generated public/private keys generated using openssl for RSA key pair or Elliptic Curve key pair and registered in you Google Cloud IoT during the last step -- cf. the chapter prerequisites.


## <a name='application-definition-file'>Application definition file</a>

Let’s take a look at our `mangohToGCloud.adef` file:

```cpp
// application version
version: 1.0.0

// maximum amount of RAM that can be consumed by the temporary (volatile) file system at runtime.
maxFileSystemBytes: 512K

// the app should start automatically at start-up
start: manual

// the app will be launched inside a sandbox.
sandboxed: true

executables:
{
  mangohToGCloud = ( myComponent )
}

processes:
{
  envVars:
  {
    LE_LOG_LEVEL = DEBUG
  }

  run:
  {
    ( mangohToGCloud )
  }

  // maximum size of core dump files.
  maxCoreDumpFileBytes: 512K

	// files are not allowed to grow bigger than this.
  maxFileBytes: 512K
}

bindings:
{
  // we want to use mqtt services (from an external app)
  mangohToGCloud.myComponent.mqtt -> mqttService.mqtt

  // we want to use the radio WAN app so that we can start an IP session over WAN (3G)
  mangohToGCloud.myComponent.le_data -> dataConnectionService.le_data

  // we want to use le_mdc because we need to know if the WAN interface is up
  mangohToGCloud.myComponent.le_mdc -> modemService.le_mdc
}

requires:
{
  device:
  {
    // allow read + write access to I2C port.
    [rw]    /dev/i2c-0   /dev/i2c-0
  }
}
```

The most important things are `bindings` and `requires` which allow us to connect
the application to others services (`MQTT`, Legato data + mdc) and to allow access to `I2C` bus.

## <a name='component'>Component</a>

The main directory is our `myComponent`

```cpp
// things the component needs from its runtime environment.
requires:
{
  // IPC APIs used by this component.
  api:
  {
    // \*.{h,c} files are created by building tools
    mqtt.api [manual-start]

    // allow WAN IP interface
    le_data.api [manual-start]

    // allow check on IP address and connection state
    modemServices/le_mdc.api
  }
}

sources:
{
  utils/i2c/i2c.cc
  utils/lsm6ds3/lsm6ds3.cc
  deviceToCloud.cc
  main.cc
}

// collections of information that can be exchanged with cloud services
assets:
{
  office =
  {
    variables:
    {
      float temperature
      float gyroscope_x
      float gyroscope_y
      float gyroscope_z
    }
  }
}

// command-line arguments to pass to the c++ compiler
cxxflags:
{
  -std=c++11
}
```

The most important things is the listing of `IPC` APIs (`MQTT`, `Data`, etc.)

## <a name='steps'>Steps</a>

We focus explanation to the main classes.<br />
Utils classes like `Lsm6ds3` or `I2c` are well commented and are simply wrappers
from common specifications.<br />
Don’t hesitate to take a look and to ask us any questions ;)

#### Main

Let’s explore our simple entry point:

```cpp
COMPONENT_INIT
{
  LE_INFO("Hi, from deviceToCloud app!");

  // instantiate our application as a singleton
  DeviceToCloud* s = DeviceToCloud::getInstance();

  s->start();
}
```

We decide to create a singleton for more flexibility and to handle callback stuff with ease.<br />
Easy right? :smiley:

#### DeviceToCloud class

- The first thing to do is managing termination/cleaning handlers.
```cpp
DeviceToCloud::DeviceToCloud() : _is_mqtt_connected(false)
{
  // blocks SIGTERM in the calling thread
  le_sig_Block(SIGTERM);

  // sets SIGTERM event handler for the calling thread.
  le_sig_SetEventHandler(SIGTERM, DeviceToCloud::appTerminationCallback);

  // register appCleanup as the method to be called on exit
  ::atexit(DeviceToCloud::appCleanUp);
}
```

- Generate Json Web Token for authentication:

```cpp
void SensorToGoogleCloud::generateJWT()
{
    // Reading the private key file
    std::ifstream ifs("config/rsa_private.pem");
    std::string key((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));


    // Validate private key with RS256 encryption
    RS256Validator signer(key.c_str());

    // About expiration time
    std::time_t iat = std::time(nullptr);

    std::tm expiration_date = *std::localtime(&iat);
    expiration_date.tm_year += 1;

    std::time_t exp = mktime(&expiration_date);

    // Creating the json payload that expires 1 year from today
    json_ptr json(json_pack("iat", iat, "exp", exp, "aud", GOOGLE_CLOUD_PROJECT_ID));

    // Let's encode the token to a char[]
    _jwt = JWT::Encode(&signer, json.get());

    LE_INFO("GENERATED TOKEN: %s", _jwt.get());
}
```

- Configure and connecting MQTT publisher is pretty simple

```cpp
void DeviceToCloud::configureMQTT()
{
  // connect to external services
  le_data_ConnectService();

  // connect to mqtt service
  mqtt_ConnectService();

  LE_INFO("MQTT CONNECTED!");

  // an experiment to see if we can close mqtt if has been left connected
  if(mqtt_GetConnectionState()) {
    DeviceToCloud::appCleanUp();
  }

  // lambda for mqtt session
  auto mqttSessionCallback = [](bool is_connected, int32_t connect_err_code, int32_t sub_err_code, void* ctxt_ptr) {
    if(is_connected) {
      DeviceToCloud::getInstance()->setMqttSession(true);
    }
    else {
      LE_INFO("MQTT DISCONNECTED - error:%d and sub erorr: %d", connect_err_code, sub_err_code);
    }
  };

  // setting up the mqtt service
  mqtt_AddSessionStateHandler(mqttSessionCallback, NULL);

  // configure mqtt session with AirVantage informations
  mqtt_Config(MQTT_BROKER_URL, MQTT_PORT_NUMBER, MQTT_KEEP_ALIVE, MQTT_QOS);
}
```

Thanks to the `mqtt.api` we can use all `MQTT` client functions.

> For configuration we are using values defined in our header, this is a sample
where the password is stored as a simple string... very bad ;p

- Time to check our main loop

```cpp
void DeviceToCloud::start()
{
  // calling lsm6ds3 configuration section
  _lsm6ds3.begin();

  LE_INFO("LSM6DS3 BEGIN!");

  // create timer with an interval of every 10 seconds, repeating forever (0)
  le_clk_Time_t repeat_interval;

  repeat_interval.sec  = 10;
  repeat_interval.usec = 0;

  // create our timer object
  _timer_ref = le_timer_Create("mainLoopTimer");

  // lambda for our loop acquisition
  auto callback = [](le_timer* timer) {
    DeviceToCloud* s = DeviceToCloud::getInstance();

    // data acquisition
    s->fetch();
    s->dump();

    // -->> ☁️
    s->send();
  };

  // about timer configuration
  le_timer_SetHandler(_timer_ref, callback);
  le_timer_SetInterval(_timer_ref, repeat_interval);
  le_timer_SetRepeat(_timer_ref, 0);
  le_timer_Start(_timer_ref);

  return;
}
```

When everything is initialized as `LSM6DS3` and `MQTT` configuration, we just
create a timer calling a simple loop to fetch, display and send acquired data.

- Send data to with MQTT

```cpp
void DeviceToCloud::send()
{
  // reference to the modem data connection profile (here: default profile definition)
  le_mdc_ProfileRef_t profile = le_mdc_GetProfile(LE_MDC_DEFAULT_PROFILE);

  // connection state value
  le_mdc_ConState_t mdc_state;

  char json_payload[2048];
  int32_t json_err_code;

  // get WAN connected info
  le_mdc_GetSessionState(profile, &mdc_state);

  // check that we have WAN
  if(mdc_state == LE_MDC_DISCONNECTED) {
    le_data_Request();
    LE_INFO("WAN CONNECTED" );
  }
  else if(!_is_mqtt_connected) {
    // check that mqttService is up
    mqtt_Connect(MQTT_BROKER_PASSWORD);
    LE_INFO("MQTT CONNECTED");
  }
  else {
    // format our data into json
    sprintf
      (json_payload,
       "{"
       "\"sensor.temperature\":%f,"
       "\"sensor.AX\":%f,"
       "\"sensor.AY\":%f,"
       "\"sensor.AZ\":%f"
       "}",
       _last_temperature,
       _last_gyroscope.Ax,
       _last_gyroscope.Ay,
       _last_gyroscope.Az);

    // send playload to AirVantage
    mqtt_SendJson(json_payload, &json_err_code);
  }
}
```

The purpose is to simply verify connection, format last acquired data and send a playload as a `MQTT` publisher.

#### Compilation + installation (command line)

Just call `make` with our specific target and install the app on the board:

```sh
❯ make wp85
mkapp -v -t wp85 \
-i /legato/interfaces/airVantage \
-i myComponent/utils/i2c \
-i myComponent/utils/lsm6ds3 \
mangohToGCloud.adef
Command-line arguments from previous run not found.
Parsing file: '/srcstmp/mangohToGCloud.adef'.
Modelling application: 'mangohToGCloud'
  defined in: '/srcstmp/mangohToGCloud.adef'
Application 'mangohToGCloud' contains executable 'mangohToGCloud'.
...
❯❯❯ ls -l *update
-rw-r--r--@ 1 majdi  staff  30914 Feb 16 16:48 mangohToGCloud.wp85.update
```

```sh
❯ app install mangohToGCloud.wp85.update 192.168.2.2
Applying update from file 'mangohToGCloud.wp85.update' to device at address '192.168.2.2'.
Unpacking package: 100% ++++++++++++++++++++++++++++++++++++++++++++++++++
Applying update: 100% ++++++++++++++++++++++++++++++++++++++++++++++++++
SUCCESS
Done
```

#### Check

We are going to run our applications.

```sh
❯ ssh root@192.168.2.2
root@swi-mdm9x15:~# app status
[running] audioService
...
[running] mqttService
[stopped] mangohToGCloud
root@swi-mdm9x15:~# app start mangohToGCloud
...
Jul 25 17:06:37 swi-mdm9x15 user.info Legato:  INFO | mangohToGCloud[8312]/myComponent T=main | main.cc _myComponent_COMPONENT_INIT() 24 | Hi, from deviceToCloud app!
...
Jul 25 17:06:37 swi-mdm9x15 user.info Legato:  INFO | mangohToGCloud[8312]/myComponent T=main | deviceToCloud.cc configureMQTT() 52 | AV CONNECTED
Jul 25 17:06:37 swi-mdm9x15 user.info Legato:  INFO | mangohToGCloud[8312]/myComponent T=main | deviceToCloud.cc configureMQTT() 56 | MQTT CONNECTED!
Jul 25 17:06:37 swi-mdm9x15 user.debug Legato:  DBUG | mqttService[6918]/framework T=main | mqtt_server.c Handle_mqtt_AddSessionStateHandler() 713 | Sending response to client session 0x27cac : 4 bytes sent
Jul 25 17:06:37 swi-mdm9x15 user.debug Legato:  DBUG | mangohToGCloud[8312]/framework T=main | mqtt_client.c mqtt_Config() 561 | Sending message to server and waiting for response : 33 bytes sent
Jul 25 17:06:37 swi-mdm9x15 user.info Legato:  INFO | mqttService[6918]/mqttServiceComponent T=main | mqttService.c mqtt_Config() 142 | MQTT Broker URL('eu.airvantage.net' -> 'eu.airvantage.net')
Jul 25 17:06:37 swi-mdm9x15 user.info Legato:  INFO | mqttService[6918]/mqttServiceComponent T=main | mqttService.c mqtt_Config() 148 | MQTT Broker Port(1883 -> 1883)
Jul 25 17:06:37 swi-mdm9x15 user.info Legato:  INFO | mqttService[6918]/mqttServiceComponent T=main | mqttService.c mqtt_Config() 154 | Keep Alive(60 -> 60 seconds)
Jul 25 17:06:37 swi-mdm9x15 user.info Legato:  INFO | mqttService[6918]/mqttServiceComponent T=main | mqttService.c mqtt_Config() 160 | QoS(0 -> 0)
```

#### Log visualization

After building and installing our new application, we can see on the logread terminal that we have some outputs:

```sh
root@swi-mdm9x15:~# logread -f
Jul 25 17:07:57 swi-mdm9x15 user.info Legato:  INFO | mangohToGCloud[8312]/myComponent T=main | deviceToCloud.cc dump() 143 | DUMP: 24.875000 0.017080 -0.025376 -0.990640
Jul 25 17:07:57 swi-mdm9x15 user.debug Legato:  DBUG | mangohToGCloud[8312]/framework T=main | le_mdc_client.c le_mdc_GetProfile() 950 | Sending message to server and waiting for response : 4 bytes sent
Jul 25 17:07:57 swi-mdm9x15 user.debug Legato:  DBUG | mangohToGCloud[8312]/framework T=main | le_mdc_client.c le_mdc_GetSessionState() 1542 | Sending message to server and waiting for response : 4 bytes sent
Jul 25 17:07:57 swi-mdm9x15 user.debug Legato:  DBUG | mangohToGCloud[8312]/framework T=main | mqtt_client.c mqtt_SendJson() 786 | Sending message to server and waiting for response : 101 bytes sent
Jul 25 17:07:57 swi-mdm9x15 user.info Legato:  INFO | mqttService[6918]/mqttServiceComponent T=main | mqttService.c mqtt_SendJson() 198 | send json({"sensor.temperature":24.875000,"sensor.ax":0.017080,"sensor.ay":-0.025376,"sensor.az":-0.990640})
Jul 25 17:07:57 swi-mdm9x15 user.info Legato:  INFO | mqttService[6918]/mqttServiceComponent T=main | mqttService.c mqttService_SendMessageJson() 83 | topic('359377060005641/messages/json') payload('{"sensor.temperature":24.875000,"sensor.ax":0.017080,"sensor.ay":-0.025376,"se
```

#### Result in Google Cloud console

in progress

### Next

Well, that's it. Easy right? 😎<br />
Take a look at our other samples using IoT cards: bluetooth, WiFi, canBUS in our repository.

### Credits

Develop with :heart: by [**Majdi Toumi**](http://majditoumi.com)|[**Mhirba**](http://mhirba.com) in **Paris** for [**Sierra Wireless**](https://www.sierrawireless.com/).
