/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

#include "TrigMuonEvent/IsoMuonFeature.h"

//////////////////////////////////////////////////////////////////
int testEqual_IsoMuonFeature() {
  MsgStream log(Athena::getMessageSvc(), "testEqual_IsoMuonFeature");  
  log << MSG::INFO << "testing identical IsoMuonFeatures" << endmsg;

  IsoMuonFeature a, b;
  a.setRoiMu(1,10000.,1.,3.14,1.,1);
  b.setRoiMu(1,10000.,1.,3.14,1.,1);
  a.setCaloInfos(0.0,4000.,5000.,6000.);
  b.setCaloInfos(0.0,4000.,5000.,6000.);
  a.setTrackInfos(1000.,4000.,5000.,7000.,10000.,2000.);
  b.setTrackInfos(1000.,4000.,5000.,7000.,10000.,2000.);

  if ( !(a == b) ) {
    log << MSG::ERROR << "Identical IsoMuonFeatures found distinct! " << endmsg;
    return -1;
  }


  log << MSG::INFO << "testing distinct IsoMuonFeatures" << endmsg;

  IsoMuonFeature c;
  c.setRoiMu(2,14000.,-1.,3.14,1.,1);
  c.setCaloInfos(0.0,3000.,3500.,4000.);
  c.setTrackInfos(1000.,1500.,1900.,3000.,14000.,0.0);

  if ( a == c ) {
    log << MSG::ERROR << "Different IsoMuonFeatures found same! " << endmsg;
    log << MSG::ERROR << a << endmsg;
    log << MSG::ERROR << c << endmsg;
    return -1;
  }

  return 0;
}

//////////////////////////////////////////////////////////////////
int testPrint_IsoMuonFeature() {
  IsoMuonFeature a,b;
  a.setRoiMu(2,14000.,-1.,3.14,1.,1);
  a.setCaloInfos(0.0,3000.,3500.,4000.);
  a.setTrackInfos(1000.,1500.,1900.,3000.,14000.,0.0);
  
  MsgStream log(Athena::getMessageSvc(), "testPrint_IsoMuonFeature");
  log << MSG::INFO << "MSG a " << a << endmsg;
  log << MSG::INFO << "MSG b " << b << endmsg;
  return 0;
}

int main() {
  MsgStream log(Athena::getMessageSvc(), "Operators_test");

  // IsoMuonFeature
  if ( testEqual_IsoMuonFeature() != 0 ) {
    log << MSG::ERROR << "IsoMuonFeature == operator is wrong" << endmsg;
    return -1;
  }
  log << MSG::INFO << "IsoMuonFeature == operator is correct " << endmsg;    
  if ( testPrint_IsoMuonFeature() != 0 ) {
    log << MSG::ERROR << "IsoMuonFeature << operator is wrong" << endmsg;
    return -1;
    
  }
  log << MSG::INFO << "IsoMuonFeature << operator is correct " << endmsg;    

  return 0;
}
