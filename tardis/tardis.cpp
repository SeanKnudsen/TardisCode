#include "tardis.h"

Location location = Location();
Screen screen = Screen();
Button button = Button();
useCase uberState;
shelfCase shelfState;

bool batteryPower = false;



Tardis::Tardis() :
  pixel(Adafruit_NeoPixel(NEOPIXEL_NUM, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800)),
  strip(Adafruit_DotStar(DOTSTAR_COUNT, DOTSTAR_BRG)),
  //display(Adafruit_SSD1306(OLED_RESET)),
  //gps(Adafruit_GPS(&Serial1)),
  // Create the motor shield object with the default I2C address
  motor_shield(Adafruit_MotorShield()),
  interpolate(HSVInterpolator(HSV(0.0, 1.0, 1.0), HSV(1.0, 1.0, 1.0), 0, 50000))
{
  solenoids[0] = new Solenoid(motor_shield.getMotor(1), OUTER);
  solenoids[1] = new Solenoid(motor_shield.getMotor(1), INNER);
  solenoids[2] = new Solenoid(motor_shield.getMotor(2), INNER);
  solenoids[3] = new Solenoid(motor_shield.getMotor(2), OUTER);
  solenoids[4] = new Solenoid(motor_shield.getMotor(3), OUTER);
  solenoids[5] = new Solenoid(motor_shield.getMotor(3), INNER);

// Power switch 32, 33; Switch closed when on wall power (we think!)
  pinMode(32, OUTPUT);
  digitalWrite(32, LOW);
  pinMode(33, INPUT);
  digitalWrite(33, HIGH);       // turn on p

  batteryPower = !digitalRead(33);
  if(batteryPower)
  {
      Serial.println("I am on battery power");
  }
  else
  {
      Serial.println("I am on wall power");
  }
  
}
/*
void pin_ISR()
{
  button.pinISR();
}
*/

void Tardis::updateSolenoids()
{
  unsigned long now = millis();
  for (int i=0; i < SOLENOIDS_NUM; i++) {
    solenoids[i]->update(now);
  }
}

void Tardis::setup()
{
  //initialize state machine when we first boot up
  uberState = shelfMode;
  shelfState = shelfClock;
  
  // Initialize inputs
  Serial.begin(115200);
  location.setup();
  location.setTarget(TARGET_LAT, TARGET_LON);

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
  for (int i=0; i<SOLENOIDS_NUM; i++) {
    solenoids[i]->setup();
  }
}

void Tardis:: doCollectGPS()
{
  location.collectGPS();
}

void Tardis::solenoidTest()
{
  static int activeSolenoid = SOLENOIDS_NUM-1;
  
  if ( solenoids[activeSolenoid]->getState() == REST && solenoids[(activeSolenoid+1)%SOLENOIDS_NUM]->getState() == REST ) {
    activeSolenoid = (activeSolenoid+1) % SOLENOIDS_NUM;
    solenoids[activeSolenoid]->energize();
  }
}

void Tardis::do_input()
{
  // I think we can read GPS no matter what?
  location.collectGPS();
  
  solenoidTest();

    // TODO: serial read?
}

void Tardis::do_update()
{
  static unsigned long lastShowTime = 0;
  static int menuState = 1;
  static int ledState = 0;
  static int subMenuState = 0;
  unsigned long now = millis();
  static bool timeInitialized = false;
  RGB color;
  
  button.pinPoll();
  if(!timeInitialized)
  {
    lastShowTime = now;
    timeInitialized = true;
  }
  switch(uberState)
  {
    case weddingMode:
    
      break;
    case shelfMode:

      switch(shelfState)
      {
        case shelfClock:
          if((now - lastShowTime) >=1000)
          {
            screen.showTime(location.Minute, location.Hour, location.Day, location.Month, location.Year);
            lastShowTime = now;

           }

         // TODO: demo update state.
          color = interpolate.interpolate(now % 50001).toRgb();
          for( int i=0; i < DOTSTAR_COUNT; i++) {
              strip.setPixelColor(i, color.r, color.g, color.b);
          }
          pixel.setPixelColor(0, color.r, color.g, color.b);
       
          updateSolenoids();


        
          if(button.MenuPress)
          {
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

            break;

            case subDoors:

            break;
          }

        
            // TODO: implement timeout to go back to shelfClockMode
          break;
        case shelfMissionReady:

          break;
      }

      button.clearButtonPress();

      break;
    case seekMode:

      break;
  }





  

}



void Tardis::do_output()
{
  strip.show();
  pixel.show();  // clear immediately.
  //display.display();
}
