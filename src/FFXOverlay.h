//
//  FFXOverlay.h
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#ifndef FFX_OVERLAY_H
#define FFX_OVERLAY_H

#include "FFXBase.h"

/*!
 *  FFXOverlay - Base class of overlay effects.  Overlays can be sized independently
 *  and independent from the strip they are run against.  The overlay has an
 *  array of "Alpha" values, which determines the opacity of each pixel in the 
 *  effect.  This can be used to make the entire overlay, or segments of it, partially
 *  transparent.  
 *  
 *  Overlay effects have independent speed, a repeat value (how many times the cycle will run),
 *  and a repeat delay (the number of milliseconds between each repeat). 
 */
class FFXOverlay : public FFXBase {
  protected:
       bool continuous = false;
       bool completed = false;
       uint8_t repeat = 1;
       unsigned long repeatDelayms = 500;
       unsigned long nextCycleStart = 0;
       std::vector<uint8_t> alpha;
       uint8_t maxAlpha = 255;

   public:
     FFXOverlay( uint16_t initSize, uint8_t initSpeed, bool initCont ) : FFXBase( initSize, initSpeed, 0, 50 ) {
       alpha.resize(initSize);
       alpha.clear();
       continuous = initCont;
     }
     FFXOverlay( uint16_t initSize, bool initCont ) : FFXOverlay(initSize, 126, initCont) {
     }
     FFXOverlay( uint16_t initSize ) : FFXOverlay( initSize, 126, false ) {
     }
     FFXOverlay( uint16_t initSize, uint8_t initSpeed, uint8_t numCycles, unsigned long cycleDelay ) : FFXOverlay(initSize, initSpeed, false) {
       repeat = numCycles;
       repeatDelayms = cycleDelay;
     }
    ~FFXOverlay() {
      alpha.clear();
    }

     std::vector<uint8_t> *getAlpha() { return &alpha; }
     virtual void applyOverlay( CRGB* overlay, CRGB* leds );
     virtual void whileFrozen() override;

     virtual void onVCycleEnd() override;
     bool isDone() { return completed; }

    void setLag( unsigned long newLag ) { if (repeatDelayms != newLag) { repeatDelayms = newLag; } }
    unsigned long getLag() { return repeatDelayms; }
    
    void setMaxAlpha( uint8_t newMax ) {
      if (newMax != maxAlpha) { maxAlpha = newMax; }
    }

    uint8_t getMaxAlpha() { return maxAlpha; }
    
  protected:

    /*  Move a smaller "a" buffer into larger "b" buffer - use with caution - very little range checking.
     *  Bad parameters can cause GPF's easily since this uses generic pointers and memmove operations...
     */
    void moveAintoB( byte *a, byte *b,  uint16_t size_a, uint16_t size_b, uint16_t end_position, size_t elementsize );
    
};
#endif
