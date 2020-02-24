/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_energycorrelatorgeneralizedtool_header
#define jetsubstructuremomenttools_energycorrelatorgeneralizedtool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetSubStructureMomentTools/ECFHelper.h"

class EnergyCorrelatorGeneralizedTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(EnergyCorrelatorGeneralizedTool, IJetModifier)
    
    public:
      // Constructor and destructor
      EnergyCorrelatorGeneralizedTool(std::string name);

      virtual StatusCode initialize() override;

      int modifyJet(xAOD::Jet &injet) const override;

    private:
      float m_Beta;
      bool m_doN3;
      bool m_doLSeries;
      std::vector<float> m_betaVals;
      std::vector<float> m_cleaned_betaVals; // Local vector for cleaned up inputs
      bool m_doDichroic;

};

#endif
