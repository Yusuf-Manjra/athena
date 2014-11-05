/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJiveXML/xAODElectronRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "xAODEgamma/ElectronContainer.h" 

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   *
   * code reference for xAOD:     jpt6Feb14
   *  https://svnweb.cern.ch/trac/atlasgroups/browser/PAT/AODUpgrade/xAODReaderAlgs
   *
   * This is afirst 'skeleton' try for many xAOD retrievers to be done:
   *  xAOD::Electron, xAOD::Vertex, xAOD::Photon, xAOD::CaloCluster, xAOD::Jet
   *  xAOD::TrackParticle, xAOD::TauJet, xAOD::Muon
   *
   **/
  xAODElectronRetriever::xAODElectronRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent), typeName("Electron"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    m_sgKey = "ElectronCollection"; // is xAOD name
    declareProperty("StoreGateKey", m_sgKey, 
        "Collection to be first in output, shown in Atlantis without switching");
  }
  
  /**
   * For each jet collections retrieve basic parameters.
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode xAODElectronRetriever::retrieve(ToolHandle<IFormatTool> FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieveAll()" << endreq;
    
    const DataHandle<xAOD::ElectronContainer> iterator, end;
    const xAOD::ElectronContainer* electrons;
    
    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKey << ")" << endreq;
    StatusCode sc = evtStore()->retrieve(electrons, m_sgKey);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq; 
    }else{
      DataMap data = getData(electrons);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey+"_xAOD", &data).isFailure()){ //suffix can be removed later
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKey << " not found in SG " << endreq;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKey << ") Electron retrieved" << endreq;
      }
    }
 
    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve basic parameters, mainly four-vectors, for each collection.
   * Also association with clusters and tracks (ElementLink).
   */
  const DataMap xAODElectronRetriever::getData(const xAOD::ElectronContainer* elCont) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in getData()" << endreq;

    DataMap m_DataMap;

    DataVect pt; pt.reserve(elCont->size());
    DataVect phi; phi.reserve(elCont->size());
    DataVect eta; eta.reserve(elCont->size());
    DataVect mass; mass.reserve(elCont->size());
    DataVect energy; energy.reserve(elCont->size());
    DataVect pdgId; energy.reserve(elCont->size());

    DataVect isEMString; isEMString.reserve(elCont->size());
    DataVect author; author.reserve(elCont->size());
    DataVect label; label.reserve(elCont->size());

    xAOD::ElectronContainer::const_iterator elItr  = elCont->begin();
    xAOD::ElectronContainer::const_iterator elItrE = elCont->end();

    int counter = 0;

    for (; elItr != elItrE; ++elItr) {

    std::string electronAuthor = "";
    std::string electronIsEMString = "none";
    std::string electronLabel = "";

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "  Electron #" << counter++ << " : eta = "  << (*elItr)->eta() << ", phi = " 
          << (*elItr)->phi() 
	// << ", ntrk = " << (*elItr)->getNumberOfTrackParticles() 
          << ", author = " << (*elItr)->author() 
//	  << ", isEM/Tight: " << (*elItr)->passSelection(passesTight, "Tight")
		      << ", charge = " << (*elItr)->trackParticle()->charge() 
		      << ", pdgId = " << -11.*(*elItr)->trackParticle()->charge()
          << endreq;
    }

      phi.push_back(DataType((*elItr)->phi()));
      eta.push_back(DataType((*elItr)->eta()));
      pt.push_back(DataType((*elItr)->pt()/CLHEP::GeV));

      mass.push_back(DataType((*elItr)->m()/CLHEP::GeV));
      energy.push_back( DataType((*elItr)->e()/CLHEP::GeV ) );
      pdgId.push_back(DataType( -11.*(*elItr)->trackParticle()->charge() )); // pdgId not available anymore in xAOD

      bool passesTight(false);
      bool passesMedium(false);
      bool passesLoose(false);
      const bool tightSelectionExists = (*elItr)->passSelection(passesTight, "Tight");
       msg(MSG::DEBUG) << "tight exists " << tightSelectionExists 
	 << " and passes? " << passesTight << endreq;
      const bool mediumSelectionExists = (*elItr)->passSelection(passesMedium, "Medium");
       msg(MSG::DEBUG) << "medium exists " << mediumSelectionExists 
	 << " and passes? " << passesMedium << endreq;
      const bool looseSelectionExists = (*elItr)->passSelection(passesLoose, "Loose");
       msg(MSG::DEBUG) << "loose exists " << looseSelectionExists 
	<< " and passes? " << passesLoose << endreq;

      electronAuthor = "author"+DataType( (*elItr)->author() ).toString(); // for odd ones eg FWD
      electronLabel = electronAuthor;
      if (( (*elItr)->author()) == 0){ electronAuthor = "unknown"; electronLabel += "_unknown"; }
      if (( (*elItr)->author()) == 8){ electronAuthor = "forward"; electronLabel += "_forward"; }
      if (( (*elItr)->author()) == 2){ electronAuthor = "softe"; electronLabel += "_softe"; }
      if (( (*elItr)->author()) == 1){ electronAuthor = "egamma"; electronLabel += "_egamma"; }

      if ( passesLoose ){  
            electronLabel += "_Loose"; 
            electronIsEMString = "Loose"; // assume that hierarchy is obeyed !
      } 
      if ( passesMedium ){ 
            electronLabel += "_Medium"; 
            electronIsEMString = "Medium"; // assume that hierarchy is obeyed !
      }   
      if ( passesTight ){ 
            electronLabel += "_Tight"; 
            electronIsEMString = "Tight"; // assume that hierarchy is obeyed !
      }     
      author.push_back( DataType( electronAuthor ) );
      label.push_back( DataType( electronLabel ) );
      isEMString.push_back( DataType( electronIsEMString ) );

    } // end ElectronIterator 

    // four-vectors
    m_DataMap["phi"] = phi;
    m_DataMap["eta"] = eta;
    m_DataMap["pt"] = pt;
    m_DataMap["energy"] = energy;
    m_DataMap["mass"] = mass;
    m_DataMap["pdgId"] = pdgId;
    m_DataMap["isEMString"] = isEMString;
    m_DataMap["label"] = label;
    m_DataMap["author"] = author;

    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endreq;
    }

    //All collections retrieved okay
    return m_DataMap;

  } // retrieve

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
