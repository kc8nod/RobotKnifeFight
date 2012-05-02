#include <Arduino.h>
#include <Print.h>

#define DEFAULT_BAUD_RATE  2400
#define DEFAULT_RX_PIN       11

class RKF_Position
{
  public:
  
  byte x;
  byte y;
  byte heading : 4;
  byte valid   : 1;
 
  int distance(RKF_Position&);
  double bearing(RKF_Position&);
};


class RKF_Packet : public Printable
{
  public:
  
  union
  {
    byte data[27];
    
    struct
    {
      byte message;
      byte reserved0;
      byte reserved1;
    
      RKF_Position robot[8]; 
    };
    
  };
 
  size_t printTo(Print&) const;
};

class RKF_Radio
{
  public:
  
  RKF_Packet packet;

  int start();
  int start(int);
  int start(int, int);
  int recv();
};
