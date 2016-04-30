#ifndef _LED_UPDATER_H_
#define _LED_UPDATER_H_

#include <Adafruit_DotStar.h>

void solid_fader_update(Adafruit_DotStar& strip, unsigned long now);
void chaser_update(Adafruit_DotStar& strip, unsigned long now);
void spiral_update(Adafruit_DotStar& strip, unsigned long now);
void pulse_update(Adafruit_DotStar& strip, unsigned long now);
void random_update(Adafruit_DotStar& strip, unsigned long now);

#endif
