/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./EMTauInputProviderFEX.h"

#include <math.h>
#include "TH1.h"
#include "TH2.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"
#include "TrigT1CaloEvent/CPCMXTopoData.h"

#include "TrigT1Interfaces/CPRoIDecoder.h"

#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/TopoInputEvent.h"


using namespace std;
using namespace LVL1;

EMTauInputProviderFEX::EMTauInputProviderFEX(const std::string& type, const std::string& name, 
                                       const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name),
   m_eFEXClusterLoc ( "SClusterCl")
{
   declareInterface<LVL1::IInputTOBConverter>( this );
   declareProperty( "eFEXClusterInput", m_eFEXClusterLoc, "StoreGate location of eFEX Cluster inputs" );
}

EMTauInputProviderFEX::~EMTauInputProviderFEX()
{}

StatusCode
EMTauInputProviderFEX::initialize() {

   CHECK(m_histSvc.retrieve());

   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "EnergyInputProviderFEX");
   CHECK(incidentSvc.retrieve());
   incidentSvc->addListener(this,"BeginRun", 100);
   incidentSvc.release().ignore();

   return StatusCode::SUCCESS;
}


void
EMTauInputProviderFEX::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   m_hEMEt = new TH1I( "EMTOBEt", "EM TOB Et", 40, 0, 200);
   m_hEMEt->SetXTitle("E_{T}");
   m_hEMEtaPhi = new TH2I( "EMTOBPhiEta", "EM TOB Location", 25, -50, 50, 64, 0, 64);
   m_hEMEtaPhi->SetXTitle("#eta");
   m_hEMEtaPhi->SetYTitle("#phi");

   m_hTauEt = new TH1I( "TauTOBEt", "Tau TOB Et", 40, 0, 200);
   m_hTauEt->SetXTitle("E_{T}");
   m_hTauEtaPhi = new TH2I( "TauTOBPhiEta", "Tau TOB Location", 25, -50, 50, 64, 0, 64);
   m_hTauEtaPhi->SetXTitle("#eta");
   m_hTauEtaPhi->SetYTitle("#phi");

   m_histSvc->regHist( histPath + "EMTOBEt", m_hEMEt ).ignore();
   m_histSvc->regHist( histPath + "EMTOBPhiEta", m_hEMEtaPhi ).ignore();
   m_histSvc->regHist( histPath + "TauTOBEt", m_hTauEt ).ignore();
   m_histSvc->regHist( histPath + "TauTOBPhiEta", m_hTauEtaPhi ).ignore();
}



StatusCode
EMTauInputProviderFEX::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {
  
  const xAOD::TrigEMClusterContainer* eFEXCluster = nullptr; // cluster from eFEX
  if(  evtStore()->contains< xAOD::TrigEMClusterContainer >(m_eFEXClusterLoc)  ) {
    CHECK ( evtStore()->retrieve( eFEXCluster, m_eFEXClusterLoc ) );
    ATH_MSG_DEBUG( "Retrieved eFEX em cluster container '" << m_eFEXClusterLoc << "' with size " << eFEXCluster->size());
  }
  else {
    ATH_MSG_WARNING("No xAOD::TrigEMClusterContainer with SG key '" << m_eFEXClusterLoc.toString() << "' found in the event. No EM cluster input for the L1Topo simulation.");
    return StatusCode::RECOVERABLE;
  }

  std::vector< CPTopoTOB > tobs;
  for( const auto & cl : *eFEXCluster ) {

    ATH_MSG_DEBUG( "EMTAU TOB with c" 
		   //removing the "isolaion for now in printouts
		   << " : e = "   << setw(3) << cl->et()   
		   << ", eta = "  << setw(2) << cl->eta()  << ", phi = "     << cl->phi()
		   << ", word = " << cl->RoIword() 
		   );
    
    //Check if this is a tau candidate, based upon https://gitlab.cern.ch/l1calo-run3-offline/r3athanalysis/blob/master/source/NtupleDumper/src/NtupleDumperAlg.cxx#L315-338
    //This is not what is done in the CTP emulation https://gitlab.cern.ch/atlas/athena/blob/21.3/Trigger/TrigT1/TrigT1CTP/src/CTPEmulation.h, and metioned in twiki https://twiki.cern.ch/twiki/bin/viewauth/Atlas/L1CaloUpgradeSimulation (to be cross checked with experts)
    bool is_a_tau = cl->et()>=5 && cl->auxdecor<int>("PassRun3wstot") && cl->auxdecor<int>("PassRun3RHad") && cl->auxdecor<int>("PassRun3REta");
    //eventually need to implement two different definition of isolation for EM clusters and taus according to the twiki above
    TCS::ClusterTOB cluster( (unsigned int)cl->et(), (unsigned int)0, (float)cl->eta(), (float)cl->phi(), is_a_tau ? TCS::TAU : TCS::CLUSTER , (long int)cl->RoIword() );
    cluster.setEtaDouble( cl->eta() );
    cluster.setPhiDouble( cl->phi() );
    
    if( is_a_tau ) inputEvent.addTau( cluster );
    else  inputEvent.addCluster( cluster );

    //Fill the histograms here
  }

  /*
  if(is_overflow){
    inputEvent.setOverflowFromEmtauInput(true);
    ATH_MSG_DEBUG("setOverflowFromEmtauInput : true");
  }
  */

  return StatusCode::SUCCESS;

}
 

void 
EMTauInputProviderFEX::CalculateCoordinates(int32_t roiWord, double & eta, double & phi) const {
   CPRoIDecoder get;
   double TwoPI = 2 * M_PI;
   CoordinateRange coordRange = get.coordinate( roiWord );
   
   eta = coordRange.eta();
   phi = coordRange.phi();
   if( phi > M_PI ) phi -= TwoPI;
}
