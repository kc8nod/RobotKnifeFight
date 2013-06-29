#include <Arduino.h>
#include <Print.h>

#define DEFAULT_BAUD_RATE  4800
#define DEFAULT_RX_PIN       11
#define RKF_POSITION_MESSAGE 0
#define RKF_MAX_BOTS 4

class RKF_Position
{
  public:
  
  byte x;
  byte y;
  byte heading : 4;
  byte fresh   : 1;
  byte alive   : 1;
 
  int distance(RKF_Position&);
  double bearing(RKF_Position&);
};


class RKF_Packet 
{
  public:
  
  union
  {
    byte data[13];
    
    struct
    {
      byte message : 4;
      byte reserved : 4;
      RKF_Position robot[RKF_MAX_BOTS];
    }; 
  };
};

class RKF_Radio : public Printable
{
  public:
  
  RKF_Packet packet;

  int start();
  int start(int);
  int start(int, int);
  int recv();
  size_t printTo(Print&) const;
};
