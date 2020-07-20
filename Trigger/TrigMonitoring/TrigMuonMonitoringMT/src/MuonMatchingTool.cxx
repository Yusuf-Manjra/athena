/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonMatchingTool.h"
#include "xAODTrigger/MuonRoIContainer.h"

const static double ZERO_LIMIT = 1.e-5;

MuonMatchingTool :: MuonMatchingTool(const std::string& type, const std::string& name, const IInterface*  parent)
  : AthAlgTool(type, name, parent)
{}


StatusCode MuonMatchingTool :: initialize(){

  ATH_CHECK( m_trigDec.retrieve() );
  if(m_use_extrapolator){
    ATH_CHECK( m_extrapolator.retrieve() );
  }
  ATH_CHECK( m_MuonContainerKey.initialize() );
  ATH_CHECK( m_MuonRoIContainerKey.initialize() );

  return StatusCode::SUCCESS;
}


const Amg::Vector3D MuonMatchingTool :: offlineMuonAtPivot(const xAOD::Muon* mu) const{
  const xAOD::TrackParticle* track = mu->primaryTrackParticle();
  std::unique_ptr<const Trk::TrackParameters> extPars(extTrackToPivot(track));
  return extPars ? extPars->position() : Amg::Vector3D(0.,0.,0.);
}


template<>
std::tuple<bool, double,double> MuonMatchingTool :: trigPosForMatch<xAOD::L2StandAloneMuon>(const xAOD::L2StandAloneMuon *trig){
  return std::forward_as_tuple(true, trig->roiEta(), trig->roiPhi());
}


std::tuple<bool, double,double> MuonMatchingTool :: trigPosForMatchEFSA(const xAOD::Muon *trig){
  const xAOD::TrackParticle* mooreMuon = trig->trackParticle(xAOD::Muon::TrackParticleType::ExtrapolatedMuonSpectrometerTrackParticle);
  return mooreMuon ? std::forward_as_tuple(true, mooreMuon->eta(), mooreMuon->phi()) : std::forward_as_tuple(false, 0., 0.);
}


const xAOD::Muon* MuonMatchingTool :: matchEFSA(  const xAOD::Muon *mu, std::string trig, bool &pass) const {
  ATH_MSG_DEBUG("MuonMonitoring::matchEFSA()");
  float reqdR = 0.03;
  return match<xAOD::Muon>( mu, trig, reqdR, pass, &MuonMatchingTool::trigPosForMatchEFSA);
}


const xAOD::Muon* MuonMatchingTool :: matchEFCB(  const xAOD::Muon *mu, std::string trig, bool &pass) const {
  ATH_MSG_DEBUG("MuonMonitoring::matchEFCB()");
  float reqdR = 0.03;
  return match<xAOD::Muon>( mu, trig, reqdR, pass);
}


const xAOD::L2StandAloneMuon* MuonMatchingTool :: matchL2SA(  const xAOD::Muon *mu, std::string trig, bool &pass) const {
  ATH_MSG_DEBUG("MuonMonitoring::matchL2SA()");
  float reqdR = m_L2SAreqdR;
  if(m_use_extrapolator){
    reqdR = reqdRL1byPt(mu->pt());
    const Amg::Vector3D extPos = offlineMuonAtPivot(mu);
    if(extPos.norm()>ZERO_LIMIT){
      return match<xAOD::L2StandAloneMuon>( &extPos, trig, reqdR, pass);
    }
  }
  return match<xAOD::L2StandAloneMuon>( mu, trig, reqdR, pass);
}

const TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> MuonMatchingTool :: searchL2SALinkInfo(  const xAOD::Muon *mu, std::string trig) const {
  ATH_MSG_DEBUG("MuonMonitoring::searchL2SALinkInfo()");
  bool pass = false;
  return searchLinkInfo<xAOD::L2StandAloneMuon>( mu, trig, 1000., pass);
}


const xAOD::L2CombinedMuon* MuonMatchingTool :: matchL2CB(  const xAOD::Muon *mu, std::string trig, bool &pass) const {
  ATH_MSG_DEBUG("MuonMonitoring::matchL2CB()");
  return match<xAOD::L2CombinedMuon>( mu, trig, m_L2CBreqdR, pass);
}

const TrigCompositeUtils::LinkInfo<xAOD::L2CombinedMuonContainer> MuonMatchingTool :: searchL2CBLinkInfo(  const xAOD::Muon *mu, std::string trig) const {
  ATH_MSG_DEBUG("MuonMonitoring::searchL2CBLinkInfo()");
  bool pass = false;
  return searchLinkInfo<xAOD::L2CombinedMuon>( mu, trig,  1000., pass);
}


const xAOD::MuonRoI* MuonMatchingTool :: matchL1(  const xAOD::Muon *mu, const EventContext& ctx, std::string trig, bool &pass) const {

  double refEta = mu->eta();
  double refPhi = mu->phi();
  double reqdR = 0.25;
  if(m_use_extrapolator){
    reqdR = reqdRL1byPt(mu->pt());
    const Amg::Vector3D extPos = offlineMuonAtPivot(mu);
    if(extPos.norm()>ZERO_LIMIT){
      refEta = extPos.eta();
      refPhi = extPos.phi();
    }
  }

  pass = false;
  const xAOD::MuonRoI *closest = nullptr;
  SG::ReadHandle<xAOD::MuonRoIContainer> rois(m_MuonRoIContainerKey, ctx);
  if (! rois.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain xAOD::MuonRoI collection with name "<< m_MuonRoIContainerKey);
    return closest;
  }
  if(rois->getConstStore()==nullptr){
    xAOD::MuonRoIContainer *ncptr = const_cast<xAOD::MuonRoIContainer*>(rois.get());
    ncptr->setStore(DataLink<SG::IConstAuxStore>(m_MuonRoIContainerKey.key()+"Aux.", ctx));
  }

  for(const auto &roi : *rois){
    double roiEta = roi->eta();
    double roiPhi = roi->phi();
    int roiThr = roi->getThrNumber();
    
    double deta = refEta - roiEta;
    double dphi = xAOD::P4Helpers::deltaPhi(refPhi, roiPhi);
    double dR = sqrt(deta*deta + dphi*dphi);
    ATH_MSG_VERBOSE("L1 muon candidate eta=" << roiEta << " phi=" << roiPhi  << " dR=" << dR);
    if( dR<reqdR && roiThr>=L1ItemSTI(trig)){
      reqdR = dR;
      pass = true;
      closest = roi;
      ATH_MSG_DEBUG("* L1 muon eta=" << roiEta << " phi=" << roiPhi  << " dR=" << dR <<  " isPassed=true" ); 
    }
  }

  return closest;
}


const xAOD::Muon* MuonMatchingTool :: matchL2SAtoOff( const EventContext& ctx, float trigEta, float trigPhi) const {
  return matchOff(ctx, trigEta, trigPhi, m_L2SAreqdR);
}

const xAOD::Muon* MuonMatchingTool :: matchL2CBtoOff( const EventContext& ctx, float trigEta, float trigPhi) const {
  return matchOff(ctx, trigEta, trigPhi, m_L2CBreqdR);
}

const xAOD::Muon* MuonMatchingTool :: matchOff( const EventContext& ctx, float trigEta, float trigPhi, float DR_cut) const {

  const xAOD::Muon *muon = nullptr;

  SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey, ctx);
  if (! muons.isValid() ) {
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_MuonContainerKey);
    return muon;
  }

  for(const auto &mu : *muons){
    float offEta = mu->eta();
    float offPhi = mu->phi();

    float deta = offEta - trigEta;
    float dphi = xAOD::P4Helpers::deltaPhi(offPhi, trigPhi);
    double dR = sqrt(deta*deta + dphi*dphi);

    if(dR < DR_cut){
      DR_cut = dR;
      muon = mu;
      ATH_MSG_DEBUG("* Trigger muon eta=" << trigEta << " phi=" << trigPhi  << " offEta=" << offEta << " offPhi=" << offPhi << " dR=" << dR);
    }
  }  

  return muon;
}


bool MuonMatchingTool :: isMatchedL2SA(const xAOD::L2StandAloneMuon* samu, const xAOD::Muon* mu) const{
  float offlEta = mu->eta();
  float offlPhi = mu->phi();
  float trigEta = samu->roiEta();
  float trigPhi = samu->roiPhi();

  float deta = offlEta - trigEta;
  float dphi = xAOD::P4Helpers::deltaPhi(offlPhi, trigPhi);
  float dR = sqrt(deta*deta + dphi*dphi);
  return (dR < m_L2SAreqdR) ? true : false;
}

bool MuonMatchingTool :: isMatchedL2CB(const xAOD::L2CombinedMuon* cbmu, const xAOD::Muon* mu) const{
  float dR = xAOD::P4Helpers::deltaR(cbmu, mu, false);
  return (dR < m_L2CBreqdR) ? true : false;
}


double MuonMatchingTool :: FermiFunction(double x, double x0, double w) const {
  return 1/(1+TMath::Exp(-10*(x-x0)/w));
}


  
const Trk::TrackParameters* MuonMatchingTool :: extTrackToPivot(const xAOD::TrackParticle* track) const {

  const Trk::TrackParameters *extRPC = nullptr;
  const Trk::TrackParameters *extTGC = nullptr;

  if(!track) return extTGC;
  double trkEta = track->eta();
  double extEta =0.;
  bool isBarrel = true;

  if( fabs(trkEta)<1.05){
    extRPC = extTrackToRPC(track);
    if(!extRPC){
      isBarrel = false;
      extTGC = extTrackToTGC(track);
    }
    else{
      isBarrel = true;
      extEta = extRPC->position().eta();
      if(fabs(extEta)>=1.05){
	extTGC = extTrackToTGC(track);
	isBarrel = (extTGC)? false : true; 
      }
    }
  }
  else if( fabs(trkEta)>=1.05 ){
    extTGC = extTrackToTGC(track);
    if(!extTGC){
      isBarrel = true;
      extRPC = extTrackToRPC(track);
    }
    else{
      isBarrel = false;
      extEta = extTGC->position().eta();
      if(fabs(extEta)<1.05){
	extRPC = extTrackToRPC(track);
	isBarrel = (extRPC) ? true : false;
      }
    }
  }

  ATH_MSG_DEBUG("extTGC=" << extTGC << " extRPC=" << extRPC << " isBarrel=" << isBarrel);
  if(   isBarrel) delete extTGC;
  else if(!isBarrel) delete extRPC;
  return (isBarrel) ? extRPC : extTGC;
}

  

const Trk::TrackParameters* MuonMatchingTool :: extTrackToTGC( const xAOD::TrackParticle* trk ) const {
  ATH_MSG_DEBUG("extTrackToTGC");
  if(!trk) return nullptr;
  double TGC_Z = ( trk->eta()>0 )? 15153.0:-15153.0;
  std::unique_ptr<Amg::Transform3D> matrix(new Amg::Transform3D( Amg::Vector3D( 0.,0.,TGC_Z ) ));
  //object pointed by matrix will be deleted in destructer of DiscSurface, therefore release it
  std::unique_ptr<Trk::DiscSurface> disc(new Trk::DiscSurface( matrix.release(), 0., 15000.));
  const bool boundaryCheck = true;

  const Trk::TrackParameters* param = m_extrapolator->extrapolate(*trk,
                                                                  *disc,
                                                                  Trk::anyDirection,
                                                                  boundaryCheck,
                                                                  Trk::muon);

  ATH_MSG_DEBUG("param=" << param 
		<< " eta=" << ((param) ? param->position().eta() : 0) 
		<< " phi=" << ((param) ? param->position().phi() : 0));;
  return param;
}



const Trk::TrackParameters* MuonMatchingTool :: extTrackToRPC( const xAOD::TrackParticle* trk ) const {
  ATH_MSG_DEBUG("extTrackToRPC");
  if(!trk) return nullptr;
  std::unique_ptr<Trk::CylinderSurface> barrel(new Trk::CylinderSurface(  7478., 15000. ));
  const bool boundaryCheck = true;

  const Trk::TrackParameters* param = m_extrapolator->extrapolate(*trk,
                                                                  *barrel,
                                                                  Trk::anyDirection,
                                                                  boundaryCheck,
                                                                  Trk::muon);
  return param;
}



double MuonMatchingTool :: reqdRL1byPt( double mupt){
  double dR = 0.08;
  if( mupt < 10000. ) {
    dR = -0.00001*mupt + 0.18;
  } 
  return dR;
}
