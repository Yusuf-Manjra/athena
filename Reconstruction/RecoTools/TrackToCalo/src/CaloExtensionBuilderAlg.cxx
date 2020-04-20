/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     CaloExtensionBuilderAlg
PACKAGE:  offline/Reconstruction/RecoTools/TrackToCalo/CaloExtensionBuilderAlg

Based on: offline/Reconstruction/egamma/egammaTrackTools/EMGSFCaloExtensionBuilder

AUTHORS:  Anastopoulos/Capriles
PURPOSE:  Performs Calo Extension for all selected tracks 
 **********************************************************************/
#include "CaloExtensionBuilderAlg.h"
//
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "VxVertex/RecVertex.h"
#include "VxVertex/VxCandidate.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"

//std includes
#include <algorithm>
#include <cmath>
#include <memory>

StatusCode Trk::CaloExtensionBuilderAlg::initialize() 
{
    
    ATH_CHECK(m_TrkSelection.retrieve());  
    ATH_CHECK(m_particleCaloExtensionTool.retrieve());

    ATH_CHECK(m_ParticleCacheKey.initialize());
    ATH_CHECK(m_TrkPartContainerKey.initialize());   
    
    return StatusCode::SUCCESS;
}  

StatusCode Trk::CaloExtensionBuilderAlg::CaloExtensionBuilderAlg::finalize(){ 
    return StatusCode::SUCCESS;
}

StatusCode Trk::CaloExtensionBuilderAlg::execute()
{    

    SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_TrkPartContainerKey);
    if(!tracks.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve TrackParticle container: "<< m_TrkPartContainerKey.key());
        return StatusCode::FAILURE;
    }

    // creating and saving the calo extension collection
    SG::WriteHandle<CaloExtensionCollection> lastCache(m_ParticleCacheKey); 
    ATH_CHECK(lastCache.record(std::make_unique<CaloExtensionCollection>()));

    const xAOD::TrackParticleContainer* ptrTracks=tracks.cptr();
    CaloExtensionCollection* ptrPart=lastCache.ptr();
    std::vector<bool> mask (ptrTracks->size(),false);
    for (auto track: *tracks){      
      if ( static_cast<bool>(m_TrkSelection->accept(*track, nullptr))) mask[track->index()] = true;      
    }

    ATH_CHECK(m_particleCaloExtensionTool->caloExtensionCollection(*ptrTracks,mask,*ptrPart));

    return StatusCode::SUCCESS;
}
