/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// xAODL1Calo includes
#include "iostream"
#include <sstream>
#include <fstream>
#include <TMath.h>

#include "GaudiKernel/ITHistSvc.h"

#include "StoreGate/WriteHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "TrigT1CaloFexSim/JGTowerMaker.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloTriggerTool/LArTTCell.h"
#include "CaloTriggerTool/GTowerSCMap.h"
#include "CaloTriggerTool/JTowerSCMap.h"
#include "TrigT1CaloFexSim/JGTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "TFile.h"
JGTowerMaker::JGTowerMaker( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ){

  declareProperty("useSCQuality",m_useSCQuality=true);
  declareProperty("useAllCalo",m_useAllCalo=false);
  declareProperty("useAllRun2TT",m_useAllRun2TT=false);
  declareProperty("SuperCellType",m_scType="SCell");
  declareProperty("SuperCellQuality",m_scQuality=0x200);
  declareProperty("EmulateSuperCellTiming",m_EmulateSC=false); 
  declareProperty("minSCETp", m_minSCETp=-1);//initialize to -1. To require positive ET > 125 MeV, set to 125 
  declareProperty("maxSCETm", m_maxSCETm=1);//initialize to 1. To require negative ET < -125 MeV, set to -125 
  declareProperty("minTowerET",m_minTowerEt=-9e9); 
}


JGTowerMaker::~JGTowerMaker() {

  m_jT.clear();
  m_gT.clear();
}


StatusCode JGTowerMaker::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
  CHECK( histSvc.retrieve() );

  CHECK(detStore()->retrieve(m_scid));
  CHECK(detStore()->retrieve(m_tid));
  CHECK(detStore()->retrieve(m_jTowerId));
  CHECK(detStore()->retrieve(m_gTowerId));
  CHECK(detStore()->retrieve(m_sem_mgr));
  CHECK(detStore()->retrieve (m_ccIdHelper) );
  detStore()->dump();
  CHECK(SCTowerMapping()); 
  CHECK(ForwardMapping());
  m_TileMapped = false;

  return StatusCode::SUCCESS;
}

StatusCode JGTowerMaker::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode JGTowerMaker::FexAlg(const std::vector<std::shared_ptr<JGTower>>& jgT, xAOD::JGTowerContainer*jgTContainer){

  const CaloCellContainer* scells = 0;
  CHECK( evtStore()->retrieve( scells, m_scType.data()) );
  const CaloCellContainer* cells = nullptr;
  const xAOD::TriggerTowerContainer* TTs = nullptr;
  if ( m_useAllCalo ) {
     CHECK( evtStore()->retrieve( cells, "AllCalo") );
     ATH_MSG_DEBUG ( "Retrieved CaloCellContainer::AllCalo with " << cells->size() << " elements");
  } else {
     if(evtStore()->retrieve(TTs,"xAODTriggerTowers").isFailure() ) {
        ATH_MSG_FATAL("ERROR loading trigger tower");
        return StatusCode::FAILURE;
     }
  }


  for (unsigned hs=0;hs<jgT.size();++hs){
      
      std::shared_ptr<JGTower> jgt = jgT.at(hs);      
      float jgEt=0;
      float lar_et=0;
      float tile_et=0;
      std::vector<int> jgTowerTileIndex = jgt->GetTileIndices();

      std::vector<int> jgTowerSCIndex = jgt->GetSCIndices();

      //Filling tower energy with SC
      if(!m_useAllRun2TT){ 
        for(unsigned i=0; i<jgTowerSCIndex.size(); i++){
           const Identifier scid=m_scid->cell_id(jgTowerSCIndex.at(i));
           const IdentifierHash sc_hash = m_scid->calo_cell_hash(scid);
           CaloCell* scell = (CaloCell*) scells->findCell(sc_hash);
           if(scell==nullptr)continue; 
	   float scell_et = scell->et();
	   if(isnan(scell_et))ATH_MSG_ERROR("Supercell ET is nan. Likely due to BCID correction or something else upstream");
	   float time = scell->time(); 
	   if(m_EmulateSC){
	     if(scell_et<10e3 && fabs(time)>8) continue;
	     if(scell_et>=10e3 && (time > 16 || time<-8)) continue; 
	   }
	   else{
	     if(m_useSCQuality && !( scell->provenance()  &  m_scQuality ) )  continue;
	     if(scell_et>0 && scell_et<m_minSCETp)continue;//allows us to implement supercell ET cuts when building towers 
	     if(scell_et<0 && scell_et>m_maxSCETm)continue; 
	   }
           jgEt += scell_et; 
           lar_et+=scell_et;
        }

        if(jgt->sampling()==1){
          if(m_useAllCalo){
            for(unsigned cell_hs=0 ; cell_hs<jgTowerTileIndex.size(); cell_hs++){
               const CaloCell * cell = cells->findCell(jgTowerTileIndex.at(cell_hs));
               jgEt+=cell->e()*cell->sinTh();
               tile_et+=cell->e()*cell->sinTh();
            }
          } 

          else{

            for(unsigned tt_hs=0 ; tt_hs<TTs->size(); tt_hs++){
               const xAOD::TriggerTower * tt = TTs->at(tt_hs);
               if(tt->sampling()!=1 || fabs(tt->eta())>1.5) continue; // Tile raneg upto 1.5 with sampling == 1
	       float ttPhi = TVector2::Phi_mpi_pi(tt->phi() );//converted [0,2pi] -> [-pi,pi]
               if(!inBox(jgt->Eta(),tt->eta(),jgt->dEta()/2,jgt->Phi(),ttPhi,jgt->dPhi()/2)) continue;
               float cpET = 500*tt->cpET();
	       jgEt+=cpET;
               tile_et+=cpET;
            }

          }
        }
      }
      else{
          for(unsigned tt_hs=0 ; tt_hs<TTs->size(); tt_hs++){
             const xAOD::TriggerTower * tt = TTs->at(tt_hs);
             if(tt->sampling()!= jgt->sampling() && fabs(jgt->Eta())<3.1) continue; // Tile raneg upto 1.5 with sampling == 1
             float ttPhi = TVector2::Phi_mpi_pi(tt->phi() );//converted [0,2pi] -> [-pi,pi]
             if(!inBox(jgt->Eta(),tt->eta(),jgt->dEta()/2,jgt->Phi(),ttPhi,jgt->dPhi()/2)) continue;
             float cpET = 500*tt->cpET();
             jgEt+=cpET;
             tile_et+=cpET;
          }
   
      }
      if(jgEt<m_minTowerEt)jgEt=0; 
      xAOD::JGTower* m_trigTower = new xAOD::JGTower();
      jgTContainer->push_back(m_trigTower);
      m_trigTower->initialize(hs,jgt->Eta(),jgt->Phi());
      m_trigTower->setdEta(jgt->dEta());
      m_trigTower->setdPhi(jgt->dPhi());
      m_trigTower->setEt(jgEt);
      m_trigTower->setSCIndex(jgTowerSCIndex);
      m_trigTower->setTileIndex(jgTowerTileIndex);
      m_trigTower->setSampling(jgt->sampling());
  }
  return StatusCode::SUCCESS; 
}

StatusCode JGTowerMaker::execute() {

   ATH_MSG_DEBUG ("Executing " << name() << "...");
  
   if(!m_TileMapped){
      CHECK( TileMapping() );
   }
   m_TileMapped = true;

 
   xAOD::JGTowerAuxContainer* jTAuxContainer = new xAOD::JGTowerAuxContainer() ;
   xAOD::JGTowerContainer*    jTContainer    = new xAOD::JGTowerContainer() ;
   jTContainer->setStore(jTAuxContainer);
   xAOD::JGTowerAuxContainer* gTAuxContainer = new xAOD::JGTowerAuxContainer() ;
   xAOD::JGTowerContainer*    gTContainer    = new xAOD::JGTowerContainer() ;
   gTContainer->setStore(gTAuxContainer);

   CHECK( FexAlg(m_jT,jTContainer));
   CHECK( FexAlg(m_gT,gTContainer));

   CHECK( evtStore()->record( gTContainer, "GTower" ) );
   CHECK( evtStore()->record( gTAuxContainer, "GTowerAux." )) ;
   ATH_MSG_DEBUG ( "Recorded JGTowerAuxContainer::GTower with " << gTContainer->size() << " elements");

   CHECK( evtStore()->record( jTContainer, "JTower" ) );
   CHECK( evtStore()->record( jTAuxContainer, "JTowerAux." )) ;
   ATH_MSG_DEBUG ( "Recorded JGTowerAuxContainer::JTower with " << jTContainer->size() << " elements");


   return StatusCode::SUCCESS;
}

StatusCode JGTowerMaker::ForwardMapping(){

  unsigned sc_hashMax = m_scid-> calo_cell_hash_max();
  // Forward supercells are jTowers
  for(unsigned sc_hs=0;sc_hs<sc_hashMax;++sc_hs) {
     const Identifier scid=m_scid->cell_id(sc_hs);

     if((m_scid->is_tile(scid)&&m_scid->sampling(scid)!=2)) continue; //skip all tile SCs

     if(m_sem_mgr->get_element(scid)==nullptr) {
       ATH_MSG_INFO("ERROR loading CaloDetDescrElement");
       return StatusCode::FAILURE;
     }


     const CaloDetDescrElement* dde = m_sem_mgr->get_element(scid);
     float scEta = dde->eta_raw();
     float scPhi = dde->phi_raw();
     if(fabs(scEta)<3.1) continue;
     float scDEta = dde->deta();
     float scDPhi = dde->dphi();

     std::shared_ptr<JGTower> JGT = std::make_shared<JGTower>(scEta,scDEta,scPhi,scDPhi);
     JGT->SetSCIndices(sc_hs);

     // forward region sampling should have more detail.
     unsigned int samplingEnum = m_ccIdHelper->calo_sample(scid);
     std::string detName = CaloSampling::getSamplingName(samplingEnum);
     if(detName.find("FCAL0")!=std::string::npos) JGT->SetSampling(2);
     if(detName.find("FCAL1")!=std::string::npos) JGT->SetSampling(3);
     if(detName.find("FCAL2")!=std::string::npos) JGT->SetSampling(4);
     m_jT.push_back(JGT);
  }
  //define gTowers geometry

  float fgT_Etas[5] = {3.2, 3.5, 4.0, 4.45,4.9};
  int nTowers = 17;
  float fgT_dPhi = 2*TMath::Pi()/nTowers;


  for(unsigned int iEta=0; iEta<4; iEta++){
     float fgT_Eta = (fgT_Etas[iEta]+fgT_Etas[iEta+1])/2;
     float fgT_dEta = fgT_Etas[iEta+1]-fgT_Etas[iEta];

     for(int isign=1; isign>-2; isign=isign-2){
        fgT_Eta = fgT_Eta*isign;
        for(unsigned int iPhi = 0; iPhi< 17; iPhi++ ){
           float fgT_Phi = iPhi*fgT_dPhi+fgT_dPhi/2;
           if(fgT_Phi>TMath::Pi()) fgT_Phi = fgT_Phi-2*TMath::Pi();
           //Allocate supercells into gTowers
           std::shared_ptr<JGTower> JGT = std::make_shared<JGTower>(fgT_Eta,fgT_dEta,fgT_Phi,fgT_dPhi);
           JGT->SetSampling(2);

           for(unsigned sc_hs=0;sc_hs<sc_hashMax;++sc_hs) {
              const Identifier scid=m_scid->cell_id(sc_hs);

              if((m_scid->is_tile(scid)&&m_scid->sampling(scid)!=2)) continue; //skip all tile SCs

              if(m_sem_mgr->get_element(scid)==nullptr) {
                ATH_MSG_INFO("ERROR loading CaloDetDescrElement");
                return StatusCode::FAILURE;
              }

              const CaloDetDescrElement* dde = m_sem_mgr->get_element(scid);

              float scEta = dde->eta_raw();
              float scPhi = dde->phi_raw();
              if(fabs(scEta)<3.2) continue;
              if(inBox(fgT_Eta,scEta,fgT_dEta/2,fgT_Phi,scPhi,fgT_dPhi/2) && isign*m_scid->pos_neg(scid) > 0){
                JGT->SetSCIndices(sc_hs);
              }
           }
           m_gT.push_back(JGT);
        }
     }
  }

  return StatusCode::SUCCESS;
}

StatusCode JGTowerMaker::TileMapping(){

  // Tile mapping only available for calocells. TTs only reconstructed with criteria satisfied
  if(m_useAllCalo){
    const CaloCellContainer* cells = 0;
    CHECK( evtStore()->retrieve( cells, "AllCalo") );

    for(unsigned cell_hs=0 ; cell_hs<cells->size(); cell_hs++){
       
       const CaloCell * cell = cells->findCell(cell_hs);
       if(cell==nullptr) continue;
       if(!(m_ccIdHelper->is_tile(cell->ID()))) continue;

       for (unsigned hs=0;hs<m_jT.size();++hs){
           std::shared_ptr<JGTower> jt = m_jT.at(hs);
           if(jt->sampling()==0) continue;
           if(inBox(jt->Eta(),cell->eta(),jt->dEta()/2,jt->Phi(),cell->phi(),jt->dPhi()/2)) jt->SetTileIndices(cell_hs);
       }

       for (unsigned hs=0;hs<m_gT.size();++hs){
           std::shared_ptr<JGTower> gt = m_gT.at(hs);
           if(gt->sampling()==0) continue;
           if(inBox(gt->Eta(),cell->eta(),gt->dEta()/2,gt->Phi(),cell->phi(),gt->dPhi()/2)) gt->SetTileIndices(cell_hs);
       }
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode JGTowerMaker::SCTowerMapping(){


  int m_jTowerHashMax=m_jTowerId->tower_hash_max();
  LArTTCell jSCs;

  for (int hs=0;hs<m_jTowerHashMax;++hs){

      const Identifier jid=m_jTowerId->tower_id(hs);
      const Identifier rid = m_jTowerId->region_id(jid);

      int detSide = 1;
      if ( m_jTowerId->pos_neg(jid) < 0) detSide = -1;

      // Get the phi and eta boundaries of the towers (only advisable for barrel)
      float jDEta = m_jTowerId->etaGranularity(rid);
      float jDPhi = m_jTowerId->phiGranularity(rid);
      int nTowers = (int)(TMath::Pi()/jDPhi)+1;
      jDPhi = TMath::Pi()/nTowers;

      float jEta = (m_jTowerId->eta(jid)+1-0.5)*jDEta*detSide+m_jTowerId->eta0(rid)*detSide;
      float jPhi = (m_jTowerId->phi(jid)+1-0.5)*jDPhi+m_jTowerId->phi0(rid);
      if(jPhi>TMath::Pi()) jPhi = jPhi-2*TMath::Pi(); //m_jTowerId->phi0(rid)-jDPhi*(2*nTowers-m_jTowerId->phi(jid));

      std::shared_ptr<JGTower> JT = std::make_shared<JGTower>(jEta,jDEta,jPhi,jDPhi);
 
      unsigned sc_hashMax = m_scid-> calo_cell_hash_max();
      JT->SetSampling(m_jTowerId->sampling(jid));
      for (unsigned sc_hs=0;sc_hs<sc_hashMax;++sc_hs) {
          const Identifier scid=m_scid->cell_id(sc_hs);
          const Identifier tid=m_tid->cell_id(scid);
          //const IdentifierHash sc_hash = m_scid->calo_cell_hash(scid);
          
          if( (m_scid->is_tile(scid)&&m_scid->sampling(scid)!=2)) continue; //skip all tile SCs
          if(m_sem_mgr->get_element(scid)==nullptr) {
            ATH_MSG_INFO("ERROR loading CaloDetDescrElement");
            return StatusCode::FAILURE;
          }
          const CaloDetDescrElement* dde = m_sem_mgr->get_element(scid);
          int det = m_scid->sub_calo(scid);
        
          // Very speical case being handle here. At the end of the barrel there is a constant-eta ring that has
          // eta_raw=1.4 (which is the midpoint of the scell). These will be put into the g/jTower that starts at eta=1.4
          float scEta = dde->eta_raw();
          float scPhi = dde->phi_raw();
          if(fabs(scEta)>3.2) continue;
          if(fabs(fabs(dde->eta_raw())-1.4)<0.001 && m_scid->region(scid) == 0 && m_scid->sampling(scid) == 2){
            if(scEta > 0) scEta += 0.05;
            else          scEta -= 0.05;
          }

          if(inBox(jEta,scEta,jDEta/2,jPhi,scPhi,jDPhi/2) && m_jTowerId->pos_neg(jid)*m_scid->pos_neg(scid) > 0 
            &&((m_scid->is_em(scid) && m_jTowerId->sampling(jid)==0) 
            ||(!m_scid->is_em(scid) && m_jTowerId->sampling(jid)==1))){

            JT->SetSCIndices(sc_hs);

            LArTTCell_t m;
            m.tpn     = m_jTowerId->pos_neg(jid);
            m.tsample = m_jTowerId->sampling(jid);
            m.tregion = m_jTowerId->region(jid);
            m.teta    = m_jTowerId->eta(jid);
            m.tphi    = m_jTowerId->phi(jid);
            m.det    = det;
            if(!m_scid->is_tile(scid)){
              m.pn     = m_scid->pos_neg(scid);
              m.sample = m_scid->sampling(scid);
              m.region = m_scid->region(scid);
              m.eta    = m_scid->eta(scid);
              m.phi    = m_scid->phi(scid);
            }
            else{
              m.pn     = m_tid->side(tid);
              m.sample = m_tid->sampling(tid);
              m.region = m_tid->region(tid);
              m.eta    = m_tid->tower(tid);
              m.phi    = m_tid->module(tid);
            }

            jSCs.push_back(m);
          } //inbox matching
          if(hs!=0) continue;

      }     //SC loop
      m_jT.push_back(JT);
  }         //jTower loop

  m_JMap->set(jSCs);
  CHECK(detStore()->record(m_JMap,"JTowerSCMap"));

  int m_gTowerHashMax = m_gTowerId->tower_hash_max();


  LArTTCell gSCs;
  for (int hs=0;hs<m_gTowerHashMax;++hs){
      const Identifier gid=m_gTowerId->tower_id(hs);
      const Identifier rid = m_gTowerId->region_id(gid);

      int detSide = 1;
      if ( m_gTowerId->pos_neg(gid) < 0) detSide = -1;

      // Get the phi and eta boundaries of the towers (only advisable for barrel)
      float gDEta = m_gTowerId->etaGranularity(rid);
      float gDPhi = m_gTowerId->phiGranularity(rid);
      int nTowers = (int)(TMath::Pi()/gDPhi)+1;
      gDPhi = TMath::Pi()/nTowers;

      float gEta = (m_gTowerId->eta(gid)+1-0.5)*gDEta*detSide+m_gTowerId->eta0(rid)*detSide;
      float gPhi = (m_gTowerId->phi(gid)+1-0.5)*gDPhi+m_gTowerId->phi0(rid);
      if(gPhi>TMath::Pi()) gPhi = gPhi-2*TMath::Pi();
      std::shared_ptr<JGTower> GT = std::make_shared<JGTower>(gEta,gDEta,gPhi,gDPhi);

      unsigned sc_hashMax = m_scid-> calo_cell_hash_max();
      GT->SetSampling(m_jTowerId->sampling(gid));

      for (unsigned sc_hs=0;sc_hs<sc_hashMax;++sc_hs) {
          const Identifier scid=m_scid->cell_id(sc_hs);
          const Identifier tid=m_tid->cell_id(scid);

          if(m_scid->is_tile(scid)&&m_scid->sampling(scid)!=2) continue;

          if(m_sem_mgr->get_element(scid)==nullptr) {
            ATH_MSG_INFO("ERROR loading CaloDetDescrElement");
            return StatusCode::FAILURE;
          }

          const CaloDetDescrElement* dde = m_sem_mgr->get_element(scid);
          int det = m_scid->sub_calo(scid);

          // Very speical case being handle here. At the end of the barrel there is a constant-eta ring that has
          // eta_raw=1.4 (which is the midpoint of the scell). These will be put into the g/gTower that starts at eta=1.4
          float scEta = dde->eta_raw();
          float scPhi = dde->phi_raw();
          if(fabs(scEta)>3.2) continue;
          if(fabs(fabs(dde->eta_raw())-1.4)<0.001 && m_scid->region(scid) == 0 && m_scid->sampling(scid) == 2){
            if(scEta > 0) scEta += 0.05;
            else          scEta -= 0.05;
          }

          if(inBox(gEta,scEta,gDEta/2,gPhi,scPhi,gDPhi/2) 
            && m_gTowerId->pos_neg(gid)*m_scid->pos_neg(scid) > 0
            &&((m_scid->is_em(scid) && m_gTowerId->sampling(gid)==0)
            ||(!m_scid->is_em(scid) && m_gTowerId->sampling(gid)==1))){

            GT->SetSCIndices(sc_hs);
            LArTTCell_t m;
            m.tpn     = m_gTowerId->pos_neg(gid);
            m.tsample = m_gTowerId->sampling(gid);
            m.tregion = m_gTowerId->region(gid);
            m.teta    = m_gTowerId->eta(gid);
            m.tphi    = m_gTowerId->phi(gid);
            m.det    = det;
            if(!m_scid->is_tile(scid)){
              m.pn     = m_scid->pos_neg(scid);
              m.sample = m_scid->sampling(scid);
              m.region = m_scid->region(scid);
              m.eta    = m_scid->eta(scid);
              m.phi    = m_scid->phi(scid);
            }
            else{
              m.pn     = m_tid->side(tid);
              m.sample = m_tid->sampling(tid);
              m.region = m_tid->region(tid);
              m.eta    = m_tid->tower(tid);
              m.phi    = m_tid->module(tid);
            }
            gSCs.push_back(m);

        } //inbox matching
      }     //SC looping
      m_gT.push_back(GT);
  }         //gTower looping

  m_GMap->set(gSCs);  //not properly in use, but still taken as the tool to identify duplication of SCs
  CHECK(detStore()->record(m_GMap,"GTowerSCMap"));


  return StatusCode::SUCCESS;
}

StatusCode JGTowerMaker::beginInputFile() {  

  return StatusCode::SUCCESS;
}


