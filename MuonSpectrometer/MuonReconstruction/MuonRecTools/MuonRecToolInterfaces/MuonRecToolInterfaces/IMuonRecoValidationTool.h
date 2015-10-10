/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONRECOVALIDATIONTOOL_H
#define MUON_IMUONRECOVALIDATIONTOOL_H

/** 
    @class IMuonRecoValidationTool

    @author Niels.Van.Eldik@cern.ch
*/

#include "GaudiKernel/IAlgTool.h"

#include "xAODTracking/TrackParticle.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "MuonLayerHough/MuonLayerHough.h"

namespace Trk {
  class PrepRawData;
}

namespace Muon {
  
  class MuonSegment;

  /** Interface ID*/  
  static const InterfaceID IID_IMuonRecoValidationTool("Muon::IMuonRecoValidationTool", 1, 0);

  class IMuonRecoValidationTool : virtual public IAlgTool {
  public:
     /**Virtual destructor*/
    virtual ~IMuonRecoValidationTool(){};
  
    /** AlgTool and IAlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IMuonRecoValidationTool; }
    
    /** add a new TrackParticle with it's muon system extension */
    virtual bool addTrackParticle(  const xAOD::TrackParticle& indetTrackParticle, const MuonSystemExtension& muonSystemExtention ) = 0;
    
    /** add a new segment, indicate the stage of the reco */
    virtual bool add( const MuonSystemExtension::Intersection& intersection, const MuonSegment& segment, int stage ) = 0;

    /** add a new hough maximum */
    virtual bool add( const MuonSystemExtension::Intersection& intersection,const MuonHough::MuonLayerHough::Maximum& maximum ) = 0;

    /** add a new prd */
    virtual bool add( const MuonSystemExtension::Intersection& intersection, const Trk::PrepRawData& prd, float expos, float expos_err ) = 0;

  };

}

#endif
