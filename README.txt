Avery Tan Section A1
Alden Tan Section A2
Final Project


Program that allows us to connect 2 Arduinos together to play
a crude game of pong


Required Components:
* Arduino Megaboard
* Controller Joystick
* LCD Screen
* Jumper Cables
* 6 LEDs
* 6 560 Ohms resistors
* Piezo Buzzer


Wiring Instructions(same for both Arduinos):

*Position is based off of the standard provided on eclass
*Arduino is placed landscape-style with the breadboard facing away from you
*The LCD screen is connected with the micro SD slot facing away from you and the 
 pins inserted into the breadboard at E1-E10. 
*The joystick is inserted with the vertical arrow pointing towards you and the 
 pins inserted into F43-F47.
*The piezo buzzer is inserted with its pins in J59 and J61.

*LED0 is inserted with its longer end at Breadboard H33 and its shorter end at Breadboard H34
*A resistor is placed one end at Breadboard J34 and the other at the nearest Breadboard negative bus
*LED1 is inserted with its longer end at Breadboard G30 and its shorter end at Breadboard G31
*A resistor is placed one end at Breadboard J31 and the other at the nearest Breadboard negative bus
*LED2 is inserted with its longer end at Breadboard G26 and its shorter end at Breadboard G27
*A resistor is placed one end at Breadboard J27 and the other at the nearest Breadboard negative bus
*LED3 is inserted with its longer end at Breadboard G23 and its shorter end at Breadboard G24
*A resistor is placed one end at Breadboard J24 and the other at the nearest Breadboard negative bus
*LED4 is inserted with its longer end at Breadboard G19 and its shorter end at Breadboard G20
*A resistor is placed one end at Breadboard J20 and the other at the nearest Breadboard negative bus
*LED5 is inserted with its longer end at Breadboard G16 and its shoter end at Breadkboard G17
*A resistor is placed one end at Breadboard J17 and the other at the nearest Breadboard negative bus


Arduino Pin 2 <--> Breadboard F33
Arduino Pin 3 <--> Breadboard F30
Arduino Pin 4 <--> Breadboard F26
Arduino Pin 10 <--> Breadboard F23
Arduino Pin 11 <--> Breadboard F19
Arduino Pin 12 <--> Breadboard F16
Arduino Pin 13 <--> Breadboard F59
Arduino Pin 9 <--> Breadboard G43
Arduino Pin 8 <--> Breadboard A3
Arduino Pin 7 <--> Breadboard A4
Arduino Pin 6 <--> Breadboard A6
Arduino Pin 5 <--> Breadboard A5
Arduino Analog Pin A0 <--> Breadboard G46
Arduino Analog Pin A1 <--> Breadboard G45
Arduino Digital Pin 52 <--> Breadboard A8
Arduino Digital Pin 50 <--> Breadboard A9
Arduino Digital Pin 51 <--> Breadboard A7
Arduino 5V power supply <-->Breadboard positive bus
Arduino GND <--> Breadboard negative bus
Breadboard G43 <--> Breadboard  negative bus
Breadboard G47 <--> Breadboard positive bus
Breadboard A10 <--> Breadboard positive bus
Breadboard A1 <--> Breadboard negative bus
Breadboard A2 <--> Breadboard positive bus
Breadboard F61 <--> Breadboard negative bus
A jumper cable connecting both Breadboard negative buses

For cross-communication between the two arduinos, three more jumper cables are necessary:
A jumper cable connecting the Breadboard negative buses of the two Arduinos
A jumper cable from Arduino Pin 14(TX3) of one Arduino to the Arduino Pin 15(RX3) of the other Arduino and vice versa



Instructions for using the Restaurant Finder Program:
*Scroll down using the joystick in order to highlight a selection

*Scroll down to Ball Colour and press down on the joystick in order to open the color selections. Choose the 
 desired colour and press down on the joystick again to lock in selection and return to the main menu. Default colour is white

*Scroll down to Paddle Colour and press down on joystick in order to open the colour selections. CHoose the 
 desired colour and press down on the joystick again to lock in selection and return to the main menu. Default colour is white.

*Scroll to 'Multiplaye' in order to begin a game with the other Arduino. Use the joystick to move paddle. First to score 3 points wins. 
 Once game is over, the screen will display the winner. Press down on the joystick in order to return to main menu.

*Scroll to 'Single Player' in order to start a game in practise mode with the AI. 

