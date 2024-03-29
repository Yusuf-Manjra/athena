/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAU_PANTAUPROCESSOR_H
#define PANTAU_PANTAUPROCESSOR_H

// Gaudi includes
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "tauRecTools/TauRecToolBase.h"
#include "xAODPFlow/PFOContainer.h"

// C++ includes
#include <string>
#include <map>
#include <vector>

// PanTau includes
#include "PanTauAlgs/ITool_InformationStore.h"
#include "PanTauAlgs/ITool_TauConstituentGetter.h"
#include "PanTauAlgs/ITool_TauConstituentSelector.h"
#include "PanTauAlgs/ITool_DetailsArranger.h"
#include "PanTauAlgs/ITool_PanTauTools.h"

namespace PanTau 
{

  /** @class PanTauProcessor

      @author  Peter Wienemann <peter.wienemann@cern.ch>
      @author  Sebastian Fleischmann <Sebastian.Fleischmann@cern.ch>
      @author  Robindra Prabhu <robindra.prabhu@cern.ch>
      @author  Christian Limbach <limbach@physik.uni-bonn.de>
      @author  Peter Wagner <peter.wagner@cern.ch>
      @author  Lara Schildgen <schildgen@physik.uni-bonn.de>
  */  

  class PanTauProcessor : virtual public TauRecToolBase
    {
    public:

       ASG_TOOL_CLASS2( PanTauProcessor, TauRecToolBase, ITauToolBase )

       PanTauProcessor(const std::string& name);
       ~PanTauProcessor();

       virtual StatusCode initialize();
       virtual StatusCode finalize();
       virtual StatusCode executePanTau(xAOD::TauJet& pTau, xAOD::ParticleContainer& pi0Container, xAOD::PFOContainer& neutralPFOContainer) const;
       
    private:
        
        std::string                                         m_Name_InputAlg;
        
        //Tools used in seed building
        ToolHandle<PanTau::ITool_InformationStore>          m_Tool_InformationStore;
        ToolHandle<PanTau::ITool_TauConstituentGetter>      m_Tool_TauConstituentGetter;
        ToolHandle<PanTau::ITool_TauConstituentSelector>    m_Tool_TauConstituentSelector;
        ToolHandle<PanTau::ITool_PanTauTools>               m_Tool_FeatureExtractor;
        
        //Tools used in seed finalizing
        ToolHandle<PanTau::ITool_PanTauTools>               m_Tool_DecayModeDeterminator;
        ToolHandle<PanTau::ITool_DetailsArranger>           m_Tool_DetailsArranger;

        //Tools used in seed building
        std::string          m_Tool_InformationStoreName;
        std::string          m_Tool_TauConstituentGetterName;
        std::string          m_Tool_TauConstituentSelectorName;
        std::string          m_Tool_FeatureExtractorName;
        
        //Tools used in seed finalizing
        std::string               m_Tool_DecayModeDeterminatorName;
        std::string               m_Tool_DetailsArrangerName;
        
        std::vector<double>                                 m_Config_PtBins;
        double                                              m_Config_MinPt;
        double                                              m_Config_MaxPt;
        
        void                                                fillDefaultValuesToTau(xAOD::TauJet* tauJet) const;
        
        
    }; //end class
} // end of namespace

#endif 
