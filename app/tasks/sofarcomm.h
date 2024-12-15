#ifndef TASK_SOFARCOMM_1
#define TASK_SOFARCOMM_1

#include "header.h"
#include <ModbusSlave.h>
#include "classes/SofarComm-rtos.h"


static SofarComm SofarInv1;
static SofarComm SofarInv2;
EspSoftwareSerial::UART SofarSerial;

bool SofarSerialClaimed = false;
QueueHandle_t batteryCloudDataQueue;


void SofarNettoDeliveryCallback(void * parameter){
  //Console0.printf("%d [ENERGY] Run Init\n",millis());
    for(;;){
      long start = millis();
      //Console0.printf("%d [ENERGY] Run\n",millis());
     /* if (meterdata_obj.lastvalue + 10000 < millis()){
        Console0.printf("%d [ENERGY] No recent meterdata\n",millis());
      
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        continue;
      };*/
      
      //value set is not older then 1 hour
      if (power_setpoint.timestamp + 3600000 < millis()){
        Console0.printf("%d [ENERGY] Set value older then 1h\n",millis());
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        continue;
      };
      
      int num_inv = 1;
      
      if (power_setpoint.setpoint == power_setpoint.last_setpoint){
        //Console0.printf("%d [ENERGY] new setpoint is same as old one %dW\n",millis(),power_setpoint.setpoint);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        continue;
      }

      int req = power_setpoint.setpoint;//(power_setpoint.setpoint - (int)meterdata_obj.pt);
      int diff = (req - power_setpoint.last_setpoint); 
      //if (diff > 10) {
        Console0.printf("%d [ENERGY] set INV delivery to %dW for setpoint %dW (diff %dW)\n",millis(),(req*-1),power_setpoint.setpoint,diff);
        //SofarInv1.setBatteryPower(req/ num_inv);
        power_setpoint.last_setpoint = req;
        long end = millis();
      //} else {
      //  Console0.printf("%d [ENERGY] postphone INV delivery change to %dW for setpoint %dW (diff <= 10 (%dW))\n",millis(),(req*-1),power_setpoint.setpoint,diff);
      //}
      

      // Schedule the task to run again in 1 second (while
      // taking into account how long measurement took)
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }    
}


void SofarBatterySetpointCallback(void * parameter){
  //Console0.printf("%d [ENERGY] Run Init\n",millis());
    for(;;){
      long start = millis();
      //Console0.printf("%d [ENERGY] Run\n",millis());
     /* if (meterdata_obj.lastvalue + 10000 < millis()){
        Console0.printf("%d [ENERGY] No recent meterdata\n",millis());
      
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        continue;
      };*/
      
      //value set is not older then 1 hour
      if (battery_setpoint.timestamp + 3600000 < millis()){
        Console0.printf("%d [ENERGY] Set value older then 1h\n",millis());
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        continue;
      };
      
      int num_inv = 1;
      
      if (battery_setpoint.setpoint == power_setpoint.last_setpoint){
        //Console0.printf("%d [ENERGY] new setpoint is same as old one %dW\n",millis(),power_setpoint.setpoint);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        continue;
      }

      int req = battery_setpoint.setpoint;//(power_setpoint.setpoint - (int)meterdata_obj.pt);
      int diff = (req - battery_setpoint.last_setpoint); 
      if (battery_setpoint.setpoint != battery_setpoint.last_setpoint) {
        Console0.printf("%d [ENERGY] set INV batterycharge to %dW for setpoint %dW (diff %dW)\n",millis(),(req*-1),battery_setpoint.setpoint,diff);
        //SofarInv1.setBatteryPower(req/ num_inv);
        battery_setpoint.last_setpoint = req;
        long end = millis();
      }
      

      // Schedule the task to run again in 1 second (while
      // taking into account how long measurement took)
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }    
}


void setup_SofarComm() {

    
     // Create queues
    batteryCloudDataQueue = xQueueCreate(1, sizeof(struct batteryclouddata));
    // Check if queues were created successfully
    if (batteryCloudDataQueue == NULL ) {
        Console0.print("Failed to create batteryCloudDataQueue!\n");
        return;
    }

    
    InverterDataQueue = xQueueCreate ( 10, sizeof(struct Inverter ) );
    
    if( InverterDataQueue == NULL )
    {
        Console0.print("InverterDataQueue Creation Failure!\n");
    }
    //AutoConfigQueue = xQueueCreate( 20, sizeof(struct MQTT_HA_ConfigMessage ) );
    //AutoConfigQueue = xQueueCreate( 20, 500 );
    
    Console0.print("AutoConfigQueue Size: ");
    Console0.print(sizeof(struct MQTT_HA_ConfigMessage ));
    Console0.print("\n");

    SofarSerial.begin(115200,EspSoftwareSerial::SWSERIAL_8N1,16,17);
    SofarSerial.enableIntTx(false);

/*
    SofarInv1.configure(11,"INV1",1,SofarSerial,SofarSerialClaimed,InverterDataQueue);
    SofarInv1.start(1);
    
    xTaskCreate(
      sendInverterData2HA,           // Task function
      "Sent Inverterdata1 2 HA",               // Task name
      2000,                  // Stack size (bytes)
      NULL,                   // Task parameter
      2,                    // Task priority
      NULL                  // Task handle
    );
*/

    SofarInv1.configure(12,"INV1",1,SofarSerial,SofarSerialClaimed,InverterDataQueue,timeClient,inverterData,batteryCloudDataQueue);
    SofarInv1.AttachConsole(Serial);
    //SofarInv1.enableHAAutoDiscovery(AutoConfigQueue);
    /*if( AutoConfigQueue == NULL )
    {
        Console0.print("AutoConfigQueue Creation Failure!\n");
    }*/
    SofarInv1.start(1);
    
    
    power_setpoint.setpoint = 0;
  power_setpoint.timestamp = millis();
 
/*
    xTaskCreate(
      SofarNettoDeliveryCallback,           // Task function
      "netto Delivery calculation",               // Task name
      2000,                  // Stack size (bytes)
      NULL,                   // Task parameter
      1,                    // Task priority
      NULL                  // Task handle
    );

     xTaskCreate(
      SofarBatterySetpointCallback,           // Task function
      "netto Delivery calculation",               // Task name
      2000,                  // Stack size (bytes)
      NULL,                   // Task parameter
      1,                    // Task priority
      NULL                  // Task handle
    );

*/

}

void setInverterDelivery(float &payload) {
    // Convert the float payload to an integer (rounding it)
    power_setpoint.setpoint = static_cast<int>(payload);  // Or use round(payload), floor(payload), or ceil(payload) if needed
    power_setpoint.timestamp = millis();
    Console0.printf("%d [ENERGY] set optimization to setpoint %dW\n", millis(), power_setpoint.setpoint);
}

void setInverterBatterySetpoint(float &payload) {
    // Convert the float payload to an integer (rounding it)
    battery_setpoint.setpoint = static_cast<int>(payload);  // Or use round(payload), floor(payload), or ceil(payload) if needed
    battery_setpoint.timestamp = millis();
    Console0.printf("%d [ENERGY] Battery setpoint changed to %dW\n", millis(), battery_setpoint.setpoint);
}




#endif
