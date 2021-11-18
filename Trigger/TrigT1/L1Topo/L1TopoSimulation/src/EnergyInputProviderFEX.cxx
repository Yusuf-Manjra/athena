/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h> /* atan2 */

#include "GaudiKernel/ITHistSvc.h"

#include "EnergyInputProviderFEX.h"
#include "TrigT1CaloEvent/EnergyRoI_ClassDEF.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "L1TopoEvent/MetTOB.h"
#include "L1TopoEvent/TopoInputEvent.h"

#include <unistd.h>

using namespace std;
using namespace LVL1;

EnergyInputProviderFEX::EnergyInputProviderFEX(const std::string& type, const std::string& name, 
                                         const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name)
{
   declareInterface<LVL1::IInputTOBConverter>( this );
}

EnergyInputProviderFEX::~EnergyInputProviderFEX()
{}

StatusCode
EnergyInputProviderFEX::initialize() {

  CHECK(m_histSvc.retrieve());
  
  ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "EnergyInputProviderFEX");
  CHECK(incidentSvc.retrieve());
  incidentSvc->addListener(this,"BeginRun", 100);
  incidentSvc.release().ignore();
  
  auto is_jMet_EDMvalid = m_jMet_EDMKey.initialize();
  
  //Temporarily check EDM status by hand to avoid the crash!                                                                                                 
  if (is_jMet_EDMvalid != StatusCode::SUCCESS) {
    ATH_MSG_WARNING("No EDM found for jFEX Met..");
  }

  return StatusCode::SUCCESS;
}


void
EnergyInputProviderFEX::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   auto h_met_Pt = std::make_unique<TH1I>( "MET", "Missing ET TOB", 200, 0, 2000);
   h_met_Pt->SetXTitle("p_{T}");

   auto h_met_Phi = std::make_unique<TH1I>( "METPhi", "MET TOB Phi", 32, -3.2, 3.2);
   h_met_Phi->SetXTitle("#phi");

   if (m_histSvc->regShared( histPath + "MET", std::move(h_met_Pt), m_h_met_Pt ).isSuccess()){
     ATH_MSG_DEBUG("MET histogram has been registered successfully for EnergyProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register MET histogram for EnergyProvider");
   }
   if (m_histSvc->regShared( histPath + "METPhi", std::move(h_met_Phi), m_h_met_Phi ).isSuccess()){
     ATH_MSG_DEBUG("METPhi histogram has been registered successfully for EnergyProvider.");
   }
   else{
     ATH_MSG_WARNING("Could not register METPhi histogram for EnergyProvider");
   }

}


StatusCode
EnergyInputProviderFEX::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {

  SG::ReadHandle<xAOD::jFexMETRoIContainer> jMet_EDM(m_jMet_EDMKey);
  //Temporarily check EDM status by hand to avoid the crash!                                                                                                  
  if(!jMet_EDM.isValid()){
    ATH_MSG_WARNING("Could not retrieve EDM Container " << m_jMet_EDMKey.key() << ". No jFEX MET input for L1Topo");
     return StatusCode::SUCCESS;
  }

  // The jFEX MET container has 12 elements, 2 TOBs per jFEX module, so a total of 12. 
  // According to the documentation https://gitlab.cern.ch/l1calo-run3-simulation/documentation/Run3L1CaloOfflineSWReqs/-/blob/master/l1caloreqs.pdf
  // we want to do a vector sum of Etx/y for the FPGA 0 and FPGA 3.  
  int global_et_x =0;
  int global_et_y =0;

  for(const auto it : *jMet_EDM){
    const xAOD::jFexMETRoI *jFEXMet = it;
    //Get the MET components in 200 MeV units
    int et_x = jFEXMet->getE_x();
    int et_y = jFEXMet->getE_y();
    int jFexNumber = jFEXMet->getjFexNumber();
    int fpgaNumber = jFEXMet->getfpgaNumber();  

    if( fpgaNumber==0 || fpgaNumber==3)
      {
	global_et_x+=et_x;
	global_et_y+=et_y;
      }

    ATH_MSG_DEBUG("jFEX Met Ex = " << et_x << ", Ey = " << et_y <<", jFexNumber="<<jFexNumber<<", fpgaNumber="<<fpgaNumber);
  }

  ATH_MSG_DEBUG("Global MET candidate Ex = " << global_et_x << ", Ey = " <<global_et_y);
  unsigned int et =  std::sqrt( global_et_x*global_et_x + global_et_y*global_et_y );
  TCS::MetTOB met( -(global_et_x), -(global_et_y), et );
  inputEvent.setMET( met );
  m_h_met_Pt->Fill(met.Et());
  m_h_met_Phi->Fill( atan2(met.Ey(),met.Ex()) );
    
  /* not checking overflow currently to be enabled in release 22. 
     const bool has_overflow = (topoData->ExOverflow() or
     topoData->EyOverflow() or
     topoData->EtOverflow());
     if(has_overflow) {
     inputEvent.setOverflowFromEnergyInput(true);
     ATH_MSG_DEBUG("setOverflowFromEnergyInput : true");
     }
  */

  return StatusCode::SUCCESS;
}
