//
//  FFXBase.cpp
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#include "FFXBase.h"


const uint8_t PROGMEM gamma8[] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 }; 


FFXBase::FFXBase( uint16_t initSize, unsigned long initInterval, unsigned long minRefresh, unsigned long maxRefresh ) : FFXStateNotifier(), FlexTimer(minRefresh, maxRefresh, (unsigned long)initInterval, false )  {
  initialize( initSize );
}

FFXBase::FFXBase( uint16_t initSize ) : FlexTimer( 10, true ) {
   initialize( initSize );
}

void FFXBase::initialize( uint16_t initSize ) {
  numLeds = initSize;
   vCycleRange = initSize;
   setStartExpired(true);  // Start immediately - Timer starts in triggered state, so initLeds, then writeNextFrame is called on first call to update() 
}

String FFXBase::movementTypeStr( MovementType mvt ) {
  switch(mvt) {
    case MVT_FORWARD : { return "forward"; break; }
    case MVT_BACKWARD : { return "backward"; break; }
    case MVT_BACKFORTH : { return "back and forth"; break; }
    case MVT_RANDOM : { return "random"; break; }
    case MVT_STILL : { return "still"; break; }
    default : {return "unknown"; break; }
  }
}

uint16_t FFXBase::getMovementPhase() {
  switch (currMovement) {
    case MVT_FORWARD : { return currPhase; break; }
    case MVT_BACKWARD : { return mirror(currPhase-1)+1; break; }
    case MVT_BACKFORTH : { return ( ((currCycle & 1)==1) ? mirror(currPhase-1)+1 : currPhase); break; }
    case MVT_RANDOM : { return 1+random16(numLeds-1); break; }
    case MVT_STILL : { return 1; break; }
    default : {return currPhase; break; }
  }
}

String  FFXBase::fadeMethodStr( FadeType value ) { 
    switch (value) {
      case GAMMA : { return "gamma"; break; }
      case CUBIC : { return "cubic"; break; }    
      case LINEAR : { return "linear"; break; }  
      default : { return "unknown"; break; }
    }
   }

CRGB FFXBase::alphaBlend( CRGB &a, CRGB &b, uint8_t alpha, FadeType ftUp, FadeType ftDown ) {
      uint8_t aLuma = a.getLuma();
      uint8_t bLuma = b.getLuma();
      if (aLuma > bLuma) 
      { 
        switch (ftUp) {
          case GAMMA : { alpha = pgm_read_byte(&gamma8[alpha]); break; }
          case CUBIC : { alpha = ease8InOutApprox(alpha); break; } 
        }
      } 
      else if (bLuma > aLuma) {
        switch (ftDown) {
          case GAMMA : { alpha = pgm_read_byte(&gamma8[alpha]); break; }
          case CUBIC : { alpha = ease8InOutApprox(alpha); break; } 
        }
      }
      return CRGB( alphaBlend( a.r, b.r, alpha ), 
                   alphaBlend( a.g, b.g, alpha ),
                   alphaBlend( a.b, b.b, alpha ) );
   }   

void FFXBase::rotateBufferForwardWithWrap( CRGB *source, CRGB *dest, uint8_t numLeds, uint8_t steps ) {
     if ( (steps > 0) && (steps < numLeds)) {
       bool buffertail = (steps<(numLeds/2));
       uint8_t buffersize = buffertail ? steps : numLeds-steps;
       CRGB* tempsrc = new CRGB[buffersize];
       if (buffertail) {
          //  Move the tail end of the array into temp buffer
          memmove8( &tempsrc[0], &source[numLeds-steps], buffersize*sizeof(CRGB) );
          //  Move the front end of source array into the tail of the dest array
          memmove8( &dest[steps], &source[0], (numLeds-buffersize)*sizeof(CRGB) );
          //  Move the temp buffer into the front end of the dest array
          memmove8( &dest[0], &tempsrc[0], buffersize*sizeof(CRGB) );
       }
       else
       {
          // Move the front end of the array into temp buffer
          memmove8( &tempsrc[0], &source[0], buffersize * sizeof(CRGB) );
          // Move the tail end of the source array into front of the dest array
          memmove8( &dest[0], &source[numLeds-steps], (numLeds-buffersize) * sizeof(CRGB) );
          // Move the temp buffer into tail end of the dest array
          memmove8( &dest[steps], &tempsrc[0], buffersize * sizeof(CRGB) );
       }
       delete tempsrc;
     }
   }


void FFXBase::setColor( CRGB newColor )
{
   currColor.setColorMode(FFXColor::FFXColorMode::singleCRGB);
   currColor.setCRGB(newColor);
}

void FFXBase::update(CRGB *frameBuffer ) {
      if (!frozen) {

        if (!initialized) {
            initLeds(frameBuffer);
            initialized = true;
         }
        if (currPhase == 1) {
           cycleStart();
        }
        if (currVPhase == 1) {
           vCycleStart();
        }
        if (currColor.isUpdated()) { changed = true; }
        writeNextFrame( frameBuffer );
        uint16_t next = getNextPhase();
        if ( next==1 ) {
          cycleEnd();
        }
        uint16_t vNext = getNextVPhase();
        if ( vNext==1 ) {
          vCycleEnd();
        }
        currPhase = next;
        currVPhase = vNext;
      }
      else { whileFrozen(); }
      if ( timeSinceStarted() > ((secondsElapsed+1)*1000) ) {
        onEachSecond(++secondsElapsed);
      }
      step();
   }


  
