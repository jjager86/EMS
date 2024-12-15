#ifndef TASK_VIRTUALDSSU666
#define TASK_VIRTUALDSSU666

#include "header.h"
#include <ModbusSlave.h>

#define DSSU666_1_RX_PIN 33//36//39
#define DSSU666_1_TX_PIN 32//39//36
#define DSSU666_2_RX_PIN 34
#define DSSU666_2_TX_PIN 35
#define DSSU666_MODBUSADDRESS 1

extern meterdata meterdata_obj;

EspSoftwareSerial::UART DSSU666Serial_1;
//EspSoftwareSerial::UART DSSU666Serial_2;

Modbus DSSU666mb_1(DSSU666Serial_1);
//Modbus DSSU666mb_2(DSSU666Serial_2);


uint8_t  cb_sendPowerdataInv_total(uint8_t fc, uint16_t address, uint16_t length) {
    int status = 6;

		//Console0.printf("%d > %d = true , then enable virtual meter\n",(meterdata_obj.lastvalue + 5000),millis());
	if (meterdata_obj.lastvalue + 5000 > millis()){
		//Console0.printf("%d [vDSSU666] Register %d\n",millis(),address);
		if (address == 6){
			//Current transformer ratio
			//Console0.printf("%d [vDSSU666] Total: Current transformer ratio\n",millis());
			DSSU666mb_1.writeRegisterToBuffer(0, (1));
			status = 0;
		}
		
		if (address == 11){
		//	Console0.printf("%d [vDSSU666] Total: write metertype\n",millis());
			//write metertype
			DSSU666mb_1.writeRegisterToBuffer(0, (10));
			status = 0;
		}

		if (address == 8210){
			/*if ((meterdata_obj.pt/2) > (230*25)){
				float2int.value = ((meterdata_obj.pa*10))*-1;
				Console0.printf("%d [vDSSU666] Calculated Power Exceeds 25A! reports phase power (calc:%10.3fW / phase A: %10.3fW)\n",millis(),(meterdata_obj.pt/2),(meterdata_obj.pa)); 
			} else {
				float2int.value = ((meterdata_obj.pt*10))*-1;
			}*/
			//float2int.value = ((meterdata_obj.pt*10))*-1;
float2int.value = ((meterdata_obj.pt*10)*-1 );
//			float2int.value = (((meterdata_obj.pt*10)+ (power_setpoint.setpoint*10)) *-1 );
			//Console0.printf("%d [vDSSU666] Reporting power %4.3fW with offset %4.3fW - netto %4.3fW\n",millis(),(meterdata_obj.pt),power_setpoint.setpoint,float2int.value);
			DSSU666mb_1.writeRegisterToBuffer(0, (float2int.int1));
			DSSU666mb_1.writeRegisterToBuffer(1, (float2int.int0));
			status = 0;
			
		}
	} 
   return status;
}
/*
uint8_t  cb_sendPowerdataInv_pha(uint8_t fc, uint16_t address, uint16_t length) {
    int status = 6;
	//Console0.printf("%d > %d = true , then enable virtual meter\n",(meterdata_obj.lastvalue + 5000),millis());
	if (meterdata_obj.lastvalue + 5000 > millis()){
		Console0.printf("%d [vDSSU666] Register %d\n",millis(),address);
		if (address == 6){
			//Current transformer ratio
			Console0.printf("%d [vDSSU666] A: Current transformer ratio\n",millis());
			DSSU666mb_1.writeRegisterToBuffer(0, (1));
			status = 0;
		}
		
		if (address == 11){
			Console0.printf("%d [vDSSU666] A: write metertype\n",millis());
			//write metertype
			DSSU666mb_1.writeRegisterToBuffer(0, (10));
			status = 0;
		}

		if (address == 8210){
			if ((meterdata_obj.pt/2) > (230*25)){
				float2int.value = ((meterdata_obj.pa*10))*-1;
				Console0.printf("%d [vDSSU666] Calculated Power Exceeds 25A! reports phase power (calc:%10.3fW / phase A: %10.3fW)\n",millis(),(meterdata_obj.pt/2),(meterdata_obj.pa)); 
			} else {
				float2int.value = ((meterdata_obj.pt*10))*-1;
			}
			//Console0.printf("%d [vDSSU666] Calculated Power power (Phase Total:%4.3fW / phase A: %4.3fW) reporting power %4.3fW\n",millis(),(meterdata_obj.pt),(meterdata_obj.pa),(float2int.value/10));
			DSSU666mb_1.writeRegisterToBuffer(0, (float2int.int1));
			DSSU666mb_1.writeRegisterToBuffer(1, (float2int.int0));
			status = 0;
			
		}
	} 
   return status;
}

*/

/*
uint8_t  cb_sendPowerdataInv_phb(uint8_t fc, uint16_t address, uint16_t length) {
    int status = 6;
	//Console0.printf("%d > %d = true , then enable virtual meter\n",(meterdata_obj.lastvalue + 5000),millis());
	if (meterdata_obj.lastvalue + 5000 > millis()){
		Console0.printf("%d [vDSSU666] Register %d\n",millis(),address);
		if (address == 6){
			//Current transformer ratio
			Console0.printf("%d [vDSSU666] B: Current transformer ratio\n",millis());
			DSSU666mb_2.writeRegisterToBuffer(0, (1));
			status = 0;
		}
		
		if (address == 11){
			//write metertype
			Console0.printf("%d [vDSSU666] B: write metertype\n",millis());
			DSSU666mb_2.writeRegisterToBuffer(0, (10));
			status = 0;
		}

		if (address == 8210){
			if ((meterdata_obj.pt/2) > (230*25)){
				float2int.value = ((meterdata_obj.pb*10))*-1;
				Console0.printf("%d [vDSSU666] Calculated Power Exceeds 25A! reports phase power (calc:%10.3fW / phase B: %10.3fW)\n",millis(),(meterdata_obj.pt/2),(meterdata_obj.pb)); 
			} else {
				float2int.value = ((meterdata_obj.pt*10))*-1;
			}
			//Console0.printf("%d [vDSSU666] Calculated Power power (Phase Total:%4.3fW / phase B: %4.3fW) reporting power %4.3fW\n",millis(),(meterdata_obj.pt),(meterdata_obj.pb),(float2int.value/10));
			DSSU666mb_2.writeRegisterToBuffer(0, (float2int.int1));
			DSSU666mb_2.writeRegisterToBuffer(1, (float2int.int0));
			status = 0;
			
		}
	} 
   return status;
}
*/

void DSSU666mb_1_cb(void* pvParameters){
	for(;;){
		DSSU666mb_1.poll();
		//Console0.printf("%d [DSSU666] - %d Modbus Poll\n",millis(),1);
		vTaskDelay( 5 / portTICK_PERIOD_MS);
	}
}
/*
void DSSU666mb_2_cb(void* pvParameters){
	for(;;){
		DSSU666mb_2.poll();
		//Console0.printf("%d [DSSU666] - %d Modbus Poll\n",millis(),2);
		vTaskDelay( 05 / portTICK_PERIOD_MS);
	}
	
}
*/

void setup_DSSU666(){

	
	DSSU666Serial_1.begin(9600,EspSoftwareSerial::SWSERIAL_8N1, DSSU666_1_RX_PIN, DSSU666_1_TX_PIN);
	//DSSU666Serial_2.begin(9600,EspSoftwareSerial::SWSERIAL_8N1, DSSU666_2_RX_PIN, DSSU666_2_TX_PIN, false,128);

    DSSU666Serial_1.enableIntTx(false);
    //DSSU666Serial_2.enableIntTx(false);

	DSSU666mb_1.setUnitAddress(DSSU666_MODBUSADDRESS);
  	DSSU666mb_1.cbVector[CB_READ_HOLDING_REGISTERS] = cb_sendPowerdataInv_total;
	//DSSU666mb_2.setUnitAddress(DSSU666_MODBUSADDRESS);
  	//DSSU666mb_2.cbVector[CB_READ_HOLDING_REGISTERS] = cb_sendPowerdataInv_phb;


	DSSU666mb_1.begin(9600);
	
	DSSU666mb_1.enable();
	//DSSU666mb_2.begin(9600);
	//DSSU666mb_2.enable();
	Console0.printf("%d [vDSSU666] Enabled Meter Virtualisation Tasks\n",millis());





	xTaskCreatePinnedToCore(
      DSSU666mb_1_cb,           // Task function
      "PollSP1",               // Task name
   	4096,                  // Stack size (bytes)
      NULL,                   // Task parameter
      10,                      // Task priority
      NULL,
	  0                     // Task handle
	);
	/*xTaskCreatePinnedToCore(
      DSSU666mb_2_cb,           // Task function
      "Poll serialport 2",               // Task name
      4096,                  // Stack size (bytes)
      NULL,                   // Task parameter
      10,                    // Task priority
      NULL,
	  0                  // Task handle
    );*/
}



  
 //   DSSU666mb_1.poll();
 // DSSU666mb_2.poll();

#endif