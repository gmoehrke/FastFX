//
//  FFXAFXFader.cpp
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#include "FFXAutoFader.h"

void FFXAutoFader::updateFader() {
  // uint8_t oldValue = vValue;
  if (isFading()) {
    if (fadeTimer.isUp()) {
      fadeTimer.stop();
      vValue = targetValue;
    }
    else {
      vValue = fixed_map( fadeTimer.timeSinceTriggered(), 0, fadeTimer.getInterval(), prevValue, targetValue );
    }
    updated = true;
  }
  else {
    updated = false;
  }
}

void FFXAutoFader::setTarget( uint8_t newTarget ) {
      if (newTarget != targetValue) {
        // Changed from prevValue = targetValue - if changing mid-transition, we want to fade from the current value, not the previous target...
        prevValue = vValue;
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

