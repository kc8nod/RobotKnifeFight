/*
  outputStatus
output important variables to the Serial port
------------------------------------------------------------------------------*/
void outputStatus(){
  Serial.print("now:");
  Serial.print(now);
  Serial.print("\ttimeToStop:");
  if (timeToStop > now)
  {
    Serial.print(timeToStop-now);
  }else{
    Serial.print(0);
  }
  Serial.print("\tL:");
  Serial.print(speed_L);
  Serial.print("\tR:");
  Serial.print(speed_R);
  Serial.println();
  
  for (byte i=0; i<8; i++){
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
//  Serial.println("S\tStop");
//  Serial.println("F[#]\tGo Forward for # ms or forever if not set");
//  Serial.println("B[#]\tGo Backward for # ms or forever if not set");
//  Serial.println("L[#]\tGo Left for # ms or forever if not set");
//  Serial.println("R[#]\tGo Right for # ms or forever if not set");
//  Serial.println("SL####\tSet Left Servo to a timing between 1000 and 2000 ms");
//  Serial.println("SR####\tSet Right Servo to a timing between 1000 and 2000 ms");
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
  Serial.print(radio.packet.robot[i].valid);
}
