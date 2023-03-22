//
//  FFXTrigMotion.cpp
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#include "FFXTrigMotion.h"  

FFXTrigMotion::FFXTrigMotion() {
    limit = 255;
    currPhase = 0;
    currCycle = 0;
    cycleDelay = 0;
    cycleTarget = 0;
    progressFract = 0;
    motionType = TRIG_MOTION_SIN;
  };

FFXTrigMotion::FFXTrigMotion( uint16_t initLimit, trigMotionType initMotion, uint8_t initMin, uint8_t initMax, uint16_t initDelay ) : FFXTrigMotion() { 
    limit = initLimit;
    min = initMin;
    max = initMax;
    cycleDelay = initDelay;
    motionType = initMotion;
}

FFXTrigMotion::FFXTrigMotion( uint16_t initLimit, trigMotionType initMotion, uint8_t initMin, uint8_t initMax) : FFXTrigMotion(initLimit, initMotion, initMin, initMax, 0) {  }
  
uint8_t FFXTrigMotion::cyclesForPhase(uint8_t tPhase )  {
    uint8_t result = 1;
    switch (motionType) {
      case TRIG_MOTION_SIN : {   result = fixed_map(      sin8(fixed_map(tPhase, 0, limit, 0, 127)), 127, 255, min, max); break; }
      case TRIG_MOTION_CUBIC : { result = fixed_map(cubicwave8(fixed_map(tPhase, 0, limit, 0, 255)), 0, 255, min, max); break; }
      case TRIG_MOTION_QUAD : {  result = fixed_map( quadwave8(fixed_map(tPhase, 0, limit, 0, 255)), 0, 255, min, max); break; }
      case TRIG_MOTION_TRI : {   result = fixed_map(  triwave8(fixed_map(tPhase, 0, limit, 0, 255)), 0, 255, min, max); break; }
      case TRIG_MOTION_COS : {   result = fixed_map(      cos8(fixed_map(tPhase, 0, limit, 0, 127)), 127, 255, min, max); break; }
    }    
    return result;
  }

  uint16_t FFXTrigMotion::getNextPosition() {
    if (currPhase == limit) { 
      return limit-1; 
    }
    else if (currPhase==0) { 
      return 1; 
    }
    else { return forward ? currPhase+1 : currPhase-1; }
  }

  void FFXTrigMotion::step() {
    if (cycleDelay > 0) {
      cycleDelay = cycleDelay-1;
    }
    else if (currCycle>=cycleTarget) {
      if (currPhase == 0) { forward = true; } 
      else if (currPhase == limit) { forward = false; }
      currPhase = getNextPosition();
      currCycle=0;
      cycleTarget = cyclesForPhase(currPhase);
      progressFract = 0;
    }
    else {
      currCycle += 1;
      progressFract = fixed_map( currCycle, 0, cycleTarget, 0, 255);
    }
  }

  uint16_t FFXTrigMotion::getPosition() {
    return currPhase;
  }
