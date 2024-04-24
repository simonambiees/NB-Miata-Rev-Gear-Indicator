void update_rpm(float n_freq){
  int new_rpm = n_freq*RPM_PER_HZ/10;
  new_rpm = new_rpm*10;
  if (abs(new_rpm - rpm) < 1000) {
    rpm = new_rpm;
  }
}

void update_speedo(float n_freq){
  float new_spd = n_freq/(4000.0/3600.0)*1.0/1.0;
  if (new_spd < 140){
    speedo = new_spd;
  }
}

void update_rev_light(float n_freq){
  if (n_freq >= 0){
    int new_rpm = n_freq*RPM_PER_HZ/50;
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
        int palette_index = 255.0/10*i;
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
 
    Serial.print (" nLED: ");
    Serial.print (n_active_lights);
    // so we can read it  
    delay (100);
  }
}
