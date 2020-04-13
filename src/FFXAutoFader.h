//
//  FFXAutoFader.h
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
//   
#ifndef FFX_AUTO_FADER_H
#define FFX_AUTO_FADER_H

#include "FlexTimer.h"
#include "FFXBase.h"
/*!
 *  FFXAutoFader - Abstract class to implement timed fading.  Abstract
 *  method onUpdate( CRGBSet& ) should be overriden in descendant classes
 *  to implement the "effect".  The update() method is included in looping
 *  to "track" the status of the fader.  Target attribute always refers to the 
 *  desired/set value (0-255), while the Value attribute refers to the current
 *  value.  Upon a call to setTarget(), the fader will move consistently from 
 *  its current value to the target value over the desired interval.
 */ 
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