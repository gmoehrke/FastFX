//
//  FFXColor.cpp
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#include "FFXColor.h"
#include "FlexTimer.h"

FFXColor::FFXColor() {
  currPalette = RainbowColors_p;
  currColorMode = FFXColorMode::singleCRGB;
  changed = true;
  pRange = 16;
  currPindex = 0;
  currPoffset = 0;
  stepDelta = 1;
  shiftDelta = 0;
}

FFXColor::FFXColor(const FFXColor &src) {
  currColorMode = src.currColorMode;
  currCRGB = src.currCRGB;
  currHSV = src.currHSV;
  currPalette = src.currPalette;
  pRange = src.pRange;
  currPindex = src.currPindex;
  currPoffset = src.currPoffset;
  stepDelta = src.stepDelta;
  shiftDelta = src.shiftDelta;
  changed = src.changed;
}

void FFXColor::setColorMode(FFXColorMode const newMode) {
  if (newMode != currColorMode) {
    currColorMode=newMode;
    changed = true;
  }
}


uint8_t FFXColor::scaleIndex( uint8_t index ) {
  if (index==0) { return 0; }
  else {
    switch (currColorMode) {
      case FFXColorMode::palette16 : { return (index==0 ? 0 : (index*16)); break; }
      default : { return index; }
      }
  }
}

uint8_t FFXColor::relativePaletteIndex(uint8_t index) {
  return StepTimer::addOffsetWithWrap(scaleIndex(index), currPoffset, (uint8_t)((16*(uint16_t)pRange)-1));
}

CRGB FFXColor::peekCRGB(uint8_t index) {
  CRGB result;
  switch (currColorMode) {
    case FFXColorMode::singleCRGB : { result = currCRGB; break; }
    case FFXColorMode::singleCHSV : { result = CRGB( currHSV ); break; }
    default : { result = ColorFromPalette( currPalette, relativePaletteIndex() ); }
  }
  return result;
}

CRGB FFXColor::getCRGB(uint8_t index) {
  changed = false;
  return peekCRGB(index);
}

void FFXColor::setPalette(CRGBPalette16 const newPalette, uint8_t range) {
  changed = true;
  currPalette = newPalette;
  setPaletteRange(range);
}

/*void FFXColor::setRange( uint8_t newRange ) {
  if (pRange != newRange) {
    pRange = newRange;
    changed = true;
  }
}*/

void FFXColor::step(uint8_t steps) {
  switch (currColorMode) {
    case FFXColorMode::singleCHSV : { break; }
    case FFXColorMode::singleCRGB : { break; }
    case FFXColorMode::palette16 : { currPindex = (StepTimer::addOffsetWithWrap(currPindex, steps, pRange-1)); break; }
    case FFXColorMode::palette256: { currPindex = (StepTimer::addOffsetWithWrap(currPindex, steps, 255)); break; }
  }
  //changed = true;
}


void FFXColor::shift(uint8_t steps) {
  if (steps > 0) {
    switch (currColorMode) {
      case FFXColorMode::singleCHSV : { break; }
      case FFXColorMode::singleCRGB : { break; }
      case FFXColorMode::palette16 : { currPoffset = StepTimer::addOffsetWithWrap( currPoffset, steps, (uint8_t)(uint16_t)(16*pRange)-1 ); break; }
      case FFXColorMode::palette256 : { currPoffset = StepTimer::addOffsetWithWrap( currPoffset, steps, 255 ); break; }
    }
  //changed = true;
  }
}
