/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructuremomenttools_energycorrelatortool_header
#define jetsubstructuremomenttools_energycorrelatortool_header

#include "JetSubStructureMomentTools/JetSubStructureMomentToolsBase.h"
#include "JetSubStructureMomentTools/ECFHelper.h"

class EnergyCorrelatorTool :
  public JetSubStructureMomentToolsBase {
    ASG_TOOL_CLASS(EnergyCorrelatorTool, IJetModifier)
    
    public:
      // Constructor and destructor
      EnergyCorrelatorTool(std::string name);
     
      virtual StatusCode initialize() override;

      int modifyJet(xAOD::Jet &injet) const override;

    private:
      float m_Beta;
      bool m_doC3;
      std::vector<float> m_betaVals;
      std::vector<float> m_cleaned_betaVals; // Local vector for cleaned up inputs
      bool m_doDichroic;

};

#endif
