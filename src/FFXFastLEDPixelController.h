//
//  FFXFastLEDPixelController.h
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#ifndef FFX_FASTLED_PIXEL_CONTROLLER_H
#define FFX_FASTLED_PIXEL_CONTROLLER_H

#include "FFXPixelController.h"
/*! 
 *  FFXFastLEDPixelController - Class to wrap FastLED.show() and overall brighness settings.  Note that 
 *  the FFX framework uses its own brightness framework so different segments can 
 *  have different brightness levels.  This can be utilized to limit the overall Maximum
 *  brightness if needed.
 */ 
class FFXFastLEDPixelController : public FFXPixelController {
  public:
    FFXFastLEDPixelController( CRGB *initLeds, uint16_t numLeds ) : FFXPixelController( initLeds, numLeds ) { }
    virtual void updateBrightness( uint8_t newBrightness ) { FastLED.setBrightness( newBrightness ); }
    virtual void show() override { FastLED.show(); }
};

#endif
