//
//  FFXSegment.cpp
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#include "FFXSegment.h"
#include "FFXController.h"

bool FFXSegment::isPrimary() { return getTag()==PRIMARY_SEG_NAME; }
  
FFXSegment::FFXSegment( String initTag, uint16_t initStartIdx, uint16_t initEndIdx, FFXBase* initEffect, CRGB *initFrame, FFXController *parentController ) : FFXStateObserver() {
    tag = initTag;
    startIdx = initStartIdx;
    endIdx = initEndIdx;
    effect = initEffect;
    controller = parentController;
    localDimmer = ((tag==PRIMARY_SEG_NAME) ? new FFXAFDimmer(500) : nullptr );
    stateChanged = true;
    if (tag!=PRIMARY_SEG_NAME) {
      opacity = new FFXAFXFader( getLength() );
      opacity->setInterval(750);
      opacity->setTarget(0);
    }
    else {
      opacity = nullptr;
    }
    frameView = new FFXFrameProvider( this, initFrame );        
  }

  FFXSegment::~FFXSegment() {
    if (effect) { delete effect; }
    if (frameView) { delete frameView; }
    if (localDimmer) { delete localDimmer; }
    if (opacity) { delete opacity; }
  }

 void FFXSegment::onNotify(String source, String attribute, String value ) {
     if (attribute=="LOG") { 
       controller->onFXEvent( getTag(), FFXController::FXEventType::FX_LOG, value ); 
     }
     else if (attribute=="Interval") {
        frameView->checkCrossFade(effect);
     }
     else if (attribute=="Brightness" && source=="Primary") {
        if (localDimmer && offWithPrimary && !isPrimary()) {
          if (value == "0") {
            savedBrightness = getBrightness();
            this->setBrightness(0);
            forcedOff = true;
          }
          else {            
            forcedOff = false;
            this->setBrightness(savedBrightness);            
          }
        }
     }
     else {
       controller->onFXEvent( getTag(), FFXController::FXEventType::FX_PARAM_CHANGE, attribute );
       frameView->checkCrossFade(effect);
     }
     stateChanged = true;
  }  

void FFXSegment::setFX( FFXBase *newFX ) { 
    if (effect) {
      effect->stop();
      delete effect ;
      effect = nullptr;
    }
    if (newFX) {
      effect = newFX;      
      effect->addObserver(this); 
      effect->start();
      frameView->checkCrossFade(effect);
      effect->onBrightness(getActiveDimmer()->getValue());
      stateChanged = true;
    }
  }

  void FFXSegment::removeOverlay() {
    if (ovlFP) {
    delete ovlFP;
    ovlFP = nullptr;
    }
    if (overlay) {
      delete overlay;
      overlay = nullptr;
     }
    if (ovlLeds) {
      delete ovlLeds;
      ovlLeds = nullptr; 
    } 
  }


  void FFXSegment::setOverlay( FFXOverlay *newOvl ) {
  if (newOvl) {
    if (overlay) { 
      controller->onFXEvent( getTag(), FFXController::FX_OVERLAY_STOPPED, overlay->getFXName() ); 
      removeOverlay(); 
    }
    if (ovlLeds == nullptr) {
      ovlLeds = new CRGB[getLength()];
    }
    fill_solid( ovlLeds, getLength(), CRGB::Black );
    if (ovlFP == nullptr) { 
      ovlFP = new FFXFrameProvider(this, ovlLeds); 
    }
    overlay = newOvl;
    overlay->start();
    controller->onFXEvent(  getTag(), FFXController::FX_OVERLAY_STARTED, overlay->getFXName() );
  }
}

  bool FFXSegment::isVisible() {
    bool result = false;
    if (overlay) { 
      result = true; 
    }
    else if (effect) {
      result = (effect->isStarted() //&&
                //(this->isPrimary() ||
                //!(offWithPrimary && getController()->getPrimarySegment()->getCurrentBrightness()==0))
                );
    }    
    return result;
  } 

  void FFXSegment::setOpacity(uint8_t level) {
       if (opacity) {
         opacity->setTarget(level); 
         controller->onFXEvent( getTag(), FFXController::FXEventType::FX_OPACITY_CHANGED, "Segment:"+ (isPrimary() ? "Primary" : getTag()) );
         stateChanged = true;
     }
  }

  void FFXSegment::setBrightness( uint8_t newBrightness ) {
    if (forcedOff) {
        savedBrightness = newBrightness;
    }        
    else {
      if (!hasDimmer()) {
        localDimmer = new FFXAFDimmer(500, controller->getPrimarySegment()->getBrightness() );
        controller->onFXEvent( getTag(), FFXController::FXEventType::FX_LOCAL_BRIGHTNESS_ENABLED, "Segment:"+ (isPrimary() ? "Primary" : getTag()) );
      }
      localDimmer->setTarget(newBrightness);
      controller->onFXEvent( getTag(), FFXController::FXEventType::FX_BRIGHTNESS_CHANGED, "Segment:"+ (isPrimary() ? "Primary" : getTag()) );
      if (isPrimary()) { controller->notifySegments( false, "Primary", "Brightness", String(newBrightness)); }
      stateChanged = true;
    }
  }

  uint8_t FFXSegment::getBrightness() { 
    return getActiveDimmer()->getTarget();
  }

  uint8_t FFXSegment::getCurrentBrightness() {
    return getActiveDimmer()->getValue();
  }

  FFXAFDimmer *FFXSegment::getActiveDimmer() {
    if (localDimmer) {return localDimmer; } else {return controller->getPrimarySegment()->getActiveDimmer(); }
  }

  void FFXSegment::removeDimmer() { 
    if (!isPrimary() && localDimmer) {
      // set flag to remove dimmer when target is reached
      removeDimmerPending = true;
      // set the target to match the primary dimmer
      localDimmer->setTarget( controller->getPrimarySegment()->getActiveDimmer()->getTarget());
    }
  } 
 
  bool FFXSegment::isUpdated() {
    bool result = false;
    if (isVisible()) {
      if (overlay) {
        result = true;
      }
      else {
        result = (effect->isUpdated() || getActiveDimmer()->isUpdated());
      }
      if (opacity) { result = result || opacity->isUpdated(); }
    }
    return result;
  }

  void FFXSegment::updateOverlay( CRGB *frameBuffer ) {
      if (overlay) {
        // v1.1.1 Moved this check here so overlay will not be removed before last frame is drawn
        if (overlay->isDone()) { 
          controller->onFXEvent(  getTag(), FFXController::FX_OVERLAY_COMPLETED, overlay->getFXName()); 
          removeOverlay(); 
        }  
        else {
          ovlFP->updateFrame( ovlLeds, overlay );
          // controller->onFXEvent(  getTag(), FFXController::FX_OVERLAY_UPDATED, overlay->getFXName()); 
          overlay->applyOverlay( ovlLeds, &(frameBuffer[startIdx]));
        }
      }
  }

  void FFXSegment::updateFrame( CRGB *frameBuffer ) {
    if (isVisible()) { 
      if (getActiveDimmer()->isUpdated()) {
        effect->onBrightness(getCurrentBrightness());
        // If the dimmer is pending - to be removed, and target is reached...
        if (removeDimmerPending && (localDimmer->getValue()==localDimmer->getTarget())) {
          delete localDimmer; 
          localDimmer = nullptr;
          removeDimmerPending = false;
        }
      }
      getFrameProvider()->updateFrame( &(frameBuffer[startIdx]), effect );      
      CRGBSet pixels = CRGBSet(frameBuffer, startIdx, endIdx);
      getActiveDimmer()->update(pixels);
      if (opacity) {
          CRGBSet bkdest = CRGBSet(opacity->getBackgroundBuffer(),getLength());
          controller->getPrimarySegment()->getFrameProvider()->getLastFrame(bkdest.leds, startIdx, endIdx );
          controller->getPrimarySegment()->getActiveDimmer()->update(bkdest);    
          nblend( &(frameBuffer[startIdx]), opacity->getBackgroundBuffer(), getLength(), 255-opacity->getValue());
          opacity->update(pixels);
      }
    }
    else {
        if (opacity) { opacity->freeBackgroundBuffer(); }
    }
  }