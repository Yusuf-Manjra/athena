// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMRoICnvAlg.h 576328 2013-12-19 16:09:32Z morrisj $
#ifndef XAODTRIGL1CALOCNV_CPMROICNVALG_H
#define XAODTRIGL1CALOCNV_CPMROICNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/ICPMRoICnvTool.h"

namespace xAODMaker {
   /**
    *  @short Algorithm for creating xAOD::CPMRoI from an ESD
    *
    *         This algorithm can be used to create an xAOD CPMRoI container
    *         out of the objects found in an ESD file.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576328 $
    * $Date: 2013-12-19 17:09:32 +0100 (Thu, 19 Dec 2013) $
    */
   class CPMRoICnvAlg : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
      CPMRoICnvAlg( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      /// StoreGate key of the input container
      std::string m_esdKey;
      /// StoreGate key for the output container
      std::string m_xaodKey;

      /// Handle to the converter tool
      ToolHandle< ICPMRoICnvTool > m_cnvTool;

   }; // class EmTauRoICnvAlg

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_CPMROICNVALG_H
