#include "tardis.h"


Tardis::Tardis() :
  motor_shield(Adafruit_MotorShield())
{
  solenoids[0] = new Solenoid(&motor_shield, M1, OUTER);
  solenoids[1] = new Solenoid(&motor_shield, M1, INNER);
  solenoids[2] = new Solenoid(&motor_shield, M2, INNER);
  solenoids[3] = new Solenoid(&motor_shield, M2, OUTER);
  solenoids[4] = new Solenoid(&motor_shield, M3, OUTER);
  solenoids[5] = new Solenoid(&motor_shield, M3, INNER);
  solenoids[6] = new Solenoid(&motor_shield, M4, INNER);
  solenoids[7] = new Solenoid(&motor_shield, M4, OUTER);
}

void Tardis::setup()
{
  motor_shield.begin();
  Solenoid::beginAll();
}


void Tardis::do_input()
{
  Solenoid::testAll();
}

void Tardis::do_update()
{
  Solenoid::updateAll();
}



void Tardis::do_output()
{
}
