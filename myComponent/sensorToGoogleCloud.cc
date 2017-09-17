// sensorToGoogleCloud.cc ---
//
// Author: Majdi Toumi
// Created: Sat Feb 11 17:48:26 2017 (+0100)
// Last-Updated: Wed Feb 15 19:14:10 2017 (+0100)
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

#include <functional>
#include <fstream>
#include <string>
#include <ctime>
// #include "jwt/jwt_all.h"

#include "legato.h"
#include "interfaces.h"
#include "sensorToGoogleCloud.hh"

// Null, because instance will be initialized on demand.
SensorToGoogleCloud* SensorToGoogleCloud::_instance = NULL;

SensorToGoogleCloud* SensorToGoogleCloud::getInstance()
{
  if (SensorToGoogleCloud::_instance == NULL) {
    SensorToGoogleCloud::_instance = new SensorToGoogleCloud();
  }
  // just return the single class instance
  return SensorToGoogleCloud::_instance;
}

SensorToGoogleCloud::SensorToGoogleCloud() : _is_mqtt_connected(false)
{
    // blocks SIGTERM in the calling thread
    le_sig_Block(SIGTERM);

    // sets SIGTERM event handler for the calling thread.
    le_sig_SetEventHandler(SIGTERM, SensorToGoogleCloud::appTerminationCallback);

    // register appCleanup as the method to be called on exit
    ::atexit(SensorToGoogleCloud::appCleanUp);
}

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
    this->_jwt = JWT::Encode(&signer, json.get());

    std::cout << "GENERATED TOKEN:" << this->_jwt.get() << std::endl;
}

void SensorToGoogleCloud::configureMQTT()
{
    // connect to external services
    le_data_ConnectService();

    // connect to mqtt service
    mqtt_ConnectService();

    LE_INFO("MQTT CONNECTED!");

    // an experiment to see if we can close mqtt if has been left connected
    if(mqtt_GetConnectionState()) {
        SensorToGoogleCloud::appCleanUp();
    }

    // lambda for mqtt session
    auto mqttSessionCallback = [](bool is_connected, int32_t connect_err_code, int32_t sub_err_code, void* ctxt_ptr) {
        if(is_connected) {
            SensorToGoogleCloud::getInstance()->setMqttSession(true);
        }
        else {
            LE_INFO("MQTT disconnected - error:%d and sub erorr: %d", connect_err_code, sub_err_code);
        }
    };

    // setting up the mqtt service
    mqtt_AddSessionStateHandler(mqttSessionCallback, NULL);

    // configure mqtt session with AirVantage informations
    mqtt_Config(MQTT_BRIDGE_HOSTNAME, MQTT_BRIDGE_PORT, MQTT_KEEP_ALIVE, MQTT_QOS);
}

void SensorToGoogleCloud::start()
{
    // calling lsm6ds3 configuration section
    _lsm6ds3.begin();

    // JWT generation
    this->generateJWT();

    // MQTT configuration
    this->configureMQTT();

    // create timer with an interval of every 10 seconds, repeating forever (0)
    le_clk_Time_t repeat_interval;

    repeat_interval.sec  = 10;
    repeat_interval.usec = 0;

    // create our timer object
    _timer_ref = le_timer_Create("mainLoopTimer");

    // lambda for our loop acquisition
    auto callback = [](le_timer* timer) {
        SensorToGoogleCloud* s = SensorToGoogleCloud::getInstance();

        // data acquisition
        s->fetch();
        s->dump();

        // -->> ☁
        s->send();
    };

    // about timer configuration
    le_timer_SetHandler(_timer_ref, callback);
    le_timer_SetInterval(_timer_ref, repeat_interval);
    le_timer_SetRepeat(_timer_ref, 0);
    le_timer_Start(_timer_ref);

    return;
}

void SensorToGoogleCloud::setMqttSession(bool v)
{
  _is_mqtt_connected = v;
}

void SensorToGoogleCloud::fetch()
{
    // get last temperature from LSM6DS3
    _last_temperature = _lsm6ds3.readTemperatureByUnity('C');

    // same for X->Z gyroscope axis
    _last_gyroscope.Ax = _lsm6ds3.readAccelerationByAxis('X');
    _last_gyroscope.Ay = _lsm6ds3.readAccelerationByAxis('Y');
    _last_gyroscope.Az = _lsm6ds3.readAccelerationByAxis('Z');
}

void SensorToGoogleCloud::dump()
{
  LE_INFO
    ("DUMP: %f %f %f %f",
     this->_last_temperature,
     this->_last_gyroscope.Ax,
     this->_last_gyroscope.Ay,
     this->_last_gyroscope.Az);
}

void SensorToGoogleCloud::send()
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
        LE_INFO("Requesting WAN connect" );
        le_data_Request();
    }
    else if(!_is_mqtt_connected) {
        // check that mqttService is up
        LE_INFO("Requesting MQTT connect");
        mqtt_Connect(this->_jwt.get());
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

void SensorToGoogleCloud::appTerminationCallback(int sig)
{
  SensorToGoogleCloud::appCleanUp();
}

void SensorToGoogleCloud::appCleanUp()
{
  LE_INFO("See ya!");
}
