//
//  FFXFFrameProvider.cpp
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#include "FFXFrameProvider.h"
#include "FFXSegment.h"
#include "FFXController.h"

void FFXFrameProvider::allocateBuffer(CRGB **buffer) {
   if (*buffer) { deallocateBuffer(buffer); }
   *buffer = (CRGB *)malloc(segment->getBufferSize());
 }

void FFXFrameProvider::deallocateBuffer(CRGB **buffer) {
   if (*buffer) {
     free( *buffer);
     *buffer = nullptr;
   }
}

FFXFrameProvider::FFXFrameProvider( FFXSegment *initSegment, CRGB *fdLEDBuffer, CRGB *initFrame ) {
  segment = initSegment;
  if (fdLEDBuffer) {
    currFrameBuffer = fdLEDBuffer;
    crossFade = true;
  }
  else if (segment) {
    crossFade = true;
    allocateBuffer(&currFrameBuffer);
    if (initFrame==nullptr) { fill_solid(currFrameBuffer, segment->getLength(), CRGB::Black); }
  }
  if (initFrame) {
    memmove8(currFrameBuffer, initFrame, segment->getBufferSize() );
  }
}

void FFXFrameProvider::setCrossFade( boolean newValue ) {
  if (newValue != crossFade) {
    if (newValue) {
       allocateBuffer(&nextFrameBuffer);
       memmove8( nextFrameBuffer, currFrameBuffer, segment->getBufferSize());
       crossFade = true;
       //segment->getController()->onFXEvent( FXController::FXEventType::FX, "Segment:"+ (isPrimary() ? "Primary" : getTag()) );
    }
   else {
     if (nextFrameBuffer) { memmove8( currFrameBuffer, nextFrameBuffer, segment->getBufferSize() ); }
     deallocateBuffer(&nextFrameBuffer);
     crossFade = false;
   }
   if (segment) {
     segment->onNotify( segment->getTag(), "FrameProvider:Crossfade", String(crossFade) );
   }
  }
}

void FFXFrameProvider::checkCrossFade( FFXBase *effect ) {
  if (effect->getInterval() <= fadeThresholdms) {
    setCrossFade(false);    
  }
  else {
    setCrossFade(getCrossFadePref());
  }
}

void FFXFrameProvider::updateFrame( CRGB *destLEDs, FFXBase* effect ) {
  if (effect->isUp() || (effect->timeRemaining() <= fadeThresholdms)) {    
    if (effect->isUp()  || !nextFrameBuffer ) {
      priorBlendAmt = 0;
      blendSteps = 0;
      step( effect );
      memmove8( destLEDs, currFrameBuffer, segment->getBufferSize() ); 
    }
    else {
      // Not enough time to draw a blended frame - so draw a frame...until the timer expires to begin the next transition
      if (blendSteps>0) {
        // if a blended frame has already been drawn - then resolve to the next frame
        memmove8( destLEDs, nextFrameBuffer, segment->getBufferSize() ); 
      }
      else {
        // if nothing has been blended, keep drawning the current frame until the timer expires
        memmove8( destLEDs, currFrameBuffer, segment->getBufferSize() ); 
      }
      priorBlendAmt = 255;
    }
  }
  else {
    // if crossFading and the nextFrameBuffer is allocated...
    if ((crossFade && nextFrameBuffer && effect->isUpdated())) {
        // Calculate the crossfade blend to the next frame
        // Account for changes to updateInterval - don't revert back to frames with 
        // lower blend value (in cases where the interval is increased), just wait for it to catch up...
        uint8_t blendAmt = maximum<uint8_t>(fixed_map(effect->timeSinceTriggered(), 1, effect->getCurrInterval(), 1, 255), priorBlendAmt);
        // Blend between current frame and next frame
        FFXBase::alphaBlend( currFrameBuffer, nextFrameBuffer, destLEDs, segment->getLength(), blendAmt, fadeMethodUp, fadeMethodDown );
        blendSteps += 1;
        priorBlendAmt = blendAmt;
    }
    else if ((currFrameBuffer != destLEDs) && destLEDs) {
      memmove8(destLEDs, currFrameBuffer, segment->getBufferSize() );
    }
  }
}

void FFXFrameProvider::getLastFrame(CRGB *destLEDs, uint16_t startIdx, uint16_t endIdx ) {
  if (!crossFade && currFrameBuffer) {
    memmove8( destLEDs, &(currFrameBuffer[startIdx]), sizeof(CRGB)*(endIdx-startIdx+1) );
  }
  else {
    if ((priorBlendAmt == 0)||(!nextFrameBuffer)&&currFrameBuffer) {
      memmove8( destLEDs, &(currFrameBuffer[startIdx]), sizeof(CRGB)*(endIdx-startIdx+1) );
    }
    else if ((priorBlendAmt == 255)&&(nextFrameBuffer)) {
      memmove8( destLEDs, &(nextFrameBuffer[startIdx]), sizeof(CRGB)*(endIdx-startIdx+1) );
    }
    else if (currFrameBuffer && nextFrameBuffer) {
      FFXBase::alphaBlend( &(currFrameBuffer[startIdx]), &(nextFrameBuffer[startIdx]), destLEDs, (endIdx-startIdx+1), priorBlendAmt, fadeMethodUp, fadeMethodDown );
    }
  }
}

void FFXFrameProvider::step( FFXBase* effect ) {
  if (crossFade) {
      if (nextFrameBuffer) {
        memmove8(currFrameBuffer,nextFrameBuffer, segment->getBufferSize());
        }
      else {
        allocateBuffer(&nextFrameBuffer);
    }
    effect->update( nextFrameBuffer );
  }
  else {
    effect->update( currFrameBuffer );
  }
}
