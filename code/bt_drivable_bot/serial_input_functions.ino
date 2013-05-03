/*
  getIntsFromString
trim the command from the input string and convert what is left to an integer
------------------------------------------------------------------------------*/
void getIntsFromString(int param[], String strInput, byte paramStart){
  strInput = strInput.substring(paramStart);
  int cp = -1;
  byte i = 0;
  byte len = 0;
  String strTmp;
  char buf[6];
  do{
    len = strInput.length();
    cp = strInput.indexOf(',');
    if(cp==-1){
      cp = len;
    }
    strTmp = strInput.substring(0, cp);
    strTmp.toCharArray(buf, 6);
    param[i] = atoi(buf);
    i++;
    strInput = strInput.substring(cp+1);
  }while(cp < len);
  if(i==1){
    param[i] = param[0];
  }
}


/*
  processSerialInput
process and serial input until a \n is recieved.  Then parse the input for a
valid command.  Then reset the input string
------------------------------------------------------------------------------*/
boolean processSerialInput(){
  boolean newdata = false;
  while (Serial.available()) {  //check for any debug command
    newdata = true;
    char inChar = (char)Serial.read(); 
    serialInputString += inChar;  // add it to the inputString
    if ((inChar==';' || inChar=='\n' || inChar=='\r') && serialInputString.length() > 1) {
      serialInputString.replace(';',' ');
      serialInputString.trim();
      serialInputString.toUpperCase();
      int param[2] = {-1,-1};
        
      //Check if serialInputString is a command
      if(serialInputString.startsWith("FWD")){ //Forward with speed
        getIntsFromString(param, serialInputString, 4);
        setLeftSpeed(true, param[0]);
        setRightSpeed(true, param[1]);        
        
      }else if(serialInputString.startsWith("REV")){ //Reverse with speed
        getIntsFromString(param, serialInputString, 4);
        setLeftSpeed(false, param[0]);
        setRightSpeed(false, param[1]);
        
      }else if(serialInputString.startsWith("RIGHT")){ //Rotate right with speed
        getIntsFromString(param, serialInputString, 6);
        setLeftSpeed(true, param[0]);
        setRightSpeed(false, param[1]);
        
      }else if(serialInputString.startsWith("LEFT")){ //Rotate left with speed
        getIntsFromString(param, serialInputString, 5);
        setLeftSpeed(false, param[0]);
        setRightSpeed(true, param[1]);
        
      }else if(serialInputString.startsWith("?")){ //"What are you thinking?"
        outputStatus(); //"Robot Stuff"
        
      }else if(serialInputString.startsWith("STOP")){ //Stop both Servos
        speed_L = LEFT_STOP;
        speed_R = RIGHT_STOP;
        
      }else{
        Serial.print("INVALID COMMAND: \"");
        Serial.print(serialInputString);
        Serial.println("\"");
      }
      
      serialInputString = "";  // clear the string
    }
  }
  digitalWrite(STATUS_LED_PIN, newdata);
  return newdata;
}
