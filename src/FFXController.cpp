//
//  FFXController.cpp
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#include "FFXController.h"
#include "FFXCoreEffects.h"

FFXController::FFXController( FFXPixelController *initSC ) {
  initialize( initSC );
}

FFXController::FFXController() {
}

void FFXController::initialize( FFXPixelController *initPC ) {
  if (ledController) {
    delete ledController;
  }
  for (auto seg : segments) { delete seg; }
  segments.clear();
  ledController = initPC;
  ledController->setBrightness(255);
  numLeds = ledController->getNumLeds();
  liveLeds = ledController->getLeds();
  addSegment( PRIMARY_SEG_NAME, 0, numLeds-1, nullptr );
  minRefreshTimer.start();
}

void FFXController::setFX( FFXBase *newFX ) {
  if (newFX) {
   getPrimarySegment()->setFX(newFX);
  }
}

void FFXController::setOverlayFX( FFXOverlay *newFX ) {
  getPrimarySegment()->setOverlay( newFX );
}

FFXSegment *FFXController::addSegment(String initTag, uint16_t initStartIdx, uint16_t initEndIdx, FFXBase* initEffect ) {
  FFXSegment *result = nullptr;
  auto it = std::find_if( segments.begin(), segments.end(), [&initTag](FFXSegment*& element) -> bool { return element->compareTag(initTag); } );
  if (it!=segments.end()) {
    result = *it;
  }
  else {
    if (!initEffect) { initEffect = new SolidFX( initEndIdx-initStartIdx+1 ); }
    result = new FFXSegment( initTag, initStartIdx, initEndIdx, initEffect, liveLeds, this );
    segments.push_back( result );
    if (result->getFX()) {
      result->getFX()->start();
    }
  }
  return result;
}

FFXSegment *FFXController::findSegment(String &tag) {
    auto it = std::find_if( segments.begin(), segments.end(), [&tag](FFXSegment*& element) -> bool { return element->compareTag(tag); } ); 
    if (it==segments.end()) { 
      return getPrimarySegment(); 
    }
    else { 
      return *it; 
    } 
}


void FFXController::show() {
      if (centerOffset > 0) {
        FFXBase::rotateBufferForwardWithWrap( liveLeds, liveLeds, numLeds, centerOffset );
      }
      //if (ovlFX) { ovlFX->applyOverlay( ovlLeds, liveLeds ); }
      if (centerOffset > 0) {
        FFXBase::rotateBufferBackwardWithWrap( liveLeds, liveLeds, numLeds, centerOffset );
      }
      ledController->show();
}

void FFXController::update() {
    bool redraw = false;
    for (auto seg : segments) {
        seg->updateFrame( liveLeds );
        if (seg->isStateChanged()) { this->onFXStateChange(seg); seg->resetStateChanged(); }
    }
    for (auto seg : segments ) {
      seg->updateOverlay( liveLeds );
      if (seg->isUpdated()) { redraw=true; }
    }
    // v1.1.1 - add timer to force refresh at specified interval 
    if (minRefreshTimer.isUp()) {
      redraw = true;
      minRefreshTimer.step();
    }
    if (redraw) {
      show();
      showCount++;
    }
}
