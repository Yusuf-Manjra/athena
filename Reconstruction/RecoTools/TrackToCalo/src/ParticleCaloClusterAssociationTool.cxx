/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleCaloClusterAssociationTool.h"
// forward declares

#include "ParticleCaloExtension/ParticleClusterAssociationCollection.h"
#include "ParticlesInConeTools/ICaloClustersInConeTool.h"

#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"

#include "FourMomUtils/P4Helpers.h"

#include "CaloGeoHelpers/CaloSampling.h"

namespace Rec {

  ParticleCaloClusterAssociationTool::ParticleCaloClusterAssociationTool(const std::string& t, const std::string& n, const IInterface*  p )
    : AthAlgTool(t,n,p),
      m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
      m_clustersInConeTool("xAOD::CaloClustersInConeTool/CaloClustersInConeTool"),
      m_caloClusters("CaloCalTopoClusters")
  {
    declareInterface<IParticleCaloClusterAssociationTool>(this);
    declareProperty("ParticleCaloExtensionTool",   m_caloExtensionTool );
    declareProperty("ClustersInConeTool",          m_clustersInConeTool);
    declareProperty("CaloClusterLocation",         m_caloClusters);
    declareProperty("AssociationCollectionName",   m_assCollection);
    //coneSize for including calo cells around track
    declareProperty("ConeSize",                    m_coneSize = 0.1);
    declareProperty("UseCovariance",               m_useCovariance = true);

  }

  ParticleCaloClusterAssociationTool::~ParticleCaloClusterAssociationTool() {}

  StatusCode ParticleCaloClusterAssociationTool::initialize() {
    /* Retrieve track extrapolator from ToolService */
    ATH_CHECK( m_caloExtensionTool.retrieve() );

    if (!m_clustersInConeTool.empty()) ATH_CHECK(m_clustersInConeTool.retrieve());

    return StatusCode::SUCCESS;
  }

  StatusCode ParticleCaloClusterAssociationTool::finalize() {
    return StatusCode::SUCCESS;
  }

  bool ParticleCaloClusterAssociationTool::particleClusterAssociation( const xAOD::IParticle& particle,  const ParticleClusterAssociation*& association, float dr, 
                                                                       const xAOD::CaloClusterContainer* container, bool useCaching ) const {


    ATH_MSG_DEBUG(" particleClusterAssociation: ptr " << &particle << " dr " << dr << " useCaching " << useCaching);

    // reset pointer
    association = 0;
    // check if link is already there
    if( useCaching ){
      if( particle.isAvailable< ParticleClusterAssociation* >("clusterAssociation") ){
        ParticleClusterAssociation* theAssociation = particle.auxdata< ParticleClusterAssociation* >("clusterAssociation");
        if( theAssociation ){
          // check whether the cached association is from the same container
          if( container && theAssociation->container() != container ){
            ATH_MSG_WARNING("Calling cached version with different container pointer");
            return false;
          }
          // check if we need to resize the cone
          if( dr > theAssociation->associationConeSize() ){
            ATH_MSG_DEBUG(" dr larger then cached dr: " << dr << " cached dr " << theAssociation->associationConeSize());
            ParticleClusterAssociation::Data clusters;
            associateClusters(container,theAssociation->caloExtension(),dr,clusters);    
            theAssociation->updateData(std::move(clusters),dr);
          }
          association = theAssociation;
          ATH_MSG_DEBUG("Found existing calo extension");
          return true;
        }
      }
    }

    // get the extrapolation into the calo
    const Trk::CaloExtension* caloExtension = 0;
    if( !m_caloExtensionTool->particleToCaloExtrapolate(particle,caloExtension) ) {
      ATH_MSG_DEBUG("Failed to get calo extension");      
      return false;
    }
    if( caloExtension->caloLayerIntersections().empty()){
      ATH_MSG_DEBUG( "Received a caloExtension object without track extrapolation");
      return false;
    }
    
    //retrieve the cluster container if not provided, return false it retrieval failed
    if( !container && !(container = getClusterContainer()) ) {
      ATH_MSG_DEBUG("Failed to get calo cluster container");      
      return false;
    }
    
    // update cone size in case it is smaller than the default
    if( dr < m_coneSize ) dr = m_coneSize;
    ParticleClusterAssociation::Data clusters;
    associateClusters(container,*caloExtension,dr,clusters);    
    
//     ParticleClusterAssociation* theAssocation = new ParticleClusterAssociation( *caloExtension, std::move(clusters), dr, container );
// 
//     // now add the extension to the output collection so we are not causing any leaks
//     ParticleClusterAssociationCollection* collection = 0;
//     if( !evtStore()->contains<ParticleClusterAssociationCollection>(m_assCollection) ){
//       collection = new ParticleClusterAssociationCollection();
//       if( evtStore()->record( collection, m_assCollection).isFailure() ) {
//         ATH_MSG_WARNING( "Failed to record output collection, will leak the ParticleCaloExtension");
//         delete collection;
//         collection = 0;
//       }
//     }else{
//       if(evtStore()->retrieve(collection,m_assCollection).isFailure()) {
//         ATH_MSG_WARNING( "Unable to retrieve " << m_assCollection << " will leak the ParticleCaloExtension" );
//       }
//     }
//     if( collection ) collection->push_back(theAssocation);
//     else{
//       ATH_MSG_WARNING( "No ParticleCaloCellAssociationCollection, failing extension to avoid memory leak");
//       delete theAssocation;
//       theAssocation = 0;
//     }
// 
//     association = theAssocation;
    association = new ParticleClusterAssociation( *caloExtension, std::move(clusters), dr, container );
    
//     if( useCaching ) particle.auxdecor< ParticleClusterAssociation* >("clusterAssociation") = theAssocation;


    return true;

  }

  void ParticleCaloClusterAssociationTool::associateClusters( const xAOD::CaloClusterContainer* container, 
                                                              const Trk::CaloExtension& caloExtension,
                                                              float dr,
                                                              ParticleClusterAssociation::Data& clusters ) const {    

    const Trk::TrackParameters*  pars = caloExtension.caloEntryLayerIntersection();
    if(!pars) {
      ATH_MSG_WARNING( " NO TrackParameters caloExtension.caloEntryLayerIntersection() ");
      return;
    } 
    double uncertEta = 0.;
    if(pars->covariance()) {
      uncertEta =   -2.*sin(pars->position().theta()) / (cos(2.*pars->position().theta())-1.) * sqrt((*pars->covariance())(Trk::theta,Trk::theta));
      // ATH_MSG_DEBUG( " TrackParameters have covariance; phi*phi " << sqrt((*pars->covariance())(Trk::phi,Trk::phi)) << " theta*theta  " << sqrt((*pars->covariance())(Trk::theta,Trk::theta)) << " eta " << uncertEta);
    } 

    float eta = pars->position().eta();
    float phi = pars->position().phi();
    // ATH_MSG_DEBUG("eta trk " << eta << " phi trk " << phi );
    if( container ){
      float dr2Cut = dr*dr;
      for( unsigned int i=0;i<container->size();++i ){
        float dPhi = P4Helpers::deltaPhi( (*container)[i]->phi(), phi);
        float dEta = (*container)[i]->eta()-eta;
        float dr2  = dPhi*dPhi+ dEta*dEta;
        if(m_useCovariance){
            float eInSample = 0.; 
            float eInSampleFull = 0.; 
            float emfrac = 0.; 
            for (int s=0;s<CaloSampling::Unknown; s++){
                eInSampleFull += (*container)[i]->eSample(CaloSampling::CaloSample(s));
            }
            eInSample += (*container)[i]->eSample(CaloSampling::EMB1);
            eInSample += (*container)[i]->eSample(CaloSampling::EMB2);
            eInSample += (*container)[i]->eSample(CaloSampling::EMB3);
            eInSample += (*container)[i]->eSample(CaloSampling::EME1);
            eInSample += (*container)[i]->eSample(CaloSampling::EME2);
            eInSample += (*container)[i]->eSample(CaloSampling::EME3);
            eInSample += (*container)[i]->eSample(CaloSampling::FCAL1);
            
            emfrac  = eInSample/eInSampleFull;
            if ( emfrac > 1.0 ) emfrac = 1.;
            if ( emfrac < 0.0 ) emfrac = 0.;
            
            double rad;
            (*container)[i]->retrieveMoment(xAOD::CaloCluster::SECOND_R,rad);
            double cent;
            (*container)[i]->retrieveMoment(xAOD::CaloCluster::CENTER_MAG,cent);
            double emProb;
            (*container)[i]->retrieveMoment(xAOD::CaloCluster::EM_PROBABILITY,emProb);
            double sigmaWidth = atan(sqrt(rad)/cent)*cosh((*container)[i]->eta());
            double dr2CutTmp = (sigmaWidth+uncertEta)*(sigmaWidth+uncertEta)+(sigmaWidth+sqrt((*pars->covariance())(Trk::phi,Trk::phi)))*(sigmaWidth+sqrt((*pars->covariance())(Trk::phi,Trk::phi)));
            
            (*container)[i]->auxdecor<double>("sigmaWidth")       = sigmaWidth;
            (*container)[i]->auxdecor<double>("dr2CutTmp")       = sqrt(dr2CutTmp);
            (*container)[i]->auxdecor<double>("dr2")       = sqrt(dr2);
            (*container)[i]->auxdecor<double>("uncertEta")       = uncertEta;
            (*container)[i]->auxdecor<double>("uncertPhi")       = sqrt((*pars->covariance())(Trk::phi,Trk::phi));
            (*container)[i]->auxdecor<double>("emFrac")       = emfrac;
            
            if(sqrt(dr2)<0.1 && dr2 < dr2CutTmp) ATH_MSG_DEBUG("1. selections match! dR " << sqrt(dr2) << " new cut value " << sqrt(dr2CutTmp) << " pt trk " << pars->pT() << " sigma(phi) trk " << sqrt((*pars->covariance())(Trk::phi,Trk::phi)) << " sigma(eta) trk " << uncertEta << " energy cluster " << (*container)[i]->e() << " sigma width " << sigmaWidth << " em frac " << emfrac);
            if(sqrt(dr2)<0.1 && dr2 > dr2CutTmp) ATH_MSG_DEBUG("2. only dR matches! dR " << sqrt(dr2) << " new cut value " << sqrt(dr2CutTmp) << " pt trk " << pars->pT() << " sigma(phi) trk " << sqrt((*pars->covariance())(Trk::phi,Trk::phi)) << " sigma(eta) trk " << uncertEta << " energy cluster " << (*container)[i]->e() << " sigma width " << sigmaWidth << " em frac " << emfrac);
            if(sqrt(dr2)>0.1 && dr2 < dr2CutTmp) ATH_MSG_DEBUG("3. only new matches! dR " << sqrt(dr2) << " new cut value " << sqrt(dr2CutTmp) << " pt trk " << pars->pT() << " sigma(phi) trk " << sqrt((*pars->covariance())(Trk::phi,Trk::phi)) << " sigma(eta) trk " << uncertEta << " energy cluster " << (*container)[i]->e() << " sigma width " << sigmaWidth << " em frac " << emfrac);
            
            dr2Cut = dr2CutTmp;
        }        
        if( dr2 < dr2Cut ){
            clusters.push_back( (*container)[i]);
        } 
      }
    }else{
      ATH_MSG_DEBUG("We're at the clustersinconetool." );
      if( !m_clustersInConeTool->particlesInCone(eta,phi,dr,clusters) ) {
        ATH_MSG_WARNING("Failed to get clusters");
      }
    }
    // ATH_MSG_DEBUG("associated clusters " << clusters.size() << " using cone " << dr );
  }
  
  const xAOD::CaloClusterContainer* ParticleCaloClusterAssociationTool::getClusterContainer() const {

    const xAOD::CaloClusterContainer* container = 0;
    //retrieve the cell container
    if( evtStore()->retrieve(container, m_caloClusters).isFailure() || !container ) {
      ATH_MSG_WARNING( "Unable to retrieve the cluster container  " << m_caloClusters << " container ptr " << container );
      return 0;
    }
    if( container ) ATH_MSG_DEBUG("Retrieved cluster container " << container->size());
    return container;
  }

} // end of namespace Trk
