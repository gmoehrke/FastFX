#ifndef FFX_FASTLED_PIXEL_CONTROLLER_H
#define FFX_FASTLED_PIXEL_CONTROLLER_H

#include "FFXPixelController.h"

class FFXFastLEDPixelController : public FFXPixelController {
  public:
    FFXFastLEDPixelController( CRGB *initLeds, uint16_t numLeds ) : FFXPixelController( initLeds, numLeds ) { }
    virtual void updateBrightness( uint8_t newBrightness ) { FastLED.setBrightness( newBrightness ); }
    virtual void show() override { FastLED.show(); }
};

#endif
