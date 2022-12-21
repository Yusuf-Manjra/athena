/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackingGeometry/MuonStationBuilder.h"

// constructor
Muon::MuonStationBuilder::MuonStationBuilder(const std::string& t,
                                             const std::string& n,
                                             const IInterface* p)
    : Muon::MuonStationBuilderImpl(t, n, p) {
  declareInterface<Trk::IDetachedTrackingVolumeBuilder>(this);
}

StatusCode Muon::MuonStationBuilder::initialize() {
  // get Muon Spectrometer Description Manager
  ATH_CHECK(detStore()->retrieve(m_muonMgr));
  ATH_MSG_INFO(m_muonMgr->geometryVersion());
  return Muon::MuonStationBuilderImpl::initialize();
}

std::unique_ptr<std::vector<std::unique_ptr<Trk::DetachedTrackingVolume>>>
Muon::MuonStationBuilder::buildDetachedTrackingVolumes(bool blend) const{
  if (!m_muonMgr) {
    ATH_MSG_FATAL("No muon manager is provided");
    return nullptr;
  }

  return Muon::MuonStationBuilderImpl::buildDetachedTrackingVolumesImpl(m_muonMgr, blend);
}
