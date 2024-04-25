
void setup(){
  Serial.begin(115200);
  pinMode(A5, INPUT);
}

void loop(){
    Serial.println(map(analogRead(A5),1023,0,5,0));
}
