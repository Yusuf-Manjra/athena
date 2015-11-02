/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// KinematicSimSelector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_KINEMATICSIMSELECTOR_H
#define ISF_TOOLS_KINEMATICSIMSELECTOR_H 1

// ISF includes
#include "ISF_Event/KinematicParticleCuts.h"
#include "ISF_Interfaces/ISimulationSelector.h"

namespace ISF {

  /** @class KinematicSimSelector
  
      Simplistic kinematic filter on energy and pseudorapidity.
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class KinematicSimSelector : public ISimulationSelector, public KinematicParticleCuts {
      
    public: 
     /** Constructor with parameters */
     KinematicSimSelector( const std::string& t, const std::string& n, const IInterface* p );

     /** Destructor */
     ~KinematicSimSelector();

     // Athena algtool's Hooks
     virtual StatusCode  initialize();
     virtual StatusCode  finalize();
     
     /** check whether given particle passes all cuts -> will be used for routing decision*/
     virtual bool passSelectorCuts(const ISFParticle& particle) const;
  }; 
  
}

#endif //> !ISF_TOOLS_KINEMATICSIMSELECTOR_H
