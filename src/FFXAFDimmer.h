/*  
 *  Definition of FXAFDimmer class - used by FXSegment to provide dimming for individual ranges of pixels.  Incorporates auto-fade for smooth dimming 
 *  appearance.
 *  
 *  setTarget() to set desired dimming level (255 = no dimming/full brightness, 0 = off)
 *  getTarget() to return the set level (when fading - the level may not have been reached yet) 
 *  getValue()  to return the current level - when fading, this may not be equal to the target value
 *  apply( CRGBSet& ) applies the dimmer to the pixels passed in the CRGBSet reference
 *  isUpdated() return true when the dimming value is not the same as the last time apply() was called
 *  setInterval( uint16_t ms) to set the speed of fade transitions - will complete in ms milliseconds
*/

#ifndef FFX_AF_DIMMER_H
#define FFX_AF_DIMMER_H

#include "FFXAutoFader.h"

class FFXAFDimmer : public FFXAutoFader {
  public:
    FFXAFDimmer() : FFXAutoFader() { }
    FFXAFDimmer(unsigned int initInterval) : FFXAFDimmer() { setInterval(initInterval); }
    virtual ~FFXAFDimmer() { }
    virtual void onUpdate( CRGBSet &pixels ) override;    
};

#endif