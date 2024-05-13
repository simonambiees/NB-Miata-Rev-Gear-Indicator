#include <FastLED.h>

// delay time in ms
#define TIME_BETWEEN_LOOPS 50

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

#define LED_PIN     3
#define NUM_LEDS    9
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define LIGHT_SENSOR_PIN A4
#define BRIGHTNESS_LOWER 30
#define BRIGHTNESS_UPPER 255
volatile int brightness = 0;

CRGB black  = CRGB::Black;
CRGB red  = CRGB::Red;
CRGB green  = CRGB::Green;
CRGB blue  = CRGB::Blue;

CRGB shift_pattern[] = {blue, blue, blue, red, red, red, red, green, green, green};
CRGBPalette16 shift_palette(
  blue, blue, blue, blue, red,
  red, red, red, red, green, green,
  green, green, green, green, green
);

volatile boolean flashing_on;
volatile int cruise_count = 0;
volatile int rpm = 0;
// Freq Input: Pin D2

volatile boolean first;
volatile boolean triggered;
volatile unsigned long overflowCount;
volatile unsigned long startTime;
volatile unsigned long finishTime;

// here on rising edge
void isr () 
{
  unsigned int counter = TCNT1;  // quickly save it
  
  // wait until we noticed last one
  if (triggered)
    return;

  if (first)
    {
    startTime = (overflowCount << 16) + counter;
    first = false;
    return;  
    }
    
  finishTime = (overflowCount << 16) + counter;
  triggered = true;
  detachInterrupt(0);   
}  // end of isr

// timer overflows (every 65536 counts)
ISR (TIMER1_OVF_vect) 
{
  overflowCount++;
}  // end of TIMER1_OVF_vect


void prepareForInterrupts ()
  {
  // get ready for next time
  EIFR = bit (INTF0);  // clear flag for interrupt 0
  first = true;
  triggered = false;  // re-arm for next time
  attachInterrupt(0, isr, RISING);     
  }  // end of prepareForInterrupts

void blank_led ()
  {
    for( int i = 0; i < NUM_LEDS; i++) {
      leds[i] = black;
    }
  }

void red_led ()
  {
    for( int i = 0; i < NUM_LEDS; i++) {
      leds[i] = red;
    }
  }

void start_up_sweep ()
  {
    for( int i = 0; i < NUM_LEDS; i++) {
      // leds[i] = blue;
      int palette_index = 220.0/(NUM_LEDS-1)*i;
      leds[i] = ColorFromPalette(shift_palette, palette_index);
      FastLED.show();
      delay(TIME_BETWEEN_LOOPS);
    }
    delay(1000);
    
  }

void setup () 
  {
  Serial.begin(115200);       
  Serial.println("Frequency Counter");
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  delay( 2000 ); // power-up safety delay

  int value = analogRead(LIGHT_SENSOR_PIN);
  brightness = min(BRIGHTNESS_UPPER,max(BRIGHTNESS_LOWER,map(value, 1023, 700, BRIGHTNESS_UPPER, BRIGHTNESS_LOWER)));
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(brightness);
  
  blank_led();
  FastLED.show();
  flashing_on = false;
  start_up_sweep();
  // blank_led();
  FastLED.show();
  
  
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
  prepareForInterrupts ();   
  
  } // end of setup

void loop () 
  {
  
  int value = analogRead(LIGHT_SENSOR_PIN);
  brightness = min(BRIGHTNESS_UPPER,max(BRIGHTNESS_LOWER,map(value, 1023, 700, BRIGHTNESS_UPPER, BRIGHTNESS_LOWER)));
  FastLED.setBrightness(brightness);

  if (!triggered){
    FastLED.show();
    return;
  }
 
  unsigned long elapsedTime = finishTime - startTime;
  float freq = F_CPU / float (elapsedTime);  // each tick is 62.5 ns at 16 MHz
  int new_rpm = freq*RPM_PER_HZ/50;
  new_rpm = new_rpm*50;
  if (abs(new_rpm - rpm) < 1000) {
    rpm = new_rpm;
  }

  int n_active_lights = (rpm - RPM_LOW) / ((RPM_FLASH_THRESHOLD-RPM_LOW)/NUM_LEDS);
  if (n_active_lights > NUM_LEDS){
    n_active_lights = 0;
  }
  if (rpm < RPM_LOW) {
    n_active_lights = 0;
  }

  for( int i = 0; i < NUM_LEDS; i++) {
    if (i < n_active_lights) {
      int palette_index = 220.0/(NUM_LEDS-1)*i;
      leds[i] = ColorFromPalette(shift_palette, palette_index);
    } else {
      leds[i] = black;
    }
  }

  if (rpm >= RPM_FLASH_THRESHOLD) {
    if (flashing_on) {
      red_led();
    } else {
      blank_led();
    }
    if (flashing_on == true) {
      flashing_on = false;
    } else {
      flashing_on = true;
    }
  }


  // Cruise Mode logic
  if ((rpm >= CRUISE_LOW) && (rpm <= CRUISE_HIGH)){
    cruise_count += 1;
  } else {
    cruise_count -= 1;
    if ((rpm > CRUISE_HIGH) || (cruise_count < 0) || (cruise_count < CRUISE_THRESHOLD)){
      cruise_count = 0;
    }
  }

  if (cruise_count*TIME_BETWEEN_LOOPS >= CRUISE_THRESHOLD) {
    if (cruise_count*TIME_BETWEEN_LOOPS >= CRUISE_THRESHOLD/3.0*4) {
      cruise_count = CRUISE_THRESHOLD/3.0*4/TIME_BETWEEN_LOOPS;
    }
    blank_led();
  }

  FastLED.show();
  

  Serial.print ("RPM: ");
  Serial.print (rpm);
  Serial.print (" cruise: ");
  Serial.print (cruise_count);
  Serial.print (" nLED: ");
  Serial.print (n_active_lights);
  Serial.print (" Brightness: ");
  Serial.println (brightness);

  // so we can read it  
  delay (TIME_BETWEEN_LOOPS);

  prepareForInterrupts ();   
}   // end of loop
