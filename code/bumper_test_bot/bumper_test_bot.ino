#include "bot_id.h"
#include "pin_defs.h"
#include "drive_actions.h"
#include <Servo.h> 

Servo LeftDrive, RightDrive;

#define RIGHT_FF 0  //Full Forward
#define RIGHT_STOP 93
#define RIGHT_FR 180 //Full Reverse
byte RightDrivePos = RIGHT_STOP;  //The right drive systems "throttle"
boolean RightForward = false;
boolean RightReverse = false;

#define LEFT_FF 180 //Full Forward
#define LEFT_STOP 93
#define LEFT_FR 0  //Full Reverse
byte LeftDrivePos = LEFT_STOP;    //The left drive systems "throttle"
boolean LeftForward = false;
boolean LeftReverse = false;

boolean StatusLED = false;

unsigned long Time = 0; //the time the current loop was started
unsigned long TimeServos = 0; //the last time the position of the drive and sensor servos were updated
unsigned long TimeStatusLED = 0; //the last time the Status LED was updated
unsigned long TimeEndDriveAction = 0;  //the next time to stop all drive system actions

unsigned int DriveActionQue[5][2] = {{0,0},{0,0},{0,0},{0,0},{0,0}}; // {drive command, length of command time}
byte DAQ_ReadIndex = 0;
byte DAQ_WriteIndex = 0;

void setup(){
  LeftDrive.attach(LEFT_DRIVE_PIN);
  RightDrive.attach(RIGHT_DRIVE_PIN);
  pinMode(BUMP_SWITCH_RIGHT_PIN, INPUT);
  pinMode(BUMP_SWITCH_LEFT_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  Serial.begin(115200);
}


void loop(){
  Time = millis();

  //decode the current action
  LeftForward = LeftReverse = RightForward = RightReverse = false;
  switch(DriveActionQue[DAQ_ReadIndex][0]){
    case STOP:   break;
    case GO_FORWARD:   LeftForward = RightForward = true;  break;
    case GO_BACKWARD:  RightReverse = LeftReverse = true;  break;
    case SWEEP_LEFT:   RightForward = LeftReverse = true;  break;
    case SWEEP_RIGHT:  LeftForward = RightReverse = true;  break;
    case PIVOT_LEFT_FORWARD:    RightForward = true;  break;
    case PIVOT_LEFT_BACKWARD:   RightReverse = true;  break;
    case PIVOT_RIGHT_FORWARD:   LeftForward = true;   break;
    case PIVOT_RIGHT_BACKWARD:  LeftReverse = true;   break;
  }
  
  if(TimeEndDriveAction > 0){
    //a timer is set
    //has is expired?
    if(Time >= TimeEndDriveAction){
      DriveActionQue[DAQ_ReadIndex][0] = STOP;
      DriveActionQue[DAQ_ReadIndex][1] = STOP;
      TimeEndDriveAction = 0;
      
      DAQ_ReadIndex = (DAQ_ReadIndex == 4) ? 0 : (DAQ_ReadIndex+1); //next drive action que position
      
      if(DriveActionQue[DAQ_ReadIndex][0] == STOP){
        StopDrive();
      }
    }
  }else{
    //set the timer
    TimeEndDriveAction = Time + DriveActionQue[DAQ_ReadIndex][1];
  }

  
  //Blink the Status LED ever 1000ms
  if((Time - TimeStatusLED) > 1000){
    StatusLED = !StatusLED;
    digitalWrite(STATUS_LED_PIN, StatusLED);
    TimeStatusLED = Time;
  }
  
  
  //Update the Servos postion ever 15ms
  if((Time - TimeServos) > 15){
    setServoPostions();
    
    //Update the servo positions.
    LeftDrive.write(LeftDrivePos);
    RightDrive.write(RightDrivePos);
    
    TimeServos = Time;
  }
  
  
  //Thought Processes
  /*****************************/
  
  //Right bumper hit something
  if(!digitalRead(BUMP_SWITCH_RIGHT_PIN)){
    //if Going forward or stopped, but not backing up
    if(LeftForward || RightForward || (!LeftForward && !RightForward && !LeftReverse && !RightReverse)){
      StopDrive();
      DriveActionQue_Clear();
      
      DriveActionQue_Add(GO_BACKWARD,150); //Backward
      DriveActionQue_Add(SWEEP_RIGHT,250); //Sweep Right, comment this out to make less aggressive
      DriveActionQue_Add(SWEEP_LEFT,500); //Sweep Left
      DriveActionQue_Add(GO_FORWARD,30000 ); //Forward
    }
  }
  
  //Left bumper hit something
  if(!digitalRead(BUMP_SWITCH_LEFT_PIN)){
    //if Going forward or stopped, but not backing up
    if(LeftForward || RightForward || (!LeftForward && !RightForward && !LeftReverse && !RightReverse)){
      StopDrive();
      DriveActionQue_Clear();
    
      DriveActionQue_Add(GO_BACKWARD,150); //Backward
      DriveActionQue_Add(SWEEP_LEFT,250); //Sweep Left, comment this out to make less aggressive
      DriveActionQue_Add(SWEEP_RIGHT,500); //Sweep Right
      DriveActionQue_Add(GO_FORWARD,30000); //Forward
    }
  }

}

/********************************************************************************************************/
void DriveActionQue_Clear(){
  for(int i=0; i<5; i++){
    DriveActionQue[i][0] = STOP;
    DriveActionQue[i][1] = STOP;
  }
  TimeEndDriveAction = 0;
  DAQ_WriteIndex = DAQ_ReadIndex = 0;
}


void DriveActionQue_Add(int command, int duration){
  if(DriveActionQue[DAQ_WriteIndex][0] == STOP){
    DriveActionQue[DAQ_WriteIndex][0] = command;
    DriveActionQue[DAQ_WriteIndex][1] = duration;
    DAQ_WriteIndex = (DAQ_WriteIndex==4) ? 0 : (DAQ_WriteIndex+1);
  }else{
    //Command Buffer Overflow... command goes in the bit bucket
  }
}


void StopDrive(){
  RightForward = LeftForward = RightReverse = LeftReverse = false; //cancel "actions"
  setServoPostions();
}


void setServoPostions(){
    //Figure out the new Left position if needed
    if(LeftForward && LeftReverse){
      LeftForward = LeftReverse = false; //This shouldn't happen...
    }
    if(LeftForward){
      LeftDrivePos = LEFT_FF;
    }else if(LeftReverse){
      LeftDrivePos = LEFT_FR;
    }else{
      LeftDrivePos = LEFT_STOP;
    }
    
    
    //Figure out the new Right position if needed
    if(RightForward && RightReverse){
      RightForward = RightReverse = false; //This shouldn't happen...
    }
    if(RightForward){
      RightDrivePos = RIGHT_FF;
    }else if(RightReverse){
      RightDrivePos = RIGHT_FR;
    }else{
      RightDrivePos = RIGHT_STOP;
    }
}


