
#include <VirtualWire.h>
#include "rkf_packet.h"

#define BAUD_RATE 2400

RKF_Packet rxBuffer;
uint8_t rxLen;



int rxCount;
int avg_rx_period;
long rx_time;  


void setup()
{
  Serial.begin(57600);	// Debugging only
  Serial.println("setup:");

  // Initialise the IO and ISR
  vw_set_rx_pin(11); // data RX pin
  vw_setup(BAUD_RATE);    // Bits per sec
  vw_rx_start();     // Start the receiver PLL running
  rxCount = 0;
  

  Serial.print("rx baud: ");
  Serial.println(BAUD_RATE);


  // wait here and do nothing
  // until we recieve one valid message
  rxLen = sizeof(RKF_Packet);
  while(!vw_get_message((uint8_t*)&rxBuffer, &rxLen));
  rx_time = millis();
  avg_rx_period = 0;

}


void loop()
{
   int i;
    
    rxLen = sizeof(RKF_Packet);
    if(vw_get_message((uint8_t*)&rxBuffer, &rxLen))
    {
        digitalWrite(13, true);
        
        avg_rx_period += (millis() - rx_time);
        avg_rx_period >> 1;
        rx_time = millis();

	// Message with a good checksum received, dump it.
        Serial.print("rx ");
        Serial.print(rxCount++);
	Serial.print(" ");
        Serial.print(rxLen);
        Serial.print(" bytes:");
	
	for (i = 0; i < rxLen; i++)
	{
	    Serial.print(*((uint8_t*)&rxBuffer + i), HEX);
	    Serial.print(" ");
	}

        
	Serial.println("");


        printPacket(rxBuffer);
        
        Serial.print("avg_rx_period: ");
        Serial.println(avg_rx_period);

        //
        delay(100);
        digitalWrite(13, false);

//        //blink the LED the value of each byte of the message
//	for (i = 0; i < rxBufLen; i++)
//	{
//	    while(rxBuffer[i]--)
//            {
//              digitalWrite(13, true);
//              delay(100);
//              digitalWrite(13, false);
//              delay(100);
//            }
//            
//            delay(200);
//	}
         
    }
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
