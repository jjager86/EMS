#ifndef TASK_HOME_ASSISTANT
#define TASK_HOME_ASSISTANT

#if HA_ENABLED == true

    #include "header.h"
    #include <WiFiClient.h>
    #include <MQTTClient.h>
    #include <classes/SofarComm-rtos.h>

    WiFiClient HA_net;
    MQTTClient HA_mqtt(1024);
   
    /*const char* PROGMEM HA_discovery_msg = "{"
            "\"name\":\"" DEVICE_NAME "EnergyMeter\","
            "\"device_class\":\"power\","
            "\"unit_of_measurement\":\"W\","
            "\"stat_t\": \"~/state\","
            "\"value_template\":\"{{ value_json.power}}\","
            "\"device\": {"
                "\"name\":\"" DEVICE_NAME "\","
                "\"sw_version\":\"0.1\","
                "\"model\":\"V0.1\","
                "\"manufacturer\":\"Johan Jager\","
                "\"identifiers\":[\"" DEVICE_NAME "\"]"
            "}"
        "}";
*/




 void HA_mqtt_callback(String &topic, String &payload) {
        Console0.print("Message arrived on topic: ");
        Console0.print(topic);
        Console0.print(". Message: ");
        Console0.print(payload);
        Console0.println();
        
       // if(topic.startsWith(DEVICE_NAME "/Inverter/INV1/control")){
       //     setInverterDelivery(payload.toFloat());
       // }
    /*
        if(topic.startsWith("dsmr/reading/electricity_currently_delivered")){
            if (payload.toInt() > 0){
                meterdata Meterdata;
                Meterdata.lastvalue = millis();
                Meterdata.pt = (payload.toFloat() * 1000);
                meterdata_obj = Meterdata;
                Console0.print("Write positive Metervalue from MQTT to meterdata_obj\n");
            }
        }

        if(topic.startsWith("dsmr/reading/electricity_currently_returned")){
            if (payload.toInt() > 0){
                meterdata Meterdata;
                Meterdata.lastvalue = millis();
                Meterdata.pt = (payload.toFloat() * -1000);
                meterdata_obj = Meterdata;
                Console0.print("Write negative Metervalue from MQTT to meterdata_obj\n");
                
            }
        }
*/
        

    }

    /**
     * Established a connection to Home Assistant MQTT broker.
     * 
     * This task should run continously. It will check if an
     * MQTT connection is active and if so, will sleep for 1
     * minute. If not, a new connection will be established.
     */
    void keepHAConnectionAlive(void * parameter){
        for(;;){
            // When we are connected, loop the MQTT client and sleep for 0,5s
            if(!WiFi.isConnected()){
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue;
            }

            if(HA_mqtt.connected()){
                HA_mqtt.loop();
                vTaskDelay(250 / portTICK_PERIOD_MS);
                continue;
            }

            if(WiFi.status() == WL_CONNECTED){
                Console0.printf("%d [MQTT] Connecting to HA\n",millis());
                HA_mqtt.begin(HA_ADDRESS, HA_PORT, HA_net);
                HA_mqtt.onMessage(HA_mqtt_callback);

                long startAttemptTime = millis();
            
                while (!HA_mqtt.connect(DEVICE_NAME, HA_USER, HA_PASSWORD) && millis() - startAttemptTime < MQTT_CONNECT_TIMEOUT)
                {
                    vTaskDelay(MQTT_CONNECT_DELAY / portTICK_PERIOD_MS);
                }

                if(!HA_mqtt.connected()){
                    Console0.printf("%d [MQTT] HA connection failed. Waiting 30s..\n",millis());
                    vTaskDelay(30000 / portTICK_PERIOD_MS);
                }

                Console0.printf("%d [MQTT] HA Connected!\n",millis());
                      // Subscribe
                //HA_mqtt.publish("homeassistant/switch/" DEVICE_NAME "/Inverter/powerdelivery", 0);
                HA_mqtt.subscribe(DEVICE_NAME "/Inverter/INV1/control/power");
                //HA_mqtt.subscribe("dsmr/reading/electricity_currently_delivered");
                //HA_mqtt.subscribe("dsmr/reading/electricity_currently_returned");
                
            }
        }
    }

    /**
     * TASK: Every 15 minutes we send Home Assistant a discovery message
     *       so that the energy monitor shows up in the device registry.
     */
    void HADiscovery(void * parameter){
        for(;;){
            if(!HA_mqtt.connected()){
                vTaskDelay(30 * 1000 / portTICK_PERIOD_MS);
                continue;
            }
            if( AutoConfigQueue123 != NULL ){
                int msg;
                //MQTT_HA_ConfigMessage msg;
                 if (xQueueReceive(AutoConfigQueue123, &msg, 4) == pdPASS ) {
            /*     String message;   
                   
                  message = "{"
						"\"~\":\"" DEVICE_NAME "/Inverter/" + msg.obj_name + "\","
						"\"name\":\"" DEVICE_NAME "-" + msg.mqtt_name + "\","
						"\"unique_id\":\"" + msg.mqtt_unique_id +"\",";

			if (msg.mqtt_class != ""){
				message +="\"device_class\":\"" + msg.mqtt_class + "\","
				"\"unit_of_measurement\":\"" + msg.mqtt_unit + "\",";
			}
			
			message +="\"value_template\":\"{{ value_json." + msg.mqtt_name + "}}\","
						"\"stat_t\": \"~/state\","
						"\"json_attr_t\": \"~/json\","
						"\"device\": {"
							"\"name\":\"" DEVICE_NAME "\","
							//"\"sw_version\":\"0.3\","
							//"\"model\":\"V0.3\","
							//"\"manufacturer\":\"Johan Jager\","
							"\"identifiers\":[\"" DEVICE_NAME "\"]"
						"}"
					"}";
 */

			//msg.topicprefix = DEVICE_NAME "_Inverter_" + _this->_I.name + "_" + mqtt_status_reads[l].mqtt_name;
			//msg.obj_name=
            //msg.mqtt_name=mqtt_status_reads[l].mqtt_name;
			//msg.mqtt_class=mqtt_status_reads[l].mqtt_class;
			//msg.mqtt_unique_id=DEVICE_NAME "_Inverter_" + _this->_I.name;
                   Console0.printf("%d [MQTT] HA sending auto discovery %d\n",millis(),msg);
               //     HA_mqtt.publish("homeassistant/sensor/" + msg.mqtt_unique_id + "_" + msg.mqtt_name + "/config", message );
                    continue;
                 } else {
                    vTaskDelay(1 * 10 * 1000 / portTICK_PERIOD_MS); //sleep for 60s
                 }
                    
            } else {
                vTaskDelay(500 / portTICK_PERIOD_MS); //sleep for 500ms
            }
        }
    }
   

    void sendInverterData2HA(void * parameter){
        Inverter _I;
        for(;;){
            if(!HA_mqtt.connected()){
                vTaskDelay(30 * 1000 / portTICK_PERIOD_MS);
                continue;
            }
             
            if( InverterDataQueue != NULL ) {
               
                if (xQueueReceive(InverterDataQueue, &_I, 4) == pdPASS ) {
                    if (_I.jsonbuffer.length() > 0){
                        if(!HA_mqtt.connected()){
                        Console0.printf("%d [MQTT_INV] Can't send to HA without MQTT. Abort.\n",millis());
                        vTaskDelete(NULL);
                        }
                        Console0.printf("%d [MQTT_INV] JSONBUFFER of %s: %s\n",millis(),_I.name,_I.jsonbuffer);
                        char msg[30];
                        //strcat(msg, String(_I.jsonbuffer).c_str());
                        
                        Console0.printf("%d [MQTT_INV] HA publish: \n",millis());
                        //Console0.printf(msg);

                        HA_mqtt.publish(DEVICE_NAME "/Inverter/" + _I.name + "/json", _I.jsonbuffer);
                        HA_mqtt.publish(DEVICE_NAME "/Inverter/" + _I.name + "/state", (String)_I.RunningState);
                    }
                    

                }
            }
            
            // Task is done!
            //vTaskDelete(NULL);/*/
            
                                    
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }


    void sendMeterData2HA(void * parameter){
        Console0.printf("%d [MQTT_INV] HA meterdata Task started\n",millis());
        //extern meterdata meterdata_obj;
        meterdata md;
        for(;;){
            if( MeterDataQueue != NULL ) {
if(!HA_mqtt.connected()){
                        Console0.printf("%d [MQTT_INV] Can't send to HA without MQTT. Abort.\n",millis());
                        
                        vTaskDelay(3000 / portTICK_PERIOD_MS);
                        continue;
                    }

                
                if (xQueueReceive(MeterDataQueue, &md, 4) == pdPASS ) {
                
                    
                    //String msg;
                    char msg[100];
                    sprintf(msg, "{\"power_total\":%10.1f,\"power_pha\":%10.1f,\"power_phb\":%10.1f,\"power_phc\":%10.1f}", md.pt,md.pa,md.pb,md.pc);

                    //Console0.printf("%d [MQTT_INV] HA publish: %s\n",millis(),msg);
                    HA_mqtt.publish(DEVICE_NAME "/EnergyMeter/state", msg);

                } /*else {
                    vTaskDelay(3000 / portTICK_PERIOD_MS);
                    //queue is empty
                    //Console0.printf("Error while read from MeterDataQueue\n");
                }*/
                
                
                // Task is done!
                //vTaskDelete(NULL);
                
            }
            
            vTaskDelay(3000 / portTICK_PERIOD_MS);
        }
    }

    

   

#endif
#endif
