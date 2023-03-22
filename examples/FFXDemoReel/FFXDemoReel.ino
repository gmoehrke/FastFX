
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


// This function sets up the leds and tells the controller about them
// Then creates and adds the effect for the primary segment.
void setup() {
  	// sanity check delay - allows reprogramming if accidently blowing power w/leds
   	delay(1000);
     
    pinMode( 5, OUTPUT );
    FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear();

    fxctrlr.initialize( new FFXFastLEDPixelController( leds, NUM_LEDS ) );
    fxctrlr.getPrimarySegment()->setFX( new CycleFX( NUM_LEDS ) );
    fxctrlr.getPrimarySegment()->setBrightness( 100 );   
}

StepTimer fxTimer = StepTimer( 10000, true );

void flash( const CRGBPalette16 &color ) {
  FFXOverlay *newFX = new PulseOverlayFX( fxctrlr.getPrimarySegment()->getLength(), 220, 1, color );
  fxctrlr.setOverlayFX( newFX );
}

void stepFX() {
  FFXBase *newFX = nullptr;
  CRGBPalette16 txcolor;
  switch (fxctrlr.getPrimarySegment()->getFX()->getFXID()) {
    case CYCLE_FX_ID   : { newFX = new RainbowFX( fxctrlr.getPrimarySegment()->getLength()); txcolor = NamedPalettes::getInstance()["red"]; break; }
    case RAINBOW_FX_ID : { newFX = new CylonFX( fxctrlr.getPrimarySegment()->getLength());   txcolor = NamedPalettes::getInstance()["yellow"]; break; }
    case CYLON_FX_ID   : { newFX = new JuggleFX( fxctrlr.getPrimarySegment()->getLength());  txcolor = NamedPalettes::getInstance()["blue"]; break; }
    case JUGGLE_FX_ID  : { newFX = new MotionFX( fxctrlr.getPrimarySegment()->getLength());  txcolor = NamedPalettes::getInstance()["green"]; break; }
    case MOTION_FX_ID  : { newFX = new ChaseFX( fxctrlr.getPrimarySegment()->getLength());   txcolor = NamedPalettes::getInstance()["orange"]; break; }
    case CHASE_FX_ID   : { newFX = new PaletteFX( fxctrlr.getPrimarySegment()->getLength()); txcolor = NamedPalettes::getInstance()["green"]; break; }
    case PALETTE_FX_ID : { newFX = new CycleFX( fxctrlr.getPrimarySegment()->getLength());   txcolor = NamedPalettes::getInstance()["blue"]; break; }
  }
  if (newFX) { fxctrlr.getPrimarySegment()->setFX( newFX ); }
  flash( txcolor );
}

void loop() {
  fxctrlr.update();
  if (fxTimer.isUp()) {
    stepFX();
    fxTimer.step();
  }
}
