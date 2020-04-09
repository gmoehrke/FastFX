#ifndef FFX_OVERLAY_H
#define FFX_OVERLAY_H

#include "FFXBase.h"

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
