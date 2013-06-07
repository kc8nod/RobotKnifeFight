#include "bot_id.h"
#include <ServoTimer2.h>
#include <VirtualWire.h>
#include "pin_defs.h"
#include "RKF_Radio.h"

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
unsigned long timeToGo = 0;
unsigned long timeLastStatus = 0;

RKF_Radio radio;
extern "C" { uint8_t vw_rx_active; };
unsigned long timeLastMessage = 0;
unsigned long timeLastRadioAttempt = 0;

String serialInputString = "";  // a string to hold incoming data

RKF_Position Me;
RKF_Position Target;

int headingTo = 0;
byte distanceTo = 0;
byte rotAmountTo = 0;
byte actionCount = 0;

boolean gameOn = false;

byte DestIndex = MY_BOT_ID;
byte Destinations[4][2] = {{6,6},{6,40},{64,40},{64,6}};

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
  
  //Define Target point
  Target.x = Destinations[DestIndex][0];
  Target.y = Destinations[DestIndex][1];
  
}


/* 
  Main loop
------------------------------------------------------------------------------*/
void loop(){
  //Get Radio data if available
  if(millis()-timeLastRadioAttempt>50){  //onlt check for new message every 50ms
    Serial.print(".");
    if(radio.recv()){  //if a radio message has been received
      switch(radio.packet.message)  //Do stuff based on message type.  
      {
        case RKF_POSITION_MESSAGE:  // position message
          timeLastMessage = millis();
          if(radio.packet.robot[MY_BOT_ID].x>0 || radio.packet.robot[MY_BOT_ID].y>0){
            Me = radio.packet.robot[MY_BOT_ID]; //update my location
          }
          Serial.print("+");
          break;  
      }
    }
    timeLastRadioAttempt = millis();
  }
  
  processSerialInput();
  
  //"Thought processes"
  //-------------------------------------------
  
  //indicate time of last message with status LED
  if((millis()-timeLastMessage) < 5000) {
    digitalWrite(STATUS_LED_PIN, true); //message recieved in last 5000ms
    gameOn = true;
  }else{
    digitalWrite(STATUS_LED_PIN, false);
    gameOn = false;
    Stop();
  }
  if(gameOn){
    //go to the Target point
    
    if(Me.x>0 || Me.y>0){  //if my position is valid.
      
      //what is the distance to the Target point?
      distanceTo = byte(Me.distance(Target));
      
      //what is the heading to the Target point?
      headingTo = int(16 + round( -Me.bearing(Target)/(PI/8) + (PI/16) ))%16; //convert the bearing to a heading of 0-15 increasing counter clockwise
      
      //if farther than 6 inches to Target
      if(distanceTo > 6 ){
        
        //what is the rotation direction and amount needed?
        int hdiff = (headingTo-Me.heading);
        if (hdiff==0){
          //I'm facing the right way to the Target point
          Stop();
          rotAmountTo = 0;
          
          //drive towards it.
          speed_L = throttle(LEFT_FWD, LEFT_STOP, 0.1);
          speed_R = throttle(RIGHT_FWD, RIGHT_STOP, 0.1);
          timeToStop = millis() + 200;
          timeToGo = timeToStop + 1000;
          actionCount++;
          
        }else if (hdiff < -8 || (0 < hdiff && hdiff < 8)){
          //Turn left
          rotAmountTo = (16+abs(hdiff))%16;
          if (rotAmountTo > 0){
            //turn left then wait a little bit
            if(timeToStop == 0 and timeToGo<millis()){
              speed_L = throttle(LEFT_REV, LEFT_STOP, 0.1);
              speed_R = throttle(RIGHT_FWD, RIGHT_STOP, 0.1);
              timeToStop = millis() + (100+50*(rotAmountTo-1));
              timeToGo = timeToStop + 1000;
              actionCount++;
            }
          }
          
        }else{
          //Turn right
          rotAmountTo = (16-abs(hdiff))%16;
          if (rotAmountTo > 0){
            //turn right then wait a little bit
            if(timeToStop == 0 and timeToGo<millis()){
              speed_L = throttle(LEFT_FWD, LEFT_STOP, 0.1);
              speed_R = throttle(RIGHT_REV, RIGHT_STOP, 0.1);
              timeToStop = millis() + (100+50*(rotAmountTo-1));
              timeToGo = timeToStop + 1000;
              actionCount++;
            }
          }
        }
      }else{
        //withinrange of the current destination, set the nest one.
        DestIndex = (DestIndex+1)%4;
        Target.x = Destinations[DestIndex][0];
        Target.y = Destinations[DestIndex][1];
      }
      
    } //end valid Me position
    
    //if a bump switch is triggered backup
    if(!digitalRead(BUMP_SWITCH_RIGHT_PIN) || !digitalRead(BUMP_SWITCH_LEFT_PIN)){
      speed_L = LEFT_REV;
      speed_R = RIGHT_REV;
      timeToStop = millis() + 200; //reverse for 200ms
      timeToGo = timeToStop + 1000;
    }
    
  } //end game on
  
  
  //-------------------------------------------
  
  
  //Is it time to stop?  I consider this more of a reflex than thought.
  if(timeToStop > 0 && millis() >= timeToStop){
    Stop();
  }
  
  //Update Servo positions  
  if(millis() - timeLastServoUpdate > 15){  //limit servo updating to every 15ms at most
    LeftDrive.write(speed_L);
    RightDrive.write(speed_R);
    timeLastServoUpdate = millis();
  }
  
  if(millis()-timeLastStatus > 1000){
    outputStatus();
    timeLastStatus = millis();
  }
    
  delay(1);
}
/*----------------------------------------------------------------------------*/


void Stop(){
  speed_L = LEFT_STOP;
  speed_R = RIGHT_STOP;
  timeToStop = 0;
  
  if(timeToGo <= millis()){
    actionCount = 0;
  }
}

int throttle(int Direction, int Stop, float Throttle){
  return int((Direction-Stop)*Throttle + Stop);
}
