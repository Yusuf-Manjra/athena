/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_RECOINFOPLOTS_H
#define TRKVALHISTUTILS_RECOINFOPLOTS_H

#include "PlotBase.h"
#include "xAODTracking/TrackParticle.h"

namespace Trk{

class RecoInfoPlots: public PlotBase {
  public:
    RecoInfoPlots(PlotBase *pParent, std::string sDir):PlotBase(pParent, sDir){ init();}
    void fill(const xAOD::TrackParticle& trkprt);
 		
    TH1* trackfitchi2;
    TH1* trackfitndof;
    TH1* trackcon;
    TH1* trackchi2prob;
    
  private:
    void init();
    void initializePlots();
			
};

}

#endif

