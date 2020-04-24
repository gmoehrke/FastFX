//
//  FFXCoreEffects.h
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
/*
    Definition of basic set of FFX Effects.  These are primarily examples of what can be done.  The only effect defined here that is required by the 
    framework itself is the SolidFX class, which is used as a default effect for newly created segments.

*/
#ifndef FFX_CORE_FX_H
#define FFX_CORE_FX_H

#include "FFXRotate.h"
#include "FFXTrigMotion.h"
#include "FFXOverlay.h"
#include "FFXFrameProvider.h"
#include "FFXController.h"

#define SOLID_FX_ID 1
#define SOLID_FX_NAME "Solid"

#define CHASE_FX_ID 2
#define CHASE_FX_NAME "Chase"

#define MOTION_FX_ID 3
#define MOTION_FX_NAME "Motion"

#define RAINBOW_FX_ID 4
#define RAINBOW_FX_NAME "Rainbow"

#define JUGGLE_FX_ID 5
#define JUGGLE_FX_NAME "Juggle"

#define CYLON_FX_ID 6
#define CYLON_FX_NAME "Cylon"

#define CYCLE_FX_ID 7
#define CYCLE_FX_NAME "Cycle"

#define TWINKLE_FX_ID 8
#define TWINKLE_FX_NAME "Twinkle"

#define DIM_PAL_FX_ID 9
#define DIM_PAL_FX_NAME "DimPalette"

#define PALETTE_FX_ID 11
#define PALETTE_FX_NAME "Palette"

#define PACIFICA_FX_ID 10
#define PACIFICA_FX_NAME "Pacifica"

#define WAVE_OVLY_FX_NAME "Wave"
#define PULSE_OVLY_FX_NAME "Pulse"

const CRGBPalette16 Multi_p = CRGBPalette16( CRGB::Red,CRGB::Blue,CRGB::DarkOrange,CRGB::Green,
                                             CRGB(255,25,0), CRGB::Purple, CRGB(50,100,255), CRGB::Red,
                                             CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black,
                                             CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Black );
const uint8_t Multi_size = 7;                

const CRGBPalette16 pacifica_palette_1 = 
      { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
        0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
const CRGBPalette16 pacifica_palette_2 = 
      { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
        0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
const CRGBPalette16 pacifica_palette_3 = 
      { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33, 
        0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };                      

DECLARE_GRADIENT_PALETTE( blue_wave_gp );
DECLARE_GRADIENT_PALETTE( red_wave_gp );
DECLARE_GRADIENT_PALETTE( yellow_wave_gp );
DECLARE_GRADIENT_PALETTE( green_wave_gp );
DECLARE_GRADIENT_PALETTE( orange_wave_gp );
DECLARE_GRADIENT_PALETTE( soft_white_dim_gp ); 

/*!  NamedPalettes - Singleton class that allows access to pre-defined palettes by name:
 *   
 *   ```
 *   CRGBPalette16 myPal = NamedPalettes::getInstance()["green"]
 *   ```
 */
class NamedPalettes {
  public: 
  static NamedPalettes &getInstance() {
      static NamedPalettes instance;
      return instance; 
  }
    
  const CRGBPalette16 operator[](String index);
  String operator[](CRGBPalette16 index);
  void addNamedPalette( String name, CRGBPalette16 pal ) {
    plist.push_back( std::make_pair( name, pal ) );
  }
  private:
    std::vector<std::pair<String, CRGBPalette16>> plist;
    NamedPalettes() { 
      plist = std::vector<std::pair<String, CRGBPalette16>>();
      addNamedPalette( String("multi"), Multi_p );
      addNamedPalette( String("red"), red_wave_gp );
      addNamedPalette( String("yellow"), yellow_wave_gp );
      addNamedPalette( String("blue"), blue_wave_gp );
      addNamedPalette( String("green"), green_wave_gp );
      addNamedPalette( String("orange"), orange_wave_gp );
      addNamedPalette( String("softwhite_scale"), soft_white_dim_gp );   
      addNamedPalette( String("ocean"), OceanColors_p );
      addNamedPalette( String("cloud"), CloudColors_p );
      addNamedPalette( String("forest"), ForestColors_p );
      addNamedPalette( String("lava"), LavaColors_p );
      addNamedPalette( String("heat"), HeatColors_p );   
      addNamedPalette( String("party"), PartyColors_p );
    };
    ~NamedPalettes() {    
      plist.clear();
    }
    NamedPalettes(const NamedPalettes&) {}
    NamedPalettes& operator=(const NamedPalettes&) { return *this; }
};

/*!
 * SolidFX - Base effect for static, solid colors.
 */
class SolidFX : public FFXBase {
  public:
    SolidFX(  uint16_t initSize ) : FFXBase( initSize, (uint8_t)255, 100, 1000 ) {
      fxid = SOLID_FX_ID;
      fxName = SOLID_FX_NAME;
      currColor.setColorMode( FFXColor::FFXColorMode::singleCRGB );
    }

   virtual void initLeds( CRGB *bufLeds ) override {
    // Force the first frame to draw on first call to writeNextFrame - new frame won't be drawn unless color is changed after that...
    currColor.setUpdated(true);
   }

  virtual void writeNextFrame( CRGB *bufLeds ) override {
    if (currColor.isUpdated()) {
      fill_solid( bufLeds, numLeds, getColor() );
      setUpdated(true);
    } 
    else {
      setUpdated(false);
    }
  }
  
};

/*!
 * PaletteFX - Base effect 'rotating' palette'.  Supports both 16 and 256 color modes and 
 * MVT_FORWARD or MVT_BACKWARD motion.
 */
class PaletteFX : public FFXBase {
  public:
     PaletteFX( uint8_t initSize ) : FFXBase( initSize, (uint8_t)200, 10, 1000 ) {
       fxid = PALETTE_FX_ID;
       fxName = PALETTE_FX_NAME;
       currColor.setColorMode( FFXColor::FFXColorMode::palette256);
       currColor.setPalette( Multi_p );
     }

  virtual void writeNextFrame( CRGB *bufLeds ) {
    uint16_t currPhase = getCurrPhase();
    if (currColor.isUpdated()) {
      for (uint16_t i = 0; i<numLeds; i++) {
        if (currColor.getColorMode()==FFXColor::FFXColorMode::palette256) {
          switch (getMovement()) {
            case FFXBase::MVT_FORWARD : { bufLeds[i] = ColorFromPalette( currColor.getPalette(), fixed_map(StepTimer::subtractOffsetWithWrap(currPhase,i,numLeds), 1, numLeds, 0, 255), 255, LINEARBLEND ); break; }
            case FFXBase::MVT_BACKWARD: { bufLeds[i] = ColorFromPalette( currColor.getPalette(), fixed_map(StepTimer::addOffsetWithWrap(currPhase,i,numLeds), 1, numLeds, 0, 255), 255, LINEARBLEND ); break; }
            default: { bufLeds[i] =  ColorFromPalette( currColor.getPalette(), fixed_map(i,  0, numLeds-1, 0, 255), 255, LINEARBLEND ); break; }
          }
        }
        if (currColor.getColorMode()==FFXColor::FFXColorMode::palette16) {
          switch (getMovement()) {
            case FFXBase::MVT_FORWARD : { bufLeds[i] = currColor.getPalette()[(uint8_t)(fixed_map(StepTimer::addOffsetWithWrap(currPhase,i,numLeds), 1, numLeds, 0, currColor.getPaletteRange()-1))]; break; }
            case FFXBase::MVT_BACKWARD: { bufLeds[i] = currColor.getPalette()[(uint8_t)(fixed_map(StepTimer::subtractOffsetWithWrap(currPhase,i,numLeds), 1, numLeds, 0, currColor.getPaletteRange()-1))]; break; }
            default: { bufLeds[i] =  currColor.getPalette()[(uint8_t)(fixed_map( i, 0, numLeds-1, 0, currColor.getPaletteRange()-1))]; break; }
          }
        }
      } 
    }   
    else {
      setUpdated(false);
    }
  }

};

/*!
 * ChaseFX - Theater Chase effect.  Sopports various settings to control the number, spacing, 
 * movement and color of the "dots".
 */
class ChaseFX : public FFXRotate {

  private:
    uint8_t dotWidth = 1;
    uint8_t dotSpacing = 2;
    uint8_t blurAmount = 2;
    CRGB fieldColor = CRGB::Black;
    boolean hueShift = false;
    uint16_t shiftDelay = 3;

  public:
    ChaseFX( uint8_t initSize, uint8_t initSpeed ) : FFXRotate( initSize, initSpeed, 10UL, 2000UL ) {
      fxid = CHASE_FX_ID;
      fxName = CHASE_FX_NAME;
      currColor.setColorMode( FFXColor::FFXColorMode::palette16 );
      currColor.setPalette( ::Multi_p, 6 );
      currColor.setStepDelta( 1 );
      currColor.setShiftDelta(0);
      currColor.reset();
    }
    ChaseFX( uint8_t initSize ) : ChaseFX( initSize, 128 ){}

    uint8_t getDotWidth() { return dotWidth; }
    void setDotWidth( uint8_t newWidth ) {
      if ((newWidth != dotWidth) && (newWidth <= numLeds/2)) {
        dotWidth = newWidth;
        redrawFull = true;
      }
    }

    uint8_t getDotSpacing() { return dotSpacing; }
    void setDotSpacing( uint8_t newSpace ) {
      if (newSpace != dotSpacing)  {
        dotSpacing = newSpace;
        redrawFull = true;
      }
    }

    uint8_t getBlurAmount() { return blurAmount; }
    void setBlurAmount( uint8_t newBlur ) {
      if (newBlur != blurAmount) {
        blurAmount = newBlur;
        redrawFull = true;
      }
    }
    
    virtual void initLeds(CRGB *bufLeds) override {      
      if (hueShift) { currColor.setShiftDelta(16); }
    }

    virtual void fillLeds(CRGB *bufLeds, uint16_t phase ) override {
        currColor.resetStep();
        if (dotSpacing>0) { fill_solid( bufLeds, numLeds, fieldColor ); }
        uint16_t i = 0;
        CPixelView<CRGB> pixels = CPixelView<CRGB>( bufLeds, numLeds );
        while (i <= numLeds-1-(dotSpacing==0 ? 0 : 1)) {
          uint8_t space = (i==1 ? 0 : dotSpacing);
          //uint16_t strt = (i==1 ? dotSpacing : i);
          pixels( i+space, minimum(i+space+(dotWidth-1),numLeds-1) ) = getColor();
          i = i+space+(dotWidth-1)+1;
          if (((currColor.getColorMode() == FFXColor::FFXColorMode::palette16)) || (currColor.getColorMode() == FFXColor::FFXColorMode::palette256)) {
            currColor.step();
          }  
        }
        rotate( bufLeds, phase-1 );
        if (blurAmount > 0) { blur1d( bufLeds, numLeds, blurAmount ); }
        if (hueShift) {
          if ((currColor.getShiftDelta() > 0)&&getCurrVPhase()==1) { currColor.shift(); }
        }
    }

    virtual void writeNextFrame(CRGB *bufLeds) override {
      // Force the entire frame to be redrawn if the hue is shifting or the underlying color object has been updated.
      if (((hueShift) && (currColor.getShiftDelta() > 0)) || currColor.isUpdated()) {
        redrawFull = true;        
      }
      FFXRotate::writeNextFrame(bufLeds);
    }

    void setBackground( const CRGB &newBack ) {
      fieldColor = newBack;
      redrawFull = true;
    }

    void setShift( boolean newValue ) {
      if (hueShift != newValue ) {
        hueShift = newValue;
      }
      currColor.setShiftDelta( hueShift ? 16 : 0 );
      setVCycleRange(shiftDelay);
    }
};

/*!
 * MotionFX - Palette based motion effect that simulates "random" motion.  Defaults to 
 * a blue, water-like pattern.  A subtler version of the "Pacifica" effect.
 */
class MotionFX : public FFXBase {
  private:
    CRGBPalette16 pendPal;    
    uint8_t pRepeat = 1;  
    uint8_t normRange = 136;
      
  public:
    MotionFX( uint16_t initSize, unsigned long initInterval, CRGBPalette16 initPal ) : FFXBase( initSize, initInterval, 10UL, 250UL ) {
      fxid = MOTION_FX_ID;
      fxName = MOTION_FX_NAME;
      currColor.setColorMode( FFXColor::FFXColorMode::singleCHSV );
      setVCycleRange(255);
      pendPal = initPal;
    }
    MotionFX( uint16_t initSize ) : MotionFX( initSize, 20, OceanColors_p ) {};
    MotionFX( uint16_t initSize, CRGBPalette16 initPal ) : MotionFX( initSize, 20, initPal ) { };
    
    uint8_t getNormalizationRange() { return normRange; }
    uint8_t setNormalizationRange( uint8_t newRange ) { normRange = newRange; }


    void updatePaletteHue( CRGBPalette16 &pal, uint8_t newHue, uint8_t newSat, uint8_t newVal ) {
      CRGBPalette16 refp = OceanColors_p;
      CHSV ref;
      for (int i=0; i<16; i++) {
        ref = rgb2hsv_approximate( refp[i] );
        pal[i] = CHSV( newHue+fixed_map(ref.h,0,255,0,normRange), ref.s < newSat ? newSat : ref.s, ref.v ); 
      }
    }
    
    virtual void initLeds(CRGB *bufLeds) override {
      currColor.setPalette(pendPal);  
      pendPal = currColor.getPalette();
      currColor.setCHSV( CHSV(0, 0, 0) );
      currColor.setUpdated(false);
      setUpdated(true);
    }

    void fillLeds(CRGB *bufLeds, uint16_t phase) {
        if (currColor.isUpdated()) {
          CHSV temp = currColor.getCHSV();
          if (temp.v > 0) { updatePaletteHue( pendPal, temp.h, temp.s, temp.v ); }
        }
        for (uint16_t i=0; i < numLeds; i++) {          
          uint8_t cindex = fixed_map( StepTimer::addOffsetWithWrap(numLeds-phase, i, numLeds/pRepeat), 0, numLeds/pRepeat, 0, 255 );
          uint8_t rindex = StepTimer::addOffsetWithWrap(getCurrVPhase(),i,getVCycleRange()-1);
          bufLeds[i] = blend(ColorFromPalette( currColor.getPalette(), cindex ), ColorFromPalette( currColor.getPalette(), rindex ),200);
        }
        blur1d( bufLeds, numLeds, 10 ); 
        setUpdated(true);
    }
  
  virtual void writeNextFrame(CRGB *bufLeds) override {
    if ((pendPal != currColor.getPalette())) { nblendPaletteTowardPalette( currColor.getPalette(), pendPal, 100); }
       fillLeds(bufLeds, getCurrPhase());
  }
};

/*!
 * RainbowFX - The standard rainbow!
 */
class RainbowFX : public FFXBase {
  private:
    uint8_t startHue = 0;
    uint8_t deltahue = 0;
  
  public:
    RainbowFX( uint16_t initSize, unsigned long initTimer ) :FFXBase( initSize, initTimer, 25UL, 1000UL )  { 
      fxid = RAINBOW_FX_ID;       
      fxName = RAINBOW_FX_NAME;
      deltahue = 256/numLeds;
    }
    
    RainbowFX( uint16_t initSize) : RainbowFX( initSize, 30 ) {};  
    virtual void writeNextFrame( CRGB *bufLeds ) override {
        fill_rainbow( bufLeds, numLeds, startHue, deltahue);
        switch (getMovement()) {
          case MVT_BACKWARD : { startHue -= deltahue; break; }
          case MVT_STILL : { break; }
          default : { startHue += deltahue; break; }
        } 
        setUpdated(true);
    }
};

/*!
 * JuggleFX - "Ported" from FastLED examples.  Uses a helper FXXTrigMotion object which
 * is used to vary the speed of motion, rather than relying on beatsinXX calculations.
 */
class JuggleFX : public FFXBase {
  private:
    uint8_t balls = 10;
    std::vector<FFXTrigMotion *> motion = std::vector<FFXTrigMotion *>();

  public:
    JuggleFX( uint16_t initSize, unsigned long initTimer ) : FFXBase( initSize, initTimer, 10UL, 1000UL ) {
         fxid = JUGGLE_FX_ID;  
         fxName = JUGGLE_FX_NAME;   
         currColor.setColorMode( FFXColor::FFXColorMode::palette16 );
         currColor.setPalette( ::Multi_p, ::Multi_size );
         currColor.setStepDelta( 1 );
         currColor.setShiftDelta( 0 );
         currColor.reset();       
         for( uint16_t i = 0; i < balls; i++) { 
            motion.push_back(new FFXTrigMotion(initSize-1, FFXTrigMotion::TRIG_MOTION_TRI, 0, random8(0,8), random16(0, numLeds*2)));
        }
      }
    JuggleFX(  uint16_t initSize ) : JuggleFX( initSize, 15 ) {}

    ~JuggleFX() { 
      for (auto m : motion) { 
        delete m; 
      }
      motion.clear(); 
    }
    
    virtual void initLeds(CRGB *bufLeds) override {            
      fill_solid(bufLeds, numLeds, CRGB::Black );
    }

    virtual void writeNextFrame(CRGB* bufLeds) override {
    // N colored dots, weaving in and out of sync with each other
    currColor.resetStep();    
    fadeToBlackBy( bufLeds, numLeds, 50 );
    for (auto m : motion) {
      if (m->getDelay()==0) {
        uint16_t pos = m->getPosition();      
        uint16_t next_pos = m->getNextPosition();
        bufLeds[pos] |= currColor.getCRGB();
        if (m->fractComplete() > 0) {
          bufLeds[next_pos] = FFXBase::alphaBlend( bufLeds[next_pos], bufLeds[pos], m->fractComplete(), FFXBase::GAMMA, FFXBase::GAMMA ); 
        }
        if (pos==0) { 
          m->setRangeMax( random8(0,8) ); 
        }
      }
      currColor.step();
      m->step();
    }      
    currColor.shift();       
    setUpdated(true);
  }
    
};

/*!
 * CylonFX - "Ported from FastLED examples.  Single or twin red-pixels moving - in 
 * contrary motion (if twin pixels), uses the FFXTrigMotion helper object to give it some "boounce".
 */
class CylonFX : public FFXBase {
  private:
    FFXTrigMotion mt;
    bool twin = false;

  public:
    CylonFX( uint16_t initSize, unsigned long initTimer ) : FFXBase( initSize, initTimer, 5UL, 150UL ) {
      fxid = CYLON_FX_ID;
      fxName = CYLON_FX_NAME;
      mt.setLimit( initSize-1 );
      mt.setRangeMin( 0 );
      mt.setRangeMax( 3 );   
      mt.setMotion(FFXTrigMotion::TRIG_MOTION_TRI);   
      currColor.setColorMode( FFXColor::FFXColorMode::palette16 );
      currColor.setPalette( ::Multi_p, ::Multi_size );
      currColor.setStepDelta( 1 );
      currColor.setShiftDelta( 1 );
      currColor.reset();       
      }
    CylonFX(  uint16_t initSize ) : CylonFX( initSize, 10 ) {}
    
    virtual void initLeds( CRGB *bufLeds ) override {
      fill_solid( bufLeds, numLeds, CRGB::Black );
    }
        
    virtual void writeNextFrame(CRGB *bufLeds) override {        
        fadeToBlackBy( bufLeds, numLeds, 50 );
        uint16_t pos = mt.getPosition();
        bufLeds[pos] = currColor.getCRGB();
        if (mt.fractComplete() > 0) {
          bufLeds[mt.getNextPosition()] = FFXBase::alphaBlend( bufLeds[mt.getNextPosition()], bufLeds[pos], mt.fractComplete(), FFXBase::GAMMA, FFXBase::GAMMA ); 
        }
        if (twin) {
          bufLeds[mirror(pos)] = bufLeds[pos];        
          if (mt.fractComplete() > 0) {
            bufLeds[mirror(mt.getNextPosition())] = FFXBase::alphaBlend( bufLeds[mirror(mt.getNextPosition())], bufLeds[mirror(pos)], mt.fractComplete(), FFXBase::GAMMA, FFXBase::GAMMA ); 
          }
        }
        mt.step();
        setUpdated(true);
    }
    
};

/*!
 * CycleFX - Cycle through color hues with smooth fades between each.
 */
class CycleFX : public FFXBase {
  private:
    CHSV currHSV;
    CHSV nextHSV;
    uint8_t deltahue = 16;
    StepTimer colorTimer = StepTimer( 5000, false );
    StepTimer transitionTimer = StepTimer( 1000, false );  
  
  public:
    CycleFX( uint16_t initSize, unsigned long initTimer ) :FFXBase( initSize, initTimer, 10UL, 1000UL )  { 
      fxid = CYCLE_FX_ID;       
      fxName = CYCLE_FX_NAME;
      currColor.setColorMode( FFXColor::singleCHSV );
      currHSV = CHSV(random8(15)*16, 255, 255);
      currColor.setCHSV( currHSV );      
    }
    
    CycleFX( uint16_t initSize) : CycleFX( initSize, 100 ) {};  

    virtual void initLeds( CRGB *bufLeds ) override {
      fill_solid( bufLeds, numLeds, currColor.getCHSV() );
      colorTimer.start();
    }
    
    virtual void writeNextFrame( CRGB *bufLeds ) override {
        if (colorTimer.isStarted() && colorTimer.isUp()) {
          nextHSV = CHSV( random8(15)*16, currHSV.s, currHSV.v);
          transitionTimer.start();
          colorTimer.stop();
        }
        if (transitionTimer.isStarted()) {
          if (transitionTimer.isUp()) {
            currHSV = nextHSV;
            currColor.setCHSV( currHSV );
            fill_solid( bufLeds, numLeds, currColor.getCHSV() );            
            transitionTimer.stop();
            colorTimer.start();
            setUpdated(true);
          }
          else {
            CHSV transHSV;
            transHSV = blend( currHSV, nextHSV, fixed_map( transitionTimer.timeSinceTriggered(), 0, transitionTimer.getInterval(), 0, 255 ) );
            if (transHSV != currColor.getCHSV()) {
              currColor.setCHSV(transHSV );
              fill_solid( bufLeds, numLeds, currColor.getCHSV() );
              setUpdated(true);
            }
          }  
        }
    }
};

/*!
 * TwinklwFX - Ported from a random twinkle effect by Mark Kriegsman found here: https://gist.github.com/kriegsman/88954aae22b03a664081
 */
class TwinkleFX : public FFXBase {
public:
  uint8_t *pixelState;
  CRGB deltaColorUp = CRGB(5, 5, 5);
  CRGB deltaColorDown = CRGB( 4, 3, 4);
  CRGB baseColor = CRGB( 0, 0, 0);
  CRGB peakColor = CRGB( 255, 175, 22 );
  uint8_t chanceOfTwinkle = 2;
  enum { SteadyDim, GettingBrighter, GettingDimmerAgain };
  
  TwinkleFX( uint16_t initSize, unsigned long initTimer ) :FFXBase( initSize, initTimer, 10UL, 1000UL )  { 
      fxid = TWINKLE_FX_ID;       
      fxName = TWINKLE_FX_NAME;      
      currColor.setColorMode( FFXColor::singleCRGB );
      currColor.setCRGB( baseColor );      
      pixelState = new uint8_t[numLeds];
      memset( pixelState, sizeof(pixelState), SteadyDim);      
    }  
  
  TwinkleFX( uint16_t initSize ) : TwinkleFX( initSize, 30 ) {}; 
  ~TwinkleFX() { delete pixelState; }
  
  virtual void initLeds( CRGB *bufLeds ) override {
      fill_solid( bufLeds, numLeds, currColor.getCRGB() );
    }
    
    void mapPixels(CRGB *bufLeds)
    {
      for( uint16_t i = 0; i < numLeds; i++) {
        if( pixelState[i] == SteadyDim) {
          // this pixels is currently: SteadyDim
          // so we randomly consider making it start getting brighter
          if( random8() < chanceOfTwinkle) {
            pixelState[i] = GettingBrighter;
          }
      
        } else if( pixelState[i] == GettingBrighter ) {
          // this pixels is currently: GettingBrighter
          // so if it's at peak color, switch it to getting dimmer again
          if( bufLeds[i] >= peakColor ) {
            pixelState[i] = GettingDimmerAgain;
          } else {
            // otherwise, just keep brightening it:
            bufLeds[i] += CRGB( random8(deltaColorUp.r), random8(deltaColorUp.g), random8(deltaColorUp.b) );            
          }      
        } else { // getting dimmer again
          // this pixels is currently: GettingDimmerAgain
          // so if it's back to base color, switch it to steady dim
          if( bufLeds[i] == baseColor ) {
            // bufLeds[i] = baseColor; // reset to exact base color, in case we overshot
            pixelState[i] = SteadyDim;
          } else {
            // otherwise, just keep dimming it down:
            //bufLeds[i] -= CRGB( random8(deltaColorDown.r), random8(deltaColorDown.g), random8(deltaColorDown.b) );;
            fadeToBlackBy( &bufLeds[i], 1, 64 );
          }
        }
      }
    }

    virtual void writeNextFrame(CRGB *bufLeds) override {        
        mapPixels( bufLeds );
        setUpdated(true);  
    }
    
};


/*!
 * DimUsingPaletteFX - Solid light from palette, using brightness to determine the 
 * color to use from the palette.  Useful for creating a "white" light, that fades
 * consistently across the full brightness range.  See the defined palette soft_white_dim_gp
 * as an example.  Finding a good palette is tricky because it depends on many factors.
 */
class DimUsingPaletteFX : public FFXBase {
  public:
    DimUsingPaletteFX(  uint16_t initSize ) : FFXBase( initSize, (uint8_t)10, 10, 1000 ) {
      fxid = DIM_PAL_FX_ID;
      fxName = DIM_PAL_FX_NAME;
      currColor.setColorMode( FFXColor::FFXColorMode::palette256 );
      currColor.setPalette( ::soft_white_dim_gp );
    }

   virtual void onBrightness( uint8_t newBrightness ) override {
      currBrightness = newBrightness;
      currColor.setPaletteIndex(scale8(currBrightness,240));
      currColor.setUpdated(true);
   }

   virtual void initLeds( CRGB *bufLeds ) override {
    // Force the first frame to draw on first call to writeNextFrame - new frame won't be drawn unless color is changed after that...
    currColor.setUpdated(true);
   }

  virtual void writeNextFrame( CRGB *bufLeds ) override {
    if (currColor.isUpdated()) {      
      fill_solid( bufLeds, numLeds, getColor() );
      setUpdated(true);
    } 
    else {
      setUpdated(false);
    }
  }
  private:
    uint8_t currBrightness = 255;
};

/*!
 * PacificaFX - Ported from the FastLED Pacifica effect.
 */
class PacificaFX : public FFXBase {
//  
//   This lighting effect called "Pacifica" was specially created to feel like moving light on water and to look like light moving under water. 
//   Programmer Mark Kriegsman created it on the FASTLED platform in consultation and collaboration with Mary March for her art work 
//   titled "Beneath These Waves Lies Light".
//
//////////////////////////////////////////////////////////////////////////
//
// In this animation, there are four "layers" of waves of light.  
//
// Each layer moves independently, and each is scaled separately.
//
// All four wave layers are added together on top of each other, and then 
// another filter is applied that adds "whitecaps" of brightness where the 
// waves line up with each other more.  Finally, another pass is taken
// over the led array to 'deepen' (dim) the blues and greens.
//
// The speed and scale and motion each layer varies slowly within independent 
// hand-chosen ranges, which is why the code has a lot of low-speed 'beatsin8' functions
// with a lot of oddly specific numeric ranges.
//
// These three custom blue-green color palettes were inspired by the colors found in
// the waters off the southern coast of California, https://goo.gl/maps/QQgd97jjHesHZVxQ7
//
//  Modified from original FastLED native sketch foound here:  https://gist.github.com/kriegsman/36a1e277f5b4084258d9af1eae29bac4
//
public:
    PacificaFX( uint16_t initSize ) : FFXBase( initSize, (uint8_t)255, 10, 20 ) { 
        fxid = PACIFICA_FX_ID;
        fxName = PACIFICA_FX_NAME;
      }

    virtual void writeNextFrame( CRGB *bufLeds ) override {
      pacifica_loop( bufLeds  );
    }

private:
    void pacifica_loop( CRGB *bufLeds ) {
      // Increment the four "color index start" counters, one for each wave layer.
      // Each is incremented at a different speed, and the speeds vary over time.
      uint32_t ms = millis();
      uint32_t deltams = ms - sLastms;
      sLastms = ms;
      uint16_t speedfactor1 = beatsin16(3, 179, 269);
      uint16_t speedfactor2 = beatsin16(4, 179, 269);
      uint32_t deltams1 = (deltams * speedfactor1) / 256;
      uint32_t deltams2 = (deltams * speedfactor2) / 256;
      uint32_t deltams21 = (deltams1 + deltams2) / 2;
      sCIStart1 += (deltams1 * beatsin88(1011,10,13));
      sCIStart2 -= (deltams21 * beatsin88(777,8,11));
      sCIStart3 -= (deltams1 * beatsin88(501,5,7));
      sCIStart4 -= (deltams2 * beatsin88(257,4,6));
    
      // Clear out the LED array to a dim background blue-green
      fill_solid( bufLeds, numLeds, CRGB( 2, 6, 10));
    
      // Render each of four layers, with different scales and speeds, that vary over time
      pacifica_one_layer( bufLeds, pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
      pacifica_one_layer( bufLeds, pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
      pacifica_one_layer( bufLeds, pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
      pacifica_one_layer( bufLeds, pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));
    
      // Add brighter 'whitecaps' where the waves lines up more
      pacifica_add_whitecaps(bufLeds);
    
      // Deepen the blues and greens a bit
      pacifica_deepen_colors(bufLeds);
    }
    
    // Add one layer of waves into the led array
    void pacifica_one_layer( CRGB *bufLeds,const CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff) {
      uint16_t ci = cistart;
      uint16_t waveangle = ioff;
      uint16_t wavescale_half = (wavescale / 2) + 20;
      for( uint16_t i = 0; i < numLeds; i++) {
        waveangle += 250;
        uint16_t s16 = sin16( waveangle ) + 32768;
        uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
        ci += cs;
        uint16_t sindex16 = sin16( ci) + 32768;
        uint8_t sindex8 = scale16( sindex16, 240);
        CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
        bufLeds[i] += c;
      }
    }

    // Add extra 'white' to areas where the four layers of light have lined up brightly
    void pacifica_add_whitecaps( CRGB *bufLeds ) {
      uint8_t basethreshold = beatsin8( 9, 55, 65);
      uint8_t wave = beat8( 7 );
      
      for( uint16_t i = 0; i < numLeds; i++) {
        uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
        wave += 7;
        uint8_t l = bufLeds[i].getAverageLight();
        if( l > threshold) {
          uint8_t overage = l - threshold;
          uint8_t overage2 = qadd8( overage, overage);
          bufLeds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
        }
      }
    }

    // Deepen the blues and greens
    void pacifica_deepen_colors( CRGB *bufLeds)
    {
      for( uint16_t i = 0; i < numLeds; i++) {
        bufLeds[i].blue = scale8( bufLeds[i].blue,  145); 
        bufLeds[i].green= scale8( bufLeds[i].green, 200); 
        bufLeds[i] |= CRGB( 2, 5, 7);
      }
    }

  private:
    uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
    uint32_t sLastms = 0;
    
};

/* **********************************************************************************************************************************
 * 
 *  Overlay FX - Used for flashes and "notifications" - can be blended in real-time with underlying effect - both continue to animate 
 *  
 *
 * **********************************************************************************************************************************
 */
/*!
 * WaveOverlayFX - Overlay Effect which moves a wave of gradient color (by palette) across the 
 * entire strip.
 */
class WaveOverlayFX : public FFXOverlay {
    
  public:
    CRGB pattern[50];
    uint8_t alphamap[54];

    WaveOverlayFX( uint16_t initSize, uint8_t speed, uint8_t repeat, MovementType dir = MVT_FORWARD ) : FFXOverlay(initSize, speed, repeat, 0) {
      fxName = WAVE_OVLY_FX_NAME;
      setVCycleRange( numLeds + 50 );
      CRGBSet leds(pattern, 50);
      currColor.setColorMode(FFXColor::FFXColorMode::palette256);
      setMovement( dir );
    }

    WaveOverlayFX( uint16_t initSize, uint8_t speed, uint8_t repeat, const CRGBPalette16& pal ) : WaveOverlayFX( initSize, speed, repeat ) {
      currColor.setPalette(pal);
    }

    void fillPattern() {
      CRGBPalette16 myPal = currColor.getPalette();
      fill_palette( &pattern[0], 50, 1, 5, myPal, 255, LINEARBLEND );
      for (uint8_t i=0; i<27; i++ ) {
        alphamap[i] = ease8InOutCubic( fixed_map( i, 0, 26, 200, 255 ) );
      }
      for (uint8_t i=27; i<53; i++ ) {
        alphamap[i] = ease8InOutCubic( fixed_map( i, 27, 53, 255, 200 ) );
      }
    }

    virtual void initLeds(CRGB *bufLeds ) override {
      fill_solid( bufLeds, numLeds, CRGB(0,0,0) );
      fillPattern();
      setUpdated(true);
    }

   virtual void writeNextFrame( CRGB *bufLeds ) override {
        uint16_t range = getVCycleRange();
        // Clear overlay and alpha buffers
        fill_solid( bufLeds, numLeds, CRGB(0,0,0) );
        memset ( alpha.data(), 0, numLeds );
        uint16_t centerpos = range/2;
        uint16_t endpos;
        if (currVPhase > centerpos) {
          endpos = fixed_map( ease8InOutQuad( fixed_map( currVPhase, centerpos, range, 0, 255 ) ), 0, 255, centerpos, range+1 );
        }
        else
        {
          endpos = fixed_map( ease8InOutQuad( fixed_map( currVPhase, 1, centerpos, 0, 255 ) ), 0, 255, 0, centerpos );
        }
        if (getMovement()==MVT_BACKWARD || (getMovement()==MVT_BACKFORTH && ((currVCycle % 2)==0))) { 
          endpos = mirror(endpos, range); 
        }
        if ((currVPhase != 1) && (currVPhase < range)) {
          moveAintoB( (byte *)pattern, (byte *)bufLeds, 50, numLeds, endpos, sizeof(CRGB) );    
          moveAintoB( alphamap, alpha.data(), 54, numLeds, endpos+2, 1 );
          blur1d( bufLeds, numLeds, 126 );
        }
        setUpdated(true);
    }

};

/*!
 * PulseOverlayFX - Overlay Effect which "Pulses" the entire strip or a range of pixels using
 * the specified palette.  The pulse is faded in and out while the animation continues behind it.
 */
class PulseOverlayFX : public FFXOverlay {
 
  public:
    PulseOverlayFX( uint16_t initSize, uint8_t speed, uint8_t repeat ) : FFXOverlay(initSize, speed, repeat, 0) {
      fxName = PULSE_OVLY_FX_NAME;
      currColor.setColorMode(FFXColor::FFXColorMode::palette256);
      setMovement( MVT_STILL );
      setMaxAlpha(240);
      rangeLo = 0;
      rangeHi = numLeds-1;
    }
    
    PulseOverlayFX( uint16_t initSize, uint8_t speed, uint8_t repeat, const CRGBPalette16 &pal) : PulseOverlayFX(initSize, speed, repeat) { currColor.setPalette(pal); }

    virtual void initLeds(CRGB *bufLeds ) override {
      for (uint16_t i=rangeLo; i<=rangeHi; i++) {
        bufLeds[i] = ColorFromPalette( currColor.getPalette(), fixed_map(i,rangeLo,rangeHi,0,255) );
        alpha[i] = 0;
      }      
      setUpdated(true);
    }

   void setPixelRange( uint16_t lo, uint16_t hi ) {
      rangeLo = lo > (numLeds-1) ? numLeds-1 : lo;
      rangeHi = hi < rangeLo ? rangeLo : ( hi > (numLeds-1) ? (numLeds-1) : hi );
   }

   virtual void writeNextFrame( CRGB *bufLeds ) override {
      uint8_t valpha;
      if (getCurrPhase() < numLeds/2) { valpha = ease8InOutCubic( fixed_map(getCurrPhase(), 0, numLeds/2-1, 0, 255 ) ); }
      else { valpha = ease8InOutCubic( fixed_map(getCurrPhase(), numLeds/2, numLeds, 255, 0 ) ); }
      for (uint16_t i=rangeLo; i<=rangeHi; i++) {
        alpha[i] = valpha;
      }
      setUpdated(true);
    }

    private:
      uint16_t rangeLo = 0;
      uint16_t rangeHi = 0;

};



#endif
