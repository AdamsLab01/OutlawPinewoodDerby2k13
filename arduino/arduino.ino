/**********************************************************************************************************************

This sketch was written to control a motorized Pinewood Derby car via IR remote. 

For more information see - http://awaitinginspiration.com/2013/04/motorized-pinewood-derby-car-2013/ or ping adam@adambyers.com

Coin Operated XBOX
Copyright (C) 2013 Adam Byers

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**********************************************************************************************************************/

#include <IRremote.h> // Included the IR library

int directionState = 0;
int readyState = 0;
int fwdSpeed = 0;
int revSpeed = 0;

bool runAllowed = false;
bool goBabyGo = false;

int pwmA = 6; // Motor controler PWM pin for motor A 
int pwmB = 5; // Motor controler PWM pin for motor B
int ain1 = 9; // Motor controler direction control pin 2 for motor A
int ain2 = 8; // Motor controler direction control pin 1 for motor A
int standby = 10; // Motor controler standby pin
int bin1 = 2; // Motor controler direction control pin 1 for motor B
int bin2 = 12; // Motor controler direction control pin 2 for motor B
int recv = 4; // IR detector
int runAllowedLED = A0; // Run allowed indicator LED
int fwdLED = A1; // Foward indicator LED
int revLED = A2; // Reverse indicator LED 
IRrecv irrecv(recv);
decode_results results;

void setup()
{
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  pinMode(standby, OUTPUT);
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);
  pinMode(runAllowedLED, OUTPUT);
  pinMode(fwdLED, OUTPUT);
  pinMode(revLED, OUTPUT);
  irrecv.enableIRIn(); 
}

void loop()
{
  
/*
Begin IR detection sequance
*/
  
  if (irrecv.decode(&results)) 
  {
    if (results.value == 0x77E1401D) // Menu button (Apple remote) | remote control enabled
    { 
      readyState++;
    }
		
    if (results.value == 0x77E1201D && runAllowed == true) // Play/Pause button (Apple remote) | go foward full speed
    { 
      goBabyGo = true;
    }
   
    if (results.value == 0x77E1E01D && goBabyGo == false) // ffwd button (Apple remote) | Forward direction enabled
    { 
      directionState = 1;
      digitalWrite(fwdLED, HIGH);
      digitalWrite(revLED, LOW);
    }
		
    if (results.value == 0x77E1101D && goBabyGo == false) // rwd button (Apple remote) | Reverse direction enabled
    { 
      directionState = 2;
      digitalWrite(fwdLED, LOW);
      digitalWrite(revLED, HIGH);
    }
     
    if (results.value == 0x77E1D01D && directionState == 1 && runAllowed == true && goBabyGo == false) // Volume up button (Apple remote) | Increase forward speed
    { 
      stop();
      revSpeed = 0;
      fwdSpeed++; 
    }
    
    if (results.value == 0x77E1B01D && directionState == 2 && runAllowed == true && goBabyGo == false) // Volume down button (Apple remote) | Increase reverse speed
    {
      stop();
      fwdSpeed = 0;
      revSpeed++;
    }
  
  irrecv.resume();
  }
  
/*
End IR detection sequance
*/
  
  if (readyState == 1) // If the run allowed button has been pushed once then we can run the car
  {
    runAllowed = true;
    digitalWrite(runAllowedLED, HIGH);
  }
  
  if (readyState == 2) // If the run allowed button has been pushed twice then set the ready state to 0
  {
    readyState = 0;
    runAllowed = false;
    fwdSpeed = 0;
    revSpeed = 0;
    digitalWrite(runAllowedLED, LOW);
    digitalWrite(fwdLED, LOW);
    digitalWrite(revLED, LOW);
    goBabyGo = false;
  }

  if (readyState == 0) // If readyState is 0 then we don't run
  {
    stop();
    runAllowed = false;
    directionState = 0;
    digitalWrite(runAllowedLED, LOW);
    digitalWrite(fwdLED, LOW);
    digitalWrite(revLED, LOW);
    goBabyGo = false;
  }
  
  if (goBabyGo && runAllowed) // If the GoBabyGo button has been pushed and the runAllowed is TRUE then we run both motors foward at full speed
  {
    fwdSpeed = 0;
    revSpeed = 0;
    move(1, 255, 0);
    move(2, 255, 0);
  }
  
// set speed based on how many times the foward or revers speed buttons were pressed

  if (fwdSpeed == 1) 
  {
    move(1, 64, 0);
    move(2, 64, 0);
  }
  
  if (fwdSpeed == 2) 
  {
    move(1, 128, 0);
    move(2, 128, 0);
  }
  
  if (fwdSpeed == 3) 
  {
    move(1, 255, 0);
    move(2, 255, 0);
  }
  
  if (fwdSpeed == 4) 
  {
    stop();
    fwdSpeed = 0;
  }
  
  if (revSpeed == 1) 
  {
    move(1, 64, 1);
    move(2, 64, 1);
  }
  
  if (revSpeed == 2) 
  {
    move(1, 128, 1);
    move(2, 128, 1);
  }
  
  if (revSpeed == 3)
  {
    move(1, 255, 1);
    move(2, 255, 1);
  }
  
  if (revSpeed == 4) 
  {
    stop();
    revSpeed = 0;
  } 
}

void move(int motor, int speed, int direction){
//Move specific motor at speed and direction
//motor: 0 for B 1 for A
//speed: 0 is off, and 255 is full speed
//direction: 0 clockwise, 1 counter-clockwise

  digitalWrite(standby, HIGH); //disable standby

  boolean inPin1 = LOW;
  boolean inPin2 = HIGH;

  if(direction == 1){
    inPin1 = HIGH;
    inPin2 = LOW;
  }

  if(motor == 1)
  {
    digitalWrite(ain1, inPin1);
    digitalWrite(ain2, inPin2);
    analogWrite(pwmA, speed);
  }
  
  if(motor == 2)
  {
    digitalWrite(bin1, inPin1);
    digitalWrite(bin2, inPin2);
    analogWrite(pwmB, speed);
  }
}

void stop(){
//enable standby  
  digitalWrite(standby, LOW);
}
