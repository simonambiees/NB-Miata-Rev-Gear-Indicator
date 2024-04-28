
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
  B00111000,
  B00011000,
  B00011000,
  B00011000,
  B01111110
},{
  B00000000,
  B00111100,
  B01100110,
  B00000110,
  B00001100,
  B00110000,
  B01100000,
  B01111110
},{
  B00000000,
  B00111100,
  B01100110,
  B00000110,
  B00011100,
  B00000110,
  B01100110,
  B00111100
},{
  B00000000,
  B00001100,
  B00011100,
  B00101100,
  B01001100,
  B01111110,
  B00001100,
  B00001100
},{
  B00000000,
  B01111110,
  B01100000,
  B01111100,
  B00000110,
  B00000110,
  B01100110,
  B00111100
},{
  B00000000,
  B01111100,
  B01100110,
  B01100110,
  B01111100,
  B01111000,
  B01101100,
  B01100110
}};

byte revs[6][8] = {
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
  B11111111,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B11111111
},{
  B11111111,
  B11111111,
  B11000011,
  B11000011,
  B11000011,
  B11000011,
  B11111111,
  B11111111
},{
  B11111111,
  B11111111,
  B11111111,
  B11100111,
  B11100111,
  B11111111,
  B11111111,
  B11111111
},{
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111
},{
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
}};

int calculate_rev_stage(){
  int rev_stage = min(5,max(0, map(rpm, RPM_FLASH_THRESHOLD, RPM_LOW, 4, 1)));
  if (rpm > RPM_FLASH_THRESHOLD){
    rev_stage = 5;
  }
  return rev_stage;
}

void display_rpm(int rev_stage){
  lc.setIntensity(0,brightness);
  if (rpm > RPM_FLASH_THRESHOLD){
    if (flashing_on){
      update_display(revs[rev_stage]);
    } else {
      fill_display();
    }
    if (flashing_on == true) {
      flashing_on = false;
    } else {
      flashing_on = true;
    }
  } else {
    update_display(revs[rev_stage]);
  }
}

//int calculate_gear(){
////  if (speedo < 15){
////    return 0;
////  }
//  float ratio = rpm/speedo;
//  if (ratio >= GEAR_1*(100.0-TOLERANCE_PERCENT)/100.0 && ratio <= GEAR_1*(100.0+TOLERANCE_PERCENT)/100.0){
//    return 1;
//  } else if (ratio >= GEAR_2*(100.0-TOLERANCE_PERCENT)/100.0 && ratio <= GEAR_2*(100.0+TOLERANCE_PERCENT)/100.0){
//    return 2;
//  } else if (ratio >= GEAR_3*(100.0-TOLERANCE_PERCENT)/100.0 && ratio <= GEAR_3*(100.0+TOLERANCE_PERCENT)/100.0){
//    return 3;
//  } else if (ratio >= GEAR_4*(100.0-TOLERANCE_PERCENT)/100.0 && ratio <= GEAR_4*(100.0+TOLERANCE_PERCENT)/100.0){
//    return 4;
//  } else if (ratio >= GEAR_5*(100.0-TOLERANCE_PERCENT)/100.0 && ratio <= GEAR_5*(100.0+TOLERANCE_PERCENT)/100.0){
//    return 5;
//  } else {
//    return 0;
//  }
//}

void setup_matrix_display(){
  lc.shutdown(0,false);
  // Clear the display
  lc.clearDisplay(0);
  // Set brightness to a medium value
  lc.setIntensity(0,brightness);
}

void start_up_sequence(){
  for (int i=0; i<=3; i++){
    update_display(revs[i]);
    delay(500);
    blank_display();
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

//void display_gear(int gear, int rpm){
//  lc.setIntensity(0,brightness);
//  if (rpm > RPM_FLASH_THRESHOLD){
//    if (flashing_on){
//      update_display(gears[gear]);
//    } else {
//      fill_display();
//    }
//    if (flashing_on == true) {
//      flashing_on = false;
//    } else {
//      flashing_on = true;
//    }
//  } else {
//    update_display(gears[gear]);
//  }
//}
