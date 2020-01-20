/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Ketevi A. Assamagan, Woochun Park
/// BNL, April 03 2005

/// algorithm to decode RDO into PrepRawData

#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"

#include "CscRdoToCscPrepDataToolCore.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "EventPrimitives/EventPrimitives.h" 

#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "CscCalibTools/ICscCalibTool.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"

#include "GaudiKernel/ThreadLocalContext.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

static const InterfaceID IID_ICscRdoToCscPrepDataToolCore
("CscRdoToCscPrepDataToolCore",1,0);

const InterfaceID& CscRdoToCscPrepDataToolCore::interfaceID()
{ 
  return IID_ICscRdoToCscPrepDataToolCore; 
}

CscRdoToCscPrepDataToolCore::CscRdoToCscPrepDataToolCore
(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_muonMgr(0),
    m_cscCalibTool( "CscCalibTool/CscCalibTool", this),
    m_cscRdoDecoderTool ("Muon::CscRDO_Decoder/CscRDO_Decoder", this),
    m_cabling( "CSCcablingSvc" ,name),
    m_fullEventDone(false) {

  declareInterface<IMuonRdoToPrepDataTool>(this);
  declareProperty("CSCHashIdOffset",     m_cscOffset = 22000);
  declareProperty("DecodeData",          m_decodeData = true ); 
  // tools 
  declareProperty("CscCalibTool",        m_cscCalibTool );
  declareProperty("CscRdoDecoderTool",   m_cscRdoDecoderTool );
  // DataHandle
  declareProperty("RDOContainer",        m_rdoContainerKey = std::string("CSCRDO"),"CscRawDataContainer to retrieve");
  declareProperty("OutputCollection", 	 m_outputCollectionKey = std::string("CSC_Measurements"),"Muon::CscStripPrepDataContainer to record");
}  

// destructor 
CscRdoToCscPrepDataToolCore::~CscRdoToCscPrepDataToolCore(){}

StatusCode CscRdoToCscPrepDataToolCore::finalize() {

  ATH_MSG_DEBUG("in finalize()");

  
  return StatusCode::SUCCESS;

}

StatusCode CscRdoToCscPrepDataToolCore::initialize(){

  ATH_MSG_DEBUG(" in initialize()");

  /// get the detector descriptor manager
  if (detStore()->retrieve( m_muonMgr ).isFailure()) {
    ATH_MSG_ERROR(" Cannot retrieve MuonGeoModel ");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("The Geometry version is " << m_muonMgr->get_DBMuonVersion());

  // get cscCalibTool
  if (m_cscCalibTool.retrieve().isFailure()){
    ATH_MSG_ERROR ( "Can't get handle on CSC calibration tools" );
    return StatusCode::FAILURE;
  } 

  // get cscCalibTool
  if (m_cscRdoDecoderTool.retrieve().isFailure()){
    ATH_MSG_ERROR ( "Can't get handle on CSC RDO Decoder tool" );
    return StatusCode::FAILURE;
  } 
  
  /// create an empty CSC cluster container for filling
  ATH_CHECK( m_muonIdHelperTool.retrieve() );

  
  if ( m_cabling.retrieve().isFailure() )
  {
    ATH_MSG_ERROR ( " Cannot get CSCcablingSvc " );
    return StatusCode::FAILURE;
  }

  // check if initializing of DataHandle objects success
  ATH_CHECK( m_rdoContainerKey.initialize() );
 
  ATH_CHECK( m_outputCollectionKey.initialize() );

  return StatusCode::SUCCESS;
}

//_________________________________________
StatusCode CscRdoToCscPrepDataToolCore::decode(std::vector<IdentifierHash>&, std::vector<IdentifierHash>&) {
  return StatusCode::FAILURE;
}

//*****************************************
//************** Process for the givenId EF Filter case...
StatusCode CscRdoToCscPrepDataToolCore::decode(const CscRawDataContainer* rdoContainer, IdentifierHash givenHashId, std::vector<IdentifierHash>& decodedIdhs) {
  
  
  //typedef CscRawDataContainer::const_iterator collection_iterator;
  
  IdContext cscContext = m_muonIdHelperTool->cscIdHelper().module_context();
  
  // if CSC decoding is switched off stop here
  if( !m_decodeData ) {
    ATH_MSG_DEBUG ( "Stored empty container; Decoding CSC RDO into CSC PrepRawData is switched off" );
    return StatusCode::SUCCESS;
  }

  //These collections can be empty for the trigger
  if(!m_outputCollection || m_outputCollection->size()==0){
    ATH_MSG_DEBUG("Stored empty collection.");
    return StatusCode::SUCCESS;
  }

  // identifiers of collections already decoded and stored in the container will be skipped
  if (m_outputCollection->indexFind(givenHashId) != m_outputCollection->end()) {
    decodedIdhs.push_back(givenHashId);
    ATH_MSG_DEBUG ("A collection already exists in the container for offline id hash. "
                   << (int) givenHashId);
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG ( "Decoding CSC RDO into CSC PrepRawData" );
  /// create the CSC RDO decoder

  //**********************************************
  // retrieve specific collection for the givenID
  uint16_t idColl = 0xffff;
  m_cabling->hash2CollectionId(givenHashId,idColl);
  CscRawDataContainer::const_iterator it_coll = rdoContainer->indexFind(idColl);
  if (rdoContainer->end() ==  it_coll) {
    // unsigned int coll_hash = idColl;  
    ATH_MSG_DEBUG ( "Specific CSC RDO collection retrieving failed for collection hash = " << idColl );
    return StatusCode::SUCCESS;
  }


  const CscRawDataCollection * rawCollection = *it_coll;
  ATH_MSG_DEBUG ( "Retrieved " << rawCollection->size() << " CSC RDOs.");
  //************************************************
  Identifier oldId;
  CscStripPrepDataCollection * collection = 0;
  IdentifierHash cscHashId;

  unsigned int samplingTime = rawCollection->rate();
  unsigned int numSamples   = rawCollection->numSamples();
  bool samplingPhase        = rawCollection->samplingPhase();
  std::vector<float> charges;
  charges.reserve(4);
  std::vector<uint16_t> samples;
  samples.reserve(4);

  if ( int(samplingTime) != int(m_cscCalibTool->getSamplingTime())) {
    ATH_MSG_WARNING ( " CSC sampling time from Collection is NOT consistant to calibTool parameter!!!!!!! " );
  }
  
  /** for each Rdo, loop over RawData, converter RawData to PrepRawData
      retrieve/create PrepRawData collection, and insert PrepRawData into collection */
  CscRawDataCollection::const_iterator itD   =  rawCollection->begin(); 
  CscRawDataCollection::const_iterator itD_e =  rawCollection->end();
      
  for (; itD!=itD_e; ++itD) { 
    const CscRawData * data = (*itD);
    uint16_t width = data->width();
    uint16_t totalSamples = (data->samples()).size();
    
    Identifier stationId = m_cscRdoDecoderTool->stationIdentifier(data);
    uint32_t hashOffset = data->hashId();

    ATH_MSG_DEBUG ( " Size of online cluster in this RawData: " 
		    << " Width = " << width << " Samples = " << totalSamples
		    << " stationId : " << stationId << "  hashOffset : " << hashOffset);

    if (m_muonIdHelperTool->cscIdHelper().get_hash(stationId, cscHashId, &cscContext)) {
      ATH_MSG_WARNING ( "Unable to get CSC digiti collection hash id "
			<< "context begin_index = " << cscContext.begin_index()
			<< " context end_index  = " << cscContext.end_index()
			<< " the identifier is "     );
      stationId.show();
    }

    if (oldId != stationId) {
      Muon::CscStripPrepDataContainer::const_iterator it_coll = m_outputCollection->indexFind(cscHashId);
      if (m_outputCollection->end() == it_coll) {
	CscStripPrepDataCollection * newCollection = new CscStripPrepDataCollection(cscHashId);
	newCollection->setIdentifier(stationId);
	collection = newCollection;
	if ( m_outputCollection->addCollection(newCollection, cscHashId).isFailure() )
	  ATH_MSG_WARNING ("Couldn't record CscStripPrepdataCollection with key="
			   << (unsigned int) cscHashId << " in StoreGate!" );
	decodedIdhs.push_back(cscHashId); //Record that this collection contains data
      } else {  // It won't be needed because we already skipped decoded one (should be checked it's true)

//Hack for transition to athenaMT classes
	CscStripPrepDataCollection * oldCollection = const_cast<CscStripPrepDataCollection*>( *it_coll );
	collection = oldCollection;
	cscHashId = collection->identifyHash();
      }
      oldId = stationId;
    }
    
    for (unsigned int j=0; j<width; ++j) {
      const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(data, j);
      ATH_MSG_DEBUG ( "        LOOP over width  " << j <<  " " << channelId );

      const CscReadoutElement * descriptor = m_muonMgr->getCscReadoutElement(channelId);
      //calculate local positions on the strip planes
      if ( !descriptor ) {
        ATH_MSG_WARNING ( "Invalid descriptor for " << m_muonIdHelperTool->cscIdHelper().show_to_string(channelId)
                          << " Skipping channel " );
        continue;
      } else if (!descriptor->containsId(channelId)) {
        ATH_MSG_WARNING ("Identifier from the cabling service <"
                         <<m_muonIdHelperTool->cscIdHelper().show_to_string(channelId)
                         <<"> inconsistent with the geometry of detector element <"
                         <<m_muonIdHelperTool->cscIdHelper().show_to_string(descriptor->identify())
                         <<">  =>>ignore this hit");
        continue;
      }

      // if(!(m_muonIdHelperTool->cscIdHelper().valid(channelId))) {
      //   ATH_MSG_WARNING ( "CscRdoToCscPrepDataToolCore::decode Invalid CSC Identifier " << channelId
      //                     << " most likely due to old RDO sample generated in 15.X");
      //   continue;
      // }
      
      float timeOfFirstSample = 0.0;
      bool extractSamples = data->samples(j, numSamples, samples);
      if (!extractSamples) {
        ATH_MSG_WARNING ( "Unable to extract samples for strip " << j 
                          << " Online Cluster width = " << width 
                          << " for number of Samples = " << numSamples 
                          << " continuing ..." ); 
        continue;
      }	  
      
      //      uint32_t stripHash = hashOffset+j;
      IdentifierHash stripHash;
      if (m_muonIdHelperTool->cscIdHelper().get_channel_hash(channelId, stripHash)) {
        ATH_MSG_WARNING ( "Unable to get CSC strip hash id");
        channelId.show();
      }
      
      bool adctocharge = m_cscCalibTool->adcToCharge(samples, stripHash, charges);
      if ( !adctocharge ) {
        ATH_MSG_WARNING ( " CSC conversion ADC to Charge failed " 
                          << "CSC PrepData not build ... skipping " );
        continue;
      }
      if ( samples.size() >=4 ) 
        ATH_MSG_DEBUG ( "ADC: " << m_muonIdHelperTool->cscIdHelper().show_to_string(channelId) 
                        << " " << samples[0] << " " << samples[1] << " " << samples[2] << " " << samples[3]
                        << " Charges: "  
                        << " " << charges[0] << " " << charges[1] << " " << charges[2] << " " << charges[3] ); 
      
      int measuresPhi    = m_muonIdHelperTool->cscIdHelper().measuresPhi(channelId);

      /* moving to manage these local (tracking wise) strip position in CscReadoutElement 
	 HepGeom::Point3D<double> globalStripPosition = descriptor->stripPos(channelId); // position on strip planes
      
	 // get local position on wire plane, here we have to use a tolarance as the wire plane is located 2.5 mm 
	 // from the strip plane
	 double tolerance = 3.;
	 const Trk::LocalPosition * localWirePos = descriptor->surface(channelId).globalToLocal(globalStripPosition,tolerance);
      
	 // the globalToLocal should not fail with the 3 mm tolerance, if it does produce a WARNING
	 if( !localWirePos ){
	 ATH_MSG_WARNING ( " globalToLocal failed!!!, trying simple transform " );
	 HepGeom::Point3D<double> localStripPos       = descriptor->surface(channelId).transform().inverse()*globalStripPosition;
	 Trk::LocalPosition *lp = new Trk::LocalPosition();
	 (*lp)[Trk::locX] = localStripPos.x();
	 (*lp)[Trk::locY] = localStripPos.y();
	 ATH_MSG_WARNING ( " local position in measurement frame (" <<  localStripPos.x() 
	 << "," << localStripPos.y()  << "," << localStripPos.z() << ") " );
	 localWirePos = lp;
	 }
	 if (measuresPhi==1){
	 if (descriptor->getStationEta()>0) ATH_MSG_INFO( "PHI-side A current ... local position:       " << localWirePos->x() << " " << localWirePos->y());
	 else                               ATH_MSG_INFO( "PHI-side C current ... local position:       " << localWirePos->x() << " " << localWirePos->y());
	 }
	 else {
	 if (descriptor->getStationEta()>0) ATH_MSG_INFO( "ETA-side A current ... local position:       " << localWirePos->x() << " " << localWirePos->y());
	 else                               ATH_MSG_INFO( "ETA-side C current ... local position:       " << localWirePos->x() << " " << localWirePos->y());
	 }
      */
      // Amg::Vector3D localStripPosNew = descriptor->stripPosOnTrackingSurface(channelId);
      // Amg::Vector2D localWirePos1(localStripPosNew.x(), localStripPosNew.y());
      // double locX = descriptor->xCoordinateInTrackingFrame(channelId );
      // if( fabs(locX-localWirePos1.x()) > 1e-6 || fabs(localWirePos1.y()) > 1e-6 ){
      //   ATH_MSG_WARNING(" bad loc pos: fast " << locX << " slow " << localWirePos1.x() << " " << localWirePos1.y());
      // }else ATH_MSG_INFO(" loc pos: fast " << locX << " slow " << localWirePos1.x() << " " << localWirePos1.y());

      Amg::Vector2D localWirePos1( descriptor->xCoordinateInTrackingFrame(channelId ),0.);
     // if (measuresPhi==1){
      //   if (descriptor->getStationEta()>0) ATH_MSG_DEBUG( "PHI-side A new     ... local position:       " << localWirePos1.x() << " " << localWirePos1.y());
      //   else                               ATH_MSG_DEBUG( "PHI-side C new     ... local position:       " << localWirePos1.x() << " " << localWirePos1.y());
      // }
      // else {
      //   if (descriptor->getStationEta()>0) ATH_MSG_DEBUG( "ETA-side A new     ... local position:       " << localWirePos1.x() << " " << localWirePos1.y());
      //   else                               ATH_MSG_DEBUG( "ETA-side C new     ... local position:       " << localWirePos1.x() << " " << localWirePos1.y());
      // }
      /* the following is the way local cluster positions are computed in the clusterization; the reason for doing that is that 17/06/2011 
	 there are no methods like clusterPosOnTrackingSurface() yet in CscReadoutElement
	 HepGeom::Point3D<double> localStripPosN = descriptor->nominalLocalStripPos(channelId);
	 Trk::LocalPosition *localWirePos2;
	 if (measuresPhi==1) localWirePos2 = new Trk::LocalPosition(localStripPosN.y(), localStripPosN.z());
	 else                localWirePos2 = new Trk::LocalPosition(localStripPosN.z(), localStripPosN.y());
	 if (measuresPhi==1){
	 if (descriptor->getStationEta()>0) ATH_MSG_INFO( "PHI-side A as-clus ... local position:       " << localWirePos2->x() << " " << localWirePos2->y());
	 else                               ATH_MSG_INFO( "PHI-side C as-clus ... local position:       " << localWirePos2->x() << " " << localWirePos2->y());
	 }
	 else {
	 if (descriptor->getStationEta()>0) ATH_MSG_INFO( "ETA-side A as-clus ... local position:       " << localWirePos2->x() << " " << localWirePos2->y());
	 else                               ATH_MSG_INFO( "ETA-side C as-clus ... local position:       " << localWirePos2->x() << " " << localWirePos2->y());
	 }
      
	 const GlobalPosition* globalWirePos =  descriptor->surface(channelId).localToGlobal( *localWirePos );
	 if (!(descriptor->surface(channelId).isOnSurface( *globalWirePos,true ) ) ) 
	 ATH_MSG_WARNING ("Measurement at ("<<localWirePos->x()<<","<< localWirePos->y()
	 <<") does not appear to be on wire plane surface! (which has bounds"
	 <<descriptor->surface().bounds()<<")"); //sanity check
	 delete globalWirePos;
      */
      
      int chamberLayer   = m_muonIdHelperTool->cscIdHelper().chamberLayer(channelId);
      float stripWidth   = descriptor->cathodeReadoutPitch( chamberLayer, measuresPhi );
      double errPos      = stripWidth / sqrt(12.0);

      AmgSymMatrix(2) covariance;
      covariance.setIdentity();
      covariance *= errPos*errPos;
      Amg::MatrixX * errClusterPos = new Amg::MatrixX(covariance);

      /** new CscStripPrepRawData */
      CscStripPrepData * newPrepData = new CscStripPrepData(channelId,
                                                            cscHashId,
                                                            localWirePos1,
                                                            errClusterPos,
                                                            descriptor,
                                                            charges,
                                                            timeOfFirstSample,
                                                            samplingTime);
      
      if (samplingPhase) newPrepData->set_samplingPhase();
      newPrepData->setHashAndIndex(collection->identifyHash(), collection->size());
      collection->push_back(newPrepData);
      
    }
  }

  return StatusCode::SUCCESS;
}



//************** Process for all in case of Offline
StatusCode CscRdoToCscPrepDataToolCore::decode(const CscRawDataContainer* rdoContainer, std::vector<IdentifierHash>& decodedIdhs)
{
  
  typedef CscRawDataContainer::const_iterator collection_iterator;
  
  IdContext cscContext = m_muonIdHelperTool->cscIdHelper().module_context();
 
  // if CSC decoding is switched off stop here
  if( !m_decodeData ) {
    ATH_MSG_DEBUG ( "Stored empty container. "
                    << "Decoding CSC RDO into CSC PrepRawData is switched off" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG ( "Decoding CSC RDO into CSC PrepRawData" );

  collection_iterator rdoColl     = rdoContainer->begin(); 
  collection_iterator lastRdoColl = rdoContainer->end();
  std::vector<float> charges;
  charges.reserve(4);
  std::vector<uint16_t> samples;
  samples.reserve(4);

  Identifier oldId;
  CscStripPrepDataCollection * collection = 0;
  IdentifierHash cscHashId;
  for (; rdoColl!=lastRdoColl; ++rdoColl) {
    if ( (*rdoColl)->size() > 0 ) {
      ATH_MSG_DEBUG ( " Number of RawData in this rdo " << (*rdoColl)->size() );
      
      const  CscRawDataCollection* cscCollection = *rdoColl; 
      unsigned int samplingTime = cscCollection->rate();
      unsigned int numSamples   = cscCollection->numSamples();
      bool samplingPhase        = cscCollection->samplingPhase();

      if ( int(samplingTime) != int(m_cscCalibTool->getSamplingTime())) {
        ATH_MSG_WARNING ( " CSC sampling time from Collection is NOT consistant to calibTool parameter!!!!!!! " );
      }
      /** for each Rdo, loop over RawData, converter RawData to PrepRawData
          retrieve/create PrepRawData collection, and insert PrepRawData into collection */
      CscRawDataCollection::const_iterator itD   =  cscCollection->begin(); 
      CscRawDataCollection::const_iterator itD_e =  cscCollection->end();
      
      for (; itD!=itD_e; ++itD) { 
        const CscRawData * data = (*itD);
        uint16_t width = data->width();
        uint16_t totalSamples = (data->samples()).size();
        
        Identifier stationId = m_cscRdoDecoderTool->stationIdentifier(data);
        uint32_t hashOffset = data->hashId();

        ATH_MSG_DEBUG ( "DecodeAll*Size of online cluster in this RawData: " 
			<< " Width = " << width << " Samples = " << totalSamples
			<< " stationId : " << stationId << "  hashOffset : " << hashOffset);

	if (m_muonIdHelperTool->cscIdHelper().get_hash(stationId, cscHashId, &cscContext)) {
	  ATH_MSG_WARNING ( "Unable to get CSC digiti collection hash id "
			    << "context begin_index = " << cscContext.begin_index()
			    << " context end_index  = " << cscContext.end_index()
			    << " the identifier is "     );
	  stationId.show();
	}

	if (oldId != stationId) {
	  Muon::CscStripPrepDataContainer::const_iterator it_coll = m_outputCollection->indexFind(cscHashId);
	  if (m_outputCollection->end() == it_coll) {
	    CscStripPrepDataCollection * newCollection = new CscStripPrepDataCollection(cscHashId);
	    newCollection->setIdentifier(stationId);
	    collection = newCollection;
	    if ( m_outputCollection->addCollection(newCollection, cscHashId).isFailure() )
	      ATH_MSG_WARNING( "Couldn't record CscStripPrepdataCollection with key=" << (unsigned int) cscHashId
			       << " in StoreGate!" );
	    decodedIdhs.push_back(cscHashId); //Record that this collection contains data
	    
	  } else {  
//Hack for transition to athenaMT classes
            CscStripPrepDataCollection * oldCollection = const_cast<CscStripPrepDataCollection*>( *it_coll );

	    collection = oldCollection;
	    cscHashId = collection->identifyHash();
	    
	  }
	  oldId = stationId;
	}

        
        for (unsigned int j=0; j<width; ++j) {
          const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(data, j);
          ATH_MSG_DEBUG ( "DecodeAll**LOOP over width  " << j <<  " " << channelId );
          // if(!(m_muonIdHelperTool->cscIdHelper().valid(channelId))) {
          //   ATH_MSG_WARNING ( "CscRdoToCscPrepDataToolCore::decode Invalid CSC Identifier " << channelId
          //                     << " most likely due to old RDO sample generated in 15.X");
          //   continue;
          // }

          const CscReadoutElement * descriptor = m_muonMgr->getCscReadoutElement(channelId);
          //calculate local positions on the strip planes
          if ( !descriptor ) {
            ATH_MSG_WARNING ( "Invalid descriptor for " << m_muonIdHelperTool->cscIdHelper().show_to_string(channelId)
                              << " Skipping channel " );
            continue;
          }
          else if (!descriptor->containsId(channelId)) {
            ATH_MSG_WARNING ("Identifier from the cabling service <"
                             <<m_muonIdHelperTool->cscIdHelper().show_to_string(channelId)
                             <<"> inconsistent with the geometry of detector element <"
                             <<m_muonIdHelperTool->cscIdHelper().show_to_string(descriptor->identify())
                             <<">  =>>ignore this hit");
            continue;
          }
          
          float timeOfFirstSample = 0.0;
          bool extractSamples = data->samples(j, numSamples, samples);
          if (!extractSamples) {
            ATH_MSG_WARNING ( "Unable to extract samples for strip " << j 
                              << " Online Cluster width = " << width 
                              << " for number of Samples = " << numSamples 
                              << " continuing ..." ); 
            continue;
          }	 
          

          //      uint32_t stripHash = hashOffset+j;
          IdentifierHash stripHash;
          if (m_muonIdHelperTool->cscIdHelper().get_channel_hash(channelId, stripHash)) {
            ATH_MSG_WARNING ( "Unable to get CSC strip hash id");
            channelId.show();
          }


          Identifier channelIdFromHash;
          m_muonIdHelperTool->cscIdHelper().get_id(stripHash, channelIdFromHash, &cscContext);
          
          
          bool adctocharge = m_cscCalibTool->adcToCharge(samples, stripHash, charges);
          if ( !adctocharge ) {
            ATH_MSG_WARNING ( " CSC conversion ADC to Charge failed " 
                              << "CSC PrepData not build ... skipping " );
            continue;
          }
          if ( samples.size() >=4 ) 
            ATH_MSG_DEBUG ( "DecodeAll*** ADC: " << m_muonIdHelperTool->cscIdHelper().show_to_string(channelId) << " "
                            << (int) stripHash << " "
                            << m_muonIdHelperTool->cscIdHelper().show_to_string(channelIdFromHash) 
                            << " " << samples[0] << " " << samples[1] << " " << samples[2] << " " << samples[3]
                            << " Charges: "  
                            << " " << charges[0] << " " << charges[1] << " " << charges[2] << " " << charges[3] ); 
          if (m_muonIdHelperTool->cscIdHelper().get_hash(stationId, cscHashId, &cscContext)) {
            ATH_MSG_WARNING ( "Unable to get CSC hash id from CSC RDO collection "
                              << "context begin_index = " << cscContext.begin_index()
                              << " context end_index  = " << cscContext.end_index() 
                              << " the identifier is "      );
            stationId.show();
          }
                    
          // Check if this strip is already decoded.. Then we don't have to decode it again
          bool IsThisStripDecoded =0; 
          for ( CscStripPrepDataCollection::const_iterator idig=collection->begin();
                idig!=collection->end(); ++idig ) {
            const CscStripPrepData& dig = **idig;
            Identifier did = dig.identify();
            if (did ==channelId) {
              IsThisStripDecoded =1;
              break;
            }
          }
          if (IsThisStripDecoded) continue;

	  int measuresPhi    = m_muonIdHelperTool->cscIdHelper().measuresPhi(channelId);
	  /* moving to manage these local (tracking wise) strip position in CscReadoutElement 
	     HepGeom::Point3D<double> globalStripPosition = descriptor->stripPos(channelId); // position on strip planes
          
	     // get local position on wire plane, here we have to use a tolarance as the wire plane is located 2.5 mm 
	     // from the strip plane
	     double tolerance = 3.;
	     const Trk::LocalPosition * localWirePos = descriptor->surface(channelId).globalToLocal(globalStripPosition,tolerance);
          
	     // the globalToLocal should not fail with the 3 mm tolerance, if it does produce a WARNING
	     if( !localWirePos ){
	     ATH_MSG_WARNING ( " globalToLocal failed!!!, trying simple transform " );
	     HepGeom::Point3D<double> localStripPos       = descriptor->surface(channelId).transform().inverse()*globalStripPosition;
	     Trk::LocalPosition *lp = new Trk::LocalPosition();
	     (*lp)[Trk::locX] = localStripPos.x();
	     (*lp)[Trk::locY] = localStripPos.y();
	     ATH_MSG_WARNING ( " local position in measurement frame (" <<  localStripPos.x() 
	     << "," << localStripPos.y()  << "," << localStripPos.z() << ") " );
	     localWirePos = lp;
	     }
	     if (measuresPhi==1){
	     if (descriptor->getStationEta()>0) ATH_MSG_INFO( "PHI-side A current ... local position:       " << localWirePos->x() << " " << localWirePos->y());
	     else                               ATH_MSG_INFO( "PHI-side C current ... local position:       " << localWirePos->x() << " " << localWirePos->y());
	     }
	     else {
	     if (descriptor->getStationEta()>0) ATH_MSG_INFO( "ETA-side A current ... local position:       " << localWirePos->x() << " " << localWirePos->y());
	     else                               ATH_MSG_INFO( "ETA-side C current ... local position:       " << localWirePos->x() << " " << localWirePos->y());
	     }
	  */
	  // Amg::Vector3D localStripPosNew = descriptor->stripPosOnTrackingSurface(channelId);
	  // Amg::Vector2D localWirePos1(localStripPosNew.x(), localStripPosNew.y());
          // double locX =;
          // if( fabs(locX-localWirePos1.x()) > 1e-6 || fabs(localWirePos1.y()) > 1e-6 ){
          //   ATH_MSG_WARNING( m_muonIdHelperTool->cscIdHelper().print_to_string(channelId) << " bad loc pos: fast " << locX << " slow " << localWirePos1.x() << " " << localWirePos1.y());
          // }else ATH_MSG_INFO(m_muonIdHelperTool->cscIdHelper().print_to_string(channelId) << " loc pos: fast " << locX << " slow " << localWirePos1.x() << " " << localWirePos1.y());
          Amg::Vector2D localWirePos1( descriptor->xCoordinateInTrackingFrame(channelId ),0.);
	  // if (measuresPhi==1){
	  //   if (descriptor->getStationEta()>0) ATH_MSG_DEBUG( "PHI-side A new     ... local position:       " << localWirePos1.x() << " " << localWirePos1.y());
	  //   else                               ATH_MSG_DEBUG( "PHI-side C new     ... local position:       " << localWirePos1.x() << " " << localWirePos1.y());
	  // }
	  // else {
	  //   if (descriptor->getStationEta()>0) ATH_MSG_DEBUG( "ETA-side A new     ... local position:       " << localWirePos1.x() << " " << localWirePos1.y());
	  //   else                               ATH_MSG_DEBUG( "ETA-side C new     ... local position:       " << localWirePos1.x() << " " << localWirePos1.y());
	  // }
	  /*  the following is the way local cluster positions are computed in the clusterization; the reason for doing that is that 17/06/2011 
	      there are no methods like clusterPosOnTrackingSurface() yet in CscReadoutElement
	      HepGeom::Point3D<double> localStripPosN = descriptor->nominalLocalStripPos(channelId);
	      Trk::LocalPosition *localWirePos2;
	      if (measuresPhi==1) localWirePos2 = new Trk::LocalPosition(localStripPosN.y(), localStripPosN.z());
	      else                localWirePos2 = new Trk::LocalPosition(localStripPosN.z(), localStripPosN.y());
	      if (measuresPhi==1){
	      if (descriptor->getStationEta()>0) ATH_MSG_INFO( "PHI-side A as-clus ... local position:       " << localWirePos2->x() << " " << localWirePos2->y());
	      else                               ATH_MSG_INFO( "PHI-side C as-clus ... local position:       " << localWirePos2->x() << " " << localWirePos2->y());
	      }
	      else {
	      if (descriptor->getStationEta()>0) ATH_MSG_INFO( "ETA-side A as-clus ... local position:       " << localWirePos2->x() << " " << localWirePos2->y());
	      else                               ATH_MSG_INFO( "ETA-side C as-clus ... local position:       " << localWirePos2->x() << " " << localWirePos2->y());
	      }
          
	      const GlobalPosition* globalWirePos =  descriptor->surface(channelId).localToGlobal( *localWirePos );
	      if (!(descriptor->surface(channelId).isOnSurface( *globalWirePos,true ) ) ) 
	      ATH_MSG_WARNING ("Measurement at ("<<localWirePos->x()<<","<< localWirePos->y()
	      <<") does not appear to be on wire plane surface! (which has bounds"
	      <<descriptor->surface().bounds()<<")"); //sanity check
	      delete globalWirePos;
	  */
 
          int chamberLayer   = m_muonIdHelperTool->cscIdHelper().chamberLayer(channelId);
          float stripWidth   = descriptor->cathodeReadoutPitch( chamberLayer, measuresPhi );
          double errPos      = stripWidth / sqrt(12.0);
          
	  AmgSymMatrix(2) covariance;
	  covariance.setIdentity();
	  covariance *= errPos*errPos;
	  Amg::MatrixX * errClusterPos = new Amg::MatrixX(covariance);

          /** new CscPrepRawData */
          CscStripPrepData * newPrepData = new CscStripPrepData(  channelId,
                                                                  cscHashId,
                                                                  localWirePos1,
                                                                  errClusterPos,
                                                                  descriptor,
                                                                  charges,
                                                                  timeOfFirstSample,
                                                                  samplingTime);
          
          if (samplingPhase) newPrepData->set_samplingPhase();
          newPrepData->setHashAndIndex(collection->identifyHash(), collection->size());
          collection->push_back(newPrepData);
          
        }
      }
    }
  }
  return  StatusCode::SUCCESS;
}


void CscRdoToCscPrepDataToolCore::printPrepData()
{
  ATH_MSG_INFO ( "***************************************************************" );
  ATH_MSG_INFO ( "****** Listing Csc(Strip)PrepData collections content *********" );

  if (m_outputCollection->size() <= 0)
    ATH_MSG_INFO ( "No Csc(Strip)PrepRawData collections found" );
  else {
    ATH_MSG_INFO ( "Number of Csc(Strip)PrepRawData collections found in this event is "
		   << m_outputCollection->size() );
    
    int ict = 0;
    int ncoll = 0;
    ATH_MSG_INFO ( "-------------------------------------------------------------" );
    for (IdentifiableContainer<Muon::CscStripPrepDataCollection>::const_iterator
	   icscColl = m_outputCollection->begin();
         icscColl!=m_outputCollection->end(); ++icscColl ) {

      const Muon::CscStripPrepDataCollection* cscColl = *icscColl;
        
      if ( cscColl->size() <= 0 ) continue;

      ATH_MSG_INFO ("PrepData Collection ID " << m_muonIdHelperTool->cscIdHelper().show_to_string(cscColl->identify())
                    <<" with size = "<<cscColl->size());
      CscStripPrepDataCollection::const_iterator it_cscStripPrepData;
      int icc = 0;
      int iccphi = 0;
      int icceta = 0;
      for (it_cscStripPrepData=cscColl->begin(); it_cscStripPrepData != cscColl->end(); ++it_cscStripPrepData) {
        icc++;
        ict++;
        if (m_muonIdHelperTool->cscIdHelper().measuresPhi((*it_cscStripPrepData)->identify()))
          iccphi++;
        else
          icceta++;
        
        ATH_MSG_INFO ( ict <<" in this coll. " << icc << " prepData id = "
                       << m_muonIdHelperTool->cscIdHelper().show_to_string((*it_cscStripPrepData)->identify()) );
      }
      ncoll++;
      ATH_MSG_INFO ( "*** Collection " << ncoll << " Summary: "
                     << iccphi <<" phi hits / " << icceta << " eta hits " );
      ATH_MSG_INFO ( "-------------------------------------------------------------" );

    }
  }
}  


void CscRdoToCscPrepDataToolCore::printInputRdo() {
  return;
}
  
