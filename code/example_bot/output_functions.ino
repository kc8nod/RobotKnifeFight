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
  Serial.println("Help Menu:");
  Serial.println("----------------");
  Serial.println("H\tDisplay this Help Menu");
  Serial.println("S\tStop");
  Serial.println("F[#]\tGo Forward for # ms or forever if not set");
  Serial.println("B[#]\tGo Backward for # ms or forever if not set");
  Serial.println("L[#]\tGo Left for # ms or forever if not set");
  Serial.println("R[#]\tGo Right for # ms or forever if not set");
  Serial.println("SL####\tSet Left Servo to a timing between 1000 and 2000 ms");
  Serial.println("SR####\tSet Right Servo to a timing between 1000 and 2000 ms");
  Serial.println("?\tDisplay important status variables");
  Serial.println("----------------");
}
