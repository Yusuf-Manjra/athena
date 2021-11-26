/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoCommon/Types.h"
#include "L1TopoCommon/Exception.h"

// Configuration
std::string
TCS::inputTypeAsString(TCS::inputTOBType_t type) {
  if(type == TCS::CLUSTER) return "Clusters";
  else if(type == TCS::TAU) return "Taus";
  else if(type == TCS::EEM) return "eEms";
  else if(type == TCS::JEM) return "jEms";
  else if(type == TCS::JET) return "Jets";
  else if(type == TCS::ETAU) return "eTaus";
  else if(type == TCS::JTAU) return "jTaus";
  else if(type == TCS::CTAU) return "cTaus";
  else if(type == TCS::JLARGERJET) return "jLargeRJets";
  else if(type == TCS::GLARGERJET) return "gLargeRJets";
  else if(type == TCS::JJET) return "jJets";
  else if(type == TCS::GJET) return "gJets";
  else if(type == TCS::MET) return "MET";
  else if(type == TCS::MUON) return "Muons";
  else if(type == TCS::MUONNEXTBC) return "MuonsNextBC";
  else if(type == TCS::LATEMUON) return "LateMuons";
  else return "None";
}


TCS::inputTOBType_t
TCS::inputType(const std::string& input) {

   if ( input == "Clusters" || input == "ClusterTobArray" || input == "EmTobArray" )
      return TCS::CLUSTER;

   if ( input == "eEM" || input == "eEMTobArray" || input == "eEmTobs" )
      return TCS::EEM;

   if ( input == "jEM" || input == "jEMTobArray" || input == "jEmTobs" )
      return TCS::JEM;

   if ( input == "jEM" || input == "jEMTobArray" || input == "jEmTobs" )
      return TCS::CLUSTER;

   if ( input == "eTAU" || input == "eTauTobArray" || input == "eTauTobs" )
      return TCS::ETAU;

   if ( input == "jTau" || input == "jTauTobArray" || input == "jTauTobs" )
      return TCS::JTAU;

   if ( input == "cTau" || input == "cTauTobArray" || input == "cTauTobs" || input == "cTAU" )
      return TCS::CTAU;

   if ( input == "Jets" || input == "JetTobArray" )
      return TCS::JET;

   if ( input == "jJ" || input == "jJetTobArray" || input == "jJetTobs" )
      return TCS::JJET;

   if ( input == "gJ" || input == "gJetTobArray" || input == "gJetTobs" )
      return TCS::GJET;

   if ( input == "jLargeRJet" || input == "jLargeRJetTobArray" || input == "jLargeRJetTobs" || input ==  "jLJet" || input == "jLJ")
      return TCS::JLARGERJET;

   if ( input == "gLargeRJet" || input == "gLargeRJetTobArray" || input == "gLargeRJetTobs" )
      return TCS::GLARGERJET;

   if ( input == "Muons" || input == "MuonTobArray" || input == "MuonTobs")
      return TCS::MUON;

   if ( input == "MuonsNextBC" || input == "MuonNextBCTobArray" )
      return TCS::MUONNEXTBC;
          
   if ( input == "LateMuons" || input == "LateMuonTobArray" || input == "LateMuonTobs")
      return TCS::LATEMUON;

   if ( input == "MuonsNextBC" || input == "MuonNextBCTobArray" )
      return TCS::MUONNEXTBC;
          
   if ( input == "Taus" || input == "TauTobArray" )
      return TCS::TAU;
          
   if ( input == "MET" || input == "MetTobArray" || input == "MetTobs" )
      return TCS::MET;
          
   if ( input == "SumEt" || input == "SumEtTobArray" || input == "SumEtTobs" )
      return TCS::MET;
          

   TCS_EXCEPTION("L1TopoCommon: unknown input type " + input);
   
   return TCS::NONE;
}
