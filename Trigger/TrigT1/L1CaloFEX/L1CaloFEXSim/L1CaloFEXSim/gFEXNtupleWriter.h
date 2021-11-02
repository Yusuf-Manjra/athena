/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXNtupleWriter
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#ifndef GFEXTUPLEWRITER_H
#define GFEXTUPLEWRITER_H
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"
#include <memory>
#include "L1CaloFEXSim/gFEXOutputCollection.h"

namespace LVL1 {
class gFEXNtupleWriter : public AthAlgorithm
{
public:
  // this is a standard algorithm constructor
  gFEXNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator);
  ~gFEXNtupleWriter();
  // these are the functions inherited from Algorithm
   StatusCode initialize ();
   StatusCode execute ();
   StatusCode finalize ();

private:
  //readhandle key for gFEXOutputCollection
  SG::ReadHandleKey<LVL1::gFEXOutputCollection> m_gFEXOutputCollectionSGKey {this, "MyOutputs", "gFEXOutputCollection", "MyOutputs"};

  gFEXOutputCollection* m_gFEXOutputCollection;
  float m_jet_nTOBs;
  float m_global_nTOBs;
  bool m_load_truth_jet;

  std::vector<float> m_truth_jet_eta;
  std::vector<float> m_truth_jet_phi;
  std::vector<float> m_truth_jet_ET;

  std::vector<float> m_jet_TOB;
  std::vector<float> m_jet_TOB_Eta;
  std::vector<float> m_jet_TOB_Phi;
  std::vector<float> m_jet_TOB_ET;
  std::vector<float> m_jet_TOB_ID;
  std::vector<float> m_jet_TOB_Status;


  std::vector<float> m_global_TOB;
  std::vector<float> m_global_TOB_Quantity1;
  std::vector<float> m_global_TOB_Quantity2;
  std::vector<float> m_global_TOB_Saturation;
  std::vector<float> m_global_TOB_ID;
  std::vector<float> m_global_TOB_Status1;
  std::vector<float> m_global_TOB_Status2;

  std::string m_jet_container_name = "AntiKt10TruthJets";
  TTree *m_myTree;

  StatusCode loadJetAlgoVariables(SG::ReadHandle<LVL1::gFEXOutputCollection>);
  StatusCode loadGlobalAlgoVariables(SG::ReadHandle<LVL1::gFEXOutputCollection>);
  StatusCode loadTruthElectron();
  StatusCode loadTruthJets();

};

}
#endif
