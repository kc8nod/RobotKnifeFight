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
     print ("F,");
   }else if(down){
     print ("R,");
   }else if(left){
     print ("L,");
   }else if(right){
     print ("R,");
   }
 }


 void keyPressed() {
   if(keyCode > 0){
     println("pressed: "+ keyCode +" = '"+ key +"'");
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
