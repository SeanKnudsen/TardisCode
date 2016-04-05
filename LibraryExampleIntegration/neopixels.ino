void setup_neopixels() {
  // FIXME: this Trinket stuff exists for dotstar and neopixels
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  neopixel_strip.begin();
  neopixel_strip.show(); // Initialize all pixels to 'off'
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<neopixel_strip.numPixels(); i++) {
      neopixel_strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    neopixel_strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return neopixel_strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return neopixel_strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return neopixel_strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
