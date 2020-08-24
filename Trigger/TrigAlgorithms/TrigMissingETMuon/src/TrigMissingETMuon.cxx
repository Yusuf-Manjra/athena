/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigMissingETMuon.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigMissingETMuon
//
// AUTHOR:   Hongtao Yang
// CREATED:  Nov 18, 2011
//
// Xin Chen: Add muon pt cut (>5 GeV) to be consistent with offline 
//           and remove double counting of the same muComb muon (Apr. 24, 2012)
//
// Description: MissingET AllTEAlgo (FEX) algorithm. Adds
//              L2 or EF muon correction
//
// ********************************************************************

#include "TrigMissingETMuon.h"

#include "xAODMuon/MuonContainer.h"

#include "GaudiKernel/SystemOfUnits.h"
#include "TrigT1Interfaces/RecEnergyRoI.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigNavigation/Navigation.h"
#include "CxxUtils/sincos.h"
#include "CxxUtils/sincosf.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "eformat/DetectorMask.h"
#include "eformat/SourceIdentifier.h"

#include <cmath>
#include <cstdio>

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

TrigMissingETMuon::TrigMissingETMuon(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_useCachedResult(false),
    m_cachedTE(0)
{

  declareProperty("METLabel", m_featureLabel = "T2MissingET", "label for the MET feature in the HLT Navigation");
  declareProperty("MuonPtCut", m_muonptcut = 5.0, "Muon Pt threshold");

  declareMonitoredVariable("Muon_Ex",     m_mu_ex);
  declareMonitoredVariable("Muon_Ey",     m_mu_ey);
  declareMonitoredVariable("Muon_SumEt",    m_mu_set);

  declareMonitoredVariable("Muon_Ex_log",      m_mu_ex_log);
  declareMonitoredVariable("Muon_Ey_log",      m_mu_ey_log);
  declareMonitoredVariable("Muon_SumEt_log",    m_mu_set_log);

  /** definition of the meaning for the component flag bits **/
  m_maskErrMuon            = 0x0004; // bit  2
  m_maskCompErrors         = 0x8000; // bit 15
  //   m_maskGlobErrors           = 0x80000000; // bit 31
  m_met=NULL;
}


//////////////////////////////////////////////////////////
HLT::ErrorCode TrigMissingETMuon::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out)
{
  // CACHING
  // first check whether we executed this instance before:
  if (m_useCachedResult) {
    ATH_MSG_DEBUG("Executing this xAOD::TrigMissingETMuon " << name() << " in cached mode");
    // Only count MET as an input TE (for seeding relation of navigation structure)
    HLT::TEVec allTEs;
    if ( (tes_in.size()>0) && (tes_in[0].size()>0) ) allTEs.push_back( tes_in[0][0] );
    // Create an output TE seeded by the inputs
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
    outputTE->setActiveState(true);

    // save (cached) met feature to output TE:
    m_config->getNavigation()->copyAllFeatures( m_cachedTE, outputTE );

    return HLT::OK;
  }

  // event status flag
  int flag=0;

  // start monitoring
  beforeExecMonitors().ignore();

  m_mu_ex = -9e9;
  m_mu_ey = -9e9;
  m_mu_set = -9e9;

  m_mu_ex_log = -9e9;
  m_mu_ey_log = -9e9;
  m_mu_set_log = -9e9;

  ATH_MSG_DEBUG("Executing xAOD::TrigMissingETMuon::hltExecute()" );
  ATH_MSG_DEBUG("REGTEST: tes_in.size() = " );
  for (unsigned u=0; u<tes_in.size(); ++u) {
    ATH_MSG_DEBUG("REGTEST: tes_in[" << u << "].size() = " << tes_in[u].size());
  }

  //algorithm is muon seeded, therefore:
  //    algorithm has TrigMissingET as only seed if in unseeded mode
  //                  muon as second seed of in seeded mode  
  //so number of TEs must be 1 or 2 
  // ======================================================
  bool seeded = false;
  switch(tes_in.size()){
    case 1: seeded = false; ATH_MSG_DEBUG("Running in unseeded mode"); break;
    case 2: seeded = true; ATH_MSG_DEBUG("Running in seeded mode"); break;
    default:
      ATH_MSG_WARNING ( "Configuration error: tes_in.size() is " << tes_in.size()
        << " but can only be 2 in seeded mode or 1 in unseeded mode.  Aborting chain");
        return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::BAD_JOB_SETUP);

  }

  // 0th element in this tes_in vector must have exactly one object
  if (tes_in[0].size() != 1) {
    ATH_MSG_WARNING("Configuration error: expecting exactly 1 TrigMissingET object. Aborting chain");
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::BAD_JOB_SETUP);
  }

  // Muon features? Not present if unseeded, or seeded but tes_in[1].size==0.
  if(!seeded || (seeded && tes_in[1].size()==0))
  {
    ATH_MSG_DEBUG("No muon feature found: skipping muon correction");
    HLT::ErrorCode status = makeOutputTE(tes_in, type_out);
    return status;
  }

  /* If muon features are present,
     then get the stuff and start doing the stuff
   */

  // Retrieve the TrigMissingET object
  std::vector<const xAOD::TrigMissingET*> vectorOfMET;
  for (HLT::TEVec::const_iterator met_it = tes_in[0].begin(); met_it != tes_in[0].end(); ++met_it) {
    HLT::ErrorCode status = getFeatures( (*met_it) , vectorOfMET);
    if(status != HLT::OK) {
      ATH_MSG_ERROR("no TrigMissingET object found for this TE ... ?!?");
      return HLT::NAV_ERROR;
    }
  }

  // Check if EDM is there
  if(vectorOfMET.size() == 0) {
    ATH_MSG_ERROR("vectorOfMET.size() == 0 !");
    return HLT::NAV_ERROR;      
  }

  // ++++++++++++++++++++++++++++++++++++++++++
  const xAOD::TrigMissingET *met_temp=vectorOfMET[0];
  m_met = const_cast<xAOD::TrigMissingET *>(met_temp);
  // ++++++++++++++++++++++++++++++++++++++++++
  ATH_MSG_DEBUG("xAOD::TrigMissingET object retrieved.");

  flag=m_met->flag();
  ATH_MSG_DEBUG("Flag got.");

  // Now we loop over all the muons
  float muon_ex=0;
  float muon_ey=0;
  float muon_ez=0;
  float muon_sum_et=0;
  float muon_sum_e=0;
  //int muon_charge=0;
  unsigned int nMuons=0;
  bool MuonError=false;

  ATH_MSG_DEBUG("REGTEST: Ready to access xAOD::MuonContainer  muons ");

  std::vector<const xAOD::TrackParticle*> vecOfMuonTrk;

  ATH_MSG_DEBUG("REGTEST: Using muon pt threshold "<<m_muonptcut<<" GeV.");

  // Muon TE in tes_in[1] // for some reason \_( )_/
  for (HLT::TEVec::const_iterator muon_it = tes_in[1].begin(); muon_it != tes_in[1].end(); ++muon_it) 
  {
    // Get the muon container linked to the TE
    const xAOD::MuonContainer* muonContainer(0);

    HLT::ErrorCode status = getFeature( (*muon_it) , muonContainer);
  
    if(status != HLT::OK || muonContainer==0) 
    {
      ATH_MSG_DEBUG( "no MuonContainer found for this TE ... ?!?");
      continue;
    }

    // Check that there is at least one muon in TrigMuonEFInfoContainer
    if (muonContainer->size() == 0)
    {
      ATH_MSG_DEBUG("Size of vectorOfMuons is 0. Skipping muon correction");
      MuonError=true;
      continue;
      //       return HLT::NAV_ERROR;
    }

    unsigned int kk=0;
    for(auto muon : *muonContainer) 
    {
      ATH_MSG_DEBUG("Looking at muon " << kk++ << ": (pt, eta, phi) = (" << muon->pt() << ", " << muon->eta() << ", " << muon->phi() << ")");

      // combined or segment tagged muon
      if(muon->muonType() == xAOD::Muon::MuonType::Combined 
          || muon->muonType() == xAOD::Muon::MuonType::SegmentTagged ) 
      { 

        // Minimum pt cut
        float Et = fabs(muon->pt());
        if (Et/Gaudi::Units::GeV < m_muonptcut) continue;

        // Reject muons without IDtracks
        const xAOD::TrackParticle* idtrk = muon->trackParticle( xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle );
        if (!idtrk) continue;

        // Skip double counted muons
        bool doubleCounted=false;
        for(auto existingMuonTrk : vecOfMuonTrk)
          if (existingMuonTrk == idtrk)
          {
            doubleCounted = true;
            break;
          }
        if(doubleCounted) continue;

        vecOfMuonTrk.push_back(idtrk);  // store for double counting prevention

        float eta = muon->eta();
        float phi = muon->phi();
        float cosPhi, sinPhi;
        sincosf(phi, &sinPhi, &cosPhi);
        float Ex = Et*cosPhi;
        float Ey = Et*sinPhi;
        float Ez = Et*sinhf(eta);
        float E = sqrtf(Et*Et + Ez*Ez);

        muon_ex     -= Ex;
        muon_ey     -= Ey;
        muon_ez     -= Ez;
        muon_sum_et += Et;
        muon_sum_e  += E;
        ++nMuons;

        ATH_MSG_DEBUG("Adding muon with (pt, eta, phi) = (" << muon->pt() << ", " << muon->eta() << ", " << muon->phi() << ")");
      } // end if muonType
    } // end for(muon : muonContainer)
  } // end for(te : muonTE's)

  m_mu_ex = muon_ex;
  m_mu_ey = muon_ey;
  m_mu_set = muon_sum_et;
  
  ATH_MSG_DEBUG("Muon correction calculated.");

  ATH_MSG_DEBUG("Setting energies and flag");

  unsigned int muonComp = m_met->getNumberOfComponents() - 1; // Muons are always the last component \_(y)_/
  ATH_MSG_VERBOSE("Outputting met component names:");
  ATH_MSG_VERBOSE("ii \t m_met->nameOfComponent(ii)");
  for(uint ii=0; ii<m_met->getNumberOfComponents(); ii++){
    ATH_MSG_VERBOSE(ii << '\t' << m_met->nameOfComponent(ii));
  }

  // Suggested by Diego: Checking the component name of the input
  // Turns out to be a good test 
  if( (m_met->nameOfComponent(muonComp)).substr(0,4)!="Muon") 
  {
    ATH_MSG_ERROR("Could not find MET container with muon information!!! Exit...");
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::BAD_JOB_SETUP);    
  }
  // Fetch Muon Components and set muon variables
  m_met->setExComponent(muonComp,muon_ex);
  m_met->setEyComponent(muonComp,muon_ey);
  m_met->setEzComponent(muonComp,muon_ez);
  m_met->setSumEtComponent(muonComp,muon_sum_et);
  m_met->setSumEComponent(muonComp,muon_sum_e);
  m_met->setUsedChannelsComponent(muonComp, nMuons);

  if (MuonError){ // set muon error bit 
    flag |= m_maskErrMuon; flag |= m_maskCompErrors;
  }
  m_met->setFlag(flag);
  ATH_MSG_DEBUG( "Setting energies and flag done.");

  if (msgLvl() <= MSG::DEBUG) {
    char buff[128];
    std::snprintf(buff,128,"REGTEST: (Muon FEX) Event status = 0x%08x", (unsigned)flag);
    ATH_MSG_DEBUG( buff );
    ATH_MSG_DEBUG( "REGTEST: (Muon FEX) nMuons = " << nMuons );
    if (nMuons>0) {
      ATH_MSG_DEBUG( "REGTEST: (Muon FEX) muon_ex = " << muon_ex*1e-3 << " GeV" );
      ATH_MSG_DEBUG( "REGTEST: (Muon FEX) muon_ey = " << muon_ey*1e-3 << " GeV" );
      ATH_MSG_DEBUG( "REGTEST: (Muon FEX) muon_ez = " << muon_ez*1e-3 << " GeV" );
      ATH_MSG_DEBUG( "REGTEST: (Muon FEX) muon_sum_et = " << muon_sum_et*1e-3 << " GeV" );
      ATH_MSG_DEBUG( "REGTEST: (Muon FEX) muon_sum_e = " << muon_sum_e*1e-3 << " GeV" );
    }
  }

  // monitoring: log-scale quantities
  float epsilon = 1e-6;  // 1 keV
  m_mu_ex_log  = fabsf( m_mu_ex)>epsilon ? copysign(log10(fabsf( m_mu_ex)),  m_mu_ex) : 0;
  m_mu_ey_log  = fabsf( m_mu_ey)>epsilon ? copysign(log10(fabsf( m_mu_ey)),  m_mu_ey) : 0;
  m_mu_set_log = fabsf(m_mu_set)>epsilon ? copysign(log10(fabsf(m_mu_set)), m_mu_set) : 0;

  HLT::ErrorCode status = makeOutputTE(tes_in, type_out);
  if(status != HLT::OK) return status;
  // stop monitoring
  afterExecMonitors().ignore();

  return HLT::OK;
}

//////////////////////////////////////////////////////////
HLT::ErrorCode TrigMissingETMuon::makeOutputTE(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out) {

  // Only count MET as an input TE (for seeding relation of navigation structure)
  HLT::TEVec allTEs;
  if ( (tes_in.size()>0) && (tes_in[0].size()>0) ) allTEs.push_back( tes_in[0][0] );

  // create output TE:
  // Create an output TE seeded by the inputs
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
  HLT::ErrorCode status = HLT::OK;
  m_config->getNavigation()->copyAllFeatures(tes_in[0][0], outputTE);
  if (status != HLT::OK){
    ATH_MSG_ERROR("Write of xAOD::TrigMissingET feature into outputTE failed");
    return status;
  }

  ATH_MSG_DEBUG("We assume success, set TE with label active to signal positive result.");


  // CACHING
  // if we got here, everything was okay. so, we cache the feature for further execution of this instance in e.g. other MET Sequences:
  ATH_MSG_DEBUG("Updated feature copied to output TE.");
  m_useCachedResult = true;
  m_cachedTE = outputTE;

  return HLT::OK;
}

