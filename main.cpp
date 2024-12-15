#include <FS.h>
#include <EEPROM.h>
#include <DNSServer.h>
#include <M5StickCPlus.h>
#include <Ticker.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include "mqttserial.h"
#include "restart.h"

// * Include settings
#include "settings.h"

// * Initiate led blinker library
Ticker ticker;

// * Initiate WIFI client
WiFiClient espClient;

// * Initiate MQTT client
PubSubClient mqtt_client(espClient);



bool busy = false;

#if defined(ARDUINO_M5Stick_C) || defined(ARDUINO_M5Stick_C_Plus)
long LCDTimeout = 40000;//Keep screen ON for 40s then turn off. ButtonA will turn it On again.
#endif


// **********************************
// * Ticker (System LED Blinker)    *
// **********************************

// * Blink on-board Led
void tick()
{
    // * Toggle state
    //int state = digitalRead(LED_BUILTIN);    // * Get the current state of GPIO1 pin
    //digitalWrite(LED_BUILTIN, !state);       // * Set pin to the opposite state
}

// **********************************
// * MQTT                           *
// **********************************

// * Send a message to a broker topic
void send_mqtt_message(const char *topic, char *payload)
{
    Serial.printf("MQTT Outgoing on %s: ", topic);
    Serial.println(payload);

    bool result = mqtt_client.publish(topic, payload, false);

    if (!result)
    {
        Serial.printf("MQTT publish to topic %s failed\n", topic);
    }
}

// * Reconnect to MQTT server and subscribe to in and out topics
bool mqtt_reconnect()
{
    // * Loop until we're reconnected
    int MQTT_RECONNECT_RETRIES = 0;

    while (!mqtt_client.connected() && MQTT_RECONNECT_RETRIES < MQTT_MAX_RECONNECT_TRIES)
    {
        MQTT_RECONNECT_RETRIES++;
        Serial.printf("MQTT connection attempt %d / %d ...\n", MQTT_RECONNECT_RETRIES, MQTT_MAX_RECONNECT_TRIES);

        // * Attempt to connect
        if (mqtt_client.connect(HOSTNAME, MQTT_USERNAME, MQTT_PASSWORD))
        {
            Serial.println(F("MQTT connected!"));

            // * Once connected, publish an announcement...
            char *message = new char[16 + strlen(HOSTNAME) + 1];
            strcpy(message, "p1 meter alive: ");
            strcat(message, HOSTNAME);
            mqtt_client.publish("hass/status", message);

            Serial.printf("MQTT root topic: %s\n", MQTT_ROOT_TOPIC);
        }
        else
        {
            Serial.print(F("MQTT Connection failed: rc="));
            Serial.println(mqtt_client.state());
            Serial.println(F(" Retrying in 5 seconds"));
            Serial.println("");

            // * Wait 5 seconds before retrying
            delay(5000);
        }
    }

    if (MQTT_RECONNECT_RETRIES >= MQTT_MAX_RECONNECT_TRIES)
    {
        Serial.printf("*** MQTT connection failed, giving up after %d tries ...\n", MQTT_RECONNECT_RETRIES);
        return false;
    }

    return true;
}

void send_metric(String name, long metric)
{
    Serial.print(F("Sending metric to broker: "));
    Serial.print(name);
    Serial.print(F("="));
    Serial.println(metric);

    char output[10];
    ltoa(metric, output, sizeof(output));

    String topic = String(MQTT_ROOT_TOPIC) + "/" + name;
    send_mqtt_message(topic.c_str(), output);
}

void send_data_to_broker()
{
    send_metric("consumption_low_tarif", CONSUMPTION_LOW_TARIF);
    send_metric("consumption_high_tarif", CONSUMPTION_HIGH_TARIF);
    send_metric("returndelivery_low_tarif", RETURNDELIVERY_LOW_TARIF);
    send_metric("returndelivery_high_tarif", RETURNDELIVERY_HIGH_TARIF);
    send_metric("actual_consumption", ACTUAL_CONSUMPTION);
    send_metric("actual_returndelivery", ACTUAL_RETURNDELIVERY);

    send_metric("l1_instant_power_usage", L1_INSTANT_POWER_USAGE);
    send_metric("l2_instant_power_usage", L2_INSTANT_POWER_USAGE);
    send_metric("l3_instant_power_usage", L3_INSTANT_POWER_USAGE);
    send_metric("l1_instant_power_current", L1_INSTANT_POWER_CURRENT);
    send_metric("l2_instant_power_current", L2_INSTANT_POWER_CURRENT);
    send_metric("l3_instant_power_current", L3_INSTANT_POWER_CURRENT);
    send_metric("l1_voltage", L1_VOLTAGE);
    send_metric("l2_voltage", L2_VOLTAGE);
    send_metric("l3_voltage", L3_VOLTAGE);
    
    send_metric("gas_meter_m3", GAS_METER_M3);

    send_metric("actual_tarif_group", ACTUAL_TARIF);
    send_metric("short_power_outages", SHORT_POWER_OUTAGES);
    send_metric("long_power_outages", LONG_POWER_OUTAGES);
    send_metric("short_power_drops", SHORT_POWER_DROPS);
    send_metric("short_power_peaks", SHORT_POWER_PEAKS);
}


bool isNumber(char *res, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (((res[i] < '0') || (res[i] > '9')) && (res[i] != '.' && res[i] != 0))
            return false;
    }
    return true;
}

int FindCharInArrayRev(char array[], char c, int len)
{
    for (int i = len - 1; i >= 0; i--)
    {
        if (array[i] == c)
            return i;
    }
    return -1;
}

long getValue(char *buffer, int maxlen, char startchar, char endchar)
{
    int s = FindCharInArrayRev(buffer, startchar, maxlen - 2);
    int l = FindCharInArrayRev(buffer, endchar, maxlen - 2) - s - 1;

    char res[16];
    memset(res, 0, sizeof(res));

    if (strncpy(res, buffer + s + 1, l))
    {
        if (endchar == '*')
        {
            if (isNumber(res, l))
                // * Lazy convert float to long
                return (1000 * atof(res));
        }
        else if (endchar == ')')
        {
            if (isNumber(res, l))
                return atof(res);
        }
    }
    return 0;
}





// **********************************
// * Setup OTA                      *
// **********************************

void setup_ota()
{
    Serial.println(F("Arduino OTA activated."));

    // * Port defaults to 8266
    ArduinoOTA.setPort(8266);

    // * Set hostname for OTA
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.onStart([]()
    {
        Serial.println(F("Arduino OTA: Start"));
    });

    ArduinoOTA.onEnd([]()
    {
        Serial.println(F("Arduino OTA: End (Running reboot)"));
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
    {
        Serial.printf("Arduino OTA Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error)
    {
        Serial.printf("Arduino OTA Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
            Serial.println(F("Arduino OTA: Auth Failed"));
        else if (error == OTA_BEGIN_ERROR)
            Serial.println(F("Arduino OTA: Begin Failed"));
        else if (error == OTA_CONNECT_ERROR)
            Serial.println(F("Arduino OTA: Connect Failed"));
        else if (error == OTA_RECEIVE_ERROR)
            Serial.println(F("Arduino OTA: Receive Failed"));
        else if (error == OTA_END_ERROR)
            Serial.println(F("Arduino OTA: End Failed"));
    });

    ArduinoOTA.begin();
    Serial.println(F("Arduino OTA finished"));
}

// **********************************
// * Setup MDNS discovery service   *
// **********************************

void setup_mdns()
{
    Serial.println(F("Starting MDNS responder service"));

    bool mdns_result = MDNS.begin(HOSTNAME);
    if (mdns_result)
    {
        MDNS.addService("http", "tcp", 80);
    }
}




// **********************************
// * WiFi Setup                     *
// **********************************

void checkWifi()
{
  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (i++ == 120)
    {
      Serial.printf("Tried connecting for 60 sec, rebooting now.");
      restart_board();
    }
  }
}

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  mqttSerial.printf("Connecting to %s\n", WIFI_SSID);

  #if defined(WIFI_IP) && defined(WIFI_GATEWAY) && defined(WIFI_SUBNET)
    IPAddress local_IP(WIFI_IP);
    IPAddress gateway(WIFI_GATEWAY);
    IPAddress subnet(WIFI_SUBNET);

    #ifdef WIFI_PRIMARY_DNS
      IPAddress primaryDNS(WIFI_PRIMARY_DNS);
    #else
      IPAddress primaryDNS();
    #endif

    #ifdef WIFI_SECONDARY_DNS
      IPAddress secondaryDNS(WIFI_SECONDARY_DNS);
    #else
      IPAddress secondaryDNS();
    #endif

    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      mqttSerial.println("Failed to set static ip!");
    }
  #endif

  WiFi.begin(WIFI_SSID, WIFI_PWD, 0,0,true);
  checkWifi();
  mqttSerial.printf("Connected. IP Address: %s\n", WiFi.localIP().toString().c_str());
}



void setupScreen(){
#if defined(ARDUINO_M5Stick_C) || defined(ARDUINO_M5Stick_C_Plus)
  M5.begin();
  M5.Axp.EnableCoulombcounter();
  M5.Lcd.setRotation(1);
  M5.Axp.ScreenBreath(12);
  M5.Lcd.fillScreen(TFT_WHITE);
  M5.Lcd.setFreeFont(&FreeSansBold12pt7b);
  m5.Lcd.setTextDatum(MC_DATUM);
  int xpos = M5.Lcd.width() / 2; // Half the screen width
  int ypos = M5.Lcd.height() / 2; // Half the screen width
  M5.Lcd.setTextColor(TFT_DARKGREY);
  M5.Lcd.drawString("SmartEnergy v0.9.1", xpos,ypos,1);
  delay(2000);
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextFont(1);
  M5.Lcd.setTextColor(TFT_GREEN);
#endif
}



// **********************************
// * Loop                           *
// **********************************

void loop()
{
    
    unsigned long start = millis();
  if (WiFi.status() != WL_CONNECTED)
  { //restart board if needed
    checkWifi();
  }
    
    ArduinoOTA.handle();
    long now = millis();

    if (!mqtt_client.connected())
    {
        if (now - LAST_RECONNECT_ATTEMPT > 5000)
        {
            LAST_RECONNECT_ATTEMPT = now;

            if (mqtt_reconnect())
            {
                LAST_RECONNECT_ATTEMPT = 0;
            }
        }
    }
    else
    {
        mqtt_client.loop();
    }
    
    if (now - LAST_UPDATE_SENT > UPDATE_INTERVAL) {
        mqttSerial.println("Quering P1...");
        //read_p1_hardwareserial();
        mqttSerial.println("OK\n");
    }
}

uint16_t loopcount =0;

void extraLoop()
{
  loop();
  ArduinoOTA.handle();
  while (busy)
  { //Stop processing during OTA
    ArduinoOTA.handle();
  }

#if defined(ARDUINO_M5Stick_C) || defined(ARDUINO_M5Stick_C_Plus)
  if (M5.BtnA.wasPressed()){//Turn back ON screen
    M5.Axp.ScreenBreath(12);
    LCDTimeout = millis() + 30000;
  }else if (LCDTimeout < millis()){//Turn screen off.
    M5.Axp.ScreenBreath(0);
  }
  M5.update();
#endif
}

void waitLoop(uint ms){
      unsigned long start = millis();
      while (millis() < start + ms) //wait .5sec between registries
      {
        extraLoop();
      }
}


// **********************************
// * Setup Main                     *
// **********************************


void setup()
{
  Serial.begin(115200);
  setupScreen();


    mqttSerial.print("Setting up wifi...");
    setup_wifi();
    
    // * Configure EEPROM
    //EEPROM.begin(512);

    // Setup a hw serial connection for communication with the P1 meter and logging (not using inversion)
    
    mqttSerial.print("Configuring P1 Interface...");
    //Serial.begin(BAUD_RATE, SERIAL_8N1);
    //Serial.println("");
    //Serial.println("Swapping UART0 RX to inverted");
    //Serial.flush();

    // Invert the RX serialport by setting a register value, this way the TX might continue normally allowing the serial monitor to read println's
    //USC0(UART0) = USC0(UART0) | BIT(UCRXI);
    mqttSerial.println("OK\n");

    
    // * Configure OTA
    //setup_ota();

    // * Startup MDNS Service
    //setup_mdns();

    // * Setup MQTT
    mqttSerial.printf("MQTT connecting to: %s:%s\n", MQTT_SERVER, MQTT_PORT);

    //mqtt_client.setServer(MQTT_SERVER, MQTT_PORT);

  

while (true)
    {
      mqttSerial.println(".");

      //extraLoop();
    }
 
}





