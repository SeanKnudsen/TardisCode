#ifndef _TARDIS_H_
#define _TARDIS_H_

#include "solenoid.h"

class Tardis {
  public:
      Tardis();
    void setup();
    void do_input();
    void do_update();
    void do_output();
  // These should probably be private but whatever.

    Adafruit_MotorShield motor_shield;
    Solenoid *solenoids[8];

};

#endif
