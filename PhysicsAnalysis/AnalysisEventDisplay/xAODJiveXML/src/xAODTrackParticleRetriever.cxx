/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJiveXML/xAODTrackParticleRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODTracking/TrackParticleContainer.h" 
//#include "TrkTrackSummary/TrackSummary.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   *
   * code reference for xAOD:     jpt6Feb14
   *  https://svnweb.cern.ch/trac/atlasgroups/browser/PAT/AODUpgrade/xAODReaderAlgs
   * classes:
   *  https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD
   *
   **/
  xAODTrackParticleRetriever::xAODTrackParticleRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent), m_typeName("Track"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    //In xAOD: GSFTrackParticles, CombinedFitMuonParticles, InDetTrackParticlesForward, ExtrapolatedMuonSpectrometerParticles
    declareProperty("StoreGateKey", m_sgKey = "InDetTrackParticles", 
        "Collection to be first in output, shown in Atlantis without switching");
    declareProperty("PriorityTrackCollection", m_sgKey = "TrackParticleCandidate", 
        "Track collections to retrieve first, shown as default in Atlantis");
    declareProperty("OtherTrackCollections"   , m_otherKeys , "Track collections to retrieve, all if empty");
    declareProperty("DoWriteHLT"              , m_doWriteHLT = false, "Wether to write HLTAutoKey objects");
  }
  
  /**
   * For each TrackParticle collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode xAODTrackParticleRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endmsg;
    
    SG::ConstIterator<xAOD::TrackParticleContainer> iterator, end;
    const xAOD::TrackParticleContainer* TrackParticles;
    
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endmsg;
    StatusCode sc = evtStore()->retrieve(TrackParticles, m_sgKey);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endmsg; 
    }else{
      DataMap data = getData(TrackParticles);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey+"_xAOD", &data).isFailure()){
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endmsg;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") TrackParticle retrieved" << endmsg;
      }
    }

    if ( m_otherKeys.empty() ) {
      //obtain all other collections from StoreGate
      if (( evtStore()->retrieve(iterator, end)).isFailure()){
         if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Unable to retrieve iterator for Jet collection" << endmsg;
//        return false;
      }
      
      for (; iterator!=end; iterator++) {
	  if (iterator.key()!=m_sgKey) {
       	     if ((iterator.key().find("HLT",0) != std::string::npos) && (!m_doWriteHLT)){
	          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ignoring HLT-AutoKey collection " << iterator.key() << endmsg;
	         continue;  }
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve all. Current collection: " << dataTypeName() << " (" << iterator.key() << ")" << endmsg;
             DataMap data = getData(&(*iterator));
	     //Check if this is an HLT-AutoKey collection
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key()+"_xAOD", &data).isFailure()){
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endmsg;
	    }else{
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") AODJet retrieved" << endmsg;
	    }
	}
      }
    }else {
      //obtain all collections with the given keys
      std::vector<std::string>::const_iterator keyIter;
      for ( keyIter=m_otherKeys.begin(); keyIter!=m_otherKeys.end(); ++keyIter ){
        if ( !evtStore()->contains<xAOD::TrackParticleContainer>( (*keyIter) ) ){ continue; } // skip if not in SG
	StatusCode sc = evtStore()->retrieve( TrackParticles, (*keyIter) );
	if (!sc.isFailure()) {
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve selected " << dataTypeName() << " (" << (*keyIter) << ")" << endmsg;
          DataMap data = getData(TrackParticles);
          if ( FormatTool->AddToEvent(dataTypeName(), (*keyIter)+"_xAOD", &data).isFailure()){
	    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << (*keyIter) << " not found in SG " << endmsg;
	  }else{
	     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << (*keyIter) << ") retrieved" << endmsg;
	  }
	}
      }
    } 
    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve basic parameters, mainly four-vectors, for each collection.
   * Also association with clusters and tracks (ElementLink).
   */
  const DataMap xAODTrackParticleRetriever::getData(const xAOD::TrackParticleContainer* TrackParticleCont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endmsg;

    DataMap DataMap;

    DataVect d0; d0.reserve(TrackParticleCont->size());
    DataVect z0; z0.reserve(TrackParticleCont->size());
    DataVect pt; pt.reserve(TrackParticleCont->size());
    DataVect phi0; phi0.reserve(TrackParticleCont->size());
    DataVect cotTheta; cotTheta.reserve(TrackParticleCont->size());
    DataVect label; label.reserve(TrackParticleCont->size());
    DataVect nBLayerHits; nBLayerHits.reserve(TrackParticleCont->size());
    DataVect nPixHits; nPixHits.reserve(TrackParticleCont->size());
    DataVect nSCTHits; nSCTHits.reserve(TrackParticleCont->size());
    DataVect nTRTHits; nTRTHits.reserve(TrackParticleCont->size());

    xAOD::TrackParticleContainer::const_iterator TrackParticleItr  = TrackParticleCont->begin();
    xAOD::TrackParticleContainer::const_iterator TrackParticleItrE = TrackParticleCont->end();

    int counter = 0;
    float charge = 0.;
    float myQOverP = 0.;
    double countHits = 0.;
    std::string labelStr = "unknownHits";

    for (; TrackParticleItr != TrackParticleItrE; ++TrackParticleItr) {

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << " TrackParticle #" << counter++ << " : d0 = "  << (*TrackParticleItr)->d0() << ", z0 = " 
                        << (*TrackParticleItr)->z0() << ", pt[GeV] = "  << (*TrackParticleItr)->pt()*(1./CLHEP::GeV)  
          << ", phi = "  << (*TrackParticleItr)->phi() 
          << ", qOverP = "  << (*TrackParticleItr)->qOverP() 
          << ", abs(qOverP) = "  << fabs((*TrackParticleItr)->qOverP()) 
	  << endmsg;
    }

    // Event/xAOD/xAODTrackingCnv/trunk/src/TrackParticleCnvAlg.cxx#L190 Info from Nick Styles

    uint8_t numberOfBLayerHits=0;
    uint8_t numberOfBLayerHits_tmp=0;
    if ( (*TrackParticleItr)->summaryValue(numberOfBLayerHits_tmp,xAOD::numberOfBLayerHits) ){
      numberOfBLayerHits += numberOfBLayerHits_tmp;
    }
    uint8_t numberOfPixelHits = 0;
    uint8_t numberOfPixelHits_tmp = 0;
    if ( (*TrackParticleItr)->summaryValue(numberOfPixelHits_tmp,xAOD::numberOfPixelHits)){
      numberOfPixelHits += numberOfPixelHits_tmp;
    }
    uint8_t numberOfTRTHits = 0;
    uint8_t numberOfTRTHits_tmp = 0;
    if ( (*TrackParticleItr)->summaryValue(numberOfTRTHits_tmp,xAOD::numberOfTRTHits)){
      numberOfTRTHits += numberOfTRTHits_tmp;     
    }
    uint8_t numberOfSCTHits = 0;
    uint8_t numberOfSCTHits_tmp = 0;
    if ( (*TrackParticleItr)->summaryValue(numberOfSCTHits_tmp,xAOD::numberOfSCTHits)){ 
      numberOfSCTHits += numberOfSCTHits_tmp;
    }
    labelStr = "_PixelHits"+DataType( (double)numberOfPixelHits ).toString() 
         + "_SCTHits"+DataType( (double)numberOfSCTHits ).toString() 
         + "_BLayerHits"+DataType( (double)numberOfBLayerHits ).toString() 
         + "_TRTHits"+DataType( (double)numberOfTRTHits ).toString() ;
 
    countHits = (double)numberOfBLayerHits + (double)numberOfPixelHits
      + (double)numberOfSCTHits + (double)numberOfTRTHits;

    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " TrackParticle #" << counter 
         << " BLayer hits: " << (double)numberOfBLayerHits
         << ", Pixel hits: " << (double)numberOfPixelHits
         << ", SCT hits: " << (double)numberOfSCTHits
	 << ", TRT hits: " << (double)numberOfTRTHits 
         << ", Total hits: " << countHits
         << ";  Label: " << labelStr << endmsg;

      nBLayerHits.push_back( DataType( (double)numberOfBLayerHits ));
      nPixHits.push_back( DataType( (double)numberOfPixelHits ));
      nSCTHits.push_back( DataType( (double)numberOfSCTHits ));
      nTRTHits.push_back( DataType( (double)numberOfTRTHits ));

      label.push_back( DataType( labelStr ).toString() );
      d0.push_back(DataType((*TrackParticleItr)->d0()/CLHEP::cm));
      z0.push_back(DataType((*TrackParticleItr)->z0()/CLHEP::cm));
      phi0.push_back(DataType((*TrackParticleItr)->phi()));

      // pt is always positive, get charge from qOverP
      myQOverP = (*TrackParticleItr)->qOverP() ;
      if (fabs(myQOverP) != myQOverP){  
         charge = -1.;
      }else{
         charge = +1.;
      }
      pt.push_back(DataType(( charge*(*TrackParticleItr)->pt() )/CLHEP::GeV));

     if ( (*TrackParticleItr)->theta()  == 0.) {
         cotTheta.push_back(DataType(9999.));
     } else {
         cotTheta.push_back(DataType(1./tan((*TrackParticleItr)->theta())));
     }
    } // end TrackParticleIterator 

    // four-vectors
    DataMap["d0"] = d0;
    DataMap["z0"] = z0;
    DataMap["pt"] = pt;
    DataMap["phi0"] = phi0;
    DataMap["cotTheta"] = cotTheta;
    DataMap["label"] = label;
    DataMap["nBLayerHits"] = nBLayerHits;
    DataMap["nPixHits"] = nPixHits;
    DataMap["nSCTHits"] = nSCTHits;
    DataMap["nTRTHits"] = nTRTHits;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << d0.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
