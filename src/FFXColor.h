#ifndef FFXCOLOR_H
#define FFXCOLOR_H

#include "FastLED.h"

/*  The FFXColor class provides a common interface for dealing with color, color changes, and palettes in effects.
 *
 *  The FFXBase parent class for all effects has a member named currColor which is available to all descendant classes for
 *  getting color values.  The FXColor class supports 4 modes - 2 single color modes and 2 palette modes for effects that use
 *  multiple colors.
 *
 *  Each effect simply needs to call the getCRGB function to return the currently selected color.  This class provides 2 methods
 *  for stepping through palette colors and shifting the palette for "rotating/shifting" palette effects.  This class operates
 *  differently depending on the mode selected (use setColorMode() to change the mode) as follows:
 *
 *  mode singleCRGB - always return the same color, specified by setCRGB
 *  mode singleCHSV - always return the same color (CHSV) as specified by setCHSV
 *  mode palette16 - Returns colors from a 16 entry palette (set with setPalette() )
 *                     step()  - steps forward 1 entry in the palette.  So, if I have a 16 entry palette - each time
 *                               step() is called, the next entry will be returned by getCRGB().  Once the last entry is
 *                               returned, the next call to step() starts over at the first entry.
 *                     shift() - will shift each entry forward n steps where n is the value of shiftDelta
 *                               (defaults to 1).  Each palette entry returned using a 0-255 index, so as
 *                               shift() is called, each entry moves closer to the next entry in the palette.  Once
 *                               we've shifted forward 16 times, the entire palette has shifted forward 1 step.
 *                     setRange() - This will allow the palette16 to work the same with fewer than 16 entries.
 *
 *                     resetStep() - sets the step position back to the first entry in the palette.
 *                     resetShift() - Shifts the entire palette back to its original position.
 *
 * mode palette256 - Works almost the same way as palette16, however calls to step() only move forward 1 position.  Shift will
 *                   still shift the palette forward each time it is called as well.  This can be used to rotate a palette through a
 *                   fixed "window" without needing to track the index ranges.  For example - I can display entries 1-100 in a loop and
 *                   call shift after each one.  This will continually rotate the entire palette through those first 100 entries.
 *
 */
class FFXColor {

  public:
    enum FFXColorMode { singleCRGB=0, singleCHSV=1, palette16=2, palette256=3 };
    String mode_name[4] = { "singleCRGB", "singleCHSV", "palette16", "palette256" };
    //static uint8_t addWithWrap( uint8_t base, uint8_t delta, uint8_t range);

    FFXColor();
    FFXColor(const FFXColor &src);

    FFXColorMode getColorMode() { return currColorMode; }
    String getColorModeName() { return String(mode_name[currColorMode]); }
    void setColorMode(FFXColorMode const newMode);

    uint8_t getPaletteRange() { return pRange; }
    uint8_t setPaletteRange(uint8_t newRange) {
      pRange = (newRange > 16 ? 16 : (newRange=0 ? 1 : newRange));
      changed = true;
      return pRange;
    }

    uint8_t getShiftDelta() { return shiftDelta; }
    uint8_t setShiftDelta( uint8_t newDelta ) { return shiftDelta = newDelta; }

    uint8_t getStepDelta() { return stepDelta; }
    uint8_t setStepDelta( uint8_t newDelta ) { stepDelta = newDelta; return stepDelta; }

    uint8_t scaleIndex( uint8_t index );

    /* The CRGB attribute is used for both singleCRGB mode and the palette modes - in the palette based color modes - getCRGB returns the palette entry 
       based on the current step, which is cycled to the next entry each time step() is called (or shifted in the palette when shift() is called) 

       Reading either the CRGB or HSV values by calling getCRGB() or getHSV() will reset the isUpdated() flag.  To get the value of either without 
       affecting the isUpdated() flag, use either peekCRGB() or peekHSV() methods.  
    */
    CRGB peekCRGB(uint8_t index);
    CRGB peekCRGB() { return peekCRGB(currPindex); }    
    CRGB getCRGB(uint8_t index);
    CRGB getCRGB() { return getCRGB(currPindex); }
    void setCRGB(CRGB const &newCRGB) { changed = true; currCRGB = newCRGB; }

    CHSV peekCHSV() { return currHSV; }
    CHSV getCHSV() { return currHSV; changed = false;}
    void setCHSV(CHSV newHSV) { changed = true; currHSV = newHSV; }

    CRGBPalette16& getPalette() { return currPalette; }
    void setPalette(CRGBPalette16 const newPalette, uint8_t range);
    void setPalette(CRGBPalette16 const newPalette) { setPalette(newPalette, 16); }
    // void setRange( uint8_t newRange );
    // uint8_t getRange() { return pRange; }

    uint8_t relativePaletteIndex(uint8_t index);
    uint8_t relativePaletteIndex() { return relativePaletteIndex( currPindex ); }
    uint8_t getIndex() { return currPindex; }
    uint8_t getPaletteIndex() { return scaleIndex(currPindex); }
    void setPaletteIndex( uint8_t newIndex ) { 
      if (currPindex != newIndex) {
        currPindex = newIndex; 
        changed = true;
      }
    }
    uint8_t getPaletteOffset() {return currPoffset; }

    void resetStep() { currPindex = 0; }
    void resetShift() { currPoffset = 0; }
    void reset() { resetStep(); resetShift(); }

    boolean isUpdated() { return changed; }
    void setUpdated(boolean newValue) { changed = newValue; }

    void step(uint8_t steps);
    void step() { step(stepDelta); }
    void shift(uint8_t steps);
    void shift() { shift( shiftDelta ); }

  protected:
    FFXColorMode currColorMode = FFXColorMode::singleCRGB;
    CRGB currCRGB = CRGB::Black;
    CHSV currHSV = CHSV(0,255,255);
    CRGBPalette16 currPalette;
    uint8_t pRange = 16;
    uint8_t currPindex = 0;
    uint8_t currPoffset = 0;
    uint8_t stepDelta = 1;
    uint8_t shiftDelta = 0;
    bool changed = false;
};

#endif
