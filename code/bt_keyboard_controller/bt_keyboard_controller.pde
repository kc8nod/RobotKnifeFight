 import processing.serial.*; 
 
 float boxX;
 float boxY;
 int boxSize;
 int boxMargin;
 
 float button1X;
 float button1Y;
 float button2X;
 float button2Y;
 float throttleX;
 float throttleY;
 
 Serial port; 
 int COM_INDEX = 0;
 boolean serialSelected = false;
 boolean serialConnected= false;
 String[] listOfPorts;

 final int STOP_ALL_BOTS = int(' '); 
 boolean stopAll = false;
 
 boolean up = false;
 boolean down = false;
 boolean left = false;
 boolean right = false;
 boolean button1 = false;
 boolean button2 = false;
 int speed = 100;
 
 void printSerialData(){
   while (port.available() > 0) {
     String inBuffer = port.readString();   
     if (inBuffer != null) {
       println(inBuffer);
     }
   }
 }
 
 void setup()  {
   size(400, 200);
   boxX = width/4.0;
   boxY = height/2.0;
   boxSize = 20;
   boxMargin = 3;
   button1X = 2.75*width/4.0;
   button1Y = height/2.0;
   button2X = 3.4*width/4.0;
   button2Y = height/2.0;
   throttleX = width/2.0;
   throttleY = height/4.0;
   rectMode(RADIUS); 
   listOfPorts = Serial.list();
   println(listOfPorts);
   println("Input serial port [0 - "+(listOfPorts.length-1)+"]:");
   
 }
 
 boolean selectSerialPort(){
   if(serialSelected == false){
     text("Select serial port [0 - "+(listOfPorts.length-1)+"]:", 10, 15);
     for(int i=0; i < listOfPorts.length; i++){
       text("["+i+"]", 20, ((i*15)+30), 1);
       text(listOfPorts[i], 40, ((i*15)+30), 1);
     }
     
     if(keyPressed){
       COM_INDEX = int(key)-48;
       if(0 <= COM_INDEX && COM_INDEX < listOfPorts.length){
         println(COM_INDEX);
         serialSelected = true;
       }
     }
   }
   return serialSelected;
 }
 
 void draw() 
 {
   background(0);
   
   //Select an available Serial Port.
   if(!selectSerialPort()){
     return;
   }
   
   //Connect to the Selected Serial Port.
   if(serialSelected == true && serialConnected == false){
     port = new Serial(this, listOfPorts[COM_INDEX], 115200);
     serialConnected = true;
     print("Connected to:");
     println(listOfPorts[COM_INDEX]);
   }
   
   //Get any new serial data and print it.
   printSerialData();
      
   // Draw the shapes
   drawCentered(stopAll);
   drawUp(up);
   drawDown(down);
   drawRight(right);
   drawLeft(left);
   drawButton1(button1);
   drawButton2(button2);
   drawThrottle();
   drawPort();
   
   //Send command to port(s)
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


 void keyPressed() {
   if(keyCode > 0){
     //println("pressed: "+ keyCode +" = '"+ key +"'");
     if(keyCode == PLAYER1_UP_KEY){
       up = true;
       
     }else if(keyCode == PLAYER1_LEFT_KEY){
       left = true;
       
     }else if(keyCode == PLAYER1_DOWN_KEY){
       down = true;
       
     }else if(keyCode == PLAYER1_RIGHT_KEY){
       right = true;
       
     }else if(keyCode == PLAYER1_B2_KEY){
       speed += 2;
       speed = constrain(speed, 0, 100);
       button2 = true;
       
     }else if(keyCode == PLAYER1_B1_KEY){
       speed -= 2;
       speed = constrain(speed, 0, 100);
       button1 = true;
       
     }else if(keyCode == STOP_ALL_BOTS){
       up = down = left = right = false;
       stopAll = true;
       speed = 0;  
     }
   }
 }
 
 
 void keyReleased() {
   if(keyCode > 0){
     //println("released: "+ keyCode +" = '"+ key +"'");
     if(keyCode == PLAYER1_UP_KEY){
       up = false;
       
     }else if(keyCode == PLAYER1_LEFT_KEY){
       left = false;
         
     }else if(keyCode == PLAYER1_DOWN_KEY){
       down = false;
         
     }else if(keyCode == PLAYER1_RIGHT_KEY){
       right = false;
         
     }else if(keyCode == PLAYER1_B2_KEY){
       button2 = false;
       
     }else if(keyCode == PLAYER1_B1_KEY){
       button1 = false;
       
     }else if(keyCode == STOP_ALL_BOTS){
       stopAll = false;
       speed = 0;  
     }
   }
 }
