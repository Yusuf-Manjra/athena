/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigSteeringTest_TrigBphysL1DiMuComboFex
#define TrigSteeringTest_TrigBphysL1DiMuComboFex
/////////////////////////////////
//
//Simple dummy Fex from ComboAlgo
// basic checks on L1 objects
// Design usage, to allow L1Topo to function correctly
// Developed from TrigSteeringTest/DummyComboUsingTopo.h
//
/////////////////////////////////


#include "TrigInterfaces/ComboAlgo.h"



//class TrigRoiDescriptor;

class TrigBphysL1DiMuComboFex : public HLT::ComboAlgo {
public:

   TrigBphysL1DiMuComboFex(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~TrigBphysL1DiMuComboFex() {}

   virtual HLT::ErrorCode hltInitialize();

   virtual HLT::ErrorCode hltFinalize();

   HLT::ErrorCode hltExecute(HLT::TEConstVec& input, HLT::TriggerElement* output);
      
private:

   std::vector<float> m_pt;   // monitoring of pT of rois (multiple per execute call)
   float              m_dEta; // monitoring of delta eta of roi combinations (one per execute call)

};

#endif
