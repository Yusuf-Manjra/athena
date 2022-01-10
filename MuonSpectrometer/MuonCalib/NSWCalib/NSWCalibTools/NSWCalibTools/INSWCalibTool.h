/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INSWCalibTool_h
#define INSWCalibTool_h

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"

#include <cmath>
#include <vector>
#include "float.h"

#include "TF1.h"


static const InterfaceID IID_INSWCalibTool("Muon::INSWCalibTool",1,0);

namespace NSWCalib { 

  struct CalibratedStrip {
    double charge = 0;
    double time = 0;
    double resTime = 0;
    double distDrift = 0;
    double resTransDistDrift = 0;
    double resLongDistDrift = 0;
    double dx = 0;
    Amg::Vector2D locPos = Amg::Vector2D(-FLT_MAX,-FLT_MAX);
    Identifier identifier;
  };

}

namespace Muon {

  class MM_RawData;
  class MMPrepData;
  class STGC_RawData;

  class INSWCalibTool : virtual public IAlgTool {
    
  public:  // static methods

    static const InterfaceID& interfaceID()  {return IID_INSWCalibTool;}

  public:  // interface methods
    
    virtual StatusCode calibrateClus(const Muon::MMPrepData* prepRawData, const Amg::Vector3D& globalPos, std::vector<NSWCalib::CalibratedStrip>& calibClus) const = 0;
    virtual StatusCode calibrateStrip(const Muon::MM_RawData* mmRawData, NSWCalib::CalibratedStrip& calibStrip) const = 0;
    virtual StatusCode calibrateStrip(const Identifier id, const double time,  const double charge, const double lorentzAngle, NSWCalib::CalibratedStrip&calibStrip) const = 0;
    virtual StatusCode calibrateStrip(const Muon::STGC_RawData* sTGCRawData, NSWCalib::CalibratedStrip& calibStrip) const = 0;
    virtual StatusCode mmGasProperties(float &vDrift, float &longDiff, float &transDiff, float &interactionDensityMean, float &interactionDensitySigma, TF1* &lorentzAngleFunction) const = 0;
    virtual float peakTime() const = 0;
  };
  
}


#endif
