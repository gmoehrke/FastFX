//
//  FFXRotate.cpp
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#include "FFXRotate.h"

void FFXRotate::setColor( CRGB newColor ) 
  {
    FFXBase::setColor( newColor );
    redrawFull = true;
  }


bool FFXRotate::rotate(CRGB *bufLeds, uint16_t steps ) {
    if (getMovement()==MVT_FORWARD || getMovement()==MVT_BACKWARD || getMovement()== MVT_BACKFORTH) {
        if (getMovement()!=MVT_BACKWARD || ((getMovement()==MVT_BACKFORTH) && ((currCycle % 2)==1))) {
          rotateForward( bufLeds, steps );
         } 
         else {
          rotateBackward( bufLeds, steps );
        }
      return( true );
    }
    else {
      return( false );
    }
  }

  void FFXRotate::writeNextFrame(CRGB *bufLeds) {
      if (redrawFull) {
        fillLeds(bufLeds, currPhase);
        setUpdated( true );
        redrawFull = false;
      }
      else {
        if (rotate(bufLeds, 1 ) ) {
          setUpdated( true );
      }
    }
  }