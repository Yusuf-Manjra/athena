/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecoValidationTool.h"
#include "TFile.h"
#include "TTree.h"

#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagMatchingTool.h"
#include "MuonRecToolInterfaces/IMuonTruthSummaryTool.h"
#include "MuonClusterization/TgcHitClustering.h"

#include "MuonSegment/MuonSegment.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "TrkPrepRawData/PrepRawData.h"

namespace Muon {

  MuonRecoValidationTool::MuonRecoValidationTool(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
      m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
      m_edmHelper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
      m_hitSummaryTool("Muon::MuonSegmentHitSummaryTool/MuonSegmentHitSummaryTool"),
      m_truthSummaryTool("Muon::MuonTruthSummaryTool/MuonTruthSummaryTool"),
      m_extrapolator("Trk::Extrapolation/AtlasExtrapolator"),
      m_matchingTool("MuTagMatchingTool/MuTagMatchingTool"),
      m_incidentSvc("IncidentSvc",n)
  {
    declareInterface<IMuonRecoValidationTool>(this);
    declareProperty("Extrapolator",m_extrapolator );    
    declareProperty("MatchTool",m_matchingTool );    
  }
    
  MuonRecoValidationTool::~MuonRecoValidationTool() {
    
  }
    
  StatusCode MuonRecoValidationTool::initialize() {

    ATH_CHECK(m_idHelper.retrieve());
    ATH_CHECK(m_edmHelper.retrieve());
    ATH_CHECK(m_hitSummaryTool.retrieve());
    ATH_CHECK(m_truthSummaryTool.retrieve());
    ATH_CHECK(m_extrapolator.retrieve());
    ATH_CHECK(m_matchingTool.retrieve());

    ITHistSvc* thistSvc = nullptr;
    ATH_CHECK( service("THistSvc", thistSvc) );

    m_tree = new TTree("data", "RecoValidation");
    ATH_CHECK( thistSvc->regTree("/MuonRecoVal/data", m_tree) );

    m_ntuple.init("",m_tree);

    // call handle in case of EndEvent
    ATH_CHECK(m_incidentSvc.retrieve());
    m_incidentSvc->addListener( this, IncidentType::EndEvent );

    return StatusCode::SUCCESS;
  }

  void MuonRecoValidationTool::clear() {
    m_ntuple.clear();
    m_trackParticles.clear();
    m_trackParticleIndexLookup.clear();
  }

  void MuonRecoValidationTool::handle(const Incident& inc) {
    // Only clear cache for EndEvent incident
    if (inc.type()  == IncidentType::EndEvent){
      ATH_MSG_DEBUG(" clearing cache at end of event " );
      m_tree->Fill();
      clear();
    }  
  }

  StatusCode MuonRecoValidationTool::finalize() {

    return StatusCode::SUCCESS;
  }

  bool MuonRecoValidationTool::addTrackParticle(  const xAOD::TrackParticle& indetTrackParticle,
                                                  const MuonSystemExtension& muonSystemExtension ) {
    
    m_ntuple.trackParticleBlock.pt->push_back(indetTrackParticle.pt());
    m_ntuple.trackParticleBlock.p->push_back(1./indetTrackParticle.qOverP());
    m_ntuple.trackParticleBlock.eta->push_back(indetTrackParticle.eta());
    m_ntuple.trackParticleBlock.phi->push_back(indetTrackParticle.phi());
    int pdg = 0;
    int barcode = -1;
    // set truth
    typedef ElementLink<xAOD::TruthParticleContainer> ElementTruthLink_t;
    if( indetTrackParticle.isAvailable<ElementTruthLink_t>("truthParticleLink") ) {
      const ElementTruthLink_t link = indetTrackParticle.auxdata<ElementTruthLink_t>("truthParticleLink");
      if( link.isValid() ){
        pdg = (*link)->pdgId();
        barcode = (*link)->barcode();
      }
    }
    m_ntuple.trackParticleBlock.truth.fill(pdg,barcode);

    // try to find the pointer of the indetTrackParticle
    bool found = false;
    for(unsigned int index = 0 ;index < m_trackParticles.size(); ++index ){
      if( &indetTrackParticle == m_trackParticles[index] ){
        found = true;
        break;
      }
    }
    if( !found ){
      // set index to the last element
      unsigned int index = m_trackParticles.size();
      m_trackParticles.push_back(&indetTrackParticle);

      const std::vector<Muon::MuonSystemExtension::Intersection>& layerIntersections = muonSystemExtension.layerIntersections();
      ATH_MSG_DEBUG("Adding ID track: pt "  << indetTrackParticle.pt() << " eta " << indetTrackParticle.eta() 
                    << " phi " << indetTrackParticle.phi() << " layers " << layerIntersections.size());

      for( std::vector<Muon::MuonSystemExtension::Intersection>::const_iterator it = layerIntersections.begin();it!=layerIntersections.end();++it ){
        m_trackParticleIndexLookup[it->trackParameters.get()]=index;
      }
    }
    return true;
  }
  
  int MuonRecoValidationTool::getBarcode( const std::set<Identifier>& ids ) const {

    // count how often a barcode occurs 
    std::map<int,int> counters;
    for( std::set<Identifier>::const_iterator it=ids.begin();it!=ids.end();++it ){
      int bc = m_truthSummaryTool->getBarcode(*it);
      if( bc != -1 ) ++counters[bc];
    }
    
    // pick the most frequent
    int barcode = -1;
    int max = -1;
    for( std::map<int,int>::iterator it=counters.begin();it!=counters.end();++it ){
      if( it->second > max ){
        barcode = it->first;
        max = it->second;
      }
    }
    
    return barcode;
  }

  bool MuonRecoValidationTool::add( const MuonSystemExtension::Intersection& intersection, const MuonSegment& segment, int stage ) {


    m_ntuple.segmentBlock.stage->push_back(stage);

    Identifier id = m_edmHelper->chamberId(segment);
    m_ntuple.segmentBlock.id.fill(m_idHelper->sector(id),m_idHelper->chamberIndex(id));

    // hit counts
    IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_hitSummaryTool->getHitCounts(segment);

    m_ntuple.segmentBlock.nmdtHits->push_back(hitCounts.nmdtHitsMl1+hitCounts.nmdtHitsMl2+hitCounts.ncscHitsEta);
    m_ntuple.segmentBlock.ntrigEtaHits->push_back(hitCounts.netaTrigHitLayers);
    m_ntuple.segmentBlock.ntrigPhiHits->push_back(hitCounts.nphiTrigHitLayers);

    // get truth from hits
    std::set<Identifier> ids;
    std::vector<const Trk::MeasurementBase*>::const_iterator mit = segment.containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = segment.containedMeasurements().end();
    for( ;mit!=mit_end;++mit ){

      // get Identifier
      Identifier id = m_edmHelper->getIdentifier(**mit);
      if( !id.is_valid() ) continue;
      ids.insert(id);
    }
    int barcode = getBarcode(ids);
    int pdg = barcode != -1 ? 13 : 0;
    m_ntuple.segmentBlock.truth.fill(pdg,barcode);

    m_ntuple.segmentBlock.track.fill(getIndex(intersection));

    // extrapolate and create an intersection @ the segment surface. 
    std::shared_ptr<const Trk::TrackParameters> exPars(m_extrapolator->extrapolate(*intersection.trackParameters,segment.associatedSurface(),Trk::anyDirection,false,Trk::muon));
    if( !exPars ){
      ATH_MSG_VERBOSE(" extrapolation failed ");
      m_ntuple.segmentBlock.quality->push_back(-2);
      m_ntuple.segmentBlock.xresiduals.fill(0.,1.,0.,1.,-1);
      m_ntuple.segmentBlock.yresiduals.fill(0.,1.,0.,1.,-1);
      m_ntuple.segmentBlock.angleXZ.fill(0.,1.,0.,1.,-1);
      m_ntuple.segmentBlock.angleYZ.fill(0.,1.,0.,1.,-1);
      m_ntuple.segmentBlock.combinedYZ.fill(0.,1.,0.,1.,-1);
      return false;
    }
    
    
    // cast to AtaPlane so we can get the segment info 
    const Trk::AtaPlane* ataPlane = dynamic_cast<const Trk::AtaPlane*>(exPars.get());
    if(!ataPlane){
      ATH_MSG_WARNING(" dynamic_cast<> failed ");
      m_ntuple.segmentBlock.quality->push_back(-2);
      m_ntuple.segmentBlock.angleXZ.fill(0.,1.,0.,1.,-1);
      m_ntuple.segmentBlock.angleYZ.fill(0.,1.,0.,1.,-1);
      m_ntuple.segmentBlock.combinedYZ.fill(0.,1.,0.,1.,-1);
      return false;
    }
    MuonCombined::MuonSegmentInfo segmentInfo = m_matchingTool->muTagSegmentInfo( nullptr, &segment, ataPlane );
    m_ntuple.segmentBlock.quality->push_back(segmentInfo.quality);
    m_ntuple.segmentBlock.xresiduals.fillResPull(segmentInfo.resX,segmentInfo.pullX,segmentInfo.segErrorX,segmentInfo.exErrorX,1);
    m_ntuple.segmentBlock.yresiduals.fillResPull(segmentInfo.resY,segmentInfo.pullY,segmentInfo.segErrorY,segmentInfo.exErrorY,1);
    m_ntuple.segmentBlock.angleXZ.fillResPull(segmentInfo.dangleXZ,segmentInfo.pullXZ,segmentInfo.segErrorXZ,segmentInfo.exErrorXZ,1);
    m_ntuple.segmentBlock.angleYZ.fillResPull(segmentInfo.dangleYZ,segmentInfo.pullYZ,segmentInfo.segErrorYZ,segmentInfo.exErrorYZ,1);
    m_ntuple.segmentBlock.combinedYZ.fillResPull(segmentInfo.resCY,segmentInfo.pullCY,1);
    
    return true;
  }

  bool MuonRecoValidationTool::add( const MuonSystemExtension::Intersection& intersection,
                                    const MuonHough::MuonLayerHough::Maximum& maximum ) {

    m_ntuple.houghBlock.maximum->push_back(maximum.max);
    
    m_ntuple.houghBlock.track.fill(getIndex(intersection));

    int sector = -1;
    int chIndex = -1;
    float maxwidth = (maximum.binposmax-maximum.binposmin);
    if( maximum.hough ) {
      maxwidth *= maximum.hough->m_binsize;
      sector   = maximum.hough->m_descriptor.sector;
      chIndex  = maximum.hough->m_descriptor.chIndex;
    }
    m_ntuple.houghBlock.id.fill(sector,chIndex);
    m_ntuple.houghBlock.residuals.fill(maximum.pos,maxwidth,intersection,Trk::loc1);

    // get truth from hits
    std::set<Identifier> ids;

    std::vector<MuonHough::Hit*>::const_iterator hit = maximum.hits.begin();
    std::vector<MuonHough::Hit*>::const_iterator hit_end = maximum.hits.end();
    for( ;hit!=hit_end;++hit ) {
      // treat the case that the hit is a composite TGC hit
      if( (*hit)->tgc ){
        for( const auto& prd : (*hit)->tgc->etaCluster.hitList ) ids.insert(prd->identify());
      }else if( (*hit)->prd ){
        ids.insert((*hit)->prd->identify());
      }
    }
    int barcode = getBarcode(ids);
    int pdg = barcode != -1 ? 13 : 0;
    m_ntuple.houghBlock.truth.fill(pdg,barcode);


    return true;
  }

  bool MuonRecoValidationTool::add( const MuonSystemExtension::Intersection& intersection, const Trk::PrepRawData& prd,
                                    float expos, float expos_err )  {

    Identifier id = prd.identify();
    m_ntuple.hitBlock.id.fill(m_idHelper->sector(id),m_idHelper->chamberIndex(id));
    m_ntuple.hitBlock.track.fill(getIndex(intersection));
    
    int barcode = m_truthSummaryTool->getBarcode(id);
    int pdg = barcode != -1 ? 13 : 0;
    m_ntuple.hitBlock.truth.fill(pdg,barcode);

    float sign = expos < 0 ? -1. : 1.;
    m_ntuple.hitBlock.residuals.fill(sign*prd.localPosition()[Trk::locX],Amg::error(prd.localCovariance(),Trk::locX),expos,expos_err);

    return true;
  }


} //end of namespace
