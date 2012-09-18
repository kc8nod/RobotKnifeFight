 
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
