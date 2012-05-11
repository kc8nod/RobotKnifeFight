#include <ServoTimer2.h>
#include <VirtualWire.h>
#include "pin_defs.h"
#include "RKF_Radio.h"

ServoTimer2 LeftDrive, RightDrive;

#define LEFT_REV 1000
#define LEFT_FWD 2040
#define LEFT_STOP 1520

#define RIGHT_FWD 1000
#define RIGHT_REV 2040
#define RIGHT_STOP 1520

int speed_L = LEFT_STOP;
int speed_R = RIGHT_STOP;

RKF_Radio radio;

void setup(){
  LeftDrive.attach(LEFT_DRIVE_PIN);
  RightDrive.attach(RIGHT_DRIVE_PIN);
  pinMode(BUMP_SWITCH_RIGHT_PIN, INPUT);
  pinMode(BUMP_SWITCH_LEFT_PIN, INPUT);
  pinMode(DEFAULT_RX_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  Serial.begin(57600);
  
  LeftDrive.write(LEFT_STOP);
  RightDrive.write(RIGHT_STOP);
  
  radio.start();
  
  Serial.println("Reset");
}


void loop()
{
  int idx;

  if(radio.recv())
  {
    Serial.print("rx:");
    Serial.println(radio);
    
    switch(radio.packet.message)
    {
      case 7:
        speed_L = map(radio.packet.data[1],
                    0, 255, LEFT_REV, LEFT_FWD);

        speed_R = map(radio.packet.data[2],
                    0, 255, RIGHT_REV, RIGHT_FWD);
                    
        Serial.print("speed_L = ");
        Serial.println(speed_L);
        Serial.print("speed_R = ");
        Serial.println(speed_R);

        break;
  
      case 0:
      default:
        // do nothing
        break;    
    }
  }
  
  LeftDrive.write(speed_L);
  RightDrive.write(speed_R);
  
}




