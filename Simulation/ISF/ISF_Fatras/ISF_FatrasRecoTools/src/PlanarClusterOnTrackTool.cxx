/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class PlanarClusterOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for PlanarClusterOnTrack object production
///////////////////////////////////////////////////////////////////

#include "ISF_FatrasRecoTools/PlanarClusterOnTrackTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "ISF_FatrasDetDescrModel/PlanarDetElement.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

namespace iFatras {
  PlanarClusterOnTrackTool::PlanarClusterOnTrackTool
  (const std::string& t,const std::string& n,const IInterface* p) :
    ::AthAlgTool(t,n,p)
  {
    declareInterface<IRIO_OnTrackCreator>(this);
  }
  
  ///////////////////////////////////////////////////////////////////
  // Destructor  
  ///////////////////////////////////////////////////////////////////
  
  PlanarClusterOnTrackTool::~PlanarClusterOnTrackTool(){}

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////
  
  StatusCode PlanarClusterOnTrackTool::initialize()
  {
    StatusCode sc = AthAlgTool::initialize(); 
    
    msg(MSG::INFO)  << name() << " initialize()" << endreq;   
    
    return sc;
  }
  
  ///////////////////////////////////////////////////////////////////
  // Finalize
  ///////////////////////////////////////////////////////////////////
  
  StatusCode PlanarClusterOnTrackTool::finalize()
  {
    StatusCode sc = AlgTool::finalize(); 
    return sc;
  }
  
  ///////////////////////////////////////////////////////////////////
  // iFatras::PlanarClusterOnTrack  production
  ///////////////////////////////////////////////////////////////////
  
  const PlanarClusterOnTrack* PlanarClusterOnTrackTool::correct
  (const Trk::PrepRawData& rio,const Trk::TrackParameters&) const
  {

    ATH_MSG_VERBOSE ("PlanarClusterOnTrackTool:: correct()");
  
    const PlanarCluster* plc=dynamic_cast<const PlanarCluster*>(&rio);
  
    if(plc==0) {
      ATH_MSG_WARNING("This is not a planar cluster, return 0.");
      return 0;
    }
    
    // Get pointer to detector element
    //
    const iFatras::PlanarDetElement* element = plc->detectorElement(); 
    if(!element) {
      ATH_MSG_WARNING ("Cannot access detector element. Aborting cluster correction...");
      return 0;
    }
  
    IdentifierHash iH = element->identifyHash();
  
    // PlanarCluster global and local position
    //
    Amg::Vector3D   glob(plc->globalPosition());
    Amg::Vector2D locpos = plc->localPosition();    
    Trk::LocalParameters locpar = Trk::LocalParameters(locpos);

    // Covariance matrix
    //
    Amg::MatrixX oldLocalCov = plc->localCovariance();
    Amg::MatrixX* loce = new Amg::MatrixX(oldLocalCov);
      
    ATH_MSG_DEBUG ("PlanarCluster: Global Position --> "  << glob);  
    ATH_MSG_DEBUG ("PlanarCluster: Local Position --> "   << locpos);  
    ATH_MSG_DEBUG ("PlanarCluster: Local Parameters --> " << locpar);  
    ATH_MSG_DEBUG ("PlanarCluster: Error Matrix --> "     << *loce);  

    iFatras::PlanarClusterOnTrack* pcot = new iFatras::PlanarClusterOnTrack(plc,locpar,*loce,iH,glob);
    
    ATH_MSG_DEBUG ("Planar: Planar Cluster On Track "     << pcot->dump(std::cout));  
    
    return pcot;
  }

void PlanarClusterOnTrackTool::handle(const Incident& inc) 
{
  if ( inc.type() == "EndEvent" ){
    ATH_MSG_VERBOSE("'EndEvent' incident caught.");
  }
}

}
