#ifndef _TARDIS_H_
#define _TARDIS_H_

#include <Adafruit_DotStar.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GPS.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
/*
 *  DotStar LEDs: https://github.com/adafruit/Adafruit_DotStar
 *    Hardware SPI:
 *      DI: Arduino 51
 *      CI: Arduino 52
 *  OLED: https://github.com/adafruit/Adafruit_SSD1306
 *    set header #define for 128x64
 *    I2C address: 0x3D
 *  Solenoids: https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library
 *    requred modification to library to include BRAKE case, and special case speed of 255 to 100% PWM otherwise audible tone (1.6kHz?) was heard when solenoid was set to 255.
 *    I2C address: 0x60 (default)
 *  GPS: https://github.com/adafruit/Adafruit_GPS
 *    Serial1
 *  NeoPixel LEDs: https://github.com/adafruit/Adafruit_NeoPixel
 *    Silly 1-wire interface:
 *      Arduino 40
 */
// NeoPixels
#define NEOPIXEL_PIN 40
#define NEOPIXEL_NUM 1

// SSD1306
#define OLED_RESET 30

class Tardis {
  public:
      Tardis();
    void setup();
    void do_input();
    void do_update();
    void do_output();
  // These should probably be private but whatever.
    Adafruit_DotStar strip;
    Adafruit_NeoPixel pixel;
    Adafruit_SSD1306 display;
    Adafruit_GPS gps;
    Adafruit_MotorShield motor_shield;
    Adafruit_DCMotor *motors[6];
};

#endif
