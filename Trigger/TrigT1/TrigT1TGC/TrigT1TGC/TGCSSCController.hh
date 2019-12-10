/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCSSCController_hh
#define TGCSSCController_hh

#include "TrigT1TGC/TGCHighPtChipOut.hh"
#include "TrigT1TGC/TGCSSCControllerOut.hh"
#include "TrigT1TGC/TGCArguments.hh"

namespace LVL1TGCTrigger {
class TGCSectorLogic;

// const int NumberOfWireHighPtBoard = 2;

class TGCSSCController {
public:
  TGCSSCController( TGCArguments*, const TGCSectorLogic* sL=0 );  
  virtual ~TGCSSCController(){} 

  TGCSSCControllerOut* distribute(TGCHighPtChipOut* wire[], TGCHighPtChipOut* strip);
  void setNumberOfWireHighPtBoard(int iData){m_NumberOfWireHighPtBoard = iData;};
  int getNumberOfWireHighPtBoard(){return m_NumberOfWireHighPtBoard;};
  void setRegion(TGCRegionType regionIn){ m_region=regionIn;};

  TGCArguments* tgcArgs() const;

private:
  const TGCSectorLogic* m_sectorLogic;
  int getSSCId(int nHPB, int chip, int block) const;
  int convertPhi(int chip, int block, int pos) const;
  int getChamberNo(int chip, int block) const;
  int getPhiPosInSSC(int chip, int block) const;
  int m_NumberOfWireHighPtBoard;
  TGCRegionType m_region;
  TGCArguments* m_tgcArgs;
};
  inline TGCArguments* TGCSSCController::tgcArgs() const {
    return m_tgcArgs;
  }
} //end of namespace bracket

#endif
