#ifndef TASK_FETCH_TIME_NTP
#define TASK_FETCH_TIME_NTP

#if NTP_TIME_SYNC_ENABLED == true
    #include "header.h"
    #include <WiFi.h>
    #include <NTPClient.h>
    #include <WiFiUdp.h>
 
    extern void reconnectWifiIfNeeded();
   
    WiFiUDP ntpUDP;

    // TODO: this does not take timezones into account! Only UTC for now.
    NTPClient timeClient(ntpUDP, NTP_SERVER, NTP_OFFSET_SECONDS, NTP_UPDATE_INTERVAL_MS);

    void fetchTimeFromNTP(void * parameter){
        for(;;){
            if(!WiFi.isConnected()){
                vTaskDelay(10*1000 / portTICK_PERIOD_MS);
                continue;
            }

            Console0.printf("%d [NTP] Fetch time\n",millis());
            timeClient.update();

            String timestring = timeClient.getFormattedTime();
            short tIndex = timestring.indexOf("T");
            
            Console0.printf("%d [NTP] Set time to %s\n",millis(),timestring);

            // Sleep for a minute before checking again
            vTaskDelay(NTP_UPDATE_INTERVAL_MS / portTICK_PERIOD_MS);
        }
    }
#endif
#endif
