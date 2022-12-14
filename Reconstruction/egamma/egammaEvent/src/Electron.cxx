/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaEvent/Electron.h"
#include "AthenaKernel/BaseInfo.h"


namespace Analysis {


Electron::~Electron()
= default;


Electron& Electron::operator=(const Electron& rhs) 
{ 
  if ( this != &rhs ) {
    egamma::operator=( rhs ); 
  }
  return *this;
}

/** @brief create photon from egamma*/
void Electron::makeElectronFromEgamma(const egamma& eg)
{
  if ( this != &eg ) {
    egamma::operator=(eg);
  }
}


} // namespace Analysis


SG_ADD_BASE (Analysis::Electron, SG_VIRTUAL (IAthenaBarCode));
