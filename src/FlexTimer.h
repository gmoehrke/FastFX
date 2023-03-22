/*
 *    FlexTimer.h - General purpose passive timer classes.  
 *   
 *                  Useful for timing things without using callbacks, virtual method calls, or delay()'s.  Simple usage model for integration into looping 
 *                  algorithms:
 *
 *                  StepTimer - Basic timer to watch for interval to elapse then return true after that amount of time.  Calling step() resets the 
 *                              timeout for the next interval.  
 * 
 *                  StepTimer timer( 1000 );
 *                  ...
 *                  timer.start();
 *                  ...
 *                  while (true) {
 *                    if timer.isUp() {
 *                                              // do stuff here - realize that stuff done here is "after cycle" so each cycle will 
 *                                              // use the time elapsed by the timer PLUS whatever time is taken by steps performed here.
 *                      
 *                      timer.step();           // timer will be "up" again exactly 1000 ms after this line...       
 *                      
 *                                              // ...or do stuff here - this is occuring while the timer is "running".  So, as long
 *                                              // as these steps take less time than the timer interval (1000 ms), each cycle will be the exact same
 *                                              // duration
 *                    }
 *  
 *                FlexTimer is a bit more robust and adds the ability to set a range for minimum and maximum intervals, the ability to change the duration
 *                of a step in progress (by calling addDelta()), and the ability to set the "speed", which is specified as 0 - 255 and evenly 
 *                spread of the range between the minimum and maximum interval values.
 *                
 */
#ifndef GP_TIMER_LIB

#ifndef FLEX_TIMER_H
#define FLEX_TIMER_H

#ifndef DEFAULT_TIMER_INTERVAL
#define DEFAULT_TIMER_INTERVAL 250UL
#endif

#include <Arduino.h>
#include <limits.h>

long fixed_map( long, long, long, long, long);

#define GET_TIME_MILLIS millis()
#define GET_TIME_MILLIS_ABS millis()

class StepTimer {
  public:
    static unsigned long addOffsetWithWrap( unsigned long index, unsigned long offset, unsigned long maximum ) {
      return (offset > (maximum-index) ? offset-(maximum-index)-1 : index + offset);
    }

    static unsigned long addOffsetWithWrap( unsigned long index, unsigned long offset )
    {
      return addOffsetWithWrap( index, offset, ULONG_MAX );
    }
    
    static unsigned long subtractOffsetWithWrap( unsigned long index, unsigned long offset, unsigned long maximum ) {
     return (offset <= index) ? index-offset : maximum-( offset-index )+1;
    }
    
    StepTimer() {}
    StepTimer( unsigned long initInterval, boolean startImmediate ): StepTimer() { setInterval( initInterval ); if (startImmediate) { start(); }  }
    StepTimer( unsigned long initInterval ) : StepTimer( initInterval, true ) {}
    virtual ~StepTimer() {}

    void start();
    void start(unsigned long currMillis);
    virtual void onStart(unsigned long currMillis) {}

    void step();
    void step(unsigned long currMillis);
    virtual void onStep(unsigned long currMillis) {}

    void stop() { started=0; }
    bool isStarted() { return (started > 0); }
    bool isUp(unsigned long currMillis) { return ((started > 0) && (currMillis >= nextUpMillis)); }
    bool isUp() { return isUp(GET_TIME_MILLIS); }
    unsigned long nextUp() { return nextUpMillis; }
    unsigned long timeRemaining(unsigned long currMillis) { return (currMillis>=nextUpMillis ? 0 : nextUpMillis-currMillis); }
    unsigned long timeRemaining() { return timeRemaining(GET_TIME_MILLIS); }
    unsigned long getRollovers() { return rollovers; }                                                               // Number if times the timer has rolled over (crossed the 49 day mark...)
    unsigned long timeSinceStarted(unsigned long currMillis) { return ((started>0) ? (currMillis > started ? (currMillis - started) : 0) : 0); } 
    unsigned long timeSinceStarted() { return timeSinceStarted(GET_TIME_MILLIS); }  
    unsigned long timeSinceTriggered(unsigned long currMillis) { return ((started>0) ? currMillis-(getLastUp()) : 0 ); }
    unsigned long timeSinceTriggered() { return timeSinceTriggered( GET_TIME_MILLIS ); }  
    virtual unsigned long getLastUp() { return (nextUpMillis-interval); }
    virtual void setInterval(unsigned long newInterval);
    virtual void setIntervalImmediate( unsigned long newInterval );
    unsigned long getInterval() { return ( (pendInterval>0) ? pendInterval : interval); }

  protected:         
    unsigned long interval=DEFAULT_TIMER_INTERVAL;        // in milliseconds
    unsigned long started=0;                              // if started - this is the millis() instant the timer was last started
    unsigned long nextUpMillis=0;                         // while running - this is the next time the timer will become "up"
                                                          // Note the timer may be up for any arbitrary length of time before it is reset by calling step() 
    uint16_t rollovers = 0;                                                         
    unsigned long pendInterval = 0;
};


class FlexTimer : public StepTimer {
  public:
    static const unsigned long MIN_INTERVAL = 1;
    static const unsigned long MAX_INTERVAL = 5000U;
    static inline uint8_t intervalToSpeed( unsigned long intv, unsigned long rMin, unsigned long rMax ) { return 255-fixed_map( intv, rMin, rMax, 0, 255); }
    static inline unsigned long speedToInterval( uint8_t speed, unsigned long rMin, unsigned long rMax ) { return fixed_map( 255-speed, 0, 255, rMin, rMax ); }

    FlexTimer( unsigned long minInterval, unsigned long maxInterval, unsigned long initInterval, boolean initStart ) : StepTimer(initInterval, initStart) { setRange( minInterval, maxInterval ); setInterval(initInterval); }    
    FlexTimer( unsigned long initInterval, boolean initStart) : FlexTimer( MIN_INTERVAL, MAX_INTERVAL, (unsigned long)initInterval, initStart ) { }    
    FlexTimer( unsigned long initInterval ) : FlexTimer(initInterval, false) { }

    FlexTimer( unsigned long minInterval, unsigned long maxInterval, bool initStart, uint8_t initSpeed ) : FlexTimer( minInterval, maxInterval, speedToInterval(initSpeed, minInterval, maxInterval), initStart ) { }
    
    virtual void onStart(unsigned long currMillis) override;
    virtual void onStep(unsigned long currMillis) override;
    
    void setStartExpired( boolean newVal ) { startExpired = newVal; }
    boolean getStartExpired() { return startExpired; }
    unsigned long getRangeMin() { return rangeMin; }
    unsigned long getRangeMax() { return rangeMax; }
    void setRange( unsigned long minInterval, unsigned long maxInterval ); 
    virtual void setSpeed( uint8_t newSpeed ) { setInterval( speedToInterval( newSpeed, rangeMin, rangeMax ) ); }
    virtual uint8_t getSpeed() { return intervalToSpeed( getInterval(), rangeMin, rangeMax );  }
    void addDelta( long delta ) { currDelta = delta; nextUpMillis += currDelta; }
    virtual unsigned long getLastUp() override { return lastUpMillis; }
    virtual void setInterval( unsigned long newInterval ) override;
    unsigned long getCurrInterval() { return interval+currDelta; }
    unsigned long getSteps() { return stepCount; }    
  
  private:
    unsigned long rangeMin = MIN_INTERVAL;
    unsigned long rangeMax = MAX_INTERVAL; 
    unsigned long lastUpMillis=0;                         // if started - the instant the timer was last reset - by calling step()
    unsigned long stepCount = 0;                          // The number of times the trigger has been reset by calling step()
    unsigned long currDelta = 0;                          // Can adjust a step while it is occuring by setting a delta value to be added to the interval - delaying the triger by currDelta ms.
    boolean startExpired = false;                         // Set to true to start timer in the "Up" state - isUp() will return true immediately until step() is called
};  

#endif

#endif