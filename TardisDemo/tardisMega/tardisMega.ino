/* Test Code for Tardis */

/**************************   Includes   **************************/

/* #includes for OLED */
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_DotStar.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

#include "screen.h"
#include "location.h"
/**************************   Defines   **************************/

/* timer #defines */
#define ONE_HUNDRED_MS 100

#define OLED_CYCLE 150 // 15000ms / 100ms/cycle = 150
#define USE_SLOT_ZERO 0
#define USE_SLOT_ONE  1

#define SHORT_PRESS_DURATION 50  //milliseconds
#define LONG_PRESS_DURATION 550  //milliseconds
#define MENU_PRESS_DURATION 1000 // milliseconds

Location location = Location();
Screen screen = Screen();

enum State
{
  shelf_mode,
  shelf_mode_menu,
  mission_ready,
  mission_ready_menu,
  seek_mode,
  found_mode
};



// #define OLED_RESET 7
// Adafruit_SSD1306 display(OLED_RESET);

/* Defines for DotStar */
//#define NUMPIXELS 100 // Number of LEDs in strip
// Here's how to control the LEDs from any two pins:
//#define DATAPIN    4
//#define CLOCKPIN   5
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BRG);
/************************** Select SPI for SSD1306 **************************/
//Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


/*!!!!!!!!!!!!!!!!!!!!!!! TODO Sean Changed on purpose for size!!!!! Must be changed back!!!!!!!!!!!!!*/
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


HardwareSerial mySerial = Serial1;

/* Function Prototypes */
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

/**************************   Global Variables   **************************/
// this keeps track of whether we're using the interrupt
boolean usingInterrupt = true;
int utcOffset = 0;
uint32_t timer = millis();
uint32_t ticks = 0;

// Runs 10 LEDs at a time along strip, cycling through red, green and blue.
// This requires about 200 mA for all the 'on' pixels + 1 mA per 'off' pixel.

int      head  = 0, tail = -10; // Index of first 'on' and 'off' pixels
uint32_t color = 0xFF0000;      // 'On' color (starts red)

// Initialize state machine
State tardis_state = shelf_mode;


/**************************   Setup   **************************/
void setup()   
{          
  location.setup();
  screen.setup();
  
  utcOffset = TIME_OFFSET;

  pinMode(47, INPUT);           // set pin to input
  digitalWrite(47, HIGH);       // turn on pullup resistors
  
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
  #endif

  //strip.begin(); // Initialize pins for output
  //strip.show();  // Turn all LEDs off ASAP
  
  location.setTarget(TARGET_LAT, TARGET_LON);
  Serial.print("TARDIS READY FOR TAKEOFF!");
}


/**************************   Loop   **************************/
void loop() 
{
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // manage the button more frequently - each 25ms we'll check.
  if (millis() - timer % 25) 
  {
    manageButton();
  }


  if (millis() - timer > 100) 
  {  
    manageTopLED();
    manageDotStars();
    manageSpeaker();
    manageSolenoids();

    manageOLED();

    clearButtonPress();

    timer = millis(); // reset the timer
    ticks = (ticks + 1)% 600;
  }
}

/******************* General Purpose Functions ***************************/

void manageTopLED()
{
  // add state machine here
  // shelf_mode,
  // shelf_mode_menu,
  // mission_ready,
  // mission_ready_menu,
  // seek_mode,
  // found_mode
}

void manageDotStars()
{
  /*
  if (millis() - timer > 1000)
  {
    strip.setPixelColor(head, color); // 'On' pixel at head
    strip.setPixelColor(tail, 0);     // 'Off' pixel at tail
    strip.show();                     // Refresh strip
    delay(20);                        // Pause 20 milliseconds (~50 FPS)
  
    if(++head >= NUMPIXELS) {         // Increment head index.  Off end of strip?
      head = 0;                       //  Yes, reset head index to start
      if((color >>= 8) == 0)          //  Next color (R->G->B) ... past blue now?
        color = 0xFF0000;             //   Yes, reset to red
    }
    if(++tail >= NUMPIXELS) tail = 0; // Increment, reset tail index
  }*/
}

void manageSpeaker()
{
  
}

void manageSolenoids()
{
  
}


uint32_t buttonHoldTime = 0;
bool buttonHeld = false;
bool ShortPress = false;
bool LongPress = false;
bool MenuPress = false;

void clearButtonPress()
{
  ShortPress = false;
  LongPress = false;
  MenuPress = false;
}

void manageButton()
{
  int val = 0;

  // Don't register additional button presses until the initial one has been handled!
  if (!ShortPress && !LongPress && !MenuPress)
  {
    val = !digitalRead(47);
    
    if(val)
    {
      if(!buttonHeld)
      {
        buttonHoldTime = millis();
      }
      buttonHeld = true;
    }
    else
    {
      if(buttonHeld)
      {
        buttonHoldTime = millis() - buttonHoldTime;
        Serial.print("Hold Time:");
        Serial.println(buttonHoldTime);
        if(buttonHoldTime > MENU_PRESS_DURATION)
        {
          MenuPress = true;
          Serial.println("Menu Press!!");
        }
        else if(buttonHoldTime > LONG_PRESS_DURATION)
        {
          LongPress = true;
        }
        else if (buttonHoldTime > SHORT_PRESS_DURATION)
        {
          ShortPress = true;
        }
      }
      buttonHeld = false;
    }
  }
}
int oledState = 0;
void manageOLED()
{
  int localTicks = ticks % 100;
  static int i = 1;

  // lets service this loop 2x/second (but let's offset the slot!

  if(ticks%6 == USE_SLOT_ONE)
  {
    switch(tardis_state) 
    {

      case shelf_mode :
  
        if(localTicks < 30)
        {
          // displayTime();
          screen.showTime(location.Minute, location.Hour, location.Day, location.Month, location.Year);
        }
        else if (localTicks < 100)
        {
          //displayGPS();
          Serial.print("i:");
          Serial.print(i); Serial.print("\r\n");
          if(i < 12)
          {
            screen.showMainMenu(i++%4);
          }
          else if(i < 24)
          {
            screen.showLEDMenu(i++%8);
          }
          else if (i < 30){
            i++;
            screen.showConnieLovesErik();
          }else if (i < 50){
            screen.noGPSFix();
            i++;
          }else if (i < 100)
          {
            Serial.println("printing utc!");
            Serial.println(screen.utcOffset);
            screen.utcOffset = -7;
            screen.showUTCOffset(i++%3);
          }else{
            i=1;
          }
        }
        //else if (localTicks < 150)
        else
        {
          //displayHeading();
        }
        
        if(MenuPress)
        {
          tardis_state = shelf_mode_menu;
          MenuPress = false;
        }
  
        break;
  
      case shelf_mode_menu :     
        //screen.showMainMenu(1);
        if(LongPress)
        {
          //selection
        }else if(ShortPress)
        {
          //increment
        }
        
        break;
    }
  }  
}

SIGNAL(TIMER0_COMPA_vect) {

location.collectGPS();
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

/**************************   OLED Display Functions   **************************/

void displayGPS()
{
  //double distanceFromSeans = 0.0;
  
  //int feet = 0;

  if(location.hasFix())
  {
    screen.showStdLatLon(location.LatDeg, location.LatMin, location.LonDeg, location.LonMin, location.Altitude, location.Distance);
  }
  else
  {
    screen.noGPSFix();
  }
}
