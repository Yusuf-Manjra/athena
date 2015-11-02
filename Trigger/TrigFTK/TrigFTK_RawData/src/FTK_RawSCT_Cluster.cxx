/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_RawData/FTK_RawSCT_Cluster.h"
#include <iostream>

#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))

using namespace std;

FTK_RawSCT_Cluster::FTK_RawSCT_Cluster() :
  m_word(0),
  m_layer(FTK_RawSCT_Cluster::missingLayer),
  m_barcode(0)
{
  // nothing to do
}

FTK_RawSCT_Cluster::FTK_RawSCT_Cluster(int i) :
  m_word(0),
  m_barcode(0)
{
  m_layer = i;
}

FTK_RawSCT_Cluster::FTK_RawSCT_Cluster(uint32_t word, signed long barcode, int layer)
{
  m_word        = word;
  m_layer       = layer;
  m_barcode     = barcode;
}

FTK_RawSCT_Cluster::~FTK_RawSCT_Cluster()
{
  // nothing to do
}

float FTK_RawSCT_Cluster::getHitCoord() const{

  // TopBit 23 BottomBit 13  (11 bits) Max 2048 LeftShift 8 (=31-TopBit) RightShift 21 (=LeftShift+BottomBit)

  uint32_t hit = (m_word <<  8) >> 21; 

  return 0.5 * (float)hit;

}

void FTK_RawSCT_Cluster::setHitCoord( float hit_coord ){

  // TopBit 23 BottomBit 13  (11 bits) Max 2048 

  uint32_t hit = uint32_t (hit_coord*2.);

  if ( hit < 2048 ){
    hit = hit << 13 ;
    m_word = (hit | m_word);
  }
  return;
}

unsigned int FTK_RawSCT_Cluster::getHitWidth() const{

  // TopBit 26 BottomBit 24  (3 bits) Max 8 LeftShift 5 (=31-TopBit) RightShift 29 (=LeftShift+BottomBit)

  unsigned int width = (m_word << 5) >> 29;
  width = width + 1;
  return width;

}

void FTK_RawSCT_Cluster::setHitWidth( unsigned int hit_width ){

  // TopBit 26 BottomBit 24  (3 bits) Max 8

  uint32_t width = hit_width;

  if(width > 8) width = 8;
  width -=1;
  width = width << 24;

  m_word = (width | m_word);
  
  return;

}

unsigned int FTK_RawSCT_Cluster::getModuleID() const{
  
  // TopBit 12 BottomBit 0  (13 bits) Max 8192 LeftShift 19 (=31-TopBit) RightShift 19 (=LeftShift+BottomBit)

  uint32_t module_id = m_word << 19;
  module_id = module_id >> 19;
  return module_id;

}


void FTK_RawSCT_Cluster::setModuleID( unsigned int module_id ){

  // TopBit 11 BottomBit 0  (13 bits) 

  uint32_t id = module_id;

  if (id <  8192) m_word = (id | m_word);

  
  return;
}


/*void FTK_RawSCT_Cluster::setHitStereo(float hit_stereo){

  float multiplier = 1.;
  float range_top  = 767.;
  float range_bot  = 0.;
  int   offset     = 0;
  int   position   = 0;

  uint32_t hit = 0;
  if(hit_stereo > range_bot && hit_stereo < range_top)
    hit = NINT( multiplier*hit_stereo + offset);
 
  hit = hit << position;
  m_word = (hit | m_word);

  return;
  }*/

/*void FTK_RawSCT_Cluster::setWidthStereo(float width_stereo){

  float multiplier = 1.;
  float range_top  = 9.;
  float range_bot  = 0.;
  int   offset     = 1;
  int   position   = 12;

  uint32_t width = 0;
  if(width_stereo > range_bot && width_stereo < range_top)
    width = NINT( multiplier*width_stereo + offset);

  width = width << position;
  m_word = (width | m_word);

  return;
}*/

/*float FTK_RawSCT_Cluster::getHitStereo(){

  float multiplier = 1.;
  int   offset     = 0;
  int   position   = 0;

  uint16_t hit = 0;
  hit = hit | (m_word >> position);
  hit = hit << 5;
  hit = hit >> 5; // Could probably do this in one line with a NOR or something.
  //  std::cout <<"JWH DEBUG HIT = " << hit << std::endl;
  float hit_stereo = (((float)(hit - offset))/multiplier);
  return hit_stereo;
  }*/

/*float FTK_RawSCT_Cluster::getHitAxial(){

  float multiplier = 1.;
  int   offset     = 0;
  int   position   = 16;

  uint16_t hit = 0;
  hit = hit | (m_word >> position);
  hit = hit << 5;
  hit = hit >> 5; // Could probably do this in one line with a NOR or something.

  float hit_axial = (((float)(hit - offset))/multiplier);
  return hit_axial;
  }*/

/*int FTK_RawSCT_Cluster::getWidthStereo(){

  float multiplier = 1.;
  int   offset     = 1;
  int   position   = 12;

  uint16_t width = 0;
  width = width | (m_word >> position);
  width = width << 13;
  width = width >> 13; // Could probably do this in one line with a NOR or something.

  int width_stereo = NINT(((float)(width - offset))/multiplier);
  return width_stereo;
  }*/

/*int FTK_RawSCT_Cluster::getWidthAxial(){

  float multiplier = 1.;
  int   offset     = 1;
  int   position   = 28;

  uint16_t width = 0;
  width = width | (m_word >> position);
  width = width << 1;
  width = width >> 1; //Dont actually need this since last bit will always be set to 0

  int width_stereo = NINT(((float)(width - offset))/multiplier);
  return width_stereo;
  }*/

/*unsigned int FTK_RawSCT_Cluster::getHashID(){

  float multiplier = 1.;
  int   offset     = 0;
  int   position   = 0;

  uint16_t hash = 0;
  hash = hash | (m_word >> position);
  hash = hash << 4;
  hash = hash >> 4;

  unsigned int hash_id = (unsigned int)hash;
  return hash_id;
  }*/

/*void FTK_RawSCT_Cluster::setHashID(unsigned int hash_id){

  float multiplier = 1.;
  int   offset     = 0;
  int   position   = 0;

  uint32_t hash = 0;
  hash = NINT( multiplier*hash_id + offset);

  hash = hash << position;
  m_word_header = (hash | m_word_header);
  return;
  }*/

#if defined(__MAKECINT__)
#pragma link C++ class FTK_RawSCT_Cluster;
#endif
