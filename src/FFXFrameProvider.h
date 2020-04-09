/*
 * The FFXFrameProvider controls the fetching of frames that get displayed by the FXController.  There are several "modes" of operation
 * that can be utilized:
 *
 *   Direct mode - (Crossfade=false AND constructed with FASTLed framebuffer passed in the constructor)
 *
 *       This is the most efficient usage.  It uses a single buffer (CRGB[]) that is passed in the constructor.
 *       This is typically the array of CRGB used by fastLED for display and allocates no additional heap.
 *
 *   Indirect modes - Indirect modes use independent buffer(s) where pixel data is written and maintained.  This means that each frame is calculated independently from
 *   what is in the FastLED display buffer.  This can be useful for overlaying or mixing effects.
 *
 *       without crossfade - Uses a single buffer, remains independent of any changes made to the fastLED display buffer.
 *
 *       with crossfade - Uses 2 buffers to smooth animated effects.  A second buffer is used to "smooth" transitions from frame to frame - uses multiple 
 *       cycles to blend from the current frame to the next one.  The number of blend steps between frames depends on the available time between frames and 
 *       the FrameViewController will fill as many steps as it can in that time.  
 */

#ifndef FFX_FRAME_PROVIDER_H
#define FFX_FRAME_PROVIDER_H

#include "FFXBase.h"

template<typename T>
inline T maximum( T a, T b ) { return a > b ? a : b ; }

template<typename T>
inline T maximum( T a, T b, T c ) { return maximum( maximum(a,b), c ) ; }

template<typename T>
inline T minimum( T a, T b ) { return a < b ? a : b ; }

template<typename T>
inline T minimum( T a, T b, T c ) { return minimum( minimum(a,b), c ) ; }

class FFXSegment;

class FFXFrameProvider {

  public:
    FFXFrameProvider( FFXSegment *initSegment, CRGB *fdLEDBuffer, CRGB *initFrame );
    FFXFrameProvider( FFXSegment *initSegment, CRGB *initFrame ) : FFXFrameProvider(initSegment,  nullptr, initFrame ) { };
    FFXFrameProvider( FFXSegment *initSegment ) : FFXFrameProvider( initSegment,  nullptr ) { };

    ~FFXFrameProvider() {
      if (currFrameBuffer) { deallocateBuffer( &currFrameBuffer ); } 
      if (nextFrameBuffer) { deallocateBuffer( &nextFrameBuffer ); }
    }

    CRGB *getCurrentFrame() { return currFrameBuffer; }

    bool getCrossFade() { return crossFade; }
    void setCrossFade( boolean newValue );
    FFXBase::FadeType getFadeMethodUp() { return fadeMethodUp; }    
    FFXBase::FadeType getFadeMethodDown() { return fadeMethodDown; }
    void setFadeMethod( FFXBase::FadeType newValueUp, FFXBase::FadeType newValueDown ) { 
      if (newValueDown != fadeMethodDown) fadeMethodDown = newValueDown; 
      if (newValueUp != fadeMethodUp) fadeMethodUp = newValueUp;
    }
    uint8_t getLastBlendSteps() { return blendSteps; }
    uint8_t getLastBlendAmount() { return priorBlendAmt; }
    void updateFrame( CRGB *destLEDs, FFXBase* effect );
    void updateFrame( FFXBase* effect ) { updateFrame( currFrameBuffer, effect ); }
    void getLastFrame(CRGB *destLEDs, uint16_t startIdx, uint16_t endIdx);
   
   protected:
    void step( FFXBase* effect );    
    CRGB *getNextFrameBuffer() { return nextFrameBuffer; }     
    CRGB *getCurrentFrameBuffer() { return currFrameBuffer; }
  
  private:
    FFXSegment *segment;
    CRGB *currFrameBuffer=nullptr;            // buffer containing the current frame - during crossfading this keeps the original frame
    CRGB *nextFrameBuffer=nullptr;            // buffer containing the next frame - this is the frame we are cross-fading to
    bool crossFade = true;
    unsigned long fadeThresholdms = 3;        // Minimum number of ms remaining between cycle steps where there is still time to draw a cross faded frame
    FFXBase::FadeType fadeMethodUp = FFXBase::FadeType::LINEAR;
    FFXBase::FadeType fadeMethodDown = FFXBase::FadeType::LINEAR;
    uint8_t priorBlendAmt = 0;             // keep track of the last crossfade blend amount used...when the interval is increased mid-frame, we don't want to jump back to an "earlier" frame.
    uint16_t blendSteps = 0;
    void allocateBuffer(CRGB **buffer);
    void deallocateBuffer(CRGB **buffer);
};

#endif