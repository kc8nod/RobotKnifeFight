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
RKF_Position Dest;

int headingTo = 0;
byte distanceTo = 0;
byte rotAmountTo = 0;

boolean gameOn = false;

byte TargetIndex = MY_BOT_ID;

byte PosIndex = 0;
//byte Pos[16][2] = {{6,6},{6,18},{6,30},{6,40}
//                  ,{18,40},{30,40},{40,40},{52,40}
//                  ,{64,40},{64,30},{64,18},{64,6}
//                  ,{52,6},{40,6},{30,6},{18,6}};
byte Pos[4][2] = {{6,6},{6,40},{64,40},{64,6}};

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
  Serial.println("example_bot: runaway");
  
  outputHelp();
}


/* 
  Main loop
------------------------------------------------------------------------------*/
void loop(){
  //Get Radio data if available
  if(millis()-timeLastRadioAttempt>50){  //only check for new message every 50ms
    Serial.print(".");
    if(radio.recv()){  //if a radio message has been received
      switch(radio.packet.message)  //Do stuff based on message type.  
      {
        case RKF_POSITION_MESSAGE:  // position message
          timeLastMessage = millis();
          if(validPosition(radio.packet.robot[MY_BOT_ID])){
            Me = radio.packet.robot[MY_BOT_ID]; //update my location
          }
          for(byte i=0; i<4; i++){
            if(i!=MY_BOT_ID && validPosition(radio.packet.robot[i]) && radio.packet.robot[i].valid==1){
              //is the current bot's distance closer than the target bot's?
              if(TargetIndex==MY_BOT_ID || Me.distance(radio.packet.robot[i]) < Me.distance(radio.packet.robot[TargetIndex])){
                //New Target
                TargetIndex = i;
              }
            }
          }
          
          Target = radio.packet.robot[TargetIndex];
          
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
  if((millis()-timeLastMessage) < 2000) {
    digitalWrite(STATUS_LED_PIN, true); //message recieved in last 2000ms
    gameOn = true;
  }else{
    digitalWrite(STATUS_LED_PIN, false);
    gameOn = false;
  }
  if(gameOn){
    if(validPosition(Me)){  //if my position is valid.
    
      //Where should I go?
      byte PosIndex = whichPosFarthest(Target);
      Dest.x = Pos[PosIndex][0];
      Dest.y = Pos[PosIndex][1];
      distanceTo = byte(Me.distance(Dest));
      headingTo = int(16 + round( -Me.bearing(Dest)/(PI/8) + (PI/16) ))%16;
      
      //if farther than 6 inches to Destination
      if(distanceTo > 6 ){
        
        //what is the rotation direction and amount needed?
        int hdiff = (headingTo-Me.heading);
        if (hdiff==0){
          //I'm facing the correct(ish) way to the Destination point
          rotAmountTo = 0;
          if(timeToStop == 0 and timeToGo<millis()){
            //drive towards it.
            speed_L = throttle(LEFT_FWD, LEFT_STOP, 0.3);
            speed_R = throttle(RIGHT_FWD, RIGHT_STOP, 0.345); //compensate for rightward drift.
            timeToStop = millis() + 3000;
            timeToGo = timeToStop + 750;
          }
        }else if (hdiff < -8 || (0 < hdiff && hdiff < 8)){
          //Turn left
          rotAmountTo = (16+abs(hdiff))%16;
          if (rotAmountTo > 0){
            //turn left then wait a little bit
            if(timeToStop == 0 and timeToGo<millis()){
              speed_L = throttle(LEFT_REV, LEFT_STOP, 0.1);
              speed_R = throttle(RIGHT_FWD, RIGHT_STOP, 0.1);
              timeToStop = millis() + 200;
              timeToGo = timeToStop + 750;
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
              timeToStop = millis() + 200;
              timeToGo = timeToStop + 750;
            }
          }
        }
      }else{
        //withinrange of the current Destination
        //face the Target, don't turn your back
        headingTo = int(16 + round( -Me.bearing(Target)/(PI/8) + (PI/16) ))%16;
        int hdiff = (headingTo-Me.heading);
        if (hdiff==0){
          //I'm facing the correct way to the Target
          rotAmountTo = 0;
          //Do Nothing, wait for an opening
          
        }else if (hdiff < -8 || (0 < hdiff && hdiff < 8)){
          //Turn left
          rotAmountTo = (16+abs(hdiff))%16;
          if (rotAmountTo > 0){
            //turn left then wait a little bit
            if(timeToStop == 0 and timeToGo<millis()){
              speed_L = throttle(LEFT_REV, LEFT_STOP, 0.1);
              speed_R = throttle(RIGHT_FWD, RIGHT_STOP, 0.1);
              timeToStop = millis() + 100;
              timeToGo = timeToStop + 750;
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
              timeToStop = millis() + 100;
              timeToGo = timeToStop + 750;
            }
          }
        }
      }
      
    } //end valid Me position
    
    
    
  } //end game on
  
  
  //-------------------------------------------
  
  
  //Is it time to stop?  I consider this more of a reflex than thought.
  if(timeToStop > 0 && millis() >= timeToStop){
    Stop();
  }
  
  //if a bump switch is triggered backup
  if(!digitalRead(BUMP_SWITCH_RIGHT_PIN) || !digitalRead(BUMP_SWITCH_LEFT_PIN)){
    speed_L = LEFT_REV;
    speed_R = RIGHT_REV;
    timeToStop = millis() + 500;
    timeToGo = timeToStop + 750;
  }
    
  //Update Servo positions  
  if(millis() - timeLastServoUpdate > 15){  //limit servo updating to every 15ms at most
    LeftDrive.write(speed_L);
    RightDrive.write(speed_R);
    timeLastServoUpdate = millis();
  }
  
  //Output various statuses and values nce in a while.
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
}

boolean validPosition(RKF_Position pos){
  if(pos.x>0 || pos.y>0){
    return true;
  }
  else{
    return false;
  }
}

int throttle(int Direction, int Stop, float Throttle){
  return int((Direction-Stop)*Throttle + Stop);
}

byte whatPosClosest(RKF_Position p0){
  RKF_Position p1;
  byte index = 0;
  byte closest = 255;
  for(byte i=0; i<4; i++){
    p1.x = Pos[i][0];
    p1.y = Pos[i][1];
    byte d = p0.distance(p1);
    if(d < closest){
      closest = d;
      index = i;
    }
  }
  return index;
}

//Which position is farther the one to the left or right?
byte whichPosFarther(RKF_Position p0, byte index){
  RKF_Position p1;
  
  byte L = (index+1)%4;
  p1.x = Pos[L][0];
  p1.y = Pos[L][1];
  byte Ld = p0.distance(p1);
  
  byte R = (index-1)%4;
  p1.x = Pos[R][0];
  p1.y = Pos[R][1];
  byte Rd = p0.distance(p1);
  
  if(Ld > Rd){
    return L;
  }else{
    return R;
  }
}

//Which position is farthest the one to the left or right?
byte whichPosFarthest(RKF_Position p0){
  RKF_Position p1;
  byte index = 0;
  byte farthest = 0;
  for(byte i=0; i<4; i++){
    p1.x = Pos[i][0];
    p1.y = Pos[i][1];
    byte d = p0.distance(p1);
    if(d > farthest){
      farthest = d;
      index = i;
    }
  }
  return index;
}
