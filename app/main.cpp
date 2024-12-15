#include "header.h"


void setInverterDelivery(float &payload);
float currentSOC = 10; 

#include "tasks/fetch-time-from-ntp.h"
#include "tasks/wifi-connection.h"

//#include "tasks/wifi-update-signalstrength.h"
#include "tasks/sofarcomm.h"
#include "tasks/api.h"
#include "tasks/mqtt-home-assistant.h"
//#include "tasks/dtsu666_em_client.h"

//#include "tasks/virtualDSSU666.h"
#include "tasks/display.h"

//I2C_AXP192 axp192(I2C_AXP192_DEFAULT_ADDRESS, Wire1);

void setup() {
    #if DEBUG == true
      Console0.begin(115200);
    #endif 
  
    
    /*Wire1.begin(21, 22);

  I2C_AXP192_InitDef initDef = {
    .EXTEN  = true,
    .BACKUP = true,
    .DCDC1  = 3300,
    .DCDC2  = 0,
    .DCDC3  = 0,
    .LDO2   = 3000,
    .LDO3   = 3000,
    .GPIO0  = 2800,
    .GPIO1  = -1,
    .GPIO2  = -1,
    .GPIO3  = -1,
    .GPIO4  = -1,
  };
  axp192.begin(initDef);*/

 
 
  // ----------------------------------------------------------------
    // TASK: Connect to WiFi & keep the connection alive.
    // ----------------------------------------------------------------
    xTaskCreatePinnedToCore(
      keepWiFiAlive,
      "keepWiFiAlive",  // Task name
      5000,            // Stack size (bytes)
      NULL,             // Parameter
      1,                // Task priority
      NULL,             // Task handle
      ARDUINO_RUNNING_CORE
    );

   
    // ----------------------------------------------------------------
    // TASK: update time from NTP server.
    // ----------------------------------------------------------------
    #if NTP_TIME_SYNC_ENABLED == true
      xTaskCreate(
        fetchTimeFromNTP,
        "Update NTP time",
        8192,            // Stack size (bytes)
        NULL,             // Parameter
        5,                // Task priority
        NULL              // Task handle
      );
    #endif

    // ----------------------------------------------------------------
    // TASK: update WiFi signal strength
    // ----------------------------------------------------------------
   /* xTaskCreate(
      updateWiFiSignalStrength,
      "Update WiFi strength",
      1000,             // Stack size (bytes)
      NULL,             // Parameter
      2,                // Task priority
      NULL              // Task handle
    );
*/


    #if HA_ENABLED == true
      xTaskCreate(
        HADiscovery,
        "MQTT-HA Discovery",  // Task name
        5000,                // Stack size (bytes)
        NULL,                 // Parameter
        5,                    // Task priority
        NULL                  // Task handle
      );

      xTaskCreate(
        keepHAConnectionAlive,
        "MQTT-HA Connect",
        5000,
        NULL,
        4,
        NULL
      );
      
      xTaskCreate(
        sendInverterData2HA,
        "Send Inverterdata to MQTT",
        5000,
        NULL,
        4,
        NULL
      );

      
    #endif



  setupAPIConfig();
  setup_SofarComm();
  //setup_DTSU666_em_client();
  //setup_DSSU666();
   displaySetup();
 

  Console0.printf("%d [MAIN] All tasks are initialized\n",millis());
}

void loop() { } 