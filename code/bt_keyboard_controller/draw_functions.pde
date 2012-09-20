 
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
 
 void drawButton1(){
   ellipse(button1X, button1Y, 2*boxSize, 2*boxSize);
 }
 
 void drawButton2(){
   ellipse(button2X, button2Y, 2*boxSize, 2*boxSize);
 }
 
 void drawThrottle(){
   rect(throttleX, (throttleY+(100-speed)), boxSize/2, boxSize/4);
   rect(throttleX, height/2.0, boxSize/8, boxSize*3.0);
 }
 
 
