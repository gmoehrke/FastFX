
#include "FFXAutoFader.h"

void FFXAutoFader::updateFader() {
  uint8_t oldValue = vValue;
  if (isFading()) {
    if (fadeTimer.isUp()) {
      fadeTimer.stop();
      vValue = targetValue;      
    }
    else {
      vValue = fadeTimer.isStarted()  
                ? fixed_map( fadeTimer.timeSinceTriggered(), 0, fadeTimer.getInterval(), prevValue, targetValue ) 
                : targetValue; 
    }
  }
  updated = (fadeTimer.isStarted() || (vValue != oldValue));
}

void FFXAutoFader::setTarget( uint8_t newTarget ) {
      if (newTarget != targetValue) {
        prevValue = targetValue;
        targetValue = newTarget;
        updated = true;
        if (fadeTimer.isStarted()) {
          fadeTimer.step();
        }
        else {
          fadeTimer.start();
        }
      }
    }

