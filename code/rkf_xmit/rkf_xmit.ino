#include <SerialCommand.h>
#include <VirtualWire.h>
#include "rkf_packet.h"

#define LED_PIN 13
#define TX_PIN  12

uint16_t tx_msg_count;
SerialCommand sCmd;     // The SerialCommand object
unsigned char tx_enabled_flag = 0;
long xmit_time = 0;
RKF_Packet send_pkt_buffer;

int xmit_period = 500;
int baud_rate   = 2400;

void (*softReset) (void) = 0; // soft reset function


void setup() {
  
  memset((void*)&send_pkt_buffer, 0, sizeof(send_pkt_buffer));

  pinMode(LED_PIN, OUTPUT);      // Configure the onboard LED for output
  digitalWrite(LED_PIN, LOW);    // default to LED off

  Serial.begin(57600);
  
  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("pos",   set_position);  // send position report for a robot
  sCmd.addCommand("start", start_tx);      // start transmitting
  sCmd.addCommand("reset", softReset);
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't defined
  Serial.println("Ready");
}



void loop()
{
  
  sCmd.readSerial();     // process serial commands
  
  if(millis() >= xmit_time
  && tx_enabled_flag)
  {
    xmit_time = millis() + xmit_period;
    
    // transmit the packet
    vw_wait_tx();
    vw_send((uint8_t*) &send_pkt_buffer, sizeof(send_pkt_buffer));

    Serial.print("tx ");
    Serial.println(tx_msg_count++);
  }
  
  digitalWrite(LED_PIN, vx_tx_active() ? HIGH : LOW);
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

  printPacket(send_pkt_buffer);
  
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

  tx_enabled_flag = 1;
  
  Serial.print("start xmit: period=");
  Serial.print(xmit_period);
  Serial.print("ms baud=");
  Serial.print(baud_rate);
  Serial.println("bps");
   
  printPacket(send_pkt_buffer);

}



// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("wat?");
}


void printPacket(RKF_Packet &pkt)
{
  int idx;
  
  Serial.print("command: ");
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
