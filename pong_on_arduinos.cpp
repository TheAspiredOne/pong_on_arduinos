//Avery Tan 
//Alden Tan  


#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

#define SD_CS    5  // Chip select line for SD card
#define TFT_CS   6  // Chip select line for TFT display
#define TFT_DC   7  // Data/command line for TFT
#define TFT_RST  8  // Reset line for TFT (or connect to +5V)

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//defining the colours
int Black = 0x0000;
int Red = 0xF800;
int Yellow = 0xFFE0;
int Orange = 0xFBE0;
int Green = 0xFE0;
int Blue = 0x1F;
int Pink = 0xF81F;
int Brown = 0x79E0;
int White = 0xFFFF;

//initializing default paddle colour
int ball_colour = White;
int paddle_colour = White;

//initializing LEDs
int SCORE_LED_[] = {2,3,4,10,11,12};
int LED[] = {LOW,LOW,LOW,LOW,LOW,LOW};


//initializing joystick constants
const int VERT = 0;
const int HORIZ = 1;  
const int SEL = 9;
//initializing paddleY constants
const int P1PaddleY = 10;
const int P2PaddleY = 150;
//initializing speaker/buzzer constants
const int speakerPin = 13;
const int period = 200;


int P1PaddleX = 64;
int P2PaddleX = 64;
int old_P1PaddleX;
int old_P2PaddleX;

//initial ball direction
int ballDirectionY = 1;
int ballDirectionX = 1;

//initializing values for joystick movement
int init_horiz,init_vert;
int delta_vert, delta_horiz;
int select;

int ballSpeed = 30; //lower numbers mean faster ballspeed

int oldBallX, oldBallY;
int ballX = 20;
int ballY=20;

//setting up the state machine
typedef enum {menuAKAServer, gameover, awaiting_player2, player1_client_start, ingame_player1, ingame_player2, singleplayer,gameover_AI } State;
State state;

int player1score;
int player2score;
int winner;

//function from assignment 1 to wait for number of bytes to come in from Serial3
bool wait_on_serial3( uint8_t nbytes, long timeout ) {
   unsigned long deadline = millis() + timeout;//wraparound not a problem
   while (Serial3.available()<nbytes && (timeout<0 || millis()<deadline)) {
      delay(1); // be nice, no busy loop
   }
   return Serial3.available()>=nbytes;
}

//function based on Arduino intro labs used to play sound on the buzzer
void playtone(int period, int duration){

   long elapsedTime = 0;
   int halfPeriod =  period/2;
   while(elapsedTime < duration*1000L){
      digitalWrite(speakerPin,HIGH);
      delayMicroseconds(halfPeriod);

      digitalWrite(speakerPin,LOW);
      delayMicroseconds(halfPeriod);

      elapsedTime = elapsedTime+period;
   }
}

//function to determine whether ball makes collision with paddle 1
boolean inP1Paddle(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight){
   boolean result = false;
   if ((x>=rectX && x <= (rectX + 20)) && (y >= rectY && y+5 <= (rectY + 10))){
      result = true;
   }
   return result;
}

//function to determine whether ball makes collision with paddle 2
boolean inP2Paddle(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight){
   boolean result = false;
   if ((x>=rectX && x <= (rectX + 20)) && (y >= rectY-5 && y <= (rectY ))){
      result = true;
   }
   return result;
}

//function to print out the single player game over screen
void gameover_protocol_AI(){
   tft.fillRect(0,0,128,160,0x0000);
   tft.setCursor(50,40);
   tft.setTextColor(0xFFFF);
   tft.setTextSize(2);
   tft.setRotation(3);
   if(player1score == 3){
      tft.print("AI is ");
      tft.setCursor(20,70);
      tft.print("the winner!");
   } 
   if(player2score == 3){
      tft.print("player 2 is the winner!"); //by some miracle
   }

   while(state  == gameover_AI){
      select = digitalRead(SEL);
      if(select == 0){
         tft.setRotation(0);

         //reinitialize the player scores
         player1score = 0;
         player2score = 0; 

         //turn all LEDs off
         for (int i = 0;i<6;i++){
            digitalWrite(SCORE_LED_[i], LOW);
            LED[i] = LOW;
         }
         P1PaddleX=64;
         P2PaddleX=64;

         state = menuAKAServer;
      }
   }
}

//function to check if threshold score reached and to light up the LEDs
int check_score(){
   int i;

   //turning on the respective LEDs
   if(player1score == 1){
      LED[0]=HIGH;
   }

   if(player1score == 2){
      LED[1]=HIGH;
   }

   if(player1score == 3){
      LED[2] = HIGH;
   }

   if(player2score == 1){
      LED[3]=HIGH;
   }

   if(player2score == 2){
      LED[4]=HIGH;
   }

   if(player2score == 3){
      LED[5] = HIGH;
   }

   for(i = 0;i<6;i++){
      digitalWrite(SCORE_LED_[i],LED[i]);
   }

   //determining if threshold score has been reached
   if(player1score >= 3){
      if(state == singleplayer){
         state = gameover_AI;
      }
      else{
      winner = 1;
      state = gameover;
      return winner;
      }
   }

   if(player2score>=3){
      winner = 2;
      state = gameover;
      return winner;
   }
}

//function to moveball in multiplayer and writes the X and Y coordinates to Serial3
void moveBall(){

   //collision with the horizontal walls
   if(ballX>123||ballX<0){
      ballDirectionX = -ballDirectionX;
   }

   //collision with the boundary behin player 1
   if(ballY>155){
      player1score++;
      Serial3.write('1'); //key so that the other Arduino can figure out what the next value sent over will be for
      Serial3.write(player1score);

      //check_score turns on LEDs and determines if the threshold score(3) has been reached yet
      check_score();
      tft.fillRect(0,155,128,10,0x0000);
      delay(500);

      //repositions the ball
      ballY = 50;
      ballX = 50;

   }

   //collision with the wall behind player 2
   if(ballY<0){
      player2score++;
      Serial3.write('2');
      Serial3.write(player2score);
      check_score();
      tft.fillRect(0,0,128,10,0x0000);
      delay(500);
      ballX = 50;
      ballY = 50;
      ballDirectionX = -ballDirectionX;
      ballDirectionY = -ballDirectionY;
   }

   //tracking old ball positions to print over them later
   oldBallY = ballY;
   oldBallX = ballX;

   ballX += ballDirectionX;
   ballY += ballDirectionY;

   //printing the new ball and covering the old position with the background colour
   tft.fillRect(oldBallX, oldBallY, 5, 5, Black);
   tft.fillRect(ballX, ballY, 5, 5, ball_colour);

   //if there is change in ball position, send the new position to the other Arduino
   if(oldBallX != ballX || oldBallY != ballY){
      Serial3.write('Z');//this is a key so that th eother Arduino knows what the next two values are for
      Serial3.write(ballX);
      Serial3.write(ballY);
   }
}

//function to move player 2's paddle
void movepaddle2(){
   //only one direction needed
   int horizontal;
   horizontal = analogRead(HORIZ);
   delta_horiz = horizontal - init_horiz;


   //initializing the paddle movement mechanics
   old_P2PaddleX = P2PaddleX;
   P2PaddleX = P2PaddleX + delta_horiz/350;

   //setting up boundaries for the paddle
   if (P2PaddleX < 0){P2PaddleX = 0;}
   if (P2PaddleX > 108){P2PaddleX = 108;}

   //reprinting only part of the paddle and not the entire thing
   int shadearea = abs(P2PaddleX - old_P2PaddleX);
   if(old_P2PaddleX != P2PaddleX){
      if(P2PaddleX<old_P2PaddleX){
         tft.fillRect(old_P2PaddleX, P2PaddleY, shadearea, 5, paddle_colour);
         tft.fillRect(old_P2PaddleX+20, P2PaddleY, shadearea, 5, Black);
      }
      if(P2PaddleX>old_P2PaddleX){
         tft.fillRect(P2PaddleX-1, P2PaddleY,shadearea,5,Black);
         tft.fillRect(P2PaddleX+19, P2PaddleY,shadearea,5,paddle_colour);
      }

      //paddle position has changed so send new values to the other arduino
      Serial3.write('h');
      Serial3.write(P2PaddleX);
   }
}

//function to move player 1's paddle
void movepaddle1(){
   //only one direction needed
   int horizontal;
   horizontal = analogRead(HORIZ);
   delta_horiz = horizontal - init_horiz;


   //initializing the paddle movement mechanics
   old_P1PaddleX = P1PaddleX;
   P1PaddleX = P1PaddleX + delta_horiz/350;

   //setting up boundaries
   if (P1PaddleX <0){P1PaddleX = 0;}
   if (P1PaddleX > 108){P1PaddleX = 108;}

   //reprinting onlt part of the paddle
   int shadearea = abs(P1PaddleX - old_P1PaddleX);
   if(old_P1PaddleX != P1PaddleX){
      if(P1PaddleX<old_P1PaddleX){
         tft.fillRect(old_P1PaddleX, P1PaddleY, shadearea, 5, paddle_colour);
         tft.fillRect(old_P1PaddleX+20, P1PaddleY, shadearea, 5, Black);
      }
      if(P1PaddleX>old_P1PaddleX){
         tft.fillRect(P1PaddleX-1, P1PaddleY,shadearea,5,Black);
         tft.fillRect(P1PaddleX+19, P1PaddleY,shadearea,5,paddle_colour);
      }

      //sending new paddle position to the other arduino
      Serial3.write('Q'); //key 
      Serial3.write(P1PaddleX);
   }
}

//code for the CLIENT's role in a multiplayer game
int ingame_player1_protocol(){
   if(Serial3.available()){
      char key = Serial3.read();//this key determines how the sequential data will be used
      if (key == 'h'){
         if(wait_on_serial3(1,1000) == true){
            P2PaddleX = int(Serial3.read());
         }
      }

      if(key == 'w'){
         playtone(period,10);
      }

      if (key =='Z'){
         if(wait_on_serial3(2,1000) == true){
            ballX = int(Serial3.read());
            ballY = int(Serial3.read());
         }
      }
      if(key == '1'){
         player1score++;
         check_score();
      }
      if(key =='2'){
         player2score++;
         check_score();
      }
   }
   
   //printing the updated ball position
   if(oldBallX!= ballX || oldBallY!=ballY){
      tft.fillRect(oldBallX, oldBallY, 5, 5, 0x0000);
      tft.fillRect(ballX, ballY, 5, 5, ball_colour);
   }

   //printing the other Arduino's updated paddle position 
   if(old_P2PaddleX != P2PaddleX){
      tft.fillRect(old_P2PaddleX, P2PaddleY, 20, 5, 0x0000);
      tft.fillRect(P2PaddleX, P2PaddleY, 20, 5, paddle_colour);
   }

   //moving your own paddle
   if (millis()%ballSpeed<2){
      movepaddle1();
   }

   //keeping track of the old positions
   old_P2PaddleX=P2PaddleX;
   oldBallY = ballY;
   oldBallX = ballX;
}

//code for the SERVER's role in a multiplayer game
int ingame_player2_protocol(){
   char key = Serial3.read(); //this key determines what the sequential data will be used for
   if (key == 'Q'){
      if(wait_on_serial3(1,1000) == true){
         P1PaddleX = int(Serial3.read());
      }
   }
   
   //printing the other Arduino's paddle
   if(old_P1PaddleX != P1PaddleX){
      tft.fillRect(old_P1PaddleX, P1PaddleY, 20, 5, Black);
      tft.fillRect(P1PaddleX, P1PaddleY, 20, 5, paddle_colour);
   }

   //server controlls the ball
   if(millis()%ballSpeed < 1){ //this function slows down the ball's speed. The ball would be too fast otherwise
      moveBall();
   }

   //move the server's paddle
   if (millis()%ballSpeed <2){
      movepaddle2();
   }

   //collision detection between ball and paddle
   if(millis()%ballSpeed*5<2){
      if(inP1Paddle(ballX, ballY, P1PaddleX,P1PaddleY, 5,20)){
         if(ballDirectionY!= 1){ballDirectionY=1;}
         Serial3.write('w');
         playtone(period,10);
      }

      if(inP2Paddle(ballX, ballY, P2PaddleX,P2PaddleY, 5,20)){
         Serial3.write('w');
         playtone(period,10);
         if(ballDirectionY != -1){ballDirectionY = -1;}
      }
   }

   old_P1PaddleX = P1PaddleX;
}

//code for playing single player
void in_game_vs_AI_protocol(){
   //ball tracker for AI
   P1PaddleX = ballX;

   //move the AI paddle
   if(old_P1PaddleX != P1PaddleX){
      tft.fillRect(old_P1PaddleX, P1PaddleY, 20, 5, Black);
      tft.fillRect(P1PaddleX, P1PaddleY, 20, 5, paddle_colour);
   }

   //move the ball
   if(millis()%ballSpeed < 2){
         moveBall();
   }

   //move your own padddle
   if (millis()%ballSpeed <2){
      movepaddle2();
   }

   //collision detection between ball and paddle
   if(inP1Paddle(ballX, ballY, P1PaddleX,P1PaddleY, 5,20)){
      if(ballDirectionY!=1){ballDirectionY=1;}
      playtone(period,10);
   }

   if(inP2Paddle(ballX, ballY, P2PaddleX,P2PaddleY, 5,20)){
      
      if(ballDirectionY!=-1){ballDirectionY = -1;}
      playtone(period,10);
   }

   old_P1PaddleX = P1PaddleX;
}

//code for the starting main menu
void drawMenu(){

   int index, old_index;
   int selection, old_selection;
   selection = 40;
   
   //creating the start-up menu
   char options[4][20];
   strcpy(options[0],"Single Player");
   strcpy(options[1],"Multiplayer");
   strcpy(options[2],"Ball Colour");
   strcpy(options[3],"Paddle Colour");
   index = 0;
   
   //creating the colour menu
   int color_index, old_color_index;
   char color[8][10];
   strcpy(color[0],"Red");
   strcpy(color[1],"Yellow");
   strcpy(color[2],"Orange");
   strcpy(color[3],"Green");
   strcpy(color[4],"Blue");
   strcpy(color[5],"Pink");
   strcpy(color[6],"Brown");
   strcpy(color[7],"White");
   color_index = 0;
   
   tft.fillRect(0, 0, 128, 160, Black); 
   tft.setCursor(50,20);
   tft.setTextColor(White);
   tft.setTextSize(2);
   tft.setRotation(3);
   tft.print("PONG!");
   
   tft.setCursor(50, 40);
   tft.setTextSize(1);
   tft.setTextColor(Black, White);
   tft.print("Single Player");
   
   tft.setCursor(50, 50);
   tft.setTextSize(1);
   tft.setTextColor(White, Black);
   tft.print("Multiplayer");
   
   tft.setCursor(50, 60);
   tft.setTextSize(1);
   tft.print("Ball Colour");
   
   tft.setCursor(50, 70);
   tft.setTextSize(1);
   tft.print("Paddle Colour");
   
   select = analogRead(SEL);
   while((state == menuAKAServer) && (select != 0)){
      int horizontal;
      horizontal = analogRead(HORIZ);
      delta_horiz = horizontal - init_horiz;

   
      if ((delta_horiz/400 > 0) && (index != 3)) {
         old_selection = selection;
         selection = selection + 10;
         old_index = index;
         index = index + 1;
         tft.setCursor(50, selection);
         tft.setTextColor(Black, White);
         tft.print(options[index]);
         tft.setCursor(50, old_selection);
         tft.setTextColor(White, Black);
         tft.print(options[old_index]);
      }

   
      if ((delta_horiz/400 < 0) && (index != 0)) {
         old_selection = selection;
         selection = selection - 10;
         old_index = index;
         index = index - 1;
         tft.setCursor(50, selection);
         tft.setTextColor(Black, White);
         tft.print(options[index]);
         tft.setCursor(50, old_selection);
         tft.setTextColor(White, Black);
         tft.print(options[old_index]);
      }

      //enters a single player game
      select = digitalRead(SEL);
      if ((select == 0) && (index == 0)) {
         tft.setRotation(0);
         tft.setCursor(0,0);
         tft.fillRect(0, 0, 128, 160, 0x0000); //fill the screen black
         tft.fillRect(P2PaddleX,P2PaddleY,20,5,paddle_colour);
         state = singleplayer;
      }
   
      //if correct key is received, the Arduino enters server mode and multiplayer game begins.
      char CR = Serial3.read();
      if (CR == 'B'){ //key
         Serial.println("You are the SERVER");
         tft.setRotation(0);
         tft.setCursor(0,0);
         tft.fillRect(0, 0, 128, 160, 0x0000); //fill the screen black
         Serial3.write('C');
         tft.fillRect(P2PaddleX,P2PaddleY,20,5,paddle_colour);
         state = ingame_player2;
      }

      //if select button is pressed, Arduuino enters the client mode and initiates multiplayer game
      select = digitalRead(SEL);
      if((select == 0) && (index == 1)){
         Serial.println("You are the CLIENT");
         tft.setRotation(0);
         tft.setCursor(0,0);
         tft.fillRect(0, 0, 128, 160, 0x0000); //fill the screen black
         Serial3.write('B');
         state = awaiting_player2;
      }

      //menu for changing the ball's colour
      if ((select == 0) && (index == 2)) {
         int start;
         start = 20;
         tft.fillRect(0, 0, 128, 160, 0x0000);
         tft.setTextColor(0xFFFF);
         tft.setTextSize(1);
         int i;
         for(i = 0; i < 8; i++) {
            tft.setCursor(40, start);
            tft.print(color[i]);
            start = start + 10;
         }

         tft.setCursor(40, 20);
         tft.setTextSize(1);
         tft.setTextColor(0x0000, 0xFFFF);
         tft.print(color[0]);
         while(true) {
            select = 1;
            int horizontal;
            horizontal = analogRead(HORIZ);
            delta_horiz = horizontal - init_horiz;
            
            if ((delta_horiz/400 > 0) && (color_index != 7)) {
               old_color_index = color_index;
               color_index = color_index + 1;
               tft.setCursor(40, 20 + color_index*10);
               tft.setTextColor(0x0000, 0xFFFF);
               tft.print(color[color_index]);
               tft.setCursor(40, 20 + old_color_index*10);
               tft.setTextColor(0xFFFF, 0x0000);
               tft.print(color[old_color_index]);
            }
      
            if ((delta_horiz/400 < 0) && (color_index != 0)) {
               old_color_index = color_index;
               color_index = color_index - 1;
               tft.setCursor(40, 20 + color_index*10);
               tft.setTextColor(0x0000, 0xFFFF);
               tft.print(color[color_index]);
               tft.setCursor(40, 20 + old_color_index*10);
               tft.setTextColor(0xFFFF, 0x0000);
               tft.print(color[old_color_index]);
            }


            select = digitalRead(SEL);
            if ((select == 0) && (color_index == 0)) {
               ball_colour = Red;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 1)) {
               ball_colour = Yellow;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 2)) {
               ball_colour = Orange;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 3)) {
               ball_colour = Green;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 4)) {
               ball_colour = Blue;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 5)) {
               ball_colour = Pink;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 6)) {
               ball_colour = Brown;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 7)) {
               ball_colour = White;
               drawMenu();
               break;
            }
         }
      }

      //enter the colour selection screen for the paddle
      if ((select == 0) && (index == 3)) {
         int start;
         start = 20;
         tft.fillRect(0, 0, 128, 160, 0x0000);
         tft.setTextColor(0xFFFF);
         tft.setTextSize(1);
         int i;
         for(i = 0; i < 8; i++) {
            tft.setCursor(40, start);
            tft.print(color[i]);
            start = start + 10;
         }
         tft.setCursor(40, 20);
         tft.setTextSize(1);
         tft.setTextColor(0x0000, 0xFFFF);
         tft.print(color[0]);
         while(true) {
            select = 1;
            int horizontal;
            horizontal = analogRead(HORIZ);
            delta_horiz = horizontal - init_horiz;
            
            if ((delta_horiz/400 > 0) && (color_index != 7)) {
               old_color_index = color_index;
               color_index = color_index + 1;
               tft.setCursor(40, 20 + color_index*10);
               tft.setTextColor(0x0000, 0xFFFF);
               tft.print(color[color_index]);
               tft.setCursor(40, 20 + old_color_index*10);
               tft.setTextColor(0xFFFF, 0x0000);
               tft.print(color[old_color_index]);
            }
      
            if ((delta_horiz/400 < 0) && (color_index != 0)) {
               old_color_index = color_index;
               color_index = color_index - 1;
               tft.setCursor(40, 20 + color_index*10);
               tft.setTextColor(0x0000, 0xFFFF);
               tft.print(color[color_index]);
               tft.setCursor(40, 20 + old_color_index*10);
               tft.setTextColor(0xFFFF, 0x0000);
               tft.print(color[old_color_index]);
            }

            select = digitalRead(SEL);
            if ((select == 0) && (color_index == 0)) {
               paddle_colour = Red;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 1)) {
               paddle_colour = Yellow;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 2)) {
               paddle_colour = Orange;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 3)) {
               paddle_colour = Green;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 4)) {
               paddle_colour = Blue;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 5)) {
               paddle_colour = Pink;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 6)) {
               paddle_colour = Brown;
               drawMenu();
               break;
            }
            if ((select == 0) && (color_index == 7)) {
               paddle_colour = White;
               drawMenu();
               break;
            }
         }
      }
   }
}

//multiplayer game over screen
void gameover_protocol(){
   tft.fillRect(0,0,128,160,0x0000);
   tft.setCursor(20,40);
   tft.setTextColor(0xFFFF);
   tft.setTextSize(2);
   tft.setRotation(3);
   if(player1score == 3){
      tft.print("player 1 is the winner!");
   } 
   if(player2score == 3){
      tft.print("player 2 is the winner!");
   }

   while(state  == gameover){
      select = digitalRead(SEL);
      if(select == 0){
         tft.setRotation(0);

         //reinitialize player scores
         player1score = 0;
         player2score = 0; 

         //turn off all LED lights
         for (int i = 0;i<6;i++){
            digitalWrite(SCORE_LED_[i], LOW);
            LED[i] = LOW;
         }

         //reposition paddles
         P1PaddleX=64;
         P2PaddleX=64;

         state = menuAKAServer;
      }
   }
}

//Client code which waits for server acknowledgment of connection request
void wait_for_player2_protocol(){
   if(wait_on_serial3(1,1000)==true){

      //once acknowledgement received, Client enters game 
      char Ack = Serial3.read();
      if (Ack == 'C'){
         tft.fillRect(P1PaddleX,P1PaddleY,20,5,paddle_colour);
         state = ingame_player1;
      }
      else {
         state = player1_client_start;
      }
   }
}

int main(){
   init();

   #ifdef USBCON
      USBDevice.attach();
   #endif

   //setting up the pull-up resisitor
   pinMode(SEL,INPUT);
   digitalWrite(SEL,HIGH);

   //setting up the speaker/buzzer thing
   pinMode(speakerPin, OUTPUT);

   //setting up the LEDs
   for (int i = 0; i < 6; i++){
      pinMode(SCORE_LED_[i], OUTPUT);
   }

   Serial.begin(9600);
   Serial3.begin(9600);
   tft.initR(INITR_BLACKTAB);
   tft.fillRect(0, 0, 128, 160, 0x0000); //fill the screen black

   init_vert = analogRead(VERT);
   init_horiz = analogRead(HORIZ);

   state = menuAKAServer; //starting state


   while(true){
      if(state == menuAKAServer){
         drawMenu();
      }

      if(state == awaiting_player2){
         wait_for_player2_protocol();
      }

      while(state == ingame_player1){
         //Serial.println(player1score);
         winner = ingame_player1_protocol();
      }

      if(state == gameover){
         //Serial.println(player1score);
         gameover_protocol();
      }

      if(state == gameover_AI){
         gameover_protocol_AI();
      }

      while(state == ingame_player2){

         ingame_player2_protocol();
      }

      while (state ==  singleplayer){
         in_game_vs_AI_protocol();
      }
   }
   Serial.end();
   return 0;
}
