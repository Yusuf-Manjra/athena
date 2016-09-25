/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Reco_mumu.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_Reco_mumu_H
#define DERIVATIONFRAMEWORK_Reco_mumu_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "JpsiUpsilonTools/JpsiFinder.h"
#include "JpsiUpsilonTools/PrimaryVertexRefitter.h"

/** forward declarations
 */
namespace Trk {
  class V0Tools;
}

/** THE reconstruction tool
 */
namespace DerivationFramework {

  class Reco_mumu : public AthAlgTool, public IAugmentationTool {
    public: 
      Reco_mumu(const std::string& t, const std::string& n, const IInterface* p);

      StatusCode initialize();
      StatusCode finalize();
      
      virtual StatusCode addBranches() const;
      
    private:
      /** tools
       */
      ToolHandle<Trk::V0Tools>                    m_v0Tools;
      ToolHandle<Analysis::JpsiFinder>            m_jpsiFinder;
      ToolHandle<Analysis::PrimaryVertexRefitter> m_pvRefitter;
      
      /** job options
       */
      std::string m_outputVtxContainerName;
      std::string m_pvContainerName;
      std::string m_refPVContainerName;
      bool        m_refitPV;
      int m_PV_max;
      int m_DoVertexType;
  }; 
}

#endif // DERIVATIONFRAMEWORK_Reco_mumu_H
