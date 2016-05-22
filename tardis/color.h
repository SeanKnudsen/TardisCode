#ifndef _COLOR_H_
#define _COLOR_H_
#include <stdint.h>

struct RGB {
    public:
      RGB(): r(0), g(0), b(0) {}
      RGB(uint8_t r, uint8_t g, uint8_t b): r(r), g(g), b(b) {}
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct HSV {
    public:
      HSV(): h(0), s(0), v(0) {}
      HSV(double h, double s, double v): h(h), s(s), v(v) {}
      RGB toRgb();
    double h;
    double s;
    double v;
};

class Interpolator {
    public:
    Interpolator(float a, float b, float start, float end);
    float interpolate(float value);
    private:
      float a;
      float b;
      float start;
      float end;
};

class HSVInterpolator {
    public:
    HSVInterpolator(HSV a, HSV b, unsigned long start, unsigned long end);
    HSV interpolate(unsigned long value);
    private:
      HSV a;
      HSV b;
      unsigned long start;
      unsigned long end;
};

#endif
