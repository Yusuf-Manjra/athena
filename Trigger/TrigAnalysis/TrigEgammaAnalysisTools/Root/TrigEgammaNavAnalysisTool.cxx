/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaAnalysisTools/TrigEgammaNavAnalysisTool.h"

using namespace std;

TrigEgammaNavAnalysisTool::TrigEgammaNavAnalysisTool( const std::string& myname ): TrigEgammaNavBaseTool(myname) 
{
  declareProperty("DirectoryPath",m_dir="Analysis");
  declareProperty("IsEMLabels",m_labels);
  m_onlElectrons=nullptr;
  m_onlPhotons=nullptr;
  m_trigElectrons=nullptr;
  m_caloClusters=nullptr;
  m_trigEMClusters=nullptr;
  m_emTauRoI=nullptr;
  m_eventInfo=nullptr;
}

StatusCode TrigEgammaNavAnalysisTool::childInitialize(){
    ATH_MSG_INFO("TrigEgammaNavAnalysisTool: child Initialize");

    m_eventCounter=0;
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavAnalysisTool::childBook(){

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
    // Container level kinematic histograms
    addDirectory(m_dir);
    const int nTrigger = (int) m_trigList.size();
    
    addHistogram(new TH1F("hlt_el_et", "HLT E_{T}; E_{T} [GeV]; Count", 50, 0., 100.));
    addHistogram(new TH2F("hlt_el_eta_phi", "HLT #eta; #eta ; Count", 50, -2.47, 2.47,50,-3.14,3.14));

    addHistogram(new TH1F("l1_energy", "L1 Energy; E [GeV]; Count", 50, 0., 100.));
    addHistogram(new TH1F("l1_roi_et", "L1 RoI Energy; E [GeV]; Count", 50, 0., 100.));
    addHistogram(new TH2F("l1_eta_phi", "L1 Calo; eta ; phi", 50, -2.47, 2.47,50,-3.14,3.14));
    addHistogram(new TH1F("l1_eta", "#eta; #eta ; Count", 50, -2.47, 2.47));
    addHistogram(new TH1F("l1_phi", "#phi; #phi ; Count", 50, -3.14, 3.14));


    addHistogram(new TH1F("electrons", "Offline Electrons; ; N_{electrons}", 6, 1., 6));   
    addHistogram(new TH1F("elperevt", "Offline Electrons; ; N_{electrons}/Evt", 6, 1., 6));   
    std::vector<std::string> el_labels;
    el_labels.push_back("loose");
    el_labels.push_back("medium");
    el_labels.push_back("tight");
    el_labels.push_back("lhloose");
    el_labels.push_back("lhmedium");
    el_labels.push_back("lhtight");
    setLabels(hist1("electrons"),el_labels);
    setLabels(hist1("elperevt"),el_labels);
    
    addHistogram(new TH1F("trigger_counts", "Trigger Counts; Trigger ; Count", nTrigger, 1, nTrigger));
    setLabels(hist1("trigger_counts"),m_trigList);
    for (int i = 0; i < (int) m_trigList.size(); i++) {
        bookPerSignature(m_trigList[i]);
    }

    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavAnalysisTool::childExecute(){
    ATH_MSG_DEBUG("Executing TrigEgammaValidationTool");

    m_eventCounter++;
    m_eventInfo = 0;

    if ( (m_storeGate->retrieve(m_eventInfo, "EventInfo")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve eventInfo ");
        return StatusCode::FAILURE;
    }
    if ((m_storeGate->retrieve(m_onlElectrons,"HLT_xAOD__ElectronContainer_egamma_Electrons")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("Online Electron container size " << m_onlElectrons->size());
    if ((m_storeGate->retrieve(m_onlPhotons,"HLT_xAOD__PhotonContainer_egamma_Photons")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Photons ");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("Online Photon container size " << m_onlPhotons->size());
    if ((m_storeGate->retrieve(m_trigElectrons,"HLT_xAOD__TrigElectronContainer_L2ElectronFex")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("Trig Electron container size " << m_trigElectrons->size());
    if ((m_storeGate->retrieve(m_emTauRoI,"LVL1EmTauRoIs")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("L1 EM container size " << m_emTauRoI->size());
    if ((m_storeGate->retrieve(m_caloClusters,"HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve CaloClusters");
        return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("CaloCluster size " << m_caloClusters->size());
    if ((m_storeGate->retrieve(m_trigEMClusters,"HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma")).isFailure() ){
        ATH_MSG_DEBUG("Failed to retrieve TrigEMClusters");
    }
    else ATH_MSG_DEBUG("TrigEMCluser size " << m_trigEMClusters->size());
   

    // Event wise distributions of persisted containers
    cd(m_dir);
    if( eventWiseSelection().isFailure() ) {
        ATH_MSG_DEBUG("Unable to retrieve offline containers");
        return StatusCode::FAILURE;
    }
    
   for (const auto& l1: *m_emTauRoI) {
        hist2("l1_eta_phi")->Fill(l1->eta(),l1->phi());
        hist1("l1_eta")->Fill(l1->eta());
        hist1("l1_phi")->Fill(l1->phi());
        hist1("l1_energy")->Fill(l1->emClus()/1.e3);
        hist1("l1_roi_et")->Fill(l1->eT()/1.e3);
    }
    float et=0.;
    for (const auto& eg : *m_onlElectrons){
        et = getEt(eg)/1e3;
        hist2("hlt_el_eta_phi")->Fill(eg->eta(),eg->phi());
        hist1("hlt_el_et")->Fill(et);
    }
    

    for(unsigned int ilist = 0; ilist != m_trigList.size(); ilist++) {
        std::string trigger = m_trigList.at(ilist);

        // Trigger counts
        cd(m_dir);
        if(m_trigdec->isPassed("HLT_"+trigger)) hist1("trigger_counts")->AddBinContent(ilist+1);
        
        // Skip event if prescaled out 
        // Prescale cut has ill effects
        // if(m_trigdec->isPassedBits("HLT_"+trigger) & TrigDefs::EF_prescaled) continue;

        std::string basePath = m_dir+"/"+trigger+"/Distributions/";
        
        std::string type="";
        float etthr=0;
        float l1thr=0;
        std::string l1type="";
        std::string pidname="";
        bool perf=false;
        bool etcut=false;
        parseTriggerName(trigger,"Loose",type,etthr,l1thr,l1type,pidname,perf,etcut); // Determines probe PID from trigger
        
        if ( executeNavigation(trigger).isFailure() ){
            ATH_MSG_WARNING("executeNavigation Fails");
            return StatusCode::SUCCESS;
        }
        ATH_MSG_DEBUG("Start Chain Analysis ============================= " << trigger);
        for(unsigned int i=0;i<m_objTEList.size();i++){
            efficiency(m_dir+"/"+trigger+"/Efficiency/",etthr,m_objTEList[i]); // Requires offline match
            inefficiency(m_dir+"/"+trigger+"/Efficiency/HLT",etthr,m_objTEList[i]); // Requires offline match
            resolution(m_dir+"/"+trigger,m_objTEList[i]); // Requires offline match
        }
       
        // Retrieve FeatureContainer for a given trigger
        ATH_MSG_DEBUG("Retrieve features for chain");
        auto fc = (m_trigdec->features("HLT_"+trigger,TrigDefs::alsoDeactivateTEs));
        auto initRois = fc.get<TrigRoiDescriptor>("initialRoI");
        
        ATH_MSG_DEBUG("Size of initialRoI" << initRois.size());
        for(auto feat : initRois){
            if(feat.te()==NULL) {
                ATH_MSG_DEBUG("initial RoI feature NULL");
                continue;
            }
            const TrigRoiDescriptor *roi = feat.cptr();
            cd(basePath+"RoI");
            hist1("roi_eta")->Fill(roi->eta());
            hist1("roi_phi")->Fill(roi->phi());
            ATH_MSG_DEBUG("ROI eta, phi" << roi->eta() << " " << roi->phi());
            ATH_MSG_DEBUG("Retrievec L1 FC");
            auto itEmTau = m_trigdec->ancestor<xAOD::EmTauRoI>(feat);
            const xAOD::EmTauRoI *l1 = itEmTau.cptr();
            if(l1==NULL) continue;
            ATH_MSG_DEBUG("Retrieve L1 Object");
            ATH_MSG_DEBUG("L1 eta, phi" << l1->eta() << " " << l1->phi() << " " << l1->eT()/1.e3);
            fillL1Calo(basePath+"L1Calo",l1);
        }
        auto vec_clus = fc.get<xAOD::CaloClusterContainer>("TrigEFCaloCalibFex",TrigDefs::alsoDeactivateTEs);
        ATH_MSG_DEBUG("EFCalo FC Size " << vec_clus.size());
        for(auto feat : vec_clus){
            if(feat.te()==NULL) continue;
            const xAOD::CaloClusterContainer *cont = feat.cptr();
            if(cont==NULL) continue;
            ATH_MSG_DEBUG("Retrieved EF Calo Container");
            for(const auto& clus : *cont){
                if(clus==NULL) continue;
                ATH_MSG_DEBUG("Retrieved EF Cluster "<<clus->eta() << clus->phi() << clus->e()/1.e3  );
                
                fillEFCalo(basePath+"EFCalo",clus);           
            }
        }
        auto vec_trk = fc.get<xAOD::TrackParticleContainer>("",TrigDefs::alsoDeactivateTEs);
        ATH_MSG_DEBUG("EF Trk FC Size " << vec_trk.size());
        for(auto feat : vec_trk){
            if(feat.te()==NULL) continue;
            const xAOD::TrackParticleContainer *cont = feat.cptr();
            if(cont==NULL) continue;
            ATH_MSG_DEBUG("Retrieved EF Trk Container");
            for(const auto& trk : *cont){
                if(trk==NULL) continue;
                ATH_MSG_DEBUG("Retrieved EF Trk");
                //fillEFCalo(basePath+"EFCalo",clus);           
            }
        }
        
        ATH_MSG_DEBUG("End Chain Analysis ============================= " << trigger);
    } // End loop over trigger list

  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaNavAnalysisTool::childFinalize(){
    ATH_MSG_DEBUG("Processed N events " << m_eventCounter);
    cd(m_dir);
    hist1("elperevt")->Scale(float(m_eventCounter));
    return StatusCode::SUCCESS;
}

void TrigEgammaNavAnalysisTool::bookPerSignature(const std::string trigger){
    ATH_MSG_DEBUG("Now booking histograms");
    std::string basePath = m_dir+"/"+trigger;
    bookAnalysisHistos(basePath);
}

void TrigEgammaNavAnalysisTool::bookPerCategory(const std::string category){
    ATH_MSG_DEBUG("Now booking category histograms " << category);
}


void TrigEgammaNavAnalysisTool::inefficiency(const std::string basePath,const float etthr,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    ATH_MSG_DEBUG("Start Inefficiency Analysis ======================= " << basePath);
    // Inefficiency analysis
    float et=0.;
    const xAOD::Egamma* eg =pairObj.first;
    const HLT::TriggerElement *feat = pairObj.second; 
    if(pairObj.first->type()==xAOD::Type::Electron){
        ATH_MSG_DEBUG("Offline Electron");
        const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
        et = getEt(el)/1e3;
    }
    else  et=eg->caloCluster()->et()/1e3;

    float eta = eg->eta();
    float phi = eg->phi();
    ATH_MSG_DEBUG("Offline et, eta, phi " << et << " " << eta << " " << phi);
    const xAOD::Electron* selEF = NULL;
    const xAOD::Photon* selPh = NULL;
    const xAOD::CaloCluster* selClus = NULL;
    const xAOD::TrackParticle* selTrk = NULL;
    
    // Can we acquire L1 information 
    //
    //auto initRois = fc.get<TrigRoiDescriptor>();
    //if ( initRois.size() < 1 ) ATH_MSG_DEBUG("No L1 RoI"); 
    //auto itEmTau = m_trigDecTool->ancestor<xAOD::EmTauRoI>(initRois[0]);
    ATH_MSG_DEBUG("Retrieve L1");
    const auto* EmTauRoI = getFeature<xAOD::EmTauRoI>(feat);
    ATH_MSG_DEBUG("Retrieve EF Electron");
    const auto* EFEl = getFeature<xAOD::ElectronContainer>(feat);
    ATH_MSG_DEBUG("Retrieve EF Photons");
    const auto* EFPh = getFeature<xAOD::PhotonContainer>(feat);
    ATH_MSG_DEBUG("Retrieve EF Cluster");
    const auto* EFClus = getFeature<xAOD::CaloClusterContainer>(feat);
    ATH_MSG_DEBUG("Retrieve EF Trk");
    const auto* EFTrk = getFeature<xAOD::TrackParticleContainer>(feat);
    float dRmax=0.5;
    bool passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(feat);
    bool passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
    unsigned int runNumber               = m_eventInfo->runNumber();
    unsigned int eventNumber             = m_eventInfo->eventNumber();
    if(EmTauRoI==NULL) ATH_MSG_DEBUG("L1 EmTauRoI NULL pointer");
    if(!passedEF && passedEFCalo){
        ATH_MSG_DEBUG("REGEST::Fails EF Electron, passes EFCalo Hypo Run " << runNumber << " Event " << eventNumber);
        if ( EFEl != NULL ){
            ATH_MSG_DEBUG("Retrieved ElectronContainer for inefficiency " << EFEl->size());
            for(const auto& el : *EFEl){
                float dr=dR(eta,phi,el->eta(),el->phi());
                if ( dr<dRmax){
                    dRmax=dr;
                    selEF = el;
                } // dR
            } // loop over EFEl
            ATH_MSG_DEBUG("Closest electron dR " << dRmax);
        } //FC exists
        else ATH_MSG_DEBUG("Electron Container NULL");
        dRmax=0.5;
        if ( EFPh != NULL ){
            ATH_MSG_DEBUG("Retrieved PhotonnContainer for inefficiency " << EFPh->size());
            for(const auto& ph : *EFPh){
                float dr=dR(eta,phi,ph->eta(),ph->phi());
                if ( dr<dRmax){
                    dRmax=dr;
                    selPh = ph;
                } // dR
            } // loop over EFEl
            ATH_MSG_DEBUG("Closest electron dR " << dRmax);
        } //FC exists
        else ATH_MSG_DEBUG("Photon Container NULL");
        dRmax=0.5;
        if ( EFClus != NULL ){
            ATH_MSG_DEBUG("Retrieved ClusterContainer for inefficiency " << EFClus->size());
            for(const auto& clus : *EFClus){
                float dr=dR(eta,phi,clus->eta(),clus->phi());
                if(dr<dRmax){
                    dRmax=dr;
                    selClus = clus;
                } // dR
            } // loop over EFPh
            ATH_MSG_DEBUG("Closest cluster dR " << dRmax);
        }
        else ATH_MSG_DEBUG("CaloCluster Container NULL");
        dRmax=0.5;
        if ( EFTrk != NULL ){
            ATH_MSG_DEBUG("Retrieved TrackContainer for inefficiency " << EFTrk->size());
            for(const auto& trk : *EFTrk){
                float dr=dR(eta,phi,trk->eta(),trk->phi());
                if(dr<dRmax){
                    dRmax=dr;
                    selTrk = trk;
                } // dR
            } // loop over EFPh
            ATH_MSG_DEBUG("Closest track dR " << dRmax);
        } //FC exists
        else ATH_MSG_DEBUG("TrackParticle Container NULL");

        fillInefficiency(basePath,selEF,selPh,selClus,selTrk);
    }
    ATH_MSG_DEBUG("End Inefficiency Analysis ======================= " << basePath);
}

void TrigEgammaNavAnalysisTool::efficiency(const std::string basePath,const float etthr,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){

    ATH_MSG_DEBUG("Calling SimpleEfficiency");

    float et=0.;
    const xAOD::Egamma* eg =pairObj.first;
    const HLT::TriggerElement *feat = pairObj.second; 
    if(pairObj.first->type()==xAOD::Type::Electron){
        ATH_MSG_DEBUG("Offline Electron");
        const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
        et = getEt(el)/1e3;
    }
    else  et=eg->caloCluster()->et()/1e3;

    float eta = eg->eta();
    float phi = eg->phi();
    float avgmu=0.;
    if(m_lumiTool)
        avgmu = m_lumiTool->lbAverageInteractionsPerCrossing();
    ATH_MSG_DEBUG("Fill probe histograms");

    bool passedL1Calo=false;
    bool passedL2Calo=false;
    bool passedL2=false;
    bool passedEFCalo=false;
    bool passedEF=false;
    if ( feat ) {
        ATH_MSG_DEBUG("Retrieve Ancestor passed");
        passedL1Calo=ancestorPassed<xAOD::EmTauRoI>(feat);
        passedL2Calo = ancestorPassed<xAOD::TrigEMCluster>(feat);
        if(pairObj.first->type()==xAOD::Type::Electron)
            passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat);

        else 
            passedL2 = ancestorPassed<xAOD::TrigPhotonContainer>(feat);
        
        passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(feat);
        
        if(pairObj.first->type()==xAOD::Type::Electron)
            passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);
        else 
            passedEF = ancestorPassed<xAOD::PhotonContainer>(feat);

    } // Features
    fillEfficiency(basePath+"L1Calo",passedL1Calo,etthr,et,eta,phi,avgmu);
    fillEfficiency(basePath+"L2Calo",passedL2Calo,etthr,et,eta,phi,avgmu);
    fillEfficiency(basePath+"L2",passedL2,etthr,et,eta,phi,avgmu);
    fillEfficiency(basePath+"EFCalo",passedEFCalo,etthr,et,eta,phi,avgmu);
    fillEfficiency(basePath+"HLT",passedEF,etthr,et,eta,phi,avgmu);
    ATH_MSG_DEBUG("Complete efficiency"); 
}

