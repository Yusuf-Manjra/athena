/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_ISOLATIONHISTOGRAMS_H
#define EGAMMAVALIDATION_ISOLATIONHISTOGRAMS_H

#include "xAODEgamma/Egamma.h"

#include <map>
#include <string>
#include <utility>

class ITHistSvc;
class TH1D;



namespace egammaMonitoring{

  class IsolationHistograms{
  public:

    // Histos
    IsolationHistograms(std::string name,
                        std::string title,
                        std::string folder,
                        ITHistSvc * &rootHistSvc
    ) :
      m_name(std::move(std::move(name))),
      m_title(std::move(std::move(title))),
      m_folder(std::move(std::move(folder))),
      m_rootHistSvc(rootHistSvc) {}

    std::map<std::string, TH1D* > histoMap;
    StatusCode initializePlots();
    void fill(const xAOD::Egamma& egamma);

  protected:

    std::string m_name;
    std::string m_title;
    std::string m_folder;
    ITHistSvc*  m_rootHistSvc =  nullptr;

    float m_ptCone20{}, m_ptCone30{}, m_ptCone40{}, m_ptVarCone20{};
    float m_ptVarCone30{}, m_ptVarCone40{}, m_etCone20{}, m_etCone30{};
    float m_etCone40{}, m_topoEtCone20{}, m_topoEtCone30{}, m_topoEtCone40{};

  };

}

#endif
