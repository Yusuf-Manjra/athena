/*  
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "eflowRec/NewEDM_PFEGamRecoAssoc.h" 
#include "StoreGate/WriteDecorHandle.h" 
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Electron.h" 
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEgamma/EgammaxAODHelpers.h" 
// keep these for the moment, although we expect to NOT need them
#include "xAODPFlow/PFOContainer.h" 
#include "xAODPFlow/PFO.h" 
// this is the new EDM to replace the above
#include "xAODPFlow/FlowElementContainer.h"
#include "xAODPFlow/FlowElement.h"

#include <typeinfo> // temp debug for type checks on objects

typedef ElementLink<xAOD::ElectronContainer> ElectronLink_t; 
typedef ElementLink<xAOD::PhotonContainer> PhotonLink_t;
typedef ElementLink<xAOD::FlowElementContainer> FlowElementLink_t; 

NewEDM_PFEGamRecoAssoc::NewEDM_PFEGamRecoAssoc(
const std::string& name,
  ISvcLocator* pSvcLocator
  ):
  AthAlgorithm(name,pSvcLocator)
{
// Class initializer 
declareProperty ("ElectronNeutralPFODecorKey", m_electronNeutralPFOWriteDecorKey = "Electrons.neutralfeLinks");   
declareProperty ("ElectronChargedPFODecorKey", m_electronChargedPFOWriteDecorKey = "Electrons.chargedfeLinks");   

declareProperty ("NeutralPFOElectronDecorKey", m_neutralpfoElectronWriteDecorKey = "JetETMissNeutralFlowElements.fe_ElectronLinks");   

declareProperty ("ChargedPFOElectronDecorKey", m_chargedpfoElectronWriteDecorKey = "JetETMissChargedFlowElements.fe_ElectronLinks"); 

declareProperty ("PhotonNeutralPFODecorKey", m_photonNeutralPFOWriteDecorKey = "Photons.neutralfeLinks");
declareProperty ("PhotonChargedPFODecorKey", m_photonChargedPFOWriteDecorKey = "Photons.chargedfeLinks");
declareProperty ("NeutralPFOPhotonDecorKey", m_neutralpfoPhotonWriteDecorKey = "JetETMissNeutralFlowElements.fe_PhotonLinks");
declareProperty ("ChargedPFOPhotonDecorKey", m_chargedpfoPhotonWriteDecorKey = "JetETMissChargedFlowElements.fe_PhotonLinks");
}

// Class destructor 
NewEDM_PFEGamRecoAssoc::~NewEDM_PFEGamRecoAssoc(){}

StatusCode NewEDM_PFEGamRecoAssoc::initialize()
{

ATH_MSG_DEBUG("Initializing "<<name() << "...");
ATH_CHECK(m_electronNeutralPFOWriteDecorKey.initialize());   
ATH_CHECK(m_electronChargedPFOWriteDecorKey.initialize());   
ATH_CHECK(m_neutralpfoElectronWriteDecorKey.initialize());   
ATH_CHECK(m_chargedpfoElectronWriteDecorKey.initialize());  

ATH_CHECK(m_photonNeutralPFOWriteDecorKey.initialize());
ATH_CHECK(m_photonChargedPFOWriteDecorKey.initialize());
ATH_CHECK(m_neutralpfoPhotonWriteDecorKey.initialize());
ATH_CHECK(m_chargedpfoPhotonWriteDecorKey.initialize()); 

 ATH_MSG_DEBUG("Initialization completed successfully");   

return StatusCode::SUCCESS;
}

StatusCode NewEDM_PFEGamRecoAssoc::finalize(){
return StatusCode::SUCCESS;
}
StatusCode NewEDM_PFEGamRecoAssoc::execute()
{
  // This algorithm does the following:
  // 1) Read the Input containers for Flow Elements, Electrons and Photons
  // 2) Match the Clusters in the flow elements to the relevant E/Photon clusters
  // 3) Link them
  // 4) output the Electron/Photon containers with the linkers to the Flow element containers

  // write decoration handles for the electron, photon and PFO containers -- these are the OUTPUT handles
  //Electron Write Handle
  SG::WriteDecorHandle<xAOD::ElectronContainer, std::vector<FlowElementLink_t> > electronNeutralPFOWriteDecorHandle (m_electronNeutralPFOWriteDecorKey);   
  SG::WriteDecorHandle<xAOD::ElectronContainer, std::vector<FlowElementLink_t> > electronChargedPFOWriteDecorHandle (m_electronChargedPFOWriteDecorKey);   
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<ElectronLink_t> > neutralpfoElectronWriteDecorHandle (m_neutralpfoElectronWriteDecorKey);   
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<ElectronLink_t> > chargedpfoElectronWriteDecorHandle (m_chargedpfoElectronWriteDecorKey); 

  //Photon Write Handle
  SG::WriteDecorHandle<xAOD::PhotonContainer, std::vector<FlowElementLink_t> > photonNeutralPFOWriteDecorHandle (m_photonNeutralPFOWriteDecorKey);
  SG::WriteDecorHandle<xAOD::PhotonContainer, std::vector<FlowElementLink_t> > photonChargedPFOWriteDecorHandle (m_photonChargedPFOWriteDecorKey);
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<PhotonLink_t> > neutralpfoPhotonWriteDecorHandle (m_neutralpfoPhotonWriteDecorKey);
  SG::WriteDecorHandle<xAOD::FlowElementContainer, std::vector<PhotonLink_t> > chargedpfoPhotonWriteDecorHandle (m_chargedpfoPhotonWriteDecorKey);  
  
  
  // This is the READ handles (so the input containers for electron, photon, PFO)
  
  SG::ReadHandle<xAOD::ElectronContainer>electronReadHandle (m_electronNeutralPFOWriteDecorKey.contHandleKey());
  SG::ReadHandle<xAOD::PhotonContainer> photonReadHandle (m_photonNeutralPFOWriteDecorKey.contHandleKey());
  
  // Charged and Neutral PFlow "Flow elements"
  SG::ReadHandle<xAOD::FlowElementContainer> neutralpfoReadHandle (m_neutralpfoElectronWriteDecorKey.contHandleKey());   
  SG::ReadHandle<xAOD::FlowElementContainer> chargedpfoReadHandle (m_chargedpfoElectronWriteDecorKey.contHandleKey());   
  
  // now initialise some Flow element link containers
  std::vector<std::vector<FlowElementLink_t>> electronNeutralPFOVec(electronReadHandle->size());
  std::vector<std::vector<FlowElementLink_t>> electronChargedPFOVec(electronReadHandle->size());
  
  std::vector<std::vector<FlowElementLink_t>> photonNeutralPFOVec(photonReadHandle->size());
  std::vector<std::vector<FlowElementLink_t>> photonChargedPFOVec(photonReadHandle->size());
  //////////////////////////////////////////////////////////////////////////
  ////                      DO MATCHING/LINKING
  //////////////////////////////////////////////////////////////////////////
  

  ///////////////////////////
  // Loop over neutral flow elements (PFOs)
  ///////////////////////////
  std::cout<<"MATT'S DEBUG: PFO elements size "<<neutralpfoElectronWriteDecorHandle->size()<<std::endl;
  for (const xAOD::FlowElement* pfo: *neutralpfoElectronWriteDecorHandle){
    //Obtain the index of the PFO calo-cluster
    size_t pfoClusterIndex=pfo->otherObjects().at(0)->index();

    // init the linkers
    std::vector<ElectronLink_t> pfoElectronLinks;
    std::vector<PhotonLink_t> pfoPhotonLinks;
    
    std::cout<<"DEBUG 1:  pfoclusterindex: "<<pfoClusterIndex<<std::endl; 
    //Loop over electrons:
    for (const xAOD::Electron* electron: *electronNeutralPFOWriteDecorHandle){
      // get the calo clusters from the electron
      std::cout<<"DEBUG 2: Electron pt : "<<electron->pt()<<std::endl;
      const std::vector<const xAOD::CaloCluster*> electronTopoClusters = xAOD::EgammaHelpers::getAssociatedTopoClusters(electron->caloCluster());
	
      for(const xAOD::CaloCluster* cluster : electronTopoClusters){

	size_t electronClusterIndex=cluster->index();	
	std::cout<<"DEBUG 3: Cluster index : "<<electronClusterIndex<<std::endl;

      } // end loop over cluster
      
    } // end Electron loop

    //Add vector of electron element links as decoration to FlowElement container
    //    neutralpfoElectronWriteDecorHandle (*pfo)=pfoElectronLinks;



  } // end neutral PFO loop


  return StatusCode::SUCCESS;
}
