#ifndef _SCREEN_H
#define _SCREEN_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#ifndef SSD1306_128_64
#error "go to <Adafruit_SSD1306.h> and make sure SSD1306_128_64 is defined"
#endif
#include <Fonts/FreeMono9pt7b.h>
#include "art.h"
#define TIME_OFFSET -7
// Should this be defined here?
#define OLED_RESET 41


class Screen {

  public:
    Screen();
    void setup();

    void showMainMenu(uint8_t selection);
    void showLEDMenu(uint8_t selection);
    void showStdLatLon(double latdeg, double latmin, double londeg, double lonmin, double altitude, double distance);
    void noGPSFix();
    void showTime(uint8_t minute, uint8_t hour, uint8_t day, uint8_t month, uint8_t year);
    
    void showConnieLovesErik();
    void showNoGPS();
    void showUTCOffset(uint8_t selection);
    void updateScreen();
    Adafruit_SSD1306 display;

    int utcOffset;
  private:
    void initLCD(int size);
    void displayCoordinate(int degree, float minute);
  
};

#endif
