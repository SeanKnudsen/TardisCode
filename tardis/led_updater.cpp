#include "led_updater.h"


int LED_lut[] = {
    74, 75, 80, 81, 73, 76, 79, 82, 72, 77, 78, 83,
    60, 65, 66, 71, 61, 64, 67, 70, 62, 63, 68, 69,
    50, 51, 56, 57, 49, 52, 55, 58, 48, 53, 54, 59,
    38, 39, 44, 45, 37, 40, 43, 46, 36, 41, 42, 47,
    26, 27, 32, 33, 25, 28, 31, 34, 24, 29, 30, 35,
    12, 17, 18, 23, 13, 16, 19, 22, 14, 15, 20, 21,
     0,  5,  6, 11,  1,  4,  7, 10,  2,  3,  8,  9,
    84, 89, 90, 95, 85, 88, 91, 94, 86, 87, 92, 93,
};

HSVInterpolator  interpolate = HSVInterpolator(HSV(0.0, 1.0, 1.0), HSV(1.0, 1.0, 1.0), 0, 50000);
HSVInterpolator  fade = HSVInterpolator(HSV(0.4, 1.0, 1.0), HSV(0.4, 1.0, 0.0), 0, 5000);

void solid_fader_update(Adafruit_DotStar& strip, unsigned long now) {
    RGB color = interpolate.interpolate(now % 50001).toRgb();
    for( int i=0; i < 96; i++) {
        // strip pixels are GRB
        strip.setPixelColor(LED_lut[i], color.g, color.r, color.b);
    }
};

void solid_color_update(Adafruit_DotStar& strip, RGB color) {
    for( int i=0; i < 96; i++) {
        // strip pixels are GRB
        strip.setPixelColor(LED_lut[i], color.g, color.r, color.b);
    }
};

void chaser_update(Adafruit_DotStar& strip, unsigned long now) {
    RGB color = interpolate.interpolate(now % 50001).toRgb();
    int col = ((now / 50) % 12);
    if (col > 5) {
        col = 11 - col;
    }
    for(int i=0;i<96;i++) {
        int x = ((i % 24) / 4);
        if (x == col) {
            strip.setPixelColor(LED_lut[i], color.g, color.r, color.b);
        } else { 
            strip.setPixelColor(LED_lut[i], 0, 0, 0);
        }
    }

}
void spiral_update(Adafruit_DotStar& strip, unsigned long now) {
    RGB color = interpolate.interpolate(now % 50001).toRgb();
    unsigned int x = (now / 50) % 96;
    unsigned int tail = x/24 + (x % 24) * 4;
    
    for(int i=0;i<96;i++) {
        if( (i == tail) || 
            (i == (tail - 4) % 96) ||
            (i == (tail - 8) % 96) ||
            (i == (tail -12) % 96) ||
            (i == (tail -16) % 96) ||
            (i == (tail -16) % 96) ||
            (i == (tail -16) % 96) ||
                0
        ) {
            strip.setPixelColor(LED_lut[i], color.g, color.r, color.b);
        } else { 
            strip.setPixelColor(LED_lut[i], 0, 0, 0);
        }
    }
}

void pulse_update(Adafruit_DotStar& strip, unsigned long now) {
    
    RGB color = fade.interpolate(abs((int)(now % 10001)-5000)).toRgb();
    for( int i=0; i < 96; i++) {
        // strip pixels are GRB
        strip.setPixelColor(LED_lut[i], color.g, color.r, color.b);
    }
}
void random_update(Adafruit_DotStar& strip, unsigned long now) {
    if ((now % 1000) == 0) {
    for( int i=0; i < 96; i++) {
        // strip pixels are GRB
          RGB color = HSV(float(random(255))/255.0, 1.0, 0.5).toRgb();
          strip.setPixelColor(i, color.g, color.r, color.b);
    }
    }
}
