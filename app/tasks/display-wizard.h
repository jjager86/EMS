#ifndef TASK_DISPLAY
#define TASK_DISPLAY

#define __USE_FreeRTOS__

#include "header.h"
#include "SSD1306Wire.h"  
#include "OLEDDisplayUi.h"
#include <WiFi.h>

#define logo_width 16
#define logo_height 16
#define MAX_PASSWORD_LENGTH 16
#define BUTTON_PIN 4  // GPIO pin for button input

// Variables for Wi-Fi network selection and password entry
String ssidList[10];  // Store up to 10 networks
int networkCount = 0;
int selectedNetwork = 0;
String password = "";
int currentPosition = 0;
char currentChar = '0';
bool passwordEntered = false;
bool wifiConnected = false;
bool wifiConnecting = false;

// Allowed characters for WPA2 password
char allowedChars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()-_=+[]{}|\\:;\"'<>,.?/";

// Images for overlay
const uint8_t img_wifi[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0xC0, 0x03, 0xF8, 0x1F, 0xFE, 0x7F, 0x1F, 0xF8, 
  0xC6, 0x63, 0xF0, 0x0F, 0xF8, 0x1F, 0x10, 0x08, 0xC0, 0x03, 0xC0, 0x03, 
  0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

SSD1306Wire display(0x3c, 21, 22); 
OLEDDisplayUi ui(&display);

// Function to draw available Wi-Fi networks
void drawWiFiScan(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  // Show scanned networks
  for (int i = 0; i < networkCount && i < 10; i++) {
    if (i == selectedNetwork) {
      display->drawString(0, i * 10, "> " + ssidList[i]);  // Highlight selected network
    } else {
      display->drawString(0, i * 10, "  " + ssidList[i]);
    }
  }
  display->display();
}

// Function to draw password entry screen
void drawPasswordEntry(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  display->drawString(0, 0, "Enter WiFi Pass:");
  display->drawString(0, 20, password + String(currentChar));  // Display current password + current character
  display->display();
}

// Function to display connection status
void drawWiFiStatus(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);

  if (wifiConnected) {
    display->drawString(64, 32, "WiFi Connected!");
  } else if (wifiConnecting) {
    display->drawString(64, 32, "Connecting...");
  } else {
    display->drawString(64, 32, "Failed to Connect!");
  }
  display->display();
}

// Function to scan Wi-Fi networks
void scanWiFiNetworks() {
  networkCount = WiFi.scanNetworks();
  for (int i = 0; i < networkCount && i < 10; i++) {
    ssidList[i] = WiFi.SSID(i);
  }
}

// Function to cycle through Wi-Fi networks using the button
void selectWiFiNetwork() {
  selectedNetwork = (selectedNetwork + 1) % networkCount;  // Cycle through networks
  drawWiFiScan(&display, NULL, 0, 0);  // Redraw network list
}

// Function to handle character cycling with button for password entry
void handlePasswordInput() {
  int charCount = sizeof(allowedChars) - 1;

  // Cycle through allowed characters
  currentChar = allowedChars[(strchr(allowedChars, currentChar) - allowedChars + 1) % charCount];

  // If button is held for a long time, confirm current character
  if (digitalRead(BUTTON_PIN) == LOW && millis() - lastPress > 500) {
    password += currentChar;  // Add the confirmed character to the password
    currentPosition++;

    if (currentPosition >= MAX_PASSWORD_LENGTH) {
      passwordEntered = true;  // Stop if max length is reached
    } else {
      currentChar = '0';  // Reset to first character for next input
    }
  }
}

// Function to connect to Wi-Fi using the selected network and entered password
void connectToWiFi(String ssid, String pass) {
  WiFi.begin(ssid.c_str(), pass.c_str());

  wifiConnecting = true;
  unsigned long startAttemptTime = millis();

  // Wait for connection or timeout
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT) {
    delay(500);
    drawWiFiStatus(&display, NULL, 0, 0);  // Display "Connecting..."
  }

  wifiConnecting = false;
  wifiConnected = (WiFi.status() == WL_CONNECTED);

  drawWiFiStatus(&display, NULL, 0, 0);  // Show the final connection status
}

// Interrupt handler for button press (you'll need to define BUTTON_PIN in your header)
void handleButtonPress() {
  static unsigned long lastPress = 0;
  unsigned long currentPress = millis();

  // Debounce: check if the button was pressed too quickly
  if (currentPress - lastPress > 200) {
    if (!passwordEntered) {
      handlePasswordInput();  // If password not entered, handle password input
    } else {
      selectWiFiNetwork();  // Else, handle Wi-Fi network selection
    }
  }

  lastPress = currentPress;
}

void mainOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  if(WiFi.status() == WL_CONNECTED){
    display->drawXbm(0, 0, logo_width, logo_height, img_wifi);
  }
  // Example additional conditions for other icons
  // if (your_condition_here) {
  //    display->drawXbm(20, 0, logo_width, logo_height, img_some_other_icon);
  // }
}

// Modify the frames array to include Wi-Fi scan and password entry screens
FrameCallback frames[] = { drawWiFiScan, drawPasswordEntry, drawWiFiStatus };
OverlayCallback overlays[] = { mainOverlay };

void displayUITask(void * parameter){
    for(;;){
        ui.update();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void displaySetup() {
    ui.setTargetFPS(5);
    ui.disableAllIndicators();
    // Setup the frames and overlays here
    ui.setFrames(frames, 3);
    ui.setOverlays(overlays, 1);  // Now 'mainOverlay' is correctly in scope
    ui.setTimePerFrame(10000);
    ui.init();
    display.display();
    ui.setTimePerTransition(0);
    ui.update();


    scanWiFiNetworks();  // Scan Wi-Fi networks on setup
    drawWiFiScan(&display, NULL, 0, 0);  // Draw the scanned networks

    xTaskCreate(
        displayUITask,         // Task function
        "displayUITask",       // Task name
        6000,                  // Stack size (bytes)
        NULL,                  // Task parameter
        2,                     // Task priority
        NULL                   // Task handle
    );

    Console0.printf("%d [DISPLAY] Display initialized\n", millis());
}

#endif
