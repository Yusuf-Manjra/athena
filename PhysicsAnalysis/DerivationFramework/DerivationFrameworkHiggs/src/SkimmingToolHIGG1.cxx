/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SkimmingToolHIGG1.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Based on DerivationFramework::SkimmingToolExample

#include "DerivationFrameworkHiggs/SkimmingToolHIGG1.h"
#include <vector>
#include <string>

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackingPrimitives.h"

// Constructor
DerivationFramework::SkimmingToolHIGG1::SkimmingToolHIGG1(const std::string& t,
							    const std::string& n,
							    const IInterface* p) : 
  AthAlgTool(t, n, p),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_isMC(false),
  m_n_tot(0),
  m_n_passGRL(0),
  m_n_passLArError(0),
  m_n_passTrigger(0),
  m_n_passPreselect(0),
  m_n_passSingleElectronPreselect(0),
  m_n_passDoubleElectronPreselect(0),
  m_n_passSingleMuonPreselect(0),
  m_n_passKinematic(0),
  m_n_passQuality(0),
  m_n_passIsolation(0),
  m_n_passInvariantMass(0),
  m_n_pass(0),
  m_e_passGRL(false),
  m_e_passLArError(false),
  m_e_passTrigger(false),
  m_e_passPreselect(false),
  m_e_passSingleElectronPreselect(false),
  m_e_passDoubleElectronPreselect(false),
  m_e_passSingleMuonPreselect(false),
  m_e_passKinematic(false),
  m_e_passQuality(false),
  m_e_passIsolation(false),
  m_e_passInvariantMass(false),
  m_ph_pos_lead(0),
  m_ph_pos_subl(0),
  m_ph_pt_lead(0.),
  m_ph_eta_lead(0.),
  m_ph_phi_lead(0.),
  m_ph_e_lead(0.),
  m_ph_pt_subl(0.),
  m_ph_eta_subl(0.),
  m_ph_phi_subl(0.),
  m_ph_e_subl(0.),
  m_ph_tight_lead(0),
  m_ph_tight_subl(0.),
  m_e_invariantMass(0.),
  m_e_diphotonZ(0.) 
{

  declareInterface<DerivationFramework::ISkimmingTool>(this);

  declareProperty("PhotonContainerKey",    m_photonSGKey="Photons");
  declareProperty("ElectronContainer",     m_electronSGKey = "Electrons");
  declareProperty("MuonContainer",         m_muonSGKey = "Muons");

  declareProperty("RequireGRL",            m_reqGRL           = true);
  declareProperty("ReqireLArError",        m_reqLArError      = true);
  declareProperty("RequireTrigger",        m_reqTrigger       = true);
  declareProperty("RequirePreselection",   m_reqPreselection  = true);
  declareProperty("IncludeSingleElectronPreselection",   m_incSingleElectron  = true);
  declareProperty("IncludeDoubleElectronPreselection",   m_incDoubleElectron  = false);
  declareProperty("IncludeSingleMuonPreselection",       m_incSingleMuon  = true);
  declareProperty("RequireKinematic",      m_reqKinematic     = true);
  declareProperty("RequireQuality",        m_reqQuality       = true);
  declareProperty("RequireIsolation",      m_reqIsolation     = true);
  declareProperty("RequireInvariantMass",  m_reqInvariantMass = true);

  declareProperty("GoodRunList",           m_goodRunList = "");

  declareProperty("DefaultTrigger",        m_defaultTrigger = "EF_g35_loose_g25_loose");
  declareProperty("Triggers",              m_triggers = std::vector<std::string>()); 

  declareProperty("MinimumPhotonPt",       m_minPhotonPt = 20*CLHEP::GeV);
  declareProperty("MinimumElectronPt",     m_minElectronPt = 20*CLHEP::GeV);
  declareProperty("MinimumMuonPt",         m_minMuonPt = 20*CLHEP::GeV);
  declareProperty("MaxMuonEta",            m_maxMuonEta = 2.7);
  declareProperty("RemoveCrack",           m_removeCrack = true);
  declareProperty("MaxEta",                m_maxEta = 2.47);

  declareProperty("RelativePtCuts",        m_relativePtCuts     = true);
  declareProperty("LeadingPhotonPtCut",    m_leadingPhotonPt    = 0.35);
  declareProperty("SubleadingPhotonPtCut", m_subleadingPhotonPt = 0.25);

  declareProperty("MinInvariantMass",      m_minInvariantMass = 105*CLHEP::GeV);
  declareProperty("MaxInvariantMass",      m_maxInvariantMass = 160*CLHEP::GeV);

}
  
// Destructor
DerivationFramework::SkimmingToolHIGG1::~SkimmingToolHIGG1() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::SkimmingToolHIGG1::initialize()
{
  ATH_MSG_VERBOSE("INITIALIZING HSG1 SELECTOR TOOL");

  ////////////////////////////
  // trigger decision tool
   if(m_trigDecisionTool.retrieve().isFailure()) {
     ATH_MSG_FATAL("Failed to retrieve tool: " << m_trigDecisionTool);
     return StatusCode::FAILURE;
   }
  if (!m_triggers.size()) m_triggers.push_back(m_defaultTrigger);
  ATH_MSG_INFO("Retrieved tool: " << m_trigDecisionTool);
  ////////////////////////////

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::SkimmingToolHIGG1::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  ATH_MSG_INFO("Processed " << m_n_tot << " events, " << m_n_pass << " events passed filter ");

  
  ATH_MSG_INFO("GRL       :: " << m_n_passGRL);
  ATH_MSG_INFO("lar       :: " << m_n_passLArError);
  ATH_MSG_INFO("trig      :: " << m_n_passTrigger);
  ATH_MSG_INFO("----------------------------");
  if(m_incDoubleElectron)
    ATH_MSG_INFO("2e        :: " << m_n_passDoubleElectronPreselect);
  if(m_incSingleElectron)
    ATH_MSG_INFO("1y1e      :: " << m_n_passSingleElectronPreselect);
  if(m_incSingleMuon)
    ATH_MSG_INFO("1y1mu     :: " << m_n_passSingleMuonPreselect);
  ATH_MSG_INFO("2y        :: " << m_n_passPreselect);
  ATH_MSG_INFO("----------------------------");
  ATH_MSG_INFO("2y - kin       :: " << m_n_passKinematic);
  ATH_MSG_INFO("2y - qual      :: " << m_n_passQuality);
  ATH_MSG_INFO("2y - iso       :: " << m_n_passIsolation);
  ATH_MSG_INFO("2y - inv       :: " << m_n_passInvariantMass);
  ATH_MSG_INFO("----------------------------");
  ATH_MSG_INFO("passed     :: " << m_n_pass);
  
  return StatusCode::SUCCESS;
}

// The filter itself
bool DerivationFramework::SkimmingToolHIGG1::eventPassesFilter() const
{

  m_n_tot++;

  bool writeEvent(false);

  const xAOD::EventInfo *eventInfo(0);
  ATH_CHECK(evtStore()->retrieve(eventInfo), false);
  m_isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);   

  if (!SubcutGoodRunList() && m_reqGRL      ) return false;
  if (!SubcutLArError()    && m_reqLArError ) return false;
  if (!SubcutTrigger()     && m_reqTrigger  ) return false;

  SubcutPreselect();
  if (!m_reqPreselection) writeEvent = true;	    

  // ey, ee, muy events
  if (SubcutOnePhotonOneElectron()  &&   m_incSingleElectron  ) writeEvent = true;
  if (SubcutTwoElectrons()          &&   m_incDoubleElectron  ) writeEvent = true;
  if (SubcutOnePhotonOneMuon()      &&   m_incSingleMuon      ) writeEvent = true;

  // There *must* be two photons for the remaining 
  // pieces, but you can still save the event...
  if (m_e_passPreselect) {
    GetDiphotonVertex(); 
    CalculateInvariantMass();

    bool passTwoPhotonCuts(true);     
    if (!SubcutQuality()       && m_reqQuality      ) passTwoPhotonCuts = false; 
    if (!SubcutKinematic()     && m_reqKinematic    ) passTwoPhotonCuts = false; 
    if (!SubcutIsolation()     && m_reqIsolation    ) passTwoPhotonCuts = false; 
    if (!SubcutInvariantMass() && m_reqInvariantMass) passTwoPhotonCuts = false;
    // yy events
    if (passTwoPhotonCuts) writeEvent = true; 
  }

  if (!writeEvent) return false;
  
  m_n_pass++;
  return true;
}

bool DerivationFramework::SkimmingToolHIGG1::SubcutGoodRunList() const {

  // Placeholder

  m_e_passGRL = true;
  
  if (m_e_passGRL) m_n_passGRL++;
  return m_e_passGRL;

}
  
  
bool DerivationFramework::SkimmingToolHIGG1::SubcutLArError() const {

  // Retrieve EventInfo
  const xAOD::EventInfo *eventInfo(0);
  ATH_CHECK(evtStore()->retrieve(eventInfo), false);

  m_e_passLArError = !(eventInfo->errorState(xAOD::EventInfo::LAr) == xAOD::EventInfo::Error);
  
  if (m_e_passLArError) m_n_passLArError++;
  return m_e_passLArError;

}


bool DerivationFramework::SkimmingToolHIGG1::SubcutTrigger() const {

  //just for counting purposes
  m_e_passTrigger = m_reqTrigger ? false : true;
  
  if(m_triggers.size()==0) m_e_passTrigger = true;

  for (unsigned int i = 0; i < m_triggers.size(); i++) {
    ATH_MSG_DEBUG("TRIGGER = " << m_triggers.at(i));
    if(m_trigDecisionTool->isPassed(m_triggers.at(i)))
      m_e_passTrigger = true;
  }
  
  if (m_e_passTrigger) m_n_passTrigger++;
  return m_e_passTrigger;

}


bool DerivationFramework::SkimmingToolHIGG1::SubcutPreselect() const {

  const xAOD::PhotonContainer *photons(0); 
  ATH_CHECK(evtStore()->retrieve(photons, m_photonSGKey), false);
  xAOD::PhotonContainer::const_iterator ph_itr(photons->begin());
  xAOD::PhotonContainer::const_iterator ph_end(photons->end());

  m_e_passPreselect = false;
  m_ph_pos_lead = m_ph_pos_subl = -1;
  m_ph_pt_lead = m_ph_pt_subl= 0;

  for(int i = 0; ph_itr != ph_end; ++ph_itr, ++i) {

    if (PhotonPreselect(*ph_itr)) {

      if ((*ph_itr)->pt() > m_ph_pt_lead) {

        m_ph_pos_subl = m_ph_pos_lead; m_ph_pos_lead = i;
        m_ph_pt_subl = m_ph_pt_lead; 
        m_ph_pt_lead = (*ph_itr)->pt();

      } else if ((*ph_itr)->pt() > m_ph_pt_subl) {
        m_ph_pos_subl = i;
        m_ph_pt_subl = (*ph_itr)->pt();
      }
    }
  }

  // save this for the derivation.
  //std::vector<int> *leadingV = new std::vector<int>();
  //leadingV->push_back(m_ph_pos_lead);
  //leadingV->push_back(m_ph_pos_subl);
  //if (!evtStore()->contains<std::vector<int> >("leadingV")) CHECK(evtStore()->record(leadingV, "leadingV"));

  // save this for this code.
  if (m_ph_pos_subl != -1) {
    m_e_leadingPhotons.clear();
    m_e_leadingPhotons.push_back(*(photons->begin() + m_ph_pos_lead));
    m_e_leadingPhotons.push_back(*(photons->begin() + m_ph_pos_subl));
    m_e_passPreselect = true;
    m_n_passPreselect++;
    return true;
  }

  m_e_passPreselect = false;
  return false;

}


bool DerivationFramework::SkimmingToolHIGG1::PhotonPreselect(const xAOD::Photon *ph) const {

  if (!ph) return false;

  if (!ph->isGoodOQ(34214)) return false;

  bool val(false);
  bool defined(false);

  if(ph->isAvailable<char>("DFCommonPhotonsIsEMLoose")){
    defined = true;
    val = static_cast<bool>(ph->auxdata<char>("DFCommonPhotonsIsEMLoose"));
  }
  else{
    defined = ph->passSelection(val, "Loose");
  }
  
  if(!defined || !val) return false;

  // // veto topo-seeded clusters 
  // uint16_t author = 0;
  // author = ph->author();  
  // if (author & xAOD::EgammaParameters::AuthorCaloTopo35) return false;

  // Check which variable versions are best...
  const xAOD::CaloCluster *caloCluster(ph->caloCluster());
  double eta = fabs(caloCluster->etaBE(2));

  if (eta > m_maxEta)             return false;
  if (m_removeCrack && 
      1.37 <= eta && eta <= 1.52) return false;
  if (caloCluster->e()/cosh(eta) < m_minPhotonPt) return false;

  return true;

}

bool DerivationFramework::SkimmingToolHIGG1::SubcutKinematic() const {

  if (m_relativePtCuts) {
    m_e_passKinematic =  (m_ph_pt_lead > m_e_invariantMass * m_leadingPhotonPt);
    m_e_passKinematic &= (m_ph_pt_subl > m_e_invariantMass * m_subleadingPhotonPt);
  } else {
    m_e_passKinematic =  (m_ph_pt_lead > m_leadingPhotonPt);
    m_e_passKinematic &= (m_ph_pt_subl > m_subleadingPhotonPt);
  }

  if (m_e_passKinematic) m_n_passKinematic++;
  return m_e_passKinematic;

}

bool DerivationFramework::SkimmingToolHIGG1::SubcutQuality() const {

  bool val(0);
  m_e_passQuality = false;
  m_e_leadingPhotons.at(0)->passSelection(val, "Tight");
  m_ph_tight_lead = val;

  m_e_leadingPhotons.at(1)->passSelection(val, "Tight");
  m_ph_tight_subl = val;

  m_e_passQuality = (m_ph_tight_lead && m_ph_tight_subl);

  if (m_e_passQuality) m_n_passQuality++;
  return m_e_passQuality;

}

bool DerivationFramework::SkimmingToolHIGG1::SubcutIsolation() const {

  // PLACEHOLDER!!!

  m_e_passIsolation= true;
  
  if (m_e_passIsolation) m_n_passIsolation++;
  return m_e_passIsolation;

}


bool DerivationFramework::SkimmingToolHIGG1::SubcutInvariantMass() const {

// ATH_MSG_INFO("val=" << m_e_invariantMass << "  min=" << m_minInvariantMass << "  max=" << m_maxInvariantMass);

  m_e_passInvariantMass =  (!m_minInvariantMass || 
                           m_minInvariantMass < m_e_invariantMass);

  m_e_passInvariantMass &= (!m_maxInvariantMass || 
                           m_e_invariantMass < m_maxInvariantMass);

  if (m_e_passInvariantMass) m_n_passInvariantMass++;
  return m_e_passInvariantMass;

}

void DerivationFramework::SkimmingToolHIGG1::CalculateInvariantMass() const {

  /// CAUTION - PLACEHOLDERS
  m_ph_e_lead   = CorrectedEnergy(m_e_leadingPhotons.at(0));
  m_ph_e_subl   = CorrectedEnergy(m_e_leadingPhotons.at(1));

  /// CAUTION - CONSTANTS SHOULD BE UPDATED.
  m_ph_eta_lead = CorrectedEta(m_e_leadingPhotons.at(0));
  m_ph_eta_subl = CorrectedEta(m_e_leadingPhotons.at(1));

  m_ph_phi_lead = m_e_leadingPhotons.at(0)->phi();
  m_ph_phi_subl = m_e_leadingPhotons.at(1)->phi();

  m_ph_pt_lead  = m_ph_e_lead / cosh(m_ph_eta_lead);
  m_ph_pt_subl  = m_ph_e_subl / cosh(m_ph_eta_subl);


  m_leadPhotonLV.SetPtEtaPhiM(m_ph_pt_lead, m_ph_eta_lead, m_ph_phi_lead, 0.);
  m_sublPhotonLV.SetPtEtaPhiM(m_ph_pt_subl, m_ph_eta_subl, m_ph_phi_subl, 0.);

  m_e_invariantMass = (m_leadPhotonLV + m_sublPhotonLV).M();

  return;

}



void DerivationFramework::SkimmingToolHIGG1::GetDiphotonVertex() const {

  m_e_diphotonZ = 0; 

}

//// THIS IS A PLACEHOLDER!!
double DerivationFramework::SkimmingToolHIGG1::CorrectedEnergy(const xAOD::Photon *ph) const {

  return ph->e();

}


//////////  THE FOLLOWING TWO FUNCTIONS ARE ADAPTED FROM 
//////////  RUN I HSG1 CUT FLOWS: USE WITH CARE AND CHECK!!!
double DerivationFramework::SkimmingToolHIGG1::CorrectedEta(const xAOD::Photon *ph) const {

  double eta1 = ph->caloCluster()->etaBE(1); 

  double R_photon_front, Z_photon_front;
  if (fabs(eta1) < 1.5) { // barrel
    R_photon_front = ReturnRZ_1stSampling_cscopt2(eta1);
    Z_photon_front = R_photon_front*sinh(eta1);
  } else { // endcap
    Z_photon_front = ReturnRZ_1stSampling_cscopt2(eta1);
    R_photon_front = Z_photon_front/sinh(eta1);
  }

  return asinh((Z_photon_front - m_e_diphotonZ)/R_photon_front);

}


double DerivationFramework::SkimmingToolHIGG1::ReturnRZ_1stSampling_cscopt2(double eta1) const {

  float abs_eta1 = fabs(eta1);

  double radius = -99999;
  if (abs_eta1 < 0.8) {
    radius = 1558.859292 - 4.990838  * abs_eta1 - 21.144279 * abs_eta1 * abs_eta1;
  } else if (abs_eta1 < 1.5) {
    radius = 1522.775373 + 27.970192 * abs_eta1 - 21.104108 * abs_eta1 * abs_eta1;
  } else { //endcap
    radius = 3790.671754;
    if (eta1 < 0.) radius = -radius;
  }

  return radius;

}

bool DerivationFramework::SkimmingToolHIGG1::SubcutOnePhotonOneElectron() const {

  const xAOD::PhotonContainer *photons(0); 
  ATH_CHECK(evtStore()->retrieve(photons, m_photonSGKey), false);
  xAOD::PhotonContainer::const_iterator ph_itr(photons->begin());
  xAOD::PhotonContainer::const_iterator ph_end(photons->end());

  const xAOD::ElectronContainer *electrons(0);
  ATH_CHECK(evtStore()->retrieve(electrons, m_electronSGKey), false);
  xAOD::ElectronContainer::const_iterator el_itr(electrons->begin());
  xAOD::ElectronContainer::const_iterator el_end(electrons->end());

  m_e_passSingleElectronPreselect = false;

  for( ; ph_itr != ph_end; ++ph_itr){
    if(PhotonPreselect(*ph_itr)){
      for( ; el_itr != el_end; ++el_itr){
        if(ElectronPreselect(*el_itr)){
          m_e_passSingleElectronPreselect = true;
        }
      }
    }
  }


  if(m_e_passSingleElectronPreselect) m_n_passSingleElectronPreselect++;
  return m_e_passSingleElectronPreselect;
}

bool DerivationFramework::SkimmingToolHIGG1::SubcutTwoElectrons() const {

  const xAOD::ElectronContainer *electrons(0);
  ATH_CHECK(evtStore()->retrieve(electrons, m_electronSGKey), false);
  xAOD::ElectronContainer::const_iterator el_itr(electrons->begin());
  xAOD::ElectronContainer::const_iterator el_end(electrons->end());

  int nEle(0);
  m_e_passDoubleElectronPreselect = false;
  
  for( ; el_itr != el_end; ++el_itr){
    if(ElectronPreselect(*el_itr))
      nEle++;
  }
  
  if(nEle >=2) m_e_passDoubleElectronPreselect = true;
  
  if(m_e_passDoubleElectronPreselect) m_n_passDoubleElectronPreselect++;
  return m_e_passDoubleElectronPreselect;
}


bool DerivationFramework::SkimmingToolHIGG1::SubcutOnePhotonOneMuon() const {

  const xAOD::PhotonContainer *photons(0); 
  ATH_CHECK(evtStore()->retrieve(photons, m_photonSGKey), false);
  xAOD::PhotonContainer::const_iterator ph_itr(photons->begin());
  xAOD::PhotonContainer::const_iterator ph_end(photons->end());

  const xAOD::MuonContainer *muons(0);
  ATH_CHECK(evtStore()->retrieve(muons, m_muonSGKey), false);
  xAOD::MuonContainer::const_iterator mu_itr(muons->begin());
  xAOD::MuonContainer::const_iterator mu_end(muons->end());

  m_e_passSingleMuonPreselect = false;

  for( ; ph_itr != ph_end; ++ph_itr){
    if(PhotonPreselect(*ph_itr)){
      for( ; mu_itr != mu_end; ++mu_itr){
        if(MuonPreselect(*mu_itr)){
          m_e_passSingleMuonPreselect = true;
        }
      }
    }
  }


  if(m_e_passSingleMuonPreselect) m_n_passSingleMuonPreselect++;
  return m_e_passSingleMuonPreselect;
}

bool DerivationFramework::SkimmingToolHIGG1::ElectronPreselect(const xAOD::Electron *el) const {

  if (!el) return false;

  bool val(false);
  bool defined(false);
  
  if(el->isAvailable<char>("DFCommonElectronsLHLoose")){
    defined = true;
    val = static_cast<bool>(el->auxdata<char>("DFCommonElectronsLHLoose"));
  }
  else{
    defined = el->passSelection(val, "Loose");
  }    
  if(!defined || !val) return false;

  double eta = fabs(el->eta());
  double pt = el->pt();

  if (eta > m_maxEta) return false;
  if (m_removeCrack && 1.37 <= eta && eta <= 1.52) return false;
  if (pt <= m_minElectronPt) return false;

  return true;

}

bool DerivationFramework::SkimmingToolHIGG1::MuonPreselect(const xAOD::Muon *mu) const {

  if (!mu) return false;
  double eta = fabs(mu->eta());
  double pt = mu->pt();

  if (eta > m_maxMuonEta) return false;
  if (pt <= m_minMuonPt) return false;

  return true;

}

const double DerivationFramework::SkimmingToolHIGG1::s_MZ = 91187.6*CLHEP::MeV; 



