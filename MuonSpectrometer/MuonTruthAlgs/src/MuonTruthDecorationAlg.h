/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHPARTICLEALGS_MUONTRUTHDECORATIONALG_H
#define TRUTHPARTICLEALGS_MUONTRUTHDECORATIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include <map>
#include <vector>

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonRecToolInterfaces/IMuonTrackTruthTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include "TrackRecord/TrackRecordCollection.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "GeneratorObjects/xAODTruthParticleLink.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteHandleKeyArray.h"

class MuonSimDataCollection;
class CscSimDataCollection;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {

class MuonTruthDecorationAlg : public AthAlgorithm  {

public:
  typedef std::map<Muon::MuonStationIndex::ChIndex, std::vector<Identifier> >   ChamberIdMap;

  // Constructor with parameters:
  MuonTruthDecorationAlg(const std::string &name,ISvcLocator *pSvcLocator);

  // Basic algorithm methods:
  virtual StatusCode initialize();
  virtual StatusCode execute();

private:
  void addTrackRecords( xAOD::TruthParticle& truthParticle, const xAOD::TruthVertex* vertex ) const;
  void addHitCounts( xAOD::TruthParticle& truthParticle, ChamberIdMap* ids = 0 ) const;
  void addHitIDVectors( xAOD::TruthParticle& truthParticle, const MuonTruthDecorationAlg::ChamberIdMap& ids) const;
  void createSegments( const ElementLink< xAOD::TruthParticleContainer >& truthLink,SG::WriteHandle<xAOD::MuonSegmentContainer> segmentContainer,
                       const MuonTruthDecorationAlg::ChamberIdMap& ids) const;

  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleContainerName;
  SG::WriteHandleKey<xAOD::TruthParticleContainer> m_muonTruthParticleContainerName;
  SG::WriteHandleKey<xAOD::MuonSegmentContainer> m_muonTruthSegmentContainerName;
  SG::ReadHandleKeyArray<TrackRecordCollection> m_trackRecordCollectionNames;
  SG::ReadHandleKeyArray<PRD_MultiTruthCollection> m_PRD_TruthNames;
  SG::ReadHandleKeyArray<MuonSimDataCollection> m_SDO_TruthNames;
  SG::ReadHandleKey<CscSimDataCollection> m_CSC_SDO_TruthNames;
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  ToolHandle<Muon::MuonEDMPrinterTool>  m_printer;
  ToolHandle<IMCTruthClassifier>        m_truthClassifier;
  ToolHandle<Trk::IExtrapolator>        m_extrapolator;
  const MuonGM::MuonDetectorManager* m_muonMgr;
  bool m_createTruthSegment;
  int m_barcodeOffset;
};

} // namespace Muon


#endif //TRUTHPARTICLEALGS_MUONTRUTHDECORATIONALG_H
