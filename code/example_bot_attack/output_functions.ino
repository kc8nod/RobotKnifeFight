/*
  outputStatus
output important variables to the Serial port
------------------------------------------------------------------------------*/
void outputStatus(){
  Serial.println();
  unsigned long now = millis();
  Serial.print("now:");
  Serial.print(now);
  Serial.print("\ttimeToStop:");
  if (timeToStop > now)
  {
    Serial.print(timeToStop-now);
  }else{
    Serial.print(0);
  }
  
  Serial.print("\ttimeLastMessage:");
  Serial.print(now-timeLastMessage);
  Serial.println();
  
  Serial.print("Me[");
  Serial.print(MY_BOT_ID);
  Serial.print("]: (");
  Serial.print(Me.x);
  Serial.print(", ");
  Serial.print(Me.y);
  Serial.print(") H");
  Serial.print(Me.heading);
  Serial.print("\tL:");
  Serial.print(speed_L);
  Serial.print("\tR:");
  Serial.print(speed_R);
  Serial.println();
  
  Serial.print("Target: (");
  Serial.print(Target.x);
  Serial.print(", ");
  Serial.print(Target.y);
  Serial.print(") H");
  Serial.print(Target.heading);
  Serial.println();
  
  Serial.print("\tdistanceTo:");
  Serial.print(distanceTo);
  Serial.print("\theadingTo:");
  Serial.print(headingTo);
  Serial.print("\trotAmountTo:");
  Serial.print(rotAmountTo);

  Serial.println();
  
  for (byte i=0; i<4; i++){
    outputBotStatus(i);
    Serial.println();
  } 
  Serial.println();
        
}


/*
  outputHelp
output the help menu to the Serial port
------------------------------------------------------------------------------*/
void outputHelp(){
  Serial.println("Help Menu:");
  Serial.println("--------------------------");
  Serial.println("'H'\tDisplay this Help Menu");
  Serial.println("'?'\tDisplay important status variables");
  Serial.println();
}


/*
  outputBotStatus
*/
void outputBotStatus(byte i){
  Serial.print(i);
  Serial.print(":(");
  Serial.print(radio.packet.robot[i].x);
  Serial.print(",");
  Serial.print(radio.packet.robot[i].y);
  Serial.print(") H");
  Serial.print(radio.packet.robot[i].heading);
  Serial.print(" ");
  Serial.print(radio.packet.robot[i].alive);
}
