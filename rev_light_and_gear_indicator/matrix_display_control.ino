
int display_intensity = 0;

// gears
byte gears[7][8] = {
{
  B00000000,
  B01100011,
  B01110011,
  B01111011,
  B01101111,
  B01100111,
  B01100011,
  B01100011
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

void setup_matrix_display(){
  lc.shutdown(0,false);
  // Set brightness to a medium value
  lc.setIntensity(0,brightness);
  // Clear the display
  lc.clearDisplay(0);
}

void update_display(byte image[]){
  //rotated 90 degrees
  for (int i=0; i<=7; i++){
    lc.setColumn(0,i,image[7-i]);
  }
}

void fill_display(){
  for (int i=0; i<=7; i++){
    lc.setRow(0,i,B11111111);
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
