/*
 *  FXPixelFader - Abstract class to handle timed fading/dimming/transitions.  Maintains a target and current value.  Uses an internal timer to handle timed fades from current value to next.
 * 
 *  Descendant class need only override the apply() method to act on a current CRGBSet object.  
 * 
 */

#ifndef FFX_AUTO_FADER_H
#define FFX_AUTO_FADER_H

#include "FlexTimer.h"
#include "FFXBase.h"

class FFXAutoFader {
  public:
    FFXAutoFader() { }
    virtual ~FFXAutoFader() { }
    bool isFading() { return fadeTimer.isStarted(); }
    bool isUpdated() { return updated; }
    uint8_t getTarget() { return targetValue; }
    void setTarget( uint8_t newTarget );
    uint8_t getValue() { return vValue; }
    void update( CRGBSet &pixels ) {
        updateFader();
        this->onUpdate( pixels );
    };    
    virtual void onUpdate( CRGBSet &pixels ) = 0;
    void setInterval( uint16_t ms) { fadeTimer.setInterval(ms); }
    uint16_t getInterval() { return fadeTimer.getInterval(); }
    void updateFader();
  private:
    uint8_t vValue = 0;
    uint8_t targetValue = 0;
    uint8_t prevValue = 0;
    bool updated = true;
    StepTimer fadeTimer = StepTimer( 1000, false );
};

#endif