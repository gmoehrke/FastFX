//
//  FFXPixelController.h
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#ifndef FFX_PIXEL_CONTROLLER_H
#define FFX_PIXEL_CONTROLLER_H

#include "FastLED.h"
/*! 
 *  FFXPixelController - The base class for pixel controllers.  Settings for total number of 
 *  LEDs and overall brightness.
 * 
 */ 
class FFXPixelController {
  private:
    CRGB *leds = NULL;
    uint16_t numLeds = 0;
    uint8_t currBrightness = 0;

  public:
    FFXPixelController( CRGB *initLeds, uint16_t initNum );
    virtual ~FFXPixelController() {};
    virtual void show()=0;
    virtual void updateBrightness( uint8_t newBrightness ) = 0;
    virtual void setBrightness(uint8_t newBrightness);
    uint8_t getBrightness() { return currBrightness; }
    CRGB* getLeds() { return leds; }
    void setLeds( CRGB *newLeds ) { leds = newLeds; };
    uint16_t getNumLeds() { return numLeds; }
    void setNumLeds( uint16_t newNum ) { numLeds = newNum; }
};

#endif
