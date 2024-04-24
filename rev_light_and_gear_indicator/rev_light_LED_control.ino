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
      leds[i] = blue;
      FastLED.show();
      delay(TIME_BETWEEN_LOOPS);
    }
    delay(1000);
    
  }
