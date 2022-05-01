char serial_command = -1;

void setup() {
  pinMode(D0, OUTPUT);

  Serial.begin(115200);
}

void loop() {

 if(Serial.available()>0){

     serial_command = Serial.read();
     if(serial_command == '0'){
        digitalWrite(D0, LOW);
        Serial.println("0");
     }
     if(serial_command == '1'){
        digitalWrite(D0, HIGH);
         Serial.println("1");
     }
     
  }
  
}
