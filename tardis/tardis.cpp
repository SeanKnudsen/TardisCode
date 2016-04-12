#include "tardis.h"

Tardis::Tardis() :
  pixel(Adafruit_NeoPixel(NEOPIXEL_NUM, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800)),
  strip(Adafruit_DotStar(DOTSTAR_COUNT, DOTSTAR_BRG)),
  display(Adafruit_SSD1306(OLED_RESET)),
  gps(Adafruit_GPS(&Serial1)),
  // Create the motor shield object with the default I2C address
  motor_shield(Adafruit_MotorShield()),
  interpolate(HSVInterpolator(HSV(0.0, 1.0, 1.0), HSV(1.0, 1.0, 1.0), 0, 50000))
{
  for (int i=0; i<6; i++) {
    solenoids[i] = new Solenoid(motor_shield.getMotor(i+1));
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
    solenoids[i]->setup();
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
  unsigned long now = millis();
  // TODO: demo update state.
  RGB color = interpolate.interpolate(now).toRgb();
  for( int i=0; i < DOTSTAR_COUNT; i++) {
      strip.setPixelColor(i, color.r, color.g, color.b);
  }
  pixel.setPixelColor(0, color.r, color.g, color.b);

  // Update Solenoids
  for (int i=0; i < 6; i++) {
    solenoids[i]->update(now);
  }
  if (solenoids[0]->getState() == REST) {
    solenoids[0]->open();
  }
  if (solenoids[2]->getState() == REST) {
    solenoids[2]->open();
  }
}

void Tardis::do_output()
{
  strip.show();
  pixel.show();  // clear immediately.
  display.display();
}
