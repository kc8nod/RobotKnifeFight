
#define RKF_MAX_BOTS 4

class RKF_Position
{
  public:
  
  byte x;
  byte y;
  byte heading : 4;
  byte valid   : 1;
  byte alive   : 1; 
};


class RKF_Packet
{
  public:
  
  union
  {
    byte data[13];
    
    struct
    {
      byte message  : 4;
      byte reserved : 4;      
      RKF_Position robot[RKF_MAX_BOTS];
    };
    
  };
};

