/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSeededSegmentFinder.h"

#include "MuonTrackMakerUtils/MuonTrackMakerStlTools.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "TrkSurfaces/StraightLineSurface.h"

#include "TrkRoad/TrackRoad.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include <iostream>

namespace Muon {

  MuonSeededSegmentFinder::MuonSeededSegmentFinder(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa),
      m_magFieldProperties(Trk::NoField)
  {
    declareInterface<IMuonSeededSegmentFinder>(this);

    declareProperty("AdcCut", m_adcCut = 50 );
    declareProperty("MdtSigmaFromTrack",m_maxSigma = 3);
  }

  StatusCode MuonSeededSegmentFinder::initialize()
  {

    ATH_CHECK(m_DetectorManagerKey.initialize());
    ATH_CHECK(m_segMaker.retrieve());
    ATH_CHECK(m_segMakerNoHoles.retrieve());
    ATH_CHECK(m_propagator.retrieve());
    ATH_CHECK(m_mdtRotCreator.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_printer.retrieve());

    ATH_CHECK(m_key_mdt.initialize());
    ATH_CHECK(m_key_csc.initialize(!m_key_csc.empty())); // check for layouts without CSCs
    ATH_CHECK(m_key_tgc.initialize());
    ATH_CHECK(m_key_rpc.initialize());
    ATH_CHECK(m_key_stgc.initialize(!m_key_stgc.empty())); // check for layouts without STGCs
    ATH_CHECK(m_key_mm.initialize(!m_key_mm.empty())); // check for layouts without MicroMegas

    return StatusCode::SUCCESS;
  }

  std::unique_ptr<Trk::SegmentCollection> MuonSeededSegmentFinder::find( const Trk::TrackParameters& pars, const std::set<Identifier>& chIds ) const {

    // get MdtPrepData collections correspondign to the chamber Identifiers
    std::vector<const MdtPrepData*> mdtPrds = extractPrds( chIds );

    if ( mdtPrds.empty() ) {
      ATH_MSG_DEBUG(" no MdtPrepData found ");
      return std::unique_ptr<Trk::SegmentCollection>();
    }

    // find segments
    return find(pars, mdtPrds);
  }

  std::unique_ptr<Trk::SegmentCollection> MuonSeededSegmentFinder::find( const Trk::TrackParameters& pars, const std::set<IdentifierHash>& chIdHs ) const {

    // get MdtPrepData collections correspondign to the chamber Identifiers
    std::vector<const MdtPrepData*> mdtPrds = extractPrds( chIdHs );
    
    if ( mdtPrds.empty() ) {
      ATH_MSG_DEBUG(" no MdtPrepData found ");
      return std::unique_ptr<Trk::SegmentCollection>();
    }

    // find segments
    return find(pars, mdtPrds);
  }


  std::unique_ptr<Trk::SegmentCollection> MuonSeededSegmentFinder::find( const Trk::TrackParameters& pars,
									 const std::vector<const MdtPrepData*>& mdtPrds ) const {

    // are we close to the chamber edge?
    bool doHoleSearch = true;

    // select and calibrate the MdtPrepData
    std::vector<const MdtDriftCircleOnTrack*> mdtROTs;
    mdtROTs.reserve( mdtPrds.size() );
    selectAndCalibrate( pars, mdtPrds, mdtROTs, doHoleSearch );

    if ( mdtROTs.empty() ) {
      ATH_MSG_DEBUG(" no MdtDriftCircles selected ");
      return 0;
    }

    // create track road
    double roadWidthEta = 1.;
    if ( pars.covariance() ) {
      double trackError = Amg::error(*pars.covariance(), Trk::theta);
      ATH_MSG_DEBUG(" local track Error on theta " << trackError);
      if ( trackError < 0.2 ) trackError = 0.2;
      roadWidthEta = 5.*trackError;
    }
    Trk::TrackRoad road( pars.position(), pars.momentum(), roadWidthEta, 1. );
    
    // create dummy vector<vector>
    std::vector< std::vector<const MdtDriftCircleOnTrack*> > mdtROTsVec;
    mdtROTsVec.push_back(mdtROTs);
    std::vector< std::vector<const MuonClusterOnTrack*> >    clusterROTsVec;

    // call segment finder
    std::unique_ptr<Trk::SegmentCollection> segments(new Trk::SegmentCollection());
    doHoleSearch ?
      m_segMaker->find( road, mdtROTsVec, clusterROTsVec, segments.get(), true ) :
      m_segMakerNoHoles->find( road, mdtROTsVec, clusterROTsVec, segments.get(), true );

    // delete ROTs
    std::for_each( mdtROTs.begin(), mdtROTs.end(), MuonDeleteObject<const MdtDriftCircleOnTrack>() );

    if ( !segments ) {
      ATH_MSG_DEBUG(" No segments found ");
    } else {
      ATH_MSG_DEBUG(" Number of segments found: " << segments->size());
    }

    return segments;
  }

  std::vector<const MdtPrepData*> MuonSeededSegmentFinder::extractPrds( const std::set<Identifier>& chIds ) const {

    SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
    const MuonGM::MuonDetectorManager* MuonDetMgr{*DetectorManagerHandle}; 
    if(MuonDetMgr==nullptr){
      ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
      // return; 
    } 

    // set of IdHashes corresponding to these identifiers
    std::set<IdentifierHash> chIdHs;

    // loop over chambers and get collections
    std::set<Identifier>::const_iterator chit = chIds.begin();
    std::set<Identifier>::const_iterator chit_end = chIds.end();
    for ( ; chit != chit_end; ++chit ) {

      if ( !m_idHelperSvc->isMdt(*chit) ) {
	ATH_MSG_WARNING(" Requested chamber is not an MDT:   " << m_idHelperSvc->toStringChamber(*chit));
	continue;
      }

      const MuonGM::MdtReadoutElement* detEl = MuonDetMgr->getMdtReadoutElement(*chit);
      if ( !detEl ) {
	ATH_MSG_WARNING(" Requested chamber does not exist in geometry:   " << m_idHelperSvc->toStringChamber(*chit));
	continue;
      }
      IdentifierHash hash_id = detEl->collectionHash();
      chIdHs.insert(hash_id);
    }

    // vector to store pointers to collections
    std::vector<const MdtPrepData*> mdtPrds = extractPrds( chIdHs );

    return mdtPrds;
  }

  std::vector<const MdtPrepData*> MuonSeededSegmentFinder::extractPrds( const std::set<IdentifierHash>& chIdHs ) const {

    SG::ReadHandle<Muon::MdtPrepDataContainer> h_mdtPrdCont(m_key_mdt);
    const Muon::MdtPrepDataContainer *mdtPrdContainer;
    if (h_mdtPrdCont.isValid()) {
      mdtPrdContainer = h_mdtPrdCont.cptr();
    }
    else{
      ATH_MSG_WARNING("Cannot retrieve mdtPrepDataContainer " << m_key_mdt.key());
      return std::vector<const MdtPrepData*>();
    }
      
    // vector to store pointers to collections 
    std::vector<const MdtPrepData*> mdtPrds;

    // loop over chambers and get collections
    std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
    std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
    for( ;chit!=chit_end;++chit ){
      MdtPrepDataContainer::const_iterator colIt = mdtPrdContainer->indexFind(*chit);
      if( colIt == mdtPrdContainer->end() ){
	continue;
      }
      
      // reserve space for the new PRDs
      mdtPrds.insert( mdtPrds.end(), (*colIt)->begin(), (*colIt)->end() );
    }

    return mdtPrds;
  }

  void MuonSeededSegmentFinder::extractMdtPrdCols( const std::set<IdentifierHash>& chIdHs, 
						   std::vector<const MdtPrepDataCollection*>& target ) const {

    SG::ReadHandle<Muon::MdtPrepDataContainer> h_mdtPrdCont(m_key_mdt);
    const Muon::MdtPrepDataContainer *mdtPrdContainer;
    if (h_mdtPrdCont.isValid()) {
      mdtPrdContainer = h_mdtPrdCont.cptr();
    }
    else{
      ATH_MSG_WARNING("Cannot retrieve mdtPrepDataContainer " << m_key_mdt.key());
      return;
    }
    if(mdtPrdContainer->size()==0) return;

    // loop over chambers and get collections
    std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
    std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
    for( ;chit!=chit_end;++chit ){
      MdtPrepDataContainer::const_iterator colIt = mdtPrdContainer->indexFind(*chit);
      if( colIt == mdtPrdContainer->end() || (*colIt)->empty() ){
	continue;
      }
      ATH_MSG_DEBUG(" Adding for:   " 
		    << m_idHelperSvc->toStringChamber( (*colIt)->front()->identify() ) << "  size " 
		    << (*colIt)->size());
      
      // reserve space for the new PRDs
      target.push_back(*colIt);
    }
  }


  void MuonSeededSegmentFinder::extractRpcPrdCols( const std::set<IdentifierHash>& chIdHs, 
						   std::vector<const RpcPrepDataCollection*>& target ) const {

    SG::ReadHandle<Muon::RpcPrepDataContainer> h_rpcPrdCont(m_key_rpc);
    const Muon::RpcPrepDataContainer *rpcPrdContainer;
    if (h_rpcPrdCont.isValid()){
      rpcPrdContainer = h_rpcPrdCont.cptr();
    }
    else{
      ATH_MSG_WARNING("Cannot retrieve rpcPrepDataContainer " << m_key_rpc.key());
      return;
    }
    if(rpcPrdContainer->size()==0) return;
      
    // loop over chambers and get collections
    std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
    std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
    for( ;chit!=chit_end;++chit ){
      RpcPrepDataContainer::const_iterator colIt = rpcPrdContainer->indexFind(*chit);
      if( colIt == rpcPrdContainer->end() || (*colIt)->empty() ){
	continue;
      }
      ATH_MSG_DEBUG(" Adding for:   " 
		    << m_idHelperSvc->toStringChamber( (*colIt)->front()->identify() ) << "  size " 
		    << (*colIt)->size());
      
      // reserve space for the new PRDs
      target.push_back(*colIt);
    }
  }


  void MuonSeededSegmentFinder::extractTgcPrdCols( const std::set<IdentifierHash>& chIdHs, 
						   std::vector<const TgcPrepDataCollection*>& target ) const {

    SG::ReadHandle<Muon::TgcPrepDataContainer> h_tgcPrdCont(m_key_tgc);
    const Muon::TgcPrepDataContainer *tgcPrdContainer;
    if(h_tgcPrdCont.isValid()) {
      tgcPrdContainer = h_tgcPrdCont.cptr();
    }
    else{
      ATH_MSG_WARNING("Cannot retrieve tgcPrepDataContainer " << m_key_tgc.key());
      return;
    }
    if(tgcPrdContainer->size()==0) return;

    // loop over chambers and get collections
    std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
    std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
    for( ;chit!=chit_end;++chit ){
      TgcPrepDataContainer::const_iterator colIt = tgcPrdContainer->indexFind(*chit);
      if( colIt == tgcPrdContainer->end() || (*colIt)->empty() ){
	continue;
      }
      ATH_MSG_DEBUG(" Adding for:   " 
		    << m_idHelperSvc->toStringChamber( (*colIt)->front()->identify() ) << "  size " 
		    << (*colIt)->size());

      // reserve space for the new PRDs
      target.push_back(*colIt);
    }
  }


  void MuonSeededSegmentFinder::extractCscPrdCols( const std::set<IdentifierHash>& chIdHs,
						   std::vector<const CscPrepDataCollection*>& target ) const {

    if(m_key_csc.key()==""){
      ATH_MSG_DEBUG("No CSC collection");
      return;
    }

    SG::ReadHandle<Muon::CscPrepDataContainer> h_cscPrdCont(m_key_csc);
    const Muon::CscPrepDataContainer *cscPrdContainer;
    if(h_cscPrdCont.isValid()) {
      cscPrdContainer = h_cscPrdCont.cptr();
    }
    else{
      ATH_MSG_WARNING("Cannot retrieve cscPrepDataContainer " << m_key_csc.key());
      return;
    }
    if(cscPrdContainer->size()==0) return;

    // loop over chambers and get collections
    std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
    std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
    for( ;chit!=chit_end;++chit ){
      CscPrepDataContainer::const_iterator colIt = cscPrdContainer->indexFind(*chit);
      if( colIt == cscPrdContainer->end() || (*colIt)->empty() ){
	continue;
      }
      
      ATH_MSG_DEBUG(" Adding for:   " 
		    << m_idHelperSvc->toStringChamber( (*colIt)->front()->identify() ) << "  size " 
		    << (*colIt)->size());

      // reserve space for the new PRDs
      target.push_back(*colIt);
    }
  }


  // New Small Wheel                                                                                                                                                                                     
  // sTGC                                                                                                                                                                                                
  void MuonSeededSegmentFinder::extractsTgcPrdCols( const std::set<IdentifierHash>& chIdHs,
						    std::vector<const sTgcPrepDataCollection*>& target ) const {

    if(m_key_stgc.key()==""){
      ATH_MSG_DEBUG("no sTGC collection");
      return;
    }

    SG::ReadHandle<Muon::sTgcPrepDataContainer> h_stgcPrdCont(m_key_stgc);
    const Muon::sTgcPrepDataContainer *stgcPrdContainer;
    if(h_stgcPrdCont.isValid()) {
      stgcPrdContainer = h_stgcPrdCont.cptr();
    }
    else{
      ATH_MSG_WARNING("Cannot retrieve stgcPrepDataContainer " << m_key_stgc.key());
      return;
    }
    if(stgcPrdContainer->size()==0) return;

    // loop over chambers and get collections                                                                                                                                                          
    std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
    std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
    for( ;chit!=chit_end;++chit ){
      sTgcPrepDataContainer::const_iterator colIt = stgcPrdContainer->indexFind(*chit);
      if( colIt == stgcPrdContainer->end() || (*colIt)->empty() ){                                                                                                           
        continue;
      }
      ATH_MSG_DEBUG(" Adding for:   "
                    << m_idHelperSvc->toStringChamber( (*colIt)->front()->identify() ) << "  size "
                    << (*colIt)->size());

      // reserve space for the new PRDs                                                                                                                                                                
      target.push_back(*colIt);
    }

    if(chIdHs.size()==0) {
      // Temp fix because Region selector is not implemented  
      sTgcPrepDataContainer::const_iterator colIt     = stgcPrdContainer->begin();
      sTgcPrepDataContainer::const_iterator colIt_end = stgcPrdContainer->end();
      for ( ; colIt != colIt_end; ++colIt ) {
	if((*colIt)->size()>0) {  
	  ATH_MSG_DEBUG(" Adding ALL Prds for:   "
			<< m_idHelperSvc->toStringChamber( (*colIt)->front()->identify() ) << "  size "
			<< (*colIt)->size());
	  // reserve space for the new PRDs
	  target.push_back(*colIt);
	}
      }
    }
  }

  // MM
  void MuonSeededSegmentFinder::extractMMPrdCols( const std::set<IdentifierHash>& chIdHs,
						  std::vector<const MMPrepDataCollection*>& target ) const {

    if(m_key_mm.key()==""){
      ATH_MSG_DEBUG("no MM collection");
      return;
    }

    SG::ReadHandle<Muon::MMPrepDataContainer> h_mmPrdCont(m_key_mm);
    const Muon::MMPrepDataContainer *mmPrdContainer;
    if(h_mmPrdCont.isValid()) {
      mmPrdContainer = h_mmPrdCont.cptr();
    }
    else{
      ATH_MSG_WARNING("Cannot retrieve mmPrepDataContainer " << m_key_mm.key());
      return;
    }
    if(mmPrdContainer->size()==0) return;

    // loop over chambers and get collections
    std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
    std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
    for ( ; chit != chit_end; ++chit ) {
      MMPrepDataContainer::const_iterator colIt = mmPrdContainer->indexFind(*chit);
      if ( colIt == mmPrdContainer->end() || (*colIt)->empty() ) {
	continue;
      }
      ATH_MSG_DEBUG(" Adding for:   "
		    << m_idHelperSvc->toStringChamber( (*colIt)->front()->identify() ) << "  size "
		    << (*colIt)->size());
      
      target.push_back(*colIt);
    }
    
    if(chIdHs.size()==0) {
      // Temp fix because Region selector is not implemented  
      MMPrepDataContainer::const_iterator colIt     = mmPrdContainer->begin();
      MMPrepDataContainer::const_iterator colIt_end = mmPrdContainer->end();
      for ( ; colIt != colIt_end; ++colIt ) {
	if((*colIt)->size()>0) {  
	  ATH_MSG_DEBUG(" Adding ALL Prds for:   "
			<< m_idHelperSvc->toStringChamber( (*colIt)->front()->identify() ) << "  size "
			<< (*colIt)->size());
	  target.push_back(*colIt);
	}
      }
    }
    
  }



  void MuonSeededSegmentFinder::selectAndCalibrate( const Trk::TrackParameters& pars,
						    const std::vector<const MdtPrepData*>& mdtPrdCols,
						    std::vector<const MdtDriftCircleOnTrack*>& mdtROTs,
						    bool& doHoleSearch ) const {
    
    ATH_MSG_VERBOSE(" in selectAndCalibrate, get PRDs  " << mdtPrdCols.size());
    
    // loop over MdtPrepDataCollections
    std::vector<const MdtPrepData*>::const_iterator mit = mdtPrdCols.begin();
    std::vector<const MdtPrepData*>::const_iterator mit_end = mdtPrdCols.end();
    for ( ; mit != mit_end; ++mit ) {
      if ((*mit)->status() != 1) continue;
      // calibrate MDT
      const MdtDriftCircleOnTrack* mdt = handleMdtPrd( pars, **mit, doHoleSearch );
      
      // not selected
      if ( !mdt ) continue;
      
      mdtROTs.push_back(mdt);
    }
    ATH_MSG_VERBOSE(" calibrated " << mdtROTs.size() << " prds out of " << mdtPrdCols.size() );

  }

  const MdtDriftCircleOnTrack* MuonSeededSegmentFinder::handleMdtPrd( const Trk::TrackParameters& pars,
								      const MdtPrepData& mdtPrd,
								      bool& doHoleSearch ) const {
    
    // skip noise hits
    if ( mdtPrd.adc() < m_adcCut ) return 0;

    // get surface of PRD
    const Identifier& id = mdtPrd.identify();
    const MuonGM::MdtReadoutElement& detEl = *mdtPrd.detectorElement();
    const Trk::StraightLineSurface& surf = detEl.surface(id);

    // propagate segment parameters to first measurement
    const Trk::TrackParameters* exPars = m_propagator->propagate(pars, surf, Trk::anyDirection, false, m_magFieldProperties);
    if ( !exPars ) {
      ATH_MSG_DEBUG(" Propagation failed!! ");
      return 0;
    }

    // calculate position on wire + error
    double distanceToWire = exPars->parameters()[Trk::locR];
    double posAlongWire = exPars->parameters()[Trk::locZ];

    double errorR = exPars->covariance() ? fabs(Amg::error(*exPars->covariance(), Trk::locR)) : 500.;
    double errorZ = exPars->covariance() ? fabs(Amg::error(*exPars->covariance(), Trk::locZ)) : 300.;

    // range check
    bool isOnSurface = surf.isOnSurface(exPars->position(), true, 5 * errorR, 5 * errorZ);

    // get tube length
    int layer = m_idHelperSvc->mdtIdHelper().tubeLayer(id);
    int tube  = m_idHelperSvc->mdtIdHelper().tube(id);
    double halfTubeLength = 0.5 * detEl.getActiveTubeLength(layer, tube);
    double tubeRadius     = detEl.innerTubeRadius();

    // take into account the tube width
    double roadWidthR = 5 * errorR + 4 * tubeRadius;
    double roadWidthZ = 5 * errorZ + 100.;

    double driftdr = Amg::error(mdtPrd.localCovariance(), Trk::locR);
    double nSigmaFromTrack = fabs(fabs(distanceToWire) - mdtPrd.localPosition()[Trk::locR]) / sqrt(errorR * errorR + driftdr * driftdr);

    if ( msgLvl(MSG::VERBOSE) ) {
      std::string boundCheckStr = isOnSurface ? "  onSurface" : " outOfSurface";
      msg() << MSG::VERBOSE << "  " << m_idHelperSvc->toString(mdtPrd.identify())
	    << " r " << distanceToWire << " range " << roadWidthR
	    << " z " << posAlongWire << " range " << halfTubeLength + roadWidthZ
	    << boundCheckStr;
    }

    //if( fabs(distanceToWire) > roadWidthR || fabs(posAlongWire) > halfTubeLength + roadWidthZ ){
    if (nSigmaFromTrack > m_maxSigma || fabs(posAlongWire) > halfTubeLength + roadWidthZ ) {
      if ( msgLvl(MSG::VERBOSE) ) msg() << " --- dropped" << endmsg;
      delete exPars;
      return 0;
    }



    // update hole search flag, set to false if we are close to the tube edge
    if ( doHoleSearch && fabs(posAlongWire) < halfTubeLength + roadWidthZ ) doHoleSearch = false;

    // pointer to ROT
    const MdtDriftCircleOnTrack* mdtROT = m_mdtRotCreator->createRIO_OnTrack(mdtPrd, exPars->position(), &(exPars->momentum()));

    // clean up pointers
    delete exPars;

    // check whether ROT is created
    if ( !mdtROT ) {
      ATH_MSG_DEBUG(" failed to calibrate MdtPrepData " << m_idHelperSvc->toString(mdtPrd.identify()));
      return 0;
    }


    if ( msgLvl(MSG::VERBOSE) ) {
      double radius = mdtROT->localParameters()[Trk::locR];
      double error  = driftdr;
      double residual = radius - fabs(distanceToWire);
      double fullError = sqrt( errorR * errorR + error * error );
      double radialPull = residual / fullError;
      std::string hitType;
      if ( fabs(radialPull) < 5 ) hitType = "onTrack";
      else if ( fabs(radialPull) > 5 && residual > 0 )  hitType = "delta";
      else                       hitType = "outOfTime";
      msg() << " r_drift  " << radius << " res " << residual  << " pull " << radialPull << " "  << hitType << endmsg;
    }
    return mdtROT;
  }


}
