/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#undef NDEBUG

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eTowerBuilder.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "L1CaloFEXSim/eFEXDriver.h"

#include "L1CaloFEXSim/eSuperCellTowerMapper.h"

#include "L1CaloFEXSim/eFEXSim.h"
#include "L1CaloFEXSim/eFEXOutputCollection.h"
#include "L1CaloFEXSim/eFEXegTOB.h"
#include "L1CaloFEXSim/eFakeTower.h"

#include "TROOT.h"
#include "TH1.h"
#include "TH1F.h"
#include "TPad.h"
#include "TCanvas.h"

#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"

#include "xAODTrigger/eFexEMRoI.h"
#include "xAODTrigger/eFexEMRoIContainer.h"

#include <cassert>
#include "SGTools/TestStore.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <ctime>

#include <iostream>
#include <fstream>

#define DEBUG_VHB 1


namespace LVL1 {

  eFEXDriver::eFEXDriver(const std::string& name, ISvcLocator* pSvcLocator)
    :  AthAlgorithm(name, pSvcLocator)//AthReentrantAlgorithm(name, pSvcLocator)
  { 
  
  }


 eFEXDriver::~eFEXDriver()
{
  ATH_MSG_DEBUG("Destroying " << name() << "...");
}


StatusCode eFEXDriver::initialize()
{

  m_numberOfEvents = 1;

  ServiceHandle<ITHistSvc> histSvc("THistSvc","");
  StatusCode scHist = histSvc.retrieve();
  if (scHist ==  StatusCode::FAILURE) {ATH_MSG_ERROR("Failed to retrieve THistSvc"); }

  //Reta
  TH1F* hReta = new TH1F("Reta", "Reta",20,0,1);
  hReta->GetXaxis()->SetTitle("TObs Reta");
  hReta->GetYaxis()->SetTitle("Events");
  
  StatusCode scReg = histSvc->regHist("/ISO/Reta", hReta); 
  if (scReg ==  StatusCode::FAILURE) {ATH_MSG_ERROR("Failed to define stream"); }


  ATH_CHECK( m_eTowerBuilderTool.retrieve() );

  ATH_CHECK( m_eSuperCellTowerMapperTool.retrieve() );

  ATH_CHECK( m_eFEXSysSimTool.retrieve() );

  ATH_CHECK( m_eTowerContainerSGKey.initialize() );

  ATH_CHECK( m_eEDMKey.initialize() );

  // test vector code for validation
  // if(false){ // replace SuperCell Et with the values from the online simulation test vector
  //   ATH_CHECK( m_eFakeTowerTool.retrieve() );
  //   std::string inputfile = "/afs/cern.ch/work/t/tqiu/public/testvector/";
  //   ATH_CHECK( m_eFakeTowerTool->init(inputfile) );
  // }

  //ATH_CHECK( m_eFEXOutputCollectionSGKey.initialize() );

  return StatusCode::SUCCESS;

}


StatusCode eFEXDriver::finalize()
{
  ATH_MSG_DEBUG("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}


  StatusCode eFEXDriver::execute(/*const EventContext& ctx*/) //const
{

  ATH_MSG_DEBUG("Executing " << name() << ", processing event number " << m_numberOfEvents );

  // OLD DIMA STUFF---------------------- Maybe useful in the future again
  //if (fabsf((*cell)->eta()) > 2.55) continue;
  //if (!((*cell)->provenance() & 0x40)) continue; // BCID cut
  //// if (!((*cell)->provenance() & 0x200)) continue;
  //// 8192 & 0x40 = 0
  //// 8256 & 0x40 = 64
  //// 8704 & 0x40 = 0
  //// 8768 & 0x40 = 64

  // STEP 0 - Make a fresh local eTowerContainer
  //eTowerContainer* local_eTowerContainerRaw = new eTowerContainer();
  std::unique_ptr<eTowerContainer> local_eTowerContainerRaw = std::make_unique<eTowerContainer>();

  // STEP 1 - Do some monitoring (code to exported in the future to another algorithm accessing only StoreGate and not appearing in this algorithm)
  eFEXOutputCollection* my_eFEXOutputCollection = new eFEXOutputCollection();
  bool savetob = true;
  if(savetob)
  {
    StatusCode sctob = evtStore()->record(my_eFEXOutputCollection,"eFEXOutputCollection");
    if(sctob == StatusCode::SUCCESS){}
    else if (sctob == StatusCode::FAILURE){ATH_MSG_ERROR("Event " << m_numberOfEvents << " , Failed to put eFEXOutputCollection into Storegate.");}
    
    /*
    SG::WriteHandle<eFEXOutputCollection> eFEXOutputCollectionSG(m_eFEXOutputCollectionSGKey,ctx);
    ATH_CHECK(eFEXOutputCollectionSG.record(std::make_unique<eFEXOutputCollection>()));
    */
  }

  // STEP 2 - Make some eTowers and fill the local container
  ATH_CHECK( m_eTowerBuilderTool.retrieve() );
  m_eTowerBuilderTool->init(local_eTowerContainerRaw);
  local_eTowerContainerRaw->clearContainerMap();
  local_eTowerContainerRaw->fillContainerMap();

  // STEP 3 - Do the supercell-tower mapping - put this information into the eTowerContainer
  ATH_CHECK( m_eSuperCellTowerMapperTool.retrieve() );
  ATH_CHECK(m_eSuperCellTowerMapperTool->AssignSuperCellsToTowers(local_eTowerContainerRaw));
  ATH_CHECK(m_eSuperCellTowerMapperTool->AssignTriggerTowerMapper(local_eTowerContainerRaw));

  // STEP 3.5 - Set up a the first CSV file if necessary (should only need to be done if the mapping changes, which should never happen unless major changes to the simulation are required)
  if(false){ // CSV CODE TO BE RE-INTRODUCED VERY SOON
    if(m_numberOfEvents == 1){
      std::ofstream sc_tower_map;
      sc_tower_map.open("./sc_tower_map.csv");
      sc_tower_map << "#eTowerID,scID,slot,isSplit" << "\n";
      
      DataVector<LVL1::eTower>::iterator thistower;
      for (thistower = local_eTowerContainerRaw->begin(); thistower != local_eTowerContainerRaw->end(); thistower++){
        int slotcount = 0;
        for (int layer = 0; layer<=4; layer++){
          std::vector<Identifier> scIDs = (*thistower)->getLayerSCIDs(layer);
          std::vector<int> splits = (*thistower)->getETSplits();
          for (long unsigned int ncell = 0; ncell < scIDs.size(); ncell++){
            sc_tower_map << (*thistower)->id() << "," << scIDs[ncell] << "," << slotcount << "," << splits[slotcount] << "\n";
            slotcount++;
          }
        }
      }
      sc_tower_map.close();
      
    }
  }

  // test vector code for validation
  // if(false){ // replace SuperCell Et with the values from the online simulation test vector
  //   ATH_CHECK( m_eFakeTowerTool->loadnext() );
  //   ATH_CHECK( m_eFakeTowerTool->seteTowers(local_eTowerContainerRaw.get()) );
  //   ATH_CHECK( m_eFakeTowerTool->execute() );
  // }


  // STEP 4 - Write the completed eTowerContainer into StoreGate (move the local copy in memory)
  SG::WriteHandle<LVL1::eTowerContainer> eTowerContainerSG(m_eTowerContainerSGKey/*, ctx*/);
  //std::unique_ptr<LVL1::eTowerContainer> my_eTowerContainerRaw(local_eTowerContainerRaw);
  ATH_CHECK(eTowerContainerSG.record(std::move(/*my_eTowerContainerRaw*/local_eTowerContainerRaw)));

  // STEP 5 - Set up the eFEXSysSim
  ATH_CHECK( m_eFEXSysSimTool.retrieve() );
  m_eFEXSysSimTool->init();

  // STEP 6 - Run THE eFEXSysSim
  ATH_CHECK(m_eFEXSysSimTool->execute());

  ///STEP 6.5 - test the EDM
  ATH_CHECK(testEDM());

  // STEP 7 - Close and clean the event  
  m_eFEXSysSimTool->cleanup();
  m_eSuperCellTowerMapperTool->reset();
  m_eTowerBuilderTool->reset();

  ATH_MSG_DEBUG("Executed " << name() << ", closing event number " << m_numberOfEvents );

  m_numberOfEvents++;

  return StatusCode::SUCCESS;
}


  StatusCode eFEXDriver::testEDM(){

    const xAOD::eFexEMRoI* myRoI = 0;
    
    SG::ReadHandle<xAOD::eFexEMRoIContainer> myRoIContainer(m_eEDMKey);
    if(!myRoIContainer.isValid()){
      ATH_MSG_FATAL("Could not retrieve EDM Container " << m_eEDMKey.key());
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("----got container: " << myRoIContainer.key());

    for(const auto& it : * myRoIContainer){
      myRoI = it;
      ATH_MSG_DEBUG("EDM eFex Number: " 
                    << +myRoI->eFexNumber() // returns an 8 bit unsigned integer referring to the eFEX number 
                    << " et: " 
                    << myRoI->et() // returns the et value of the EM cluster in MeV
                    << " eta: "
                    << myRoI->eta() // returns a floating point global eta (will be at full precision 0.025, but currently only at 0.1)
                    << " phi: "
                    << myRoI->phi() // returns a floating point global phi
                    << " is TOB? "
                    << +myRoI->isTOB() // returns 1 if true, returns 0 if xTOB
                    );
    }

    return StatusCode::SUCCESS;
  }
  
} // end of LVL1 namespace
