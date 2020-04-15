#include "Arduino.h"
#include "FlexTimer.h"

#define MS_IN_SEC      1000UL
#define MS_IN_MIN     60000UL
#define MS_IN_HOUR  3600000UL
#define MS_IN_DAY  86400000UL

#define DAYS_BEFORE_ROLLOVER 49

long fixed_map(long x, long in_min, long in_max, long out_min, long out_max) { if ((in_max - in_min) > (out_max - out_min)) { return (x - in_min) * (out_max - out_min+1) / (in_max - in_min+1) + out_min; } else { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; } }

void StepTimer::start(unsigned long currMillis) { 
      started = currMillis;
      nextUpMillis = addOffsetWithWrap(currMillis, interval); 
      if (nextUpMillis < currMillis) { rollovers++; }
      onStart( currMillis );
    }

void StepTimer::start() { 
  start(GET_TIME_MILLIS); 
}

void StepTimer::step() { 
  step(GET_TIME_MILLIS); 
}

void StepTimer::step(unsigned long currMillis) {
      if (pendInterval) {
        interval = pendInterval;
        pendInterval = 0;
      }
      if (started) {
        nextUpMillis = addOffsetWithWrap(currMillis, interval);
        if (nextUpMillis < currMillis) { rollovers++; }
        onStep(currMillis);
      }
    }    

void StepTimer::setInterval( unsigned long newInterval ) {
  if (newInterval != interval) {
    if (started) { 
      pendInterval = newInterval;
    }
    else {
      interval = newInterval;
    }
  }
}

void StepTimer::setIntervalImmediate(unsigned long newInterval) { 
  if (newInterval != interval) {
    interval = newInterval;   
    //unsigned long lastup = getLastUp();
    //if (isStarted() && lastup+interval < nextUpMillis) { nextUpMillis = (lastup+interval>GET_TIME_MILLIS ? GET_TIME_MILLIS : lastup+interval); } 
    step();
  }
}    
    
void FlexTimer::setRange( unsigned long minInterval, unsigned long maxInterval ) {
      if (maxInterval > MAX_INTERVAL) { maxInterval = MAX_INTERVAL; }
      if (minInterval < MIN_INTERVAL) { minInterval = MIN_INTERVAL; }      
      if (minInterval > maxInterval) { minInterval = maxInterval; } 
      rangeMin = minInterval;
      rangeMax = maxInterval;
}
  
void FlexTimer::onStart(unsigned long currMillis) {
  lastUpMillis = currMillis;
  nextUpMillis= addOffsetWithWrap(currMillis, (startExpired ? 0 : interval) );
  if (nextUpMillis < currMillis) { rollovers++; }
}

void FlexTimer::onStep(unsigned long currMillis) {
  lastUpMillis = currMillis;  
  currDelta = 0;
  stepCount = addOffsetWithWrap(stepCount, 1);
}

void FlexTimer::setInterval( unsigned long newInterval) {
   StepTimer::setInterval(newInterval < rangeMin ? rangeMin : (newInterval > rangeMax ? rangeMax : newInterval)); 
}
