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
  processSerialInput
process and serial input until a \n is recieved.  Then parse the input for a
valid command.  Then reset the input string
------------------------------------------------------------------------------*/
void processSerialInput(){
  while (Serial.available()) {  //check for any debug command
    char inChar = (char)Serial.read(); 
    serialInputString += inChar;  // add it to the inputString
    if ((inChar=='\n' || inChar=='\r') && serialInputString.length() > 1) {
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
        unsigned int param = getIntFromCommand(serialInputString, 1);
        if(param > 0){
          timeToStop = now + param;
        }
        
      }else if(serialInputString.startsWith("B")){ //Go Backward
        speed_L = LEFT_REV;
        speed_R = RIGHT_REV;
        unsigned int param = getIntFromCommand(serialInputString, 1);
        if(param > 0){
          timeToStop = now + param;
        }
        
      }else if(serialInputString.startsWith("L")){ //Turn Left
        speed_L = LEFT_REV;
        speed_R = RIGHT_FWD;
        unsigned int param = getIntFromCommand(serialInputString, 1);
        if(param > 0){
          timeToStop = now + param;
        }
        
      }else if(serialInputString.startsWith("R")){ //Turn Right
        speed_L = LEFT_FWD;
        speed_R = RIGHT_REV;
        unsigned int param = getIntFromCommand(serialInputString, 1);
        if(param > 0){
          timeToStop = now + param;
        }
        
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
