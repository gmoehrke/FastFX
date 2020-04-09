
#ifndef FFX_AF_X_FADER_H
#define FFX_AF_X_FADER_H

#include "FFXAutoFader.h"

class FFXAFXFader: public FFXAutoFader {
  public:
    FFXAFXFader(uint16_t pixels) : FFXAutoFader() { size = pixels; }
    virtual ~FFXAFXFader() { if (background) { free( background); } }
    virtual void onUpdate( CRGBSet &pixels ) override; 
    CRGB *getBackgroundBuffer() { if (!background) { background = (CRGB *)malloc(sizeof(CRGB)*size); } return background; } 
    void freeBackgroundBuffer() { if (background) { free( background ); background = nullptr; } }
    private:
      uint16_t size;
      CRGB* background = nullptr;   
};

#endif