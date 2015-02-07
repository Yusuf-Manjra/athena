/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// 
// Implementation for SiTrigSPSeededTrackFinder
// (see class definition)
//
//****************************************************************************
#include "GaudiKernel/MsgStream.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "SiTrigSPSeededTrackFinder/SiTrigSPSeededTrackFinder.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

//Trigger stuff
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "IRegionSelector/IRoiDescriptor.h"

//tools
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "InDetRecToolInterfaces/ISiZvertexMaker.h" 
#include "InDetRecToolInterfaces/ISiTrackMaker.h" 

//
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

//Eigen
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiTrigSPSeededTrackFinder::SiTrigSPSeededTrackFinder
(const std::string& name,ISvcLocator* pSvcLocator): 
  HLT::FexAlgo(name, pSvcLocator),
  m_useZvertexTool(true),
  m_useSeedMaker(true),
  m_outputlevel(0),
  m_nprint(0),
  m_nseeds(0),
  m_ntracks(0),
  m_nseedsTotal(0),
  m_ntracksTotal(0),
  m_nInvoked(0),
  m_nfreeCut(5),
  m_seedsmaker("InDet::SiSpacePointsSeedMaker_ATLxk/InDetTrigSiSpacePointsSeedMaker"),
  m_zvertexmaker("InDet::SiZvertexMaker_xk/InDetTrigZvertexMaker"),
  m_trackmaker("InDet::SiTrackMaker_xk/InDetTrigSiTrackMaker"),
  m_regionSelector("RegSelSvc", name),
  m_etaHalfWidth(0.1),
  m_phiHalfWidth(0.1),
  m_doFullScan(false),
  m_fastTracking(false)
{
  // SiTrigSPSeededTrackFinder steering parameters
  declareProperty("SeedsTool"     ,m_seedsmaker    );
  declareProperty("ZvertexTool"   ,m_zvertexmaker  );
  declareProperty("TrackTool"     ,m_trackmaker    );
  
  declareProperty("useZvertexTool",m_useZvertexTool);
  declareProperty("useSeedMaker",m_useSeedMaker);
  declareProperty("RegionSelectorTool",m_regionSelector);
  declareProperty("EtaHalfWidth",  m_etaHalfWidth);
  declareProperty("PhiHalfWidth",  m_phiHalfWidth);
  declareProperty("doFullScan",    m_doFullScan);
  declareProperty("FastTracking",  m_fastTracking);
  declareProperty("FreeClustersCut"   ,m_nfreeCut      );
  
  //declare properties to be monitored
  declareMonitoredVariable("numSeeds",m_nseeds);
  declareMonitoredVariable("numtracks",m_ntracks);
  declareMonitoredVariable("numZvtx",m_nZvtx);
  declareMonitoredVariable("numDetElPixel",m_nDetElPixel);
  declareMonitoredVariable("numDetElSCT",m_nDetElSCT);
  declareMonitoredVariable("RoIEta",m_RoIEta);
  declareMonitoredVariable("RoIPhi",m_RoIPhi);
  declareMonitoredVariable("nGoodSeedsTotal",m_nGoodSeedsTotal);
  declareMonitoredVariable("nBadSeedsTotal",m_nBadSeedsTotal);
  declareMonitoredVariable("timeGoodSeeds",m_timeGoodSeeds);
  declareMonitoredVariable("timeBadSeeds",m_timeBadSeeds);
  declareMonitoredVariable("AvtimeGoodSeeds",m_AvtimeGoodSeeds);
  declareMonitoredVariable("AvtimeBadSeeds",m_AvtimeBadSeeds);
 
  m_timerRegSel = 0;
  m_timerZVertexTool = 0;
  m_timerSeedsMaker = 0;
  m_timerTrackMaker = 0;
  m_timerSeedProcessing = 0;

  m_AvtimeGoodSeeds = 0.;
  m_AvtimeBadSeeds = 0.;

  m_nseedsTotal  = 0;
  m_nGoodSeedsTotal =0;
  m_nBadSeedsTotal =0;
  m_ntracksTotal = 0;

}

//----------------------------------
//          beginRun method:
//----------------------------------------------------------------------------
HLT::ErrorCode InDet::SiTrigSPSeededTrackFinder::hltBeginRun() {
  
  msg() << MSG::INFO << "SiTrigSPSeededTrackFinder::beginRun()" << endreq;
  return HLT::OK;
}
//----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

HLT::ErrorCode InDet::SiTrigSPSeededTrackFinder::hltInitialize() {
  // Get tool for space points seed maker
  if ( m_seedsmaker.retrieve().isFailure() ) {
    msg() << MSG::FATAL << "Failed to retrieve tool " << m_seedsmaker << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } 
  else {
    msg() << MSG::INFO << "Retrieved tool " << m_seedsmaker << endreq;
  }
  
  if(m_useZvertexTool) {
    
    // Get tool for z-coordinates primary vertices search
    if ( m_zvertexmaker.retrieve().isFailure() ) {
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_zvertexmaker << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } 
    else {
      msg() << MSG::INFO << "Retrieved tool " << m_zvertexmaker << endreq; 
    }
  }

  // Get track-finding tool
  if ( m_trackmaker.retrieve().isFailure() ) {
    msg() << MSG::FATAL << "Failed to retrieve tool " << m_trackmaker << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } 
  else {
    msg() << MSG::INFO << "Retrieved tool " << m_trackmaker << endreq;
  }

  msg() << MSG::INFO << "Using internal seedMaker: " << m_useSeedMaker << endreq;


  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) {
    m_nprint=0; msg()<<MSG::DEBUG<<(*this)<<endreq;
  }
  
  m_nseedsTotal  = 0;
  m_nGoodSeedsTotal =0;
  m_nBadSeedsTotal =0;
  m_ntracksTotal = 0;

  if(!m_doFullScan){
    // Retrieving Region Selector Tool
    if ( m_regionSelector.retrieve().isFailure() ) {
      msg() << MSG::FATAL
	    << "Unable to retrieve RegionSelector tool "
	    << m_regionSelector.type() << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
  }
  else{
    msg() << MSG::INFO
	  << "RegionSelector not initialized due to FullScan mode. " << endreq;
  }

  // Initialize timers:
  m_timerRegSel = addTimer("RegSel");
  m_timerZVertexTool = addTimer("ZVertexTool");
  m_timerSeedsMaker = addTimer("SeedsMaker");
  m_timerTrackMaker = addTimer("TrackMaker");
  m_timerSeedProcessing = addTimer("SeedProcessing");

  return HLT::OK;
}


///////////////////////////////////////////////////////////////////
// Execute HLT Algorithm
///////////////////////////////////////////////////////////////////
HLT::ErrorCode InDet::SiTrigSPSeededTrackFinder::hltExecute(const HLT::TriggerElement*,
							    HLT::TriggerElement* outputTE) {
  
  //----------------------------------------------------------------------
  // Trigger specific part: navigate throw the trigger element to get the
  // ROI center. Use the region selector in order to retrieve the list of
  // detector elements that describe the region of the detector to be 
  // reconstructed.
  //----------------------------------------------------------------------
  
  //initialize monitoring variables
  m_nseeds = 0;
  m_ntracks= 0;  m_nZvtx  = 0;
  m_nDetElPixel = m_nDetElSCT = 0;
  m_RoIEta = m_RoIPhi =0;
  ++m_nInvoked;

  // Get RoiDescriptor
  const TrigRoiDescriptor* roi;
  if ( ( HLT::OK != getFeature(outputTE, roi) ) || !roi ) {
    msg() << MSG::WARNING << "Can't get RoI" << endreq;
    return HLT::NAV_ERROR;
  }
  
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "REGTEST:" << *roi << endreq;
    msg() << MSG::DEBUG << "PhiHalfWidth: " << m_phiHalfWidth << " EtaHalfWidth: "<< m_etaHalfWidth <<endreq;
  }
  m_RoIEta = roi->eta();
  m_RoIPhi = roi->phi();


  
  std::vector<IdentifierHash> listOfSCTIds; 
  std::vector<IdentifierHash> listOfPixIds; 


  
  if(!m_doFullScan){
    if(doTiming()) 
      m_timerRegSel->start();
    
    // SCT hash id's:
    m_regionSelector->DetHashIDList(SCT, *roi, listOfSCTIds );
    
    m_nDetElSCT = listOfSCTIds.size();
    
    // pixels hash id's:
    m_regionSelector->DetHashIDList( PIXEL, *roi, listOfPixIds);
    m_nDetElPixel = listOfPixIds.size();

    if(doTiming()) 
      m_timerRegSel->stop();
  }
  
  const InDet::SiSpacePointsSeed* seed = 0;

  // Test is sct clusters collection for given event
  //
  bool PIX = true;
  bool SCT = true;

  if (m_useSeedMaker){
    if( m_useZvertexTool ) {
      
      if(doTiming()) m_timerZVertexTool->start();
      
      if(!m_doFullScan){
	if (m_fastTracking){
	  m_zvertexmaker->newRegion(listOfPixIds,listOfSCTIds,*roi);
	}
	else {
	  m_zvertexmaker->newRegion(listOfPixIds,listOfSCTIds);
	}
      }
      else{
	m_zvertexmaker->newEvent();
      }
      
      if(doTiming()) m_timerZVertexTool->stop();
      
      if(doTiming()) m_timerSeedsMaker->start();
      
      m_seedsmaker->find3Sp(m_zvertexmaker->getVertices());
      
      if(doTiming()) m_timerSeedsMaker->stop();
      
      if(m_outputlevel <= 0){ 
	msg() << MSG::DEBUG << "REGTEST: Number of zvertex found = " << (m_zvertexmaker->getVertices()).size() << endreq;
      }
      m_nZvtx = (m_zvertexmaker->getVertices()).size();
    }
    else{
      if(doTiming()) m_timerSeedsMaker->start();
      if(!m_doFullScan){
	if (m_fastTracking){
	  m_seedsmaker  ->newRegion(listOfPixIds,listOfSCTIds,*roi);
	} else {
	  m_seedsmaker  ->newRegion(listOfPixIds,listOfSCTIds);
	}

      }
      else{
	m_seedsmaker  ->newEvent();
      }
      
      std::list<Trk::Vertex> VZ; m_seedsmaker->find3Sp(VZ);
      if(doTiming()) m_timerSeedsMaker->stop();
    }
  }

  else   {     //!m_useSeedMaker
    //?
  }
 
  if(doTiming()) m_timerTrackMaker->start();
  if (m_fastTracking){
    m_trackmaker->newTrigEvent(PIX,SCT);
    //m_trackmaker->newEvent(PIX,SCT);
  } else {
    m_trackmaker->newEvent(PIX,SCT);
  }

  // Loop through all seeds and reconsrtucted tracks collection preparation
  

  TrackCollection* foundTracks = new TrackCollection;
  std::multimap<double,Trk::Track*>    qualityTrack;
  
  m_nseeds  = 0;
  m_ntracks = 0;
  m_nGoodSeeds  = 0;
  m_nBadSeeds = 0;
  
  m_timeGoodSeeds=0.0;
  m_timeBadSeeds=0.0;
  m_nGoodSeedsTotal=0;
  m_nBadSeedsTotal=0;
  m_timeGoodSeedsTotal=0;
  m_timeBadSeedsTotal=0;
  
  if (m_useSeedMaker){
    
    ++m_nseeds;
    
    if(doTiming()) 
      m_timerSeedProcessing->start();
    
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Using internal seeding::::  " << endreq;

    int nseedwithtrack(0);
    ///////////////////////////////////////
    
    while((seed = m_seedsmaker->next())) {
      
      ++m_nseeds;
      
      const std::list<Trk::Track*>& T = 
	m_trackmaker->getTracks(seed->spacePoints());
      
      if (m_fastTracking){
	for(std::list<Trk::Track*>::const_iterator t=T.begin(); t!=T.end(); ++t) {
	  if((*t)) qualityTrack.insert(std::make_pair(-trackQuality((*t)),(*t)));
	}
      }
      
      if (msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE << "Using 3SPs seed with ====> " << endreq; 
	for(std::list<const Trk::SpacePoint*>::const_iterator it=seed->spacePoints().begin(); it != seed->spacePoints().end(); it++) {
	  msg() << MSG::VERBOSE << "Using SP with :::: " << endreq; 
	  msg() << " :: r :: " << (*it)->globalPosition().mag() 
		<< " :: perp :: " << (*it)->globalPosition().perp() 
		<< " :: phi :: " << (*it)->globalPosition().phi() 
		<< " :: theta :: " << (*it)->globalPosition().theta() 
		<< " :: eta :: " << (*it)->globalPosition().eta() 
		<< " :: x :: " << (*it)->globalPosition().x() 
		<< " :: y :: " << (*it)->globalPosition().y() 
		<< " :: z :: " << (*it)->globalPosition().z() <<endreq;
	  (*it)->dump(msg());
	}
      }
      
      if(doTiming()){
	m_timerSeedProcessing->stop();
	if(T.empty()) {
	  m_nBadSeeds++;
	  m_timeBadSeeds+=m_timerSeedProcessing->elapsed();
	} 
	else {
	  m_nGoodSeeds++;
	  m_timeGoodSeeds+=m_timerSeedProcessing->elapsed();
	}
      }


      int TRACK_COUNTER(0);
      if (!m_fastTracking){
	for(std::list<Trk::Track*>::const_iterator t=T.begin(); t!=T.end(); ++t) {
	  ++m_ntracks; foundTracks->push_back((*t));
	  
	  TRACK_COUNTER++;
	  
	  if (msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Found track with ====> " << endreq; 
	    for(unsigned int i(0); i<((*t)->trackParameters())->size(); i++) {
	      double trkx = ((*t)->trackParameters()->at(i))->parameters()[Trk::x];
	      double trky = ((*t)->trackParameters()->at(i))->parameters()[Trk::y];
	      double trkz = ((*t)->trackParameters()->at(i))->parameters()[Trk::z];
	      double trktheta = ((*t)->trackParameters()->at(i))->parameters()[Trk::theta];
	      double trkphi = ((*t)->trackParameters()->at(i))->parameters()[Trk::phi];
	      msg() << MSG::VERBOSE << "Track Parameters :::: " << endreq;
	      msg() << " :: r :: " << sqrt( trkx*trkx + trky*trky + trkz*trkz )
		    << " :: perp :: " << sqrt( trkx*trkx + trky*trky )
		    << " :: phi :: " << trkphi
		    << " :: theta :: " << trktheta
		    << " :: eta :: " << -log( tan( trktheta/2 ) )
		    << " :: d0 :: " << ((*t)->trackParameters()->at(i))->parameters()[Trk::d0]
		    << " :: z0 :: " << ((*t)->trackParameters()->at(i))->parameters()[Trk::z0]
		    << " :: x :: " << trkx
		    << " :: y :: " << trky
		    << " :: z :: " << trkz <<endreq;
	    }
	    msg() << MSG::VERBOSE << endreq 
		  << "========================== back ========================== " << endreq << endreq; 
	    ((*t)->trackParameters()->back())->dump(msg());
	    msg() << MSG::VERBOSE << endreq 
		  << "========================================================== " << endreq << endreq; 
	    msg() << MSG::VERBOSE << endreq 
		  << "========================== front ========================== " << endreq << endreq; 
	    ((*t)->trackParameters()->front())->dump(msg());
	    msg() << MSG::VERBOSE << endreq 
		  << "=========================================================== " << endreq << endreq; 
	  }
	}
      }

      if(TRACK_COUNTER!=0)
	{
	  nseedwithtrack++;
	  
	  if (msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Using seed :::: " << m_nseeds << " :::: " << endreq;
	    
	    msg() << MSG::VERBOSE << endreq 
		  << "========================== DUMP SEED ========================== " << endreq << endreq; 
	    seed->dump(msg());
	    msg() << MSG::VERBOSE << endreq 
		  << "====================== END DUMP SEED ========================== " << endreq << endreq; 
	    
	    
	    msg() <<MSG::VERBOSE
		  << "TRACK_COUNTER = " << TRACK_COUNTER 
		  << " ====> Found " << TRACK_COUNTER << " track(s) with this seed. " << endreq;
	  }
	}
    }
    
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Processed number of seeds = " << m_nseeds << ". " << endreq; 
      msg() << MSG::DEBUG << "Number of seeds with at least 1 track = " << nseedwithtrack << ". " << endreq; 
    }
    
  } else {      // external seeding
    

    const std::list<Amg::Vector3D> dummyp;

    const TrackCollection *tx;

    if (getFeature(outputTE, tx) != HLT::OK || !tx) {
      msg() << MSG::ERROR << "Can't get InDetTrackCollection " << endreq;
      return HLT::NAV_ERROR;
    }
       
    TrackCollection::const_iterator itt, ite;
    itt = tx->begin();
    ite = tx->end();
      
    while (itt != ite){
      ++m_nseeds;
      
      const Trk::Perigee* perig = (*itt)->perigeeParameters();
      /////////////////////////////////////////////
      if (msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE << "Using seed with ====> " << endreq; 
	msg() << MSG::VERBOSE 
	      << "========================== DUMP Measured Perigee ========================== " << endreq << endreq; 
	double MPphi = perig->parameters()[Trk::phi]; 
	double MPtheta = perig->parameters()[Trk::theta]; 
	double MPd0 = perig->parameters()[Trk::d0]; 
	double MPz0 = perig->parameters()[Trk::z0]; 
	double MPx = perig->parameters()[Trk::x]; 
	double MPy = perig->parameters()[Trk::y]; 
	double MPz = perig->parameters()[Trk::z]; 
	msg() << " :: r :: " << sqrt( MPx*MPx + MPy*MPy + MPz*MPz )
	      << " :: perp :: " << sqrt( MPx*MPx + MPy*MPy )
	      << " :: phi :: " << MPphi
	      << " :: theta :: " << MPtheta
	      << " :: eta :: " << -log( tan( MPtheta/2 ) )
	      << " :: d0 :: " << MPd0
	      << " :: z0 :: " << MPz0
	      << " :: x :: " << MPx
	      << " :: y :: " << MPy
	      << " :: z :: " << MPz << endreq;
	perig->dump(msg());
	msg() << MSG::VERBOSE 
	      << "========================== DUMP Measured Perigee ========================== " << endreq << endreq; 
      }
      
      std::list<Amg::Vector3D> gpList;
      
      gpList.clear();
      

      DataVector<const Trk::TrackStateOnSurface>::const_iterator it= (*itt)->trackStateOnSurfaces()->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator ite= (*itt)->trackStateOnSurfaces()->end();

      //TODO selection
      while (it != ite) {
	if ((*it)->type(Trk::TrackStateOnSurface::Measurement)){
	  const Trk::MeasurementBase* mesh = (*it)->measurementOnTrack();
	  gpList.push_back(mesh->globalPosition());
	}
	it++;
      }
      
      if (msgLvl() <= MSG::VERBOSE) {
	
	msg() << MSG::VERBOSE << "Using gpList with ====> " << endreq; 
	for(std::list<Amg::Vector3D>::iterator it=gpList.begin(); it != gpList.end(); it++) {
	  msg() << MSG::VERBOSE << "Global Position :::: " << endreq; 
	  msg() << " :: r :: " << (*it).mag() 
		<< " :: perp :: " << (*it).perp() 
		<< " :: phi :: " << (*it).phi() 
		<< " :: theta :: " << (*it).theta() 
		<< " :: eta :: " << (*it).eta() 
		<< " :: x :: " << (*it).x() 
		<< " :: y :: " << (*it).y() 
		<< " :: z :: " << (*it).z() <<endreq;
	}
      }
      
	  
	const std::list<Trk::Track*>& T = 
	  m_trackmaker->getTracks(*perig, gpList);//dummyp); //
	  
	  
	if(doTiming()){
	  m_timerSeedProcessing->stop();
	  if(T.empty()) {
	    m_nBadSeeds++;
	    m_timeBadSeeds+=m_timerSeedProcessing->elapsed();
	  } 
	  else {
	    m_nGoodSeeds++;
	    m_timeGoodSeeds+=m_timerSeedProcessing->elapsed();
	  }
	}
	  
	///////////////////////////////////////
	int TRACK_COUNTER(0);
	///////////////////////////////////////
	for(std::list<Trk::Track*>::const_iterator t=T.begin(); t!=T.end(); ++t) {
	  ++m_ntracks; foundTracks->push_back((*t));
	    
	  ///////////////////////////////////////
	  TRACK_COUNTER++;
	  if (msgLvl() <= MSG::VERBOSE) {

	    msg() << MSG::VERBOSE << "Found track with ====> " << endreq; 
	    for(unsigned int i(0); i<((*t)->trackParameters())->size(); i++) {
	      double trkx = ((*t)->trackParameters()->at(i))->parameters()[Trk::x];
	      double trky = ((*t)->trackParameters()->at(i))->parameters()[Trk::y];
	      double trkz = ((*t)->trackParameters()->at(i))->parameters()[Trk::z];
	      double trktheta = ((*t)->trackParameters()->at(i))->parameters()[Trk::theta];
	      double trkphi = ((*t)->trackParameters()->at(i))->parameters()[Trk::phi];
	      msg() << MSG::VERBOSE << "Track Parameters :::: " << endreq;
	      msg() << " :: r :: " << sqrt( trkx*trkx + trky*trky + trkz*trkz )
		    << " :: perp :: " << sqrt( trkx*trkx + trky*trky )
		    << " :: phi :: " << trkphi
		    << " :: theta :: " << trktheta
		    << " :: eta :: " << -log( tan( trktheta/2 ) )
		    << " :: d0 :: " << ((*t)->trackParameters()->at(i))->parameters()[Trk::d0]
		    << " :: z0 :: " << ((*t)->trackParameters()->at(i))->parameters()[Trk::z0]
		    << " :: x :: " << trkx
		    << " :: y :: " << trky
		    << " :: z :: " << trkz <<endreq;
	    }
	    msg() << MSG::VERBOSE << endreq 
		  << "========================== back ========================== " << endreq << endreq; 
	    ((*t)->trackParameters()->back())->dump(msg());
	    msg() << MSG::VERBOSE << endreq 
		  << "========================================================== " << endreq << endreq; 
	    msg() << MSG::VERBOSE << endreq 
		  << "========================== front ========================== " << endreq << endreq; 
	    ((*t)->trackParameters()->front())->dump(msg());
	    msg() << MSG::VERBOSE << endreq 
		  << "=========================================================== " << endreq << endreq; 
	    ///////////////////////////////////////
	  }
	}
	///////////////////////////////////////
	if (msgLvl() <= MSG::DEBUG) {
	  msg() <<MSG::DEBUG 
		<< "TRACK_COUNTER = " << TRACK_COUNTER 
		<< " ====> Found " << TRACK_COUNTER << " track(s) with this seed. " << endreq;
	}
	///////////////////////////////////////
	  
	  
      ++itt;
	
    }
      
  } //external seeding
    /*
      delete newTracks;
    */
    //    
    
    
    
  if(doTiming()){
    if (m_nGoodSeeds) m_AvtimeGoodSeeds=m_timeGoodSeeds/m_nGoodSeeds;
    if (m_nBadSeeds)  m_AvtimeBadSeeds=m_timeBadSeeds/m_nBadSeeds;
  }
    
    
  
  
  m_trackmaker->endEvent();

  if (m_fastTracking){
    // Remove shared tracks with worse quality
    //
    filterSharedTracks(qualityTrack);
    std::multimap<double,Trk::Track*>::iterator 
      q = qualityTrack.begin(), qe =qualityTrack.end(); 
    for(; q!=qe; ++q) {
      foundTracks->push_back((*q).second); ++m_ntracks; 
    }

  }
    
  if(doTiming()) 
    m_timerTrackMaker->stop();
    
  m_nGoodSeedsTotal = m_nGoodSeeds;
  m_nBadSeedsTotal = m_nBadSeeds;
  m_nseedsTotal += m_nseeds;
  m_ntracksTotal += m_ntracks;
    
  if(m_outputlevel<=0) 
    msg() << MSG::DEBUG << "REGTEST: Number of seeds found = " << m_nseeds << endreq;
    
    
  // Save reconstructed tracks
  const std::string sgkey = "SPSeeded";
  HLT::ErrorCode ec = attachFeature(outputTE, foundTracks, sgkey);
  if ( ec != HLT::OK ) {
    msg() << MSG::ERROR << "Could not record feature " << sgkey << endreq;
  } 
    
  // Print common event information
  //
  if(m_outputlevel<=0) {
    msg() << MSG::DEBUG << "REGTEST: Investigated "
	  << std::setw(7) <<  m_nseeds 
	  << " and found "
	  << std::setw(5) << foundTracks->size() 
	  << " tracks stored to SG/" << sgkey
	  << endreq;
    // m_nprint=1; msg()<<MSG::DEBUG<<(*this)<<endreq;
  }
    
  if (msgLvl() <= MSG::DEBUG){
    for (size_t i=0; i<foundTracks->size() ; i++){
      msg() << MSG::DEBUG << "REGTEST track " << i << endreq;
      msg() << MSG::VERBOSE << *(foundTracks->at(i)) << endreq;
    }
  }
    
  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////
HLT::ErrorCode InDet::SiTrigSPSeededTrackFinder::hltFinalize() {
  
  m_nprint=2; msg()<<MSG::INFO<<(*this)<<endreq;
  msg() << MSG::INFO << "REGTEST Investigated " << m_nseedsTotal
	<< " seeds and found " << m_ntracksTotal 
	<< ". Invoked " << m_nInvoked << " times." << endreq;
  return HLT::OK;
}
//----------------------------------
//          endRun method:
//----------------------------------------------------------------------------
HLT::ErrorCode InDet::SiTrigSPSeededTrackFinder::hltEndRun() {
   
  msg() << MSG::INFO << "SiTrigSPSeededTrackFinder::endRun()" << endreq;
 
  return HLT::OK;
}
 
//---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::SiTrigSPSeededTrackFinder& se)
{ 
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////
std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SiTrigSPSeededTrackFinder& se)
{
  return se.dump(sl);
}   

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::SiTrigSPSeededTrackFinder::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpevent(out); return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::SiTrigSPSeededTrackFinder::dumptools( MsgStream& out ) const
{
  int n = 65-m_zvertexmaker.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  n     = 65-m_seedsmaker.type().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 65-m_trackmaker.type().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");
  
  out<<"|----------------------------------------------------------------"
     <<"--------------------------------|"
     <<std::endl;
  if(m_useZvertexTool)
    out<<"| Tool for primary vertices z-coordinates finding | "<<m_zvertexmaker.type()<<s1
       <<std::endl;
  out<<"| Tool for space points seeds             finding | "<<m_seedsmaker.type()<<s2
     <<std::endl;
  out<<"| Tool for space points seeded track      finding | "<<m_trackmaker.type()<<s3
     <<std::endl;
  out<<"|----------------------------------------------------------------"
     <<"--------------------------------|"
     <<std::endl;
  return out;
}
///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::SiTrigSPSeededTrackFinder::dumpevent( MsgStream& out ) const
{
  int ns = m_nseeds;
  int nt = m_ntracks;
  if(m_nprint > 1) {ns = m_nseedsTotal; nt = m_ntracksTotal;}

  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  out<<"|  Investigated "
     <<std::setw(7)<<ns<<" space points seeds and found "
     <<std::setw(7)<<nt<<" tracks                              |"
     <<std::endl;
  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  return out;
}
///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////
std::ostream& InDet::SiTrigSPSeededTrackFinder::dump( std::ostream& out ) const
{
  return out;
}


///////////////////////////////////////////////////////////////////
// Track quality calculation
///////////////////////////////////////////////////////////////////

double InDet::SiTrigSPSeededTrackFinder::trackQuality(const Trk::Track* Tr)
{
 
 DataVector<const Trk::TrackStateOnSurface>::const_iterator  
    m  = Tr->trackStateOnSurfaces()->begin(), 
    me = Tr->trackStateOnSurfaces()->end  ();

 double quality = 0. ;
 double W       = 17.;

 for(; m!=me; ++m) {
   const Trk::FitQualityOnSurface* fq =  (*m)->fitQualityOnSurface();
   if(!fq) continue;

   double x2 = fq->chiSquared();
   double q;
   if(fq->numberDoF() == 2) q = (1.2*(W-x2*.5)); 
   else                     q =      (W-x2    );
   if(q < 0.) q = 0.; quality+=q;
 }
 return quality;
}

///////////////////////////////////////////////////////////////////
// Filter shared tracks
///////////////////////////////////////////////////////////////////

void InDet::SiTrigSPSeededTrackFinder::filterSharedTracks
(std::multimap<double,Trk::Track*>& QT)
{
  std::set<const Trk::PrepRawData*> clusters;
  std::multimap<double,Trk::Track*>::iterator q = QT.begin();
  
  const Trk::PrepRawData* prd[100]; 

  
  while(q!=QT.end()) {

    std::set<const Trk::PrepRawData*>::iterator fe = clusters.end();

    DataVector<const Trk::MeasurementBase>::const_iterator 
      m  = (*q).second->measurementsOnTrack()->begin(), 
      me = (*q).second->measurementsOnTrack()->end  ();
 
    int nf = 0, nc = 0; 
    for(; m!=me; ++m) {

      const Trk::PrepRawData* pr = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
      if(pr) {
	++nc; if(clusters.find(pr)==fe) {prd[nf++]=pr; if(nf==100) break;}
      }
    }

    if(nf >= m_nfreeCut|| nf == nc) {
      for(int n=0; n!=nf; ++n) clusters.insert(prd[n]); ++q;
    } 
    else  {
      delete (*q).second; QT.erase(q++);
    }
  }
}

