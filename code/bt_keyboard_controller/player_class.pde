/**
Player Class
*/
 class Player {
   int id;
   int COM_INDEX;
   boolean serialSelected = false;
   boolean serialConnected= false;
   Serial port; 
  
   boolean up = false;
   boolean down = false;
   boolean left = false;
   boolean right = false;
   boolean button1 = false;
   boolean button2 = false;
   int speed = 100;
   
   int UP_KEY, DOWN_KEY, LEFT_KEY, RIGHT_KEY, B1_KEY, B2_KEY;
   
   PApplet parent;
   
   Player(PApplet tempParent, int tempId){
     parent = tempParent;
     id = tempId;
     switch(id){
       case 1:
         UP_KEY = int('W');
         DOWN_KEY = int('S');
         LEFT_KEY = int('A');
         RIGHT_KEY = int('D');
         B1_KEY = int('E');
         B2_KEY = int('Q');
         break;
        
       case 2:
         UP_KEY = int('I');
         DOWN_KEY = int('K');
         LEFT_KEY = int('J');
         RIGHT_KEY = int('L');
         B1_KEY = int('O');
         B2_KEY = int('U');
         break;
        
       case 3:
         UP_KEY = int('T');
         DOWN_KEY = int('G');
         LEFT_KEY = int('F');
         RIGHT_KEY = int('H');
         B1_KEY = int('Y');
         B2_KEY = int('R');
         break;
        
       case 4:
         UP_KEY = int('8');
         DOWN_KEY = int('5');
         LEFT_KEY = int('4');
         RIGHT_KEY = int('6');
         B1_KEY = int('9');
         B2_KEY = int('7');
         break;
    }
 }


  boolean serialPortConnected(){
    if(serialSelected == false){
      listOfPorts = Serial.list();
      textAlign(LEFT);
      int y_offset = 200 * (id-1);
      text("Select serial port [0 - "+(listOfPorts.length-1)+"]:", 10, 30+y_offset);
      for(int i=0; i < listOfPorts.length; i++){
        text("["+i+"]", 20, ((i*15)+45+y_offset));
        text(listOfPorts[i], 40, ((i*15)+45+y_offset));
      }
      if(typedKeyCode > 0){
        println("Select serial port [0 - "+(listOfPorts.length-1)+"]:");
        for(int i=0; i < listOfPorts.length; i++){
          println(" ["+i+"] "+listOfPorts[i]);
        }
        
        COM_INDEX = int(key)-48;
        if(0 <= COM_INDEX && COM_INDEX < listOfPorts.length){
          println(COM_INDEX);
          serialSelected = true;
        }
        typedKeyCode = 0;
     }
     return false;
   }else{
     //Connect to the Selected Serial Port.
     if(serialSelected == true && serialConnected == false){
       println("Selected "+listOfPorts[COM_INDEX]);
       port = new Serial(parent, listOfPorts[COM_INDEX], 115200);
       serialConnected = true;
       print("Connected to:");
       println(listOfPorts[COM_INDEX]);
     }
     return serialConnected;
   }
 }

 
 void printSerialData(){
   while (port.available() > 0) {
     String inBuffer = this.port.readString();   
     if (inBuffer != null) {
       println(inBuffer);
     }
   }
 }
 
 
 //Send command to port(s)
 void sendCommandToPort(){
   if(stopAll){
     speed = 0;
     port.write("STOP;");
   }else if(up && left){
     port.write("FWD "+(speed/10)+","+speed+";");
   }else if(up && right){
     port.write("FWD "+speed+","+(speed/10)+";");
   }else if(down && left){
     port.write("REV "+(speed/10)+","+speed+";");
   }else if(down && right){
     port.write("REV "+speed+","+(speed/10)+";");
   }else if(up){
     port.write("FWD "+speed+";");
   }else if(down){
     port.write("REV "+speed+";");
   }else if(left){
     port.write("LEFT "+speed+";");
   }else if(right){
     port.write("RIGHT "+speed+";");
   }else{
     port.write("STOP;");
   }
 }
 
//end class
}
