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
     print("FL,");
   }else if(up && right){
     print ("FR,");
   }else if(down && left){
     print ("RL,");
   }else if(down && right){
     print ("RR,");
   }else if(up){
     print ("FWD,"+speed+";");
     port.write("FWD,"+speed+";");
   }else if(down){
     print ("REV,"+speed+";");
     port.write("REV,"+speed+";");
   }else if(left){
     print ("LEFT,"+speed+";");
     port.write("LEFT,"+speed+";");
   }else if(right){
     print ("RIGHT,"+speed+";");
     port.write("RIGHT,"+speed+";");
   }
 }


 void keyPressed() {
   if(keyCode > 0){
     println("pressed: "+ keyCode +" = '"+ key +"'");
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
     println("released: "+ keyCode +" = '"+ key +"'");
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
