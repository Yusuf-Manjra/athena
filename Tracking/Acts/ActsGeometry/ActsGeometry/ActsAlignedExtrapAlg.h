/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"

// PACKAGE
#include "ActsGeometry/IActsExCellWriterSvc.h"
#include "ActsGeometry/ActsExtrapolationTool.h"

// STL
#include <string>

namespace Acts {
  class TrackingGeometry;
  
  template<typename>
  class ExtrapolationCell;

}
  
class ParticleGun;

class ActsAlignedExtrapAlg  :  public AthAlgorithm {
  
public:
    
  ActsAlignedExtrapAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ActsAlignedExtrapAlg();
  
  virtual bool isClonable() const override { return true; }

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

  
private:
  
  SG::ReadCondHandleKey<GeoAlignmentStore> m_rch {this, "PixelAlignmentKey", "PixelAlignment", "cond read key"};

  ServiceHandle<IActsExCellWriterSvc> m_exCellWriterSvc;
  ServiceHandle<IAthRNGSvc> m_rndmGenSvc;
  
  ToolHandle<ActsExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "ActsExtrapolationTool"};

};

