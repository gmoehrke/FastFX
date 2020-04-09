/*
 *   Definition of FFXTrigMotion.  
 *   
 * Use the trig/wave functions to create motion that speeds up at the end-points and slows in the middle (or vice versa).  The method does not adjust
 * core timing within the effect.  Instead, it employs the use of extra cycles to slow the motion where desired.  The object uses a min/max range
 * which is used to control the range in variation of the speed.  
 *
 */

#ifndef FFX_TRIG_MOTION_H
#define FFX_TRIG_MOTION_H

#include "FastLED.h"
#include "FlexTimer.h"

class FFXTrigMotion {  

public:
  enum trigMotionType { TRIG_MOTION_SIN, TRIG_MOTION_CUBIC, TRIG_MOTION_QUAD, TRIG_MOTION_TRI, TRIG_MOTION_COS };
  uint8_t cyclesForPhase(uint8_t tPhase );

  FFXTrigMotion();
  FFXTrigMotion( uint16_t initLimit, trigMotionType initMotion, uint8_t initMin, uint8_t initMax );
  FFXTrigMotion( uint16_t initLimit, trigMotionType initMotion, uint8_t initMin, uint8_t initMax, uint16_t initDelay );
  
  void setMotion( trigMotionType newMotion )  { motionType = newMotion; }
  void setLimit( uint16_t newLimit ) { limit = newLimit; }
  uint8_t getRangeMin() { return min; }
  void setRangeMin( uint8_t newMin ) { min = newMin; }
  uint8_t getRangeMax() { return max; }
  void setRangeMax( uint8_t newMax ) { max = newMax; }
   
  uint8_t getPhase() { return currPhase; }
  void setPhase( uint8_t newPhase );
  uint16_t getDelay() { return cycleDelay; }
  uint8_t getCurrCycle() { return currCycle; }
  uint8_t fractComplete() { return progressFract; }
  uint16_t getNextPosition();
  uint16_t getPosition();
  void step();

private:
  trigMotionType motionType = TRIG_MOTION_SIN;
  uint16_t limit = 255;         // Limit is the number of LEDs to calculate the motion (upper bound) 
  uint8_t min = 1;              // The baseline minimum number of cycles for each step to last.  This will be the shortest duration and set the overall 
                                // speed of motion (along with whatever time is used between calls to getPosition())
  uint8_t max = 5;              // this sets the upper range of the number of cycles - the slowest phases will min + max number of cycles  

  uint8_t currPhase = 0;            // Internally track the phase - for successive calls to getPosition()
  uint8_t currCycle = 1;            // Internally track the cycle for each phase - stepping to the next phase when complete;
  uint8_t cycleTarget = 1;
  uint8_t progressFract = 1;
  uint16_t cycleDelay = 0;
  boolean forward = true;  
};

#endif