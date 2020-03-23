/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/JetBTaggingAlg.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODBTagging/BTagVertexAuxContainer.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/ReadDecorHandle.h"

#include "JetTagTools/JetFitterVariablesFactory.h"

#include <iostream>
#include <string>

namespace Analysis {


  JetBTaggingAlg::JetBTaggingAlg(const std::string& n, ISvcLocator *p) : 
    AthAlgorithm(n,p),
    m_JetName(""),
    m_BTagLink(".btaggingLink"),
    m_bTagTool("Analysis::BTagTool",this),
    m_bTagSecVtxTool("Analysis::BTagSecVertexing",this),
    m_magFieldSvc("AtlasFieldSvc",n)
  {
    declareProperty("JetCalibrationName", m_JetName);
    declareProperty("BTaggingLink", m_BTagLink);
    declareProperty("BTagTool", m_bTagTool);
    declareProperty("BTagSecVertexing", m_bTagSecVtxTool);
    declareProperty("MagFieldSvc",    m_magFieldSvc );
  }

  JetBTaggingAlg::~JetBTaggingAlg()
  {
    /////....
  }


  StatusCode JetBTaggingAlg::initialize() {

    // This will check that the properties were initialized properly
    // by job configuration.
    ATH_CHECK( m_JetCollectionName.initialize() );
    ATH_CHECK( m_jetParticleLinkNameList.initialize() );
    ATH_CHECK( m_BTagSVCollectionName.initialize() );
    ATH_CHECK( m_BTagJFVtxCollectionName.initialize() );
    ATH_CHECK( m_BTaggingCollectionName.initialize() );
    m_jetBTaggingLinkName = m_JetCollectionName.key() + m_BTagLink;
    ATH_CHECK( m_jetBTaggingLinkName.initialize() );
   
    /// retrieve the main BTagTool
    if ( m_bTagTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_bTagTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_bTagTool);
    }

    /// retrieve the bTagSecVtxTool
    if ( m_bTagSecVtxTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAGVTX# Failed to retrieve tool " << m_bTagSecVtxTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAGVTX# Retrieved tool " << m_bTagSecVtxTool);
    }

    /// retrieve the magnetic field service
    if (m_magFieldSvc.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not get " << m_magFieldSvc);
      return StatusCode::FAILURE;
    }

    if (m_jetParticleLinkNameList.size() == 0) {
      ATH_MSG_FATAL( "#BTAG# Please provide track to jet association list");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }


  StatusCode JetBTaggingAlg::execute() {
    //retrieve the Jet container
    SG::ReadHandle<xAOD::JetContainer> h_JetCollectionName (m_JetCollectionName);
    if (!h_JetCollectionName.isValid()) {
      ATH_MSG_ERROR( " cannot retrieve jet container with key " << m_JetCollectionName.key()  );
      return StatusCode::FAILURE;
    }

    if (h_JetCollectionName->size() == 0) {
      ATH_MSG_DEBUG("#BTAG# Empty JetContainer !!");
    }
    else {
      ATH_MSG_DEBUG("#BTAG#  Nb jets in JetContainer: "<< h_JetCollectionName->size());
    }

    //retrieve the JF Vertex container
    SG::ReadHandle<xAOD::BTagVertexContainer> h_BTagJFVtxCollectionName (m_BTagJFVtxCollectionName.key() );
    if (!h_BTagJFVtxCollectionName.isValid()) {
      ATH_MSG_ERROR( " cannot retrieve JF Vertex container with key " << m_BTagJFVtxCollectionName.key()  );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("#BTAG# Size of the JF Vertex container: " <<  h_BTagJFVtxCollectionName->size());

    //retrieve the Secondary Vertex container
    SG::ReadHandle<xAOD::VertexContainer> h_BTagSVCollectionName ( m_BTagSVCollectionName.key() );
    if (!h_BTagSVCollectionName.isValid()) {
      ATH_MSG_ERROR( " cannot retrieve Sec Vertex container with key " << m_BTagJFVtxCollectionName.key()  );
      return StatusCode::FAILURE;
    }


    //Decor Jet with element link to the BTagging
    SG::WriteDecorHandle<xAOD::JetContainer,ElementLink< xAOD::BTaggingContainer > > h_jetBTaggingLinkName(m_jetBTaggingLinkName);

    //Create a xAOD::BTaggingContainer in any case (must be done)
    std::string bTaggingContName = m_BTaggingCollectionName.key();
    ATH_MSG_DEBUG("#BTAG#  Container name: "<< bTaggingContName);

    /* Record the BTagging  output container */
    SG::WriteHandle<xAOD::BTaggingContainer> h_BTaggingCollectionName (m_BTaggingCollectionName);
    ATH_CHECK( h_BTaggingCollectionName.record(std::make_unique<xAOD::BTaggingContainer>(),
                    std::make_unique<xAOD::BTaggingAuxContainer>()) );

    if (!m_magFieldSvc->solenoidOn()) {
      for (size_t jetIndex=0; jetIndex < h_JetCollectionName->size() ; ++jetIndex) {
        const xAOD::Jet * jet = h_JetCollectionName->at(jetIndex);
        ElementLink< xAOD::BTaggingContainer> linkBTagger;
        h_jetBTaggingLinkName(*jet) = linkBTagger;
      }
      return StatusCode::SUCCESS;
    }
    else { //Solenoid ON
      for ( auto jet : *h_JetCollectionName.ptr()) {
        xAOD::BTagging * newBTagMT  = new xAOD::BTagging();
        h_BTaggingCollectionName->push_back(newBTagMT);
        //Track association
        for(SG::ReadDecorHandleKey<xAOD::JetContainer > elTP : m_jetParticleLinkNameList) {
          SG::ReadDecorHandle<xAOD::JetContainer, std::vector<ElementLink< xAOD::TrackParticleContainer> > > h_jetParticleLinkName(elTP);
          if (!h_jetParticleLinkName.isAvailable()) {
            ATH_MSG_ERROR( " cannot retrieve jet container particle EL decoration with key " << elTP.key()  );
            return StatusCode::FAILURE;
          }
          std::string::size_type iofs=h_jetParticleLinkName.key().rfind(".");
          std::string assocN = h_jetParticleLinkName.key().substr(iofs+1);
          const std::vector< ElementLink< xAOD::TrackParticleContainer > > associationLinks = h_jetParticleLinkName(*jet);
          newBTagMT->auxdata<std::vector<ElementLink<xAOD::TrackParticleContainer> > >(assocN) = associationLinks;
        }
      }
    }

    // Secondary vertex reconstruction.
    StatusCode SV = m_bTagSecVtxTool->BTagSecVertexing_exec(h_JetCollectionName.ptr(), h_BTaggingCollectionName.ptr());
    if (SV.isFailure()) {
      ATH_MSG_WARNING("#BTAG# Failed to reconstruct sec vtx");
    }


    //Tag the jets
    SV = m_bTagTool->tagJet( h_JetCollectionName.ptr(), h_BTaggingCollectionName.ptr(), m_JetName);
    if (SV.isFailure()) {
      ATH_MSG_WARNING("#BTAG# Failed in taggers call");
    }

    //Create the element link from the jet to the btagging
    for (size_t jetIndex=0; jetIndex < h_JetCollectionName->size() ; ++jetIndex) {
      const xAOD::Jet * jetToTag = h_JetCollectionName->at(jetIndex);
      xAOD::BTagging * itBTag = h_BTaggingCollectionName->at(jetIndex);
      ElementLink< xAOD::BTaggingContainer> linkBTagger;
      linkBTagger.toContainedElement(*h_BTaggingCollectionName.ptr(), itBTag);
      h_jetBTaggingLinkName(*jetToTag) = linkBTagger;
    }

    return StatusCode::SUCCESS;

  }

} //// namespace analysis
