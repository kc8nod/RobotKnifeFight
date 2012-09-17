int inByte = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  establishContact();
}

void loop() {
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    Serial.write(inByte);               
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.print('A');   // send a capital A
    delay(300);
  }
}
