/*
 *   Definition of FXController.  
 *   
 *   FXController - is the primary class used for displaying/running effects/colors/etc.  Initialize with a PixelController
 *   like: new FXController( new FastLEDPixelController( ledBufferPtr, numberOfLEDs ) );
 * 
 *   Effects/Colors are placed into FXSegments which are managed by the FXController.  Every controller has a "Primary" segment, which 
 *   represents the entire strip/array.  This segment is always present and does not need to be created manually.
 * 
 *   Additional (overlay) segments are added "on top" of the primary segment by calling AddSegment(...).  Each of these overlay 
 *   segments, can have its own effect running and has independent opacity/transparency and brightness.  Each additional segment
 *   is referenced by a unique "Tag" or Name, which is used to address and manipulate the segment.
 * 
 *   Displaying and changing effects (Primary Segment):
 * 
 *       FX = new RainbowFX( numberOfLEDs, updateInterval );
 *       FXController->getPrimarySegment()->setFX( FX ); 
 *               
 *   Displaying and changing effects (Additional Segments)
 *   
 *      FXController->AddSegment( "Left", 0, 25, null )
 *      FXSegment *seg = FXController->findSegment("Left");
 *      if (seg) {
 *        seg->setFX( new RainbowFX(seg->getLength()));
 *        set->setOpacity(255);
 *      }
 * 
 *      ** This will show the rotating rainbow effect on pixels 0-25 and the remainder of the strip will continue to show the primary effect.
 * 
 *   inside main processing loop:
 *   
 *       FFXController.update();
 * 
 *   Each FFXSegment automatically supports cross-fading by using an independent FXFrameProvider.  The Frame Provider does this by allocating extra buffers and using cycles
 *   between refreshes to fade from one frame to the next. Turn this on and off using FXController->getPrimarySegment()->getFrameProvider()->setFade( boolean ) or 
 *   FXController->findSegment("name")->getFrameProvider()->setFade( boolean )
 *
 *   Overlay FX are effects that can be displayed over the top of existing/running effects (the entire array), such as flashes, pulses, 
 *   etc.  These are also cross-faded and blended with the underlying FX as desired.  Both the overlay and the underlying
 *   effect are cross-faded and continue to animate while displaying.  These are done as follows:
 *   
 *   FFXController.setFX( FX );
 *   OFX = new PulseOveralyFX( numberOfLEDs, speed );
 *   OFX.getFXColor().setPalette( myPal );
 *   FFXController.setOverlayFX( OFX );
 *   
 *   Note that FFXController() automatically deletes the overlay FX, when it is complete so it MUST be created using - new OverlayFX(...);
 *   See FXOverlay.h for more info.
 */
#ifndef FFX_CONTROLLER_H
#define FFX_CONTROLLER_H

#include "FlexTimer.h"
#include "FFXColor.h"
#include "FFXPixelController.h"
#include "FFXBase.h"
#include "FFXOverlay.h"
#include "FFXFrameProvider.h"
#include "FFXAFDimmer.h"
#include "FFXSegment.h"

#define PRIMARY_SEG_NAME "FXController::PrimarySegmentName"

class FFXController {

  public:
    unsigned long long showCount = 0;
    enum FXEventType { FX_STARTED, FX_STOPPED, FX_OVERLAY_STARTED, FX_OVERLAY_STOPPED, FX_OVERLAY_COMPLETED, FX_PAUSED, FX_RESUMED, FX_BRIGHTNESS_CHANGED, FX_LOCAL_BRIGHTNESS_ENABLED, FX_OPACITY_CHANGED, FX_PARAM_CHANGE, FX_LOG };
    FFXController( FFXPixelController *initPC );
    FFXController();
    ~FFXController() { 
      if (ledController) { delete ledController; } 
      if (ovlFX) { delete ovlFX; }
      if (segments.size() > 0) {
        for (auto seg : segments) {
          delete seg;
        }
        segments.clear();
      }
    }

    void initialize( FFXPixelController *initPC );

    virtual void onFXEvent( const String &segment, FXEventType event, const String &name ) { };
    virtual void onFXStateChange(FFXSegment *segment) {};
    void setFX( FFXBase *newFX );
    FFXBase *getFX() { return getPrimarySegment()->getFX(); }

    FFXSegment *addSegment(String initTag, uint16_t initStartIdx, uint16_t initEndIdx, FFXBase* initEffect );
    FFXSegment *addSegment(String initTag, uint16_t initStartIdx, uint16_t initEndIdx) { return addSegment(initTag, initStartIdx, initEndIdx, nullptr ); }
    FFXSegment *findSegment(String &tag);
    FFXSegment *getPrimarySegment() { return segments[0]; }
  
    FFXPixelController *getStripController() { return ledController; }
    unsigned long getUpdateMillis() { return (getFX()==nullptr) ? 250 : getFX()->getInterval(); }

    void setOverlayFX( FFXOverlay *newFX );
    void removeOverlayFX();
    FFXOverlay *getOverlayFX() { return ovlFX; }
    
    String getFXName() { return (getFX() == nullptr ? "None" : getFX()->getFXName()); }
    uint8_t getBrightness() { return getPrimarySegment()->getBrightness(); }
    void setBrightness( uint8_t newBrightness ) { getPrimarySegment()->setBrightness(newBrightness); }

    void show();
    void update();

  private:
     boolean initialized = false;                       
     uint16_t centerOffset = 0;
     std::vector<FFXSegment *> segments = std::vector<FFXSegment *>();
     FFXFrameProvider *ovlFrameView =nullptr;

  protected:
    FFXPixelController *ledController = nullptr;
    FFXOverlay *ovlFX = nullptr;
    CRGB *liveLeds = nullptr;
    CRGB *ovlLeds = nullptr;
    uint16_t numLeds;    
 };

#endif
