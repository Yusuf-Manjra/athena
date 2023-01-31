/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ICaloTrackingVolumeBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ICALOTRACKINGVOLUMEBUILDER_H
#define TRKDETDESCRINTERFACES_ICALOTRACKINGVOLUMEBUILDER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

class CaloDetDescrManager;
namespace Trk {

class TrackingVolume;
class BinUtility1D;

/** @class ICaloTrackingVolumeBuilder
  Interface class ICaloTrackingVolumeBuilders
  It inherits from IAlgTool. The actual implementation of the AlgTool depends on
  the SubDetector, more detailed information should be found there.
  */
class ICaloTrackingVolumeBuilder : virtual public IAlgTool
{

public:
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ICaloTrackingVolumeBuilder, 1, 0);

  /**Virtual destructor*/
  virtual ~ICaloTrackingVolumeBuilder() {}

  /** TrackingVolumeBuilder interface method - returns vector of Volumes */
  virtual std::vector<TrackingVolume*>* trackingVolumes(
    const CaloDetDescrManager& caloDDM) const = 0;
};

} // end of namespace

#endif
