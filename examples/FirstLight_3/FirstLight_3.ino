
#define FASTLED_INTERRUPT_RETRY_COUNT 1
#include "FastLED.h"
#include "FlexTimer.h"
#include "FastFX.h"
#include "FFXCoreEffects.h"

// How many leds are in the strip?
#define NUM_LEDS 100

// Data pin that led data will be written out over
#define DATA_PIN 5

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];
FFXController fxctrlr = FFXController();

// Create the custom effect class - override the writeNextFrame method to create the animation
class FirstLightFX : public FFXBase {
  public:    
    FirstLightFX(uint16_t initSize) : FFXBase( initSize,/* interval */   20UL, 
                                                        /* min interval */ 10UL, 
                                                        /* max interval */ 100UL ) {
      currColor.setColorMode( FFXColor::FFXColorMode::singleCRGB );
      currColor.setCRGB( CRGB::White );
    }

    virtual void initLeds( CRGB *bufLeds ) override {
      fill_solid( bufLeds, numLeds, CRGB::Black );
    }
     virtual void writeNextFrame( CRGB *bufLeds ) override {
      fadeToBlackBy( bufLeds, numLeds, 50 );
      bufLeds[getMovementPhase()-1] = currColor.getCRGB();
      switch (currColor.getColorMode()) {
        // Blend the moving pixel through the entire palette range
        case FFXColor::FFXColorMode::palette256 : { currColor.step(); break; }
        // Step through the active colors in the palette - switching at either end of the strip
        case FFXColor::FFXColorMode::palette16 : { if (getCurrPhase()==0 || getCurrPhase()==numLeds) { currColor.step(); } break; }
        default:  { }
      }
    }
};

// This function sets up the leds and tells the controller about them
// Then creates and adds the effect for the primary segment.
void setup() {
  	// sanity check delay - allows reprogramming if accidently blowing power w/leds
   	delay(1000);
     
    pinMode( 5, OUTPUT );
    FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear();

    fxctrlr.initialize( new FFXFastLEDPixelController( leds, NUM_LEDS ) );
    fxctrlr.getPrimarySegment()->setFX( new FirstLightFX( NUM_LEDS ) );
    fxctrlr.getPrimarySegment()->setBrightness( 100 );
    // fxctrlr.getPrimarySegment()->getFrameView()->setCrossFade(false);
    // fxctrlr.getPrimarySegment()->getFX()->setInterval( 30 );
    fxctrlr.getPrimarySegment()->getFX()->setMovement( FFXBase::MovementType::MVT_BACKFORTH );
    // fxctrlr.getPrimarySegment()->getFX()->getFXColor().setColorMode( FFXColor::FFXColorMode::palette16 );
    // fxctrlr.getPrimarySegment()->getFX()->getFXColor().setPalette( NamedPalettes::getInstance()["multi"] );
    // fxctrlr.getPrimarySegment()->getFX()->getFXColor().setPaletteRange( 6 );
    fxctrlr.getPrimarySegment()->getFX()->getFXColor().setColorMode( FFXColor::FFXColorMode::palette256 );
    fxctrlr.getPrimarySegment()->getFX()->getFXColor().setPalette( NamedPalettes::getInstance()["party"] );

    // Setup segments...
    FFXSegment *seg;
    seg = fxctrlr.addSegment( "Left", 0, 32 );
    seg->setFX( new FirstLightFX( seg->getLength() ) );
    seg->setBrightness(255);
    seg->getFX()->getFXColor().setCRGB( CRGB::Red );
    seg->getFX()->setMovement( FFXBase::MovementType::MVT_FORWARD );
    seg->setOpacity(192);
    
    seg = fxctrlr.addSegment( "Center", 33, 66 );
    seg->setFX( new FirstLightFX( seg->getLength() ) );
    seg->setBrightness(255);
    seg->getFX()->getFXColor().setCRGB( CRGB::Blue );
    seg->getFX()->setMovement( FFXBase::MovementType::MVT_BACKFORTH );
    seg->setOpacity(192);
    
    seg = fxctrlr.addSegment( "Right", 67, 99 );
    seg->setFX( new FirstLightFX( seg->getLength() ) );
    seg->setBrightness(255);
    seg->getFX()->getFXColor().setCRGB( CRGB::Green );
    seg->getFX()->setMovement( FFXBase::MovementType::MVT_BACKWARD );
    seg->setOpacity(192);
  }


void loop() {
  fxctrlr.update();
}
