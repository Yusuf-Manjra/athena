/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_SHOWERSHAPESHISTOGRAMS_H
#define EGAMMAVALIDATION_SHOWERSHAPESHISTOGRAMS_H

#include "GaudiKernel/ITHistSvc.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "TH1.h"
#include "TH2.h"

namespace egammaMonitoring{

  class ShowerShapesHistograms{
  public:

    // Histos
    ShowerShapesHistograms(std::string name,
                   std::string title,
                   std::string folder,
                   ITHistSvc * &rootHistSvc
    ) :
      m_name(name),
      m_title(title),
      m_folder(folder),
      m_rootHistSvc(rootHistSvc) {}

    std::map<std::string, TH1D* > histoMap;
    std::map<std::string, TH2D* > histo2DMap;

    StatusCode initializePlots();
    void fill(const xAOD::Egamma& egamma);

  protected:
    std::string m_name;
    std::string m_title;
    std::string m_folder;
    ITHistSvc*  m_rootHistSvc =  nullptr;

    
  };

}

#endif
