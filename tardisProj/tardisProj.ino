
#include <Adafruit_DotStar.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GPS.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Adafruit_MotorShield.h>
#include <Wire.h>
//#include "FreeMono9pt7b.h"
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
  tardis.doCollectGPS();
}
