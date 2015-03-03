/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIJetClusterIndexAssociationTool.h"
#include "HIGlobal/HICaloHelper.h"
#include <iomanip>

//**********************************************************************

HIJetClusterIndexAssociationTool::HIJetClusterIndexAssociationTool(const std::string& n)
   :  JetModifierBase(n)
{
  declareProperty("ContainerKey",m_container_key);
  declareProperty("AssociationName",m_assoc_name);
  declareProperty("DeltaR", m_DR = 0.8);
  declareProperty("ApplyFilter", m_apply_filter=false);
  declareProperty("FilterMinE", m_E_min = -999);
  declareProperty("FilterMinPt", m_pT_min = 0);


}

//**********************************************************************


int HIJetClusterIndexAssociationTool::modify(xAOD::JetContainer& jets) const
{

  const xAOD::IParticleContainer* ppars=0;
  if ( evtStore()->contains<xAOD::IParticleContainer>(m_container_key) ) 
  {
    ATH_MSG_DEBUG("Retrieving xAOD container " << m_container_key );
    ppars = evtStore()->retrieve<const xAOD::IParticleContainer>(m_container_key);
    if (!ppars)
    {
      ATH_MSG_ERROR("Failed to retrieve xAOD container " << m_container_key );
      return 0;
    }
  }
  else
  {
    ATH_MSG_ERROR("Failed to retrieve xAOD container " << m_container_key );
    return 0;
  }

  for (xAOD::JetContainer::iterator ijet=jets.begin(); ijet!=jets.end(); ijet++)
  {
    std::vector<const xAOD::IParticle*> ParticleVector;
    xAOD::Jet* theJet=(*ijet);

    float eta=HICaloHelper::GetBinCenterEta(HICaloHelper::FindBinEta(theJet->eta()));
    float phi=HICaloHelper::GetBinCenterPhi(HICaloHelper::FindBinPhi(theJet->phi()));
    
    float etabinmin=HICaloHelper::FindBinEta(eta-m_DR);
    float etabinmax=HICaloHelper::FindBinEta(eta+m_DR);
    
    if(etabinmin < 0) etabinmin=0;
    if(etabinmax > HICaloHelper::NumEtaBins) etabinmax=HICaloHelper::NumEtaBins;
    
    for(int ieta=etabinmin; ieta<=etabinmax; ieta++)
    {
      
      float ev=HICaloHelper::GetBinCenterEta(ieta);
      float delta_ev2=(ev-eta)*(ev-eta);
      float phimin=phi-std::sqrt( m_DR*m_DR-delta_ev2+1e-4 );
      unsigned int iphi=HICaloHelper::FindBinPhi(phimin);
      float pbc=HICaloHelper::GetBinCenterPhi(iphi);
      float dphi=TVector2::Phi_mpi_pi(phi-pbc);
      float rv2=dphi*dphi+delta_ev2;
      
      if(rv2 > m_DR*m_DR)
      {
	iphi++;
	pbc=HICaloHelper::GetBinCenterPhi(iphi);
	dphi=TVector2::Phi_mpi_pi(phi-pbc);
	rv2=dphi*dphi+delta_ev2;
      }
      while(rv2 < m_DR*m_DR)
      {
	float pv=HICaloHelper::GetBinCenterPhi(iphi);
	unsigned int eta_phi_index=HICaloHelper::FindEtaPhiBin(ev,pv);
	const xAOD::IParticle* ap=(*ppars)[eta_phi_index];
	ParticleVector.push_back( ap );
	//update condition for next iteration in while loop
	iphi++;
	pbc=HICaloHelper::GetBinCenterPhi(iphi);
	dphi=TVector2::Phi_mpi_pi(phi-pbc);
	rv2=dphi*dphi+delta_ev2;
      }
    }

    theJet->setAssociatedObjects(m_assoc_name,ParticleVector);

  }
  // return 1;

  // msg(MSG::INFO) << std::setw(20) << "CHECKING ASSOCIATIONS" << endreq;
  // for (xAOD::JetContainer::iterator ijet=jets.begin(); ijet!=jets.end(); ijet++)
  // {
  //   xAOD::Jet* theJet=(*ijet);
  //   msg(MSG::INFO) << std::setw(20) << "JET"
  // 		   << std::setw(15) << theJet->e()
  // 		   << std::setw(15) << theJet->eta()
  // 		   << std::setw(15) << theJet->phi()
  // 		   << std::setw(15) << theJet->pt()
  // 		   << endreq;
    
  //   std::vector<const xAOD::IParticle*> theParticles=theJet->getAssociatedObjects<xAOD::IParticle>(m_assoc_name);
  //   unsigned int ip=0;
  //   for(auto pItr=theParticles.begin(); pItr!=theParticles.end(); pItr++, ip++)
  //   {
  //     msg(MSG::INFO) << std::setw(20) << ip
  // 		     << std::setw(15) << (*pItr)->e()
  // 		     << std::setw(15) << (*pItr)->eta()
  // 		     << std::setw(15) << (*pItr)->phi()
  // 		     << std::setw(15) << (*pItr)->pt()
  // 		     << endreq;
  //   }
  // }
  return 1;
}

