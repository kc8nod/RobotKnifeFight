#include <Servo.h> 

//pin 0 - avoid using for easier serial uploading
//pin 1 - avoid using for easier serial uploading
#define BUMP_SWITCH_RIGHT_PIN 2
#define RGB_LED_RED_PIN 3
#define BUMP_SWITCH_LEFT_PIN 4
#define RGB_LED_GREEN_PIN 5
#define RGB_LED_BLUE_PIN 6
//pin 7 not currently used
//pin 8 not currently used
#define RIGHT_DRIVE_PIN 9
#define LEFT_DRIVE_PIN 10
#define SENSOR_SERVO_PIN 11
//pin 12 not currently used
#define STATUS_LED_PIN 13
#define IR_RANGE_PIN A0
//A1 not currently used
//A2 not currently used
//A3 not currently used
//A4 not currently used
//A5 not currently used

Servo LeftDrive, RightDrive, SensorServo;

#define SENSOR_FL 10
#define SENSOR_MID 90
#define SENSOR_FR 170
byte SensorPos = SENSOR_MID;       //the IR range sensors pan servo position
boolean SensorLeft = false;
boolean SensorRight = false;

#define RIGHT_FF 0
#define RIGHT_STOP 90
#define RIGHT_FR 180
byte RightDrivePos = RIGHT_STOP;  //The right drive systems "throttle", 0=full forward, 90=stopped, 180=full reverse
boolean RightForward = false;
boolean RightReverse = false;

#define LEFT_FF 180
#define LEFT_STOP 90
#define LEFT_FR 0
byte LeftDrivePos = LEFT_STOP;    //The right drive systems "throttle", 180=full forward, 90=stopped, 0=full reverse
boolean LeftForward = false;
boolean LeftReverse = false;

boolean StatusLED = false;
byte ColorLED[3] = {0,0,0}; //0:red 1:green 2:blue

unsigned long Time = 0; //the time the current loop was started
unsigned long TimeServos = 0; //the last time the position of the drive and sensor servos were updated
unsigned long TimeStatusLED = 0; //the last time the Status LED was updated
unsigned long TimeEndDriveAction = 0;  //the next time to stop all drive system actions
unsigned long TimeSensor = 0; //the last time the Range Sensor was read from.

int DriveActionQue[5][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}}; //second dimension indexes: 0=drive command, 1=length of command time
byte DAQ_ReadIndex = 0;
byte DAQ_WriteIndex = 0;

float dataAvg = 0.0;

void setup(){
  LeftDrive.attach(LEFT_DRIVE_PIN);
  RightDrive.attach(RIGHT_DRIVE_PIN);
  SensorServo.attach(SENSOR_SERVO_PIN);
  pinMode(BUMP_SWITCH_RIGHT_PIN, INPUT);
  pinMode(BUMP_SWITCH_LEFT_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  Serial.begin(9600);
  
  //starting thoughts  
  SensorLeft = true; 
}


void loop(){
  Time = millis();
  
  //if there is a current action
  if(DriveActionQue[DAQ_ReadIndex][0] >= 0){
    //decode the action
    LeftForward = LeftReverse = RightForward = RightReverse = false;
    switch(DriveActionQue[DAQ_ReadIndex][0]){
      case 0:
        break;
      case 1:
        RightReverse = true;
        break;
      case 2:
        RightForward = true;
        break;
      case 4:
        LeftReverse = true;
        break;
      case 5:
        RightReverse = LeftReverse = true;
        break;
      case 6:
        RightForward = LeftReverse = true;
        break;
      case 8:
        LeftForward = true;
        break;
      case 9:
        LeftForward = RightReverse = true;
        break;
      case 10:
        LeftForward = RightForward = true;
        break;
    }
    
    if(TimeEndDriveAction > 0){
      //a timer is set
      //has is expired?
      if(Time >= TimeEndDriveAction){
        DriveActionQue[DAQ_ReadIndex][0] = -1;
        DriveActionQue[DAQ_ReadIndex][1] = -1;
        TimeEndDriveAction = 0;
        
        DAQ_ReadIndex = (DAQ_ReadIndex == 4) ? 0 : (DAQ_ReadIndex+1); //next que position
        
        if(DriveActionQue[DAQ_ReadIndex][0] == -1){
          StopDrive();
        }
      }
    }else{
      //set the timer
      TimeEndDriveAction = Time + DriveActionQue[DAQ_ReadIndex][1];
    }

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
  
  
  //Read IR Range Sensor ever 40ms
  if((Time - TimeSensor) > 40){
    float newdata = read_gp2d12_range(IR_RANGE_PIN);
    if(newdata > 0 && newdata <120){ //average only valid values 0-120cm
      dataAvg = (dataAvg + newdata)/2.0;
    }
    Serial.println(dataAvg);
    ColorLED[0] = 0;
    ColorLED[1] = 0;
    ColorLED[2] = 0;
    if(dataAvg < 0.0){
      //error with sensor    
    }else if(dataAvg < 20.0){
      ColorLED[0] = 255;
    }else if(dataAvg < 40.0){
      ColorLED[0] = 255;
      ColorLED[1] = 165;
    }else if(dataAvg < 60.0){
      ColorLED[0] = 255;
      ColorLED[1] = 255;
    }else if(dataAvg < 80.0){
      ColorLED[0] = 127;
      ColorLED[1] = 255;
    }else if(dataAvg < 100.0){
      ColorLED[1] = 255;
      ColorLED[2] = 255;
    }else if(dataAvg < 120.0){
      ColorLED[1] = 255;
    }
    analogWrite(RGB_LED_RED_PIN, ColorLED[0]);
    analogWrite(RGB_LED_GREEN_PIN, ColorLED[1]);
    analogWrite(RGB_LED_BLUE_PIN, ColorLED[2]);
    
    TimeSensor = Time;
  }
  
  
  //Thought Processes
  /*****************************/
  //Sensor: pan continuosly
  if(SensorPos == SENSOR_FL){
    SensorLeft = false;
    SensorRight = true;
  }else if(SensorPos == SENSOR_FR){
    SensorLeft = true;
    SensorRight = false;
  }
  
  //Go forward until you hit something
  if(!digitalRead(BUMP_SWITCH_RIGHT_PIN)){
    //hit something
    if(LeftForward || RightForward || (!LeftForward && !RightForward && !LeftReverse && !RightReverse)){
      StopDrive();
      DriveActionQue_Clear();
      
      DriveActionQue_Add(5,250); //Backward for 250ms
      DriveActionQue_Add(6,250); //Pivot Left for 250ms
      DriveActionQue_Add(10,5000); //Forward for 5000ms
    }
  }
  
  if(!digitalRead(BUMP_SWITCH_LEFT_PIN)){
    //hit something
    if(LeftForward || RightForward || (!LeftForward && !RightForward && !LeftReverse && !RightReverse)){
      StopDrive();
      DriveActionQue_Clear();
    
      DriveActionQue_Add(5,250); //Backward for 250ms
      DriveActionQue_Add(9,250); //Pivot Right for 250ms
      DriveActionQue_Add(10,5000); //Forward for 5000ms
    }
  }

}

/********************************************************************************************************/
void DriveActionQue_Clear(){
  for(int i=0; i<5; i++){
    DriveActionQue[i][0] = -1;
    DriveActionQue[i][1] = -1;
  }
  TimeEndDriveAction = 0;
  DAQ_WriteIndex = DAQ_ReadIndex = 0;
}

void DriveActionQue_Add(int command, int duration){
  if(DriveActionQue[DAQ_WriteIndex][0] == -1){
    DriveActionQue[DAQ_WriteIndex][0] = command;
    DriveActionQue[DAQ_WriteIndex][1] = duration;
    DAQ_WriteIndex = (DAQ_WriteIndex==4) ? 0 : (DAQ_WriteIndex+1);
  }else{
    //Command Buffer Overflow
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
    
    //Figure out the new Sensor position if needed
    if(SensorLeft && SensorRight){
      SensorLeft = SensorRight = false; //This shouldn't happen...
    }
    if(SensorLeft){
      SensorPos = (SensorPos > SENSOR_FL)?(SensorPos-1):SENSOR_FL;
    }else if(SensorRight){
      SensorPos = (SensorPos < SENSOR_FR)?(SensorPos+1):SENSOR_FR;
    }
    
    //Update the servo positions.
    LeftDrive.write(LeftDrivePos);
    RightDrive.write(RightDrivePos);
    SensorServo.write(SensorPos);
}


/* 
 read_gp2d12_range
 Function that reads a value from GP2D12 infrared distance sensor and returns a value in centimeters.
 This sensor should be used with a refresh rate of 36ms or greater.
 Javier Valencia 2008
 float read_gp2d12_range(byte pin)
 It can return -1 if something gone wrong.
 */

float read_gp2d12_range(byte pin) {
	int tmp;

	tmp = analogRead(pin);
	if (tmp < 3)
		return -1; // invalid value

	return (6787.0 /((float)tmp - 3.0)) - 4.0;
} 
