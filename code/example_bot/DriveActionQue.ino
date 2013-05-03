unsigned int DriveActionQue[5][2] = {{0,0},{0,0},{0,0},{0,0},{0,0}}; // {drive command, length of command time}

void DriveActionQue_Add(int command, int duration){
  if(DriveActionQue[DAQ_WriteIndex][0] == STOP){
    DriveActionQue[DAQ_WriteIndex][0] = command;
    DriveActionQue[DAQ_WriteIndex][1] = duration;
    DAQ_WriteIndex = (DAQ_WriteIndex==4) ? 0 : (DAQ_WriteIndex+1);
  }else{
    //Command Buffer Overflow... command goes in the bit bucket
  }
}
