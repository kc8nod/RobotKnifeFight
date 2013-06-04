#include "bot_id.h"
#include <ServoTimer2.h>
#include <VirtualWire.h>
#include "pin_defs.h"
#include "RKF_Radio.h"

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
unsigned long timeToStop = 0;
unsigned long timeLastStatus = 0;


RKF_Radio radio;
unsigned long timeLastMessage = 0;
unsigned int message_count = 0;

extern "C" { uint8_t vw_rx_active; };

String serialInputString = "";  // a string to hold incoming data


/*
  Setup
------------------------------------------------------------------------------*/
void setup(){
  LeftDrive.attach(LEFT_DRIVE_PIN);
  RightDrive.attach(RIGHT_DRIVE_PIN);
  pinMode(BUMP_SWITCH_RIGHT_PIN, INPUT);
  pinMode(BUMP_SWITCH_LEFT_PIN, INPUT);
  pinMode(DEFAULT_RX_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  LeftDrive.write(LEFT_STOP);
  RightDrive.write(RIGHT_STOP);
  
  radio.start();
  
  serialInputString.reserve(16);
  Serial.begin(115200);
  Serial.println("example_bot: Reset");
  outputHelp();
}


/* 
  Main loop
------------------------------------------------------------------------------*/
void loop(){
  now = millis();
  
  if(radio.recv()){  //if a radio message has been received
    timeLastMessage = now;
    message_count++;
    
    outputStatus(); //comment out if not debugging

    switch(radio.packet.message)  //Do stuff based on message type.  
    {
      case 0:  // position message
        //parse position data.
        Serial.print("got msg ");
        Serial.println(message_count);
        break;  
      default: 
        break;  // do nothing    
    }
  }
  
  if(vw_rx_active){  //if radio is busy receiving a message
    
  }
  
  digitalWrite(STATUS_LED_PIN, (now - timeLastMessage < 1000));  //Turn off status LED if no messages for 1 second
  
  processSerialInput();
  if(now - timeLastStatus > 5000){  //send a ."now" ever 5 seconds 
    Serial.print('.');
    Serial.println(now);
    timeLastStatus = now;
  }
  
  //"Thought process"
  //-------------------------------------------
  //if a bump switch is triggered backup until it is not triggered then stop
  boolean hit_something = false;
  if(!digitalRead(BUMP_SWITCH_RIGHT_PIN) || !digitalRead(BUMP_SWITCH_LEFT_PIN)){
    hit_something = true;
    speed_L = LEFT_REV;
    speed_R = RIGHT_REV;
    timeToStop = now + 200; //reverse for 200ms
  }
  if(timeToStop > 0 && now > timeToStop){
    speed_L = LEFT_STOP;
    speed_R = RIGHT_STOP;
    timeToStop = 0;
  }
  //if the game has started 
  if(message_count > 0){
    //if I haven't hit something go forward
    if(!hit_something){
      speed_L = LEFT_FWD;
      speed_R = RIGHT_FWD;
      timeToStop = now + 200; //200ms from now
    }
  }
  
  //Update Servo positions  
  if(now - timeLastServoUp > 15){  //limit servo updating to every 15ms at most
    LeftDrive.write(speed_L);
    RightDrive.write(speed_R);
    timeLastServoUp = now;
  }
  
  delay(1);
}
/*----------------------------------------------------------------------------*/

