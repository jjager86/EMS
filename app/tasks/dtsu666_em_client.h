#ifndef TASK_DTSU666_EM_CLIENT
#define TASK_DTSU666_EM_CLIENT


#include "header.h"
#include <ModbusMaster.h>

//DTSU666 Meter interface

#define DTSU666_RX_PIN 19
#define DTSU666_TX_PIN 18
#define DTSU666_MODBUSADDRESS 1


EspSoftwareSerial::UART DTSU666Serial;

ModbusMaster DTSU666mb;



void getEnergyStatsFromDTSU666_cb(void* pvParameters){
	
	for(;;){
    

    
        //Console0.printf("\n\n{Meterdata");
        if (DTSU666mb.readHoldingRegisters(0x2012, 2) != 0){
          Console0.printf("%d [DTSU666] Meter unavailable, probe again\n",millis());
          vTaskDelay( 200 / portTICK_PERIOD_MS);
          continue;
        };
        meterdata Meterdata;
      
        
        float2int.int1 = DTSU666mb.getResponseBuffer(0);
        float2int.int0 = DTSU666mb.getResponseBuffer(1);
        Meterdata.pt = float2int.value/10;
        /*float2int.int1 = DTSU666mb.getResponseBuffer(2);
        float2int.int0 = DTSU666mb.getResponseBuffer(3);
        Meterdata.pa = float2int.value/10;
        float2int.int1 = DTSU666mb.getResponseBuffer(4);
        float2int.int0 = DTSU666mb.getResponseBuffer(5);
        Meterdata.pb = float2int.value/10;
        float2int.int1 = DTSU666mb.getResponseBuffer(6);
        float2int.int0 = DTSU666mb.getResponseBuffer(7);
        Meterdata.pc = float2int.value/10;*/
        //Meterdata.pa = 300,33;
        //Meterdata.pb = 400,33;
        //Meterdata.pc = 500,33;
        //Meterdata.pt = 1500.55;
        Meterdata.lastvalue = millis();
        //get CTR Ratio
        //if (DTSU666mb.readHoldingRegisters(0x0006, 1) != 0){
        //  Meterdata.ctr = DTSU666mb.getResponseBuffer(0);
        //}
        Meterdata.ctr = 1;
        meterdata_obj = Meterdata;
  //Console0.printf("%d - METER: %10.3f,%10.3f,%10.3f,%10.3f CTR %d\n",millis(),meterdata_obj.pt,meterdata_obj.pa,meterdata_obj.pb,meterdata_obj.pc,meterdata_obj.ctr);
          
        
        /*if( MeterDataQueue != NULL ) {
          //struct meterdata *pxMeterdata;
          //pxMeterdata = & Meterdata;

          //Console0.print(pxMeterdata->pt);
          
          xQueueOverwrite(MeterDataQueue, (void *) &Meterdata);
          
        } else {
          //Console0.print("Failure? ->MeterDataQueue = 0\n");
          Console0.printf("%d [DTSU666] No MeterDataQueue AVAILABLE\n",millis());
        }*/
        
      // Console0.printf("}Meterdata\n\n");
      //dtsu_update_requested = false;
        vTaskDelay( 200 / portTICK_PERIOD_MS);
      }
 
  }

void setup_DTSU666_em_client(){
  DTSU666Serial.begin(9600,EspSoftwareSerial::SWSERIAL_8N1, DTSU666_RX_PIN, DTSU666_TX_PIN, false,64);
 	DTSU666mb.begin(DTSU666_MODBUSADDRESS, DTSU666Serial);
Console0.printf("Struct size %d", sizeof(struct meterdata ));
  MeterDataQueue = xQueueCreate ( 1, sizeof(struct meterdata ) );
    if( MeterDataQueue == NULL ) { Console0.printf("%d [DTSU666] MeterDataQueue Creation Failure!\n",millis());  }


  xTaskCreate(
      getEnergyStatsFromDTSU666_cb,           // Task function
      "DTSU666MeterTask",               // Task name
      4096,                  // Stack size (bytes)
      NULL,                   // Task parameter
      50,                      // Task priority
      NULL                   // Task handle
      
  );
  	Console0.printf("%d [DTSU666] Enabled DTSU666 Meterclient task\n",millis());
#if HA_ENABLED == true
  xTaskCreate(
      sendMeterData2HA,
      "MQTT-HA Meterdata",
      4000,
      NULL,
      3,
      NULL
  );
  #endif

}

#endif
