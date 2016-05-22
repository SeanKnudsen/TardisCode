#include "screen.h"

// constructor
Screen::Screen() :
  display(Adafruit_SSD1306(OLED_RESET))
  {
  }

bool updateNeeded = false;

void Screen::setup()
{
   display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)  
   // utcOffset = TIME_OFFSET; 
   updateNeeded = false;
}


void Screen::showMainMenu(uint8_t selection)
{
  //this screen has 3 options
  int screenOptions = 3;
  if(selection > 0 && selection <= screenOptions)
  {
    display.setTextSize(1);
  
    display.setFont();
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0,0);
  
    // 8,16 spaces is fine
    // 18 spaces is 1 too many
    display.println("MENU");
    //display.println("-----------");
    display.setFont(&FreeMono9pt7b);
      display.setCursor(0,24);
    switch(selection)
    {
      case 1:
        display.println("  - LEDS -");
        display.println("   CLOCK ");
        display.println("   DOORS ");
        break;
      case 2:
        display.println("   LEDS");
        display.println("  - CLOCK -");
        display.println("   DOORS ");
        break;
      case 3:
        display.println("   LEDS");
        display.println("   CLOCK ");
        display.println("  - DOORS - ");
        break;
    }
    //display.display();
    updateNeeded = true;
      display.setFont();
  }
}

void Screen::showLEDMenu(uint8_t selection)
{
  //this screen has 3 options
  int screenOptions = 7;
  if(selection > 0 && selection <= screenOptions)
  {
    display.setTextSize(1);
  
    display.setFont();
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0,0);
  
    // 8,16 spaces is fine
    // 18 spaces is 1 too many
    display.println("LED MENU");
    //display.println("-----------");
    display.setFont(&FreeMono9pt7b);
      display.setCursor(0,24);
    switch(selection)
    {
      // Case 0 - 2 are screen one
      case 0:
        display.println("  - OFF -");
        display.println("   LED 1");
        display.println("   LED 2");
        break;
      case 1:
        display.println("   OFF");
        display.println("  - LED 1 -");
        display.println("   LED 2");
        break;
      case 2:
        display.println("   OFF");
        display.println("   LED 1");
        display.println("  - LED 2 -");
        break;
        
      // Case 3 - 5 are screen two
      case 3:
        display.println("  - LED 3 -");
        display.println("   LED 4");
        display.println("   LED 5");
        break;
      case 4:
        display.println("   LED 3");
        display.println("  - LED 4 -");
        display.println("   LED 5");
        break;
      case 5:
        display.println("   LED 3");
        display.println("   LED 4");
        display.println("  - LED 5 -");
        break;
  
      // Case 6 is screen three
      case 6:
        display.println("  - LED 6 -");
        break;
    }
    
    // display.display();
    updateNeeded = true;
    display.setFont();
  }
}

void Screen::showUTCOffset(uint8_t selection, int utcOffset)
{
  int screenOptions = 2;
  if(selection > 0 && selection <= screenOptions)
  {
  
    display.setTextSize(1);
    display.setFont();
    display.clearDisplay();
  
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    Serial.print("setFont!");
    // 8,16 spaces is fine
    // 18 spaces is 1 too many
    display.println("UTC OFFSET MENU");
  
    display.setFont(&FreeMono9pt7b);
    display.setCursor(0,24);
  
    display.print("   "); display.print(utcOffset); display.print("\r\n");
    
    
    switch(selection)
    {
      // Case 1 - 3 are screen one
      case 1:
        display.println(" - Add - ");
        display.println("  Subtract");
        break;
       case 2:
        display.println("  Add");
        display.println(" -Subtract-");
        break;
    }
    
    //display.display();
    updateNeeded = true;
    Serial.print("end of func!");
  }
}


void Screen::showConnieLovesErik()
{
    display.clearDisplay();

  // miniature bitmap display
  display.drawBitmap(0, 0,  connieLovesErik, 128, 64, 1);
  //display.display();
  updateNeeded = true;
}


void Screen::showSomaMessage()
{
    display.clearDisplay();

  // miniature bitmap display
  display.drawBitmap(0, 0,  somaMessage, 128, 64, 1);
  //display.display();
  updateNeeded = true;
}

void Screen::showYouMadeIt()
{
    display.clearDisplay();

  // miniature bitmap display
  display.drawBitmap(0, 0,  youMadeIt, 128, 64, 1);
  //display.display();
  updateNeeded = true;
}

void Screen::showNoGPS()
{
  /*
    display.clearDisplay();
   display.print("No GPS Fix");
  // miniature bitmap display
  display.drawBitmap(32, 0,  noGps, 64, 64, 1);
  display.display();
  */
}




void Screen::showStdLatLon(double latdeg, double latmin, double londeg, double lonmin, double altitude, double distance)
{
  double targetDistance = 0.0;
  int feet = 0;
   display.setFont();
  initLCD(1); // initialize LCD for size 1

  
  display.print("Latitude : "); 
  displayCoordinate(latdeg, latmin);

  display.print("Longitude: ");
  displayCoordinate(londeg, lonmin);
  
  display.print("Altitude : "); display.print(altitude); display.print(" ft\r\n");
  
  targetDistance = distance;    
  
  display.print("Distance : ");
  if(targetDistance >= 1.00)
  {
    display.print(targetDistance); display.print(" mi\r\n");
  }
  else
  {
    feet = (int)(targetDistance * 5280);
    display.print(feet); display.print(" ft\r\n");
  }
  
  //display.display();
  updateNeeded = true;
}


void Screen::showTime(uint8_t minute, uint8_t hour, uint8_t day, uint8_t month, uint8_t year)
{
  static bool toggleColon = false;
  //int hour = 0;
  int dispHour = 0;
  
  display.setFont(&FreeMono9pt7b);
  initLCD(1);
    display.setCursor(0,12);

  display.print("\r\n");
  display.print("  ");
  if(month < 10)
  {
    display.print("0");
  }
  display.print(month); display.print('/'); 

  if(day < 10)
  {
    display.print("0");
  }
  
  display.print(day); display.print('/');  display.print(year); display.print("\r\n");

  display.print("   ");
  // hour = (24 + hour + utcOffset)%24;
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

  if(minute < 10)
  {
    display.print("0");
  }
  display.print(minute); 
  if(hour < 12)
  {
    display.print("AM");
  }
  else
  {
    display.print("PM");
  }
  // display.display();
  updateNeeded = true;

  toggleColon = !toggleColon;
}

void Screen::showMissionReady()
{
  
  //display.setFont(&FreeMono9pt7b);
  initLCD(2);
  display.setCursor(0,0);
  //            "<press bt|
  display.print(" Time to\r\n");
  display.print("  travel!\r\n"); //Ready\r\n");
  
  updateNeeded = true;
}

void Screen::noGPSFix()
{
   display.print("No GPS Fix");
   //showNoGPS();
}

void Screen::updateScreen()
{
  if(updateNeeded)
  {
    display.display();
    updateNeeded = true;
  }
  updateNeeded = false;
}










/************************* Private Methods ************************/


void Screen::initLCD(int size)
{
  display.clearDisplay();
  display.setTextSize(size);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
}

void Screen::displayCoordinate(int degree, float minute)
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
