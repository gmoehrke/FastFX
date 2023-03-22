//
//  FFXBase.h
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#ifndef FFX_BASE_H
#define FFX_BASE_H

#include "FFXColor.h"
#include "FlexTimer.h"

/*!
 *   FFXStateObserver - Base class for FFX event observer class.  Descendents of this class may
 *   be registered with any FFXStateNotifier.  Override the pure virtual onNotify() 
 *   method to handle notification events.
*/
class FFXStateObserver {
  public:
    FFXStateObserver() {}
    virtual ~FFXStateObserver() { }
    virtual void onNotify( String source, String attribute, String value ) = 0;
};

/*!
 *   FFXStateNotifier - Base class for FFX event publisher class.  Maintains a list of 
 *   FFXStageObserver objects to send notifications.  Calls to notify() will be passed
 *   to the onNotify() method of each registered FFXStateObjerver.
*/
class FFXStateNotifier {
  public:
    FFXStateNotifier() { }
    virtual ~FFXStateNotifier() { 
      observers.clear();
    }
 
    void addObserver( FFXStateObserver *obs ) {
      if (obs) { 
        observers.push_back( obs );
      }
    }
    void notify( String source, String attribute, String value) {
      for (auto o : observers) { o->onNotify( source, attribute, value ); }
    }

  private:
    std::vector<FFXStateObserver*> observers = std::vector<FFXStateObserver*>();
};

/*!
 *   FFXBase - Base class for all LED Strip animation effects.
 *
 *     All static and animated effects should inherit from this class.  The 2 key functions to override/implement in descendant
 *     classes are:
 * 
 *        initLeds( CRGB *buffer )
 *        writeNextFrame( CRGB *buffer )
 * 
 *     Most effects can be written by simply overriding one or both of the above methods (see FXCoreEffects.h for examples).
 * 
 *     FFXBase is a descendent of the FlexTimer class which allows for setting the speed/timing of the animation.  FXBase also 
 *     inherits from FXStateNotifier, which provides a means for sending state notification changes to any descendant of the 
 *     FXStateObserver class (defined here as well).  FXSegment, which is the "container" for all FXBase objects, is a descendent
 *     of FXStateObserver, so changes to timing or other state info can be pushed back to the "parent" segment.
 * 
 *     Phase/Cycle - Each effect has a phase and a cycle, the cycle is one complete iteration from 1 to numLeds - the phase being
 *        the current index of that iteration.  
 *
 *     VPhase/VCycle - We can also define a "virtual" phase and cycle which allows have a cycle that is larger or smaller than the number
 *        of LEDs on a strip.  A larger VCycle is useful if we want to do additional things at either end of a cycle (i.e. where an animation
 *        will scroll off the end of the strip without wrapping around immediately.  A smaller VCycle can be used if an animation needs fewer 
 *        phases to complete (ex: quickly switching between to colors, jiggling back and forth, etc.)  A VCycle is established by calling 
 *        setVCycleRange()
 *
 *     MovementType is defined here, then used by descendant classes to determine the type of movement for a given effect.
 *
 *     FFXBase also provides several "utility" methods for performing common operations used in animated effects:
 *     
 *     Rotate pixels within a CRGB Buffer, wrapping around at the ends:
 *       rotateBufferForwardwithWrap( ... )
 *       rotateBufferBackwardWithWrap( ... )
 *     
 *     Calculate the "mirror" image position of a given pixel (i.e. same distance from center, on the other side):
 * 
 *        mirror( uint16_t position )
 * 
 *     Several virtual methods are provided, which can be overriden in descendent classes 
 * 
 *       onEachSecond(unsigned long) // called for each elapsed second while effect is running (udpate() called within a loop)
 *       onCycleStart()
 *       onCycleEnd()
 *       onVCycleStart()
 *       onVCycleEnd()
 *       onBrightness()   // called when brightness is changed (by FXXSegment)
 *       whileFrozen()    // called each time update() is called while effect is "Frozen"/paused 
 */
class FFXBase : public FFXStateNotifier, public FlexTimer {

public:
   enum MovementType { MVT_FORWARD=1, MVT_BACKWARD=2, MVT_BACKFORTH=3, MVT_RANDOM=4, MVT_STILL=5 };   
   enum FadeType { LINEAR=1, GAMMA=2, CUBIC=3 };
   
   FFXBase( uint16_t initSize, unsigned long initInterval, unsigned long minRefresh, unsigned long maxRefresh );
   FFXBase( uint16_t initSize, unsigned long initInterval ): FFXBase( initSize, initInterval, MIN_INTERVAL, MAX_INTERVAL ) {}
   FFXBase( uint16_t initSize, uint8_t initSpeed, unsigned long minRefresh, unsigned long maxRefresh) : FFXBase( initSize, FlexTimer::speedToInterval(initSpeed, minRefresh, maxRefresh), minRefresh, maxRefresh ) { setSpeed(initSpeed); }
   FFXBase( uint16_t initSize, uint8_t initSpeed ) : FFXBase( initSize, initSpeed, MIN_INTERVAL, MAX_INTERVAL ) {}
   FFXBase( uint16_t initSize );

   void initialize( uint16_t initSize ); 

   static String movementTypeStr( MovementType mvt );
   static String fadeMethodStr( FadeType value );
   
   String getFXName() { return fxName; }
   uint8_t getFXID() { return fxid; }

   virtual void setInterval( unsigned long newInterval ) override {
     FlexTimer::setInterval(newInterval);
     notify( getFXName(), "Interval", String(newInterval));
   }

   virtual void setColor( CRGB newColor );

   MovementType getMovement() { return currMovement; }
   
   /*! Returns the current movement resulting from the MovementType setting - only differs from getMovement when using 
       back and forth movement - this will return the current direction*/
   FFXBase::MovementType getCurrMovement(uint16_t cycle);

   void setMovement( MovementType newMovement ) { 
     currMovement = newMovement;
     notify( getFXName(), "Movement", movementTypeStr(newMovement) ); 
    }

   FFXColor &getFXColor() { return currColor; }
   CRGB getColor() { return currColor.getCRGB(); }
   void setFXColor( FFXColor &newColor) { 
     currColor = newColor;
     notify( getFXName(), "Color", currColor.getColorModeName()); 
   }

   static uint8_t inline alphaBlend( uint8_t a, uint8_t b, uint8_t alpha ) { return scale8(a, 255-alpha) + scale8(b, alpha); }
   static CRGB alphaBlend( CRGB &a, CRGB &b, uint8_t alpha, FadeType ftUp = LINEAR, FadeType ftDown = LINEAR );
      
   static void alphaBlend( CRGB *a, CRGB *b, CRGB *dest, uint16_t num, uint8_t alpha, FadeType ftUp = LINEAR, FadeType ftDown = LINEAR ) {
    for (uint16_t i=0; i<num; i++ ) {
      dest[i] = alphaBlend( a[i], b[i], alpha ); 
    }
   }
   
   static uint16_t mirror( uint16_t index, uint16_t range ) {
     return (index > range) ? 0 : range-1-index;
   }
   uint16_t mirror( uint16_t index ) {
     return mirror( index, numLeds );
   }

   static void rotateBufferForwardWithWrap( CRGB *source, CRGB *dest, uint8_t numLeds, uint8_t steps );
   static void rotateBufferBackwardWithWrap( CRGB *source, CRGB *dest, uint16_t numLeds, uint16_t steps ) {
     rotateBufferForwardWithWrap( source, dest, numLeds, numLeds-steps );
   }

   void rotateForward( CRGB *bufLeds, uint16_t steps ) {
     rotateBufferForwardWithWrap( bufLeds, bufLeds, numLeds, steps );
   }

   void rotateBackward( CRGB *bufLeds, uint16_t steps ) {
     rotateBufferBackwardWithWrap( bufLeds, bufLeds, numLeds, steps );
   }

   uint16_t addOffsetWithWrap( uint16_t index, uint16_t offset ) {
      return StepTimer::addOffsetWithWrap( (unsigned long)index, (unsigned long)offset, (unsigned long)numLeds );
     }
   
   uint16_t inline subtractOffsetWithWrap( uint16_t index, uint16_t offset ) {
    return StepTimer::subtractOffsetWithWrap( index,  offset, numLeds );
   }
   
   uint16_t getNextPhase() {
    return ((currPhase+1 <= numLeds) ? currPhase+1 : 1);
   }

   uint16_t getNextVPhase() {
    return ((currVPhase+1 <= vCycleRange) ? currVPhase+1 : 1 );
   }

   uint16_t getPrevPhase() {
     return ((currPhase==1) ? numLeds : currPhase-1);
   }

   uint16_t getPrevVphase() {
     return ((currVPhase==1) ? vCycleRange : currVPhase-1);
   }

   uint16_t getCurrPhase(){ return currPhase; }
   uint16_t getCurrVPhase() { return currVPhase; }
   unsigned long getCurrCycle() { return currCycle; }
   unsigned long getCurrVCycle() { return currVCycle; }

   uint16_t getMovementPhase(); 
   uint16_t getMovementVPhase();

   uint16_t getNumLeds() { return numLeds; }
   uint16_t getVCycleRange() { return vCycleRange; }
   uint16_t setVCycleRange( uint16_t newRange ) { 
     return vCycleRange = newRange; 
   }

   // timingDelta can be overriden by descendant effects to adjust the time for the current frame (of phase) - can return a positive of negative 
   virtual int timingDelta(uint8_t phase, unsigned long sourceMillis ) { return 0; }
   
   int timingDelta( unsigned long sourceMillis ) { return timingDelta( currPhase, sourceMillis ); }

   virtual void onEachSecond( unsigned long secsRunning ) { }
   virtual void onCycleStart( CRGB *currFrame ) { }
   virtual void onCycleEnd( CRGB *currFrame ) { }
   virtual void whileFrozen( CRGB *currFrame ) { }
   virtual void onVCycleStart( CRGB *currFrame ) { }
   virtual void onVCycleEnd( CRGB *currFrame ) { }
   virtual void onBrightness( uint8_t newBrightness ) { }
   
   // initLeds is called once before the first call to writeNextFrame.  This can be overridden to do any additional initialization, clear the background, set colors, etc.
   // Note that LEDs are not updated/shown between the call to initLeds and the first call to writeNextFrame - this is simply a hook that gets called once to setup anything
   // needed by writeNextFrame().
   virtual void initLeds(CRGB *bufLeds) {}

   // writeNextFrame is the workhorse of the descendant class - this is called each time a new frame is to be drawn.  frameBuffer is the LED buffer 
   // that contains the current frame data and into which the next frame is written.
   virtual void writeNextFrame(CRGB *frameBuffer) = 0;

   void vCycleStart( CRGB *currFrame ) { onVCycleStart( currFrame );  }
   void vCycleEnd( CRGB *currFrame ) { onVCycleEnd( currFrame ); currVCycle += 1;  }

   void cycleStart( CRGB *currFrame ) { onCycleStart( currFrame ); }
   void cycleEnd( CRGB *currFrame ) { onCycleEnd( currFrame ); currCycle += 1;  }

   void freeze() { frozen = true; }
   void unFreeze() { frozen = false; }
   bool isFrozen() { return frozen; }

   void update(CRGB *frameBuffer );

   boolean isUpdated() { if (currColor.isUpdated()) { changed=true; } return changed;  }
   void setUpdated(boolean newValue) { changed = newValue; }

  
  protected:
    uint8_t fxid = 0;
    String fxName = "";
    FFXColor currColor = FFXColor();
    uint16_t numLeds;    
    MovementType currMovement = MVT_FORWARD;
    uint16_t currPhase = 1;
    uint16_t currVPhase = 1;
    unsigned long currCycle = 1;
    unsigned long currVCycle = 1;
    uint16_t vCycleRange;
    bool changed = true;
    bool initialized = false;
    unsigned long defaultFrameRefresh = 50;
    unsigned long secondsElapsed = 0;
    bool frozen = false;  
};

#endif
