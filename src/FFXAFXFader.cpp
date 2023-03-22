#include "FFXAFXFader.h"

void FFXAFXFader::onUpdate( CRGBSet &pixels ) {
  if (getValue() < 255)  {
    pixels = pixels.nblend( CRGBSet(background, size), 255-getValue() );
  }
  if (getValue()==255 && !isFading() ) {
    freeBackgroundBuffer();
  }
}

