#include "bot_id.h"
#include <ServoTimer2.h>
#include <VirtualWire.h>
#include "pin_defs.h"
#include "RKF_Radio.h"

int incomingByte;

unsigned long now = 0;

ServoTimer2 LeftDrive, RightDrive;
#define LEFT_FWD 2000
#define LEFT_STOP 1500
#define LEFT_REV 1000
#define RIGHT_FWD 1000
#define RIGHT_STOP 1500
#define RIGHT_REV 2000
unsigned int speed_L = LEFT_STOP;
unsigned int speed_R = RIGHT_STOP;
unsigned long timeLastServoUp = 0;

RKF_Radio radio;

void setup() {
  Serial.begin(115200);
  
  LeftDrive.attach(LEFT_DRIVE_PIN);
  RightDrive.attach(RIGHT_DRIVE_PIN);
  
  pinMode(BUMP_SWITCH_RIGHT_PIN, INPUT);
  pinMode(BUMP_SWITCH_LEFT_PIN, INPUT);
  pinMode(DEFAULT_RX_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  LeftDrive.write(LEFT_STOP);
  RightDrive.write(RIGHT_STOP);
  
  radio.start();
  
}

void loop() {
  
  now = millis();
  
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    switch(incomingByte) {
      case 'w':
      case 'W':
        digitalWrite(STATUS_LED_PIN, HIGH);
        speed_L = LEFT_FWD;
        speed_R = RIGHT_FWD;
        break;
        
      case 'a':
      case 'A':
        digitalWrite(STATUS_LED_PIN, HIGH);
        speed_L = LEFT_REV;
        speed_R = RIGHT_FWD;
        break;
        
      case 's':
      case 'S':
        digitalWrite(STATUS_LED_PIN, HIGH);
        speed_L = LEFT_REV;
        speed_R = RIGHT_REV;
        break;
        
      case 'd':
      case 'D':
        digitalWrite(STATUS_LED_PIN, HIGH);
        speed_L = LEFT_FWD;
        speed_R = RIGHT_REV;
        break;
        
      default:
        digitalWrite(STATUS_LED_PIN, LOW);
        speed_L = LEFT_STOP;
        speed_R = RIGHT_STOP;
        break;
    }
  }
  
  //Update Servo positions  
  if(now - timeLastServoUp > 15){  //limit servo updating to every 15ms at most
    LeftDrive.write(speed_L);
    RightDrive.write(speed_R);
    timeLastServoUp = now;
  }
  
}
