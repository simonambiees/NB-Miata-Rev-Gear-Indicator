
int display_intensity = 0;

// gears
byte gears[7][8] = {
{
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
},{
  B00000000,
  B00011000,
  B00011000,
  B00011100,
  B00011000,
  B00011000,
  B00011000,
  B01111110
},{
  B00000000,
  B00111100,
  B01100110,
  B01100000,
  B00110000,
  B00001100,
  B00000110,
  B01111110
},{
  B00000000,
  B00111100,
  B01100110,
  B01100000,
  B00111000,
  B01100000,
  B01100110,
  B00111100
},{
  B00000000,
  B00110000,
  B00111000,
  B00110100,
  B01100100,
  B01111110,
  B00110000,
  B00110000
},{
  B00000000,
  B01111110,
  B00000110,
  B00111110,
  B01100000,
  B01100000,
  B01100110,
  B00111100
},{
  B00000000,
  B00111110,
  B01100110,
  B01100110,
  B00111110,
  B00011110,
  B01101100,
  B01100110
}};

int calculate_gear(){
//  if (speedo < 15){
//    return 0;
//  }
  float ratio = rpm/speedo;
  if (ratio >= GEAR_1*(100.0-TOLERANCE_PERCENT)/100.0 && ratio <= GEAR_1*(100.0+TOLERANCE_PERCENT)/100.0){
    return 1;
  } else if (ratio >= GEAR_2*(100.0-TOLERANCE_PERCENT)/100.0 && ratio <= GEAR_2*(100.0+TOLERANCE_PERCENT)/100.0){
    return 2;
  } else if (ratio >= GEAR_3*(100.0-TOLERANCE_PERCENT)/100.0 && ratio <= GEAR_3*(100.0+TOLERANCE_PERCENT)/100.0){
    return 3;
  } else if (ratio >= GEAR_4*(100.0-TOLERANCE_PERCENT)/100.0 && ratio <= GEAR_4*(100.0+TOLERANCE_PERCENT)/100.0){
    return 4;
  } else if (ratio >= GEAR_5*(100.0-TOLERANCE_PERCENT)/100.0 && ratio <= GEAR_5*(100.0+TOLERANCE_PERCENT)/100.0){
    return 5;
  } else {
    return 0;
  }
}

void setup_matrix_display(){
  lc.shutdown(0,false);
  // Set brightness to a medium value
  lc.setIntensity(0,brightness);
  // Clear the display
  lc.clearDisplay(0);
}

void start_up_sequence(){
  for (int i=0; i<=7; i++){
    lc.setColumn(0,i,B11111111);
    delay(500);
    lc.setColumn(0,i,B00000000);
  }
}

void update_display(byte image[]){
  //rotated 90 degrees
  for (int i=0; i<=7; i++){
    lc.setColumn(0,i,image[i]);
  }
}

void fill_display(){
  for (int i=0; i<=7; i++){
    lc.setRow(0,i,B11111111);
  }
}

void blank_display(){
  for (int i=0; i<=7; i++){
    lc.setRow(0,i,B00000000);
  }
}

void display_gear(int gear, int rpm){
  lc.setIntensity(0,brightness);
  if (rpm > RPM_FLASH_THRESHOLD){
    if (flashing_on){
      update_display(gears[gear]);
    } else {
      fill_display();
    }
    if (flashing_on == true) {
      flashing_on = false;
    } else {
      flashing_on = true;
    }
  } else {
    update_display(gears[gear]);
  }
}
