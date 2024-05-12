#include "LedControl.h"
#include "binary.h"

//-------------------------Start--------------------------------
// Matrix Display Portion
// Define hardware type, size, and output pins:
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1
// Setup for software SPI:
#define DIN_PIN 6
#define CS_PIN 5
#define CLK_PIN 4
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);
volatile int brightness = 0;
volatile int brightness_counter = 0;

// Matrix Display Portion
//-------------------------End----------------------------------

void setup(){
  Serial.begin(115200);
  lc.shutdown(0,false);
  // Set brightness to a medium value
  lc.setIntensity(0,brightness);
  // Clear the display
  lc.clearDisplay(0);
}

void loop(){
    fill_display();
    Serial.println("haha");
}

void fill_display(){
  for (int i=0; i<=7; i++){
    lc.setRow(0,i,B11111111);
  }
}
