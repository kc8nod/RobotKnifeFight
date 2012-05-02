#include <ServoTimer2.h>
#include <VirtualWire.h>
#include "pin_defs.h"
#include "RKF_Radio.h"

ServoTimer2 LeftDrive, RightDrive;

RKF_Radio radio;

void setup(){
  LeftDrive.attach(LEFT_DRIVE_PIN);
  RightDrive.attach(RIGHT_DRIVE_PIN);
  pinMode(BUMP_SWITCH_RIGHT_PIN, INPUT);
  pinMode(BUMP_SWITCH_LEFT_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  Serial.begin(57600);
  
  radio.start();
}


void loop()
{
  if(radio.recv())
  {
    switch(radio.packet.message)
    {
      case 7:
        LeftDrive.write(radio.packet.data[1]);
        RightDrive.write(radio.packet.data[2]);
        break;
  
      case 0:
      default:
        // do nothing
        break;    
    }
  }
  
  
}




