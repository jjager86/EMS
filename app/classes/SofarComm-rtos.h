#ifndef SOFARCOMM_H_
#define SOFARCOMM_H_

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include "config/enums.h"


#include <SoftwareSerial.h>
#include "ModbusMaster.h"
#include "PubSubClient.h"

#include "SofarComm-def.h"

   #include <WiFi.h>
    #include <NTPClient.h>
    #include <WiFiUdp.h>



class SofarComm {
//vars

public:
  TaskHandle_t RunStateTask;
  TaskHandle_t BatterySaveTask;
 

private:
    char _server;
    char _username;
    char _password;
    char _topicname;
    //Invmeterdata _invmeterdata;
	  Inverter _I;
    bool *_SerialClaimed;
    PubSubClient _PubSubClient;
    ModbusMaster _Modbus;
    EspSoftwareSerial::UART *_Serial;
    QueueHandle_t *_InverterDataQueue;
    QueueHandle_t _AutoConfigQueue;
    TaskHandle_t generateHAAutoDiscoveryHandle;
    HardwareSerial *Console;
    int lastPowerToBattery;
    QueueHandle_t *_batteryCloudDataQueue;
    inverterdata *_inverterData; 
    NTPClient *_timeClient;



//functions

public:
	SofarComm(void);
  void configure(uint8_t modbusId,String name,int Phase,EspSoftwareSerial::UART &Serial,bool &SerialClaimed, QueueHandle_t &InverterDataQueue,NTPClient &timeClient,inverterdata &inverterData,QueueHandle_t &batteryCloudDataQueue);

  void start(int core);
  void setMQTTdata(PubSubClient &PubSubClient,char &server,char &username,char &password,char &topicname);
  void enableHAAutoDiscovery(QueueHandle_t AutoConfigQueue);
  void AttachConsole(HardwareSerial &console);
  void getObj();
  float getCurrentSOC();

  void setMode(String cmd);

  void setFieldValue(const std::string& mqttname, float val);
  bool isDataChanged(const batteryclouddata& data, const batteryclouddata& origdata);

  //void setDelivery(int wattage);
  //void setBatteryPower(int wattage);
  
   
private:
     
    static void PostInverterObjtoQueue(void* pvParameters);
    
    static void setBatteryManagement(void* pvParameters);
    void UpdateInvStatus(String status, int conn_status);
    bool checkTimer(unsigned long *lastRun, unsigned long interval);
    void calcCRC(uint8_t frame[], byte frameSize);
    bool checkCRC(uint8_t frame[], byte frameSize);
    void sendMqtt(char* topic, String msg_str);
    uint8_t ModbusWriteMemory(uint8_t fc, uint16_t address, uint16_t length);
    int addStateInfo(String &state, uint16_t reg, float multiplier, const char* Unit, const char* mqttname, ValueType value_type);
    static void generateHAAutoDisoveryTask(void* pvParameters);
    static void sendDataCallback(void* pvParameters);
    static void batterySaveCallback(void* pvParameters);
    static void RunStateCallback(void* pvParameters);
    static void HeartbeatCallback(void* pvParameters);
    unsigned int batteryWatts() ;
    //int listen(modbusResponse *resp);
    //int sendModbus(uint8_t frame[], byte frameSize, modbusResponse *resp);
    int sendPassiveCmd(uint8_t id, uint16_t cmd, uint16_t param);
    void mqttCallback(String topic, byte *message, unsigned int length);
    void mqttReconnect();
    //int readSingleReg(uint8_t id, uint16_t reg, modbusResponse *rs);
    void loopSofar();
};
#endif