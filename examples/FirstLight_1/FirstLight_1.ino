
#define FASTLED_INTERRUPT_RETRY_COUNT 1
#include "FastLED.h"
#include "FastFX.h"

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
      getFXColor().setColorMode( FFXColor::FFXColorMode::singleCRGB );
      getFXColor().setCRGB( CRGB::White );
    }

    virtual void initLeds( CRGB *bufLeds ) override {
      fill_solid( bufLeds, numLeds, CRGB::Black );
    }
    virtual void writeNextFrame( CRGB *bufLeds ) override {
      fadeToBlackBy( bufLeds, numLeds, 50 );
      bufLeds[getMovementPhase()-1] = currColor.getCRGB();
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
    // fxctrlr.getPrimarySegment()->getFX()->setSpeed(245);
    // fxctrlr.getPrimarySegment()->getFrameView()->setCrossFade(false);
}


void loop() {
  fxctrlr.update();
}
