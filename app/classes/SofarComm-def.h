#ifndef SOFARCOMM_DEFINITION_H
#define SOFARCOMM_DEFINITION_H


// Sofar run states
#define waiting 0
#define check 1

#define normal			2
#define epsState		3
#define faultState		4
#define permanentFaultState	5
#define normal1			6

// State names are a bit strange - makes sense to also match to these?
#define charging		2
#define discharging		6

#define HUMAN_CHARGING		"Normal"
#define HUMAN_DISCHARGING	"Normal1"

#define MAX_FRAME_SIZE          64
#define MODBUS_FN_READSINGLEREG 0x03
#define SOFAR_FN_PASSIVEMODE    0x42
#define SOFAR_PARAM_STANDBY     0x5555

//unsigned int INVERTER_RUNNINGSTATE;

// Battery Save mode is a hybrid mode where the battery will charge from excess solar but not discharge.
//bool BATTERYSAVE = true;

// SoFar ME3000 Information Registers
#define SOFAR_REG_SERIAL	0x0470
#define SOFAR_REG_OPERSTATE	0x0404

#define SOFAR_REG_GRIDV		0x048D
#define SOFAR_REG_GRIDA		0x048E
#define SOFAR_REG_GRIDFREQ	0x0484
#define SOFAR_REG_GRIDW	    0x0488


#define SOFAR_REG_PV1		0x0586
#define SOFAR_REG_PV2		0x0589
#define SOFAR_REG_PVT		0x05C4

#define SOFAR_REG_BATTV		0x604
#define SOFAR_REG_BATTA		0x605
#define SOFAR_REG_BATTW		0x606
#define SOFAR_REG_BATTTEMP	0x607
#define SOFAR_REG_BATTSOC	0x608
#define SOFAR_REG_BATTSOH	0x609
#define SOFAR_REG_BATTCYC	0x60A

#define SOFAR_REG_BATTCHARGE	0x694
#define SOFAR_REG_BATTDECHARGE	0x698

#define SOFAR_REG_SYSIOW	0x04AF

//#define SOFAR_REG_PVW		0x04AE
//#define SOFAR_REG_PVDAY		0x0218
//#define SOFAR_REG_EXPDAY	0x0219
//#define SOFAR_REG_IMPDAY	0x021a
//#define SOFAR_REG_LOADDAY	0x021b

#define SOFAR_REG_INTTEMP	0x6021
#define SOFAR_REG_HSTEMP	0x6022

#define SOFAR_FN_STANDBY	0x0
#define SOFAR_FN_DISCHARGE	0x01
#define SOFAR_FN_CHARGE		0x02

#define SOFAR_PAS_DESIREDPWR		0x1187
#define SOFAR_PAS_MAXBATPWR		0x1189
#define SOFAR_PAS_PERPWRSELL		0x118d //toegestaan hoeveelheid inkoop/verkoop neg=naar grid
#define SOFAR_PAS_PURCHASEPWR		0x118f

/*
0x1187 i32 desired grid power
0x1189 i32 max bat power + charge + discharge
0x118d i32 permissible power selling + means from the grid - to the grid
0x118f i32 purchase of power + grid to system
*/

#define SOFAR_OPER_WAITING	0x0
#define SOFAR_OPER_DETECT   0x1
#define SOFAR_OPER_GRIDCONN	0x2
#define SOFAR_OPER_EMERPWR	0x3
#define SOFAR_OPER_RECFAULT	0x4
#define SOFAR_OPER_PERMFAULT	0x5
#define SOFAR_OPER_UPGRADE	0x6
#define SOFAR_OPER_SELFCHARGE	0x7
#define SOFAR_OPER_SVGSTATE	0x8
#define SOFAR_OPER_PIDSTATE	0x9

#define	MAX_POWER		5000	


// These timers are used in the main loop.
#define HEARTBEAT_INTERVAL 9000
#define RUNSTATE_INTERVAL 5000
#define SEND_INTERVAL 10000
#define BATTERYSAVE_INTERVAL 3000

struct Inverter {
  uint8_t modbusId;
  String name;
  int Phase;
  int conn_status = 0;
  String status;
  uint16_t GridPower;
  uint16_t PvPower;
  uint16_t BatteryPower;
  String jsonbuffer;
  unsigned int RunningState;
  bool BatterySaveMode = true;
};


enum ValueType {
    INT32,
    UINT32,
    UINT16,
    INT16,
    FLOAT
};


//MQTT Solar Inverter Register
struct mqtt_status_register
{
    uint16_t regnum;
    float multiplier;
    char unit[4];  // Maximum size for units ("Hz", "V", "A", etc.)
    char mqtt_name[50];  // Adjust the size based on your requirements
    char mqtt_class[50];  // Adjust the size based on your requirements
    ValueType value_type; // Add the value type
};

static struct mqtt_status_register mqtt_status_reads[] =
{
    { SOFAR_REG_OPERSTATE, 0, "", "operational_state", "", UINT16 },
    { SOFAR_REG_GRIDV, 10, "V", "grid_voltage", "", INT16 },
    { SOFAR_REG_GRIDA, 100, "A", "grid_current", "", INT16 },
    { SOFAR_REG_GRIDFREQ, 100, "Hz", "grid_freq", "", UINT16 },
    { SOFAR_REG_GRIDW, 0.1, "W", "grid_power", "", INT16 },
    { SOFAR_REG_BATTW, 0.1, "W", "battery_power", "", INT16 },
    { SOFAR_REG_BATTV, 10, "V", "battery_voltage", "", INT16 },
    { SOFAR_REG_BATTA, 100, "A", "battery_current", "", INT16 },
    { SOFAR_REG_BATTSOC, 1, "%", "battery_soc", "", UINT16 },
    { SOFAR_REG_BATTSOH, 1, "%", "battery_soh", "", UINT16 },
    { SOFAR_REG_BATTTEMP, 1, "C", "battery_temp", "", INT16 },
    { SOFAR_REG_BATTCYC, 1, "C", "battery_cycles", "", UINT16 },
    { SOFAR_REG_PV1, 0.1, "W", "solarpv1_pwr", "", UINT16 },
    { SOFAR_REG_PV2, 0.1, "W", "solarpv2_pwr", "", UINT16 },
    { SOFAR_REG_BATTCHARGE, 10000, "W", "battery_charged", "", UINT32 },
    { SOFAR_REG_BATTDECHARGE, 10000, "W", "battery_decharged", "", UINT32 },
    { 0x0692, 10000, "W", "energy_selling_total", "", UINT32 },
    { 0x068E, 10000, "W", "energy_purchase_total", "", UINT32 }
};

struct inverterdata {
    unsigned long timestamp;
    uint16_t operational_state;
    float grid_voltage;
    float grid_current;
    float grid_freq;
    float grid_power;
    float battery_power;
    float battery_voltage;
    float battery_current;
    uint16_t battery_soc;
    uint16_t battery_soh;
    float battery_temp;
    uint16_t battery_cycles;
    float solarpv1_pwr;
    float solarpv2_pwr;
    uint32_t battery_charged;
    uint32_t battery_decharged;
    uint32_t energy_selling_total;
    uint32_t energy_purchase_total;
};

static inverterdata inverterData;

#endif  // SOFARCOMM_DEFINITION_H