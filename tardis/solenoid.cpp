#include "solenoid.h"

int Solenoid::numSolenoids = 0;
Solenoid* Solenoid::solenoids[SOLENOIDS_MAX] = {NULL};

Solenoid::Solenoid(Adafruit_DCMotor *m, Solenoid_Half h):
  motor(m),
  half(h),
  state(REST),
  energize_req(false)
{
	Solenoid::solenoids[numSolenoids] = this;
	Solenoid::numSolenoids++;
}

// TODO: instead of sequentially populating solenoids[], map each to unique index ((mn-1)*2 + h)
Solenoid::Solenoid(Adafruit_MotorShield *ms, Motor_Num mn, Solenoid_Half h):
  motor(ms->getMotor(mn)),
  half(h),
  state(REST),
  energize_req(false)
{
	Solenoid::solenoids[numSolenoids] = this;
	Solenoid::numSolenoids++;
}

// steps through activating one solenoid at a time, must still update
// TODO: test
// TODO: consider what happens when there's only one solenoid
void Solenoid::testAll() {
  static uint8_t activeSolenoid = numSolenoids-1;
  static uint8_t nextSolenoid = (activeSolenoid+1)%numSolenoids;
  
  if ( solenoids[activeSolenoid]->getState() == REST && solenoids[nextSolenoid]->getState() == REST ) {
    activeSolenoid = nextSolenoid;
    solenoids[activeSolenoid]->energize();
    nextSolenoid = (activeSolenoid+1)%numSolenoids;
  }
}

// TODO: see if this can work in a timer driven ISR
void Solenoid::updateAll() {
	uint8_t i;
	unsigned long now = millis();
	for ( i = 0; i < SOLENOIDS_MAX; i++ ) {
		if ( solenoids[i] ) {
			solenoids[i]->update(now);
		} else {
			break;
		}
	}
}

void Solenoid::beginAll() {
	uint8_t i;
	for ( i = 0; i < SOLENOIDS_MAX; i++ ) {
		if ( solenoids[i] ) {
			solenoids[i]->setup();
		} else {
			break;
		}
	}
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

