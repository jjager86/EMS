#include "SofarComm-rtos.h"
#include "config/queueDefinitions.h"
#include "config/enums.h"

SofarComm::SofarComm(){}

   void SofarComm::setFieldValue(const std::string& mqttname, float val) {
    try {
		this->_inverterData->timestamp = millis();
        if (mqttname == "operational_state") {
            this->_inverterData->operational_state = static_cast<uint16_t>(val);
        } else if (mqttname == "grid_voltage") {
            this->_inverterData->grid_voltage = val;
        } else if (mqttname == "grid_current") {
            this->_inverterData->grid_current = val;
        } else if (mqttname == "grid_freq") {
            this->_inverterData->grid_freq = val;
        } else if (mqttname == "grid_power") {
            this->_inverterData->grid_power = val;
        } else if (mqttname == "battery_power") {
            this->_inverterData->battery_power = val;
        } else if (mqttname == "battery_voltage") {
            this->_inverterData->battery_voltage = val;
        } else if (mqttname == "battery_current") {
            this->_inverterData->battery_current = val;
        } else if (mqttname == "battery_soc") {
            this->_inverterData->battery_soc = static_cast<uint16_t>(val);
        } else if (mqttname == "battery_soh") {
            this->_inverterData->battery_soh = static_cast<uint16_t>(val);
        } else if (mqttname == "battery_temp") {
            this->_inverterData->battery_temp = val;
        } else if (mqttname == "battery_cycles") {
            this->_inverterData->battery_cycles = static_cast<uint16_t>(val);
        } else if (mqttname == "solarpv1_pwr") {
            this->_inverterData->solarpv1_pwr = val;
        } else if (mqttname == "solarpv2_pwr") {
            this->_inverterData->solarpv2_pwr = val;
        } else if (mqttname == "battery_charged") {
            this->_inverterData->battery_charged = static_cast<uint32_t>(val);
        } else if (mqttname == "battery_decharged") {
            this->_inverterData->battery_decharged = static_cast<uint32_t>(val);
        } else {
            this->Console->printf("%d [SOFAR] Field '%s' not found\n", millis(), mqttname.c_str());
        }
    } catch (const std::exception& e) {
        this->Console->printf("%d [SOFAR] Error setting field '%s': %s\n", millis(), mqttname.c_str(), e.what());
    }
}

void SofarComm::PostInverterObjtoQueue(void* pvParameters){
	SofarComm *_this = (SofarComm *) pvParameters;
	for(;;){
		if (_this->_I.conn_status == 1){
			QueueHandle_t queue = *_this->_InverterDataQueue;
			if( queue != 0 )
			{
				if (xQueueSend(queue, (void *) &_this->_I, (TickType_t )0) == pdPASS ){
					_this->Console->printf("%d [SOFAR_%s] Send data to MQTT Queue\n",millis(),_this->_I.name);
				} else {
					_this->Console->printf("%d [SOFAR_%s] Send data to MQTT Queue FAILED, does queue exist\n",millis(),_this->_I.name);
				}
			}
		}
		
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}


float SofarComm::getCurrentSOC(){
	return this->_inverterData->battery_soc;
}

void SofarComm::generateHAAutoDisoveryTask(void *pvParameters){
	SofarComm *_this = (SofarComm *) pvParameters;
	vTaskDelay((5 * 1000) / portTICK_PERIOD_MS);
	AutoConfigQueue123 = xQueueCreate( 80, sizeof(int) );
	for(;;){
		int i;
		i = 1;
		if( AutoConfigQueue123 != NULL ){
						_this->Console->println("Queue Set");
		//				_this->Console->println(i);
						if (xQueueSend(AutoConfigQueue123, (void *) &i, (TickType_t )0) == pdPASS ){
							_this->Console->printf("%d [SOFAR_%s] HA Autoconfig queued\n",millis());
						} else {
							_this->Console->printf("%d [SOFAR_%s] HA Autoconfig failed, queue full?\n",millis());
						}
					} else {
						_this->Console->printf("%d [SOFAR_%s] HA Autoconfig failed, no queue\n",millis());
					}
					vTaskDelay((200) / portTICK_PERIOD_MS);
	}
	vTaskDelay((20000) / portTICK_PERIOD_MS);
	/*_this->Console->printf("%d [SOFAR] HA Autoconfig task started\n",millis());
	AutoConfigQueue = xQueueCreate( 15, sizeof(int) );
	for(;;){
		int	l;
		_this->Console->printf("%d [SOFAR] HA Autoconfig run\n",millis());
		for(l = 0; l < sizeof(mqtt_status_reads)/sizeof(struct mqtt_status_register); l++){
			int i;
			i = l;
			MQTT_HA_ConfigMessage _msg;
			/*_msg.obj_name= String(_this->_I.name);
			_msg.mqtt_unit=String(mqtt_status_reads[l].unit);
			_msg.mqtt_name=String(mqtt_status_reads[l].mqtt_name);
			_msg.mqtt_class=String(mqtt_status_reads[l].mqtt_class);
			_msg.mqtt_unique_id=String("_Inverter_" + _this->_I.name)
			snprintf(_msg.obj_name, sizeof(_msg.obj_name), "%s", _this->_I.name);
      snprintf(_msg.mqtt_unit, sizeof(_msg.mqtt_unit), "%s", mqtt_status_reads[l].unit);
      snprintf(_msg.mqtt_name, sizeof(_msg.mqtt_name), "%s", mqtt_status_reads[l].mqtt_name);
      snprintf(_msg.mqtt_class, sizeof(_msg.mqtt_class), "%s", mqtt_status_reads[l].mqtt_class);
      snprintf(_msg.mqtt_unique_id, sizeof(_msg.mqtt_unique_id), "_Inverter_%s", _this->_I.name);
						
			_this->Console->println(sizeof(_msg));
			if( AutoConfigQueue != NULL ){
				_this->Console->println("Queue Set");
				if (xQueueSend(AutoConfigQueue, (void *) &i, (TickType_t )0) == pdPASS ){
					_this->Console->printf("%d [SOFAR_%s] HA Autoconfig queued\n",millis());
				} else {
					_this->Console->printf("%d [SOFAR_%s] HA Autoconfig failed, queue full?\n",millis());
				}
			} else {
				_this->Console->printf("%d [SOFAR_%s] HA Autoconfig failed, no queue\n",millis());
			}
			vTaskDelay((200) / portTICK_PERIOD_MS);
		}
		vTaskDelay((10 * 1000) / portTICK_PERIOD_MS);
	}*/

	
}

void SofarComm::AttachConsole(HardwareSerial &console)
{
	Console = &console;
}


void SofarComm::setMQTTdata(PubSubClient &PubSubClient, char &server,char &username,char &password,char &topicname){
	_server = server;
	_username = username;
	_password = password;	
	_topicname = topicname;
}

void SofarComm::configure(uint8_t modbusId,String name,int Phase,EspSoftwareSerial::UART &Serial,bool &SerialClaimed, QueueHandle_t &InverterDataQueue,NTPClient &timeClient,inverterdata &inverterData,batteryclouddata &batteryCloudData){
	SofarComm::_I.modbusId = modbusId;
	SofarComm::_I.name = name;
	SofarComm::_I.Phase = Phase;
	_Modbus.begin(_I.modbusId, Serial);
	_Serial = &Serial;
	_SerialClaimed = &SerialClaimed;
	_InverterDataQueue = &InverterDataQueue;
	_timeClient = &timeClient;
	_inverterData = &inverterData;
	_batteryCloudData = &batteryCloudData;
	

}

void SofarComm::enableHAAutoDiscovery(QueueHandle_t AutoConfigQueue){
	_AutoConfigQueue = AutoConfigQueue;
	xTaskCreatePinnedToCore(
			generateHAAutoDisoveryTask,           // Task function
			"AutoConfigQueueTask",               // Task name
			2000,                  // Stack size (bytes)
			(void *)this,                   // Task parameter
			 1,
			 NULL,
			 1
	);
	
}

void SofarComm::start(int core = APP_CPU_NUM){
	/*xTaskCreatePinnedToCore(
    	this->HeartbeatCallback,           // Task function
      	"HeartbeatInverter",               // Task name
      	2000,                  // Stack size (bytes)
      	(void *)this,                   // Task parameter
      	1,                      // Task priority
      	//&RunStateTask,
	  	NULL,
	  	core
	);*/

	


	
	xTaskCreatePinnedToCore(
      this->RunStateCallback,           // Task function
      "FetchDataFromInverter",               // Task name
      2000,                  // Stack size (bytes)
      (void *)this,                   // Task parameter
      2,                      // Task priority
     //&RunStateTask,
	  	NULL,
	  	core
	);

		xTaskCreatePinnedToCore(
      this->setBatteryManagement,           // Task function
      "setBatteryManagement",               // Task name
      2000,                  // Stack size (bytes)
      (void *)this,                   // Task parameter
      2,                      // Task priority
     //&RunStateTask,
	  	NULL,
	  	core
	);
	                  
   /*xTaskCreatePinnedToCore(
      this->batterySaveCallback,           // Task function
      "BatterySave",               // Task name
      2000,                  // Stack size (bytes)
      (void *)this,                   // Task parameter
      2,                      // Task priority
      //&RunStateTask,
	  	NULL,
	  	core
	);*/

   xTaskCreatePinnedToCore(
      this->sendDataCallback,           // Task function
      "FetchValuesFromInverter",               // Task name
      3000,                  // Stack size (bytes)
      (void *)this,                   // Task parameter
      2,                      // Task priority
      //&RunStateTask,
	  	NULL,
	  	core
	);

   xTaskCreatePinnedToCore(
		this->PostInverterObjtoQueue,           // Task function
      	"PostVariablestoMQTTQueue",               // Task name
      	2000,                  // Stack size (bytes)
      	(void *)this,                   // Task parameter
      	2,                      // Task priority
      	//&RunStateTask,
	  	NULL,
	  	core
	);

	
}




void SofarComm::UpdateInvStatus(String status, int conn_status)
{
	_I.status = status;
	_I.conn_status = conn_status;
}

// This function is executed when an MQTT message arrives on a topic that we are subscribed to.
void SofarComm::setMode(String cmd)
{
	int fnCode;
	int fnParam; 

if(cmd == "PassiveManual")
	{
		fnCode = 0x11a9;
		fnParam = 1;
		
	}
	
/*
	if(cmd == "standby")
	{
		fnCode = SOFAR_FN_STANDBY;
		fnParam = SOFAR_PARAM_STANDBY;
		
	}
	else if(cmd == "auto")
	{
		fnCode = SOFAR_FN_AUTO;
		_I.BatterySaveMode = true;
	}
*/	
	if(fnCode)
	{
		_I.BatterySaveMode = false;
		this->sendPassiveCmd(_I.modbusId, fnCode, fnParam);
	}
}


/*
// This function is executed when an MQTT message arrives on a topic that we are subscribed to.
void SofarComm::setDelivery(int wattage)
{
	String cmd;
	int fnCode;
	int fnParam; 
	this->Console->printf("%d [SOFAR_%s] %dWatts\n",millis(),_I.name, wattage );
	this->setMode("PassiveManual");
	
	if((wattage > 0)) {
		cmd = "Disired GRID PWR (obtain)";
		fnCode = SOFAR_PAS_DESIREDPWR;
		if (wattage > MAX_POWER){
			this->Console->printf("%d [SOFAR_%s] overcharge requested limited to %dWatts\n",millis(),_I.name, MAX_POWER );
			fnParam = MAX_POWER;
		} else {
			fnParam = wattage;
		}
		
		this->Console->printf("%d [SOFAR_%s] Set battery charging with %dWatts\n",millis(),_I.name, fnParam );
	}

	if((0 > wattage)) {
		cmd = "Disired GRID PWR (delivery)";
		fnCode = SOFAR_PAS_DESIREDPWR;
		if (wattage < (MAX_POWER*-1)){
			this->Console->printf("%d [SOFAR_%s] over decharge requested limited to %dWatts\n",millis(),_I.name, MAX_POWER );
			fnParam = (MAX_POWER*-1);
		} else {
			fnParam = wattage *-1;
		}
		this->Console->printf("%d [SOFAR_%s] Set battery discharge with %dWatts\n",millis(),_I.name, (fnParam*-1) );
	}

	if(fnCode)
	{
		_I.BatterySaveMode = false;

		int32_t param = -3000;
		// Split the 32-bit integer into two 16-bit integers
		uint16_t data[2];
	data[0] = (uint16_t)(param >> 16); // High word
	data[1] = (uint16_t)(param & 0xFFFF); // Low word

uint8_t result;
		SofarComm::_Modbus.setTransmitBuffer(0, data[0]);
		SofarComm::_Modbus.setTransmitBuffer(1, data[1]);
		result = SofarComm::_Modbus.writeMultipleRegisters(0x1189,2);
		if(result == SofarComm::_Modbus.ku8MBSuccess)	{
			this->Console->printf("%d [SOFAR_%s] Suc6 on Command 0x%04x@%d \n",millis(),_I.name, fnCode, _I.modbusId );
		} else 	{
			this->Console->printf("%d [SOFAR_%s] Err on Command 0x%04x@%d with error 0x%04x\n",millis(),_I.name, fnCode, _I.modbusId, result  );
		this->Console->println(result);
		}
		//this->sendPassiveCmd(_I.modbusId, fnCode, fnParam);
	}
}

*/







/*
void SofarComm::setDelivery(int wattage)
{
    String cmd;
    int fnCode = 0;
    int fnParam; 
    this->Console->printf("%d [SOFAR_%s] %dWatts\n", millis(), _I.name, wattage);
    this->setMode("PassiveManual");

    if (wattage > 0) {
        cmd = "Desired GRID PWR (obtain)";
        fnCode = SOFAR_PAS_MAXBATPWR;//SOFAR_PAS_DESIREDPWR;
        if (wattage > MAX_POWER) {
            this->Console->printf("%d [SOFAR_%s] Overcharge requested limited to %dWatts\n", millis(), _I.name, MAX_POWER);
            fnParam = MAX_POWER;
        } else {
            fnParam = wattage;
        }
        this->Console->printf("%d [SOFAR_%s] Set battery charging with %dWatts\n", millis(), _I.name, fnParam);
    }

    if (wattage < 0) {
        cmd = "Desired GRID PWR (delivery)";
        fnCode = SOFAR_PAS_MAXBATPWR;//SOFAR_PAS_DESIREDPWR;
        if (wattage < (MAX_POWER * -1)) {
            this->Console->printf("%d [SOFAR_%s] Over-discharge requested limited to %dWatts\n", millis(), _I.name, MAX_POWER);
            fnParam = MAX_POWER ;
        } else {
            fnParam = wattage ;
        }
        this->Console->printf("%d [SOFAR_%s] Set battery discharge with %dWatts\n", millis(), _I.name, fnParam * -1);
    }

    //if (fnCode) {
        _I.BatterySaveMode = false;
		this->Console->printf("%d [SOFAR_%s] Send battery chargestate at %dWatts\n", millis(), _I.name, fnParam);
        // Define the parameters to be tracked
        int32_t Gdes = wattage; // 
        int32_t blo = -5000;   // Minimal Charge and discharge of battery
        int32_t bup = 5000;  // Maximum Charge and Discharge of battery
        int32_t Gdzup = 5500; // Permissible power selling
        int32_t Gdzlo = 5500; // Purchase of power

        // Split the 32-bit integers into 16-bit integers
        uint16_t data[6];
        data[0] = (uint16_t)(Gdes >> 16);  // High word of Gdes
        data[1] = (uint16_t)(Gdes & 0xFFFF); // Low word of Gdes
        data[2] = (uint16_t)(blo >> 16);  // High word of blo
        data[3] = (uint16_t)(blo & 0xFFFF); // Low word of blo
        data[4] = (uint16_t)(bup >> 16);  // High word of bup
        data[5] = (uint16_t)(bup & 0xFFFF); // Low word of bup
        data[6] = (uint16_t)(Gdzup >> 16);  // High word of Gdzup
        data[7] = (uint16_t)(Gdzup & 0xFFFF); // Low word of Gdzup
        data[8] = (uint16_t)(Gdzlo >> 16);  // High word of Gdzlo
        data[9] = (uint16_t)(Gdzlo & 0xFFFF); // Low word of Gdzlo

        uint8_t result;
        for (int i = 0; i < 6; i++) {
			this->Console->printf("Register 0x%04x with value DEC %d HEX 0x%04x \n",(0x1187+i),data[i],data[i]);
            SofarComm::_Modbus.setTransmitBuffer(i, data[i]);
        }
        result = SofarComm::_Modbus.writeMultipleRegisters(0x1187, 6);
        
        if (result == SofarComm::_Modbus.ku8MBSuccess) {
            this->Console->printf("%d [SOFAR_%s] Success on Command 0x%04x@%d\n", millis(), _I.name, fnCode, _I.modbusId);
        } else {
            this->Console->printf("%d [SOFAR_%s] Error on Command 0x%04x@%d with error 0x%04x\n", millis(), _I.name, fnCode, _I.modbusId, result);
            this->Console->println(result);
        }
   // }
}
*/
/*
void SofarComm::setBatteryPower(int wattage)
{
    String cmd;
    int fnCode = 0;
    int fnParam; 
    this->Console->printf("%d [SOFAR_%s] %dWatts\n", millis(), _I.name, wattage);
    this->setMode("PassiveManual");

    if (wattage > 0) {
        cmd = "Desired Power Usage (obtain from battery)";
        fnCode = SOFAR_PAS_MAXBATPWR;//SOFAR_PAS_DESIREDPWR;
        if (wattage > MAX_POWER) {
            this->Console->printf("%d [SOFAR_%s] Overcharge requested limited to %dWatts\n", millis(), _I.name, MAX_POWER);
            fnParam = MAX_POWER;
        } else {
            fnParam = wattage;
        }
        this->Console->printf("%d [SOFAR_%s] Set battery charging with %dWatts\n", millis(), _I.name, fnParam);
    }

    if (wattage < 0) {
        cmd = "Desired Power Charge (charge to battery)";
        fnCode = SOFAR_PAS_MAXBATPWR;//SOFAR_PAS_DESIREDPWR;
        if (wattage < (MAX_POWER * -1)) {
            this->Console->printf("%d [SOFAR_%s] Over-discharge requested limited to %dWatts\n", millis(), _I.name, MAX_POWER);
            fnParam = MAX_POWER ;
        } else {
            fnParam = wattage ;
        }
        this->Console->printf("%d [SOFAR_%s] Set battery discharge with %dWatts\n", millis(), _I.name, fnParam * -1);
    }

    //if (fnCode) {
        _I.BatterySaveMode = false;
		this->Console->printf("%d [SOFAR_%s] Send battery chargestate at %dWatts\n", millis(), _I.name, fnParam);
        // Define the parameters to be tracked
        int32_t Gdes = 5500; // 
        int32_t blo = wattage *-1;   // Minimal Charge and discharge of battery
        int32_t bup = wattage *-1;  // Maximum Charge and Discharge of battery
        int32_t Gdzup = 5500; // Permissible power selling
        int32_t Gdzlo = 5500; // Purchase of power

        // Split the 32-bit integers into 16-bit integers
        uint16_t data[6];
        data[0] = (uint16_t)(Gdes >> 16);  // High word of Gdes
        data[1] = (uint16_t)(Gdes & 0xFFFF); // Low word of Gdes
        data[2] = (uint16_t)(blo >> 16);  // High word of blo
        data[3] = (uint16_t)(blo & 0xFFFF); // Low word of blo
        data[4] = (uint16_t)(bup >> 16);  // High word of bup
        data[5] = (uint16_t)(bup & 0xFFFF); // Low word of bup
        data[6] = (uint16_t)(Gdzup >> 16);  // High word of Gdzup
        data[7] = (uint16_t)(Gdzup & 0xFFFF); // Low word of Gdzup
        data[8] = (uint16_t)(Gdzlo >> 16);  // High word of Gdzlo
        data[9] = (uint16_t)(Gdzlo & 0xFFFF); // Low word of Gdzlo

        uint8_t result;
        for (int i = 0; i < 6; i++) {
			this->Console->printf("Register 0x%04x with value DEC %d HEX 0x%04x \n",(0x1187+i),data[i],data[i]);
            SofarComm::_Modbus.setTransmitBuffer(i, data[i]);
        }
        result = SofarComm::_Modbus.writeMultipleRegisters(0x1187, 6);
        
        if (result == SofarComm::_Modbus.ku8MBSuccess) {
            this->Console->printf("%d [SOFAR_%s] Success on Command 0x%04x@%d\n", millis(), _I.name, fnCode, _I.modbusId);
        } else {
            this->Console->printf("%d [SOFAR_%s] Error on Command 0x%04x@%d with error 0x%04x\n", millis(), _I.name, fnCode, _I.modbusId, result);
            this->Console->println(result);
        }
   // }
}

*/
/*    
		
		global var outside function use this data
					
					batteryCloudData.calc_gridload <float>
					batteryCloudData.calc_batteryload <float>
					batteryCloudData.calc_solarload <float>
					batteryCloudData.d_soc <int>;
					batteryCloudData.cur_tariff <float>;

					inverterData.cur_soc <int>

					timeClient check if defined outside function otherwise  vTaskDelay((5000) / portTICK_PERIOD_MS) then continue;

		*/

void SofarComm::setBatteryManagement(void* pvParameters) {
    SofarComm *_this = (SofarComm *) pvParameters;

    for (;;) {
        String cmd;
        int fnCode = 0;
        int fnParam; 


		//_this->setMode("PassiveManual");
        // Ensure NTPClient is initialized and updated successfully before continuing
        /*if (_this->_timeClient->getMinutes() > 0) {
			_this->Console->printf("%d [SOFAR_%s] Time Client is zero, backoff 5s\n", millis(), _this->_I.name);
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            continue;
        }
		if (_this->_batteryCloudData->timestamp + 60000 > millis()){
			_this->Console->printf("%d [SOFAR_%s] No CloudData received yet, backoff 5s\n", millis(), _this->_I.name);
			vTaskDelay(5000 / portTICK_PERIOD_MS);
			continue;
		}

		if (_this->_inverterData->timestamp + 60000 > millis()){
			_this->Console->printf("%d [SOFAR_%s] No inverterData received yet, backoff 5s\n", millis(), _this->_I.name);
			vTaskDelay(5000 / portTICK_PERIOD_MS);
			continue;
		}*/

        // Get the current time from the NTP client
        int currentMinutes = _this->_timeClient->getMinutes(); // Get the current minute of the hour
        int remainingMinutes = 60 - currentMinutes;   // Calculate how many minutes are left in this hour

        // Retrieve the current and target SoC (in percentage)
        float currentSoC = static_cast<float>(_this->_inverterData->battery_soc);  // Current SoC
        float targetSoC = static_cast<float>(_this->_batteryCloudData->d_soc);    // Target SoC for the next hour
        float batteryCapacity = static_cast<float>(_this->_batteryCloudData->battery_capacity); // Battery capacity in watt-hours (example)

        // Calculate the amount of energy (in Wh) needed to reach the target SoC
        float requiredEnergyWh = ((targetSoC - currentSoC) / 100) * batteryCapacity;

        // Calculate the power required for the remaining minutes in the hour
        float powerForRemainingTime = requiredEnergyWh * (60 / remainingMinutes);  // Adjust power based on remaining minutes

        // Get the available solar energy in Watts
        int solarEnergyAvailable = _this->_batteryCloudData->calc_solarload;

		_this->Console->printf("%d [SOFAR_%s] Current->Dest SOC %.2f->%.2f, requires charge of %.2f Wh, total cap: %.2f\n, remaining min in hour %d",millis(), _this->_I.name,currentSoC,targetSoC,requiredEnergyWh,batteryCapacity,remainingMinutes);
		_this->Console->printf("---> %.2f <---\n",powerForRemainingTime);	
        // Determine the power to be sent to or drawn from the battery
        int powerToBattery = 0;
        if (requiredEnergyWh > 0) {  // Charging is required
            //powerToBattery = min(static_cast<int>(powerForRemainingTime), solarEnergyAvailable);  // Prioritize solar energy
			 powerToBattery = static_cast<int>(powerForRemainingTime);
			 //powerToBattery = powerToBattery * -1;
        } else if (requiredEnergyWh < 0) {  // Discharging is required
            //powerToBattery = max(static_cast<int>(powerForRemainingTime), static_cast<int>(_this->_batteryCloudData->calc_batteryload));  // Discharge based on load
			powerToBattery = static_cast<int>(powerForRemainingTime);
			//powerToBattery = powerToBattery * -1;
        }

		_this->Console->printf("--_this->lastPowerToBattery--> %d <---\n",_this->lastPowerToBattery);	
		_this->Console->printf("--powerToBattery--> %d <---\n",powerToBattery);	
		

		
        // Update the command based on the calculated power to charge or discharge
        if (powerToBattery > 0) {
            cmd = "Desired Power Usage (obtain from battery)";
            fnCode = SOFAR_PAS_MAXBATPWR;
            fnParam = min(powerToBattery, MAX_POWER);
            _this->Console->printf("%d [SOFAR_%s] Set battery charging with %d Watts\n", millis(), _this->_I.name, fnParam);
        } else if (powerToBattery < 0) {
            cmd = "Desired Power Charge (charge to battery)";
            fnCode = SOFAR_PAS_MAXBATPWR;
            fnParam = max(powerToBattery, -MAX_POWER);
            _this->Console->printf("%d [SOFAR_%s] Set battery discharge with %d Watts\n", millis(), _this->_I.name, fnParam * -1);
        } else {
            _this->Console->printf("%d [SOFAR_%s] No battery operation required at this moment\n", millis(), _this->_I.name);
			fnCode = SOFAR_PAS_MAXBATPWR;
			fnParam = 0;
		}

		if (_this->lastPowerToBattery == fnParam){
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			continue;
		} else {
			_this->lastPowerToBattery = fnParam;
			_this->_batteryCloudData->requested_batteryload = static_cast<float>(fnParam);
		}

        // Define the parameters to be tracked
        int32_t Gdes = 5500; 
        int32_t blo = fnParam;// * -1;   // Minimal Charge and discharge of battery
        int32_t bup = fnParam;// * -1;  // Maximum Charge and Discharge of battery
        int32_t Gdzup = 5500; // Permissible power selling
        int32_t Gdzlo = 5500; // Purchase of power

         // Split the 32-bit integers into 16-bit integers
        uint16_t data[6];
        data[0] = (uint16_t)(Gdes >> 16);  // High word of Gdes
        data[1] = (uint16_t)(Gdes & 0xFFFF); // Low word of Gdes
        data[2] = (uint16_t)(blo >> 16);  // High word of blo
        data[3] = (uint16_t)(blo & 0xFFFF); // Low word of blo
        data[4] = (uint16_t)(bup >> 16);  // High word of bup
        data[5] = (uint16_t)(bup & 0xFFFF); // Low word of bup
        data[6] = (uint16_t)(Gdzup >> 16);  // High word of Gdzup
        data[7] = (uint16_t)(Gdzup & 0xFFFF); // Low word of Gdzup
        data[8] = (uint16_t)(Gdzlo >> 16);  // High word of Gdzlo
        data[9] = (uint16_t)(Gdzlo & 0xFFFF); // Low word of Gdzlo

        uint8_t result;
        for (int i = 0; i < 6; i++) {
			_this->Console->printf("Register 0x%04x with value DEC %d HEX 0x%04x \n",(0x1187+i),data[i],data[i]);
            _this->_Modbus.setTransmitBuffer(i, data[i]);
        }
        result = _this->_Modbus.writeMultipleRegisters(0x1187, 6);
        
        if (result == _this->_Modbus.ku8MBSuccess) {
            _this->Console->printf("%d [SOFAR_%s] Success on Command 0x%04x@%d\n", millis(), _this->_I.name, fnCode, _this->_I.modbusId);
        } else {
            _this->Console->printf("%d [SOFAR_%s] Error on Command 0x%04x@%d with error 0x%04x\n", millis(), _this->_I.name, fnCode, _this->_I.modbusId, result);
            _this->Console->println(result);
        }

        // Delay to run this loop every second
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}




int SofarComm::sendPassiveCmd(uint8_t id, uint16_t cmd, uint16_t param)
{
	//modbusResponse	rs;
	//uint8_t	frame[] = { id, SOFAR_FN_PASSIVEMODE, cmd >> 8, cmd & 0xff, param >> 8, param & 0xff, 0, 0 };
	int		err = -1;
	String		retMsg;
	uint8_t result;
	
	result = SofarComm::_Modbus.writeSingleRegister(cmd, param);
	if(result == SofarComm::_Modbus.ku8MBSuccess)	{
		retMsg = true;
		this->Console->printf("%d [SOFAR_%s] Suc6 on Command 0x%04x@%d \n",millis(),_I.name, cmd, id );
	} else 	{
		retMsg = result;
		this->Console->printf("%d [SOFAR_%s] Err on Command 0x%04x@%d with error 0x%04x\n",millis(),_I.name, cmd, id, retMsg  );
		err = 0;
	}
	
	return err;
}

//SOFAR

int SofarComm::addStateInfo(String &state, uint16_t reg, float multiplier, const char* Unit, const char* mqttname, ValueType value_type) {
    float val;
	int len;
	len = 1;
    uint8_t result;
	if (value_type == INT32 || value_type == UINT32 || value_type == FLOAT){
		len = 2;
	}
    result = SofarComm::_Modbus.readHoldingRegisters(reg, len);
    if (result == SofarComm::_Modbus.ku8MBSuccess) {
        if (!(state == "{")) {
            state += ",";
        }
        switch (value_type) {
            case UINT32: {  
                uint32_t raw_val = ((uint32_t)SofarComm::_Modbus.getResponseBuffer(1) << 16) | SofarComm::_Modbus.getResponseBuffer(0);
                val = multiplier > 0 ? raw_val / multiplier : raw_val;
                break;
            }
			case INT32: {
                int32_t raw_val = ((int32_t)SofarComm::_Modbus.getResponseBuffer(1) << 16) | SofarComm::_Modbus.getResponseBuffer(0);
                if (multiplier > 0) {
                    val = raw_val / multiplier;
                } else {
                    val = raw_val;
                }
                break;
            }
            
		   case UINT16: {
                uint16_t raw_val = SofarComm::_Modbus.getResponseBuffer(0);
                if (multiplier > 0) {
                    val = raw_val / multiplier;
                } else {
                    val = raw_val;
                }
                break;
            }
            case INT16: {
                int16_t raw_val = (int16_t)SofarComm::_Modbus.getResponseBuffer(0);
                if (multiplier > 0) {
                    val = raw_val / multiplier;
                } else {
                    val = raw_val;
                }
                break;
            }
            case FLOAT: {
                uint32_t raw_val = ((uint32_t)SofarComm::_Modbus.getResponseBuffer(1) << 16) | SofarComm::_Modbus.getResponseBuffer(0);
                val = *((float*)&raw_val);
                break;
            }
        }
        state += "\"" + String(mqttname) + "\":" + String(val);
        //this->Console->printf("Stateinfo: %s = %.2f%s\n", mqttname, val, Unit);
		this->setFieldValue(mqttname, val);
    }
    return 0;
}




void SofarComm::sendDataCallback(void* pvParameters) {
    SofarComm *_this = (SofarComm *) pvParameters;

    for (;;) {
        bool sc = *_this->_SerialClaimed;
        *_this->_SerialClaimed = false;
        _this->Console->print(sc);
        long start = millis();
        if (_this->_I.conn_status > 0 && sc == false) {
            *_this->_SerialClaimed = true;
            int l;
            String state = "{";

            for (l = 0; l < sizeof(mqtt_status_reads) / sizeof(struct mqtt_status_register); l++) {
                _this->addStateInfo(state, mqtt_status_reads[l].regnum, mqtt_status_reads[l].multiplier, mqtt_status_reads[l].unit, mqtt_status_reads[l].mqtt_name, mqtt_status_reads[l].value_type);

                vTaskDelay(500 / portTICK_PERIOD_MS); // delay 500ms between tasks
            }

            state += "}";
            _this->Console->print(state);

            _this->_I.jsonbuffer = state;
            *_this->_SerialClaimed = false;
        }
        vTaskDelay((5000) / portTICK_PERIOD_MS);
    }
}


void SofarComm::batterySaveCallback(void* pvParameters){
	SofarComm *_this = (SofarComm *) pvParameters;
	for(;;){
		//_this->Console->printf("\n\nBS%d{",_this->_I.modbusId);
		bool sc = *_this->_SerialClaimed;
		//*_this->Console.print(sc);
		long start = millis();
		if(_this->_I.BatterySaveMode && _this->_I.conn_status > 0 && sc == false) {
			*_this->_SerialClaimed = true;
			//Get grid power
			unsigned int	p = 0;
			int8_t result;
			result = _this->_Modbus.readHoldingRegisters(SOFAR_REG_GRIDW, 1);
			if(result == _this->_Modbus.ku8MBSuccess)	{
				p = _this->_Modbus.getResponseBuffer(0);
				_this->Console->print(p);
				_this->Console->printf("%d [SOFAR_%s] Inverter at %02d,Grid power %0dW\n",millis(),_this->_I.name,_this->_I.modbusId ,p);
			} else {
				_this->Console->printf("%d [SOFAR_%s] Inverter at %02d unavailable, to obtain grid power, backoff 5s\n",millis(),_this->_I.name,_this->_I.modbusId);
				vTaskDelay( 5000 / portTICK_PERIOD_MS);
				*_this->_SerialClaimed = false;
				continue;
			}
				
			// Switch to auto when any power flows to the grid.
			// We leave a little wriggle room because once you start charging the battery,
			// gridPower should be floating just above or below zero.
			/*if((p < 65535 || p > 65525) && (_this->_I.RunningState != discharging))
			{
				//exporting to the grid
				result = _this->_Modbus.writeMultipleRegisters(SOFAR_REG_OPERSTATE,0);
				if(result == _this->_Modbus.ku8MBSuccess)	{
					_this->Console->printf("%d [SOFAR_%s] Battery Save Mode: Switched to mode AUTO\n",millis(),_this->_I.name );
				}
			} else 	{
				//importing from the grid
				result = _this->_Modbus.writeMultipleRegisters(SOFAR_FN_STANDBY,SOFAR_PARAM_STANDBY);
				if(result == _this->_Modbus.ku8MBSuccess)	{
					_this->Console->printf("%d [SOFAR_%s] Battery Save Mode: Switched to mode STANDBY\n",millis(),_this->_I.name );
				}
			}*/
			
		}
		*_this->_SerialClaimed = false;
		long end = millis();
		//_this->Console->printf("}BS%d\n\n",_this->_I.modbusId);
		vTaskDelay((BATTERYSAVE_INTERVAL-(end-start)) / portTICK_PERIOD_MS);
	}
}


unsigned int SofarComm::batteryWatts(){ 
	if(_I.RunningState == charging || _I.RunningState == discharging)
	{
		uint8_t result;
		result = SofarComm::_Modbus.readHoldingRegisters(SOFAR_REG_BATTW, 1);
		if(result == SofarComm::_Modbus.ku8MBSuccess)	{
			unsigned int w = SofarComm::_Modbus.getResponseBuffer(0);

			switch(_I.RunningState)
			{
				case charging:
					w = w*10;
					break;

				case discharging:
					w = (65535 - w)*10;
					//w = w*-10;
			}

			return w;
		} else  {
			this->Console->printf("%d [SOFAR_%s] Inverter unavailable at %d, to obtain battery stats\n",millis(),this->_I.name,this->_I.modbusId);
			//_this->Console->printf("%s: CRC-FAULT %s",_I.name ,(String)response.errorMessage);
			//this->UpdateInvStatus("NULL", "NULL", "CRC-FAULT", "NULL");
		}
	}

	return 0;
}

void SofarComm::RunStateCallback(void* pvParameters){
	SofarComm *_this = (SofarComm *) pvParameters;
	
	for(;;){
		//_this->Console->printf("\n\nRS%d{",_this->_I.modbusId);
		long start = millis();
		bool sc = *_this->_SerialClaimed;
		_this->_I.conn_status = 1;
		//*_this->Console.print(sc);
		if(_this->_I.conn_status > 0 && sc == false) {
			char buffer[40];
			uint8_t result;
			*_this->_SerialClaimed = true;
			result = _this->_Modbus.readHoldingRegisters(SOFAR_REG_OPERSTATE, 1);
			
			//*_this->Console.println(result);
			_this->Console->printf("%d [SOFAR_%s] Get runstate at Modbus addr %d... ",millis(),_this->_I.name,_this->_I.modbusId);
			if(result == _this->_Modbus.ku8MBSuccess)	{
				_this->_I.RunningState = _this->_Modbus.getResponseBuffer(0);
				
				//_this->Console->printf("RUNSTATE %d\n",_this->_I.RunningState);
				switch(_this->_I.RunningState)	{
					case SOFAR_OPER_WAITING:
						/*if (_this->_I.BatterySaveMode){
							_this->Console->printf("Battery Save\n");
							_this->UpdateInvStatus(String("Battery Save"),1);
						} else {
							_this->Console->printf("Standby\n");
							_this->UpdateInvStatus(String("Standby"),1);
						}*/
						_this->Console->printf("Standby\n");
					break;

					case SOFAR_OPER_GRIDCONN:
						_this->Console->printf("Grid Connected\n");
						//_this->UpdateInvStatus(String("Checking"),1);
						
						//this->UpdateInvStatus("NULL", "NULL", "Checking", "NULL");
					break;

					case SOFAR_OPER_DETECT:
						_this->Console->printf("Checking\n");
						//_this->UpdateInvStatus(String("Checking"),1);
						
						//this->UpdateInvStatus("NULL", "NULL", "Checking", "NULL");
					break;

					case SOFAR_OPER_SELFCHARGE:
						_this->Console->printf("Charging %0.0dW\n",_this->batteryWatts());
						//sprintf(buffer,"BATT +%0.0dW",_this->batteryWatts());
						//_this->UpdateInvStatus(String(buffer),1);
						//this->UpdateInvStatus("NULL", "NULL", HUMAN_CHARGING, String(batteryWatts())+"W");
					break;

					/*case discharging:
						_this->Console->printf("Discharging %0.0dW\n",_this->batteryWatts());
						sprintf(buffer,"BATT -%0.0dW",_this->batteryWatts());
						_this->UpdateInvStatus(String(buffer),1);
						//this->UpdateInvStatus("NULL", "NULL", HUMAN_DISCHARGING, String(batteryWatts())+"W");
					break;*/
/*
					case epsState:
						_this->Console->printf("EPS State\n");
						_this->UpdateInvStatus(String("EPS State"),1);
						//this->UpdateInvStatus("NULL", "NULL", "EPS State", "NULL");
					break;
*/

					case SOFAR_OPER_RECFAULT:
						_this->Console->printf("Recoverable Fault\n");
						_this->UpdateInvStatus(String("Fault"),1);
						//this->UpdateInvStatus("NULL", "NULL", "FAULT", "NULL");
					break;

					case SOFAR_OPER_PERMFAULT:
						_this->Console->printf("Permanent Fault\n");
						_this->UpdateInvStatus(String("Fault"),1);
						//this->UpdateInvStatus("NULL", "NULL", "FAULT", "NULL");
					break;

					case SOFAR_OPER_UPGRADE:
						_this->Console->printf("Upgrade in Progress\n");
						//_this->UpdateInvStatus(String("Perm Fault"),1);
						//this->UpdateInvStatus("NULL", "NULL", "PERMFAULT", "NULL");
					break;

					default:
						_this->Console->printf("Unknown\n");
						_this->UpdateInvStatus(String("UNKNOWN?"),1);
						//this->UpdateInvStatus("NULL", "NULL", "Runstate?", "NULL");
					break;
				}
				*_this->_SerialClaimed = false;
			} else {
				_this->Console->printf(" unavailable, backoff 5s\n");
				*_this->_SerialClaimed = false;
				vTaskDelay( 5000 / portTICK_PERIOD_MS);
				continue;
			}
			*_this->_SerialClaimed = false;
		} else {
			//_this->Console->printf("%d [SOFAR] - %s @%02d Inverter comm postphoned, serial unavailable, backoff 500ms\n",millis(),_this->_I.name,_this->_I.modbusId);
			vTaskDelay( 5000 / portTICK_PERIOD_MS);
			continue;
		}

		*_this->_SerialClaimed = false;
		long end = millis();
		//_this->Console->printf("}RS%d\n\n",_this->_I.modbusId);
		vTaskDelay((RUNSTATE_INTERVAL) / portTICK_PERIOD_MS);
	} 
}



void SofarComm::HeartbeatCallback(void* pvParameters){
	SofarComm *_this = (SofarComm *) pvParameters;
	
	for(;;){
		long start = millis();
		bool sc = *_this->_SerialClaimed;
		if(sc == false && _this->_I.conn_status == 0) {
			*_this->_SerialClaimed = true;
			if(_this->_Modbus.readHoldingRegisters(SOFAR_REG_SERIAL, 1) == _this->_Modbus.ku8MBSuccess )	{
				*_this->_SerialClaimed = false;
				//online
				_this->Console->printf("%d [SOFAR] - Response from Inverter %s at Modbus addr %d\n",millis(),_this->_I.name,_this->_I.modbusId);
				_this->_I.conn_status = 1;
				vTaskDelay((HEARTBEAT_INTERVAL*9) / portTICK_PERIOD_MS);
				continue;
			}
			else
			{
				*_this->_SerialClaimed = false;
				//offline
				_this->Console->printf("%d [SOFAR] - --NO-- Response from Inverter %s at Modbus addr %d\n",millis(),_this->_I.name,_this->_I.modbusId);
				_this->_I.conn_status == 0;
				continue;
			}	
		
		}
		vTaskDelay((500) / portTICK_PERIOD_MS);
	}
}
