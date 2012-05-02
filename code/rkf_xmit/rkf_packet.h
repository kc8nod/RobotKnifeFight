

class RKF_Position
{
  public:
  
  byte x;
  byte y;
  byte heading : 4;
  byte valid   : 1; 
};


class RKF_Packet
{
  public:
  
  union
  {
    byte data[27];
    
    struct
    {
      byte command;
      byte reserved0;
      byte reserved1;
      
      RKF_Position robot[8];
    };
    
  };
};

