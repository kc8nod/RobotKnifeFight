#include "bot_id.h"
#include "pin_defs.h"
#include "drive_actions.h"
#include <Servo.h> 

Servo LeftDrive, RightDrive;

#define RIGHT_FF 0
#define RIGHT_STOP 93
#define RIGHT_FR 180
byte RightDrivePos = RIGHT_STOP;  //The right drive systems "throttle", 0=full forward, 90=stopped, 180=full reverse
boolean RightForward = false;
boolean RightReverse = false;

#define LEFT_FF 180
#define LEFT_STOP 93
#define LEFT_FR 0
byte LeftDrivePos = LEFT_STOP;    //The right drive systems "throttle", 180=full forward, 90=stopped, 0=full reverse
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
  
  Serial.begin(9600);
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
    TimeServos = Time;
  }
  
  
  //Thought Processes
  /*****************************/
  
  //Go forward until you hit something
  if(!digitalRead(BUMP_SWITCH_RIGHT_PIN)){
    //hit something
    if(LeftForward || RightForward || (!LeftForward && !RightForward && !LeftReverse && !RightReverse)){
      StopDrive();
      DriveActionQue_Clear();
      
      DriveActionQue_Add(GO_BACKWARD,250); //Backward for 250ms
      DriveActionQue_Add(SWEEP_RIGHT,150); //Sweep Right for 150ms
      DriveActionQue_Add(SWEEP_LEFT,500); //Sweep Left for 500ms
      DriveActionQue_Add(GO_FORWARD,30000 ); //Forward for 30000ms
    }
  }
  
  if(!digitalRead(BUMP_SWITCH_LEFT_PIN)){
    //hit something
    if(LeftForward || RightForward || (!LeftForward && !RightForward && !LeftReverse && !RightReverse)){
      StopDrive();
      DriveActionQue_Clear();
    
      DriveActionQue_Add(GO_BACKWARD,250); //Backward for 250ms
      DriveActionQue_Add(SWEEP_LEFT,150); //Sweep Left for 150ms
      DriveActionQue_Add(SWEEP_RIGHT,500); //Sweep Right for 500ms
      DriveActionQue_Add(GO_FORWARD,30000); //Forward for 30000ms
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
      LeftDrivePos = (LeftDrivePos < LEFT_FF) ? (LeftDrivePos+1) : LEFT_FF;
    }else if(LeftReverse){
      LeftDrivePos = (LeftDrivePos > LEFT_FR) ? (LeftDrivePos-1) : LEFT_FR;
    }else{
      LeftDrivePos = LEFT_STOP;
    }
    
    
    //Figure out the new Right position if needed
    if(RightForward && RightReverse){
      RightForward = RightReverse = false; //This shouldn't happen...
    }
    if(RightForward){
      RightDrivePos = (RightDrivePos > RIGHT_FF)?(RightDrivePos-1):RIGHT_FF;
    }else if(RightReverse){
      RightDrivePos = (RightDrivePos < RIGHT_FR)?(RightDrivePos+1):RIGHT_FR;
    }else{
      RightDrivePos = RIGHT_STOP;
    }
    
    //Update the servo positions.
    LeftDrive.write(LeftDrivePos);
    RightDrive.write(RightDrivePos);
}


