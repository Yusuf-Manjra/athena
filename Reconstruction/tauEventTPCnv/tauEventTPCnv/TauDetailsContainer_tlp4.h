/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauDetailsContainer_tlp3.h
/// 
/// Definition of persistent class 
/// for Analysis::TauDetailsContainer
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Ilija Vukotic

#ifndef tauEventTPCnv_TAU_DETAILS_CONTAINER_TLP4_H
#define tauEventTPCnv_TAU_DETAILS_CONTAINER_TLP4_H

#include <vector>
#include "tauEventTPCnv/TauDetailsContainer_p1.h"
#include "tauEventTPCnv/TauCommonDetails_p2.h"
#include "tauEventTPCnv/TauCommonExtraDetails_p1.h"
#include "tauEventTPCnv/TauPi0Details_p2.h"
#include "tauEventTPCnv/TauPi0Candidate_p1.h"
#include "tauEventTPCnv/TauPi0Cluster_p1.h"
#include "tauEventTPCnv/TauShot_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertex_p1.h"
#include "TrkEventTPCnv/VxVertex/Vertex_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"

//Pre declarations
class TauDetailsContainerCnv_tlp4;

///Persistent class for Analysis::TauDetailsContainer
class TauDetailsContainer_tlp4{
    friend class TauDetailsContainerCnv_tlp4;
public:
    ///Constructor 
    TauDetailsContainer_tlp4();
    
    ///Destructor
    virtual ~TauDetailsContainer_tlp4();
private:
    ///Tau details container
    std::vector<TauDetailsContainer_p1> m_tauDetailsContainers;

    ///Tau common details
    std::vector<TauCommonDetails_p2> m_tauCommonDetails;

    ///Tau extra common details
    std::vector<TauCommonExtraDetails_p1> m_tauCommonExtraDetails;

    ///TauPi0Details
    std::vector<TauPi0Details_p2> m_TauPi0Details;

    ///TauPi0Candidate
    std::vector<TauPi0Candidate_p1> m_TauPi0Candidate;
    
    ///TauPi0Cluster
    std::vector<TauPi0Cluster_p1> m_TauPi0Cluster;

    ///TauShot
    std::vector<TauShot_p1> m_TauShot;

    ///Rec vertex
    std::vector<Trk::RecVertex_p1> m_recVertices;

    ///Vertex
    std::vector<Trk::Vertex_p1> m_vertices;

    ///Fit quality
    std::vector<Trk::FitQuality_p1> m_fitQualities;

    ///Error matrix
    std::vector<Trk::HepSymMatrix_p1> m_errorMatrices;
};

#endif // tauEventTPCnv_TAU_DETAILS_CONTAINER_TLP4_H
