#include <math.h>
#include <VirtualWire.h>

#include "RKF_Radio.h"


int RKF_Position::distance(RKF_Position& target)
{
  int deltaX, deltaY;

  deltaX = int(target.x) - int(this->x);
  deltaY = int(target.y) - int(this->y);

  return sqrt(deltaX * deltaX + deltaY * deltaY);
}  


double RKF_Position::bearing(RKF_Position& target)
{
  int deltaX, deltaY;

  deltaX = int(target.x) - int(this->x);
  deltaY = int(target.y) - int(this->y);

  return atan2(double(deltaY), double(deltaX));
}


int RKF_Radio::start()
{
  return this->start(DEFAULT_BAUD_RATE, DEFAULT_RX_PIN);
}


int RKF_Radio::start(int baud)
{
  return this->start(baud, DEFAULT_RX_PIN);
}


int RKF_Radio::start(int baud, int rx_pin)
{
  // Initialise the IO and ISR
  vw_set_rx_pin(rx_pin);
  vw_setup(baud);
  vw_rx_start();
}


int RKF_Radio::recv()
{
  uint8_t dataLen = sizeof(this->packet);
  
  return vw_get_message((uint8_t*)&(this->packet), &dataLen);
}


size_t RKF_Packet::printTo(Print& prn) const
{
  int idx;
  
  prn.print("message: ");
  prn.println(this->message, HEX);
  
  switch(this->message)
  {
    case 1:
      for (idx = 0; idx < 8; idx++)
      {
        prn.print(idx);
        prn.print(": ");
        prn.print(this->robot[idx].x);
        prn.print(", ");
        prn.print(this->robot[idx].y);
        prn.print(", ");
        prn.print(this->robot[idx].heading);
        prn.print(", ");
        prn.println(this->robot[idx].valid);
      }
      break;
      
    default:
      for(idx=0; idx<sizeof(RKF_Packet); idx++)
      {
        prn.print(this->data[idx], HEX);
        prn.print(" ");
      }
      break;
  }
}
