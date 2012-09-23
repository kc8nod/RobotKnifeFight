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
   text(listOfPorts[0], 10, 30);
   println("Input serial port [0 - "+(listOfPorts.length-1)+"]:");
   
 }
 
 void draw() 
 {
   background(0);
   
   //Select an available Serial Port.
   if(serialSelected == false && serialConnected == false){
     text("Select serial port [0 - "+(listOfPorts.length-1)+"]:", 10, 15);
     for(int i=0; i < listOfPorts.length; i++){
       text("["+i+"]", 20, ((i*15)+30));
       text(listOfPorts[i], 40, ((i*15)+30));
     }
     
     if(keyPressed){
       COM_INDEX = int(key)-48;
       if(0 <= COM_INDEX && COM_INDEX < listOfPorts.length){
         println(COM_INDEX);
         serialSelected = true;
       }
     }
     return;
   }
   
   //Connect to the Selected Serial Port.
   if(serialSelected == true && serialConnected == false){
     port = new Serial(this, Serial.list()[COM_INDEX], 115200);
     serialConnected = true;
     print("Connected to:");
     println(Serial.list()[COM_INDEX]);
   }
   
   printSerialData();
  
   
      
   // Draw the shapes
   stroke(100);
   fill(100);
   drawCentered();
   drawUp();
   drawDown();
   drawRight();
   drawLeft();
   drawButton1();
   drawButton2();
   drawThrottle();
   
   stroke(255);
   fill(255); 
   if(up){drawUp();}
   if(down){drawDown();}
   if(left){drawLeft();}
   if(right){drawRight();}
   if(button1){drawButton1();}
   if(button2){drawButton2();}
   
   if(up && left){
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
     switch(key){
       case 'w':
       case 'W':
         up = true;
         break;
         
       case 'a':
       case 'A':
         left = true;
         break;
         
       case 's':
       case 'S':
         down = true;
         break;
         
       case 'd':
       case 'D':
         right = true;
         break;
       
       case 'q':
       case 'Q':
         speed += 2;
         speed = constrain(speed, 0, 100);
         button2 = true;
         break;
         
       case 'e':
       case 'E':
         speed -= 2;
         speed = constrain(speed, 0, 100);
         button1 = true;
         break;  
         
       case ' ':
         up = down = left = right = false;
         speed = 0;
         break;
     }     
   }
 }
 
 
 void keyReleased() {
   if(keyCode > 0){
     //println("released: "+ keyCode +" = '"+ key +"'");
     switch(key){
       case 'w':
       case 'W':
         up = false;
         break;
         
       case 'a':
       case 'A':
         left = false;
         break;
         
       case 's':
       case 'S':
         down = false;
         break;
         
       case 'd':
       case 'D':
         right = false;
         break;
         
       case 'q':
       case 'Q':
         button2 = false;
         break;
         
       case 'e':
       case 'E':
         button1 = false;
         break;
     } 
   }
 }
