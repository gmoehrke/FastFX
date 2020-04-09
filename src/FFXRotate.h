#ifndef FFX_ROTATE_H
#define FFX_ROTATE_H

#include "FFXBase.h"

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
