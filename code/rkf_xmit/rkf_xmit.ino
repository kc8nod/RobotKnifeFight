#include <SerialCommand.h>
#include <VirtualWire.h>
#include "rkf_packet.h"

#define LED_PIN 13
#define TX_PIN  12

uint16_t tx_msg_count;

SerialCommand sCmd;     // The SerialCommand object

boolean tx_enabled_flag = false;
unsigned long next_xmit_time = 0; //time of the next transmit
unsigned long time_xmit_complete = 0;
RKF_Packet send_pkt_buffer;
int send_data_len;

int xmit_period  = 100;
int xmit_min     = 50;
int baud_rate    = 4800;

void (*softReset) (void) = 0; // soft reset function


void setup() {
    
  memset((void*)&send_pkt_buffer, 0, sizeof(send_pkt_buffer));

  pinMode(LED_PIN, OUTPUT);      // Configure the onboard LED for output
  
  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("pos",   set_position);  // send position report for a robot
  sCmd.addCommand("start", start_tx);      // start transmitting
  sCmd.addCommand("stop", stop_tx);      // start transmitting
  sCmd.addCommand("reset", resetXmit);
  sCmd.addCommand("data",  send_raw_data);
  sCmd.addCommand("?",   send_printPacket);
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't defined
  
  Serial.begin(115200);
  Serial.println("rkf_xmit: reset");
  printHelp();
}


void loop()
{
  sCmd.readSerial();     // process serial commands
  
  if(tx_enabled_flag){
    unsigned long now = millis();
    if(now > next_xmit_time){
      next_xmit_time = now + xmit_period;
      time_xmit_complete = now + xmit_min;
      
      // transmit the packet
      vw_wait_tx();
      digitalWrite(LED_PIN, HIGH);
      vw_send((uint8_t*) &send_pkt_buffer, send_data_len);      
  
      Serial.print("TX:");
      Serial.print(tx_msg_count++);
      Serial.print(' ');
      printPacket(send_pkt_buffer);
    }
    
    if(now > time_xmit_complete){
      digitalWrite(LED_PIN, LOW);
    }
    
  }else{
    digitalWrite(LED_PIN, LOW);
  }
}


void resetXmit(){
  Serial.println("RESET");
  delay(500);
  softReset();
}

void set_position()
{
  byte index;
  byte x_pos;
  byte y_pos;
  byte heading;
  byte valid;
  RKF_Position * pRobot;
  
  index   = byte(atoi(sCmd.next()));
  x_pos   = byte(atoi(sCmd.next()));
  y_pos   = byte(atoi(sCmd.next()));
  heading = byte(atoi(sCmd.next()));
  valid   = byte(atoi(sCmd.next()));
  
  
  pRobot = &send_pkt_buffer.robot[index];
  
  pRobot->x       = x_pos;
  pRobot->y       = y_pos;
  pRobot->heading = heading;
  pRobot->valid   = valid;
  
  send_data_len = sizeof(send_pkt_buffer);

  Serial.print("SET POSITION: ");
  printPacket(send_pkt_buffer);
  
}


void send_raw_data()
{
  Serial.print("SEND RAW: ");
  char* arg;
  uint8_t* ptr = (uint8_t*)&send_pkt_buffer;
  send_data_len = 0;
  
  while(1)
  {
    arg = sCmd.next();
    if(arg == NULL) break;
   
    *ptr = byte(atoi(arg));
    ptr++;
   
    send_data_len++;
  }
}


void start_tx()
{
  char * arg;
  arg = sCmd.next();
  if(arg != NULL){
    xmit_period = atoi(arg);
  }
  
  arg = sCmd.next();
  if(arg != NULL){
    baud_rate = atoi(arg);
  }
  
  vw_set_tx_pin(TX_PIN);
  vw_setup(baud_rate);

  send_data_len = sizeof(send_pkt_buffer);
  tx_enabled_flag = true;
  
  Serial.print("START: xmit_period=");
  Serial.print(xmit_period);
  Serial.print("ms baud=");
  Serial.print(baud_rate);
  Serial.println("bps");
}


void stop_tx(){
  tx_enabled_flag = false;
  Serial.println("STOP ");
}


// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.print("INVALID COMMAND: ");
  Serial.println(command);
}


void printPacket(RKF_Packet &pkt)
{
  //Serial.print("command type: ");
  //Serial.println(pkt.command, HEX);
  for (int idx = 0; idx < 4; idx++)
  {
    if(idx>0){Serial.print(" ");}
    Serial.print('(');
    Serial.print(pkt.robot[idx].x);
    Serial.print(',');
    Serial.print(pkt.robot[idx].y);
    Serial.print(',');
    Serial.print(pkt.robot[idx].heading);
    Serial.print(',');
    Serial.print(pkt.robot[idx].valid);
    Serial.print(')');
  }
  Serial.print('\n');
}


/*
  printHelp
output the help menu to the Serial port
------------------------------------------------------------------------------*/
void printHelp(){
  Serial.println("Help Menu:");
  Serial.println("----------------");
  Serial.println("start [XMIT_PERIOD] [BAUD_RATE]");
  Serial.println("stop");
  Serial.println("pos ROBOT_NUM X Y HEADING VALID");
  Serial.println("reset");
  Serial.println("data BYTE0 [BYTE1] ... [BYTE26]");
  Serial.println("? - display packet buffer");
  Serial.println("----------------");
}

void send_printPacket(){
  Serial.print('?: ');
  printPacket(send_pkt_buffer);
}
