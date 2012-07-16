int counter = 0;
int incomingByte;
int ledpin = 13;
boolean led = false;

void setup() {
  Serial.begin(115200);
  pinMode(ledpin, OUTPUT);
  
  digitalWrite(ledpin, HIGH);
  delay(5000); 
}

void loop() {
  // see if there's incoming serial data:
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();
    // if it's a capital R, reset the counter
    if (incomingByte == 'R') {
      Serial.println("RESET");
      counter=0;
    }
  }
  
  Serial.println(counter);
  counter++;
  
  led = !led;
  digitalWrite(ledpin, led);
  
  delay(250);
}
