/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INSWCalibTool_h
#define INSWCalibTool_h

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"

#include <cmath>
#include <vector>

#include "TF1.h"


static const InterfaceID IID_INSWCalibTool("Muon::INSWCalibTool",1,0);

namespace NSWCalib { 

  struct CalibratedStrip {
    double charge;
    double time;
    double resTime;
    double distDrift;
    double resTransDistDrift;
    double resLongDistDrift;
    double dx;
    Identifier identifier;
  };

}


namespace Muon {

  class MM_RawData;

  class INSWCalibTool : virtual public IAlgTool {
    
  public:  // static methods

    static const InterfaceID& interfaceID()  {return IID_INSWCalibTool;}

  public:  // interface methods
    
    virtual StatusCode calibrate(const Muon::MM_RawData* mmRawData, const Amg::Vector3D& globalPos, NSWCalib::CalibratedStrip& calibStrip) const = 0;

    virtual StatusCode mmGasProperties(float &vDrift, float &longDiff, float &transDiff, float &interactionDensityMean, float &interactionDensitySigma, TF1* &lorentzAngleFunction) const = 0;
  };
  
}


#endif
