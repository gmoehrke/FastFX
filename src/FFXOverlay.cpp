//
//  FFXOverlay.cpp
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#include "FFXOverlay.h"

void FFXOverlay::applyOverlay( CRGB* overlay, CRGB* leds ) {
  for (uint16_t i=0; i<numLeds; i++) {
     if (overlay[i]!=CRGB(0,0,0)) {               // Black is treated as 100% transparent
       if (alpha[i] != 0) {                       // Otherwise use alpha blend if != 0
         //nblend(leds[i], overlay[i], alpha[i]);
         uint8_t a = alpha[i] > getMaxAlpha() ? getMaxAlpha() : alpha[i]; 
         leds[i] = CRGB( alphaBlend( leds[i].r, overlay[i].r, a ),
                         alphaBlend( leds[i].g, overlay[i].g, a ),
                         alphaBlend( leds[i].b, overlay[i].b, a ) );

       }
     }
  }
}

void FFXOverlay::whileFrozen() {
   if ( GET_TIME_MILLIS >= nextCycleStart ) {
     unFreeze();
   }
}

void FFXOverlay::onVCycleEnd() {
   if (currVCycle>=repeat && !continuous) {
     completed = true;
   }
   else {
     nextCycleStart = GET_TIME_MILLIS + repeatDelayms;
     freeze();
   }
}

void FFXOverlay::moveAintoB( byte *a, byte *b,  uint16_t size_a, uint16_t size_b, uint16_t end_position, size_t elementsize ) {
  if (size_a < size_b) {
   if (end_position <= size_a) {
        memmove8( &b[0], &a[(size_a-end_position)*elementsize], end_position*elementsize );
      }
      else if (end_position > size_b) {
        if (end_position-size_a <= size_b) {
          memmove8( &b[(end_position-size_a-1)*elementsize], &a[0], (size_a-(end_position-size_b)+1)*elementsize);
        }
      }
      else {
        memmove8( &b[(end_position-size_a-1)*elementsize], &a[0], size_a*elementsize );
        }
   }
   else if (size_a==size_b) { memmove8( &b[0], &a[0], size_b*elementsize ); }
  }
