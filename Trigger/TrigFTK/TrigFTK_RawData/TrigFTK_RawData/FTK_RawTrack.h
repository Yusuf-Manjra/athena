/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGFTK_RAWDATA_FTK_RAWTRACK_H
#define TRIGFTK_RAWDATA_FTK_RAWTRACK_H

/**************************************************************

NAME:     FTK_RawTrack.h
PACKAGE:  atlasoff/Trigger/TrigFTK/TrigFTK_RawData

AUTHORS:  James Howarth
CREATED:  February 2014

PURPOSE:  A base class for RAW FTK tracks.
          Based on proposed format by Jinlong Zhang.
**************************************************************/

#include <stdint.h>
#include "CLIDSvc/CLASS_DEF.h"
#include "TrigFTK_RawData/FTK_RawSCT_Cluster.h"
#include "TrigFTK_RawData/FTK_RawPixelCluster.h"

//#define NPIXLAYERS 4
//#define NSCTLAYERS 8 //0,2,4,6 = Axial, 1,3,5,7 = Stero

class FTK_RawTrack {
private:
  uint32_t m_word_th1;
  uint32_t m_word_th2;
  uint32_t m_word_th3;
  uint32_t m_word_th4;
  uint32_t m_word_th5;
  std::vector<FTK_RawPixelCluster> m_pix_clusters;
  std::vector<FTK_RawSCT_Cluster>  m_sct_clusters;
  signed long m_barcode;

  static const int npixlayers = 4; 
  static const int nsctlayers = 8;
  static const int sixteen_bit_offset = 32767; // 2**15 -1 
  static const float d0_precision;
  static const float z0_precision;
  static const float phi_precision;
  static const float curv_precision;
  static const float cot_precision;

public:
  FTK_RawTrack(); 
  FTK_RawTrack(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
  FTK_RawTrack(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const std::vector<FTK_RawPixelCluster>&, const std::vector<FTK_RawSCT_Cluster>&);
  ~FTK_RawTrack();

  float getD0() const;
  float getZ0() const;
  float getPhi() const;
  double getCotTh() const;
  double getCurv() const;
  uint32_t getRoadID() const {return m_word_th1; }
  uint32_t getTH1() const  {return m_word_th1; }
  uint32_t getTH2() const  {return m_word_th2; }
  uint32_t getTH3() const  {return m_word_th3; }
  uint32_t getTH4() const  {return m_word_th4; }
  uint32_t getTH5() const  {return m_word_th5; }
  uint32_t getPixelWordA( int )   const;
  uint32_t getPixelWordB( int )   const;
  uint32_t getPixelBarcode( int ) const;
  uint32_t getSCTWord( int )      const;
  uint32_t getSCTBarcode( int )   const;

  signed long getBarcode() const {return m_barcode; }
  int return_bits(int low, int high, uint32_t word) const;
  FTK_RawPixelCluster& getPixelCluster( int );
  FTK_RawSCT_Cluster&  getSCTCluster(   int );

  const FTK_RawPixelCluster& getPixelCluster( int ) const; 
  const FTK_RawSCT_Cluster&  getSCTCluster(   int ) const; 
  
  std::vector<FTK_RawPixelCluster>& getPixelClusters() { return m_pix_clusters;} 
  std::vector<FTK_RawSCT_Cluster>& getSCTClusters()    { return m_sct_clusters;} 

  const std::vector<FTK_RawPixelCluster>& getPixelClusters() const { return m_pix_clusters;} 
  const std::vector<FTK_RawSCT_Cluster>& getSCTClusters()    const { return m_sct_clusters;} 

  void setRoadID(int v)   { m_word_th1 = v; }
  void setTH1(uint32_t v) { m_word_th1 = v; }
  void setTH2(uint32_t v) { m_word_th2 = v; }
  void setTH3(uint32_t v) { m_word_th3 = v; }
  void setTH4(uint32_t v) { m_word_th4 = v; }
  void setTH5(uint32_t v) { m_word_th5 = v; }
  void setD0(float);
  void setZ0(float);
  void setPhi(float);
  void setCotTh(float);
  void setCurv(float);
  void setPixelCluster( FTK_RawPixelCluster );
  void setSCTCluster(   FTK_RawSCT_Cluster  );
  void setPixelClusters( std::vector<FTK_RawPixelCluster> );
  void setSCTClusters(   std::vector<FTK_RawSCT_Cluster>  );
  void setBarcode(signed long barcode) { m_barcode = barcode; }

};

CLASS_DEF( FTK_RawTrack , 239377431 , 1 )

#endif // TRIGFTK_RAWDATA_FTK_RAWTRACK_H
