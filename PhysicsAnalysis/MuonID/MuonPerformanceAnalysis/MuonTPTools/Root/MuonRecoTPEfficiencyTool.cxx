/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZmumuMuonTPEfficiencyTool.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#include "MuonTPTools/MuonRecoTPEfficiencyTool.h"

MuonRecoTPEfficiencyTool::MuonRecoTPEfficiencyTool(std::string myname)
: AsgTool(myname), MuonTPEfficiencyTool(myname) {
	declareProperty("MatchToAnyMS",m_match_MS=false);
	declareProperty("MatchToCB",m_match_CB=false);
    declareProperty("MatchToLoose",m_match_Loose=false);
	declareProperty("MatchToMedium",m_match_Medium=false);
    declareProperty("MatchToLoose_noCaloTag",m_match_Loose_noCT=false);
    declareProperty("MatchToTight",m_match_Tight=false);
	declareProperty("MatchToID",m_match_ID=false);
	declareProperty("MatchToCaloTag",m_match_CaloTag=false);
	declareProperty("IDhitCut",m_do_IDHits=true);

    declareProperty("MatchToMuidCB",m_match_MuidCB=false);
    declareProperty("MatchToSTACO",m_match_STACO=false);
    declareProperty("MatchToMuTag",m_match_MuTag=false);
    declareProperty("MatchToMuTagIMO",m_match_MuTagIMO=false);
    declareProperty("MatchToMuidSA",m_match_MuidSA=false);
    declareProperty("MatchToMuGirl",m_match_MuGirl=false);
    declareProperty("MatchToMuGirlLowBeta",m_match_MuGirlLowBeta=false);
    declareProperty("MatchToCaloLikelihood",m_match_CaloLikelihood=false);
    declareProperty("MatchToExtrapolateToIP",m_match_ExtrapolateToIP=false);

}
void MuonRecoTPEfficiencyTool::dRMatching(ProbeContainer* probes, const xAOD::IParticleContainer* matches) const
{
  // loop over probes

  for(auto probe : *probes) {
    double dRMin = 1;
    Probe* matchProbe = 0;
    const xAOD::Muon* best_match_muon = NULL;

    // loop over matches
    for(auto match : *matches) {
      // Pt Cut
      if(match->pt() < m_matchPtCut) continue;

      // Eta Cut
      if(fabs(match->eta()) > m_matchEtaCut) continue;

      if (!GoodMatchMuonType(match)) continue;

      xAOD::Muon* matchmuon = dynamic_cast <xAOD::Muon*>(match);
      if (matchmuon && m_do_IDHits){
    	  if (!m_selection_tool->passedIDCuts(*matchmuon)) continue;
      }
      // ID tracks

      if (m_match_ID){
          xAOD::TrackParticle *trk = dynamic_cast<xAOD::TrackParticle*>(match);
          if(!trk || (m_do_IDHits && !m_selection_tool->passedIDCuts(*trk))) continue;
      }

      // Calculate dR
      double dR = deltaR(probe, match);

      // test pointer-level matching! 
      if(dR < dRMin) {
		dRMin = dR;
		matchProbe = probe;
        if (matchmuon) best_match_muon = matchmuon;
      }
    }

    // check if a matched probe is found
    probe->sfweight(1.);
    probe->isMatched(matchProbe && dRMin<m_maximumDrCut);
    if (m_do_sf && best_match_muon && probe->isMatched()){
        float sf = 1.;
        if (m_sf_tool->getEfficiencyScaleFactor(*best_match_muon,sf) == CP::CorrectionCode::Ok){
            probe->sfweight(sf);
        }
    }
  }
}
bool MuonRecoTPEfficiencyTool::GoodMatchMuonType(const xAOD::IParticle* probe) const{

    const xAOD::Muon* mumatch = dynamic_cast <const xAOD::Muon*> (probe);

    // if the particle is not a muon, assume that the user knows what TrackParticleContainer he is providing!
    if (! mumatch) return true;

    // otherwise, we have to manually pick the right probe

    // ID Track
    if (m_match_ID){
        return (mumatch->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) != NULL);
    }
    // CT Match

    if (m_match_CaloTag){
        return (mumatch &&  m_selection_tool->passedCaloTagQuality(*mumatch) &&  mumatch->isAuthor(xAOD::Muon::CaloTag) );
    }
    // MS Match
    if (m_match_MS){
        return ((mumatch->muonType() == xAOD::Muon::MuonStandAlone || mumatch->muonType() == xAOD::Muon::Combined) && mumatch->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle)!= NULL);
    }
    // CB Match
    if (m_match_CB){
        return (mumatch->muonType() == xAOD::Muon::Combined && mumatch->trackParticle(xAOD::Muon::CombinedTrackParticle) != NULL);
    }
    if (m_match_Loose){
        bool ok = (m_selection_tool->getQuality(*mumatch) <= xAOD::Muon::Loose );
        return ok;
    }
    if (m_match_Loose_noCT){
        bool ok = (m_selection_tool->getQuality(*mumatch) <= xAOD::Muon::Loose && mumatch->muonType() != xAOD::Muon::CaloTagged  );
        return ok;
    }
    if (m_match_Medium){
        bool ok = (m_selection_tool->getQuality(*mumatch) <= xAOD::Muon::Medium );
        return ok;
    }
    if (m_match_Tight){
        bool ok = (m_selection_tool->getQuality(*mumatch) <= xAOD::Muon::Tight );
        return ok;
    }
    if (m_match_MuidCB){
        return (mumatch->isAuthor(xAOD::Muon::MuidCo));
    }
    if (m_match_STACO){
        return (mumatch->isAuthor(xAOD::Muon::STACO));
    }
    if (m_match_MuTag){
        return (mumatch->isAuthor(xAOD::Muon::MuTag));
    }
    if (m_match_MuTagIMO){
        return (mumatch->isAuthor(xAOD::Muon::MuTagIMO));
    }
    if (m_match_MuidSA){
        return (mumatch->isAuthor(xAOD::Muon::MuidSA));
    }
    if (m_match_MuGirl){
        return (mumatch->isAuthor(xAOD::Muon::MuGirl));
    }
    if (m_match_MuGirlLowBeta){
        return (mumatch->isAuthor(xAOD::Muon::MuGirlLowBeta));
    }
    if (m_match_CaloLikelihood){
        return (mumatch->isAuthor(xAOD::Muon::CaloLikelihood));
    }
    if (m_match_ExtrapolateToIP){
        return (mumatch->isAuthor(xAOD::Muon::ExtrapolateMuonToIP));
    }
    return false;
}
