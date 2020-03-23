/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_MUONOBJECTCOLLECTIONMAKER_H
#define ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_MUONOBJECTCOLLECTIONMAKER_H

/**
  * @author John Morris <john.morris@cern.ch>
  * 
  * @brief MuonObjectCollectionMaker
  *   Makes all systematic variations of muons.
  *   Momentum Corrections are applied using the calibration tool.
  *   Efficiency Corrections are applied using the efficiency tool.
  * 
  * $Revision: 799839 $
  * $Date: 2017-03-08 12:07:28 +0100 (Wed, 08 Mar 2017) $
  * 
  **/ 

// system include(s):
#include <memory>
#include <set>
#include <list>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

// CP Tool include(s):
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"
// the following is needed to make sure all muons for which d0sig is calculated are at least Loose
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"

// Forward declaration(s):
namespace top{
  class TopConfig;
}

namespace top{

  class MuonObjectCollectionMaker final : public asg::AsgTool {
    public:
      explicit MuonObjectCollectionMaker( const std::string& name );
      virtual ~MuonObjectCollectionMaker(){}

      // Delete Standard constructors
      MuonObjectCollectionMaker(const MuonObjectCollectionMaker& rhs) = delete;
      MuonObjectCollectionMaker(MuonObjectCollectionMaker&& rhs) = delete;
      MuonObjectCollectionMaker& operator=(const MuonObjectCollectionMaker& rhs) = delete;

      StatusCode initialize();
      StatusCode execute();

      StatusCode printout();

      // return specific Systematic
      inline virtual const std::list<CP::SystematicSet>& specifiedSystematics() const {return m_specifiedSystematics;}
  
      // return all recommendedSystematics
      inline const std::list<CP::SystematicSet>& recommendedSystematics() const {return m_recommendedSystematics;}

    protected:
      // specify Systematic
      virtual void specifiedSystematics( const std::set<std::string>& specifiedSystematics );

    private:
      std::shared_ptr<top::TopConfig> m_config;

      std::list<CP::SystematicSet> m_specifiedSystematics;
      std::list<CP::SystematicSet> m_recommendedSystematics;

      ToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calibrationTool;
      
      ///-- Isolation --///
      ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_LooseTrackOnly;
      ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_Loose;
      ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_Gradient;
      ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_GradientLoose;      
      ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_FixedCutTight;
      ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_FixedCutTightTrackOnly;
      ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_FixedCutLoose;
      // the following is needed to make sure all muons for which d0sig is calculated are at least Loose
      ToolHandle<CP::IMuonSelectionTool> m_muonSelectionToolVeryLooseVeto;

  };
} // namespace
#endif
