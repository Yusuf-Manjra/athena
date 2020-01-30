/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/JSSTaggerBase.h"

JSSTaggerBase::JSSTaggerBase(const std::string &name) :
  asg::AsgTool(name),
  m_jetPtMin(200000.),
  m_jetPtMax(3000000.),
  m_jetEtaMax(2.0),
  m_decorate(true),
  m_calibarea(""),
  m_truthLabelDecorationName("FatjetTruthLabel"),
  m_dR_truthJet(0.75),
  m_dR_truthPart(0.75),
  m_mLowTop(140.),
  m_mHighTop(-1),
  m_mLowW(50.),
  m_mHighW(100.),
  m_mLowZ(60.),
  m_mHighZ(110.){}


void JSSTaggerBase::showCuts() const{
  int nCuts = m_accept.getNCuts();
  for(int iCut=0; iCut<nCuts; iCut++){
    std::string cut_string = "";
    cut_string += "  (";
    cut_string += std::to_string(iCut);
    cut_string += ")  ";
    cut_string += m_accept.getCutName(iCut).Data();
    cut_string += " : ";
    cut_string += m_accept.getCutDescription(iCut).Data();
    ATH_MSG_INFO( cut_string );
  }
}

int JSSTaggerBase::getFatjetContainment(const xAOD::Jet& jet, const xAOD::TruthParticleContainer* truthPartsW, const xAOD::TruthParticleContainer* truthPartsZ, const xAOD::TruthParticleContainer* truthPartsTop, const xAOD::TruthParticleContainer* truthPartsH, bool isSherpa, double dRmax,  double mLowTop, double mHighTop, double mLowW, double mHighW, double mLowZ, double mHighZ) const {

    bool isMatchW=false;
    bool isMatchZ=false;
    bool isMatchH=false;
    bool isMatchTop=false;

    // find W/Z boson matched to the jet with dR<dRmax
    if ( isSherpa ) {
      int isMatchWZ=matchToWZ_Sherpa(jet, truthPartsW, dRmax);
      isMatchW=(isMatchWZ==1);
      isMatchZ=(isMatchWZ==2);
    } else {
      for( const xAOD::TruthParticle* part : *truthPartsW ){
	if ( fabs(part->pdgId()) != 24  ) continue; // W
	if ( part->nChildren() == 1 ) continue; // skip self-decay
	
	if( part->p4().DeltaR(jet.p4()) < dRmax ) {	
	  isMatchW=true;
	  break;
	}
      }

      for( const xAOD::TruthParticle* part : *truthPartsZ ){
	if ( fabs(part->pdgId()) != 23  ) continue; // Z
	if ( part->nChildren() == 1 ) continue; // skip self-decay
	
	if( part->p4().DeltaR(jet.p4()) < dRmax ) {	
	isMatchZ=true;
	break;
	}
      }
    }

    // find top quark matched to the jet with dR<dRmax
    for( const xAOD::TruthParticle* part : *truthPartsTop ){
      if ( fabs(part->pdgId()) != 6 ) continue; // top
      if ( part->nChildren() == 1 ) continue; // skip self-decay

      if( part->p4().DeltaR(jet.p4()) < dRmax ) {
	isMatchTop=true;
	break;
      }
    }

    for( const xAOD::TruthParticle* part : *truthPartsH ){
      if ( fabs(part->pdgId()) != 25  ) continue; // H
      if ( part->nChildren() == 1 ) continue; // skip self-decay

      if( part->p4().DeltaR(jet.p4()) < dRmax ) {
        isMatchH=true;
        break;
      }
    }

    // find B-hadron matched to the jet with dR<dRmaxB
    int nMatchB=0;
    if( !jet.getAttribute<int>( "GhostBHadronsFinalCount", nMatchB ) ){

      std::vector<const xAOD::TruthParticle*> ghostB; // Ghost associated B hadrons after FSR
      if( !jet.getAssociatedObjects<xAOD::TruthParticle>( "GhostBHadronsFinal", ghostB ) ){      
	ATH_MSG_ERROR("GhostBHadronsFinal cannot be retrieved! Truth label definition of W/top tagger might be wrong");
      } 
      nMatchB=ghostB.size();
    }

    if( !isMatchTop && !isMatchW && !isMatchZ && !isMatchH){
      return FatjetTruthLabel::enumToInt(FatjetTruthLabel::qcd);
    }

    if(isMatchH){
      if(nMatchB > 1) return FatjetTruthLabel::enumToInt(FatjetTruthLabel::Hbb);
      else return FatjetTruthLabel::enumToInt(FatjetTruthLabel::other_From_H);
    }
    else if( isMatchTop && nMatchB>0 &&
	mLowTop < jet.m()*0.001 && (mHighTop<0 || jet.m()*0.001 < mHighTop) ) { // if mHighTop<0, we don't apply the upper cut on jet mass
      return FatjetTruthLabel::enumToInt(FatjetTruthLabel::tqqb); 
    }else if( isMatchW && nMatchB==0 &&
	      mLowW < jet.m()*0.001 && jet.m()*0.001 < mHighW ){
      if ( isMatchTop ) {
	return FatjetTruthLabel::enumToInt(FatjetTruthLabel::Wqq_From_t);
      }else{
	return FatjetTruthLabel::enumToInt(FatjetTruthLabel::Wqq);
      } 
    }else if( isMatchZ && 
	      mLowZ < jet.m()*0.001 && jet.m()*0.001 < mHighZ ){
      return FatjetTruthLabel::enumToInt(FatjetTruthLabel::Zqq);
    }else{
      if ( isMatchTop ){
	return FatjetTruthLabel::enumToInt(FatjetTruthLabel::other_From_t); 
      }else{
	return FatjetTruthLabel::enumToInt(FatjetTruthLabel::other_From_V);
      }
    }
    
    return FatjetTruthLabel::enumToInt(FatjetTruthLabel::qcd);
}

int JSSTaggerBase::matchToWZ_Sherpa(const xAOD::Jet& jet,
				     const xAOD::TruthParticleContainer* truthParts,
				     double dRmax) const {
  // 1: W, 2: Z, 0: others
  int countStatus3=0;
  TLorentzVector p_1(0,0,0,0);
  TLorentzVector p_2(0,0,0,0);
  TLorentzVector truthWZcand(0,0,0,0);
  bool isWcand=false;
  bool isZcand=false;
  for ( unsigned int ipart = 0; ipart < truthParts->size(); ipart++ ){
    const xAOD::TruthParticle* part1=truthParts->at(ipart);
    if ( part1->status()!=3 ) continue;
    if ( fabs(part1->pdgId()) > 5 ) continue;
    countStatus3++;
    if ( countStatus3 > 3 ) continue; // want to look at first 2 partons except beam particles. sometimes beam particles are dropped from DxAODs...
    p_1=part1->p4();    
    
    // Find the next particle in the list with status==3.
    isWcand=false;
    isZcand=false;
    for ( unsigned int jpart = ipart+1; jpart < truthParts->size(); jpart++ ){
      const xAOD::TruthParticle* part2=truthParts->at(jpart);
      if ( part2->status()!=3 ) continue;
      if ( fabs(part2->pdgId()) > 5 ) continue;
      p_2=part2->p4();
      if ( part1->pdgId() + part2->pdgId() == 0 ) {
	isZcand=true; // daughters of Z decay should be same-flavor but opposite charge
      }else{
	isWcand=true;
      }
      break; // if p_1 is a daughter of W/Z decay, the next one is the other daughter
    }
    truthWZcand=p_1+p_2;
    if ( 60000 < truthWZcand.M() && truthWZcand.M() < 140000. ) break; // ~98% efficiency to W/Z signals. (95% if it is changed to [65, 105]GeV and 90% if [70,100]GeV)
  }

  if ( truthWZcand.DeltaR(jet.p4()) < dRmax ) {
    if ( isWcand ) return 1;
    if ( isZcand ) return 2;
  }
  return 0;
}

StatusCode JSSTaggerBase::decorateTruthLabel(const xAOD::Jet& jet) const {
  
  int channelNumber = -999;

  if(m_DSID == -1){

    //Get the EventInfo to identify Sherpa samples if DSID is not specified by user

    const xAOD::EventInfo * eventInfo = 0;
    if ( evtStore()->retrieve(eventInfo,"EventInfo").isFailure() || !eventInfo ) {
      ATH_MSG_ERROR("   BoostedJetTaggers::decorateTruthLabel : Failed to retrieve event information.");
      return StatusCode::FAILURE;
    }
  
    channelNumber = eventInfo->mcChannelNumber();
  }
  else
    channelNumber = m_DSID;

  const xAOD::JetContainer* truthJet=nullptr;
  if( evtStore()->contains<xAOD::TruthParticleContainer>( m_truthWBosonContainerName ) ){
    // TRUTH3
    ATH_CHECK( evtStore()->retrieve(truthJet, m_truthJetContainerName) );
    const xAOD::TruthParticleContainer* truthPartsW=nullptr;
    ATH_CHECK(evtStore()->retrieve(truthPartsW, m_truthWBosonContainerName));
    const xAOD::TruthParticleContainer* truthPartsZ=nullptr;
    ATH_CHECK(evtStore()->retrieve(truthPartsZ, m_truthZBosonContainerName));
    const xAOD::TruthParticleContainer* truthPartsTop=nullptr;
    ATH_CHECK(evtStore()->retrieve(truthPartsTop, m_truthTopQuarkContainerName));
    const xAOD::TruthParticleContainer* truthPartsH=nullptr;
    ATH_CHECK(evtStore()->retrieve(truthPartsH, m_truthHBosonContainerName));
    return decorateTruthLabel(jet, truthPartsW, truthPartsZ, truthPartsTop, truthPartsH, truthJet, m_truthLabelDecorationName,
			      m_dR_truthJet, m_dR_truthPart, m_mLowTop, m_mHighTop, m_mLowW, m_mHighW, m_mLowZ, m_mHighZ, channelNumber);
  }else if( evtStore()->contains<xAOD::TruthParticleContainer>( m_truthParticleContainerName ) ){    
    // TRUTH1
    ATH_CHECK( evtStore()->retrieve(truthJet, m_truthJetContainerName) );
    const xAOD::TruthParticleContainer* truthParts=nullptr;
    ATH_CHECK(evtStore()->retrieve(truthParts, m_truthParticleContainerName));
    return decorateTruthLabel(jet, truthParts, truthParts, truthParts, truthParts, truthJet, m_truthLabelDecorationName,
			      m_dR_truthJet, m_dR_truthPart, m_mLowTop, m_mHighTop, m_mLowW, m_mHighW, m_mLowZ, m_mHighZ, channelNumber);
  }
  
  return StatusCode::FAILURE;
}

StatusCode JSSTaggerBase::decorateTruthLabel(const xAOD::Jet& jet, const xAOD::TruthParticleContainer* truthPartsW, const xAOD::TruthParticleContainer* truthPartsZ, const xAOD::TruthParticleContainer* truthPartsTop, const xAOD::TruthParticleContainer* truthPartsH, const xAOD::JetContainer* truthJets, std::string decorName, double dRmax_truthJet, double dR_truthPart, double mLowTop, double mHighTop, double mLowW, double mHighW, double mLowZ, double mHighZ, int channelNumber ) const {

  double dRmin=9999;
  const xAOD::Jet* m_truthjet=0;
  for ( const xAOD::Jet* truthjet : *truthJets ) {
    double dR=jet.p4().DeltaR( truthjet->p4() );
    if ( dRmax_truthJet<0 || dR < dRmax_truthJet ) { // if dRmax<0, the closest truth jet is used as matched jet. Otherwise, 
      if( dR < dRmin ){
	dRmin=dR;
	m_truthjet=truthjet;
      }
    }
  }
  int jetContainment=FatjetTruthLabel::enumToInt(FatjetTruthLabel::notruth);
  bool isSherpa=getIsSherpa(channelNumber);
  if ( m_truthjet ) {
    jetContainment=getFatjetContainment(*m_truthjet, truthPartsW, truthPartsZ, truthPartsTop, truthPartsH, isSherpa, /*dR for W/Z/top matching*/dR_truthPart, mLowTop, mHighTop, mLowW, mHighW, mLowZ, mHighZ);
  }
  jet.auxdecor<int>(decorName) = jetContainment;
  
  return StatusCode::SUCCESS;
}
