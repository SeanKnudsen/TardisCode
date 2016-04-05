/*  bits and pieces of library demos jammed together in this file and other files in project.  
 *  
 *  no attempt at scheduling has been made.
 *  
 *  DotStar LEDs: https://github.com/adafruit/Adafruit_DotStar, strandtest
 *    Hardware SPI:
 *      DI: Arduino 51
 *      CI: Arduino 52
 *  OLED: https://github.com/adafruit/Adafruit_SSD1306, ssd1306_128x64_i2c
 *    set header #define for 128x64
 *    I2C address: 0x3D
 *  Solenoids: https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library, DCMotorTest
 *    requred modification to library to include BRAKE case, and special case speed of 255 to 100% PWM otherwise audible tone (1.6kHz?) was heard when solenoid was set to 255.
 *    I2C address: 0x60 (default)
 *  GPS: https://github.com/adafruit/Adafruit_GPS, parsing
 *    Serial1
 *  NeoPixel LEDs: https://github.com/adafruit/Adafruit_NeoPixel, strandtest
 *    Silly 1-wire interface:
 *      Arduino 40
 */

#include <SPI.h>
#include <Wire.h>

#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

#include <Adafruit_DotStar.h>
#define NUMPIXELS 2 // Number of LEDs in strip

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define NEOPIXEL_PIN 40
#define NEOPIXEL_NUM 4

#include <Adafruit_GPS.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 30

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


/* ------------- OLDE Globals ----------------- */
Adafruit_SSD1306 display(OLED_RESET);
/* ------------- END OLED Globals ----------------- */

/* ------------- Solenoid Globals ----------------- */
// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);
// You can also make another motor on port M2
Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(3);
/* ------------- END Solenoid Globals ----------------- */

/* ------------- Neopixel Globals ----------------- */
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel neopixel_strip = Adafruit_NeoPixel(NEOPIXEL_NUM, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.
/* ------------- END Neopixel Globals  ----------------- */

/* ------------- DotStar Globals ----------------- */
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BRG);

int      head  = 0, tail = -1; // Index of first 'on' and 'off' pixels
uint32_t color = 0xFF0000;      // 'On' color (starts red)
/* ------------- END DotStar Globals ----------------- */

/* ------------- GPS Globals ----------------- */
HardwareSerial mySerial = Serial1;
Adafruit_GPS GPS(&Serial1);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  false

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy
/* ------------- END GPS Globals ----------------- */


void setup()   {                
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  Serial.println("Big ugly pile of demo code!");

  setup_gps();

  setup_dotstar();
  
  setup_neopixels();

  setup_oled();

  setup_solenoids();
}


uint32_t timer = millis();

void loop() {
  uint8_t i;

  // hopefully force floats to be included (looks like they are somewhere already... gps?)
  static float testFloat = 1.0;
  testFloat = testFloat * 1.1;
  Serial.println(testFloat);
  

  
  Serial.print("FORWARD\n");
  dotstar_step();
  display.invertDisplay(true);
  //myMotor->setSpeed(255);
  myMotor->run(FORWARD);
  myOtherMotor->run(FORWARD);
  delay(250);


  
  Serial.print("BACKWARD\n");
  dotstar_step();
  display.invertDisplay(false);
  //myMotor->setSpeed(255);
  myMotor->run(BACKWARD);
  myOtherMotor->run(BACKWARD);
  delay(250);

  // BRAKE for turning both off when solenoids are tied low
  Serial.print("BRAKE\n");
  myMotor->run(BRAKE);
  myOtherMotor->run(BRAKE);

  // RELEASE for turning both off when solenoids are tied high
//  Serial.print("RELEASE\n");
//  myMotor->run(RELEASE);
//  myOtherMotor->run(RELEASE);


  // let the solenoids and drivers cool down.
  // this is important.
  // seriously.
  for ( int i = 0; i<10; i++ ) {
    delay(1000);
  }

  /* neopixels... */
  rainbow(20);
  /*--------------*/


  /* gps... */
  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
  }
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) { 
    timer = millis(); // reset the timer

    gps_print();
  }
  /* END gps */
}

