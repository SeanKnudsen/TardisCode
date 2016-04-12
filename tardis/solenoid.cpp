#include "solenoid.h"

Solenoid::Solenoid(Adafruit_DCMotor *m):
    motor(m),
    state(REST)
{
}

void Solenoid::setup() {
  motor->setSpeed(255);
}

void Solenoid::open() {
  if (state != REST) {
    return;
  }
  start = millis();
  motor->run(FORWARD);
}

void Solenoid::update(unsigned long now) {
  // Solenoid always updates by driving:
  // FORWARD  for 250 ms
  // BACKWARD for 250 ms
  // BREAK    for 10000 ms
  switch(state)
  {
    case IN:
      if ((now - start) > 250) {
        state = OUT;
        motor->run(BACKWARD);
      }
      break;
    case OUT:
      if ((now - start) > 500) {
        state = COOL_DOWN;
        motor->run(BRAKE);
      }
      break;
    case COOL_DOWN:
      if ((now - start) > 15000) {
        state = REST;
      }
      break;
  }
}

Solenoid_State Solenoid::getState() {
  return state;
}

