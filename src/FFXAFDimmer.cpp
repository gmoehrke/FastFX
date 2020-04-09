
#include "FFXAFDimmer.h"

void FFXAFDimmer::onUpdate( CRGBSet &pixels ) {
  if (getValue() < 255) {
      pixels.fadeToBlackBy(255-getValue());
  }
}