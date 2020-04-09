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
     else {
       controller->onFXEvent( getTag(), FFXController::FXEventType::FX_PARAM_CHANGE, attribute );
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
      effect->onBrightness(getActiveDimmer()->getValue());
      stateChanged = true;
    }
  }

  bool FFXSegment::isVisible() {
    bool result = false;
    if (effect) {
      result = (effect->isStarted());
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
    if (!hasDimmer()) {
      localDimmer = new FFXAFDimmer(500);
      controller->onFXEvent( getTag(), FFXController::FXEventType::FX_LOCAL_BRIGHTNESS_ENABLED, "Segment:"+ (isPrimary() ? "Primary" : getTag()) );
    }
    localDimmer->setTarget(newBrightness);
    controller->onFXEvent( getTag(), FFXController::FXEventType::FX_BRIGHTNESS_CHANGED, "Segment:"+ (isPrimary() ? "Primary" : getTag()) );
    stateChanged = true;
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

  bool FFXSegment::isUpdated() {
    bool result = false;
    if (isVisible()) {
      result = effect->isUpdated() || getActiveDimmer()->isUpdated();
      if (opacity) { result = result || opacity->isUpdated(); }
    }
    return result;
  }

  void FFXSegment::updateFrame( CRGB *frameBuffer ) {
    if (isVisible()) { 
      if (getActiveDimmer()->isUpdated()) {
        effect->onBrightness(getCurrentBrightness());
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