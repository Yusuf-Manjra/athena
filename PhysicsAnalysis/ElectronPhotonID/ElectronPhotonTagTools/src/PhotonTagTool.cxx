/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : PhotonTagTool.cxx
Package : offline/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : create a collection of PhotonTag

*****************************************************************************/

#include "GaudiKernel/Property.h"

#include "StoreGate/StoreGateSvc.h"

#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaEnums.h"

#include "ElectronPhotonTagTools/PhotonTagTool.h"
#include "TagEvent/PhotonAttributeNames.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <sstream>

//using namespace Analysis;

/** the constructor */
PhotonTagTool::PhotonTagTool (const std::string& type, const std::string& name, 
    const IInterface* parent) : 
  AthAlgTool( type, name, parent )
{
  /** Photon AOD Container Name */
  declareProperty("Container",     m_containerName = "PhotonCollection");

  /** selection cut of Pt */
  declareProperty("EtCut",         m_cut_Et = 7.0*CLHEP::GeV);

  /** Calo Isolation cut values */
  declareProperty("CaloIsoCutValues",m_caloisocutvalues, "Cut values for calo isolation");

  /** Calo Isolation cut values */
  declareProperty("TrackIsoCutValues",m_trackisocutvalues, "Cut values for track isolation");
  
  declareInterface<PhotonTagTool>( this );
}



/** initialization - called once at the begginning */
StatusCode  PhotonTagTool::initialize() {
  ATH_MSG_DEBUG( "in intialize()" );
 
  return StatusCode::SUCCESS;
}



/** build the attribute list - called in initialize */
StatusCode PhotonTagTool::attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap,
                                                 const int& max)
{
  ATH_MSG_DEBUG( "in attributeSpecification()" );

  /** Photon Attributes */
  attrMap[ PhotonAttributeNames[PhotonID::NPhoton] ]  = AthenaAttributeType("unsigned int", PhotonAttributeUnitNames[PhotonID::NPhoton], PhotonAttributeGroupNames[PhotonID::NPhoton]);
  attrMap[ PhotonAttributeNames[PhotonID::NConverted] ]  = AthenaAttributeType("unsigned int", PhotonAttributeUnitNames[PhotonID::NConverted], PhotonAttributeGroupNames[PhotonID::NConverted]);

  std::ostringstream os;
  for (int i=1; i<= max; ++i) {

    /** pt */
    os.str("");
    os << PhotonAttributeNames[PhotonID::Pt] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float",  PhotonAttributeUnitNames[PhotonID::Pt],  PhotonAttributeGroupNames[PhotonID::Pt]);
    m_ptStr.push_back( os.str() );

    /** eta */
    os.str("");
    os << PhotonAttributeNames[PhotonID::Eta] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float",  PhotonAttributeUnitNames[PhotonID::Eta],  PhotonAttributeGroupNames[PhotonID::Eta]);
    m_etaStr.push_back( os.str() );

    /** phi */
    os.str("");
    os << PhotonAttributeNames[PhotonID::Phi] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("float", PhotonAttributeUnitNames[PhotonID::Phi], PhotonAttributeGroupNames[PhotonID::Phi]);
    m_phiStr.push_back( os.str() );

    /** Tightness */
    os.str("");
    os << PhotonAttributeNames[PhotonID::Tight] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", PhotonAttributeUnitNames[PhotonID::Tight], PhotonAttributeGroupNames[PhotonID::Tight]);
    m_tightStr.push_back( os.str() );

    /** Isolation Photon */
    os.str("");
    os << PhotonAttributeNames[PhotonID::Isol] << std::dec << i;
    attrMap[ os.str() ] = AthenaAttributeType("unsigned int", PhotonAttributeUnitNames[PhotonID::Isol], PhotonAttributeGroupNames[PhotonID::Isol]) ;
    m_isoStr.push_back( os.str() );

  }

  return StatusCode::SUCCESS;
}




/** execute - called on every event */
StatusCode PhotonTagTool::execute(TagFragmentCollection& pTagColl, const int& max) {

  ATH_MSG_DEBUG( "in execute()" );

  /** retrieve the AOD photon container */
  const xAOD::PhotonContainer *photonContainer;
  StatusCode sc = evtStore()->retrieve( photonContainer, m_containerName);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "No AOD Photon container found in SG" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "AOD Photon container successfully retrieved" );

  xAOD::PhotonContainer userContainer( SG::VIEW_ELEMENTS );
  userContainer = *photonContainer;
  AnalysisUtils::Sort::pT( &userContainer );

  /** make the selection */
  int i=0;
  int nConverted = 0;
  xAOD::PhotonContainer::const_iterator photonItr  = userContainer.begin();
  xAOD::PhotonContainer::const_iterator photonItrE = userContainer.end();
  for (; photonItr != photonItrE; ++photonItr) { 
    
     /** photon selection - to be implemented */
     bool select = (*photonItr)->pt() > m_cut_Et;

     if ( select) { 

       if ( i<max ) {

          /** pt */
          pTagColl.insert( m_ptStr[i], (*photonItr)->pt() );

          /** eta */
          pTagColl.insert( m_etaStr[i], (*photonItr)->eta() );

          /** phi */
          pTagColl.insert( m_phiStr[i], (*photonItr)->phi() );

          /** varying levels of tighness cuts */
          unsigned int tightness = 0x0;
	  
	  bool val_loose=0;
	  bool val_tight=0;

	  if(!(*photonItr)->passSelection(val_loose,"Loose")){
	    ATH_MSG_ERROR( "No loose selection exits" );
	  }
	  
	  if(!(*photonItr)->passSelection(val_tight,"Tight")){
	    ATH_MSG_ERROR( "No Tight selection exits" );
	  }
	 

	  if (val_loose  == 1 ) tightness |= (1<<0);//loose

	  if (val_tight  == 1 ) tightness |= (1<<1);//tight
	  
          pTagColl.insert( m_tightStr[i], tightness );

          /** test for converted photon - and set the conversion flag */
          if ((*photonItr)->nVertices() != 0) nConverted++;

          /** Isolation of Photons */

          unsigned int iso = 0x0;
            /* Calo Isolation in bits from 0 to 23 */
            float elEt = (*photonItr)->pt();
            float etcone = 0;
	    if(!((*photonItr)->isolationValue(etcone,xAOD::Iso::IsolationType::etcone20))){
	      ATH_MSG_DEBUG( "No isolation etcone20pt defined" );
	    }
	    else{
	      for (unsigned int j=0; j<m_caloisocutvalues.size(); j++)
		{
		  if ( m_caloisocutvalues[j] < 1.0 ) // relative isolation
		    {
		      float relIso = etcone;
		      if ( elEt != 0.0 ) relIso = relIso/elEt;
		      if ( relIso < m_caloisocutvalues[j] ) iso |= 1 << j;
		    }
		  else if ( etcone < m_caloisocutvalues[j] ) iso |= 1 << j; // absolute isolation
		}
	    }
	    
	    if(!((*photonItr)->isolationValue(etcone,xAOD::Iso::IsolationType::topoetcone20))){
	      ATH_MSG_DEBUG( "No isolation topoetcone20 defined" );
	    }
	    else{
	      for (unsigned int j=0; j<m_caloisocutvalues.size(); j++)
		{
		  if ( m_caloisocutvalues[j] < 1.0 ) // relative isolation
		    {
		      float relIso = etcone;
		      if ( elEt != 0.0 ) relIso = relIso/elEt;
		      if ( relIso < m_caloisocutvalues[j] ) iso |= 1 << (8+j);
		    }
		  else if ( etcone < m_caloisocutvalues[j] ) iso |= 1 << (8+j); // absolute isolation
		}
	    }
	    if(!((*photonItr)->isolationValue(etcone,xAOD::Iso::IsolationType::topoetcone40))){
	      ATH_MSG_DEBUG( "No isolation topoetcone40 defined" );
	    }
	    else{
	      for (unsigned int j=0; j<m_caloisocutvalues.size(); j++)
		{
		  if ( m_caloisocutvalues[j] < 1.0 ) // relative isolation
		    {
		      float relIso = etcone;
		      if ( elEt != 0.0 ) relIso = relIso/elEt;
		      if ( relIso < m_caloisocutvalues[j] ) iso |= 1 << (16+j);
		    }
		  else if ( etcone < m_caloisocutvalues[j] ) iso |= 1 << (16+j); // absolute isolation
		}
            }
            /* Track Isolation in bits from 24 to 29 (note only 6 bits!)*/
            float ptcone =0;
	    if(!((*photonItr)->isolationValue(ptcone,xAOD::Iso::IsolationType::ptcone20))){
	      ATH_MSG_DEBUG( "No isolation ptcone20 defined" );
	    }
	    else{
	      for (unsigned int j=0; j<m_trackisocutvalues.size(); j++)
		{
		  if ( m_caloisocutvalues[j] < 1.0 ) // relative isolation
		    {
		      float relIso = ptcone;
		      if ( elEt != 0.0 ) relIso = relIso/elEt;
		      if ( relIso < m_caloisocutvalues[j] ) iso |= 1 << (24+j);
		    }
		  else if ( ptcone < m_trackisocutvalues[j] ) iso |= 1 << (24+j);
		}
	    }
	    if(!((*photonItr)->isolationValue(etcone,xAOD::Iso::IsolationType::topoetcone30))){	
	      ATH_MSG_DEBUG( "No isolation topoetcone30 defined" );
	    }
	    else{
	      for (unsigned int j=0; j<m_caloisocutvalues.size(); j++)
		{
		  if ( m_caloisocutvalues[j] < 1.0 ) // relative isolation
		    {
		      float relIso = etcone;
		      if ( elEt != 0.0 ) relIso = relIso/elEt;
		      if ( relIso < m_caloisocutvalues[j] ) iso |= 1 << (12+j);
		    }
		  else if ( etcone < m_caloisocutvalues[j] ) iso |= 1 << (12+j); // absolute isolation
		}
	    }
	    
	    if(!((*photonItr)->isolationValue(ptcone,xAOD::Iso::IsolationType::ptcone30))){
	      ATH_MSG_DEBUG( "No isolation ptcone30 defined" );
	    }
	    else{
	      for (unsigned int j=0; j<m_trackisocutvalues.size(); j++)
		{ 
		  if ( m_caloisocutvalues[j] < 1.0 ) // relative isolation
		    {
		      float relIso = ptcone;
		      if ( elEt != 0.0 ) relIso = relIso/elEt;
		      if ( relIso < m_caloisocutvalues[j] ) iso |= 1 << (20+j);
		    }
		  else if ( ptcone < m_trackisocutvalues[j] ) iso |= 1 << (20+j);
		}
	    }
	    
	    if(!((*photonItr)->isolationValue(ptcone,xAOD::Iso::IsolationType::ptcone40))){
	      ATH_MSG_DEBUG( "No isolation ptcone40 defined" );
	    }
	    else{
	      for (unsigned int j=0; j<m_trackisocutvalues.size(); j++)
		{ 
		  if ( m_caloisocutvalues[j] < 1.0 ) // relative isolation
		    {
		      float relIso = ptcone;
		      if ( elEt != 0.0 ) relIso = relIso/elEt;
		      if ( relIso < m_caloisocutvalues[j] ) iso |= 1 << (28+j);
		    }
		  else if ( ptcone < m_trackisocutvalues[j] ) iso |= 1 << (28+j);
		}
	    }
	    
	    pTagColl.insert( m_isoStr[i], iso );
	    
       }
       
       /** counter total number of accepted loose photons */
       i++;
     }
  }
  
  /** insert the number of loose photons */
  pTagColl.insert(PhotonAttributeNames[PhotonID::NPhoton], i);
  pTagColl.insert(PhotonAttributeNames[PhotonID::NConverted], nConverted);
  
  return StatusCode::SUCCESS;
}

/** finalize - called once at the end */
StatusCode PhotonTagTool::finalize() {
  ATH_MSG_DEBUG( "in finalize()" );
  return StatusCode::SUCCESS;
}

/** destructor */
PhotonTagTool::~PhotonTagTool() {}


