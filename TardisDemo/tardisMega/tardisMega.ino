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
#include "location.h"
/**************************   Defines   **************************/


/* timer #defines */
#define ONE_HUNDRED_MS 100

#define OLED_CYCLE 150 // 15000ms / 100ms/cycle = 150
#define USE_SLOT_ZERO 0
#define USE_SLOT_ONE  1

#define SHORT_PRESS_DURATION 50  //milliseconds
#define LONG_PRESS_DURATION 550  //milliseconds
#define MENU_PRESS_DURATION 2000 // milliseconds

Location location = Location();

enum State
{
  shelf_mode,
  shelf_mode_menu,
  mission_ready,
  mission_ready_menu,
  seek_mode,
  found_mode
};

#define TIME_OFFSET -7

#define OLED_RESET 7
Adafruit_SSD1306 display(OLED_RESET);

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
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)      
  location.setup();
  initOLED();
  //initGPS();
  utcOffset = TIME_OFFSET;

  pinMode(6, INPUT);           // set pin to input
  digitalWrite(6, HIGH);       // turn on pullup resistors
  
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
  #endif

//  strip.begin(); // Initialize pins for output
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

// This loop is entered once every .1 seconds
void manageGPS()
{
  //location.collectGPS();
/*
  // lets service this loop 2x per second:
  if(ticks%5 == USE_SLOT_ZERO)
  {
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if ((c) && (GPSECHO))
      Serial.write(c); 
      
    // if a sentence is received, we can check the checksum, parse it...
    if (GPS.newNMEAreceived()) 
    {
      if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
        return;  // we can fail to parse a sentence in which case we should just wait for another
    }
  }
*/
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
  //MenuPress = false;
}

void manageButton()
{
  int val = 0;

  // Don't register additional button presses until the initial one has been handled!
  if (!ShortPress && !LongPress && !MenuPress)
  {
    val = !digitalRead(6);
    
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

        if(buttonHoldTime > MENU_PRESS_DURATION)
        {
          MenuPress = true;
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
  int localTicks = ticks % 150;

  // lets service this loop 2x/second (but let's offset the slot!
  // shelf_mode,
  // shelf_mode_menu,
  // mission_ready,
  // mission_ready_menu,
  // seek_mode,
  // found_mode

  if(ticks%5 == USE_SLOT_ONE)
  {
    switch(tardis_state) 
    {

      case shelf_mode :
  
        if(localTicks < 50)
        {
          displayTime();
        }
        else if (localTicks < 100)
        {
          displayGPS();
        }
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
        initLCD(1);
        display.println("MENU");
        display.print("-----");
        display.display();
        break;
    }
  }  
}
/*
void initGPS()
{
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  Serial.begin(115200);

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);

  delay(1000);
  
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);
}
*/
// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
/*
SIGNAL(TIMER0_COMPA_vect) {

char c = GPS.read();
// if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif
}
*/

SIGNAL(TIMER0_COMPA_vect) {

location.collectGPS();
// if you want to debug, this is a good time to do it!

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


void initOLED()
{
   // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
      display.begin(SSD1306_SWITCHCAPVCC);

}

void ledHelloWorld()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Hello World!");
  display.display();
}







/**************************   OLED Display Functions   **************************/
void displayGPS()
{
  double distanceFromSeans = 0.0;
  
  int feet = 0;

  if(location.hasFix())
  {
    initLCD(1);
    
    displayLat();
    displayLon();  
  
    display.print("Altitude : ");
    display.print(location.Altitude);
    display.print(" ft\r\n");

    distanceFromSeans = location.Distance;    

    display.print("Distance : ");
    if(distanceFromSeans >= 1.00)
    {
      display.print(distanceFromSeans);
      display.print(" mi\r\n");
    }
    else
    {
      feet = (int)(distanceFromSeans * 5280);
      display.print(feet); // convert to feet
      display.print(" ft\r\n");
    }
    display.display();
  }
  else
  {
    display.print("No GPS Fix\r\nTODO: \r\n  <Insert NoGPS Icon Here>");
  }
}
/*
void displayHeading()
{
  initLCD(1);

  display.print("Satellites : ");
  display.print(GPS.satellites);
  display.print("\r\n");

  display.print("Fix Quality: ");
  display.print(GPS.fixquality);
  display.print("\r\n");

  display.print("Heading    : ");
  display.print(GPS.angle);
  display.print("\r\n");

  display.print("Speed      : ");
  display.print(GPS.speed);
  display.print(" kn");
  display.print("\r\n");

  display.display();
}
*/
void displayTime()
{
  static bool toggleColon = false;
  int hour = 0;
  int dispHour = 0;
  initLCD(2);

  display.print(" ");
  if(location.Month < 10)
  {
    display.print("0");
  }
  display.print(location.Month); display.print('/'); 

  if(location.Day < 10)
  {
    display.print("0");
  }
  display.print(location.Day); display.print('/');

  display.print(location.Year);
  display.print("\r\n");

  display.print("  ");
  hour = (24 + location.Hour + utcOffset)%24;
  dispHour = hour %12;
  if(dispHour == 0)
  {
    dispHour = 12;
  }

  display.print(dispHour);  
  
  if(toggleColon)
  {
    display.print(":");
  }
  else
  {
    display.print(" ");
  }

  if(location.Minute < 10)
  {
    display.print("0");
  }
  display.print(location.Minute); 
  if(hour < 12)
  {
    display.print("AM");
  }
  else
  {
    display.print("PM");
  }
  display.display();

  toggleColon = !toggleColon;
}

void initLCD(int size)
{
  display.clearDisplay();
  display.setTextSize(size);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
}

void displayCoordinate(int degree, float minute)
{
  if(degree < 100)
  {
    if(degree<10)
    {
      display.print(" ");
    }
    display.print(" ");
  }
  
  display.print(degree);
  display.print(", ");
  display.print(minute);
  display.print("\r\n");
}

void displayLat()
{
  display.print("Latitude : ");
  displayCoordinate(location.LatDeg, location.LatMin);
}

void displayLon()
{
  display.print("Longitude: ");
  displayCoordinate(location.LonDeg, location.LonMin);
}


