
#include "FFXPixelController.h"   

FFXPixelController::FFXPixelController( CRGB *initLeds, uint16_t initNum ) {
  leds = initLeds;
  numLeds = initNum;
}

void FFXPixelController::setBrightness(uint8_t newBrightness) {
  if (newBrightness != currBrightness) {
    currBrightness = newBrightness;
    updateBrightness( newBrightness );
    show();
  }
}
