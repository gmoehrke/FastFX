//
//  FFXCoreEffects.h
//
//  Copyright 2020 - Geoff Moehrke 
//  gmoehrke@gmail.com
// 
#include "FFXCoreEffects.h"

DEFINE_GRADIENT_PALETTE( blue_wave_gp ) {
      0,   0,  0, 150,
    106,   0,  0, 255,
    126, 25, 200, 255,
    146,   0,  0, 255,
    255,   0,  0, 150};

DEFINE_GRADIENT_PALETTE( red_wave_gp ) {
      0, 150,   0,   0,
    106, 255,  10,   0,
    126, 255,  85,  20,
    146, 255,  10,   0,
    255, 150,   0,   0};

DEFINE_GRADIENT_PALETTE( yellow_wave_gp ) {
    200, 80,   33,   0,
    106, 255,  87,   0,
    126, 255, 126,  20,
    146, 255,  87,   0,
    255, 80,   33,   0};

DEFINE_GRADIENT_PALETTE( green_wave_gp ) {
      0,   0, 150,   0,
    106,   0, 255,   0,
    126, 100, 255,   0,
    146,   0, 255,   0,
    255,   0, 150,   0};

DEFINE_GRADIENT_PALETTE( orange_wave_gp )  {
      0, 150,  40,   0,
    106, 255,  65,   0,
    126, 255,  85,   0,
    146, 255,  65,   0,
    255, 150,  80,   0};   

DEFINE_GRADIENT_PALETTE( purple_wave_gp )  {
      0, 25,   0,   45,
    106, 55,   0,  100,
    126,255,   0,  255,
    146, 55,   0,  100,
    255, 25,   0,   45};  

DEFINE_GRADIENT_PALETTE( teal_wave_gp ) {
      0,   0,140,  85,
    106,   0,225, 120,
    126,  85,255, 113,
    146,   0,225, 120,
    255,   0,140,  85};

DEFINE_GRADIENT_PALETTE( white_wave_gp ) {
    0, 255, 184,  20,
  106, 255, 184,  20,
  126, 255, 179,  40,
  246, 255, 184,  20,    // Note that the max entry goes in 240 since after that it will begin to wrap/blend back to the 0th entry
  255, 255, 184,  20 
};

DEFINE_GRADIENT_PALETTE( soft_white_dim_gp ) {
    0,   0,   0,   0,
   50, 255, 184,  20,
  110, 255, 160,  22,
  240, 255, 184,  20,    // Note that the max entry goes in 240 since after that it will begin to wrap/blend back to the 0th entry
  255, 255, 184,  20 
};     

const CRGBPalette16 NamedPalettes::operator[](String index) {
          auto it = std::find_if( plist.begin(), plist.end(), [&index](const std::pair<String, const CRGBPalette16&>& element) -> bool {return index.equals(element.first);} );
          if (it != plist.end()) {
            return (*it).second;
          }
          else {
            return blue_wave_gp;
          }
      }

String NamedPalettes::operator[](CRGBPalette16 index) {
        auto it = std::find_if( plist.begin(), plist.end(), [&index](const std::pair<String, CRGBPalette16>& element) -> bool {return index==element.second;} );
          if (it != plist.end()) {
            return (*it).first;
          }
          else {
            return "blue";
        }
      }
