 import processing.serial.*; 
 
 int typedKeyCode = 0; 
 
 float boxX;
 float boxY;
 int boxSize;
 int boxMargin;
 
 String[] listOfPorts;

 final int STOP_ALL_BOTS = int(' '); 
 boolean stopAll = false;
 
 boolean playersSelected = false;
 int numberOfPlayers = 0;
 final int maxPlayers = 2;
 
 Player[] player = new Player[maxPlayers];
  
 float button1X, button1Y, button2X, button2Y, throttleX, throttleY;
 
 void setup()  {
   size(400, (200*maxPlayers));
   boxX = 100;
   boxY = 100;
   button1X = 275;
   button1Y = 100;
   button2X = 340;
   button2Y = 100;
   throttleX = 200;
   throttleY = 100;
   rectMode(RADIUS); 
   
   for(int i=0; i<maxPlayers; i++){
     player[i] = new Player(this, i+1);
   }
 }
 
 
 void draw(){
   background(0);
   
   //Select the number of Players
   if(!selectPlayers()){
     return;
   }
   
   for(int i=0; i<numberOfPlayers; i++){
     drawPlayerLabel(i);
     
     //If not connected, select an available Serial Port and connect to it.
     if(!player[i].serialPortConnected()){
       return;
     }
     
     //Get any new serial data and print it.
     player[i].printSerialData();
     
     // Draw the shapes
     drawCentered(i);
     drawUp(i);
     drawDown(i);
     drawRight(i);
     drawLeft(i);
     drawButton1(i);
     drawButton2(i);
     drawThrottle(i);
     drawPort(i);
     
     //Send command to port(s)
     player[i].sendCommandToPort();
   }
 } 
 
 
 boolean selectPlayers(){
   if(playersSelected == false){
     text("Select Number of Players: [1-"+maxPlayers+"]", 10, 15);
     if(typedKeyCode > 0){
       println("Select Number of Players: [1-"+maxPlayers+"]");
       numberOfPlayers = int(key)-48;
       if(1 <= numberOfPlayers && numberOfPlayers <= maxPlayers){
         println(numberOfPlayers + " Players");
         playersSelected = true;
       }
       typedKeyCode = 0; //reset typed keyCode
     }
   }
   return playersSelected;
 }
 
 
  void keyPressed() {
   if(keyCode > 0){
     println("pressed: "+ keyCode +" = '"+ key +"'");
     for(int i=0; i<numberOfPlayers; i++){
       if(keyCode == player[i].UP_KEY){
         player[i].up = true;
         
       }else if(keyCode == player[i].LEFT_KEY){
         player[i].left = true;
         
       }else if(keyCode == player[i].DOWN_KEY){
         player[i].down = true;
         
       }else if(keyCode == player[i].RIGHT_KEY){
         player[i].right = true;
         
       }else if(keyCode == player[i].B2_KEY){
         player[i].speed += 2;
         player[i].speed = constrain(player[i].speed, 0, 100);
         player[i].button2 = true;
         
       }else if(keyCode == player[i].B1_KEY){
         player[i].speed -= 2;
         player[i].speed = constrain(player[i].speed, 0, 100);
         player[i].button1 = true;
         
       }else if(keyCode == STOP_ALL_BOTS){
         player[i].up = player[i].down = player[i].left = player[i].right = false;
         stopAll = true;
         player[i].speed = 0;  
       }
     }
   }
 }
 
 
 void keyReleased() {
   if(keyCode > 0){
     println("released: "+ keyCode +" = '"+ key +"'");
     for(int i=0; i<numberOfPlayers; i++){
       if(keyCode == player[i].UP_KEY){
         player[i].up = false;
         
       }else if(keyCode == player[i].LEFT_KEY){
         player[i].left = false;
           
       }else if(keyCode == player[i].DOWN_KEY){
         player[i].down = false;
           
       }else if(keyCode == player[i].RIGHT_KEY){
         player[i].right = false;
           
       }else if(keyCode == player[i].B2_KEY){
         player[i].button2 = false;
         
       }else if(keyCode == player[i].B1_KEY){
         player[i].button1 = false;
         
       }else if(keyCode == STOP_ALL_BOTS){
         stopAll = false;
         player[i].speed = 0;  
       }
     }
     typedKeyCode = keyCode; 
   }
 }
