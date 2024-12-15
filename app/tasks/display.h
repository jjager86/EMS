#ifndef TASK_DISPLAY
#define TASK_DISPLAY

#define __USE_FreeRTOS__

    #include "header.h"
    #include "SSD1306Wire.h"  
    #include "OLEDDisplayUi.h"

    

#define logo_width 16
#define logo_height 16
const uint8_t img_wifi[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0xC0, 0x03, 0xF8, 0x1F, 0xFE, 0x7F, 0x1F, 0xF8, 
  0xC6, 0x63, 0xF0, 0x0F, 0xF8, 0x1F, 0x10, 0x08, 0xC0, 0x03, 0xC0, 0x03, 
  0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };

const uint8_t img_cloud[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0F, 0xC0, 0x1F, 0xF8, 0x1F, 
  0xFC, 0x1F, 0xFE, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x7F, 
  0xFC, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t img_solar[] PROGMEM = {
	0x00, 0x00, 0xA0, 0x05, 0xE0, 0x07, 0x60, 0x06, 0x30, 0x0C, 0xFC, 0x3F, 
  0xFC, 0x3F, 0x26, 0x64, 0xA6, 0x65, 0xA6, 0x65, 0x07, 0xE0, 0xB7, 0xED, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x07, 0xE0, 0x07,
};

const uint8_t img_meter[] PROGMEM = {
	0x00, 0x00, 0x40, 0x02, 0xE0, 0x07, 0xF8, 0x1F, 0xFC, 0x3F, 0xFC, 0x3F, 
  0xFE, 0x77, 0xFE, 0x7B, 0x7C, 0x3E, 0x7E, 0x7E, 0xFE, 0x7F, 0xFE, 0x7F, 
  0x1C, 0x38, 0xE8, 0x17, 0xE0, 0x07, 0x00, 0x00,
};


    SSD1306Wire display(0x3c,21,22); 
    OLEDDisplayUi ui     ( &display );
 
void mainOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  if(WiFi.status() == WL_CONNECTED){
    display->drawXbm(0, 0, logo_width,logo_height, img_wifi);
  }
 if(ems_apiconfig.status > 1){      
  display->drawXbm(20, 0, logo_width,logo_height, img_cloud);
 } else {
  display->drawXbm(20, 0, logo_width,logo_height, img_cloud);
 }
 // if (meterdata_obj.lastvalue + 10000 > millis()){
 //   display->drawXbm(40, 0, logo_width,logo_height, img_meter);
 // }
  //display->drawXbm(60, 0, logo_width,logo_height, img_solar);
  //display->drawXbm(80, 0, logo_width,logo_height, img_solar);
}

// utility function for digital clock display: prints leading 0
String twoDigits(int digits) {
  if (digits < 10) {
    String i = '0' + String(digits);
    return i;
  }
  else {
    return String(digits);
  }
}


void drawClockFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
 
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  //display->drawString(0,16, timeClient. );
  //display->setFont(ArialMT_Plain_24);
  display->drawString(79,32, timeClient.getFormattedTime() );
  display->drawXbm(10, 64, logo_width,logo_height, img_meter);
}

void drawMeterdata(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  
if (meterdata_obj.lastvalue + 10000 > millis()){
  display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_16);
   char buffer[40];
   sprintf(buffer,"%4.0fW",meterdata_obj.pt);
   display->drawString(70, 30, String(buffer));    
   display->setFont(ArialMT_Plain_10);
   sprintf(buffer,"%4.0fW %4.0fW %4.0fW",meterdata_obj.pa,meterdata_obj.pb,meterdata_obj.pc);
  display->drawString(70, 50, String(buffer)); 
  
} else {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_10);
   display->drawString(79,32, "No Meterdata");  
};
display->drawXbm(5, 64, logo_width,logo_height, img_meter);
}

void drawCloudData(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  
if (batteryCloudData.timestamp + 60000 > millis()){
  display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_16);
   char buffer[40];
   sprintf(buffer,"SOC %d",batteryCloudData.d_soc);
   display->drawString(70, 30, String(buffer));    
   display->setFont(ArialMT_Plain_10);
   //sprintf(buffer,"%4.0fW - %4.0fW",batteryCloudData.calc_batteryload,batteryCloudData.requested_batteryload);
  display->drawString(70, 50, String(buffer)); 
  
} else {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_10);
   display->drawString(79,32, "No CloudData for 60s");  
};
display->drawXbm(5, 64, logo_width,logo_height, img_meter);
}

void drawInverterdata(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
 
 display->setTextAlignment(TEXT_ALIGN_CENTER);
   display->setFont(ArialMT_Plain_10);
   
  display->drawString(79,32, String(millis()));
  display->drawXbm(10, 64, logo_width,logo_height, img_meter);

}

FrameCallback frames[] = { drawClockFrame };
OverlayCallback overlays[] = { mainOverlay };


void displayUITask(void * parameter){
        for(;;){
         ui.update();
         //Console0.printf("%d [DISPLAY] Display update\n",millis());
         vTaskDelay( 100 / portTICK_PERIOD_MS);
        }
}

 void displaySetup(){
      ui.setTargetFPS(5);
  
  ui.disableAllIndicators();
      
      ui.setFrames(frames, 3);
      ui.setOverlays(overlays, 1);
      ui.setTimePerFrame(10000);
      ui.init();
      display.display();
      ui.setTimePerTransition(0);
      ui.update();
      
       xTaskCreate(
      displayUITask,           // Task function
      "displayUITask",               // Task name
      6000,                  // Stack size (bytes)
      NULL,                   // Task parameter
      2,                    // Task priority
      NULL                  // Task handle
    );
      
      Console0.printf("%d [DISPLAY] Display initialized\n",millis());

    }

        
        
    

    
  

#endif
