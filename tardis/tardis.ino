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
