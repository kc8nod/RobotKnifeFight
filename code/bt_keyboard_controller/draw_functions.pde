void setActive(boolean active){
  if(active){
     stroke(255);
     fill(255);
   }else{
     stroke(100);
     fill(100);
   }
}
 
 void drawUp(boolean active){
   setActive(active);
   triangle(boxX,(boxY-2.5*boxSize-boxMargin), (boxX-boxSize),(boxY-boxSize-boxMargin), (boxX+boxSize),(boxY-boxSize-boxMargin)); //Up
 }
 
 void drawDown(boolean active){
   setActive(active);
   triangle(boxX,(boxY+2.5*boxSize+boxMargin), (boxX-boxSize),(boxY+boxSize+boxMargin), (boxX+boxSize),(boxY+boxSize+boxMargin)); //Down
 }
 
 void drawRight(boolean active){
   setActive(active);
   triangle((boxX+2.5*boxSize+boxMargin),boxY, (boxX+boxSize+boxMargin),(boxY-boxSize), (boxX+boxSize+boxMargin),(boxY+boxSize)); //Right
 }
 
 void drawLeft(boolean active){
   setActive(active);
   triangle((boxX-2.5*boxSize-boxMargin),boxY, (boxX-boxSize-boxMargin),(boxY-boxSize), (boxX-boxSize-boxMargin),(boxY+boxSize)); //Left
 }
 
 void drawCentered(boolean active){
   setActive(active);
   rect(boxX, boxY, boxSize, boxSize);
 }
 
 void drawButton1(boolean active){
   setActive(active);
   ellipse(button1X, button1Y, 2*boxSize, 2*boxSize);
 }
 
 void drawButton2(boolean active){
   setActive(active);
   ellipse(button2X, button2Y, 2*boxSize, 2*boxSize);
 }
 
 void drawThrottle(){
   stroke(100);
   fill(100);
   rect(throttleX, (throttleY+(100-speed)), boxSize/1.5, boxSize/3);
   rect(throttleX, height/2.0, boxSize/8, boxSize*3.0);
   stroke(255);
   fill(255);
   textAlign(CENTER);
   text(speed, throttleX, (throttleY+(100-speed)+6),1 );
 }
 
 void drawPort(){
   stroke(100);
   fill(100);
   rect(30, 20, boxSize+3, boxSize/2.5);
   stroke(255);
   fill(255);
   textAlign(CENTER);
   text(listOfPorts[COM_INDEX], 30, 25, 10);
 }
