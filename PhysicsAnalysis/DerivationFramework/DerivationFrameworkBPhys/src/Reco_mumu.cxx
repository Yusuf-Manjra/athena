/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// Reco_mumu.cxx
///////////////////////////////////////////////////////////////////
// Author: Daniel Scheirich <daniel.scheirich@cern.ch>
// Based on the Integrated Simulation Framework
//
// Basic Jpsi->mu mu derivation example

#include "DerivationFrameworkBPhys/Reco_mumu.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "DerivationFrameworkBPhys/BPhysPVTools.h"


namespace DerivationFramework {

  Reco_mumu::Reco_mumu(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_v0Tools("Trk::V0Tools"),
    m_jpsiFinder("Analysis::JpsiFinder"),
    m_pvRefitter("Analysis::PrimaryVertexRefitter") {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    // Declare tools    
    declareProperty("V0Tools"   , m_v0Tools);
    declareProperty("JpsiFinder", m_jpsiFinder);
    declareProperty("PVRefitter", m_pvRefitter);
    
    // Declare user-defined properties
    declareProperty("OutputVtxContainerName", m_outputVtxContainerName = "OniaCandidates");
    declareProperty("PVContainerName"       , m_pvContainerName        = "PrimaryVertices");
    declareProperty("RefPVContainerName"    , m_refPVContainerName     = "RefittedPrimaryVertices");
    declareProperty("RefitPV"               , m_refitPV                = false);
    declareProperty("MaxPVrefit"            , m_PV_max                 = 1);
    declareProperty("DoVertexType"          , m_DoVertexType           = 1);
    // minimum number of tracks for PV to be considered for PV association
    declareProperty("MinNTracksInPV"        , m_PV_minNTracks          = 0);
    declareProperty("Do3d"                  , m_do3d                   = false);
    declareProperty("CheckCollections"      , m_checkCollections       = false);
    declareProperty("CheckVertexContainers" , m_CollectionsToCheck);
  }

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode Reco_mumu::initialize()
  {
  
    ATH_MSG_DEBUG("in initialize()");
 
    // retrieve V0 tools
    CHECK( m_v0Tools.retrieve() );
    
    // get the JpsiFinder tool
    CHECK( m_jpsiFinder.retrieve() );
     
    // get the PrimaryVertexRefitter tool
    CHECK( m_pvRefitter.retrieve() );

    // Get the beam spot service   
    CHECK( m_beamSpotKey.initialize() );
    return StatusCode::SUCCESS;
 }
  
 

  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  
  StatusCode Reco_mumu::addBranches() const
  {
    bool callJpsiFinder = true;
    if(m_checkCollections) {
      for(const auto &str : m_CollectionsToCheck){
         const xAOD::VertexContainer*    vertContainer = nullptr;
         ATH_CHECK( evtStore()->retrieve(vertContainer, str) );
         if(vertContainer->size() == 0) {
            callJpsiFinder = false;
            ATH_MSG_DEBUG("Container VertexContainer (" << str << ") is empty");
            break;//No point checking other containers
         }/*else{
            callJpsiFinder = true;
            ATH_MSG_DEBUG("Container VertexContainer (" << str << ") has events N= " << vertContainer->size());
         }*/
      }
    }

    // Jpsi container and its auxilliary store
    xAOD::VertexContainer*    vtxContainer = NULL;
    xAOD::VertexAuxContainer* vtxAuxContainer = NULL;
    
    if(callJpsiFinder) {
    //----------------------------------------------------
    // call Jpsi finder
    //----------------------------------------------------
    if( !m_jpsiFinder->performSearch(vtxContainer, vtxAuxContainer).isSuccess() ) {
      ATH_MSG_FATAL("Jpsi finder (" << m_jpsiFinder << ") failed.");
      return StatusCode::FAILURE;
    }

    //----------------------------------------------------
    // retrieve primary vertices
    //----------------------------------------------------
    const xAOD::VertexContainer*    pvContainer = NULL;
    CHECK( evtStore()->retrieve(pvContainer, m_pvContainerName) );

    //----------------------------------------------------
    // Try to retrieve refitted primary vertices
    //----------------------------------------------------
    bool refPvExists = false;
    xAOD::VertexContainer*    refPvContainer = NULL;
    xAOD::VertexAuxContainer* refPvAuxContainer = NULL;
    if(m_refitPV) {
      if(evtStore()->contains<xAOD::VertexContainer>(m_refPVContainerName)) {
        // refitted PV container exists. Get it from the store gate
        CHECK( evtStore()->retrieve(refPvContainer, m_refPVContainerName) );
        CHECK( evtStore()->retrieve(refPvAuxContainer, m_refPVContainerName+"Aux.") );
        refPvExists = true;
      } else {
        // refitted PV container does not exist. Create a new one.
        refPvContainer = new xAOD::VertexContainer;
        refPvAuxContainer = new xAOD::VertexAuxContainer;
        refPvContainer->setStore(refPvAuxContainer);
      }
    }
     // Give the helper class the ptr to v0tools and beamSpotsSvc to use
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
    if(not beamSpotHandle.isValid()) ATH_MSG_ERROR("Cannot Retrieve " << m_beamSpotKey.key() );
    
    // Give the helper class the ptr to v0tools and beamSpotsSvc to use
    BPhysPVTools helper(&(*m_v0Tools), beamSpotHandle.cptr());
    helper.SetMinNTracksInPV(m_PV_minNTracks);
    helper.SetSave3d(m_do3d);

    if(m_refitPV){ 
       if(vtxContainer->size() >0){
        StatusCode SC = helper.FillCandwithRefittedVertices(vtxContainer,  pvContainer, refPvContainer, &(*m_pvRefitter) , m_PV_max, m_DoVertexType);
        if(SC.isFailure()){
            ATH_MSG_FATAL("refitting failed - check the vertices you passed");
            return SC;
        }
        }
    }else{
        if(vtxContainer->size() >0)CHECK(helper.FillCandExistingVertices(vtxContainer, pvContainer, m_DoVertexType));
    }
    
    //----------------------------------------------------
    // save in the StoreGate
    //----------------------------------------------------
    if (!evtStore()->contains<xAOD::VertexContainer>(m_outputVtxContainerName))       
      CHECK(evtStore()->record(vtxContainer, m_outputVtxContainerName));
    
    if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_outputVtxContainerName+"Aux.")) 
      CHECK(evtStore()->record(vtxAuxContainer, m_outputVtxContainerName+"Aux."));
    
    if(!refPvExists && m_refitPV) {
      CHECK(evtStore()->record(refPvContainer   , m_refPVContainerName));
      CHECK(evtStore()->record(refPvAuxContainer, m_refPVContainerName+"Aux."));
    }
    }    

    if (!callJpsiFinder) { //Fill with empty containers
      vtxContainer = new xAOD::VertexContainer;
      vtxAuxContainer = new xAOD::VertexAuxContainer;
      vtxContainer->setStore(vtxAuxContainer);
      if (!evtStore()->contains<xAOD::VertexContainer>(m_outputVtxContainerName))       
        CHECK(evtStore()->record(vtxContainer, m_outputVtxContainerName));
      
      if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_outputVtxContainerName+"Aux.")) 
        CHECK(evtStore()->record(vtxAuxContainer, m_outputVtxContainerName+"Aux."));
    }
    
    return StatusCode::SUCCESS;
  }  
}
