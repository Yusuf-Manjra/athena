/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
// filename: TrigTrackingxAODCnv.h
//
// author: Jiri Masik
//         Jiri.Masik@manchester.ac.uk
//
//
// -------------------------------
// ATLAS Collaboration
*/
////////////////////////////////////////////////////////////////////////////

#ifndef INDETTRIGPARTICLECREATION_TRACKINGXAODCNVMT_H
#define INDETTRIGPARTICLECREATION_TRACKINGXAODCNVMT_H

#include "GaudiKernel/ToolHandle.h"

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


#include "xAODTracking/TrackParticleContainer.h"
#include "TrkTrack/TrackCollection.h"

// STL
#include <string>
#include <vector>

class TH1F;
class AtlasDetectorID;
class Identifier;
class PixelID;
class TrigRoiDescriptorCollection;
 

namespace Trk {
  class ITrackParticleCreatorTool;
  class IResidualPullCalculator;
}

namespace InDet
{


  class TrigTrackingxAODCnvMT : public AthReentrantAlgorithm
  {
  public:
    TrigTrackingxAODCnvMT(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrigTrackingxAODCnvMT();
    StatusCode initialize();
    StatusCode execute(const EventContext& ctx) const;

  private:
    ToolHandle< Trk::ITrackParticleCreatorTool > m_particleCreatorTool;
    ToolHandle< Trk::IResidualPullCalculator > m_residualCalc;
    const AtlasDetectorID* m_idHelper{};
    const PixelID* m_pixelId{};

    bool                         m_doIBLresidual{};

    //Monitoring
    SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;
    SG::ReadHandleKey<TrackCollection> m_trackKey;
    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_trackParticleKey;

  };

}

#endif
