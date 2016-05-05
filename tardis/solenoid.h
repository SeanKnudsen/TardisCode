#ifndef _SOLENOID_H_
#define _SOLENOID_H_

#include <Adafruit_MotorShield.h>

#define SOLENOIDS_MAX 8

enum Solenoid_State {
  REST,
  ENERGIZED,
  COOL_DOWN,
};

enum Solenoid_Half {
  INNER,
  OUTER
};

enum Motor_Num {
	M1 = 1,
	M2,
	M3,
	M4
};

#define ENERGIZE_TIME 250
#define COOL_DOWN_TIME 10000

class Solenoid {
  public:
    Solenoid(Adafruit_DCMotor *m, Solenoid_Half h);
    Solenoid(Adafruit_MotorShield *ms, Motor_Num mn, Solenoid_Half h);
    void setup();
    bool energize();
    void update(unsigned long now);
    Solenoid_State getState();
    static void testAll();
    static void updateAll();
  private:
    Adafruit_DCMotor *motor;
    Solenoid_Half half;
    Solenoid_State state;
    bool energize_req;
    unsigned long start;
    static int numSolenoids;
    static Solenoid* solenoids[SOLENOIDS_MAX];
};

#endif
