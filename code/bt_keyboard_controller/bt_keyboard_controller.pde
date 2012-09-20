 import processing.serial.*; 
 
 float boxX;
 float boxY;
 int boxSize = 20;
 int boxMargin = 3;
 boolean mouseOverBox = false;
 
 Serial port; 
 
 int val;      // Data received from the serial port
 
 boolean up = false;
 boolean down = false;
 boolean left = false;
 boolean right = false;
 
 int speed = 0;
 
 void setup()  {
   size(200, 200);
   boxX = width/2.0;
   boxY = height/2.0;
   rectMode(RADIUS); 
   
   println(Serial.list()); 
 
   int COM_INDEX = 0;
   port = new Serial(this, Serial.list()[COM_INDEX], 115200); 
   print("Connected to:");
   println(Serial.list()[COM_INDEX]);
 }
 
 void draw() 
 {
   while (port.available() > 0) {
     String inBuffer = port.readString();   
     if (inBuffer != null) {
       println(inBuffer);
     }
   }
   background(0);
      
   // Draw the shapes
   stroke(100);
   fill(100);
   drawCentered();
   drawUp();
   drawDown();
   drawRight();
   drawLeft();
   
   stroke(255);
   fill(255); 
   if(up){drawUp();}
   if(down){drawDown();}
   if(left){drawLeft();}
   if(right){drawRight();}
   
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
         speed = 100;
         break;
         
       case 'a':
       case 'A':
         left = true;
         speed = 100;
         break;
         
       case 's':
       case 'S':
         down = true;
         speed = 100;
         break;
         
       case 'd':
       case 'D':
         right = true;
         speed = 100;
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
     } 
   }
 }
