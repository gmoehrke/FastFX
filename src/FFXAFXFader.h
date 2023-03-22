//
//  FFXAFXFader.h
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
//   
#ifndef FFX_AF_X_FADER_H
#define FFX_AF_X_FADER_H

#include "FFXAutoFader.h"

/*!
 *  FFXAFXfader - Autofader for Opacity, must use a background buffer to facilitate 
 *  fading.  Buffer is automatically allocated as needed.  May be deallocated by calling
 *  freeBackgroundBuffer().  Override for onUpdate() draws fame blended with background
 *  contained in the buffer.
 */
class FFXAFXFader: public FFXAutoFader {
  public:
    FFXAFXFader(uint16_t pixels) : FFXAutoFader() { size = pixels; }
    virtual ~FFXAFXFader() { if (background) { free( background); } }
    virtual void onUpdate( CRGBSet &pixels ) override; 
    CRGB *getBackgroundBuffer() { if (!background) { background = (CRGB *)malloc(sizeof(CRGB)*size); } return background; } 
    void freeBackgroundBuffer() { if (background) { free( background ); background = nullptr; } }
    private:
      uint16_t size;
      CRGB* background = nullptr;   
};

#endif