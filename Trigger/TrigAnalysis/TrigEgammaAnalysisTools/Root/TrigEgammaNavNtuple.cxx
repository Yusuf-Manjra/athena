/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigEgammaAnalysisTools/TrigEgammaNavNtuple.h"
using namespace std;

TrigEgammaNavNtuple::TrigEgammaNavNtuple( const std::string& myname ): TrigEgammaNavBaseTool(myname) 
{
  declareProperty("DirectoryPath",          m_dir="NavNtuple"                 );
  declareProperty("DoOfflineDump",          m_doOfflineDump=false             ); 
  declareProperty("OfflineDirectoryPath",   m_offDir = "Offline/Egamma/Ntuple");
  ///Set base tool configuration
  m_forcePidSelection=false;
  m_trig_L2_calo_rings=nullptr;
  m_trig_L2_el_pt=nullptr;
  m_trig_L2_el_eta=nullptr;
  m_trig_L2_el_phi=nullptr;
  m_trig_L2_el_charge=nullptr;
  m_trig_L2_el_nTRTHits=nullptr;
  m_trig_L2_el_rcore=nullptr;
  m_trig_L2_el_eratio=nullptr;
  m_trig_L2_el_ethad=nullptr;
  m_trig_L2_el_f0=nullptr;
  m_trig_L2_el_f1=nullptr;
  m_trig_L2_el_f2=nullptr;
  m_trig_L2_el_f3=nullptr;
  m_trig_L2_el_etOverPt=nullptr;
  m_trig_L2_el_trkClusDeta=nullptr;
  m_trig_L2_el_trkClusDphi=nullptr;
  m_mc_hasMC=false     ;
  m_mc_pt=-999        ;
  m_mc_eta=-999       ;
  m_mc_phi=-999       ;
  m_mc_isTop=false     ;
  m_mc_isParton=false  ;
  m_mc_isMeson=false   ;
  m_mc_isQuark=false   ;
  m_mc_isTau=false     ;
  m_mc_isMuon=false    ;
  m_mc_isPhoton=false  ;
  m_mc_isElectron=false;
  m_mc_hasZMother=false;
  m_mc_hasWMother=false;
}


StatusCode TrigEgammaNavNtuple::childInitialize(){

  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavNtuple::childBook(){
  
  addDirectory(m_dir);
  addHistogram(new TH1F("electrons", "Offline Electrons; ; N_{electrons}", 6, 1., 6));   
  addHistogram(new TH1F("elperevt", "Offline Electrons; ; N_{electrons}/Evt", 6, 1., 6)); 


  ATH_MSG_DEBUG("Now configuring chains for analysis");
  std::vector<std::string> selectElectronChains  = m_trigdec->getListOfTriggers("HLT_e.*");
  for (int j = 0; j < (int) selectElectronChains.size(); j++) {
    ATH_MSG_DEBUG("Electron trigger " << selectElectronChains[j]);
  }
  std::vector<std::string> selectPhotonChains  = m_trigdec->getListOfTriggers("HLT_g.*");

  for (int i = 0; i < (int) m_trigInputList.size(); i++) {
    std::string trigname = "HLT_"+m_trigInputList[i];
    for (int j = 0; j < (int) selectElectronChains.size(); j++) {
      size_t found = trigname.find(selectElectronChains[j]);
      if(found != std::string::npos) {
        m_trigList.push_back(m_trigInputList[i]);
        break;
      }
    }
    for (int j = 0; j < (int) selectPhotonChains.size(); j++) {
      std::string trigname = "HLT_"+m_trigInputList[i];
      size_t found = trigname.find(selectPhotonChains[j]);
      if(found != std::string::npos) {
        m_trigList.push_back(m_trigInputList[i]);
        break;
      }
    }
  }  
  for (int i = 0; i < (int) m_trigList.size(); i++) {      

    std::string trigItem = m_trigList[i];
    TTree *t = new TTree( (trigItem).c_str(), "tree of trigger, egamma and monte carlo information");
    bookEventBranches( t );
    bookElectronBranches( t );
    //bookPhotonBranches( t );
    bookTriggerBranches( t );
    bookMonteCarloBranches( t );
    ATH_MSG_DEBUG("Already to attach the tree: " << trigItem);
    addTree(t, m_dir);
    //string name = trigItem+"_monitoring";
    //addHistogram(new TH1F(name.c_str(), "Monitoring; status; Count", 3, 0., 3.));
  }

  ///Only offline egamma ntuple
  if(m_doOfflineDump){
    ATH_MSG_DEBUG("Now configuting only the egamma analysis");
    addDirectory(m_offDir);

    TTree *t_el = new TTree("electron", "tree of egamma and monte carlo information");
    TTree *t_ph = new TTree("photons", "tree of egamma and monte carlo information");

    bookEventBranches( t_ph );
    //bookPhotonBranches( t_ph );
    bookMonteCarloBranches( t_ph );
    bookEventBranches( t_el );
    bookElectronBranches( t_el );
    bookMonteCarloBranches( t_el );

    addTree( t_el, m_offDir );
    addTree( t_ph, m_offDir );
  }

  // alloc memory to vector branchs
  alloc_space();

  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavNtuple::childExecute(){

  m_eventCounter++;
  m_eventInfo = 0;

  if ( (m_storeGate->retrieve(m_eventInfo, "EventInfo")).isFailure() ){
    ATH_MSG_ERROR("Failed to retrieve eventInfo ");
    return StatusCode::FAILURE;
  }
 
  // pileup calculation
  m_nGoodVtx = 0; m_nPileupPrimaryVtx = 0;
  if( m_storeGate->contains<xAOD::VertexContainer>("PrimaryVertices")) {
     const xAOD::VertexContainer* vxContainer(0);
     if ( m_storeGate->retrieve(vxContainer, "PrimaryVertices").isFailure() ) {
       ATH_MSG_WARNING ("Could not retrieve xAOD::VertexContainer 'PrimaryVertices'.");
     }else{
       for(unsigned ivx = 0; ivx < vxContainer->size(); ++ivx){
         int nTrackParticles = vxContainer->at(ivx)->nTrackParticles();
         if (nTrackParticles >= 4) m_nGoodVtx++;
         if ( (nTrackParticles >= 4 && vxContainer->at(ivx)->vertexType() == xAOD::VxType::PriVtx) ||
            (nTrackParticles >= 2 && vxContainer->at(ivx)->vertexType() == xAOD::VxType::PileUp) )
           m_nPileupPrimaryVtx++;
       }// loop over vertexs
     } 
   }// protection


  // Monte Carlo information;
  m_truthContainer = 0;
  if(m_storeGate->contains<xAOD::TruthParticleContainer>("egammaTruthParticles")){
    if(m_storeGate->retrieve(m_truthContainer,"egammaTruthParticles").isFailure()){
      ATH_MSG_WARNING("Could not retrieve xAOD::TruthParticleContainer 'egammaTruthParticles'");
    }
  }// protection

  cd(m_dir);
  if( eventWiseSelection().isFailure() ) {
    ATH_MSG_DEBUG("Unable to retrieve offline containers");
    return StatusCode::FAILURE;
  }


  if(!executeTrigEgammaDump())  return StatusCode::FAILURE;

  /// Egamma ntuple mode
  if(m_doOfflineDump){
    if(!executeElectronDump())  return StatusCode::FAILURE;
    if(!executePhotonDump())    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaNavNtuple::childFinalize(){
  //release_space();
  return StatusCode::SUCCESS;
}


bool TrigEgammaNavNtuple::executeElectronDump(){
    
  ATH_MSG_DEBUG("TrigEgammaNavNtuple::executeEgammaDump()");
  
  const xAOD::ElectronContainer* offElectrons = 0;
  if ( (m_storeGate->retrieve(offElectrons,m_offElContKey)).isFailure() ){
    ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
    return false;
  }

  TTree *t = tree( "electron" , m_offDir );
  if(!t){
    ATH_MSG_ERROR("Can no get the ttree pointer into the store.");
    return false;
  }
  
  linkEventBranches(t); 
  linkElectronBranches(t);
  linkMonteCarloBranches(t);
  
  for(const auto& eg : *offElectrons ){
    
    if(!eg->trackParticle()){
      ATH_MSG_DEBUG("No track Particle");
      continue;
    }
    if(!eg->caloCluster()){
      ATH_MSG_DEBUG("No caloCluster");
      continue;
    }

    //if ( (fabs(eg->eta())>1.37 && fabs(eg->eta())<1.52) || fabs(eg->eta())>2.47 )
    //  continue;

    clear();  
    fillEvent();
    fillElectron( eg );
    
    if(!fillMonteCarlo( eg ) ){
      ATH_MSG_WARNING("Cound not found any TruthParticle for this Electron");
    }

    ATH_MSG_DEBUG("Record information into the ttree.");
    t->Fill();// save information
  }

  return true;
}

bool TrigEgammaNavNtuple::executePhotonDump(){
  return true;
}



bool TrigEgammaNavNtuple::executeTrigEgammaDump(){


  for(unsigned int ilist = 0; ilist != m_trigList.size(); ilist++) {
    std::string trigItem = m_trigList.at(ilist);
    
    if ( executeNavigation(trigItem).isFailure() ){
      ATH_MSG_DEBUG("executeNavigation failure! continue...");
      return StatusCode::FAILURE;
    }
    TTree *t = tree( trigItem, m_dir);
    linkEventBranches(t); 
    linkElectronBranches(t);
    //linkPhotonBranches(t);
    linkTriggerBranches(t);
    linkMonteCarloBranches(t);
    ATH_MSG_DEBUG("ObjTEList size is: " << m_objTEList.size());
    for(unsigned int i = 0; i != m_objTEList.size(); ++i){

      const xAOD::Electron* el =static_cast<const xAOD::Electron*> (m_objTEList[i].first);
      //const xAOD::Photon*   ph =static_cast<const xAOD::Photon*>   (m_objTEList[i].first);
      const HLT::TriggerElement *feat = m_objTEList[i].second;
      if(feat == NULL){
        ATH_MSG_WARNING("TriggerElement is NULL");
        continue;
      } 
      
      clear();
      fillEvent();
      fillElectron( el );
      //fillPhoton( ph );

      if(!fillMonteCarlo( el ) ){
        ATH_MSG_WARNING("Cound not found any TruthParticle for this Electron");
      }
        

      ///Start trigger analisys...
      const xAOD::EmTauRoI *emTauRoI = getFeature<xAOD::EmTauRoI>(feat);
      m_trig_L1_emClus  = emTauRoI->emClus();
      m_trig_L1_tauClus = emTauRoI->tauClus();
      m_trig_L1_emIsol  = emTauRoI->emIsol();
      m_trig_L1_hadIsol = emTauRoI->hadIsol();
      for(unsigned i=0; i < emTauRoI->thrNames().size();++i)  m_trig_L1_thrNames->push_back(emTauRoI->thrNames().at(i));
      m_trig_L1_accept = ancestorPassed<xAOD::EmTauRoI>(feat);

      if(m_trig_L1_accept){   
        const xAOD::TrigEMCluster *emCluster = getFeature<xAOD::TrigEMCluster>(feat);
        if(emCluster){
    
          m_trig_L2_calo_et         = emCluster->et();
          m_trig_L2_calo_eta        = emCluster->eta();
          m_trig_L2_calo_phi        = emCluster->phi();
          m_trig_L2_calo_e237       = emCluster->e237();
          m_trig_L2_calo_e277       = emCluster->e277();
          m_trig_L2_calo_fracs1     = emCluster->fracs1();
          m_trig_L2_calo_weta2      = emCluster->weta2();
          m_trig_L2_calo_ehad1      = emCluster->ehad1();
          m_trig_L2_calo_emaxs1     = emCluster->emaxs1();
          m_trig_L2_calo_e2tsts1    = emCluster->e2tsts1();
          m_trig_L2_calo_wstot      = emCluster->wstot();
          m_trig_L2_calo_accept     = ancestorPassed<xAOD::TrigEMCluster>(feat);
         
          if(!fillTrigCaloRings( emCluster )){
            ATH_MSG_WARNING("Cound not attach the trigCaloRinger information into the tree.");
          }
   
          // Level 2 ID+Calo
          if(m_trig_L2_calo_accept){
            const xAOD::TrigElectronContainer *trigElCont = getFeature<xAOD::TrigElectronContainer>(feat);
            if(trigElCont){
              for(unsigned i = 0; i < trigElCont->size(); ++i ){
                const xAOD::TrigElectron *trigEl = trigElCont->at(i);
                m_trig_L2_el_pt          ->push_back(trigEl->pt()); 
                m_trig_L2_el_eta         ->push_back(trigEl->eta());  
                m_trig_L2_el_phi         ->push_back(trigEl->phi());  
                m_trig_L2_el_charge      ->push_back(trigEl->charge());      
                m_trig_L2_el_nTRTHits    ->push_back(trigEl->nTRTHits());        
                m_trig_L2_el_rcore       ->push_back(trigEl->rcore());      
                m_trig_L2_el_eratio      ->push_back(trigEl->eratio());      
                m_trig_L2_el_ethad       ->push_back(trigEl->etHad());      
                m_trig_L2_el_f0          ->push_back(trigEl->f0());      
                m_trig_L2_el_f1          ->push_back(trigEl->f1());        
                m_trig_L2_el_f2          ->push_back(trigEl->f2());      
                m_trig_L2_el_f3          ->push_back(trigEl->f3());            
                m_trig_L2_el_etOverPt    ->push_back(trigEl->etOverPt());          
                m_trig_L2_el_trkClusDeta ->push_back(trigEl->trkClusDeta());
                m_trig_L2_el_trkClusDphi ->push_back(trigEl->trkClusDphi());
              }// loop over all trigElectrons for this feat
            }// protection
    
            m_trig_L2_el_accept    = ancestorPassed<xAOD::TrigElectronContainer>(feat); 
            // Level EF
            if(m_trig_L2_el_accept){
              m_trig_EF_calo_accept    = ancestorPassed<xAOD::CaloClusterContainer>(feat);
              m_trig_EF_el_accept      = ancestorPassed<xAOD::ElectronContainer>(feat);
            }// EF
          }// protection L2 Track
        }// protection L2 Calo
      }// protection L1
      ATH_MSG_DEBUG("Record information into the ttree.");
      t->Fill();// save information
    }// loop
  } // End loop over trigger list
  return true;
}


bool TrigEgammaNavNtuple::fillEvent(){
  ///Event information
  m_runNumber               = m_eventInfo->runNumber();
  m_eventNumber             = m_eventInfo->eventNumber();
  return true;
}

bool TrigEgammaNavNtuple::fillElectron( const xAOD::Electron *el ){

  ///Cluster information
  m_el_et                   = el->pt();
  m_el_eta                  = el->eta();
  m_el_phi                  = el->phi();
  el->showerShapeValue( m_el_ethad1   , xAOD::EgammaParameters::ShowerShapeType::ethad1   ); 
  el->showerShapeValue( m_el_ehad1    , xAOD::EgammaParameters::ShowerShapeType::ehad1    );
  el->showerShapeValue( m_el_f1       , xAOD::EgammaParameters::ShowerShapeType::f1       );
  el->showerShapeValue( m_el_f3       , xAOD::EgammaParameters::ShowerShapeType::f3       );
  el->showerShapeValue( m_el_f1core   , xAOD::EgammaParameters::ShowerShapeType::f1core   );
  el->showerShapeValue( m_el_f3core   , xAOD::EgammaParameters::ShowerShapeType::f3core   );
  el->showerShapeValue( m_el_weta2    , xAOD::EgammaParameters::ShowerShapeType::weta2    );
  el->showerShapeValue( m_el_wtots1   , xAOD::EgammaParameters::ShowerShapeType::wtots1  );
  el->showerShapeValue( m_el_fracs1   , xAOD::EgammaParameters::ShowerShapeType::fracs1   );
  el->showerShapeValue( m_el_Reta     , xAOD::EgammaParameters::ShowerShapeType::Reta     );
  el->showerShapeValue( m_el_Rphi     , xAOD::EgammaParameters::ShowerShapeType::Rphi     );
  el->showerShapeValue( m_el_Eratio   , xAOD::EgammaParameters::ShowerShapeType::Eratio   );
  el->showerShapeValue( m_el_Rhad     , xAOD::EgammaParameters::ShowerShapeType::Rhad     );
  el->showerShapeValue( m_el_Rhad1    , xAOD::EgammaParameters::ShowerShapeType::Rhad1    );

  ///Combined track/Cluter information
  el->trackCaloMatchValue( m_el_deta1     , xAOD::EgammaParameters::TrackCaloMatchType::deltaEta1   );
  el->trackCaloMatchValue( m_el_deta2     , xAOD::EgammaParameters::TrackCaloMatchType::deltaEta2   );
  el->trackCaloMatchValue( m_el_dphi2     , xAOD::EgammaParameters::TrackCaloMatchType::deltaPhi2   );
  el->trackCaloMatchValue( m_el_dphiresc  , xAOD::EgammaParameters::TrackCaloMatchType::deltaPhiRescaled0 );
  el->trackCaloMatchValue( m_el_dphiresc2 , xAOD::EgammaParameters::TrackCaloMatchType::deltaPhiRescaled2 );


  ///Track particle information
  const xAOD::TrackParticle *track = el->trackParticle();
  if(track){
    m_el_pt                   = track->pt();
    m_el_d0                   = fabsf(track->d0());
    m_el_charge               = el->charge();
    
    track->summaryValue( m_el_eprobht            , xAOD::SummaryType::eProbabilityHT );
    track->summaryValue( m_el_nblayerhits        , xAOD::SummaryType::numberOfBLayerHits );
    track->summaryValue( m_el_nblayerolhits      , xAOD::SummaryType::numberOfBLayerOutliers );
    track->summaryValue( m_el_npixhits           , xAOD::SummaryType::numberOfPixelHits );
    track->summaryValue( m_el_npixolhits         , xAOD::SummaryType::numberOfPixelOutliers );
    track->summaryValue( m_el_nscthits           , xAOD::SummaryType::numberOfSCTHits );
    track->summaryValue( m_el_nsctolhits         , xAOD::SummaryType::numberOfSCTOutliers );
    track->summaryValue( m_el_ntrthightreshits   , xAOD::SummaryType::numberOfTRTHighThresholdHits );
    track->summaryValue( m_el_ntrthits           , xAOD::SummaryType::numberOfTRTHits);
    track->summaryValue( m_el_ntrthighthresolhits, xAOD::SummaryType::numberOfTRTHighThresholdOutliers );
    track->summaryValue( m_el_ntrtolhits         , xAOD::SummaryType::numberOfTRTOutliers );
    track->summaryValue( m_el_ntrtxenonhits      , xAOD::SummaryType::numberOfTRTXenonHits );
    //track->summaryValue( m_el_expectblayerhit    , xAOD::SummaryType::expectBLayerHit );
    track->summaryValue( m_el_npixdeadsensors    , xAOD::SummaryType::numberOfPixelDeadSensors);
    track->summaryValue( m_el_nsctdeadsensors    , xAOD::SummaryType::numberOfSCTDeadSensors);

    m_el_nsideadsensors       = m_el_npixdeadsensors  + m_el_nsctdeadsensors;
    m_el_nsihits              = m_el_npixhits         + m_el_nscthits;
  }

  //m_el_loose              = el->passSelection("Loose");
  //m_el_medium             = el->passSelection("Medium");
  //m_el_tight              = el->passSelection("Tight");
  //m_el_lhLoose            = el->passSelection("LHLoose");
  //m_el_lhMedium           = el->passSelection("LHMedium");
  //m_el_lhTight            = el->passSelection("LHTight"); 
  //m_el_rgLoose            = el->passSelection("RGLoose");
  //m_el_rgMedium           = el->passSelection("RGMedium");
  //m_el_rgTight            = el->passSelection("RGTight");
  //m_el_multiLepton        = el->passSelection("MultiLepton");
  m_el_loose                = ApplyElectronPid(el, "Loose");
  m_el_medium               = ApplyElectronPid(el, "Medium");
  m_el_tight                = ApplyElectronPid(el, "Tight");
  m_el_lhLoose              = ApplyElectronPid(el, "LHLoose");
  m_el_lhMedium             = ApplyElectronPid(el, "LHMedium");
  m_el_lhTight              = ApplyElectronPid(el, "LHTight"); 
  //m_el_rgLoose            = ApplyElectronPid(el, "RGLoose");
  //m_el_rgMedium           = ApplyElectronPid(el, "RGMedium");
  //m_el_rgTight            = ApplyElectronPid(el, "RGTight");
  
  m_calo_et                 = getCluster_et( el );
  m_calo_eta                = getCluster_eta( el );
  m_calo_phi                = getCluster_phi( el );
  ///Extra information about pileup
  m_el_nGoodVtx             = m_nGoodVtx;
  m_el_nPileupPrimaryVtx    = m_nPileupPrimaryVtx;


  if(!fillCaloRings( el )){
    ATH_MSG_WARNING("Could not attach the calorRings information.");
  }

  return true;
}


/*bool TrigEgammaNavNtuple::fillPhoton( const xAOD::Photon *ph ){
  return true;
}*/

bool TrigEgammaNavNtuple::fillMonteCarlo(const xAOD::Egamma *eg){

  // find MC particle
  if(m_truthContainer){
    TLorentzVector elp; elp.SetPtEtaPhiE(eg->pt(),eg->eta(),eg->phi(),eg->e());
    for(const auto& mc : *m_truthContainer ){
      bool Zfound = false;
      bool Wfound = false;
      if(mc->isElectron()){
        size_t nParents = mc->nParents();
        for(size_t iparent = 0; iparent < nParents; ++iparent){
          if((mc->parent(iparent))->isZ()){
            Zfound = true;     
          }
          if((mc->parent(iparent))->isW()){
            Wfound = true;     
          }
        }
      }
      TLorentzVector mcp;
      mcp.SetPtEtaPhiE(mc->pt(), mc->eta(), mc->phi(), mc->e() );
      if(mcp.DeltaR(elp) < 0.07){
        m_mc_hasMC        = true;
        m_mc_pt           = mc->pt();
        m_mc_eta          = mc->eta();
        m_mc_phi          = mc->phi();
        m_mc_isTop        = mc->isTop();
        m_mc_isQuark      = mc->isQuark();
        m_mc_isParton     = mc->isParton();
        m_mc_isMeson      = mc->isMeson();
        m_mc_isTau        = mc->isTau();
        m_mc_isMuon       = mc->isMuon();
        m_mc_isPhoton     = mc->isPhoton();
        m_mc_isElectron   = mc->isElectron();
        m_mc_hasZMother   = Zfound;
        m_mc_hasWMother   = Wfound;
        return true;
       }// has match
    }// loop over MC
  }// has truth?
  return false;
}




bool TrigEgammaNavNtuple::fillTrigCaloRings( const xAOD::TrigEMCluster *emCluster ){
  return TrigEgammaAnalysisBaseTool::getTrigCaloRings(emCluster, *m_trig_L2_calo_rings);
}


bool TrigEgammaNavNtuple::fillCaloRings( const xAOD::Electron *el ){
  return TrigEgammaAnalysisBaseTool::getCaloRings(el, *m_el_ringsE );
}

/*
 * book, link, clear, alloc and release method divide in:
 * trigger, Egamma and MonteCarlo data base.
 */

template <class T> 
void TrigEgammaNavNtuple::InitBranch(TTree* fChain, std::string branch_name, T* param, bool message){

  std::string bname = branch_name;
  if (fChain->GetAlias(bname.c_str()))
     bname = std::string(fChain->GetAlias(bname.c_str()));

  if (!fChain->FindBranch(bname.c_str()) && message) {
    ATH_MSG_WARNING("unknown branch " << bname);
    return;  
  }
  fChain->SetBranchStatus(bname.c_str(), 1.);
  fChain->SetBranchAddress(bname.c_str(), param);
}

void TrigEgammaNavNtuple::bookEventBranches(TTree *t){
  
  t->Branch("RunNumber",        &m_runNumber);
  t->Branch("EventNumber",      &m_eventNumber);
}

void TrigEgammaNavNtuple::bookTriggerBranches(TTree *t){

  // Level L1 cluster
  t->Branch( "trig_L1_emClus",          &m_trig_L1_emClus);
  t->Branch( "trig_L1_tauClus",         &m_trig_L1_tauClus);
  t->Branch( "trig_L1_emIsol",          &m_trig_L1_emIsol);
  t->Branch( "trig_L1_hadIsol",         &m_trig_L1_hadIsol);
  t->Branch( "trig_L1_thrNames",        &m_trig_L1_thrNames);
  t->Branch( "trig_L1_accept",          &m_trig_L1_accept);
  t->Branch( "trig_L2_calo_et",         &m_trig_L2_calo_et);
  t->Branch( "trig_L2_calo_eta",        &m_trig_L2_calo_eta);
  t->Branch( "trig_L2_calo_phi",        &m_trig_L2_calo_phi);
  t->Branch( "trig_L2_calo_e237",       &m_trig_L2_calo_e237 );
  t->Branch( "trig_L2_calo_e277",       &m_trig_L2_calo_e277 );
  t->Branch( "trig_L2_calo_fracs1",     &m_trig_L2_calo_fracs1);
  t->Branch( "trig_L2_calo_weta2",      &m_trig_L2_calo_weta2);
  t->Branch( "trig_L2_calo_ehad1",      &m_trig_L2_calo_ehad1);
  t->Branch( "trig_L2_calo_emaxs1",     &m_trig_L2_calo_emaxs1);
  t->Branch( "trig_L2_calo_e2tsts1",    &m_trig_L2_calo_e2tsts1);
  t->Branch( "trig_L2_calo_wstot",      &m_trig_L2_calo_wstot);
  t->Branch( "trig_L2_calo_rings",      &m_trig_L2_calo_rings ); 
  t->Branch( "trig_L2_calo_accept",     &m_trig_L2_calo_accept);
  t->Branch( "trig_L2_el_pt" ,          &m_trig_L2_el_pt  );
  t->Branch( "trig_L2_el_eta",          &m_trig_L2_el_eta );
  t->Branch( "trig_L2_el_phi",          &m_trig_L2_el_phi );
  t->Branch( "trig_L2_el_charge",       &m_trig_L2_el_charge );
  t->Branch( "trig_L2_el_nTRTHits",     &m_trig_L2_el_nTRTHits);
  t->Branch( "trig_L2_el_rcore" ,       &m_trig_L2_el_rcore );
  t->Branch( "trig_L2_el_eratio" ,      &m_trig_L2_el_eratio );
  t->Branch( "trig_L2_el_ethad" ,       &m_trig_L2_el_ethad );
  t->Branch( "trig_L2_el_f0" ,          &m_trig_L2_el_f0 );
  t->Branch( "trig_L2_el_f1" ,          &m_trig_L2_el_f1 );
  t->Branch( "trig_L2_el_f2" ,          &m_trig_L2_el_f2 );
  t->Branch( "trig_L2_el_f3" ,          &m_trig_L2_el_f3 );
  t->Branch( "trig_L2_el_etOverPt" ,    &m_trig_L2_el_etOverPt );
  t->Branch( "trig_L2_el_trkClusDeta" , &m_trig_L2_el_trkClusDeta );
  t->Branch( "trig_L2_el_trkClusDphi" , &m_trig_L2_el_trkClusDphi );
  t->Branch( "trig_L2_el_accept",       &m_trig_L2_el_accept );
  t->Branch( "trig_EF_calo_accept",     &m_trig_EF_calo_accept);
  t->Branch( "trig_EF_el_accept",       &m_trig_EF_el_accept);
}

void TrigEgammaNavNtuple::bookElectronBranches(TTree *t){
 
  t->Branch("el_et",                    &m_el_et);
  t->Branch("el_pt",                    &m_el_pt);
  t->Branch("el_eta",                   &m_el_eta);
  t->Branch("el_phi",                   &m_el_phi);
  t->Branch("el_ethad1",                &m_el_ethad1);
  t->Branch("el_ehad1",                 &m_el_ehad1);
  t->Branch("el_f1",                    &m_el_f1);
  t->Branch("el_f3",                    &m_el_f3);
  t->Branch("el_f1core",                &m_el_f1core);
  t->Branch("el_f3core",                &m_el_f3core);
  t->Branch("el_weta2",                 &m_el_weta2);
  t->Branch("el_d0",                    &m_el_d0);
  t->Branch("el_wtots1",                &m_el_wtots1);
  t->Branch("el_fracs1",                &m_el_fracs1);
  t->Branch("el_Reta",                  &m_el_Reta);
  t->Branch("el_Rphi",                  &m_el_Rphi);
  t->Branch("el_Eratio",                &m_el_Eratio);
  t->Branch("el_Rhad",                  &m_el_Rhad);
  t->Branch("el_Rhad1",                 &m_el_Rhad1);
  t->Branch("el_deta1",                 &m_el_deta1);
  t->Branch("el_deta2",                 &m_el_deta2);
  t->Branch("el_dphi2",                 &m_el_dphi2);
  t->Branch("el_dphiresc",              &m_el_dphiresc);
  t->Branch("el_dphiresc2",             &m_el_dphiresc2);
  t->Branch("el_eprobht",               &m_el_eprobht);
  t->Branch("el_charge",                &m_el_charge);
  t->Branch("el_nblayerhits",           &m_el_nblayerhits);
  t->Branch("el_nblayerolhits",         &m_el_nblayerolhits);
  t->Branch("el_npixhits",              &m_el_npixhits);
  t->Branch("el_npixolhits",            &m_el_npixolhits);
  t->Branch("el_nscthits",              &m_el_nscthits);
  t->Branch("el_nsctolhits",            &m_el_nsctolhits);
  t->Branch("el_ntrthightreshits",      &m_el_ntrthightreshits);
  t->Branch("el_ntrthits",              &m_el_ntrthits);
  t->Branch("el_ntrthighthresolhits",   &m_el_ntrthighthresolhits);
  t->Branch("el_ntrtolhits",            &m_el_ntrtolhits);
  t->Branch("el_ntrtxenonhits",         &m_el_ntrtxenonhits);
  t->Branch("el_expectblayerhit",       &m_el_expectblayerhit); 
  t->Branch("el_nsihits",               &m_el_nsihits          );
  t->Branch("el_nsideadsensors",        &m_el_nsideadsensors   );
  t->Branch("el_npixdeadsensors",       &m_el_npixdeadsensors  );
  t->Branch("el_nsctdeadsensors",       &m_el_nsctdeadsensors  );
  t->Branch("el_ringsE",                &m_el_ringsE );
  t->Branch("el_loose",                 &m_el_loose  );
  t->Branch("el_medium",                &m_el_medium );
  t->Branch("el_tight",                 &m_el_tight );
  t->Branch("el_lhLoose",               &m_el_lhLoose );
  t->Branch("el_lhMedium",              &m_el_lhMedium ); 
  t->Branch("el_lhTight",               &m_el_lhTight ); 
  t->Branch("el_rgLoose",               &m_el_rgLoose );
  t->Branch("el_rgMedium",              &m_el_rgMedium ); 
  t->Branch("el_rgTight",               &m_el_rgTight ); 
  t->Branch("el_multiLepton",           &m_el_multiLepton);
  t->Branch("el_nGoodVtx",              &m_el_nGoodVtx);
  t->Branch("el_nPileupPrimaryVtx",     &m_el_nPileupPrimaryVtx);
  t->Branch("calo_et",                  &m_calo_et);
  t->Branch("calo_eta",                 &m_calo_eta);
  t->Branch("calo_phi",                 &m_calo_phi);
 
}


/*void TrigEgammaNavNtuple::bookPhotonBranches(TTree *t){
} */ 
  
void TrigEgammaNavNtuple::bookMonteCarloBranches(TTree *t){
  // Monte Carlo
  t->Branch("mc_hasMC",       &m_mc_hasMC);
  t->Branch("mc_pt",          &m_mc_pt);
  t->Branch("mc_eta",         &m_mc_eta);
  t->Branch("mc_phi",         &m_mc_phi);
  t->Branch("mc_isTop",       &m_mc_isTop);
  t->Branch("mc_isParton",    &m_mc_isParton);
  t->Branch("mc_isMeson",     &m_mc_isMeson);
  t->Branch("mc_isTau",       &m_mc_isTau);
  t->Branch("mc_isMuon",      &m_mc_isMuon);
  t->Branch("mc_isPhoton",    &m_mc_isPhoton);
  t->Branch("mc_isElectron",  &m_mc_isElectron);
  t->Branch("mc_hasZMother",  &m_mc_hasZMother);
  t->Branch("mc_hasWMother",  &m_mc_hasWMother);
}

void TrigEgammaNavNtuple::linkEventBranches(TTree *t){
  
  InitBranch( t, "RunNumber",        &m_runNumber);
  InitBranch( t, "EventNumber",      &m_eventNumber);
}

void TrigEgammaNavNtuple::linkElectronBranches( TTree *t ){
  
  InitBranch( t, "el_et",                    &m_el_et);
  InitBranch( t, "el_pt",                    &m_el_pt);
  InitBranch( t, "el_eta",                   &m_el_eta);
  InitBranch( t, "el_phi",                   &m_el_phi);
  InitBranch( t, "el_ethad1",                &m_el_ethad1);
  InitBranch( t, "el_ehad1",                 &m_el_ehad1);
  InitBranch( t, "el_f1",                    &m_el_f1);
  InitBranch( t, "el_f3",                    &m_el_f3);
  InitBranch( t, "el_f1core",                &m_el_f1core);
  InitBranch( t, "el_f3core",                &m_el_f3core);
  InitBranch( t, "el_weta2",                 &m_el_weta2);
  InitBranch( t, "el_d0",                    &m_el_d0);
  InitBranch( t, "el_wtots1",                &m_el_wtots1);
  InitBranch( t, "el_fracs1",                &m_el_fracs1);
  InitBranch( t, "el_Reta",                  &m_el_Reta);
  InitBranch( t, "el_Rphi",                  &m_el_Rphi);
  InitBranch( t, "el_Eratio",                &m_el_Eratio);
  InitBranch( t, "el_Rhad",                  &m_el_Rhad);
  InitBranch( t, "el_Rhad1",                 &m_el_Rhad1);
  InitBranch( t, "el_deta1",                 &m_el_deta1);
  InitBranch( t, "el_deta2",                 &m_el_deta2);
  InitBranch( t, "el_dphi2",                 &m_el_dphi2);
  InitBranch( t, "el_dphiresc",              &m_el_dphiresc);
  InitBranch( t, "el_dphiresc2",             &m_el_dphiresc2);
  InitBranch( t, "el_eprobht",               &m_el_eprobht);
  InitBranch( t, "el_charge",                &m_el_charge);
  InitBranch( t, "el_nblayerhits",           &m_el_nblayerhits);
  InitBranch( t, "el_nblayerolhits",         &m_el_nblayerolhits);
  InitBranch( t, "el_npixhits",              &m_el_npixhits);
  InitBranch( t, "el_npixolhits",            &m_el_npixolhits);
  InitBranch( t, "el_nscthits",              &m_el_nscthits);
  InitBranch( t, "el_nsctolhits",            &m_el_nsctolhits);
  InitBranch( t, "el_ntrthightreshits",      &m_el_ntrthightreshits);
  InitBranch( t, "el_ntrthits",              &m_el_ntrthits);
  InitBranch( t, "el_ntrthighthresolhits",   &m_el_ntrthighthresolhits);
  InitBranch( t, "el_ntrtolhits",            &m_el_ntrtolhits);
  InitBranch( t, "el_ntrtxenonhits",         &m_el_ntrtxenonhits);
  InitBranch( t, "el_expectblayerhit",       &m_el_expectblayerhit); 
  InitBranch( t, "el_nsihits",               &m_el_nsihits          );
  InitBranch( t, "el_nsideadsensors",        &m_el_nsideadsensors   );
  InitBranch( t, "el_npixdeadsensors",       &m_el_npixdeadsensors  );
  InitBranch( t, "el_nsctdeadsensors",       &m_el_nsctdeadsensors  );
  InitBranch( t, "el_ringsE",                &m_el_ringsE );
  InitBranch( t, "el_loose",                 &m_el_loose  );
  InitBranch( t, "el_medium",                &m_el_medium );
  InitBranch( t, "el_tight",                 &m_el_tight );
  InitBranch( t, "el_lhLoose",               &m_el_lhLoose );
  InitBranch( t, "el_lhMedium",              &m_el_lhMedium ); 
  InitBranch( t, "el_lhTight",               &m_el_lhTight ); 
  InitBranch( t, "el_rgLoose",               &m_el_rgLoose );
  InitBranch( t, "el_rgMedium",              &m_el_rgMedium ); 
  InitBranch( t, "el_rgTight",               &m_el_rgTight ); 
  InitBranch( t, "el_multiLepton",           &m_el_multiLepton);
  InitBranch( t, "el_nGoodVtx",              &m_el_nGoodVtx);
  InitBranch( t, "el_nPileupPrimaryVtx",     &m_el_nPileupPrimaryVtx);
  InitBranch( t, "calo_et",                  &m_calo_et);
  InitBranch( t, "calo_eta",                 &m_calo_eta);
  InitBranch( t, "calo_phi",                 &m_calo_phi);

}


/*void TrigEgammaNavNtuple::linkPhotonBranches( TTree *t ){
}*/

void TrigEgammaNavNtuple::linkTriggerBranches( TTree *t ){

  InitBranch(t, "trig_L1_emClus",          &m_trig_L1_emClus);
  InitBranch(t, "trig_L1_tauClus",         &m_trig_L1_tauClus);
  InitBranch(t, "trig_L1_emIsol",          &m_trig_L1_emIsol);
  InitBranch(t, "trig_L1_hadIsol",         &m_trig_L1_hadIsol);
  InitBranch(t, "trig_L1_thrNames",        &m_trig_L1_thrNames);
  InitBranch(t, "trig_L1_accept",          &m_trig_L1_accept);
  InitBranch(t, "trig_L2_calo_et",         &m_trig_L2_calo_et);
  InitBranch(t, "trig_L2_calo_eta",        &m_trig_L2_calo_eta);
  InitBranch(t, "trig_L2_calo_phi",        &m_trig_L2_calo_phi);
  InitBranch(t, "trig_L2_calo_e237",       &m_trig_L2_calo_e237 );
  InitBranch(t, "trig_L2_calo_e277",       &m_trig_L2_calo_e277 );
  InitBranch(t, "trig_L2_calo_fracs1",     &m_trig_L2_calo_fracs1);
  InitBranch(t, "trig_L2_calo_weta2",      &m_trig_L2_calo_weta2);
  InitBranch(t, "trig_L2_calo_ehad1",      &m_trig_L2_calo_ehad1);
  InitBranch(t, "trig_L2_calo_emaxs1",     &m_trig_L2_calo_emaxs1);
  InitBranch(t, "trig_L2_calo_e2tsts1",    &m_trig_L2_calo_e2tsts1);
  InitBranch(t, "trig_L2_calo_wstot",      &m_trig_L2_calo_wstot);
  InitBranch(t, "trig_L2_calo_rings",      &m_trig_L2_calo_rings ); 
  InitBranch(t, "trig_L2_calo_accept",     &m_trig_L2_calo_accept);
  InitBranch(t, "trig_L2_el_pt" ,          &m_trig_L2_el_pt );
  InitBranch(t, "trig_L2_el_eta",          &m_trig_L2_el_eta);
  InitBranch(t, "trig_L2_el_phi",          &m_trig_L2_el_phi );
  InitBranch(t, "trig_L2_el_charge",       &m_trig_L2_el_charge);
  InitBranch(t, "trig_L2_el_nTRTHits",     &m_trig_L2_el_nTRTHits);
  InitBranch(t, "trig_L2_el_rcore" ,       &m_trig_L2_el_rcore );
  InitBranch(t, "trig_L2_el_eratio" ,      &m_trig_L2_el_eratio );
  InitBranch(t, "trig_L2_el_ethad" ,       &m_trig_L2_el_ethad );
  InitBranch(t, "trig_L2_el_f0" ,          &m_trig_L2_el_f0 );
  InitBranch(t, "trig_L2_el_f1" ,          &m_trig_L2_el_f1 );
  InitBranch(t, "trig_L2_el_f2" ,          &m_trig_L2_el_f2 );
  InitBranch(t, "trig_L2_el_f3" ,          &m_trig_L2_el_f3 );
  InitBranch(t, "trig_L2_el_etOverPt" ,    &m_trig_L2_el_etOverPt );
  InitBranch(t, "trig_L2_el_trkClusDeta" , &m_trig_L2_el_trkClusDeta );
  InitBranch(t, "trig_L2_el_trkClusDphi" , &m_trig_L2_el_trkClusDphi );
  InitBranch(t, "trig_L2_el_accept",       &m_trig_L2_el_accept );
  InitBranch(t, "trig_EF_el_accept",       &m_trig_EF_el_accept );
  InitBranch(t, "trig_EF_calo_accept",     &m_trig_EF_calo_accept );
 
}


void TrigEgammaNavNtuple::linkMonteCarloBranches(TTree *t){
 
  InitBranch(t, "mc_hasMC",       &m_mc_hasMC);
  InitBranch(t, "mc_pt",          &m_mc_pt);
  InitBranch(t, "mc_eta",         &m_mc_eta);
  InitBranch(t, "mc_phi",         &m_mc_phi);
  InitBranch(t, "mc_isTop",       &m_mc_isTop);
  InitBranch(t, "mc_isParton",    &m_mc_isParton);
  InitBranch(t, "mc_isMeson",     &m_mc_isMeson);
  InitBranch(t, "mc_isTau",       &m_mc_isTau);
  InitBranch(t, "mc_isMuon",      &m_mc_isMuon);
  InitBranch(t, "mc_isPhoton",    &m_mc_isPhoton);
  InitBranch(t, "mc_isElectron",  &m_mc_isElectron);
  InitBranch(t, "mc_hasZMother",  &m_mc_hasZMother);
  InitBranch(t, "mc_hasWMother",  &m_mc_hasWMother); 
}

void TrigEgammaNavNtuple::clear(){

  ///EventInfo
  m_runNumber             = 0;
  m_eventNumber           = 0;

  ///Egamma
  m_el_et                 = -1;
  m_el_pt                 = -1;
  m_el_eta                = -1;
  m_el_phi                = -1;
  m_el_ethad1             = -1;
  m_el_ehad1              = -1;
  m_el_f1                 = -1;
  m_el_f3                 = -1;
  m_el_f1core             = -1;
  m_el_f3core             = -1;
  m_el_weta2              = -1;
  m_el_d0                 = -1;
  m_el_wtots1             = -1;
  m_el_fracs1             = -1;
  m_el_Reta               = -1;
  m_el_Rphi               = -1;
  m_el_Eratio             = -1;
  m_el_Rhad               = -1;
  m_el_Rhad1              = -1;
  m_el_deta1              = -1;
  m_el_deta2              = -1;
  m_el_dphi2              = -1;
  m_el_dphiresc           = -1;
  m_el_dphiresc2          = -1;
  m_el_eprobht            = -1;
  m_el_charge             =  0;
  m_el_nblayerhits        =  0;
  m_el_nblayerolhits      =  0;
  m_el_npixhits           =  0;
  m_el_npixolhits         =  0;
  m_el_nscthits           =  0;
  m_el_nsctolhits         =  0;
  m_el_ntrthightreshits   =  0;
  m_el_ntrthits           =  0;
  m_el_ntrthighthresolhits=  0;
  m_el_ntrtolhits         =  0;
  m_el_ntrtxenonhits      =  0;
  m_el_expectblayerhit    =  false;
  m_el_nsihits            =  0;
  m_el_nsideadsensors     =  0;
  m_el_npixdeadsensors    =  0;
  m_el_nsctdeadsensors    =  0;
  m_el_loose              = false; 
  m_el_medium             = false; 
  m_el_tight              = false; 
  m_el_lhLoose            = false; 
  m_el_lhMedium           = false; 
  m_el_lhTight            = false;  
  m_el_rgLoose            = false; 
  m_el_rgMedium           = false; 
  m_el_rgTight            = false;  
  m_el_multiLepton        = false;
  m_el_nGoodVtx           = -1;
  m_el_nPileupPrimaryVtx  = -1;
  m_calo_et               = -1; 
  m_calo_eta              = -1; 
  m_calo_phi              = -1; 

  ///Trigger
  m_trig_L1_emClus        = -1;
  m_trig_L1_tauClus       = -1;
  m_trig_L1_emIsol        = -1;
  m_trig_L1_hadIsol       = -1;
  m_trig_L1_accept        = false;
  m_trig_L2_calo_et       = -1;
  m_trig_L2_calo_eta      = -1;
  m_trig_L2_calo_phi      = -1;
  m_trig_L2_calo_e237     = -1;
  m_trig_L2_calo_e277     = -1;
  m_trig_L2_calo_fracs1   = -1;
  m_trig_L2_calo_weta2    = -1;
  m_trig_L2_calo_ehad1    = -1;
  m_trig_L2_calo_emaxs1   = -1;
  m_trig_L2_calo_e2tsts1  = -1;
  m_trig_L2_calo_wstot    = -1; 
  m_trig_L2_calo_accept   = false;
  m_trig_L2_el_accept     = false; 
  m_trig_EF_calo_accept   = false;
  m_trig_EF_el_accept     = false;

  ///Monte Carlo
  m_mc_hasMC              = false;
  m_mc_pt                 = -1;
  m_mc_eta                = -1;
  m_mc_phi                = -1;
  m_mc_isTop              = false;
  m_mc_isParton           = false;
  m_mc_isMeson            = false;
  m_mc_isTau              = false;
  m_mc_isMuon             = false;
  m_mc_isPhoton           = false;
  m_mc_isElectron         = false;
  m_mc_hasZMother         = false;
  m_mc_hasWMother         = false;

  ///Some vectors
  m_trig_L1_thrNames      ->clear(); 
  m_trig_L2_el_pt         ->clear();   
  m_trig_L2_el_eta        ->clear();    
  m_trig_L2_el_phi        ->clear();    
  m_trig_L2_el_charge     ->clear();        
  m_trig_L2_el_nTRTHits   ->clear();          
  m_trig_L2_el_rcore      ->clear();        
  m_trig_L2_el_eratio     ->clear();        
  m_trig_L2_el_ethad      ->clear();        
  m_trig_L2_el_f0         ->clear();        
  m_trig_L2_el_f1         ->clear();          
  m_trig_L2_el_f2         ->clear();        
  m_trig_L2_el_f3         ->clear();              
  m_trig_L2_el_etOverPt   ->clear();            
  m_trig_L2_el_trkClusDeta->clear(); 
  m_trig_L2_el_trkClusDphi->clear(); 
}

void TrigEgammaNavNtuple::alloc_space(){

  m_el_ringsE              = new std::vector<float>();
  m_trig_L1_thrNames       = new std::vector<std::string>();
  m_trig_L2_calo_rings     = new std::vector<float>();
  m_trig_L2_el_pt          = new std::vector<float>();  
  m_trig_L2_el_eta         = new std::vector<float>();   
  m_trig_L2_el_phi         = new std::vector<float>();   
  m_trig_L2_el_charge      = new std::vector<float>();       
  m_trig_L2_el_nTRTHits    = new std::vector<float>();         
  m_trig_L2_el_rcore       = new std::vector<float>();       
  m_trig_L2_el_eratio      = new std::vector<float>();       
  m_trig_L2_el_ethad       = new std::vector<float>();       
  m_trig_L2_el_f0          = new std::vector<float>();       
  m_trig_L2_el_f1          = new std::vector<float>();         
  m_trig_L2_el_f2          = new std::vector<float>();       
  m_trig_L2_el_f3          = new std::vector<float>();             
  m_trig_L2_el_etOverPt    = new std::vector<float>();           
  m_trig_L2_el_trkClusDeta = new std::vector<float>();
  m_trig_L2_el_trkClusDphi = new std::vector<float>();
 
}

void TrigEgammaNavNtuple::release_space(){
  delete m_el_ringsE              ;
  delete m_trig_L1_thrNames       ;
  delete m_trig_L2_calo_rings     ;
  delete m_trig_L2_el_pt          ;  
  delete m_trig_L2_el_eta         ;   
  delete m_trig_L2_el_phi         ;   
  delete m_trig_L2_el_charge      ;       
  delete m_trig_L2_el_nTRTHits    ;         
  delete m_trig_L2_el_rcore       ;       
  delete m_trig_L2_el_eratio      ;       
  delete m_trig_L2_el_ethad       ;       
  delete m_trig_L2_el_f0          ;       
  delete m_trig_L2_el_f1          ;         
  delete m_trig_L2_el_f2          ;       
  delete m_trig_L2_el_f3          ;             
  delete m_trig_L2_el_etOverPt    ;           
  delete m_trig_L2_el_trkClusDeta ;
  delete m_trig_L2_el_trkClusDphi ;
}


