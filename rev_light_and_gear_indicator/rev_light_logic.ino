void update_rpm(float n_freq){
  int new_rpm = n_freq*RPM_PER_HZ/10;
  new_rpm = new_rpm*10;
  if (abs(new_rpm - rpm) < 1000) {
    rpm = new_rpm;
  }
}

void update_speedo(float n_freq){
  float new_spd_x2 = n_freq/(4000.0/3600.0)*1.0/1.0;
  if (abs(new_spd_x2 - speedo*2.0) < 20){
    speedo = new_spd_x2/2.0;
  }
}
