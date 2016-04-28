#ifndef _SOLENOID_H_
#define _SOLENOID_H_

#include <Adafruit_MotorShield.h>

enum Solenoid_State {
  REST,
  ENERGIZED,
  COOL_DOWN,
};

enum Solenoid_Half {
  INNER,
  OUTER
};

#define ENERGIZE_TIME 250
#define COOL_DOWN_TIME 10000

class Solenoid {
  public:
    Solenoid(Adafruit_DCMotor *m, Solenoid_Half h);
    void setup();
    bool energize();
    void update(unsigned long now);
    Solenoid_State getState();
  private:
    Adafruit_DCMotor *motor;
    Solenoid_Half half;
    Solenoid_State state;
    bool energize_req;
    unsigned long start;
};

#endif
