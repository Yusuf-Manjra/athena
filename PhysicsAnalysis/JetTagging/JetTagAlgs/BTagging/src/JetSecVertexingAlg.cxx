/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/JetSecVertexingAlg.h"

#include "xAODCore/ShallowCopy.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/PrimaryVertexSelector.h" // new since rel 17.2

//general interface for secondary vertex finders
#include "InDetRecToolInterfaces/ISecVertexInJetFinder.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"
#include "VxSecVertex/VxJetFitterVertexInfo.h"
 
#include "Particle/TrackParticle.h"
#include "JetTagEvent/ISvxAssociation.h"
#include "JetTagEvent/TrackAssociation.h"

#include "VxJetVertex/VxJetCandidate.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxJetVertex/SelectedTracksInJet.h"
#include "VxJetVertex/TwoTrackVerticesInJet.h"
#include "VxJetVertex/VxClusteringTable.h"
#include "VxJetVertex/PairOfVxVertexOnJetAxis.h"
#include "VxJetVertex/VxClusteringTable.h"

#include "VxVertex/VxTrackAtVertex.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "TrkLinks/LinkToXAODTrackParticle.h"

#include "JetTagTools/JetFitterVariablesFactory.h"
#include "JetTagTools/MSVVariablesFactory.h"

#include "xAODBTagging/BTagging.h"

#include "xAODBTagging/BTagVertex.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODBTagging/BTagVertexAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBase/IParticle.h"

#include "StoreGate/ReadDecorHandle.h"
#include "StoreGate/ReadHandle.h"

namespace Analysis {

  JetSecVertexingAlg::JetSecVertexingAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_secVertexFinderToolsHandle(this),
    m_JFvarFactory("Analysis::JetFitterVariablesFactory",this),
    m_MSVvarFactory("Analysis::MSVVariablesFactory",this),
    m_vxPrimaryName("PrimaryVertices")
  {
    declareProperty("PrimaryVertexName",  m_vxPrimaryName);
    //List of the secondary vertex finders in jet to be run
    declareProperty("SecVtxFinder",          m_secVertexFinderToolsHandle);
    //declareProperty("SecVtxFinderTrackNameList", m_secVertexFinderTrackNameList);
    declareProperty("SecVtxFinderxAODBaseName", m_secVertexFinderBaseName);
    declareProperty("JetFitterVariableFactory",          m_JFvarFactory);
    declareProperty("MSVVariableFactory",          m_MSVvarFactory);
  }

  JetSecVertexingAlg::~JetSecVertexingAlg() {
  }

  StatusCode JetSecVertexingAlg::initialize()
  {
    // This will check that the properties were initialized properly
    // by job configuration.
    ATH_CHECK( m_JetCollectionName.initialize() );
    ATH_CHECK( m_jetParticleLinkName.initialize() );
    ATH_CHECK( m_VertexCollectionName.initialize() );
    ATH_CHECK( m_BTagSVCollectionName.initialize() );
    ATH_CHECK( m_BTagJFVtxCollectionName.initialize() );

    /* ----------------------------------------------------------------------------------- */
    /*                        RETRIEVE SERVICES FROM STOREGATE                             */
    /* ----------------------------------------------------------------------------------- */

    /*if ( m_secVertexFinderToolsHandleArray.empty()) {
      ATH_MSG_ERROR("SecVtxFinderList is empty");
      return StatusCode::FAILURE;
    }*/

    /*if(m_secVertexFinderTrackNameList.size() != m_secVertexFinderToolsHandleArray.size()){
      ATH_MSG_ERROR("#BTAG# Size mismatch between secVertexFinderToolsHandleArray and track name list ");
      return StatusCode::FAILURE;
    }*/

    /*if(m_secVertexFinderBaseNameList.size() != m_secVertexFinderToolsHandleArray.size()){
      ATH_MSG_ERROR("#BTAG# Size mismatch between secVertexFinderToolsHandleArray and base name list ");
      return StatusCode::FAILURE;
    }*/

    if ( m_secVertexFinderToolsHandle.retrieve().isFailure() ) {
      ATH_MSG_ERROR("#BTAG# Failed to retrieve " << m_secVertexFinderToolsHandle);
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved " << m_secVertexFinderToolsHandle);
    }
    if ( m_JFvarFactory.retrieve().isFailure() ) {
       ATH_MSG_ERROR("#BTAG# Failed to retrieve " << m_JFvarFactory);
    } else {
       ATH_MSG_DEBUG("#BTAG# Retrieved " << m_JFvarFactory);
    }
    if ( m_MSVvarFactory.retrieve().isFailure() ) {
       ATH_MSG_ERROR("#BTAG# Failed to retrieve " << m_MSVvarFactory);
    } else {
       ATH_MSG_DEBUG("#BTAG# Retrieved " << m_MSVvarFactory);
    }

    return StatusCode::SUCCESS;
  }


  StatusCode JetSecVertexingAlg::execute() {
    //retrieve the Jet container
    SG::ReadHandle<xAOD::JetContainer> h_JetCollectionName (m_JetCollectionName);
    if (!h_JetCollectionName.isValid()) {
      ATH_MSG_ERROR( " cannot retrieve jet container with key " << m_JetCollectionName.key()  );
      return StatusCode::FAILURE;
    }

    SG::ReadDecorHandle<xAOD::JetContainer, std::vector<ElementLink< xAOD::TrackParticleContainer> > > h_jetParticleLinkName (m_jetParticleLinkName);
    /*if (!h_jetParticleLinkName.isAvailable()) { //isValid() is also crashing
      ATH_MSG_ERROR( " cannot retrieve jet container particle EL decoration with key " << m_jetParticleLinkName.key()  );
      return StatusCode::FAILURE;
    }*/
 
    const xAOD::Vertex* primaryVertex(0);

    //retrieve primary vertex
    SG::ReadHandle<xAOD::VertexContainer> h_VertexCollectionName (m_VertexCollectionName);
    if (!h_VertexCollectionName.isValid()) {
        ATH_MSG_ERROR( " cannot retrieve primary vertex container with key " << m_VertexCollectionName.key()  );
        return StatusCode::FAILURE;
    }
    unsigned int nVertexes = h_VertexCollectionName->size();
    if (nVertexes == 0) {
      ATH_MSG_DEBUG("#BTAG#  Vertex container is empty");
      return StatusCode::SUCCESS;
    }
    for (xAOD::VertexContainer::const_iterator fz = h_VertexCollectionName->begin(); fz != h_VertexCollectionName->end(); ++fz) {
      if ((*fz)->vertexType() == xAOD::VxType::PriVtx) {
	primaryVertex = *fz;
	break;
      }
    }


    if (! primaryVertex) {
      ATH_MSG_DEBUG("#BTAG#  No vertex labeled as VxType::PriVtx!");
      xAOD::VertexContainer::const_iterator fz = h_VertexCollectionName->begin();
      primaryVertex = *fz;
        if (primaryVertex->nTrackParticles() == 0) {
	  ATH_MSG_DEBUG("#BTAG#  PV==BeamSpot: probably poor tagging");
      }
    }

    const xAOD::Vertex& PrimaryVtx = *primaryVertex;
    std::string basename =  m_secVertexFinderBaseName;

    SG::WriteHandle<xAOD::VertexContainer> h_BTagSVCollectionName;
    SG::WriteHandle<xAOD::BTagVertexContainer> h_BTagJFVtxCollectionName;

    /* Record the BTagging JF Vertex  output container */
    if (basename == "JetFitter") {
      ATH_MSG_DEBUG("#BTAG# Record the BTagging JF Vertex  output container");
      //SG::WriteHandle<xAOD::BTagVertexContainer> h_BTagJFVtxCollectionName (m_BTagJFVtxCollectionName);
      h_BTagJFVtxCollectionName = SG::WriteHandle<xAOD::BTagVertexContainer>(m_BTagJFVtxCollectionName);
      ATH_CHECK( h_BTagJFVtxCollectionName.record(std::make_unique<xAOD::BTagVertexContainer>(),
                                        std::make_unique<xAOD::BTagVertexAuxContainer>()) );
    }

    /* Record the BTagging Secondary Vertex output container */
    if ((basename == "SV1") || (basename == "MSV")) {
      ATH_MSG_DEBUG("#BTAG# Record the BTagging Secondary Vertex output container");
      //SG::WriteHandle<xAOD::VertexContainer> h_BTagSVCollectionName (m_BTagSVCollectionName);
      h_BTagSVCollectionName = SG::WriteHandle<xAOD::VertexContainer>(m_BTagSVCollectionName);
      ATH_CHECK( h_BTagSVCollectionName.record(std::make_unique<xAOD::VertexContainer>(),
                                        std::make_unique<xAOD::VertexAuxContainer>()) );
    }

    for (xAOD::JetContainer::const_iterator jetIter = h_JetCollectionName->begin(); jetIter != h_JetCollectionName->end(); ++jetIter) {
      const xAOD::Jet& jetToTag = **jetIter;
      const xAOD::TrackParticleContainer* theTrackParticleContainer = 0;

        //Get it from decor jet
        const std::vector<ElementLink< xAOD::TrackParticleContainer > >& tracksInJet = h_jetParticleLinkName(jetToTag);

        if(tracksInJet.size()==0){
          continue;
	  //return StatusCode::SUCCESS;
        } 
   
        std::vector<const xAOD::IParticle*> inputIParticles;

        std::vector<ElementLink< xAOD::TrackParticleContainer > >::const_iterator itEL = tracksInJet.begin();
        std::vector<ElementLink< xAOD::TrackParticleContainer > >::const_iterator itELend = tracksInJet.end();

        
        //Before splitting empty tracksInJet case

        for (  ; itEL != itELend; ++itEL ) {
          const xAOD::TrackParticle* const *inputTrackParticle ;
          inputTrackParticle = (*itEL).cptr(); //ElementConstPointer cptr

	  /// warning -> will not work if at some point we decide to associate to several track collections at the same time (in the same assoc object)
	  
          theTrackParticleContainer = (*itEL).getStorableObjectPointer();

          inputIParticles.push_back(*inputTrackParticle);
        }

        ATH_MSG_DEBUG("#BTAG#  Running " << m_secVertexFinderToolsHandle);

        const Trk::VxSecVertexInfo* myVertexInfo = m_secVertexFinderToolsHandle->findSecVertex(PrimaryVtx, (*jetIter)->p4(), inputIParticles);

        ATH_MSG_DEBUG("#BTAG#  Storing result ");

        if (const Trk::VxSecVKalVertexInfo* myVertexInfoVKal = dynamic_cast<const Trk::VxSecVKalVertexInfo*>(myVertexInfo)) {
	  ATH_MSG_DEBUG("#BTAG# Found VKalVertexInfo information");
          if(basename.find("MSV") == 0){
            StatusCode sc = m_MSVvarFactory->createMSVContainer(jetToTag, myVertexInfoVKal, &(*h_BTagSVCollectionName), PrimaryVtx);
            if(sc.isFailure()){
              ATH_MSG_ERROR("#BTAG# error filling variables in MSVVariablesFactory" );
              delete myVertexInfo;
              return sc;
            }
          }
          else { //SV1 
	    StatusCode sc = createSecVkalContainer(&(*h_BTagSVCollectionName), myVertexInfoVKal);
	    if(sc.isFailure()){
	      ATH_MSG_ERROR("#BTAG# error filling variables from VxSecVKalVertexInfo for tool " << m_secVertexFinderToolsHandle);
              delete myVertexInfo;
	      return sc;
	    }
          }
        } else if (const Trk::VxJetFitterVertexInfo* myVertexInfoJetFitter = dynamic_cast<const Trk::VxJetFitterVertexInfo*>(myVertexInfo)) {
          ATH_MSG_DEBUG("#BTAG# Found VxJetFitterVertexInfo information");
          StatusCode sc = createJFContainer(&(*h_BTagJFVtxCollectionName), myVertexInfoJetFitter, theTrackParticleContainer);
          if(sc.isFailure()){
            ATH_MSG_ERROR("#BTAG# error filling variables from VxJetFitterVertexInfo for tool " << m_secVertexFinderToolsHandle);
            delete myVertexInfo;
            return sc;
          }
        } else {
	  ATH_MSG_WARNING("#BTAG# VxSecVertexInfo pointer cannot be interpreted for tool " << m_secVertexFinderToolsHandle);
        }
        delete myVertexInfo;
      //} /// for loop on vertex tools
    }// for loop on jets

    return StatusCode::SUCCESS;
  } 

  StatusCode JetSecVertexingAlg::createSecVkalContainer(xAOD::VertexContainer* bTagVertexContainer,
							 const Trk::VxSecVKalVertexInfo* myVertexInfoVKal) const {
  
    std::vector<xAOD::Vertex*>::const_iterator verticesBegin = myVertexInfoVKal->vertices().begin();
    std::vector<xAOD::Vertex*>::const_iterator verticesEnd   = myVertexInfoVKal->vertices().end();

    for (std::vector<xAOD::Vertex*>::const_iterator verticesIter=verticesBegin; verticesIter!=verticesEnd;++verticesIter) {
      xAOD::Vertex* btagVtx = *verticesIter;
      bTagVertexContainer->push_back(btagVtx);
    }
    
    return StatusCode::SUCCESS;
  }


  StatusCode JetSecVertexingAlg::createJFContainer(xAOD::BTagVertexContainer* bTagJFVertexContainer,
					       const Trk::VxJetFitterVertexInfo* myVertexInfoJetFitter,
					       const xAOD::TrackParticleContainer* theTrackParticleContainer) const { 

     //list of JFvertices
     const std::vector<Trk::VxJetCandidate*> JFvertices =  myVertexInfoJetFitter->verticesJF();
     
  
     int nVtx = 0;
     if (JFvertices.size() > 0) {
       Trk::VxJetCandidate* vxjetcand = dynamic_cast< Trk::VxJetCandidate*>(JFvertices[0]);
       if (!vxjetcand) {
         ATH_MSG_WARNING("#BTAG# bad VxCandidate is not a VxJetCandidate");
         return StatusCode::SUCCESS;
       }

       //compatibility with others SV
       const Trk::VxClusteringTable* ClusteringTable = vxjetcand->getClusteringTable();
       typedef std::map<Trk::VxVertexOnJetAxis*, std::vector<std::pair<Trk::VxVertexOnJetAxis*,float> > > map2vtx;
       map2vtx inverseCompMap;

       if (ClusteringTable !=0) {
         const std::map<float,Trk::PairOfVxVertexOnJetAxis>& mapPairVtxOnJetAxis = ClusteringTable->getCompatibilityPairOfVertices();
         std::map<float,Trk::PairOfVxVertexOnJetAxis>::const_iterator mapiter = mapPairVtxOnJetAxis.begin();
         for(;mapiter != mapPairVtxOnJetAxis.end(); ++mapiter){

           Trk::VxVertexOnJetAxis* vtx1 = mapiter->second.first;
           Trk::VxVertexOnJetAxis* vtx2 = mapiter->second.second;
           float comp = mapiter->first;

           std::pair<Trk::VxVertexOnJetAxis*,float> pair1(vtx1, comp);
           std::pair<Trk::VxVertexOnJetAxis*,float> pair2(vtx1, comp);

           inverseCompMap[vtx1].push_back(pair2);
           inverseCompMap[vtx2].push_back(pair1);

         }
       } 

       //vtx on jet axis
       const std::vector<Trk::VxVertexOnJetAxis*> Vtxonjetaxes = vxjetcand->getVerticesOnJetAxis();
     
       std::vector<Trk::VxVertexOnJetAxis*>::const_iterator iterBegin = Vtxonjetaxes.begin();
       std::vector<Trk::VxVertexOnJetAxis*>::const_iterator iterEnd   = Vtxonjetaxes.end();  
       float comptoPV(0.);
       double chi2(0.);
       int   ndf(0);
       nVtx = Vtxonjetaxes.size(); 
       std::vector< ElementLink< xAOD::BTagVertexContainer > > JFVerticesLinks;
       std::vector<xAOD::BTagVertex*> JFVertices;
       std::map<Trk::VxVertexOnJetAxis*, ElementLink< xAOD::BTagVertexContainer> > oldnewmap;

       for (std::vector<Trk::VxVertexOnJetAxis*>::const_iterator it=iterBegin; it!=iterEnd; ++it) {
         //ndf, chi2 , comptoPV   
         const Trk::FitQuality& fitquality = (*it)->fitQuality();
         chi2 =  fitquality.chiSquared();
         ndf  =  fitquality.numberDoF();
         comptoPV =  (*it)->getCompatibilityToPrimaryVtx();
         const std::vector<Trk::VxTrackAtVertex*> & trackatVtx = (*it)->getTracksAtVertex();
         std::vector<Trk::VxTrackAtVertex*>::const_iterator itrBegin = trackatVtx.begin();
         std::vector<Trk::VxTrackAtVertex*>::const_iterator itrEnd   = trackatVtx.end();
         std::vector< float>  tmpchi2;
         std::vector< float>  tmpndf;
         std::vector< ElementLink< xAOD::TrackParticleContainer > > trkLinks;
         std::vector< float>  tmpRefPx;
         std::vector< float>  tmpRefPy;
         std::vector< float>  tmpRefPz;
  
         //loop in trks 
         for (std::vector<Trk::VxTrackAtVertex*>::const_iterator itr=itrBegin; itr!=itrEnd; ++itr) {
           //ndf, chi2   
           const Trk::FitQuality& trkquality = (*itr)->trackQuality();
           double tpchi2 = trkquality.chiSquared();
           int tpndf = trkquality.numberDoF();
           tmpchi2.push_back(float(tpchi2));
           tmpndf.push_back(float(tpndf));
           //tracks links
           Trk::ITrackLink* trklinks = (*itr)->trackOrParticleLink();
           const Trk::LinkToXAODTrackParticle* trkLinkTPxAOD=dynamic_cast<const Trk::LinkToXAODTrackParticle *>(trklinks);
           if (!trkLinkTPxAOD) {
             ATH_MSG_WARNING("#BTAG# bad ITrackLink is not a LinkToXAODTrackParticle");
             continue;
           }
           const xAOD::TrackParticle* myTrklink = **trkLinkTPxAOD;
           ElementLink<xAOD::TrackParticleContainer> tpel;
           
           tpel.toContainedElement(*theTrackParticleContainer, myTrklink);
           trkLinks.push_back(tpel);
         
           //reffited momentum   
           const Trk::TrackParameters* trkperigee = (*itr)->perigeeAtVertex();
           Amg::Vector3D mytrack = trkperigee->momentum();
           tmpRefPx.push_back(mytrack.x());
           tmpRefPy.push_back(mytrack.y());
           tmpRefPz.push_back(mytrack.z());

         }
         xAOD::BTagVertex* newbtagVtx = new xAOD::BTagVertex();
	     JFVertices.push_back(newbtagVtx);

         bTagJFVertexContainer->push_back(newbtagVtx);
    
         newbtagVtx->setNDF(ndf);
         newbtagVtx->setChi2(chi2);
         newbtagVtx->setPV_compatibility(comptoPV);
         newbtagVtx->setTrack_chi2(tmpchi2);
         newbtagVtx->setTrack_NDF(tmpndf);
         newbtagVtx->setTrack_links(trkLinks);
         newbtagVtx->setTrack_refPx(tmpRefPx);
         newbtagVtx->setTrack_refPy(tmpRefPy);
         newbtagVtx->setTrack_refPz(tmpRefPz);

         ElementLink< xAOD::BTagVertexContainer> linkBTagVertex;
         linkBTagVertex.toContainedElement(*bTagJFVertexContainer, newbtagVtx);
         JFVerticesLinks.push_back(linkBTagVertex);
	     oldnewmap.insert(std::make_pair(*it,linkBTagVertex));
      }

       //CompToOtherSecVertices
      
     if(ClusteringTable !=0){
       //int btagvxi=0;
	   for (std::vector<Trk::VxVertexOnJetAxis*>::const_iterator it=iterBegin; it!=iterEnd; ++it) {

	       std::vector< ElementLink< xAOD::BTagVertexContainer > > JFCompLinks;
	       std::vector<float> JFCompValues;

	     bool foundmap=true;
	     map2vtx::iterator itmap = inverseCompMap.find(*it);  
	     if(itmap == inverseCompMap.end()){
	       ATH_MSG_DEBUG("#BTAG# JF compatibility value in map2vtx map not found ");
	       foundmap=false;
	     }

	     if(foundmap){

	       std::vector<std::pair<Trk::VxVertexOnJetAxis*,float> > tmpvectpair = itmap->second;

	       for(unsigned int icomppair=0; icomppair<tmpvectpair.size(); ++icomppair){
	        std::map<Trk::VxVertexOnJetAxis*, ElementLink< xAOD::BTagVertexContainer> >::iterator JFOlNewIter = oldnewmap.find(tmpvectpair[icomppair].first);
	 
	         if(JFOlNewIter == oldnewmap.end()){
		       ATH_MSG_DEBUG("#BTAG# JF compatibility vector not found ");
		       continue;
	         }

	       JFCompLinks.push_back(JFOlNewIter->second);
	       JFCompValues.push_back(tmpvectpair[icomppair].second);
	       }

	     } /// found map
	 
	   }
     }
     typedef std::vector<ElementLink<xAOD::BTagVertexContainer> > BTagVertices;

      const Trk::RecVertexPositions& recVtxposition = vxjetcand->getRecVertexPositions();
      const Amg::VectorX& vtxPositions = recVtxposition.position();
      const Amg::MatrixX& vtxCovMatrix = recVtxposition.covariancePosition();
      ATH_MSG_DEBUG("#BTAGJF# size vtxPosition "<<vtxPositions.size());
      std::vector< float > fittedPosition = std::vector<float>(nVtx+5,-1);
      std::vector< float > fittedCov = std::vector<float>(nVtx+5,-1); //only store the diagonal terms
      if(vtxPositions.rows()>4 ) {
        fittedPosition[0] = vtxPositions[Trk::jet_xv]; //position x,y,z of PV
        fittedPosition[1] = vtxPositions[Trk::jet_yv]; 
        fittedPosition[2] = vtxPositions[Trk::jet_zv]; 
        fittedPosition[3] = vtxPositions[Trk::jet_phi];  // direction of the jet axis
        fittedPosition[4] = vtxPositions[Trk::jet_theta];
              
        fittedCov[0] = vtxCovMatrix(0,0);
        fittedCov[1] = vtxCovMatrix(1,1);
        fittedCov[2] = vtxCovMatrix(2,2);
        fittedCov[3] = vtxCovMatrix(3,3);
        fittedCov[4] = vtxCovMatrix(4,4);
        
      }

      for(int i=0; i<nVtx; ++i){
        fittedPosition[i+5] = vtxPositions[i+5]; //dist of vtxi on jet axis from PV
        fittedCov[i+5] = vtxCovMatrix(i+5,i+5);
      }
      
      //trks at PV
      const std::vector<Trk::VxTrackAtVertex*> & trackatPV = vxjetcand->getPrimaryVertex()->getTracksAtVertex();
      std::vector<Trk::VxTrackAtVertex*>::const_iterator irBegin = trackatPV.begin();
      std::vector<Trk::VxTrackAtVertex*>::const_iterator irEnd   = trackatPV.end();
      std::vector< float > tracksAtPVchi2;
      std::vector< float > tracksAtPVndf;
      std::vector< ElementLink< xAOD::TrackParticleContainer > > tracksAtPVlinks;

      for (std::vector<Trk::VxTrackAtVertex*>::const_iterator it=irBegin; it!=irEnd; ++it) {
        const Trk::FitQuality& trkquality = (*it)->trackQuality();
        double tmpchi2 = trkquality.chiSquared();
        int tmpndf = trkquality.numberDoF();
        tracksAtPVchi2.push_back(float(tmpchi2));
        tracksAtPVndf.push_back(float(tmpndf));
        //links
        
        Trk::ITrackLink* trklinks = (*it)->trackOrParticleLink();
        const Trk::LinkToXAODTrackParticle* trkLinkTPxAOD = dynamic_cast<const Trk::LinkToXAODTrackParticle *>(trklinks);
        if (!trkLinkTPxAOD) {
          ATH_MSG_WARNING("#BTAG# bad ITrackLink is not a LinkToXAODTrackParticle");
          continue;
        }
        const xAOD::TrackParticle* myTrklink = **trkLinkTPxAOD;
        ElementLink<xAOD::TrackParticleContainer> tpel;
        tpel.toContainedElement(*theTrackParticleContainer, myTrklink);
        tracksAtPVlinks.push_back(tpel);
      }
    
      
    }
    else { //No JF vertices
      ATH_MSG_ERROR("#BTAG# No JF vertices. Minimum 1");
    }

    /*StatusCode sc = m_JFvarFactory->fillJetFitterVariables(myJet, newBTag, myVertexInfoJetFitter, basename);
    if(sc.isFailure()){
      ATH_MSG_ERROR("#BTAG# error filling variables in JetFitterVariablesFactory" );
      return sc;
    }*/

    return StatusCode::SUCCESS;


  }
 
} // namespace
