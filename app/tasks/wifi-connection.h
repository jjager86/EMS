#ifndef TASK_WIFI_CONNECTION
#define TASK_WIFI_CONNECTION

#include "header.h"
#include "WiFi.h"

//extern void goToDeepSleep();
extern bool wifi_laststatus;
/**
 * Task: monitor the WiFi connection and keep it alive!
 * 
 * When a WiFi connection is established, this task will check it every 10 seconds 
 * to make sure it's still alive.
 * 
 * If not, a reconnect is attempted. If this fails to finish within the timeout,
 * the ESP32 is send to deep sleep in an attempt to recover from this.
 */
void keepWiFiAlive(void * parameter){
    unsigned long startAttemptTime = millis();
    for(;;){
        if(WiFi.status() == WL_CONNECTED){
            if (wifi_laststatus == false){
                Console0.printf("%d [WIFI] Connected\n",millis());
                wifi_laststatus = true;
                //TelnetStream.begin(23);
            }
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            
            continue;
        }

        if (wifi_laststatus == true){
            Console0.printf("%d [WIFI] Disconnected, reconnect\n",millis());
            wifi_laststatus = false;
        } else 
        
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(DEVICE_NAME);
        WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

        
        // Keep looping while we're not connected and haven't reached the timeout
        while (WiFi.status() != WL_CONNECTED && (millis() - startAttemptTime < WIFI_TIMEOUT)){
            Console0.printf("%d [WIFI] Connection attempt\n",millis());
            vTaskDelay(500 / portTICK_PERIOD_MS);
            continue;
        }
         

        // Make sure that we're actually connected, otherwise go to deep sleep
        if(WiFi.status() != WL_CONNECTED){
            Console0.printf("%d [WIFI] Connection timeout backoff %ds",millis(),(WIFI_RECOVER_TIME_MS/1000));
            vTaskDelay(WIFI_RECOVER_TIME_MS / portTICK_PERIOD_MS);
        }
    }
}

#endif
