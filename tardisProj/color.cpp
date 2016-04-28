#include "color.h"
#include <stdio.h>

RGB HSV::toRgb() {
    RGB result;

    int i = int(h * 6);
    double f = h * 6 - i;
    double p = v * (1 - s);
    double q = v * (1 - f * s);
    double t = v * (1 - (1 - f) * s);

    switch(i % 6){
        case 0:
            result.r = 255 * v;
            result.g = 255 * t;
            result.b = 255 * p;
            break;
        case 1:
            result.r = 255 * q;
            result.g = 255 * v;
            result.b = 255 * p;
            break;
        case 2:
            result.r = 255 * p;
            result.g = 255 * v;
            result.b = 255 * t;
            break;
        case 3:
            result.r = 255 * p;
            result.g = 255 * q;
            result.b = 255 * v;
            break;
        case 4:
            result.r = 255 * t;
            result.g = 255 * p;
            result.b = 255 * v;
            break;
        case 5:
            result.r = 255 * v;
            result.g = 255 * p;
            result.b = 255 * q;
            break;
    }
    return result;
}

HSVInterpolator::HSVInterpolator(HSV a, HSV b, unsigned long start, unsigned long end) :
    a(a),
    b(HSV(b.h - a.h, b.s - a.s, b.v - a.v)),
    start(start),
    end(end)
{
}

HSV HSVInterpolator::interpolate(unsigned long value)
{
    double t = (double)(value) / (double)(end - start);
    return HSV(a.h + b.h * t,
               a.s + b.s * t,
               a.v + b.v * t);
}
