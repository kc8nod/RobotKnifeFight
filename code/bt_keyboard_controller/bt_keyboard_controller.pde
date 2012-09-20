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
 
 int val;      // Data received from the serial port
 
 boolean up = false;
 boolean down = false;
 boolean left = false;
 boolean right = false;
 boolean button1 = false;
 boolean button2 = false;
 
 int speed = 100;
 
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
   
   println(Serial.list()); 
 
   int COM_INDEX = 0;
   //if(Serial.list()[COM_INDEX]){
     port = new Serial(this, Serial.list()[COM_INDEX], 115200);
   //}
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
