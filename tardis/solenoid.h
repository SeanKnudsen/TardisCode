#ifndef _SOLENOID_H_
#define _SOLENOID_H_

#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

enum Solenoid_State {
  REST,
  IN,
  OUT,
  COOL_DOWN,
};

class Solenoid {
  public:
      Solenoid(Adafruit_DCMotor *m);
      void setup();
      void open();
      void update(unsigned long now);
    Solenoid_State getState();
  private:
    Adafruit_DCMotor *motor;
    unsigned long start;
    Solenoid_State state;
};

#endif
