/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CLUSTERMOMENTPLOTS_H
#define CLUSTERMOMENTPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace PFO {

  class ClusterMomentPlots : public PlotBase {

  public:

     ClusterMomentPlots(PlotBase *pParent, std::string sDir, SG::ReadHandleKey<xAOD::CaloClusterContainer>& sClusterContainerName);

     void fill(const xAOD::CaloCluster& cluster, const xAOD::EventInfo& eventInfo);

  private:

     TH1* m_LATERAL;
     TH1* m_LONGITUDINAL;
     TH1* m_SECOND_R;
     TH1* m_CENTER_LAMBDA;
     TH1* m_FIRST_ENG_DENS;
     TH1* m_ENG_FRAC_MAX;
     TH1* m_ISOLATION;
     TH1* m_ENG_BAD_CELLS;
     TH1* m_N_BAD_CELLS;
     TH1* m_BADLARQ_FRAC;
     TH1* m_ENG_POS;
     TH1* m_SIGNIFICANCE;
     TH1* m_AVG_LAR_Q;
     TH1* m_AVG_TILE_Q;
     TH1* m_CELL_SIG_SAMPLING;
     TH1* m_ENG_BAD_HV_CELLS;
     TH1* m_N_BAD_HV_CELLS;
     TH1* m_EM_PROBABILITY;

     void initializePlots();
     SG::ReadHandleKey<xAOD::CaloClusterContainer> m_sClusterContainerName;

  };

}
#endif
