/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonSegmentTruthAssociationAlg.h
//   Header file for class MuonSegmentTruthAssociationAlg
///////////////////////////////////////////////////////////////////

#ifndef TRUTHPARTICLEALGS_MUONSEGMENTTRUTHASSOCIATION_H
#define TRUTHPARTICLEALGS_MUONSEGMENTTRUTHASSOCIATION_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecToolInterfaces/IMuonTrackTruthTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "xAODMuon/MuonSegmentContainer.h"
#include "StoreGate/WriteDecorHandleKey.h"

#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/CscSimDataCollection.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "StoreGate/ReadHandleKey.h"
#include "GeneratorObjects/McEventCollection.h"

namespace Muon {

class MuonSegmentTruthAssociationAlg : public AthAlgorithm  {

public:
  // Constructor with parameters:
  MuonSegmentTruthAssociationAlg(const std::string &name,ISvcLocator *pSvcLocator);

  // Basic algorithm methods:
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  ToolHandle<Muon::MuonIdHelperTool>    m_idHelper;
  ToolHandle<Muon::MuonEDMPrinterTool>  m_printer;
  ToolHandle<Muon::IMuonTrackTruthTool> m_muonTrackTruthTool;
  Gaudi::Property<SG::WriteDecorHandleKey<xAOD::MuonSegmentContainer> >m_muonTruthSegmentContainerName{this,"MuonTruthSegmentName","MuonTruthSegments","muon truth segment container name"};
  Gaudi::Property<SG::WriteDecorHandleKey<xAOD::MuonSegmentContainer> >m_muonSegmentCollectionName{this,"MuonSegmentLocation","MuonSegments","muon segment container name"};
  SG::ReadHandleKey<McEventCollection> m_mcEventColl{this,"McEventCollectionKey","TruthEvent","McEventCollection"};
  SG::ReadHandleKeyArray<MuonSimDataCollection> m_muonSimData{this,"MuonSimDataNames",{ "MDT_SDO", "RPC_SDO", "TGC_SDO", "sTGC_SDO", "MM_SDO" },"Muon SDO maps"};
  SG::ReadHandleKey<CscSimDataCollection> m_cscSimData{this,"CSC_SDO_Container","CSC_SDO","CSC SDO"};
  SG::ReadHandleKey<TrackRecordCollection> m_trackRecord{this,"TrackRecord","MuonEntryLayerFilter","Track Record Collection"};
  int m_barcodeOffset;
  bool m_hasCSC;
  bool m_hasSTgc;
  bool m_hasMM;
};

} // namespace Muon


#endif //TRUTHPARTICLEALGS_MUONTRUTHDECORATIONALG_H
