///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// METMuonAssociator.cxx 
// Implementation file for class METMuonAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METMuonAssociator.h"

// Muon EDM
#include "xAODMuon/MuonContainer.h"

// Tracking EDM
#include "xAODTracking/Vertex.h"

// DeltaR calculation
#include "FourMomUtils/xAODP4Helpers.h"

#include "CaloClusterMatching/ICaloClusterMatchingTool.h"

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METMuonAssociator::METMuonAssociator(const std::string& name) : 
    AsgTool(name),
    METAssociator(name)
  {
    declareProperty("DoClusterMatch", m_doMuonClusterMatch=true);
  }

  // Destructor
  ///////////////
  METMuonAssociator::~METMuonAssociator()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METMuonAssociator::initialize()
  {
    ATH_CHECK( METAssociator::initialize() );
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  StatusCode METMuonAssociator::finalize()
  {
    ATH_MSG_VERBOSE ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
  // executeTool
  ////////////////
  StatusCode METMuonAssociator::executeTool(xAOD::MissingETContainer* /*metCont*/, xAOD::MissingETAssociationMap* metMap) const
  {
    ATH_MSG_VERBOSE ("In execute: " << name() << "...");

    const MuonContainer* muonCont(0);
    if( evtStore()->retrieve(muonCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input muon container " << m_input_data_key);
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Successfully retrieved muon collection");
    if (fillAssocMap(metMap,muonCont).isFailure()) {
      ATH_MSG_WARNING("Unable to fill map with muon container " << m_input_data_key);
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  //*********************************************************************************************************
  // Get constituents
  StatusCode METMuonAssociator::extractTopoClusters(const xAOD::IParticle* obj,
						    std::vector<const xAOD::IParticle*>& tclist,
						    const met::METAssociator::ConstitHolder& /*constits*/) const
  {
    const xAOD::Muon *mu = static_cast<const xAOD::Muon*>(obj);
    const CaloCluster* muclus = mu->cluster();
    if(muclus && m_doMuonClusterMatch) {
      ATH_MSG_VERBOSE("Muon " << mu->index() << " with pt " << mu->pt()
		   << ", eta "   << mu->eta()
		   << ", phi " << mu->phi()
		   << " has cluster with "
		   << "eta "   << muclus->calEta()
		   << ", phi " << muclus->calPhi()
		   << ", E "   << muclus->calE()
		   << " formed of " << muclus->size() << " cells.");
      ATH_MSG_VERBOSE("Muon Eloss type: " << mu->energyLossType()
		   << " Eloss: " << mu->floatParameter(xAOD::Muon::EnergyLoss)
		   << " MeasuredEloss: " << mu->floatParameter(xAOD::Muon::MeasEnergyLoss)
		   << " FSR E: " << mu->floatParameter(xAOD::Muon::FSR_CandidateEnergy) );
      
      static const SG::AuxElement::ConstAccessor<std::vector<ElementLink<CaloClusterContainer> > > tcLinkAcc("constituentClusterLinks");
      for(const auto& matchel : tcLinkAcc(*muclus)) {
	if(!matchel.isValid()) {continue;} // In case of thinned cluster collection
	ATH_MSG_VERBOSE("Tool found cluster " << (*matchel)->index() << " with pt " << (*matchel)->pt() );
	if((*matchel)->e()>1e-9) { // +ve E
	  tclist.push_back(*matchel);
	}
      }
    } // muon has linked cluster
    
    return StatusCode::SUCCESS;
  }

  StatusCode METMuonAssociator::extractTracks(const xAOD::IParticle *obj,
					      std::vector<const xAOD::IParticle*>& constlist,
					      const met::METAssociator::ConstitHolder& constits) const
  {
    const xAOD::Muon *mu = static_cast<const xAOD::Muon*>(obj);
    const TrackParticle* idtrack = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    if(idtrack && acceptTrack(idtrack,constits.pv) && isGoodEoverP(idtrack)) {
    // if(idtrack && acceptTrack(idtrack,pv)) {
      ATH_MSG_VERBOSE("Accept muon track " << idtrack << " px, py = " << idtrack->p4().Px() << ", " << idtrack->p4().Py());
      ATH_MSG_VERBOSE("Muon ID track ptr: " << idtrack);
      constlist.push_back(idtrack);
      // if(mu->pt()>10e3 && (mu->muonType()==xAOD::Muon::Combined || mu->muonType()==xAOD::Muon::SegmentTagged)) {
      //   mutracks.push_back(idtrack);
      // }
    }
    return StatusCode::SUCCESS;
  }

  //*********************************************************************************************************
  // Get constituents
  StatusCode METMuonAssociator::extractPFO(const xAOD::IParticle* obj,
					   std::vector<const xAOD::IParticle*>& pfolist,
					   const met::METAssociator::ConstitHolder& constits,
					   std::map<const IParticle*,MissingETBase::Types::constvec_t>& /*momenta*/) const
  {  
    const xAOD::Muon *mu = static_cast<const xAOD::Muon*>(obj);
    const TrackParticle* idtrack = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    const CaloCluster* muclus = mu->cluster();

    ATH_MSG_VERBOSE("Muon " << mu->index() << " with pt " << mu->pt()
		    << ", eta "   << mu->eta()
		    << ", phi " << mu->phi());
    if(muclus) {
      ATH_MSG_VERBOSE(" has cluster with "
		      << "eta "   << muclus->calEta()
		      << ", phi " << muclus->calPhi()
		      << ", E "   << muclus->calE()
		      << " formed of " << muclus->size() << " cells.");
    }
    ATH_MSG_VERBOSE("Muon Eloss type: " << mu->energyLossType()
		    << " Eloss: " << mu->floatParameter(xAOD::Muon::EnergyLoss)
		    << " MeasuredEloss: " << mu->floatParameter(xAOD::Muon::MeasEnergyLoss)
		    << " FSR E: " << mu->floatParameter(xAOD::Muon::FSR_CandidateEnergy) );

    // One loop over PFOs
    for(const auto& pfo : *constits.pfoCont) {
      if(pfo->isCharged()) {
	// get charged PFOs by matching the muon ID track
	// We set a small -ve pt for cPFOs that were rejected
	// by the ChargedHadronSubtractionTool
	const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");
	if(idtrack && pfo->track(0) == idtrack && PVMatchedAcc(*pfo) &&
	   ( !m_cleanChargedPFO || isGoodEoverP(pfo->track(0)) )
	   ) {
	  ATH_MSG_VERBOSE("Accept muon PFO " << pfo << " px, py = " << pfo->p4().Px() << ", " << pfo->p4().Py());
	  ATH_MSG_VERBOSE("Muon PFO index: " << pfo->index() << ", pt: " << pfo->pt() << ", eta: " << pfo->eta() << ", phi: " << pfo->phi() );
	  ATH_MSG_VERBOSE("Muon ID Track index: " << idtrack->index() << ", pt: " << idtrack->pt() << ", eta: " << idtrack->eta() << ", phi: " << idtrack->phi() );
	  pfolist.push_back(pfo);
	  break;
	} // track match
      } else {
      	// get neutral PFOs by matching the muon cluster
      	if(muclus && m_doMuonClusterMatch) {
      
      	  static const SG::AuxElement::ConstAccessor<std::vector<ElementLink<CaloClusterContainer> > > tcLinkAcc("constituentClusterLinks");
      	  for(const auto& matchel : tcLinkAcc(*muclus)) {
	    if(!matchel.isValid()) {
	      ATH_MSG_DEBUG("Invalid muon-cluster elementLink");
	    } else {
	      if((*matchel)->e()>FLT_MIN && pfo->cluster(0) == *matchel) { // +ve E && matches cluster
		ATH_MSG_VERBOSE("Tool found cluster " << (*matchel)->index() << " with pt " << (*matchel)->pt() );
		pfolist.push_back(pfo);
	      }
	    }
      	  }
      	} // muon has linked cluster
      } 
    } // end of cluster loop

    return StatusCode::SUCCESS;
  }


  // extractPFO for W precision-type measurements
  StatusCode METMuonAssociator::extractPFOHR(const xAOD::IParticle* obj,
                                            std::vector<const xAOD::IParticle*> hardObjs,
                                            std::vector<const xAOD::IParticle*>& pfolist,
                                            const met::METAssociator::ConstitHolder& constits,
                                            std::map<const IParticle*,MissingETBase::Types::constvec_t> & /*momenta*/,
                                            float& UEcorr) const
  {
    const xAOD::Muon* mu = static_cast<const xAOD::Muon*>(obj);

    // Get PFOs associated to muons
    for(const auto& pfo : *constits.pfoCont) {      
      if( pfo->isCharged()) { // Fill list with charged PFOs (using muon tracks)
        const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");
        if( mu && P4Helpers::isInDeltaR(*pfo, *mu, m_Drcone, m_useRapidity) && PVMatchedAcc(*pfo) &&
          ( !m_cleanChargedPFO || isGoodEoverP(pfo->track(0)) ) ){
          pfolist.push_back(pfo);
        }
      } 
      else{ // Fill list with neutral PFOs (using muon clusters)
        if( mu && P4Helpers::isInDeltaR(*pfo, *mu, m_Drcone, m_useRapidity) ){
          pfolist.push_back(pfo);
        }
      } // neutral PFO condition
    } // loop over all PFOs


    // Calculating UE energy correction for a given lepton (using mu)
    if(mu){
      // Vectoral sum of all PFOs 
      TLorentzVector HR;  // uncorrected HR (initialized with 0,0,0,0 automatically)
      for(const auto& pfo_itr : *constits.pfoCont) {
        if( pfo_itr->pt() < 0 || pfo_itr->e() < 0 ) { // sanity check
          continue;
        }
        HR += pfo_itr->p4();
      }

      // Create vectors of muons     
      std::vector<const xAOD::Muon*> v_mu;
      for(const auto& obj_i : hardObjs) {
        if(obj_i->pt()<5e3 && obj_i->type() != xAOD::Type::Muon) { // sanity check
          continue;
        }
        const xAOD::Muon* mu_curr = static_cast<const xAOD::Muon*>(obj_i); // current muon
        v_mu.push_back(mu_curr);
      }

      // Subtracting PFOs matched to muons from HR
      for(const auto& pfo_i : *constits.pfoCont) { // charged and neutral PFOs
        if( pfo_i->pt() < 0 || pfo_i->e() < 0 ) { // sanity check
          continue;
        }
        for(const auto& mu_i : v_mu) { // loop over muons
          double dR = P4Helpers::deltaR( pfo_i->eta(), pfo_i->phi(), mu_i->eta(), mu_i->phi() );
          if( dR < m_Drcone ) { // if PFO is in a cone around muon
            HR -= pfo_i->p4();
          } // cone requirement
        } // over v_mu
      } // over PFOs

      // Save v_mu as a vector TLV (as commonn type for electrons and muons)
      std::vector<TLorentzVector> v_muTLV;  
      for(const auto& mu_i : v_mu) { // loop over v_mu
        v_muTLV.push_back( mu_i->p4() );
      }

      // Save current mu as TLV
      TLorentzVector muTLV = mu->p4();

      // Get UE correction
      ATH_CHECK( GetUEcorr(constits, v_muTLV, muTLV, HR, m_Drcone, m_MinDistCone, UEcorr) );
    } // available mu requirement


    return StatusCode::SUCCESS;
  }
  // **********************************


} // namespace met


