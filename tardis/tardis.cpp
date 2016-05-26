#include "tardis.h"

Location location = Location();
Memory memory = Memory();
Screen screen = Screen();
Button button = Button();
useCase uberState;
shelfCase shelfState;
SerialCom serialcom = SerialCom();

bool batteryPower = false;
bool targetFound = false;

HSVInterpolator cap = HSVInterpolator(HSV(0.0, 0.0, 1.0), HSV(0.0, 0.0, 0.0), 0, 2500);

HSVInterpolator i_inst = HSVInterpolator(HSV(0.42, 0.0, 1.0), HSV(0.42, 0.0, 0.0), 0, 25);
HSVInterpolator i_door = HSVInterpolator(HSV(0.58, 0.0, 1.0), HSV(0.58, 0.0, 0.0), 0, 25);
HSVInterpolator i_clos = HSVInterpolator(HSV(0.0, 0.0, 1.0), HSV(0.0, 0.0, 0.0), 0, 25);
HSVInterpolator i_home = HSVInterpolator(HSV(0.7, 0.0, 1.0), HSV(0.7, 0.0, 0.0), 0, 25);
unsigned long flash_start = 0;
int flash_pattern = 1;


Tardis::Tardis() :
  pixel(Adafruit_NeoPixel(NEOPIXEL_NUM, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800)),
  strip(Adafruit_DotStar(DOTSTAR_COUNT, DOTSTAR_BRG)),
  //display(Adafruit_SSD1306(OLED_RESET)),
  //gps(Adafruit_GPS(&Serial1)),
  // Create the motor shield object with the default I2C address
  motor_shield(Adafruit_MotorShield()),
  interpolate(HSVInterpolator(HSV(0.0, 1.0, 1.0), HSV(1.0, 1.0, 1.0), 0, 50000))
{
// Tardis Doors
// +-----+
// | ░ ░ |
// | 0 3 |
// | 1 4 |
// | 2 5 |
// +-----+

  solenoids[0] = new Solenoid(&motor_shield, M1, OUTER); 			// Top Left
  solenoids[1] = new Solenoid(&motor_shield, M2, INNER);			// Mid Left
  solenoids[2] = new Solenoid(&motor_shield, M3, OUTER);			// Bottom Left
  solenoids[3] = new Solenoid(&motor_shield, M1, INNER);			// Top Right
  solenoids[4] = new Solenoid(&motor_shield, M2, OUTER);			// Mid Right
  solenoids[5] = new Solenoid(&motor_shield, M3, INNER);			// Bottom Right

// Power switch 32, 33; Switch closed when on wall power (we think!)
  pinMode(32, OUTPUT);
  digitalWrite(32, LOW);
  pinMode(33, INPUT);
  digitalWrite(33, HIGH);       // turn on p

  batteryPower = !digitalRead(33);
  if(batteryPower)
  {
      //Serial.println("I am on battery power");
  }
  else
  {
      //Serial.println("I am on wall power");
  }
  
}
/*
void pin_ISR()
{
  button.pinISR();
}
*/


void Tardis::setup()
{
  //initialize state machine when we first boot up
  uberState = shelfMode;
  shelfState = shelfClock;
  
  // Initialize inputs
  Serial.begin(115200);
  serialcom.setup();
  location.setup();
  
  // load which doors have been opened from memory
  memory.setup();
  // set tp_index accordingly
  location.tp_index = memory.indexOfCurrentDoor();
  
  location.setTarget();

  // Setup Button as interrupt
  // pinMode(BUTTON_PIN, INPUT_PULLUP);           
  // attachInterrupt(0, pin_ISR, CHANGE);
  button.setup();

  // Initialize outputs
  strip.begin();

  pixel.begin();

  screen.setup();

  // clear everything immediately.
  do_output();

  motor_shield.begin();
  Solenoid::beginAll();
}

void Tardis::do_timerUpdate()
{
  location.collectGPS();
}





void Tardis::do_input()
{
  // I think we can read GPS no matter what?
  location.collectGPS();
  serialcom.checkForCommand();

  if(serialcom.gotCommand)
  {
    switch(serialcom.cmdCommand)
    {
      case OPEN_DOOR:
        if(serialcom.cmdParam >= 0 && serialcom.cmdParam <=5)
        {
          solenoids[serialcom.cmdParam]->energize();
        }
        break;

      case LED:
        // Pattern displayed here?
        flash_start = millis();
        flash_pattern = serialcom.cmdParam;
 
        break;
    }

    serialcom.gotCommand = false;
  }
}

void Tardis::do_update()
{
  static unsigned long lastShowTime = 0;
  static int menuState = 1;
  static int ledState = 0;
  static int subMenuState = 0;
  unsigned long now = millis();
  static bool timeInitialized = false;
  static bool stripCleared = false;
  RGB color;
  
  
  button.pinPoll();
  if(!timeInitialized)
  {
    lastShowTime = now;
    timeInitialized = true;
  }

  // uberState {weddingMode, shelfMode, seekMode}
  
  // when in shelfMode, we have the following substates
  // shelfCase { shelfClock, shelfMenu, shelfMissionReady }

  // when in shelfMenu, we have the following substates
  // subMenuCase { subMenu, subLED, subClk, subDoors }
  // 
  switch(uberState)
  {
    case weddingMode:
    
      break;
    case shelfMode:

      switch(shelfState)
      {
        case shelfClock:


          // TODO: demo update state.
          if (flash_start != 0) {
              switch (flash_pattern) {
                  case 1:
                      color = i_home.interpolate(abs((int)(now % 51) - 25)).toRgb();
                      break;
                  case 2:
                      color = i_inst.interpolate(abs((int)(now % 51) - 25)).toRgb();
                      break;
                  case 3:
                      color = i_door.interpolate(abs((int)(now % 51) - 25)).toRgb();
                      break;
                  case 4:
                      color = i_clos.interpolate(abs((int)(now % 51) - 25)).toRgb();
                      break;
              }
              for (int i = 0; i < 96; i++) {
                  strip.setPixelColor(i, color.g, color.r, color.b);
              }

              if ((now - flash_start) > 500) {
                  flash_start = 0;
              }
          } else {
              switch ((now / 30000) % 4) {
                  case 0:
                      batteryPower ? chaser_update(strip, now): random_update(strip, now);
                      if((now - lastShowTime) >=1000)
                      {
                        screen.showTime(location.Minute, location.Hour, location.Day, location.Month, location.Year);
                        lastShowTime = now;
                      }
                      break;
                  case 1:
                      batteryPower ? chaser_update(strip, now): solid_fader_update(strip, now);
                      //screen.showStdLatLon(location.LatDeg, location.LatMin, location.LonDeg, location.LonMin, location.Altitude, location.Distance);
                      if((now - lastShowTime) >=1000)
                      {
                        screen.showTime(location.Minute, location.Hour, location.Day, location.Month, location.Year);
                        lastShowTime = now;
                      }
                      break;
                  case 2:
                      batteryPower ? chaser_update(strip, now): chaser_update(strip, now);
                      screen.showConnieLovesErik();
                      break;
                  case 3:
                      batteryPower ? chaser_update(strip, now): pulse_update(strip, now);
                      screen.showConnieLovesErik();
                      break;
              }
          }

          color = cap.interpolate(abs((int)(now % 5001) - 2500)).toRgb();
          pixel.setPixelColor(0, color.r, color.g, color.b);

          // TODO: We need to check flash and ensure not all missions have been completed. Once missions are all done, we don't want the tardis to keep moving to shelfMissionReady all the time.
          if(location.TPIndexReady())
          {
            // It's Time for an adventure! 
            shelfState = shelfMissionReady;
          }

          // Note, if for some reason the button is pressed at exactly midnight on an anniversary, we'll let the button overrule the mission ready.
          if(button.LongPress)
          {
            //uncomment to allow button to do anything
            shelfState = shelfMenu;
          }

          // TODO: add check to see if we're close enough to switch to mission ready
          break;

        case shelfMenu:
        
          switch (subMenuState)
          {
            case subMenu:
              screen.showMainMenu(menuState);
              if(button.ShortPress)
              {
                menuState = (menuState + 1);
                menuState = (menuState >= 4 )? 1 : menuState;
              }
              else if (button.LongPress)
              {
                subMenuState = menuState;
                ledState = 0;
              }
              break;
            case subLED:
              // LEDS!
              screen.showLEDMenu(ledState);
              if(button.ShortPress)
              {
                ledState = (ledState + 1);
                ledState = (ledState >= 7 )? 1 : ledState;
              }
            break;
            
            case subClk:
              //screen.showUTCOffset(
            break;

            case subDoors:

            break;
          }
            // TODO: implement timeout to go back to shelfClockMode
          break;
          
        case shelfMissionReady:
          screen.showMissionReady();
          if(button.ShortPress)
          {
            uberState = seekMode;     // Transition to new uber state
            
            // "Reset" substates of the former uber state
            shelfState = shelfClock;  
            subMenuState = subMenu;  
          }
          else if(button.LongPress)
          {
            shelfState = shelfMenu;
            subMenuState = subMenu;
          }
          
          location.setTarget();
          break;
      }

      break;
    case seekMode:
      screen.showStdLatLon(location.LatDeg, location.LatMin, location.LonDeg, location.LonMin, location.Altitude, location.Distance);
      
      // lights go blue to red as tardis gets closer.
      // on time is 333 ms of 1 second total
      if(now - lastShowTime < 333)
      {
        if(!targetFound && location.Distance < 1.0)
        {
          // get color
          if(location.Distance * 5280 > 2640)
          {
            // we're still kinda far, so have a deep blue, and increase red as we approach.
            
            // As distance decreases, color.r will increase
            // e.g. dist is 1.0 miles, r will be 0
            // e.g. dist is 2640.1, r will be 239 (I chose divide by 11 so the result would be less than 255
            color.r = (5280 - (location.Distance * 5280))/11;
            color.g = 0;    // green is fixed in this mode
            color.b = 255;  //blue is fixed in this mode        
          }
          else
          {
            // we're getting closer, so max out red, and slowly decrease the amount of blue.
            color.r = 255;  // red is fixed in this mode
            color.g = 0;    // green is fixed in this mode
  
            // As distance decreases, color b will decrease
            // e.g. dist is 2640', b will be 240
            // e.g. dist is 35', b will be 3
            color.b = (location.Distance * 5280)/11;
          }
          
          solid_color_update(strip, color);
        }
      }
      else
      {
        if(!stripCleared)
        {
          strip.clear(); // turn strip off
          stripCleared = true;
        }

        if(now - lastShowTime > 1000)
        {
          lastShowTime = now;
          stripCleared = false;
        }
        
      }
      // must convert distance to feet first!!!
      if((location.Distance * 5280) <= FOUND_DISTANCE)
      {
        if(!targetFound)
        {
          // They made it!
          targetFound = true;

          // Save results
          memory.values.doorsOpened[location.tp_index] = true;
          memory.saveConfig();

          // Update LED animation and Top Light
          chaser_update(strip, now); // Question: Is this the appropriate animation?
          color = cap.interpolate(abs((int)(now % 5001) - 2500)).toRgb();
          pixel.setPixelColor(0, color.r, color.g, color.b);
          
          // TODO: Do success sounds

          // Open the door!
          if(location.tp_index>= 0 && location.tp_index <=5)
          {
            solenoids[location.tp_index]->energize();
          }
          
          // TODO: Mark door as opened and save to flash
          
          // display the "You Made It!" screen
          screen.showYouMadeIt();
          lastShowTime = now;

          // if tp_index equals zero, we've done everything!
          // Perhaps once we're off the end we need a "found everything" state. 
          location.tp_index = (location.tp_index + 1)% 6;
        }

        // switch between soma message and you made it each 10 seconds forever...
        if(now - lastShowTime > 10000)
        {
          screen.showSomaMessage();    
        }

        if(now - lastShowTime > 20000)
        {
          screen.showYouMadeIt();
          lastShowTime = now;
        } 
      }

      if(button.LongPress || button.ShortPress)
      {
        uberState = shelfMode;
        targetFound = false;
        
        // "Reset" substates of the former uber state
        shelfState = shelfClock;  
        subMenuState = subMenu;  
      }
      break;
  }
  
  button.clearButtonPress();
}

void Tardis::do_output()
{
  Solenoid::updateAll();
  strip.show();
  pixel.show();  // clear immediately.
  screen.updateScreen();
  //display.display();
}
