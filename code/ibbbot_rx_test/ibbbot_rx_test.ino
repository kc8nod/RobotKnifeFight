
//#define VW_RX_ONLY
#include <VirtualWire.h>

uint8_t rxBuffer[VW_MAX_MESSAGE_LEN];
uint8_t rxBufLen = VW_MAX_MESSAGE_LEN;

void setup()
{
  Serial.begin(57600);	// Debugging only
  Serial.println("setup");

  // Initialise the IO and ISR
  vw_set_rx_pin(11); // data RX pin
  vw_setup(5000);    // Bits per sec
  vw_rx_start();     // Start the receiver PLL running


  // wait here and do nothing
  // until we recieve one valid message
  while(!vw_get_message(rxBuffer, &rxBufLen));

}


void loop()
{
    int i;  
    
    if(vw_get_message(rxBuffer, &rxBufLen))
    {
	// Message with a good checksum received, dump it.
	Serial.print("Got: ");
	
	for (i = 0; i < rxBufLen; i++)
	{
	    Serial.print(rxBuffer[i], HEX);
	    Serial.print(" ");
	}
	Serial.println("");

        //blink the LED the value of each byte of the message
	for (i = 0; i < rxBufLen; i++)
	{
	    while(rxBuffer[i]--)
            {
              digitalWrite(13, true);
              delay(100);
              digitalWrite(13, false);
              delay(100);
            }
            
            delay(200);
	}
         
    }
}
