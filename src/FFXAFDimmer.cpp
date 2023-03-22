//
//  FFXAFDimmer.cpp
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#include "FFXAFDimmer.h"

void FFXAFDimmer::onUpdate( CRGBSet &pixels ) {
  if (getValue() < 255) {
      pixels.fadeToBlackBy(255-getValue());
  }
}