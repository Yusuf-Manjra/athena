/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhysValFE.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/FlowElementContainer.h"

PhysValFE::PhysValFE (const std::string& type, const std::string& name, const IInterface* parent ) : 
  ManagedMonitorToolBase( type, name, parent ), 
  m_useNeutralFE(false)
{
  declareProperty("useNeutralFE", m_useNeutralFE, "Select whether to use neutral or charged FE");
}

PhysValFE::~PhysValFE() {}

StatusCode PhysValFE::initialize(){
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  ATH_CHECK(m_PFOContainerHandleKey.initialize());
  ATH_CHECK(m_vertexContainerReadHandleKey.initialize());
  ATH_CHECK(m_FEContainerHandleKey.initialize());

  ATH_CHECK(m_MuonContainerHandleKey.initialize());
  ATH_CHECK(m_ElectronContainerHandleKey.initialize());
  ATH_CHECK(m_PhotonContainerHandleKey.initialize());
  ATH_CHECK(m_TauJetContainerHandleKey.initialize());
  
  return StatusCode::SUCCESS;
}


StatusCode PhysValFE::bookHistograms(){
    
  std::string theName = "PFlow/FlowElement/"+m_FEContainerHandleKey.key();
  
  std::vector<HistData> hists;
  std::vector<HistData> additional_hists;
  if (!m_useNeutralFE){
    m_FEChargedValidationPlots.reset(new PFOChargedValidationPlots(0,theName,"", theName));
    m_FEChargedValidationPlots->setDetailLevel(100);
    m_FEChargedValidationPlots->initialize();
    hists = m_FEChargedValidationPlots->retrieveBookedHistograms();
    m_LeptonLinkerPlots_CFE.reset(new LeptonCFEValidationPlots(0,theName,theName));
    m_LeptonLinkerPlots_CFE->setDetailLevel(100);
    m_LeptonLinkerPlots_CFE->initialize();
    m_LeptonLinkerPlots_CFE->retrieveBookedHistograms();
    additional_hists=m_LeptonLinkerPlots_CFE->retrieveBookedHistograms();
  }
  else if (m_useNeutralFE){
    m_FENeutralValidationPlots.reset(new PFONeutralValidationPlots(0,theName, "",theName));
    m_FENeutralValidationPlots->setDetailLevel(100);
    m_FENeutralValidationPlots->initialize();
    hists = m_FENeutralValidationPlots->retrieveBookedHistograms();

    m_LeptonLinkerPlots_NFE.reset(new LeptonNFEValidationPlots(0,theName,theName));
    m_LeptonLinkerPlots_NFE->setDetailLevel(100);
    m_LeptonLinkerPlots_NFE->initialize();
    additional_hists=m_LeptonLinkerPlots_NFE->retrieveBookedHistograms();
  }
  
  std::cout<<"#hists prior to insertion: "<< hists.size()<<", #additional hists: "<<additional_hists.size()<<", NFE? "<<m_useNeutralFE<<std::endl;  
  hists.insert(hists.end(),additional_hists.begin(),additional_hists.end()); // append lepton-FE linker plots to collection of hists
  std::cout<<"#hists after insertion: "<< hists.size()<<", NFE? "<<m_useNeutralFE<<std::endl;

  for (auto hist : hists) {
    ATH_CHECK(regHist(hist.first,hist.second,all));
  }
  
  return StatusCode::SUCCESS;     
   
}

StatusCode PhysValFE::fillHistograms(){

  const xAOD::Vertex* theVertex = nullptr;
  
  if (!m_useNeutralFE){
    SG::ReadHandle<xAOD::VertexContainer> vertexContainerReadHandle(m_vertexContainerReadHandleKey);
    if(!vertexContainerReadHandle.isValid()){
      ATH_MSG_WARNING("Invalid ReadHandle for xAOD::VertexContainer with key: " << vertexContainerReadHandle.key());
    }
    else {
      //Vertex finding logic based on logic in JetRecTools/PFlowPseudoJetGetter tool
      //Usually the 0th vertex is the primary one, but this is not always the case. So we will choose the first vertex of type PriVtx
      for (auto vertex : *vertexContainerReadHandle) {
	if (xAOD::VxType::PriVtx == vertex->vertexType() ) {
	theVertex = vertex;
	break;
	}//If we have a vertex of type primary vertex
      }//iterate over the vertices and check their type

      if (nullptr == theVertex) ATH_MSG_WARNING("Did not find either a PriVtx or a NoVtx in this event");
      
    }//if valid read handle
  }

  SG::ReadHandle<xAOD::FlowElementContainer> FEContainerReadHandle(m_FEContainerHandleKey);
  if(!FEContainerReadHandle.isValid()){
     ATH_MSG_WARNING("Invalid ReadHandle for xAOD::FlowElementContainer with key: " << FEContainerReadHandle.key());
     return StatusCode::SUCCESS;
  }


  for (auto theFE : *FEContainerReadHandle){
    if(theFE){
       if (!m_useNeutralFE) m_FEChargedValidationPlots->fill(*theFE,theVertex);
       else if (m_useNeutralFE) m_FENeutralValidationPlots->fill(*theFE);
    }
    else ATH_MSG_WARNING("Invalid pointer to xAOD::FlowElement");
  }

  SG::ReadHandle<xAOD::PFOContainer> PFOContainerReadHandle(m_PFOContainerHandleKey);
  if(!PFOContainerReadHandle.isValid()){
    ATH_MSG_WARNING("PFO readhandle is a dud");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::MuonContainer> MuonContainerReadHandle(m_MuonContainerHandleKey);
  if(!MuonContainerReadHandle.isValid()){
    ATH_MSG_WARNING("Muon readhandle is a dud");
  }
  for (auto Muon: *MuonContainerReadHandle){
    if(!m_useNeutralFE) m_LeptonLinkerPlots_CFE->fill(*Muon);
    else m_LeptonLinkerPlots_NFE->fill(*Muon);
  }
  SG::ReadHandle<xAOD::ElectronContainer> ElectronContainerReadHandle(m_ElectronContainerHandleKey);
  if(!ElectronContainerReadHandle.isValid()){
    ATH_MSG_WARNING("Electron readhandle is a dud");
  }

  for (auto Electron: *ElectronContainerReadHandle){
    if(!m_useNeutralFE) m_LeptonLinkerPlots_CFE->fill(*Electron);
    else m_LeptonLinkerPlots_NFE->fill(*Electron);
  }


  SG::ReadHandle<xAOD::PhotonContainer> PhotonContainerReadHandle(m_PhotonContainerHandleKey);
  if(!PhotonContainerReadHandle.isValid()){
    ATH_MSG_WARNING("Photon readhandle is a dud");
  }

  for (auto Photon: *PhotonContainerReadHandle){
    if(!m_useNeutralFE) m_LeptonLinkerPlots_CFE->fill(*Photon);
    else m_LeptonLinkerPlots_NFE->fill(*Photon);
  }

  SG::ReadHandle<xAOD::TauJetContainer> TauJetContainerReadHandle(m_TauJetContainerHandleKey);
  if(!TauJetContainerReadHandle.isValid()){
    ATH_MSG_WARNING("TauJet readhandle is a dud");
  }

  for (auto Tau: *TauJetContainerReadHandle){
    if(!m_useNeutralFE) m_LeptonLinkerPlots_CFE->fill(*Tau);
    else m_LeptonLinkerPlots_NFE->fill(*Tau);
  }
  

  return StatusCode::SUCCESS;

}

StatusCode PhysValFE::procHistograms(){
   return StatusCode::SUCCESS;
}
