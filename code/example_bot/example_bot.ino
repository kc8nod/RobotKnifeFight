#include "bot_id.h"
#include <ServoTimer2.h>
#include <VirtualWire.h>
#include "pin_defs.h"
#include "RKF_Radio.h"

unsigned long now = 0;

ServoTimer2 LeftDrive, RightDrive;
#define LEFT_FWD 2000
#define LEFT_STOP 1467
#define LEFT_REV 1000
#define RIGHT_FWD 1000
#define RIGHT_STOP 1458
#define RIGHT_REV 2000
unsigned int speed_L = LEFT_STOP;
unsigned int speed_R = RIGHT_STOP;
unsigned long timeLastServoUp = 0;
unsigned long timeToStop = 0;


RKF_Radio radio;
unsigned long timeLastMessage = 0;
unsigned int radio_count = 0;

extern "C" { uint8_t vw_rx_active; };

#define DEBUG true
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
  
  speed_L = LEFT_STOP;
  speed_R = RIGHT_STOP;
  LeftDrive.write(speed_L);
  RightDrive.write(speed_R);
  
  radio.start();
  
  //Blink 3 time to indicate a reset
  for(int i=0; i<3; i++){
    digitalWrite(STATUS_LED_PIN, true);
    delay(100);
    digitalWrite(STATUS_LED_PIN, false);
    delay(100);
  }
  
  if(DEBUG){
    serialInputString.reserve(16);
    Serial.begin(57600);
    Serial.println("example_bot: Reset");
    outputHelp();
  }
}


/* 
  Main loop
------------------------------------------------------------------------------*/
void loop(){
  now = millis();
  
  if(radio.recv()){  //if a radio message has been received
    timeLastMessage = now;
    
    if(DEBUG){outputStatus();}
    
    switch(radio.packet.message)  //Do stuff based on message type.  
    {
      case 0:  // position message 
      default: 
        break;  // do nothing    
    }
  }
  
  if(vw_rx_active){  //if radio is busy receiving a message
    
  }
  
  digitalWrite(STATUS_LED_PIN, (now - timeLastMessage < 1000));  //Turn off status LED if no messages for 1 second
  
  if(DEBUG){processSerialInput();}
  
  //"Thought process"
  //if a bump switch is triggered backup until it is not triggered then stop
  if(!digitalRead(BUMP_SWITCH_RIGHT_PIN) || !digitalRead(BUMP_SWITCH_LEFT_PIN)){
    speed_L = LEFT_REV;
    speed_R = RIGHT_REV;
    timeToStop = now + 200; //reverse for 200ms
  }
  if(timeToStop > 0 && now > timeToStop){
    speed_L = LEFT_STOP;
    speed_R = RIGHT_STOP;
    timeToStop = 0;
  }
  
   
  //Update Servo positions  
  if(now - timeLastServoUp > 15){  //limit servo updating to at  least 15ms
    LeftDrive.write(speed_L);
    RightDrive.write(speed_R);
    timeLastServoUp = now;
  }  
}
/*----------------------------------------------------------------------------*/


/*
  getIntFromCommand
trim the command from the input string and convert what is left to an integer
------------------------------------------------------------------------------*/
unsigned int getIntFromCommand(String strInput, byte comlen){
  strInput = strInput.substring(comlen);
  char buf[16];
  strInput.toCharArray(buf, 16);
  return atoi(buf);
}


/*
  outputStatus
output important variables to the Serial port
------------------------------------------------------------------------------*/
void outputStatus(){
  Serial.print("now:");
  Serial.print(now);
  Serial.print(" timeToStop:");
  Serial.print(timeToStop);
  Serial.print(" L:");
  Serial.print(speed_L);
  Serial.print("\tR:");
  Serial.print(speed_R);
  Serial.print("\tvw_rx_active:");
  Serial.print(vw_rx_active);
  Serial.print("\tradio_count:");
  Serial.println(radio_count);
  Serial.println(radio);
}


/*
  outputHelp
output the help menu to the Serial port
------------------------------------------------------------------------------*/
void outputHelp(){
  Serial.println("Help Menu:\n----------------");
  Serial.println("H\tDisplay this Help Menu");
  Serial.println("S\tStop");
  Serial.println("F\tForward");
  Serial.println("B\tBackward");
  Serial.println("L\tLeft");
  Serial.println("R\tRight");
  Serial.println("SL####\tSet Left Servo to a timing between 1000 and 2000 ms");
  Serial.println("SR####\tSet Right Servo to a timing between 1000 and 2000 ms");
  Serial.println("----------------");
}


/*
  processSerialInput
process and serial input until a \n is recieved.  Then parse the input for a
valid command.  Then reset the input string
------------------------------------------------------------------------------*/
void processSerialInput(){
  while (Serial.available()) {  //check for any debug command
    char inChar = (char)Serial.read(); 
    serialInputString += inChar;  // add it to the inputString
    if (inChar == '\n' && serialInputString.length() > 1) {
      serialInputString.trim();
      serialInputString.toUpperCase();
      Serial.println(serialInputString);
      
      //Check if serialInputString is a command
      if(serialInputString.startsWith("SL")){ //set Left Servo speed/direction
        unsigned int param = getIntFromCommand(serialInputString, 2);
        if(1000 < param && param < 2000){
          speed_L = param;
        }else{
          Serial.println("--ERROR BAD PARAM--");
        }        
        
      }else if(serialInputString.startsWith("SR")){ //set Right Servo speed/direction
        unsigned int param = getIntFromCommand(serialInputString, 2);
        if(1000 < param && param < 2000){
          speed_R = param;
        }else{
          Serial.println("--ERROR BAD PARAM--");
        }
        
      }else if(serialInputString.startsWith("S")){ //Stop both Servos
        speed_L = LEFT_STOP;
        speed_R = RIGHT_STOP;
        
      }else if(serialInputString.startsWith("F")){ //Go Forward
        speed_L = LEFT_FWD;
        speed_R = RIGHT_FWD;
        
      }else if(serialInputString.startsWith("B")){ //Go Backward
        speed_L = LEFT_REV;
        speed_R = RIGHT_REV;
        
      }else if(serialInputString.startsWith("L")){ //Turn Left
        speed_L = LEFT_REV;
        speed_R = RIGHT_FWD;
        
      }else if(serialInputString.startsWith("R")){ //Turn Right
        speed_L = LEFT_FWD;
        speed_R = RIGHT_REV;
        
      }else if(serialInputString.startsWith("?")){ //"What are you thinking?"
        outputStatus(); //"Robot Stuff"
        
      }else if(serialInputString.startsWith("H")){ //Help Menu
        outputHelp();
        
      }else{
        Serial.print("INVALID COMMAND:\t");
        Serial.println(serialInputString);
      }
      
      serialInputString = "";  // clear the string
    }
  }
}



