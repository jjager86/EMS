#ifndef HEADER_H_
#define HEADER_H_

//#include <ESPmDNS.h>
//#include <PubSubClient.h>
//#include <ArduinoOTA.h>

//#include <SoftwareConsole0.h>
//#include <ModbusRTUSlave.h>

//#include <ModbusMaster.h>
//#include "mqttConsole0.h"
//#include <HTTPClient.h>
//#include <ArduinoJson.h>

//#include <M5StickCPlus.h>
//#include "I2C_AXP192.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <SoftwareSerial.h>
#include <WiFi.h>

//#include <freertos/queue.h>
//#include <freertos/task.h>
#include <config/enums.h>
#include <config/queueDefinitions.h>
#include "config/config.h"

#define Console0 Serial

static union FLOAT2INT float2int;
static meterdata meterdata_obj;
static pwrsetpoint power_setpoint ;
static pwrsetpoint battery_setpoint ; 
static batteryclouddata batteryCloudData;

static bool wifi_laststatus;

#endif