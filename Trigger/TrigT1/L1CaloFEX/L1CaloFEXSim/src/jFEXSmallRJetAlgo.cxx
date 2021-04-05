/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration  
*/
//***************************************************************************  
//		jFEXSmallRJetAlgo - Algorithm for small R jet Algorithm in jFEX
//                              -------------------
//     begin                : 03 11 2020
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************  
#include <iostream>
#include <vector>
#include "L1CaloFEXSim/jFEXSmallRJetAlgo.h"
#include "L1CaloFEXSim/jFEXSmallRJetTOB.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

namespace LVL1{

//Default Constructor
LVL1::jFEXSmallRJetAlgo::jFEXSmallRJetAlgo(const std::string& type, const std::string& name, const IInterface* parent):
   AthAlgTool(type, name, parent)   
  {
  declareInterface<IjFEXSmallRJetAlgo>(this);
  }

/** Destructor */
LVL1::jFEXSmallRJetAlgo::~jFEXSmallRJetAlgo()
{
}
StatusCode LVL1::jFEXSmallRJetAlgo::initialize()
{
   ATH_CHECK(m_jFEXSmallRJetAlgo_jTowerContainerKey.initialize());

   return StatusCode::SUCCESS;

}

//calls container for TT
StatusCode LVL1::jFEXSmallRJetAlgo::safetyTest(){

  SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey);

  if(! jk_jFEXSmallRJetAlgo_jTowerContainer.isValid()){
    ATH_MSG_FATAL("Could not retrieve  jk_jFEXSmallRJetAlgo_jTowerContainer " << m_jFEXSmallRJetAlgo_jTowerContainerKey.key());

    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

void LVL1::jFEXSmallRJetAlgo::setup(int inputTable[7][7], bool barrel_region) {

  std::copy(&inputTable[0][0], &inputTable[0][0] + 49, &m_jFEXalgoTowerID[0][0]);

  m_barrel_region = barrel_region;

}
 
//Gets the ET for the TT. This ET is EM + HAD
unsigned int LVL1::jFEXSmallRJetAlgo::getTTowerET(){
 SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey/*,ctx*/);

 const LVL1::jTower * tmpTower = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[3][3]);
 unsigned int et = tmpTower->getTotalET();  
 return et;
}
//Gets Phi of the TT
unsigned int LVL1::jFEXSmallRJetAlgo::getRealPhi() {  

  SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey/*,ctx*/);
  
  unsigned int phi = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[3][3])->phi();
  return phi;
}
//Gets Eta of the TT
unsigned int LVL1::jFEXSmallRJetAlgo::getRealEta() { 

  SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey/*,ctx*/);

  unsigned int eta = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[3][3])->eta();
  return eta;
}
//this function calculates seed for a given TT
void LVL1::jFEXSmallRJetAlgo::buildSeeds()
{

  m_seedSet = false;
  SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey);

  for(int mphi = 1; mphi < 6; mphi++){
    for(int meta = 1; meta< 6; meta++){
      int et_tmp = 0;
      int seedTotalET = 0;  
      for(int ieta = -1; ieta < 2; ieta++){
        for(int iphi = -1; iphi < 2; iphi++){
          const LVL1::jTower * tmpTower = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[meta + ieta][mphi + iphi]);
          //for that TT, build the seed
          //here we sum TT ET to calculate seed	    	
	        et_tmp = tmpTower->getTotalET();  //in the fcal it already only considers the EM layer as the hcal layer 1 and 2 are considered seperate jTower Objects
          seedTotalET += et_tmp;
       	  }
    	}
   	m_jFEXalgoSearchWindowSeedET[meta -1][mphi -1] = seedTotalET;
      	}

     }    


    m_seedSet = true;
}


//check if central TT is a local maxima
bool LVL1::jFEXSmallRJetAlgo::isSeedLocalMaxima()
{
  if(m_seedSet == false){
    //ATH_MSG_DEBUG("Local Maxima not checked due to seed not calculated.");
  }
  if(m_seedSet == true){

    //ATH_MSG_DEBUG("Local Maxima checking begins.");
    //here put the 24 conditions to determine if the [3][3] TT seed is a local maxima.

    int central_seed = m_jFEXalgoSearchWindowSeedET[3][3];
    for (int ieta = 1; ieta < 6; ieta++){
      for (int iphi = 1; iphi < 6; iphi++){

        //avoid comparing central seed to itself  
 	if ((ieta == 3) && (iphi == 3)){
          continue;
      	}
        //strictly less than central
	if( (iphi >= ieta) && !(ieta == 4 && iphi == 4) && !(ieta == 5 && iphi == 5) ){
	  if(central_seed<m_jFEXalgoSearchWindowSeedET[ieta][iphi]){
	    return false;
	  }  
	}	
	//less than or equal to central 
	if((ieta > iphi) || (ieta == 4 && iphi == 4) || (ieta == 5 && iphi == 5)){
          if(central_seed<= m_jFEXalgoSearchWindowSeedET[ieta][iphi]){
	    return false;
	    }	
	}
       }
     }
  }  

  return true;
}

//in this clustering func, the central TT in jet is the parameters
unsigned int LVL1::jFEXSmallRJetAlgo::getSmallClusterET(){

  SG::ReadHandle<jTowerContainer> jk_jFEXSmallRJetAlgo_jTowerContainer(m_jFEXSmallRJetAlgo_jTowerContainerKey/*,ctx*/);

  //first summing search window (25 TTs)
  unsigned int searchWindowET = 0;
  for(int neta = 0; neta< 7; neta++){ 
    for(int nphi = 0; nphi< 7; nphi++){
      const LVL1::jTower * tmpTower = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[neta][nphi]);
      searchWindowET += tmpTower->getTotalET();
    }
  } 

  //corners removed in 7x7 window to obtain SmallRJetCluser ET;
  int cornersET = 0;  

  const LVL1::jTower * tmpTower_a = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[0][0]);
  cornersET += tmpTower_a->getTotalET();
  const LVL1::jTower * tmpTower_b = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[6][6]);
  cornersET += tmpTower_b->getTotalET(); 
  const LVL1::jTower * tmpTower_c = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[0][6]);
  cornersET += tmpTower_c->getTotalET();
  const LVL1::jTower * tmpTower_d = jk_jFEXSmallRJetAlgo_jTowerContainer->findTower(m_jFEXalgoTowerID[6][0]);
  cornersET += tmpTower_d->getTotalET();

  //SR Jet Energy cluster
  int SRJetClusterET = searchWindowET = cornersET;

  return SRJetClusterET;
}

unsigned int LVL1::jFEXSmallRJetAlgo::getSmallETRing(){
  int SmallETRing = getSmallClusterET() - m_jFEXalgoSearchWindowSeedET[3][3];   
  return SmallETRing;
}
         
std::unique_ptr<jFEXSmallRJetTOB> LVL1::jFEXSmallRJetAlgo::getSmallRJetTOBs(){

  std::unique_ptr<jFEXSmallRJetTOB> tob = std::make_unique<jFEXSmallRJetTOB>();

  unsigned int et = getSmallClusterET();
 // unsigned int phi = getRealPhi(phi);
 // unsigned int eta = getRealEta(eta);
  
  tob->setET(et); 
  tob->setPhi(getRealPhi());
  tob->setEta(getRealEta());
  tob->setRes(0);
  tob->setSat(0);
  return tob;
}


}// end of namespace LVL1

