#include <arduino.h>
#include "button.h"

  volatile bool buttonHeld;
  volatile bool buttonState;
  volatile int buttonHoldTime = 0;
//  Constructor
  Button::Button(){}

//46, 47

// Call this from setup()
void Button::setup()
{
  //Serial.print("in button setup\n");
  buttonState = false;
  buttonHeld = false;
  buttonHoldTime = 0;
  clearButtonPress();
  pinMode(46, OUTPUT);
  digitalWrite(46, LOW);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);       // turn on pullup resistors


  
// this must appear outside this function:
//   attachInterrupt(digitalPinToInterrupt(buttonPin), pin_ISR, CHANGE);
}


void Button::pinPoll() {
  buttonState = !digitalRead(buttonPin);

  if(buttonState) {
    //Serial.print("Push!");
    if(!buttonHeld)
    {
      // If button pressed for the first time, record time
      buttonHoldTime = millis();
    }
    buttonHeld = true;
  }
  else
  {
    if(buttonHeld) {
      buttonHoldTime = millis() - buttonHoldTime;
      //Serial.print("Button Press: "); Serial.print(buttonHoldTime); Serial.print("\n");
      //Serial.println("--------------------------");

      if(buttonHoldTime > LONG_PRESS_DURATION)
      {
        LongPress = true;
      }
      else if (buttonHoldTime > SHORT_PRESS_DURATION)
      {
        ShortPress = true;
      }
    }
    buttonHeld = false;
  }
}



void Button::clearButtonPress()
{
  ShortPress = false;
  LongPress = false;
}


/*
 * 
 * 
 * 
 * 
 * 
 * 
 *  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // manage the button more frequently - each 25ms we'll check.
  if (millis() - timer % 25) 
  {
    manageButton();
  }


  if (millis() - timer > 100) 
  {  
    manageTopLED();
    manageDotStars();
    manageSpeaker();
    manageSolenoids();

    manageOLED();

    clearButtonPress();

    timer = millis(); // reset the timer
    ticks = (ticks + 1)% 600;
  }

 
 */

