// Dear Emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EGAMMAANALYSISINTERFACES_IEGAMMAMVASVC_H
#define EGAMMAANALYSISINTERFACES_IEGAMMAMVASVC_H

#include "AsgServices/IAsgService.h"
#include "AsgMessaging/StatusCode.h"
// EDM includes
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODCaloEvent/CaloClusterFwd.h"


class IegammaMVASvc : virtual public asg::IAsgService {
  ASG_SERVICE_INTERFACE(IegammaMVASvc)
public:

  virtual ~IegammaMVASvc() override {};

  /// Standard execute when you have both cluster and eg object
  virtual StatusCode execute(xAOD::CaloCluster& cluster,
			     const xAOD::Egamma& eg) const = 0;

  /// An execute that just has cluster and egType. A converted photon
  /// is treated like an unconverted photon since no access to vertex.
  virtual StatusCode execute(xAOD::CaloCluster& cluster,
			     const xAOD::EgammaParameters::EgammaType egType) const = 0;


};

#endif
