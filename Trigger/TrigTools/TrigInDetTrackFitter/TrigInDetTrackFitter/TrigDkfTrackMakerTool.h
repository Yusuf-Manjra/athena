/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGDKFTRACKMAKERTOOL_H__
#define __TRIGDKFTRACKMAKERTOOL_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigInDetToolInterfaces/ITrigDkfTrackMakerTool.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"
#include "TrkTrack/Track.h"

#include <vector>

class AtlasDetectorID;
class PixelID;
class SCT_ID;

class TrigDkfTrackMakerTool : virtual public ITrigDkfTrackMakerTool, public AthAlgTool {
 public:
      
  // standard AlgTool methods
  TrigDkfTrackMakerTool(const std::string&,const std::string&,const IInterface*);
		
  // standard Athena methods
  StatusCode initialize();

  bool createDkfTrack(std::vector<const TrigSiSpacePoint*>&, std::vector<Trk::TrkBaseNode*>&, double) const;
	bool createDkfTrack(const Trk::Track& track, std::vector<Trk::TrkBaseNode*>& vpTrkNodes, double DChi2) const;

    
 private:
  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  const AtlasDetectorID* m_idHelper;
  
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  const InDetDD::SiDetectorElement* getPixelDetectorElement(const IdentifierHash& waferHash) const;
  const InDetDD::SiDetectorElement* getSCTDetectorElement(const IdentifierHash& waferHash) const;
};

#endif 
