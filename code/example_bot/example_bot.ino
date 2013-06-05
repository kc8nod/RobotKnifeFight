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
unsigned long timeLastServoUpdate = 0;
unsigned long timeToStop = 0;
unsigned long timeLastStatus = 0;

RKF_Radio radio;
extern "C" { uint8_t vw_rx_active; };
unsigned long timeLastMessage = 0;

String serialInputString = "";  // a string to hold incoming data

RKF_Position Me;
RKF_Position Destination;
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
  Serial.println("example_bot0: patrol");
  
  outputHelp();
  
  //Define Destination point and heading
  Destination.x = 58;
  Destination.y = 36;
  Destination.heading = 0;
}


/* 
  Main loop
------------------------------------------------------------------------------*/
void loop(){
  now = millis();
  
  if(radio.recv()){  //if a radio message has been received
    timeLastMessage = now;
    
    switch(radio.packet.message)  //Do stuff based on message type.  
    {
      case RKF_POSITION_MESSAGE:  // position message
        //update my location
        Me = radio.packet.robot[MY_BOT_ID];
        outputStatus();
        break;  
    }
  }
  
  processSerialInput();
  
  //"Thought processes"
  //-------------------------------------------
  
  //indicate time of last message with status LED
  if(timeLastMessage > 0 && (now-timeLastMessage)<1000){
    digitalWrite(STATUS_LED_PIN, true); //message recieved in last 1000ms
  }else{
    digitalWrite(STATUS_LED_PIN, false);
  }
  
  //go to the Destination point
  //what is the distance?
  byte d = byte(Me.distance(Destination));
  
  //what is the heading?
  //convert the bearing to a heading of 0-15 increasing counter clockwise
  //should the RKF_Radio lib do this?
  int heading = int(16 + round( -Me.bearing(Destination)/(PI/8) + (PI/16) ))%16; 
  //Serial.print(heading);
  //Serial.print(" ");
  //Serial.print(Me.heading);
  //Serial.print(" ");
  
  //what is the rotation direction and amount needed?
  int hdiff = (heading-Me.heading);
  byte rot_amount;
  if (hdiff==0){
    //I'm facing the right way to the Destination point
    //stop
    //Serial.print("STOP  ");
    speed_L = LEFT_STOP;
    speed_R = RIGHT_STOP;
    timeToStop = now;
    
  }else if (hdiff < -8 || (0 < hdiff && hdiff < 8)){
    //Turn left
    //Serial.print("LEFT ");
    rot_amount = (16+abs(hdiff))%16;
    if (rot_amount > 0){
      speed_L = (LEFT_REV-LEFT_STOP)*0.1 + LEFT_STOP;
      speed_R = (RIGHT_FWD-RIGHT_STOP)*0.1 + RIGHT_STOP;
      timeToStop = now + 200;
    }
    
  }else{
    //Turn right
    //Serial.print("RIGHT ");
    rot_amount = (16-abs(hdiff))%16;
    if (rot_amount > 0){
      speed_L = (LEFT_FWD-LEFT_STOP)*0.1 + LEFT_STOP;
      speed_R = (RIGHT_REV-RIGHT_STOP)*0.1 + RIGHT_STOP;
      timeToStop = now + 200;
    }
  }
  //Serial.print(rot_amount);
  //Serial.print(" ");
  //Serial.print(timeToStop);
  //Serial.println();
  
  //if a bump switch is triggered backup until it is not triggered
  boolean hit_something = false;
  if(!digitalRead(BUMP_SWITCH_RIGHT_PIN) || !digitalRead(BUMP_SWITCH_LEFT_PIN)){
    hit_something = true;
    speed_L = LEFT_REV;
    speed_R = RIGHT_REV;
    timeToStop = now + 100; //reverse for 100ms
  }
  
  //Is it time to stop?
  if(timeToStop > 0 && now >= timeToStop){
    speed_L = LEFT_STOP;
    speed_R = RIGHT_STOP;
    timeToStop = 0;
  }
  
  //-------------------------------------------
  
  //Update Servo positions  
  if(now - timeLastServoUpdate > 15){  //limit servo updating to every 15ms at most
    LeftDrive.write(speed_L);
    RightDrive.write(speed_R);
    timeLastServoUpdate = now;
  }
  
  delay(1);
}
/*----------------------------------------------------------------------------*/

