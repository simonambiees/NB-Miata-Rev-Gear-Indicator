#include <FastLED.h>
#include "LedControl.h"
#include "binary.h"

// delay time in ms
#define TIME_BETWEEN_LOOPS 50

//-------------------------Start--------------------------------
// Rev Light Portion
// RPM per Hz
#define RPM_PER_HZ 30

// Cruise Mode threshold time in ms
#define CRUISE_THRESHOLD 10000
// Cruise Mode limit RPMs
#define CRUISE_LOW 3000
#define CRUISE_HIGH 5000

// RPM bar limit RPMs
#define RPM_LOW 2500

// Shift Flashing Warning threshold RPM
#define RPM_FLASH_THRESHOLD 6000

// Gear Ratios
#define GEAR_1 200
#define GEAR_2 120
#define GEAR_3 85
#define GEAR_4 64
#define GEAR_5 52
#define TOLERANCE_PERCENT 10

#define LED_PIN     5
#define NUM_LEDS    11
#define BRIGHTNESS  255
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

CRGB black  = CRGB::Black;
CRGB red  = CRGB::Red;
CRGB green  = CRGB::Green;
CRGB blue  = CRGB::Blue;

CRGB shift_pattern[] = {blue, blue, blue, red, red, red, red, green, green, green};
CRGBPalette16 shift_palette(
  blue, blue, blue, blue, blue,
  red, red, red, red, red, red,
  green, green, green, green, green
);


volatile unsigned long overflowCount;
volatile boolean flashing_on;
volatile int cruise_count = 0;
volatile int rpm = 0;
volatile float speedo = 0.0;
volatile int gear = 0;
volatile int gear_update_counter = 0;
volatile int last_suggested_gear = 0;
// Rev Light Portion
//-------------------------End----------------------------------

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


void setup() {
  Serial.begin(115200);       
  Serial.println("Rev Light & Gear Indicator");

  delay( 2000 ); // power-up safety delay
  
  //-------------------------Start----------------------------
  // Rev Light LED Setup
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  
  blank_led();
  FastLED.show();
  flashing_on = false;
  start_up_sweep();
  blank_led();
  FastLED.show();
  // Rev Light LED Setup
  //-------------------------End------------------------------

  //-------------------------Start----------------------------
  // Freq Counters Setup
  // reset Timer 1
  TCCR1A = 0;
  TCCR1B = 0;
  // Timer 1 - interrupt on overflow
  TIMSK1 = bit (TOIE1);   // enable Timer1 Interrupt
  // zero it
  TCNT1 = 0;  
  overflowCount = 0;  
  // start Timer 1
  TCCR1B =  bit (CS10);  //  no prescaling

  // set up for interrupts
  prepareForInterrupts_input_a();
  prepareForInterrupts_input_b();
  // Freq Counters Setup
  //-------------------------End------------------------------

  //-------------------------Start----------------------------
  // Matrix Display Setup
  setup_matrix_display();
  // Matrix Display Setup
  //-------------------------End------------------------------

  
  //-------------------------Start----------------------------
  // Light Sensing Setup
  pinMode(A0, INPUT);
  // Light Sensing Setup
  //-------------------------End------------------------------

}

void loop() {
  float new_freq_a = update_input_a();
  if (new_freq_a > 0){
    update_speedo(new_freq_a);
  }
  float new_freq_b = update_input_b();
  if (new_freq_b > 0){
    update_rpm(new_freq_b);
  }

  int suggested_gear = calculate_gear();
  if (suggested_gear == last_suggested_gear){
    gear_update_counter++;
  } else {
    gear_update_counter = 0;
  }
  last_suggested_gear = suggested_gear;
  if (gear_update_counter >= 5){
    gear = suggested_gear;
    gear_update_counter = 0;
  }
  display_gear(gear, rpm);
//  Serial.print("RPM:");
//  Serial.print(rpm);
//  Serial.print(",");
//  Serial.print("Speedo:");
//  Serial.print(speedo);
//  Serial.print(",");
//  Serial.print("sug_gear:");
//  Serial.print(suggested_gear);
//  Serial.print(",");
//  Serial.print("gear:");
//  Serial.print(gear);
//  Serial.print(",     ");
//if (new_freq_a > 0){
//  Serial.print ("Frequency_a:");
//
//  Serial.print (new_freq_a);
//  Serial.print ("Frequency_b:");
//  Serial.println (new_freq_b);
//}
//  Serial.print(",");
//  Serial.print ("Frequency_b:");
//  Serial.print (new_freq_b);
  int value = analogRead(A0);
  int suggested_brightness = max(0,map(value, 800, 300, 15, 0));
  suggested_brightness = min(15,((suggested_brightness % 2) + suggested_brightness));
  if (suggested_brightness != brightness){
    brightness_counter++;
  } else {
    brightness_counter = 0;
  }

  if (brightness_counter >= 50){
    brightness = suggested_brightness;
    brightness_counter = 0;
  }
//  Serial.print (",seg_brightness:");
//  Serial.print(suggested_brightness);
//  Serial.print (",actual_brightness:");
//  Serial.println(brightness);

}
