 import processing.serial.*; 
 
 float boxX;
 float boxY;
 int boxSize = 20;
 int boxMargin = 3;
 boolean mouseOverBox = false;
 
 Serial port; 
 
 int val;      // Data received from the serial port
 
 void setup()  {
   size(200, 200);
   boxX = width/2.0;
   boxY = height/2.0;
   rectMode(RADIUS); 
   
   // List all the available serial ports in the output pane. 
   // You will need to choose the port that the Arduino board is 
   // connected to from this list. The first port in the list is 
   // port #0 and the third port in the list is port #2. 
   println(Serial.list()); 
 
   // Open the port that the Arduino board is connected to (in this case #0) 
   // Make sure to open the port at the same speed Arduino is using (9600bps) 
   int COM_INDEX = 1;
   port = new Serial(this, Serial.list()[COM_INDEX], 115200); 
   print("Connected to:");
   println(Serial.list()[COM_INDEX]);
 }
 
 void draw() 
 { 
   background(0);
 
   stroke(153);
   fill(153);
     
   // Draw the box
   drawCentered();
   drawUp();
   drawDown();
   drawRight();
   drawLeft();
   
   stroke(255); 
   noFill();  
   if (keyPressed){
     switch(key){
       case 'w':
       case 'W':
         drawUp();
         port.write(key);
         val = port.read();
         println(key + " = " + char(val));
         break;
       case 'a':
       case 'A':
         drawLeft();
         port.write(key);
         val = port.read();
         println(key + " = " + char(val));
         break;
       case 's':
       case 'S':
         drawDown();
         port.write(key);
         val = port.read();
         println(key + " = " + char(val));
         break;
       case 'd':
       case 'D':
         drawRight();
         port.write(key);
         val = port.read();
         println(key + " = " + char(val));
         break;
     }     
   } else {
     drawCentered();
     port.write('x');
   }
 }
 
 void drawUp(){
   triangle(boxX,(boxY-2.5*boxSize-boxMargin), (boxX-boxSize),(boxY-boxSize-boxMargin), (boxX+boxSize),(boxY-boxSize-boxMargin)); //Up
 }
 
 void drawDown(){
   triangle(boxX,(boxY+2.5*boxSize+boxMargin), (boxX-boxSize),(boxY+boxSize+boxMargin), (boxX+boxSize),(boxY+boxSize+boxMargin)); //Down
 }
 
 void drawRight(){
   triangle((boxX+2.5*boxSize+boxMargin),boxY, (boxX+boxSize+boxMargin),(boxY-boxSize), (boxX+boxSize+boxMargin),(boxY+boxSize)); //Right
 }
 
 void drawLeft(){
   triangle((boxX-2.5*boxSize-boxMargin),boxY, (boxX-boxSize-boxMargin),(boxY-boxSize), (boxX-boxSize-boxMargin),(boxY+boxSize)); //Left
 }
 
 void drawCentered(){
   rect(boxX, boxY, boxSize, boxSize);
 }
 
