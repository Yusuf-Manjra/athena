/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonInsideOutRecoTool.h"

#include "MuonLayerEvent/MuonSystemExtension.h"
#include "MuonLayerEvent/MuonCandidate.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonLayerEvent/MuonLayerPrepRawData.h"
#include "MuonIdHelpers/MuonStationIndexHelpers.h"

#include "MuonCombinedEvent/MuGirlTag.h"
#include "xAODTracking/Vertex.h"

namespace MuonCombined {

  MuonInsideOutRecoTool::MuonInsideOutRecoTool(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_segmentFinder("Muon::MuonLayerSegmentFinderTool/MuonLayerSegmentFinderTool"),
    m_segmentMatchingTool("Muon::MuonLayerSegmentMatchingTool/MuonLayerSegmentMatchingTool"),
    m_ambiguityResolver("Muon::MuonLayerAmbiguitySolverTool/MuonLayerAmbiguitySolverTool"),
    m_candidateTrackBuilder("Muon::MuonCandidateTrackBuilderTool/MuonCandidateTrackBuilderTool"),
    m_recoValidationTool(""),
    m_trackFitter("Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder"),
    m_trackAmbiguityResolver("Trk::TrackSelectionProcessorTool/MuonAmbiProcessor"),
    m_layerHashProvider("Muon::MuonLayerHashProviderTool"),
    m_trackSummaryTool("MuonTrackSummaryTool")
  {
    declareInterface<IMuonCombinedInDetExtensionTool>(this);
    declareInterface<MuonInsideOutRecoTool>(this);

    declareProperty("MuonEDMPrinterTool",m_printer );    
    declareProperty("MuonLayerSegmentFinderTool",m_segmentFinder );    
    declareProperty("MuonLayerSegmentMatchingTool",m_segmentMatchingTool );    
    declareProperty("MuonLayerAmbiguitySolverTool",m_ambiguityResolver );    
    declareProperty("MuonCandidateTrackBuilderTool",m_candidateTrackBuilder );    
    declareProperty("MuonRecoValidationTool",m_recoValidationTool );    
    declareProperty("MuonTrackBuilder",           m_trackFitter );    
    declareProperty("TrackAmbiguityProcessor",m_trackAmbiguityResolver );    
    declareProperty("IDTrackMinPt", m_idTrackMinPt = 2500.0 );
    declareProperty("IgnoreSiAssociatedCandidates", m_ignoreSiAssocated = true );
    declareProperty("TrackSummaryTool", m_trackSummaryTool );
  }

  StatusCode MuonInsideOutRecoTool::initialize() {

    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_segmentFinder.retrieve());
    ATH_CHECK(m_segmentMatchingTool.retrieve());
    ATH_CHECK(m_ambiguityResolver.retrieve());
    ATH_CHECK(m_candidateTrackBuilder.retrieve());
    if( !m_recoValidationTool.empty() ) ATH_CHECK(m_recoValidationTool.retrieve());
    ATH_CHECK(m_trackFitter.retrieve());
    ATH_CHECK(m_trackAmbiguityResolver.retrieve());
    //trigger does not use primary vertex
    if(!m_vertexKey.empty()) ATH_CHECK(m_vertexKey.initialize());
    ATH_CHECK(m_trackSummaryTool.retrieve());
    return StatusCode::SUCCESS;
  }

  void MuonInsideOutRecoTool::extend( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, TrackCollection* combTracks, TrackCollection* meTracks,
				      Trk::SegmentCollection* segments) {
    ATH_MSG_WARNING("This version is deprecated, please use extendWithPRDs for MuGirl");
    IMuonCombinedInDetExtensionTool::MuonPrdData prdData;
    extendWithPRDs(inDetCandidates,tagMap,prdData,combTracks,meTracks,segments);
  }

  void MuonInsideOutRecoTool::extendWithPRDs( const InDetCandidateCollection& inDetCandidates, InDetCandidateToTagMap* tagMap, IMuonCombinedInDetExtensionTool::MuonPrdData prdData,
					      TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segments) {
    ATH_MSG_DEBUG(" extending " << inDetCandidates.size() );

    InDetCandidateCollection::const_iterator it = inDetCandidates.begin();
    InDetCandidateCollection::const_iterator it_end = inDetCandidates.end();
    for( ; it!=it_end;++it ){
      handleCandidate( **it,tagMap,prdData,combTracks,meTracks,segments );
    }
  }

  void MuonInsideOutRecoTool::handleCandidate( const InDetCandidate& indetCandidate, InDetCandidateToTagMap* tagMap, IMuonCombinedInDetExtensionTool::MuonPrdData prdData,
					       TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segColl) {
    
    if( m_ignoreSiAssocated && indetCandidate.isSiliconAssociated() ) {
      ATH_MSG_DEBUG(" skip silicon associated track for extension ");
      return;
    }

    const xAOD::TrackParticle& indetTrackParticle = indetCandidate.indetTrackParticle();
    if( !indetTrackParticle.track() || indetTrackParticle.pt() < m_idTrackMinPt ) return;
    
    
    // get intersections which precision layers in the muon system 
    const Muon::MuonSystemExtension* muonSystemExtension = indetCandidate.getExtension();
    if( !muonSystemExtension ) {
      //ATH_MSG_DEBUG("No MuonSystemExtension, aborting ");
      return;
    }
    
    // fill validation content
    if( !m_recoValidationTool.empty() ) m_recoValidationTool->addTrackParticle( indetTrackParticle, *muonSystemExtension );
    
    // loop over intersections, get segments
    std::vector< Muon::MuonLayerRecoData > allLayers;

    const std::vector<Muon::MuonSystemExtension::Intersection>& layerIntersections = muonSystemExtension->layerIntersections();
    ATH_MSG_DEBUG(" ID track: pt "  << indetTrackParticle.pt() << " eta " << indetTrackParticle.eta() 
                  << " phi " << indetTrackParticle.phi() << " layers " << layerIntersections.size());

    for( std::vector<Muon::MuonSystemExtension::Intersection>::const_iterator it = layerIntersections.begin();it!=layerIntersections.end();++it ){

      // vector to store segments
      std::vector< std::shared_ptr<const Muon::MuonSegment> > segments;
      
      // find segments for intersection
      Muon::MuonLayerPrepRawData layerPrepRawData;
      if(!getLayerData( (*it).layerSurface.sector, (*it).layerSurface.regionIndex, (*it).layerSurface.layerIndex,layerPrepRawData,prdData)){
	ATH_MSG_WARNING("Failed to get layer data");
	continue;
      }
      m_segmentFinder->find( *it, segments, layerPrepRawData );
      if( segments.empty() ) continue;

      // fill validation content
      if( !m_recoValidationTool.empty() ) {
        for( const auto& seg : segments ) m_recoValidationTool->add(*it,*seg,0);
      }

      // match segments to intersection
      std::vector< std::shared_ptr<const Muon::MuonSegment> > selectedSegments;
      m_segmentMatchingTool->select( *it, segments, selectedSegments );
      if( selectedSegments.empty() ) continue;

      // fill validation content
      if( !m_recoValidationTool.empty() ) {
        for( const auto& seg : segments ) m_recoValidationTool->add(*it,*seg,1);
      }
      
      // add to total list
      allLayers.push_back( Muon::MuonLayerRecoData(*it,std::move(selectedSegments)) );
    }
    
    if( msgLvl(MSG::DEBUG) ){
      msg(MSG::DEBUG) << " Layers with segments " << allLayers.size();
      for( auto& layer : allLayers ) {
        for( auto& seg : layer.segments ){
          msg(MSG::DEBUG) << std::endl << m_printer->print(*seg);
        }
      }
      msg(MSG::DEBUG) << endmsg;
    }

    // find best candidate and exit if none found
    std::pair<std::unique_ptr<const Muon::MuonCandidate>,Trk::Track*> bestCandidate = findBestCandidate(indetTrackParticle,allLayers);
    if( !bestCandidate.first.get() ){
      return;
    }

    // add candidate to indet candidate
    addTag(indetCandidate,tagMap,*bestCandidate.first.get(),bestCandidate.second,combTracks,meTracks,segColl);
  }


  std::pair<std::unique_ptr<const Muon::MuonCandidate>,Trk::Track*>
  MuonInsideOutRecoTool::findBestCandidate( const xAOD::TrackParticle& indetTrackParticle, const std::vector< Muon::MuonLayerRecoData >& allLayers) {

    // resolve ambiguities 
    std::vector< Muon::MuonCandidate > resolvedCandidates;
    m_ambiguityResolver->resolveOverlaps( allLayers, resolvedCandidates );

    // fit candidates
    TrackCollection tracks;
    std::map<const Trk::Track*,const Muon::MuonCandidate*> trackCandidateLookup;
    for( const auto& candidate : resolvedCandidates ){
      Trk::Track* track = m_candidateTrackBuilder->buildCombinedTrack(*indetTrackParticle.track(),candidate);
      if( track ){
        tracks.push_back(track);
        trackCandidateLookup[track] = &candidate;
      }
    }

    ATH_MSG_DEBUG("found "<<tracks.size()<<" combined tracks");

    // first handle easy cases of zero or one track
    if( tracks.empty() ) return std::pair<std::unique_ptr<const Muon::MuonCandidate>,Trk::Track*>(nullptr,nullptr);
    
    Trk::Track* selectedTrack = 0;
    if( tracks.size() == 1 ){
      selectedTrack = tracks.front();
    }else{
    
      // more than 1 track call ambiguity solver and select first track
      TrackCollection* resolvedTracks = m_trackAmbiguityResolver->process(&tracks);
      if (!resolvedTracks || resolvedTracks->empty() ) {
         ATH_MSG_WARNING("Ambiguity resolver returned no tracks. Arbitrarily using the first track of initial collection.");
         selectedTrack = tracks.front();
      } else {
        selectedTrack = resolvedTracks->front();
        delete resolvedTracks;
      }
    }
    // get candidate
    const Muon::MuonCandidate* candidate = trackCandidateLookup[selectedTrack];
    if( !candidate ){
      ATH_MSG_WARNING("candidate lookup failed, this should not happen");
      return std::pair<std::unique_ptr<const Muon::MuonCandidate>,Trk::Track* >(nullptr,nullptr);
    }
    // generate a track summary for this candidate
    if (m_trackSummaryTool.isEnabled()) {
      m_trackSummaryTool->computeAndReplaceTrackSummary(*selectedTrack, nullptr, false);
    }

    return std::make_pair( std::unique_ptr<const Muon::MuonCandidate>(new Muon::MuonCandidate(*candidate)),
                           new Trk::Track(*selectedTrack) );
  }

  void MuonInsideOutRecoTool::addTag( const InDetCandidate& indetCandidate, InDetCandidateToTagMap* tagMap, 
				      const Muon::MuonCandidate& candidate, Trk::Track* selectedTrack,
				      TrackCollection* combTracks, TrackCollection* meTracks, Trk::SegmentCollection* segments) const {

    const xAOD::TrackParticle& idTrackParticle = indetCandidate.indetTrackParticle();
    float bs_x = 0.;
    float bs_y = 0.;
    float bs_z = 0.;
 
    const xAOD::Vertex* matchedVertex = nullptr;
    if(!m_vertexKey.empty()){
      SG::ReadHandle<xAOD::VertexContainer> vertices { m_vertexKey };
      if (!vertices.isValid())
	{
	  ATH_MSG_WARNING("No vertex container with key = " << m_vertexKey.key() << " found");
	}
      else
	{
	  for ( const auto& vx : *vertices )
	    {
	      for ( const auto& tpLink : vx->trackParticleLinks() )
		{
		  if (*tpLink == &idTrackParticle)
		    {
		      matchedVertex = vx;
		      break;
		    }
		  if (matchedVertex) break;
		}
	    }
	}
    }
    if(matchedVertex) {
      bs_x = matchedVertex->x();
      bs_y = matchedVertex->y();
      bs_z = matchedVertex->z();
      ATH_MSG_DEBUG( " found matched vertex  bs_x " << bs_x << " bs_y " << bs_y << " bs_z " << bs_z);
    } else {
    // take for beamspot point of closest approach of ID track in  x y z 
      bs_x = -idTrackParticle.d0()*sin(idTrackParticle.phi()) + idTrackParticle.vx();
      bs_y =  idTrackParticle.d0()*cos(idTrackParticle.phi()) + idTrackParticle.vy();
      bs_z = idTrackParticle.z0() + idTrackParticle.vz();
      ATH_MSG_DEBUG( " NO matched vertex  take track perigee  x " << bs_x << " y " << bs_y << " z " << bs_z);
    }

    ATH_MSG_DEBUG("selectedTrack:");
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it = selectedTrack->trackStateOnSurfaces()->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end = selectedTrack->trackStateOnSurfaces()->end();
    int tsos = 0;
    for (; it!=it_end; ++it) {
      tsos++;
      if((*it)->trackParameters()) {
        ATH_MSG_VERBOSE("check tsos "<<tsos<< " r "<<(*it)->trackParameters()->position().perp()<<" z "<<(*it)->trackParameters()->position().z()<<" p "<<(*it)->trackParameters()->momentum().mag());
      } 
    }

    // get segments 
    std::vector<ElementLink<Trk::SegmentCollection> > segLinks;
    for( const auto& layer : candidate.layerIntersections ){
      segments->push_back( new Muon::MuonSegment(*layer.segment) );
      ElementLink<Trk::SegmentCollection> sLink(*segments,segments->size()-1);
      segLinks.push_back(sLink);
    }

    // perform standalone refit
    Trk::Track* standaloneRefit = m_trackFitter->standaloneRefit(*selectedTrack, bs_x, bs_y, bs_z);

    combTracks->push_back(selectedTrack);
    ElementLink<TrackCollection> comblink( *combTracks,combTracks->size()-1);
        
    // create tag and set SA refit
    MuGirlTag* tag = new MuGirlTag(comblink,segLinks);    
    if( standaloneRefit ){
      meTracks->push_back(standaloneRefit);
      ElementLink<TrackCollection> melink( *meTracks,meTracks->size()-1);
      tag->setUpdatedExtrapolatedTrack(melink);
    }

    // add tag to IndetCandidateToTagMap
    tagMap->addEntry(&indetCandidate,tag);
  }

  bool MuonInsideOutRecoTool::getLayerData( int sector, Muon::MuonStationIndex::DetectorRegionIndex regionIndex,
					    Muon::MuonStationIndex::LayerIndex layerIndex, Muon::MuonLayerPrepRawData& layerPrepRawData, IMuonCombinedInDetExtensionTool::MuonPrdData prdData){

    // get technologies in the given layer
    Muon::MuonStationIndex::StIndex stIndex = Muon::MuonStationIndex::toStationIndex( regionIndex, layerIndex );
    std::vector<Muon::MuonStationIndex::TechnologyIndex> technologiesInStation = Muon::MuonStationIndexHelpers::technologiesInStation( stIndex );
    std::string techString;
    for( auto tech : technologiesInStation ) techString += " " + Muon::MuonStationIndex::technologyName(tech);
    ATH_MSG_DEBUG("getLayerData: sector " << sector << " " << Muon::MuonStationIndex::regionName(regionIndex) << " " << Muon::MuonStationIndex::layerName(layerIndex)
                  << " technologies " << techString);
  
    bool isok = true;
    // loop over technologies and get data
    for( std::vector<Muon::MuonStationIndex::TechnologyIndex>::iterator it=technologiesInStation.begin();it!=technologiesInStation.end();++it ){

      // get collections, keep track of failures
      if(*it == Muon::MuonStationIndex::MDT)       isok = !getLayerDataTech<Muon::MdtPrepDataCollection>(sector,*it,regionIndex,layerIndex,prdData.mdtPrds,layerPrepRawData.mdts)    ? false : isok;
      else if(*it == Muon::MuonStationIndex::RPC)  isok = !getLayerDataTech<Muon::RpcPrepDataCollection>(sector,*it,regionIndex,layerIndex,prdData.rpcPrds,layerPrepRawData.rpcs)    ? false : isok;
      else if(*it == Muon::MuonStationIndex::TGC)  isok = !getLayerDataTech<Muon::TgcPrepDataCollection>(sector,*it,regionIndex,layerIndex,prdData.tgcPrds,layerPrepRawData.tgcs)    ? false : isok;
      else if(*it == Muon::MuonStationIndex::CSCI) isok = !getLayerDataTech<Muon::CscPrepDataCollection>(sector,*it,regionIndex,layerIndex,prdData.cscPrds,layerPrepRawData.cscs)    ? false : isok;
      else if(*it == Muon::MuonStationIndex::STGC) isok = !getLayerDataTech<Muon::sTgcPrepDataCollection>(sector,*it,regionIndex,layerIndex,prdData.stgcPrds,layerPrepRawData.stgcs) ? false : isok;
      else if(*it == Muon::MuonStationIndex::MM)   isok = !getLayerDataTech<Muon::MMPrepDataCollection>(sector,*it,regionIndex,layerIndex,prdData.mmPrds,layerPrepRawData.mms)       ? false : isok;
    }

    if( msgLvl(MSG::DEBUG) ){
      msg(MSG::DEBUG) << " Got data: sector " << sector << " " << Muon::MuonStationIndex::regionName(regionIndex)
                      << " " << Muon::MuonStationIndex::layerName(layerIndex);
      if( !layerPrepRawData.mdts.empty() )  msg(MSG::DEBUG) << " MDTs " << layerPrepRawData.mdts.size();
      if( !layerPrepRawData.rpcs.empty() )  msg(MSG::DEBUG) << " RPCs "  << layerPrepRawData.rpcs.size();
      if( !layerPrepRawData.tgcs.empty() )  msg(MSG::DEBUG) << " TGCs "  << layerPrepRawData.tgcs.size();
      if( !layerPrepRawData.cscs.empty() )  msg(MSG::DEBUG) << " CSCs "  << layerPrepRawData.cscs.size();
      if( !layerPrepRawData.stgcs.empty() ) msg(MSG::DEBUG) << " STGCs "  << layerPrepRawData.stgcs.size();
      if( !layerPrepRawData.mms.empty() )   msg(MSG::DEBUG) << " MMs "  << layerPrepRawData.mms.size();
      msg(MSG::DEBUG) << endmsg;
    }
    return isok;
 }

  template<class COL>
  bool MuonInsideOutRecoTool::getLayerDataTech( int sector, Muon::MuonStationIndex::TechnologyIndex technology, Muon::MuonStationIndex::DetectorRegionIndex regionIndex,
						Muon::MuonStationIndex::LayerIndex layerIndex, const Muon::MuonPrepDataContainer< COL >* input, std::vector<const COL*>& output ) {

    if(!input || input->size()==0) return true;
    typedef Muon::MuonPrepDataContainer< COL > ContainerType;
    // get technologies in the given layer
    unsigned int sectorLayerHash = Muon::MuonStationIndex::sectorLayerHash( regionIndex, layerIndex );

    // get hashes
    const Muon::MuonLayerHashProviderTool::HashVec& hashes = m_layerHashProvider->getHashes( sector, technology, sectorLayerHash );

    // skip empty inputs
    if( hashes.empty() ) return true;

    // loop over hashes
    for( Muon::MuonLayerHashProviderTool::HashVec::const_iterator it=hashes.begin();it!=hashes.end();++it ){

      // skip if not found
      typename ContainerType::const_iterator colIt;
      colIt=input->indexFind(*it);
      if( colIt == input->end() ) {
	continue;
      }
      ATH_MSG_VERBOSE("  adding " << m_idHelperSvc->toStringChamber((*colIt)->identify()) << " size " << (*colIt)->size());
      // else add
      output.push_back(*colIt);
    }
    return true;
  }
}
 
