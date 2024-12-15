#ifndef TASK_UPDATE_WIFI_SIGNAL
#define TASK_UPDATE_WIFI_SIGNAL

#include "header.h"
#include "WiFi.h"


/**
 * TASK: Get the current WiFi signal strength and write it to the
 * displayValues so it can be shown by the updateDisplay task
 */
void updateWiFiSignalStrength(void * parameter){
  for(;;){
    if(WiFi.isConnected()){
      Console0.printf("[WIFI] Updating signal strength...\n");
    }

    // Sleep for 10 seconds
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

#endif