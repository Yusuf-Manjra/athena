/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 766963 2016-08-08 20:07:09Z ggach $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODForward/versions/ALFADataContainer_v1.h"
#include "xAODForward/versions/AFPDataContainer_v1.h"
#include "xAODForward/versions/AFPSiHitContainer_v1.h"
#include "xAODForward/versions/AFPTrackContainer_v1.h"
#include "xAODForward/versions/ZdcModuleContainer_v1.h"
#include "xAODForward/versions/MBTSModuleContainer_v1.h"
#include "xAODForward/versions/ForwardEventInfoContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, ALFADataContainer_v1 );
ADD_NS_DV_PROXY( xAOD, AFPDataContainer_v1 );
ADD_NS_DV_PROXY( xAOD, AFPSiHitContainer_v1 );
ADD_NS_DV_PROXY( xAOD, AFPTrackContainer_v1 );
ADD_NS_DV_PROXY( xAOD, ZdcModuleContainer_v1 );
ADD_NS_DV_PROXY( xAOD, MBTSModuleContainer_v1 );
ADD_NS_DV_PROXY( xAOD, ForwardEventInfoContainer_v1 );
