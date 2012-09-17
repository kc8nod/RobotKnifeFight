 import processing.serial.*; 
 
 float boxX;
 float boxY;
 int boxSize = 20;
 boolean mouseOverBox = false;
 
 Serial port; 
 
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
   port = new Serial(this, Serial.list()[0], 9600); 
 
 }
 
 void draw() 
 { 
   background(0);
 
   // Test if the cursor is over the box 
   if (mouseX > boxX-boxSize 
     && mouseX < boxX+boxSize 
     && mouseY > boxY-boxSize 
     && mouseY < boxY+boxSize)
   {
     mouseOverBox = true;  
     // draw a line around the box and change its color:
     stroke(255); 
     fill(153);
     // send an 'H' to indicate mouse is over square:
     port.write('H');       
     println("On");
   } else {
     // return the box to it's inactive state:
     stroke(153);
     fill(153);
     // send an 'L' to turn the LED off: 
     port.write('L'); 
     println("Off"); 
     mouseOverBox = false;
   }
 
   // Draw the box
   rect(boxX, boxY, boxSize, boxSize);
 }
 
