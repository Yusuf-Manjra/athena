// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "./EMTauInputProviderFEX.h"

#include <math.h>

#include "GaudiKernel/ITHistSvc.h"

#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"
#include "TrigT1CaloEvent/CPCMXTopoData.h"

#include "TrigT1Interfaces/CPRoIDecoder.h"

#include "L1TopoEvent/eEmTOB.h"
#include "L1TopoEvent/TopoInputEvent.h"

#include "GaudiKernel/PhysicalConstants.h"

using namespace std;
using namespace LVL1;

EMTauInputProviderFEX::EMTauInputProviderFEX(const std::string& type, const std::string& name, 
                                       const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name)
{
   declareInterface<LVL1::IInputTOBConverter>( this );
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

   auto is_eEM_EDMvalid = m_eEM_EDMKey.initialize();

   //Temporarily check EDM status by hand to avoid the crash!
   if (is_eEM_EDMvalid != StatusCode::SUCCESS) {
     ATH_MSG_WARNING("No EDM found for eFEX..");
   }

   auto is_eTau_EDMvalid = m_eTau_EDMKey.initialize();

   //Temporarily check EDM status by hand to avoid the crash!
   if (is_eTau_EDMvalid != StatusCode::SUCCESS) {
     ATH_MSG_WARNING("No EDM found for eFEX..");
   }

   return StatusCode::SUCCESS;
}


void
EMTauInputProviderFEX::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   auto hEMEt = std::make_unique<TH1I>( "eEMTOBEt", "eEm TOB Et", 400, 0, 400);
   hEMEt->SetXTitle("E_{T}");
   auto hEMEtaPhi = std::make_unique<TH2I>( "eEMTOBPhiEta", "eEm TOB Location", 400, -200, 200, 128, 0, 128);
   hEMEtaPhi->SetXTitle("#eta");
   hEMEtaPhi->SetYTitle("#phi");
   auto hEMEtaPhi_local = std::make_unique<TH2I>( "eEMTOBEtEta_local", "eEm TOB local eta vs phi", 400, -200, 200, 128, 0, 128);
   hEMEtaPhi_local->SetXTitle("#eta");
   hEMEtaPhi_local->SetYTitle("#phi");
   auto hEMEtEta = std::make_unique<TH2I>( "eEMTOBEtEta", "eEm TOB Et vs eta", 40, 0, 200, 256, -128, 128);
   hEMEtEta->SetXTitle("E_{t}");
   hEMEtEta->SetYTitle("#eta");
   auto hEMEtPhi = std::make_unique<TH2I>( "eEMTOBEtPhi", "eEm TOB Et vs phi", 40, 0, 200, 128, 0, 128);
   hEMEtPhi->SetXTitle("E_{t}");
   hEMEtPhi->SetYTitle("#phi");
   auto hTauEt = std::make_unique<TH1I>( "eTauTOBEt", "eTau TOB Et", 400, 0, 400);
   hTauEt->SetXTitle("E_{T}");
   auto hTauIsolation = std::make_unique<TH1I>( "eTauIsolation", "eTau TOB isolation", 20, 0, 20);
   hTauIsolation->SetXTitle("fCore isolation");
   auto hTauEtaPhi = std::make_unique<TH2I>( "eTauTOBPhiEta", "eTau TOB Location", 400, -200, 200, 128, 0, 128);
   hTauEtaPhi->SetXTitle("#eta");
   hTauEtaPhi->SetYTitle("#phi");
   auto hTauEtEta = std::make_unique<TH2I>( "eTauTOBEtEta", "eTau TOB Et vs eta", 40, 0, 200, 256, -128, 128);
   hTauEtEta->SetXTitle("E_{t}");
   hTauEtEta->SetYTitle("#eta");
   auto hTauEtPhi = std::make_unique<TH2I>( "eTauTOBEtPhi", "eTau TOB Et vs phi", 40, 0, 200, 128, 0, 128);
   hTauEtPhi->SetXTitle("E_{t}");
   hTauEtPhi->SetYTitle("#phi");
   auto hTauEtIsolation = std::make_unique<TH2I>( "eTauTOBEtIsolation", "eTau TOB Et vs Isolation", 40, 0, 200, 20, 0, 20);
   hTauEtIsolation->SetXTitle("E_{t}");
   hTauEtIsolation->SetYTitle("fCore isolation");

   if (m_histSvc->regShared( histPath + "eEMTOBEt", std::move(hEMEt), m_hEMEt ).isSuccess()){
     ATH_MSG_DEBUG("eEMTOBEt histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEMTOBEt histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEMTOBPhiEta", std::move(hEMEtaPhi), m_hEMEtaPhi ).isSuccess()){
     ATH_MSG_DEBUG("eEMTOBPhiEta histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEMTOBPhiEta histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEMTOBPhiEta_local", std::move(hEMEtaPhi_local), m_hEMEtaPhi_local ).isSuccess()){
     ATH_MSG_DEBUG("eEMTOBPhiEta (local coordinates) histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEMTOBPhiEta (local coordinates) histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEMTOBEtEta", std::move(hEMEtEta), m_hEMEtEta ).isSuccess()){
     ATH_MSG_DEBUG("eEMTOBEtEta histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEMTOBEtEta histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eEMTOBEtPhi", std::move(hEMEtPhi), m_hEMEtPhi ).isSuccess()){
     ATH_MSG_DEBUG("eEMTOBEtPhi histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eEMTOBEtPhi histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauTOBEt", std::move(hTauEt), m_hTauEt ).isSuccess()){
     ATH_MSG_DEBUG("eTauTOBEt histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauTOBEt histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauIsolation", std::move(hTauIsolation), m_hTauIsolation ).isSuccess()){
     ATH_MSG_DEBUG("eTauIsolation histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauIsolation histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauTOBPhiEta", std::move(hTauEtaPhi), m_hTauEtaPhi ).isSuccess()){
     ATH_MSG_DEBUG("eTauTOBPhiEta histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauTOBPhiEta histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauTOBEtEta", std::move(hTauEtEta), m_hTauEtEta ).isSuccess()){
     ATH_MSG_DEBUG("eTauTOBEtEta histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauTOBEtEta histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauTOBEtPhi", std::move(hTauEtPhi), m_hTauEtPhi ).isSuccess()){
     ATH_MSG_DEBUG("eTauTOBEtPhi histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauTOBEtPhi histogram for EMTauProviderFEX");
   }
   if (m_histSvc->regShared( histPath + "eTauTOBEtIsolation", std::move(hTauEtIsolation), m_hTauEtIsolation ).isSuccess()){
     ATH_MSG_DEBUG("eTauTOBEtIsolation histogram has been registered successfully for EMTauProviderFEX.");
   }
   else{
     ATH_MSG_WARNING("Could not register eTauTOBEtIsolation histogram for EMTauProviderFEX");
   }
}



StatusCode
EMTauInputProviderFEX::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {

  //eEM
  SG::ReadHandle<xAOD::eFexEMRoIContainer> eEM_EDM(m_eEM_EDMKey);
  //Temporarily check EDM status by hand to avoid the crash!
  if(!eEM_EDM.isValid()){
    ATH_MSG_WARNING("Could not retrieve EDM Container " << m_eEM_EDMKey.key() << ". No eFEX input for L1Topo");
    
    return StatusCode::SUCCESS;
  }

  for(const auto it : * eEM_EDM){
    const xAOD::eFexEMRoI* eFexRoI = it;
    ATH_MSG_DEBUG( "EDM eFex Number: " 
		   << +eFexRoI->eFexNumber() // returns an 8 bit unsigned integer referring to the eFEX number 
		   << " et: " 
		   << eFexRoI->et() // returns the et value of the EM cluster in MeV
		   << " etTOB: " 
		   << eFexRoI->etTOB() // returns the et value of the EM cluster in units of 100 MeV
		   << " eta: "
		   << eFexRoI->eta() // returns a floating point global eta
		   << " phi: "
		   << eFexRoI->phi() // returns a floating point global phi
		   << " reta: "
		   << eFexRoI->RetaThresholds() // jet disc 1
		   << " rhad: "
		   << eFexRoI->RhadThresholds() // jet disc 2
		   << " wstot: "
		   << eFexRoI->WstotThresholds() // jet disc 3
		   << " is TOB? "
		   << +eFexRoI->isTOB() // returns 1 if true, returns 0 if xTOB)
		  );

    if (!eFexRoI->isTOB()) {continue;}

    unsigned int EtTopo = eFexRoI->etTOB();
    int etaTopo = eFexRoI->iEtaTopo();
    int phiTopo = eFexRoI->iPhiTopo();
    unsigned int reta = eFexRoI->RetaThresholds();
    unsigned int rhad = eFexRoI->RhadThresholds();
    unsigned int wstot = eFexRoI->WstotThresholds();

    //Em TOB
    TCS::eEmTOB eem( EtTopo, 0, etaTopo, static_cast<unsigned int>(phiTopo), TCS::EEM , static_cast<long int>(eFexRoI->word0()) );
    eem.setEtDouble( static_cast<double>(EtTopo/10.) );
    eem.setEtaDouble( static_cast<double>(etaTopo/40.) );
    eem.setPhiDouble( static_cast<double>(phiTopo/20.) );
    eem.setReta( reta );
    eem.setRhad( rhad );
    eem.setWstot( wstot );
    
    inputEvent.addeEm( eem );
    
    m_hEMEt->Fill(eem.EtDouble());  // GeV
    m_hEMEtaPhi->Fill(eem.eta(),eem.phi());
    m_hEMEtaPhi_local->Fill(eFexRoI->iEta(),eFexRoI->iPhi());
    m_hEMEtEta->Fill(eem.EtDouble(),eem.eta());
    m_hEMEtPhi->Fill(eem.EtDouble(),eem.phi());

  }

  //eTau
  SG::ReadHandle<xAOD::eFexTauRoIContainer> eTau_EDM(m_eTau_EDMKey);
  //Temporarily check EDM status by hand to avoid the crash!
  if(!eTau_EDM.isValid()){
    ATH_MSG_WARNING("Could not retrieve EDM Container " << m_eTau_EDMKey.key() << ". No eFEX input for L1Topo");
    
    return StatusCode::SUCCESS;
  }
  
  for(const auto it : * eTau_EDM){
    const xAOD::eFexTauRoI* eFexTauRoI = it;
    ATH_MSG_DEBUG( "EDM eFex Number: " 
		   << +eFexTauRoI->eFexNumber() // returns an 8 bit unsigned integer referring to the eFEX number 
		   << " et: " 
		   << eFexTauRoI->et() // returns the et value of the Tau cluster in MeV
		   << " etTOB: " 
		   << eFexTauRoI->etTOB() // returns the et value of the Tau cluster in units of 100 MeV
		   << " eta: "
		   << eFexTauRoI->eta() // returns a floating point global eta 
		   << " phi: "
		   << eFexTauRoI->phi() // returns a floating point global phi
		   << " fcore "
		   << eFexTauRoI->fCoreThresholds() // returns 1 if true, returns 0 if xTOB)
		  );

    if (!eFexTauRoI->isTOB()) {continue;}

    unsigned int EtTopo = eFexTauRoI->etTOB(); // MeV units
    int etaTopo = eFexTauRoI->iEtaTopo();
    int phiTopo = eFexTauRoI->iPhiTopo();
    double isolation = eFexTauRoI->fCoreThresholds();

    //Tau TOB
    TCS::eTauTOB etau( EtTopo, isolation, etaTopo, static_cast<unsigned int>(phiTopo), TCS::ETAU );
    etau.setEtDouble(  static_cast<double>(EtTopo/10.) );
    etau.setEtaDouble( static_cast<double>(etaTopo/40.) );
    etau.setPhiDouble( static_cast<double>(phiTopo/20.) );
    etau.setIsolation( static_cast<double>(isolation) );

    inputEvent.addeTau( etau );
    inputEvent.addcTau( etau );

    m_hTauEt->Fill(etau.EtDouble());  // GeV
    m_hTauIsolation->Fill(etau.isolation());  
    m_hTauEtaPhi->Fill(etau.eta(),etau.phi());
    m_hTauEtEta->Fill(etau.EtDouble(),etau.eta());
    m_hTauEtPhi->Fill(etau.EtDouble(),etau.phi());
    m_hTauEtIsolation->Fill(etau.EtDouble(),etau.isolation());
  }

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
