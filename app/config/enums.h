#ifndef ENUMS
#define ENUMS

struct invmeterdata
{
	float pa;
	float pb;
	bool active;
};

struct pwrsetpoint
{
	int setpoint;
  int last_setpoint;
	unsigned long timestamp;
};

struct batteryclouddata
{
  float calc_gridload;   // Calculated grid load (in Watts)
  float calc_batteryload; // Calculated battery load (in Watts)
  float calc_solarload;  // Calculated solar load (in Watts)
  float battery_capacity;  // battery capacity (in Watt/hour)
  int d_soc;             // Desired state of charge (in percentage)
  int c_soc;             // Desired state of charge (in percentage)
  float cur_tariff;      // Current tariff (e.g., in €/kWh)
  float requested_batteryload; // Calculated battery load (in Watts)
	unsigned long timestamp;
};


struct meterdata
{
	float pt;
	float pa;
	float pb;
	float pc;
	unsigned long lastvalue;
  uint8_t ctr;
};

union FLOAT2INT {
  float value;
  struct {
    uint16_t int0;
    uint16_t int1;
  };
};

/*struct MQTT_HA_ConfigMessage {
  String obj_name;
  String mqtt_name;
  String mqtt_unit;
  String mqtt_class;
  String mqtt_unique_id;
};*/

struct APIconfig {
  String apiKey;
  String authToken;
  int status; //0 not auth, 1 adopt , 2 auth, 3 prov, 4, update
  int pending;
  String baseurl;
  String pincode;
};


#if DEBUG == true
  #define serial_print(x)  Serial.print (x)
  #define serial_println(x)  Serial.println (x)
#else
  #define serial_print(x)
  #define serial_println(x)
#endif

#endif