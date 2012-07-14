#include "bot_id.h"
#include <ServoTimer2.h>
#include <VirtualWire.h>
#include "pin_defs.h"
#include "RKF_Radio.h"

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

RKF_Radio radio;
unsigned long timeLastMessage = 0;
unsigned int radio_count = 0;

String serialInputString = "";  // a string to hold incoming data
  
extern "C" { uint8_t vw_rx_active; };

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
    delay(200);
    digitalWrite(STATUS_LED_PIN, false);
    delay(200);
  }
  
  serialInputString.reserve(16);
  Serial.begin(57600);
  Serial.println("example_bot: Reset");
}


void loop(){
  unsigned long now = millis();
  if(radio.recv()){  //if a radio message has been received
    timeLastMessage = now;
    
    Serial.print("rx:");
    Serial.println(radio_count++);
    Serial.println(radio);
    
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
  
  while (Serial.available()) {  //check for any debug command
    char inChar = (char)Serial.read(); 
    serialInputString += inChar;  // add it to the inputString
    if (inChar == '\n' && serialInputString.length() > 1) {
      serialInputString.trim();
      serialInputString.toUpperCase();
      Serial.println(serialInputString);
      
      //Check if serialInputString is a command
      if(serialInputString.startsWith("SL")){ //set Left Servo speed/direction
        speed_L = getIntFromCommand(serialInputString);
        
      }else if(serialInputString.startsWith("SR")){ //set Right Servo speed/direction
        speed_R = getIntFromCommand(serialInputString);
        
      }else if(serialInputString.startsWith("F")){ //Go Forward
        speed_L = LEFT_FWD;
        speed_R = RIGHT_FWD;
        
      }else if(serialInputString.startsWith("B")){ //Go Backward
        speed_L = LEFT_REV;
        speed_R = RIGHT_REV;
        
      }else if(serialInputString.startsWith("TL")){ //Turn Left
        speed_L = LEFT_REV;
        speed_R = RIGHT_FWD;
        
      }else if(serialInputString.startsWith("TR")){ //Turn Right
        speed_L = LEFT_FWD;
        speed_R = RIGHT_REV;
        
      }else if(serialInputString.startsWith("?")){ //"What are you thinking?"
        //"Robot Stuff"
        Serial.print("L:");
        Serial.print(speed_L);
        Serial.print("\tR:");
        Serial.println(speed_R);
      }
      
      serialInputString = "";  // clear the string
    } 
  } 
  
  if(now - timeLastServoUp > 15){
    LeftDrive.write(speed_L);
    RightDrive.write(speed_R);
    timeLastServoUp = now;
  }  
}

unsigned int getIntFromCommand(String strInput){
  strInput = strInput.substring(2);
  char buf[16];
  strInput.toCharArray(buf, 16);
  return atoi(buf);
}






