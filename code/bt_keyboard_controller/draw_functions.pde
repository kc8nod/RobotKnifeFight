void setActive(boolean active){
  if(active){
     stroke(255);
     fill(255);
   }else{
     stroke(100);
     fill(100);
   }
}
 
 void drawUp(int i){
   setActive(player[i].up);
   int yOffset = (200*i);
   triangle(boxX,(boxY-2.5*boxSize-boxMargin+yOffset), (boxX-boxSize),(boxY-boxSize-boxMargin+yOffset), (boxX+boxSize),(boxY-boxSize-boxMargin+yOffset)); //Up
 }
 
 void drawDown(int i){
   setActive(player[i].down);
   int yOffset = (200*i);
   triangle(boxX,(boxY+2.5*boxSize+boxMargin+yOffset), (boxX-boxSize),(boxY+boxSize+boxMargin+yOffset), (boxX+boxSize),(boxY+boxSize+boxMargin+yOffset)); //Down
 }
 
 void drawRight(int i){
   setActive(player[i].right);
   int yOffset = (200*i);
   triangle((boxX+2.5*boxSize+boxMargin),boxY+yOffset, (boxX+boxSize+boxMargin),(boxY-boxSize+yOffset), (boxX+boxSize+boxMargin),(boxY+boxSize+yOffset)); //Right
 }
 
 void drawLeft(int i){
   setActive(player[i].left);
   int yOffset = (200*i);
   triangle((boxX-2.5*boxSize-boxMargin),boxY+yOffset, (boxX-boxSize-boxMargin),(boxY-boxSize+yOffset), (boxX-boxSize-boxMargin),(boxY+boxSize+yOffset)); //Left
 }
 
 void drawCentered(int i){
   setActive(stopAll);
   rect(boxX, boxY+(200*i), boxSize, boxSize);
 }
 
 void drawButton1(int i){
   setActive(player[i].button1);
   ellipse(button1X, button1Y+(200*i), 2*boxSize, 2*boxSize);
 }
 
 void drawButton2(int i){
   setActive(player[i].button2);
   ellipse(button2X, button2Y+(200*i), 2*boxSize, 2*boxSize);
 }
 
 void drawThrottle(int i){
   stroke(100);
   fill(100);
   rect(throttleX, (throttleY+(50-player[i].speed)+(200*i)), boxSize/1.5, boxSize/3);
   rect(throttleX, 100+(200*i), boxSize/8, boxSize*3.0);
   stroke(255);
   fill(255);
   textAlign(CENTER);
   text(player[i].speed, throttleX, (throttleY+(50-player[i].speed)+6+(200*i)));
 }
 
 void drawPort(int i){
   stroke(100);
   fill(100);
   rect(30, 30+(200*i), boxSize+3, boxSize/2.5);
   stroke(255);
   fill(255);
   textAlign(CENTER);
   text(listOfPorts[player[i].COM_INDEX], 30, 35+(i*200), 10);
 }
 
 
 void drawPlayerLabel(int i){
   textAlign(LEFT);
   text("Player "+player[i].id, 10, 15+(200*i));
 }
