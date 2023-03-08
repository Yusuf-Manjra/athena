/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   MuonLRTOverlapRemovalTool
//   Author: Max Goblirsch, goblirsc@SPAMNOT_CERN.ch

#include "LRTMuonAnalysisTools/MuonLRTOverlapRemovalTool.h"
#include <algorithm>
#include <AsgTools/AsgToolConfig.h>

namespace CP {

  MuonLRTOverlapRemovalTool::MuonLRTOverlapRemovalTool(const std::string& name) :
    asg::AsgTool(name)
  {
    // nothing to do here
  }

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////

  StatusCode MuonLRTOverlapRemovalTool::initialize()
  {
    if(m_muonSelectionTool.empty()){
      asg::AsgToolConfig config("CP::MuonSelectionTool/MuonSelectionTool");
      ATH_CHECK(config.setProperty("TurnOffMomCorr",true));
      ATH_CHECK(config.setProperty("IsRun3Geo",m_useRun3WP.value()));
      ATH_CHECK(config.makePrivateTool(m_muonSelectionTool));
    }

    ATH_MSG_DEBUG("retrieving Muon selection tool");
    ATH_CHECK( m_muonSelectionTool.retrieve() );
    return StatusCode::SUCCESS;
  }

  //////////////////////////////////////////////////////////////////////
  // Check overlap between the muon collections and decorate duplicates
  //////////////////////////////////////////////////////////////////////
  void MuonLRTOverlapRemovalTool::checkOverlap( const xAOD::MuonContainer & promptMuonCol,
                                                const xAOD::MuonContainer & LRTMuonCol,
                                                std::vector<bool>& promptMuonsSelectedToKeep,
                                                std::vector<bool>& lrtMuonsSelectedToKeep ) const
  {

    /// pre-fill vectors with the default 'accept' decision
    promptMuonsSelectedToKeep.resize(promptMuonCol.size(), true); 
    lrtMuonsSelectedToKeep.resize(LRTMuonCol.size(), true); 
    std::fill(promptMuonsSelectedToKeep.begin(), promptMuonsSelectedToKeep.end(), true); 
    std::fill(lrtMuonsSelectedToKeep.begin(), lrtMuonsSelectedToKeep.end(), true); 

    // loop over prompt muons
    for (const xAOD::Muon* promptMuon : promptMuonCol){
      // loop over lrt muons
      for( const xAOD::Muon* lrtMuon : LRTMuonCol){
        // check for overlap
        std::pair<bool,bool> writeDecision = {true,true};
        switch(m_strategy){
          case CP::IMuonLRTOverlapRemovalTool::defaultStrategy:
            /// Baseline strategy
            if(hasOverlap(promptMuon,lrtMuon)) { writeDecision = resolveOverlap(promptMuon, lrtMuon); }
            break;
          case CP::IMuonLRTOverlapRemovalTool::passThroughAndDecorate:
            /// passThroughAndDecorate strategy
            checkOverlapAndDecor(promptMuon, lrtMuon);
            break;
          default:
            ATH_MSG_FATAL("Unsupported overlap removal strategy type. Choose from 0 (`defaultStrategy`) or 1 (`passThroughAndDecorate`)");
            break;
        }
        // write decision into vectors
        if(!writeDecision.first){
          promptMuonsSelectedToKeep.at(promptMuon->index()) = false;
        }
        if(!writeDecision.second){
          lrtMuonsSelectedToKeep.at(lrtMuon->index()) = false;
        }
      }
    }
  }

  bool MuonLRTOverlapRemovalTool::hasOverlap(const xAOD::Muon* promptMuon,
                                             const xAOD::Muon* lrtMuon) const{

    // we compare based on MS track information to detect re-use of the same track  
    const xAOD::TrackParticle* lrtMsTrack = lrtMuon->trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );
    const xAOD::TrackParticle* promptMsTrack = promptMuon->trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle );

    // baseline case: if no two MS tracks or two different MS tracks, no overlap possible 
    if ( (!promptMsTrack && !lrtMsTrack) || (promptMsTrack != lrtMsTrack)){
      return false;
    }

    else {
      ATH_MSG_DEBUG("Found an overlap, solving");
      ATH_MSG_DEBUG("  Prompt muon has author "<< promptMuon->author()<<", type "<<promptMuon->muonType()<<", pT "<<promptMuon->pt()<<", eta "<<promptMuon->eta()<<", phi "<<promptMuon->phi());
      ATH_MSG_DEBUG("  LRT muon has author "<< lrtMuon->author()<<", type "<<lrtMuon->muonType()<<", pT "<<lrtMuon->pt()<<", eta "<<lrtMuon->eta()<<", phi "<<lrtMuon->phi());
      return true;
    }
  }

  std::pair<bool, bool> MuonLRTOverlapRemovalTool::resolveOverlap(const xAOD::Muon* promptMuon,
                                                                  const xAOD::Muon* lrtMuon) const{

    // apply the loosest available ID to resolve most overlaps using existing MCP recommendations
    bool promptPassQuality = (m_muonSelectionTool->getQuality(*promptMuon) < xAOD::Muon::VeryLoose);
    bool lrtPassQuality = (m_muonSelectionTool->getQuality(*lrtMuon) < xAOD::Muon::VeryLoose);

    if (promptPassQuality && !lrtPassQuality) { 
      return {true,false}; 
    }
    else if (!promptPassQuality && lrtPassQuality) {
      return {false,true};
    }

    // still here? Next prefer combined muons over others 
    bool promptIsCombined = promptMuon->muonType() == xAOD::Muon::Combined;
    bool lrtIsCombined = lrtMuon->muonType() == xAOD::Muon::Combined;

    if (promptIsCombined && !lrtIsCombined) {
      return {true,false};
    }
    else if (!promptIsCombined && lrtIsCombined) {
      return {false,true};
    }

    // still here? Next choose the muon with a lower ID-ME delta eta value
    float promptIDMEdEta = getIDMEdEta(promptMuon);
    float lrtIDMEdEta    = getIDMEdEta(lrtMuon);

    if (promptIDMEdEta <= lrtIDMEdEta) {
      return {true,false};
    }
    else {
      return {false,true};
    }

    // fail-safe case: choose prompt over LRT.
    ATH_MSG_DEBUG("Resolution reached the fail-safe point. Why?");
    return {true,false};
  }

  void MuonLRTOverlapRemovalTool::checkOverlapAndDecor(const xAOD::Muon* promptMuon,
                                                       const xAOD::Muon* lrtMuon) const{

    static const SG::AuxElement::Decorator<int> MuonLRTOverlapDecision("MuonLRTOverlapDecision"); //0 if no overlap, 1 if overlaps and rejected, 2 if overlaps and retained

    if (!hasOverlap(promptMuon, lrtMuon)){
      MuonLRTOverlapDecision(*promptMuon) = 0;
      MuonLRTOverlapDecision(*lrtMuon) = 0;
    }
    else {
      std::pair<bool, bool> overlapDecision = resolveOverlap(promptMuon, lrtMuon);
      if (overlapDecision.first && !overlapDecision.second) {
        MuonLRTOverlapDecision(*promptMuon) = 2;
        MuonLRTOverlapDecision(*lrtMuon) = 1;
      }
      else {
        MuonLRTOverlapDecision(*promptMuon) = 1;
        MuonLRTOverlapDecision(*lrtMuon) = 2;
      }
    }
  }

  float MuonLRTOverlapRemovalTool::getIDMEdEta(const xAOD::Muon* muon) const{
    const xAOD::TrackParticle* ID_track = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
    const xAOD::TrackParticle* ME_track = muon->trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
    if (!ID_track || !ME_track) return FLT_MAX;
    return ( std::abs( ID_track->eta() - ME_track->eta() ) );
  }

} // end namespace CP
