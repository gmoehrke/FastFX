//
//  FFXSegment.h
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#ifndef FFX_SEGMENT_H
#define FFX_SEGMENT_H

#include "FFXBase.h"
#include "FFXFrameProvider.h"
#include "FFXAFDimmer.h"
#include "FFXAFXFader.h"

class FFXController;

/*
 *  FFXSegment - Class for housing multiple effects in a single pixel array.  Each segment contains a FXFrameProvider to handle
 *  updating the range of pixels it "contains" as well as both a local and global FX dimmer to manage brightness.
 * 
 *  An FFXController will always have at least 1 segment - the Primary segment.  This represents the entire array and is always considered to 
 *  be visible.  Subsequent segments are defined "over" this segment and may be 0-100% transparent.  Each non-primary segment has an opacity
 *  controller, which sets the level of opacity/transparency.  At opacity level of 0, the primary segment shows through 100%.  At opacity 255, 
 *  the segment shows at 100% and the underlying portion of the primary segment is not visible at all.  The primary segment has no opacity control
 *  and calls to `setOpacity()` do nothing, while calls to `getOpacity()` always return 255.
 * 
 *  Each segment may or may not have a brightness controller (dimmer).  The primary segment will always have a brighness controller.  Additional 
 *  segments will "inherit" the brightness of the primary segment until a call to setBrightness() overrides this behavior.  At that point, the 
 *  segment has its own brightness controller and will remain independent from the primary segment's brightness.  A subsequent call to removeDimmer()
 *  will remove the segments dimmer and revert back to using the brightness of the primary controller.
 *
*/
class FFXSegment : public FFXStateObserver {
public:
  FFXSegment( String initTag, uint16_t initStartIdx, uint16_t initEndIdx, FFXBase* initEffect, CRGB *initFrame, FFXController *parentController );
  virtual ~FFXSegment();
  
  virtual void onNotify(String source, String attribute, String value ) override;

  inline FFXBase *getFX() { return effect; }
  void setFX( FFXBase *newFX );
  FFXController *getController() { return controller; }
  bool isPrimary();
  inline uint16_t getStart() { return startIdx; }
  inline uint16_t getEnd() { return endIdx; }
  inline uint16_t getLength() { return endIdx-startIdx+1; }
  inline uint16_t getBufferSize() { return getLength()*sizeof(CRGB); }
  FFXFrameProvider *getFrameProvider() {     
    return frameView; 
  }
  bool isVisible();
  void setOpacity(uint8_t level);
  inline uint8_t getOpacity() { if (opacity) { return opacity->getTarget(); } else { return 255; } }
  inline uint8_t getCurrentOpacity() { if (opacity) { return opacity->getValue(); } else { return 255; } }
  inline FFXAFXFader *getOpacityObj() { return opacity; } 
  void setOpacityInterval( unsigned long newInterval ) { if (opacity) { opacity->setInterval(newInterval); } }

  bool isUpdated();
  void updateFrame( CRGB *frameBuffer );
  inline bool hasDimmer() { return (localDimmer!=nullptr);  }
  void removeDimmer();
  void setBrightness( uint8_t newBrightness );
  void setBrightnessInterval( unsigned long newInterval ) { 
    if (!localDimmer) {
      setBrightness(getActiveDimmer()->getTarget());
    }
    localDimmer->setInterval( newInterval );
  }
  FFXAFDimmer *getActiveDimmer();
  uint8_t getBrightness();
  uint8_t getCurrentBrightness();
  inline String getTag() { return tag; }
  inline void setTag( String newTag ) { tag = newTag; }
  inline boolean isStateChanged() { return stateChanged; }
  inline void resetStateChanged() { stateChanged = false; }

  boolean sameAs(FFXSegment &target) { return startIdx==target.getStart() && endIdx==target.getEnd(); }
  boolean compareTag(const String &comp) { return tag==comp; }
private:
    String tag;
    uint16_t startIdx = 0;
    uint16_t endIdx = 0;
    FFXBase *effect = nullptr;
    FFXFrameProvider *frameView = nullptr;
    FFXAFDimmer *localDimmer = nullptr;
    // when removing dimmer - set the target to the target of the primary dimmer then make remove "pending" until new target is reached.
    boolean removeDimmerPending = false;
    FFXController *controller = nullptr;
    FFXAFXFader *opacity = nullptr;
    boolean stateChanged = false;
    FFXSegment() {}
};

#endif