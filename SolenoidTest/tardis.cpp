#include "tardis.h"


Tardis::Tardis() :
  motor_shield(Adafruit_MotorShield())
{
  solenoids[0] = new Solenoid(motor_shield.getMotor(1), OUTER);
  solenoids[1] = new Solenoid(motor_shield.getMotor(1), INNER);
  solenoids[2] = new Solenoid(motor_shield.getMotor(2), INNER);
  solenoids[3] = new Solenoid(motor_shield.getMotor(2), OUTER);
  solenoids[4] = new Solenoid(motor_shield.getMotor(3), OUTER);
  solenoids[5] = new Solenoid(motor_shield.getMotor(3), INNER);
  solenoids[6] = new Solenoid(motor_shield.getMotor(4), INNER);
  solenoids[7] = new Solenoid(motor_shield.getMotor(4), OUTER);
}

void Tardis::setup()
{
  motor_shield.begin();
  for (int i=0; i<8; i++) {
    solenoids[i]->setup();
  }
}


void Tardis::do_input()
{
  static int activeSolenoid = 7;

  if ( solenoids[activeSolenoid]->getState() == REST && solenoids[(activeSolenoid+1)%6]->getState() == REST ) {
    activeSolenoid = (activeSolenoid+1) % 8;
    solenoids[activeSolenoid]->energize();
  }
}

void Tardis::do_update()
{
  unsigned long now = millis();

  // Update Solenoids
  for (int i=0; i < 8; i++) {
    solenoids[i]->update(now);
  }
}



void Tardis::do_output()
{
}
