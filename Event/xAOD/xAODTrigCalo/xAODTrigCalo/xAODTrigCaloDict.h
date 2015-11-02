// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigCaloDict.h 631117 2014-11-26 08:48:22Z gwatts $
#ifndef XAODTRIGCALO_XAODTRIGCALODICT_H
#define XAODTRIGCALO_XAODTRIGCALODICT_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODTrigCalo/TrigCaloClusterContainer.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"
#include "xAODTrigCalo/versions/TrigCaloCluster_v1.h"
#include "xAODTrigCalo/versions/TrigCaloClusterContainer_v1.h"
#include "xAODTrigCalo/versions/TrigCaloClusterAuxContainer_v1.h"
#include "xAODTrigCalo/versions/TrigEMCluster_v1.h"
#include "xAODTrigCalo/versions/TrigEMClusterContainer_v1.h"
#include "xAODTrigCalo/versions/TrigEMClusterAuxContainer_v1.h"

namespace{
  struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGCALO {

    xAOD::TrigCaloClusterContainer_v1 c3;
    ElementLink< xAOD::TrigCaloClusterContainer_v1 > a1;
    std::vector< ElementLink< xAOD::TrigCaloClusterContainer_v1 > > a3;
    std::vector< std::vector< ElementLink< xAOD::TrigCaloClusterContainer_v1 > > > a4;

    xAOD::TrigEMClusterContainer_v1 c4;
    ElementLink< xAOD::TrigEMClusterContainer_v1 > b1;
    std::vector< ElementLink< xAOD::TrigEMClusterContainer_v1 > > b3;
    std::vector< std::vector< ElementLink< xAOD::TrigEMClusterContainer_v1 > > > b4;
  };
}

#endif // XAODTRIGCALO_XAODTRIGCALODICT_H
