#ifndef TASK_API
#define TASK_API

#include <ArduinoJson.h>
#include <HTTPClient.h>


APIconfig ems_apiconfig;

bool auth() {
    if (WiFi.status() == WL_CONNECTED) {
        if (ems_apiconfig.authToken == ""){
            ems_apiconfig.pending = 1;
            HTTPClient http;
            http.begin(ems_apiconfig.baseurl + "auth");
            http.addHeader("X-APIKEY", ems_apiconfig.apiKey);    
            int httpCode = http.GET();  // Send the GET request
            String payload = http.getString();  // Get the response
           
            if (httpCode == 200) {
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (error) {
                //Console0.printf("deserializeJson() failed: "));
                //Console0.printf(error.f_str());
                return false;
            }

            // Get the values from JSON
            if (doc["AUTHENTICATED"] == "FALSE") {
                // Handle unauthenticated case, show pincode screen
                Console0.printf("%d [API] Not authenticated. Show pincode screen.\n",millis());
                return false;
            } else {
                ems_apiconfig.authToken = doc["X-AUTHTOKEN"].as<String>();
                Console0.printf("%d [API] Authenticated. auth-token retrieved: %s.\n",millis(), ems_apiconfig.authToken);
                return true;
            }
            }
            ems_apiconfig.pending = 0;
            http.end();
        } else {
            //Console0.printf("Already Authenticated\n");
        }
        return true;
    } else {
            Console0.printf("%d [API] Error in WiFi connection\n",millis());
        }
        return false;
}

bool adopt() {
  if (WiFi.status() == WL_CONNECTED) {
    ems_apiconfig.pending = 1;
    HTTPClient http;
    http.begin(ems_apiconfig.baseurl + "adopt");
    http.addHeader("Content-Type", "application/json");  // Standard content type
    http.addHeader("X-APIAUTHPIN", ems_apiconfig.pincode);             // Place the pincode in the header
    http.addHeader("X-MAC", WiFi.macAddress());          // Place the MAC address in the header



    int httpCode = http.GET();  // Send the POST request
    String response = http.getString();  // Get the response

  //Console0.printf("%d HTTP Code %d\n",millis(),httpCode);
  // Console0.printf("%d HTTP Payload %s\n",millis(),response);

   if (httpCode == 200) {
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, response);

      if (error) {
        //Console0.printf("deserializeJson() failed: "));
        //Console0.printf(error.f_str());
        return false;
      }
      
      // Get the values from JSON
      if (!doc["X-APIKEY"]) {
        // Handle unauthenticated case, show pincode screen
        Console0.printf("%d [API] Not adopted\n",millis());
        return false;
      } else {
        ems_apiconfig.apiKey = doc["X-APIKEY"].as<String>();
        Console0.printf("%d [API] Adopt completed.\n",millis());
        return true;
      }
    }
    ems_apiconfig.pending = 0;
    http.end();
  }
  return false;
}

String getnextHour(int SOC) {
    Console0.printf("%d [API] fetching setpoint with SOC %d \n", millis(),SOC);
    ems_apiconfig.pending = 1;

    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(ems_apiconfig.baseurl + "api/getNextHour");

        // Add necessary headers
        http.addHeader("Content-Type", "application/json");
        http.addHeader("X-AUTHTOKEN", ems_apiconfig.authToken);

        // Create JSON body with the current power data
        String jsonBody = "{\"SOC\":" + String(SOC) + "}";

        int httpCode = http.POST(jsonBody);  // Send the POST request
        String response = http.getString();  // Get the response

        if (httpCode == 200) {
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, response);

            if (error) {
                Console0.printf("%d [API] error deserialization of API data failed: %s \n", millis(), error.f_str());
                return "ERROR";
            }

            // Check if the key "gridload" exists and is valid
            if (doc.containsKey("gridload")) {
                //float gridload = doc["gridload"].as<float>();
                //float batteryload = doc["batteryload"].as<float>();
                //gridload = gridload * -1;
                
                batteryCloudData.timestamp = millis();
                batteryCloudData.calc_gridload =  doc["gridload"].as<float>();
                batteryCloudData.calc_batteryload =  doc["batteryload"].as<float>();
                batteryCloudData.calc_solarload =  doc["solarload"].as<float>();
                batteryCloudData.d_soc =  doc["soc"].as<int>();
                batteryCloudData.cur_tariff =  doc["tariff"].as<float>();
                batteryCloudData.battery_capacity =  doc["batterycap"].as<float>();

                Console0.printf("%d [API] Cloud Battery Management Data Updated\n", millis());
                //setInverterBatterySetpoint(batteryload);  // Assuming setInverterDelivery accepts float
            } else {
                Console0.printf("%d [API] Cloud Battery Management Data response failed!\n", millis());
            }
        } else {
            Console0.printf("%d [API] HTTP error code: %d \n", millis(), httpCode);
        }

        http.end();
        ems_apiconfig.pending = 0;
        return response;
    }

    return "";
}
void update(int SOC) {
  if (WiFi.status() == WL_CONNECTED) {
    ems_apiconfig.pending = 1;
    HTTPClient http;
    http.begin(ems_apiconfig.baseurl + "update");
    
    // Add necessary headers
    http.addHeader("Content-Type", "application/json");
    http.addHeader("ap-key", ems_apiconfig.apiKey);
    http.addHeader("authentication", ems_apiconfig.authToken);


    // Create JSON body with the current power data
    String jsonBody = "{\"SOC\":" + String(SOC) + "}";

    int httpCode = http.POST(jsonBody);  // Send the POST request
    String response = http.getString();  // Get the response

   Console0.printf("%d HTTP Code %d\n",millis(),httpCode);
   Console0.printf("%d HTTP Payload %s\n",millis(),response);

  

    http.end();
    ems_apiconfig.pending = 0;
  }
}

void APIhandler(void * parameter){
    
    Console0.printf("%d [API] Handler started\n");
    unsigned long startAttemptTime = millis();
    for(;;){
        if(WiFi.status() == WL_CONNECTED){
            if (ems_apiconfig.apiKey == ""){
                adopt();
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue;
            }
            if (auth()){
                ems_apiconfig.status = 2;
                //String provisionData = provision();
                ems_apiconfig.status = 3;
                getnextHour(SofarInv1.getCurrentSOC());
                vTaskDelay(60000 / portTICK_PERIOD_MS);
                continue;
            }
                
                
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
            
        continue;
    }
   
}

void setupAPIConfig() {
  ems_apiconfig.baseurl = "https://ems.ef-it.nl/";  // Initialize baseurl here
  ems_apiconfig.pincode = 581278;

     xTaskCreatePinnedToCore(
      APIhandler,
      "API",  // Task name
      15000,            // Stack size (bytes)
      NULL,             // Parameter
      1,                // Task priority
      NULL,             // Task handle
      ARDUINO_RUNNING_CORE
    );
}

#endif