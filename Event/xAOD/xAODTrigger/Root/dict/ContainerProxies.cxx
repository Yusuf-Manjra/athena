/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 635056 2014-12-10 15:35:39Z watsona $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODTrigger/versions/TriggerMenuContainer_v1.h"

#include "xAODTrigger/versions/EmTauRoIContainer_v1.h"
#include "xAODTrigger/versions/EmTauRoIContainer_v2.h"
#include "xAODTrigger/versions/JetRoIContainer_v1.h"
#include "xAODTrigger/versions/JetRoIContainer_v2.h"
#include "xAODTrigger/versions/MuonRoIContainer_v1.h"

#include "xAODTrigger/versions/TrigCompositeContainer_v1.h"

#include "xAODTrigger/versions/BunchConfContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, TriggerMenuContainer_v1 );

ADD_NS_DV_PROXY( xAOD, EmTauRoIContainer_v1 );
ADD_NS_DV_PROXY( xAOD, EmTauRoIContainer_v2 );
ADD_NS_DV_PROXY( xAOD, JetRoIContainer_v1 );
ADD_NS_DV_PROXY( xAOD, JetRoIContainer_v2 );
ADD_NS_DV_PROXY( xAOD, MuonRoIContainer_v1 );

ADD_NS_DV_PROXY( xAOD, TrigCompositeContainer_v1 );

ADD_NS_DV_PROXY( xAOD, BunchConfContainer_v1 );
