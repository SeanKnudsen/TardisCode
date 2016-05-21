#include "tardis.h"
Tardis tardis = Tardis();

void setup() {
  tardis.setup();
}

void loop() {
  tardis.do_input();
  tardis.do_update();
  tardis.do_output();
}

// Interrupt called 1/millisecond, looks for GPS data and stores it.
SIGNAL(TIMER0_COMPA_vect) {
  tardis.do_timerUpdate();
}
