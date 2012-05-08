#include <ServoTimer2.h>
#include <VirtualWire.h>
#include "pin_defs.h"
#include "RKF_Radio.h"

ServoTimer2 LeftDrive, RightDrive;

#define LEFT_FWD 1000
#define LEFT_REV 2000
#define LEFT_STOP ((LEFT_FWD + LEFT_REV)/2)

#define RIGHT_FWD 1000
#define RIGHT_REV 2000
#define RIGHT_STOP ((RIGHT_FWD + RIGHT_REV)/2)

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
  int speed_L, speed_R;
  int idx;

  if(radio.recv())
  {
    Serial.print("rx:");
    for(idx=0; idx<8; idx++)
    {
      Serial.print(" ");
      Serial.print(radio.packet.data[idx], HEX);
    }
    Serial.println("");
    Serial.println(radio.packet.message, HEX);
    
    switch(radio.packet.message)
    {
      case 7:
        speed_L = map(radio.packet.data[1],
                    0, 255, LEFT_REV, LEFT_FWD);

        speed_R = map(radio.packet.data[2],
                    0, 255, LEFT_REV, LEFT_FWD);
                    
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




