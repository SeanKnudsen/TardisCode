#include "solenoid.h"

int Solenoid::numSolenoids = 0;

Solenoid::Solenoid(Adafruit_DCMotor *m, Solenoid_Half h):
  motor(m),
  half(h),
  state(REST),
  energize_req(false)
{
	Solenoid::numSolenoids++;
}

Solenoid::Solenoid(Adafruit_MotorShield *ms, Motor_Num mn, Solenoid_Half h):
  motor(ms->getMotor(mn)),
  half(h),
  state(REST),
  energize_req(false)
{
	Solenoid::numSolenoids++;
}

void Solenoid::setup() {
  motor->setFullSpeed();
}

// energize() just sets a flag, must still call update()
bool Solenoid::energize() {
  if (energize_req) {
    // we're already waiting to energize
    return false;
  } else {
    energize_req = true;
    return true;
  }
}

void Solenoid::update(unsigned long now) {
  // Solenoid always updates by driving:
  // REST      until energize_req
  // ENERGIZED for ENERGIZE_TIME ms
  // COOL_DOWN for COOL_DOWN_TIME ms
  // REST...
  switch (state)
  {
    case REST:
      if ( energize_req ) {
        start = millis();
        state = ENERGIZED;
        energize_req = false;
        if ( INNER == half ) {
          motor->run(BACKWARD);
        } else if ( OUTER == half ) {
          motor->run(FORWARD);
        }
      }
      break;
    case ENERGIZED:
      if ((now - start) > ENERGIZE_TIME) {
        state = COOL_DOWN;
        motor->run(RELEASE);
      }
      break;
    case COOL_DOWN:
      if ((now - start) > (COOL_DOWN_TIME + ENERGIZE_TIME)) {
        state = REST;
      }
      break;
  }
}

Solenoid_State Solenoid::getState() {
  return state;
}

