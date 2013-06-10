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
   triangle(boxX,(boxY-53+yOffset), (boxX-20),(boxY-23+yOffset), (boxX+20),(boxY-23+yOffset)); //Up
   fill(0);
   textAlign(CENTER);
   text(char(player[i].UP_KEY), boxX, boxY-28+yOffset);
 }
 
 void drawDown(int i){
   setActive(player[i].down);
   int yOffset = (200*i);
   triangle(boxX,(boxY+53+yOffset), (boxX-20),(boxY+23+yOffset), (boxX+20),(boxY+23+yOffset)); //Down
   fill(0);
   textAlign(CENTER);
   text(char(player[i].DOWN_KEY), boxX, boxY+39+yOffset);
 }
 
 void drawRight(int i){
   setActive(player[i].right);
   int yOffset = (200*i);
   triangle((boxX+53),boxY+yOffset, (boxX+23),(boxY-20+yOffset), (boxX+23),(boxY+20+yOffset)); //Right
   fill(0);
   textAlign(CENTER);
   text(char(player[i].RIGHT_KEY), boxX+33, boxY+5+yOffset);
 }
 
 void drawLeft(int i){
   setActive(player[i].left);
   int yOffset = (200*i);
   triangle((boxX-53),boxY+yOffset, (boxX-23),(boxY-20+yOffset), (boxX-23),(boxY+20+yOffset)); //Left
   fill(0);
   textAlign(CENTER);
   text(char(player[i].LEFT_KEY), boxX-33, boxY+5+yOffset);
 }
 
 void drawCentered(int i){
   setActive(stopAll);
   rect(boxX, boxY+(200*i), 20, 20);
 }
 
 void drawButton1(int i){
   setActive(player[i].button1);
   int yOffset = (200*i);
   ellipse(button1X, button1Y+yOffset, 40, 40);
   fill(0);
   textAlign(CENTER);
   text(char(player[i].B1_KEY), button1X, button1Y+5+yOffset);
 }
 
 void drawButton2(int i){
   setActive(player[i].button2);
   int yOffset = (200*i);
   ellipse(button2X, button2Y+yOffset, 40, 40);
   fill(0);
   textAlign(CENTER);
   text(char(player[i].B2_KEY), button2X, button2Y+5+yOffset);
 }
 
 void drawThrottle(int i){
   stroke(100);
   fill(100);
   int yOffset = (200*i);
   rect(throttleX, (throttleY+50-player[i].speed+yOffset), 13, 7);
   rect(throttleX, 100+yOffset, 3, 60);
   stroke(255);
   fill(255);
   textAlign(CENTER);
   text(player[i].speed, throttleX, (throttleY+56-player[i].speed+yOffset));
 }
 
 void drawPort(int i){
   stroke(100);
   fill(100);
   int yOffset = (200*i);
   rect(30, 30+yOffset, 35, 8);
   stroke(255);
   fill(255);
   textAlign(CENTER);
   text(listOfPorts[player[i].COM_INDEX], 30, 35+yOffset, 10);
 }
 
 
 void drawPlayerLabel(int i){
   textAlign(LEFT);
   text("Player "+player[i].id, 10, 15+(200*i));
 }
