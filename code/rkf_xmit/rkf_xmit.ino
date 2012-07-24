#include <SerialCommand.h>
#include <VirtualWire.h>
#include "rkf_packet.h"

#define LED_PIN 13
#define TX_PIN  12

uint16_t tx_msg_count;

SerialCommand sCmd;     // The SerialCommand object

unsigned char tx_enabled_flag = 0;
unsigned long xmit_time = 0;
RKF_Packet send_pkt_buffer;
int send_data_len;

int xmit_period = 100;
int baud_rate   = 4800;

void (*softReset) (void) = 0; // soft reset function


void setup() {
    
  memset((void*)&send_pkt_buffer, 0, sizeof(send_pkt_buffer));

  pinMode(LED_PIN, OUTPUT);      // Configure the onboard LED for output
  digitalWrite(LED_PIN, LOW);    // default to LED off

  Serial.begin(115200);
  
  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("pos",   set_position);  // send position report for a robot
  sCmd.addCommand("start", start_tx);      // start transmitting
  sCmd.addCommand("stop", stop_tx);      // start transmitting
  sCmd.addCommand("reset", resetXmit);
  sCmd.addCommand("data",  send_raw_data);
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't defined

  Serial.println("rkf_xmit: Ready");
  printHelp();
}



void loop()
{
  
  sCmd.readSerial();     // process serial commands
  
  if(millis() >= xmit_time && tx_enabled_flag)
  {
    xmit_time = millis() + xmit_period;
    
    // transmit the packet
    vw_wait_tx();
    vw_send((uint8_t*) &send_pkt_buffer, send_data_len);

    Serial.print("\n\rtx ");
    Serial.println(tx_msg_count++);
    printPacket(send_pkt_buffer);
  }
  
  digitalWrite(LED_PIN, vx_tx_active() ? HIGH : LOW);
}


void resetXmit(){
  Serial.println("RESET");
  Serial.println("");
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

  Serial.println("SET POSITION");
  printPacket(send_pkt_buffer);
  
}

void send_raw_data()
{
  Serial.println("SEND RAW");
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
  if(arg != NULL)
  {
    xmit_period = atoi(arg);
  }
  
  arg = sCmd.next();
  if(arg != NULL)
  {
    baud_rate = atoi(arg);
  }
  
  vw_set_tx_pin(TX_PIN);
  vw_setup(baud_rate);

  send_data_len = sizeof(send_pkt_buffer);
  tx_enabled_flag = 1;
  
  Serial.print("START xmit: period=");
  Serial.print(xmit_period);
  Serial.print("ms baud=");
  Serial.print(baud_rate);
  Serial.println("bps");
   
  printPacket(send_pkt_buffer);

}

void stop_tx(){
  tx_enabled_flag = 0;
  Serial.println("STOP");
}


// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.print("INVALID COMMAND: ");
  Serial.println(command);
}


void printPacket(RKF_Packet &pkt)
{
  int idx;
  
  Serial.print("command type: ");
  Serial.println(pkt.command, HEX);
  
  for (idx = 0; idx < 8; idx++)
  {
    Serial.print(idx);
    Serial.print(": ");
    Serial.print(pkt.robot[idx].x);
    Serial.print(", ");
    Serial.print(pkt.robot[idx].y);
    Serial.print(", ");
    Serial.print(pkt.robot[idx].heading);
    Serial.print(", ");
    Serial.println(pkt.robot[idx].valid);
  }
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
  Serial.println("----------------");
}
