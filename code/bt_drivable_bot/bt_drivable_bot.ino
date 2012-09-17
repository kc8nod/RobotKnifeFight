#include "bot_id.h"
#include <ServoTimer2.h>
#include <VirtualWire.h>
#include "pin_defs.h"
#include "RKF_Radio.h"

int incomingByte;

void setup() {
  Serial.begin(115200);
  pinMode(STATUS_LED_PIN, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    switch(incomingByte) {
      case 'w':
      case 'W':
        digitalWrite(STATUS_LED_PIN, HIGH);
        Serial.print("w");
        break;
      case 'a':
      case 'A':
        digitalWrite(STATUS_LED_PIN, HIGH);
        Serial.print('a');
        break;
      case 's':
      case 'S':
        digitalWrite(STATUS_LED_PIN, HIGH);
        Serial.print("s");
        break;
      case 'd':
      case 'D':
        digitalWrite(STATUS_LED_PIN, HIGH);
        Serial.print("d");
        break;
      default:
        digitalWrite(STATUS_LED_PIN, LOW);
        break;
    }
  }
}
