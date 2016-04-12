#include "tardis.h"

Tardis::Tardis() :
  pixel(Adafruit_NeoPixel(NEOPIXEL_NUM, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800)),
  strip(Adafruit_DotStar(96, DOTSTAR_BRG)),
  display(Adafruit_SSD1306(OLED_RESET)),
  gps(Adafruit_GPS(&Serial1)),
  // Create the motor shield object with the default I2C address
  motor_shield(Adafruit_MotorShield())
{
  for (int i=0; i<6; i++) {
    motors[i] = motor_shield.getMotor(i+1);
  }
}

void Tardis::setup()
{
  // Initialize inputs
  Serial.begin(115200);

  gps.begin(9600);
  //turn on RMC (recommended minimum) and GGA (fix data) including altitude
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // Request updates on antenna status, comment out to keep quiet
  gps.sendCommand(PGCMD_ANTENNA);
  Serial1.println(PMTK_Q_RELEASE);

  // Initialize outputs
  strip.begin();

  pixel.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.clearDisplay();

  // clear everything immediately.
  do_output();

  motor_shield.begin();
  for (int i=0; i<6; i++) {
    motors[i]->setSpeed(255);
  }
}

void Tardis::do_input()
{
  int val = 0;
  gps.read();
  // TODO: integrate seans button code
  val = !digitalRead(6);

  // TODO: serial read?
}

void Tardis::do_update()
{
  // TODO: demo update state.
}

void Tardis::do_output()
{
  strip.show();
  pixel.show();  // clear immediately.
  display.display();
}
