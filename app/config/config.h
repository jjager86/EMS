#ifndef CONFIG
#define CONFIG

/**
 * Set this to false to disable Serial logging
 */
#define DEBUG true

#define DEVICE_NAME "PWRCTRL-SHS17"

//const char* deviceName = "PowerControl";
///const char* version = "v0.0.1";

// **********************************
// * WiFi Settings                  *
// **********************************

#define WIFI_NETWORK "Jager"//**Your SSID here**
#define WIFI_PASSWORD "0513552070"//**Your password here** leave empty if open (bad!)

// * Wifi timeout in milliseconds
#define WIFI_TIMEOUT 3000
#define WIFI_RECOVER_TIME_MS 20000 // 20 seconds

//Uncomment this to set a static IP instead of DHCP for the ESP (Separate by commas instead of dots)
//#define WIFI_IP 192, 168, 0, 5
//#define WIFI_SUBNET 255, 255, 255, 0
//#define WIFI_GATEWAY 192, 168, 0, 1
//#define WIFI_PRIMARY_DNS 8, 8, 8, 8     //optional
//#define WIFI_SECONDARY_DNS 8, 8, 4, 4   //optional

/**
 * Syncing time with an NTP server
 */
#define NTP_TIME_SYNC_ENABLED true
#define NTP_SERVER "nl.pool.ntp.org"
#define NTP_OFFSET_SECONDS 3600
#define NTP_UPDATE_INTERVAL_MS 60000

/**
 * Wether or not you want to enable Home Assistant integration
 */
#define HA_ENABLED true
#define HA_ADDRESS "10.75.150.134"
#define HA_PORT 1883
#define HA_USER ""
#define HA_PASSWORD ""

#define MQTT_CONNECT_DELAY 200
#define MQTT_CONNECT_TIMEOUT 20000 // 20 seconds

// * The password used for OTA
#define OTA_PASSWORD "admin"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


unsigned long lastPress = 0;

#endif