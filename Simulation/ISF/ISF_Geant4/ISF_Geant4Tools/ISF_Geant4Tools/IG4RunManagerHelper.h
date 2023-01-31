/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// IG4RunManagerHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4TOOLS_IG4RUNMANAGERHELPER_H
#define ISF_GEANT4TOOLS_IG4RUNMANAGERHELPER_H 

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "CxxUtils/checker_macros.h"
#include "G4AtlasAlg/G4AtlasRunManager.h"

namespace ISF {

  /**
   @class IG4RunManagerHelper
       
   @author sarka.todorova -at- cern.ch
   */
     
  class IG4RunManagerHelper : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IG4RunManagerHelper(){}

       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(IG4RunManagerHelper, 1, 0);
       
       /** get the fully configured G4RunManager */
       virtual G4AtlasRunManager* g4RunManager ATLAS_NOT_THREAD_SAFE () = 0;

       /** get the light version of G4RunManager */
       virtual G4RunManager* fastG4RunManager ATLAS_NOT_THREAD_SAFE () = 0;
  };

} // end of namespace

#endif // ISF_GEANT4TOOLS_IG4RUNMANAGERHELPERTOOL_H
