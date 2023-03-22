//
//  FFXRotate.h
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#ifndef FFX_ROTATE_H
#define FFX_ROTATE_H

#include "FFXBase.h"
/*!
 *  FFXRotate - Helper base class for FX that rotate a static set of pixels continuously.  
 *  Allows only the contructor and the `fillLeds()` method to be overriden to implement
 *  the effect.
 * 
 */ 
class FFXRotate : public FFXBase {

  protected:
     boolean redrawFull = true;

  public:
    FFXRotate( uint8_t initSize, unsigned long initTimer, unsigned long minRefresh, unsigned long maxRefresh ) :FFXBase( initSize, initTimer, minRefresh, maxRefresh )  {}
    virtual void setColor( CRGB newColor ) override;
    virtual void fillLeds(CRGB *bufLeds, uint16_t phase) = 0;
    virtual bool rotate(CRGB *bufLeds, uint16_t steps );
    virtual void writeNextFrame(CRGB *bufLeds) override; 
};

#endif
