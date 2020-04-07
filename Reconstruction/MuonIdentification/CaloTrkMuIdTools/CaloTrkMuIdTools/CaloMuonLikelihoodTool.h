/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRKMUIDTOOLS_CALOMUONLIKELIHOODTOOL_H
#define CALOTRKMUIDTOOLS_CALOMUONLIKELIHOODTOOL_H

#include "ICaloTrkMuIdTools/ICaloMuonLikelihoodTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "CaloEvent/CaloClusterContainer.h"

#include <vector>

class TH1F;

/** @class CaloMuonLikelihoodTool
        
    Compares track energy deposition ratios found by CaloTopoClusters to
    single muon and single pion distributions to build a likelihood ratio discriminant.

    @author Luis.Flores.Castillo@cern.ch
*/
class CaloMuonLikelihoodTool : public AthAlgTool, virtual public ICaloMuonLikelihoodTool {
public:
  CaloMuonLikelihoodTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~CaloMuonLikelihoodTool()=default;

  virtual StatusCode initialize();
  
  double getLHR(const xAOD::TrackParticle* trk, const xAOD::CaloClusterContainer* ClusCollection=nullptr, const double dR_CUT=0.3) const;
  double getLHR(const xAOD::CaloClusterContainer* ClusCollection, const double eta_trk, const double p_trk, const double eta_trkAtCalo, const double phi_trkAtCalo, const double dR_CUT=0.3) const;

private:
  StatusCode        retrieveHistograms();

  const TH1F*       m_TH1F_sig[9][11];
  const TH1F*       m_TH1F_bkg[9][11];
  std::string       m_TH1F_key[9][11];
  int               m_numKeys[9];
  mutable std::atomic_int m_cnt_warn{0};

  ToolHandle <Trk::IParticleCaloExtensionTool> m_caloExtensionTool{this, "ParticleCaloExtensionTool", ""};
  std::vector<std::string> m_fileNames;
};

#endif
