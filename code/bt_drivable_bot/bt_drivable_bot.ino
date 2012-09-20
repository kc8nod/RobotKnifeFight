#include "bot_id.h"
#include <ServoTimer2.h>
#include <VirtualWire.h>
#include "pin_defs.h"
#include "RKF_Radio.h"

unsigned long now = 0;
unsigned long timeLastServoUp = 0;
unsigned long timeLastSerialInput = 0;

ServoTimer2 LeftDrive, RightDrive;

#define LEFT_FWD 2000
#define LEFT_STOP 1500
#define LEFT_REV 1000

#define RIGHT_FWD 1000
#define RIGHT_STOP 1500
#define RIGHT_REV 2000

unsigned int speed_L = LEFT_STOP;
unsigned int speed_R = RIGHT_STOP;

RKF_Radio radio;
unsigned long timeLastMessage = 0;
unsigned int message_count = 0;
extern "C" { uint8_t vw_rx_active; };

String serialInputString = "";  // a string to hold incoming data


void setup() {
  LeftDrive.attach(LEFT_DRIVE_PIN);
  RightDrive.attach(RIGHT_DRIVE_PIN);
  
  pinMode(BUMP_SWITCH_RIGHT_PIN, INPUT);
  pinMode(BUMP_SWITCH_LEFT_PIN, INPUT);
  pinMode(DEFAULT_RX_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  LeftDrive.write(LEFT_STOP);
  RightDrive.write(RIGHT_STOP);
  
  radio.start();
  
  serialInputString.reserve(16);
  Serial.begin(115200);
  Serial.println("bt_drivable_bot: reset");
}


void loop() {
  
  now = millis();
  
  if(processSerialInput()){
    timeLastSerialInput = now;
  }
   //if no new data for 5 seconds, stop.
  if(now - timeLastSerialInput > 5000){
    speed_L = LEFT_STOP;
    speed_R = RIGHT_STOP;
  }
  
  //Update Servo positions  
  if(now - timeLastServoUp > 15){  //limit servo updating to every 15ms at most
    LeftDrive.write(speed_L);
    RightDrive.write(speed_R);
    timeLastServoUp = now;
  }
  
}
//end loop()


/*
  outputStatus
output important variables to the Serial port
------------------------------------------------------------------------------*/
void outputStatus(){
  Serial.print("now:");
  Serial.print(now);
  Serial.print(", L:");
  Serial.print(speed_L);
  Serial.print(", R:");
  Serial.print(speed_R);
  //Serial.print(", vw_rx_active:");
  //Serial.print(vw_rx_active);
  //Serial.print(", radio_count:");
  //Serial.println(message_count);
  //Serial.println(radio);
}


void setLeftSpeed(boolean forward, int throttle){
  if(0 <= throttle && throttle <= 100){
    int x = (forward) ? LEFT_FWD : LEFT_REV;
    speed_L = (x - LEFT_STOP) * float(throttle)/100 + LEFT_STOP;
    //Serial.println(speed_L);
  }else{
    error_bad_param(throttle);
  }
}


void setRightSpeed(boolean forward, int throttle){
  if(0 <= throttle && throttle <= 100){
    int x = (forward) ? RIGHT_FWD : RIGHT_REV;
    speed_R = (x - RIGHT_STOP) * float(throttle)/100 + RIGHT_STOP;
    //Serial.println(speed_R);
  }else{
    error_bad_param(throttle);
  }
}

void error_bad_param(int param){
  Serial.print(" ERROR BAD PARAM: \"");
  Serial.print(param);
  Serial.println("\"");
}

