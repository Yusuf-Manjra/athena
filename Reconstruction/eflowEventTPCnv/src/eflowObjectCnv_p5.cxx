/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "eflowEvent/eflowObject.h"
#undef private
#undef protected

#include "eflowEventTPCnv/eflowObjectCnv_p5.h"
#include "Navigation/NavigableVector.h"

void eflowObjectCnv_p5::persToTrans( const eflowObject_p5 *persObj, eflowObject *transObj, MsgStream &eflowStream ){


  //data

  transObj->m_eflowType = persObj->m_eflowType;
  transObj->m_charge = persObj->m_charge;
  transObj->m_recoStatus.m_status = persObj->m_recoStatus;
  transObj->m_pi0MVA = persObj->m_pi0MVA;
  transObj->m_centerMag = persObj->m_centerMag;

  transObj->set_eta(persObj->m_eta);
  transObj->set_phi(persObj->m_phi);
  transObj->set_energy(persObj->m_e);
  transObj->set_m(persObj->m_m);

  //navigable vectors data
  //convert persistent elementlinkvector to transient first and then copy links to navigable vectors

  ElementLinkVector<Rec::TrackParticleContainer> dummyTrackVector;

  m_navTrackCnv.persToTrans(&persObj->m_eflowTrack,&dummyTrackVector,eflowStream);

  ElementLinkVector<Rec::TrackParticleContainer>::iterator firstTrack = dummyTrackVector.begin();
  ElementLinkVector<Rec::TrackParticleContainer>::iterator lastTrack = dummyTrackVector.end();

  int i = 0;

  for (; firstTrack != lastTrack; firstTrack++){
    ElementLink<Rec::TrackParticleContainer> thisTrackElementLink = *firstTrack;
    if (thisTrackElementLink.isValid()){
      transObj->m_eflowTrack.addElement(thisTrackElementLink);
    }
    else if (eflowStream.level() <= MSG::DEBUG) eflowStream << MSG::DEBUG << "This Track element link is not valid" << endreq;
    i++;

  }//track loop

  ElementLinkVector<CaloClusterContainer> dummyClusterVector;
  
  m_navClusCnv.persToTrans(&persObj->m_eflowClus,&dummyClusterVector,eflowStream);

  ElementLinkVector<CaloClusterContainer>::iterator firstClus = dummyClusterVector.begin();
  ElementLinkVector<CaloClusterContainer>::iterator lastClus = dummyClusterVector.end();

  for (; firstClus != lastClus; firstClus++){
    //const CaloClusterContainer* clusContainer = (*firstClus).getDataPtr();
    ElementLink<CaloClusterContainer> thisClusElementLink = *firstClus;
    if (thisClusElementLink.isValid()) {
      const CaloCluster* clus = **firstClus;
      //change this for now...should change back if switch from using VIEW containers
      //transObj->m_eflowClus.addElement(clusContainer,clus);
      transObj->addClus(clus);
    }
    else if (eflowStream.level() <= MSG::DEBUG) eflowStream << MSG::DEBUG << "This CaloTopoCluster element link is not valid" << endreq;

  }//cluster loop

}


void eflowObjectCnv_p5::transToPers( const eflowObject *transObj,eflowObject_p5 *persObj, MsgStream &eflowStream ){

  //data

  persObj->m_eflowType = transObj->m_eflowType;
  persObj->m_charge = transObj->m_charge;
  persObj->m_recoStatus = transObj->m_recoStatus.m_status;
  persObj->m_pi0MVA = transObj->m_pi0MVA;
  persObj->m_centerMag = transObj->m_centerMag;

  persObj->m_eta = transObj->eta();
  persObj->m_phi = transObj->phi();
  persObj->m_e = transObj->energy();
  persObj->m_m = transObj->m();

  //element links data


  //if (transObj->m_muonElementLink.isValid()) m_elMuCnv.transToPers(&transObj->m_muonElementLink, &persObj->m_muonElementLink, eflowStream);
  //if (transObj->m_convElementLink.isValid()) m_elVxCnv.transToPers(&transObj->m_convElementLink, &persObj->m_convElementLink, eflowStream);

  //navigable vectors
  //there are no converters so we have to make an ElementLinkVector first for which converters exist

  ElementLinkVector<Rec::TrackParticleContainer> *dummyTrackVector = new  ElementLinkVector<Rec::TrackParticleContainer>();

  for (int i = 0; i < transObj->numTrack();i++) {
    if (transObj->m_eflowTrack.m_store[i].isValid()) dummyTrackVector->push_back(transObj->m_eflowTrack.m_store[i]);
    else if (eflowStream.level() <= MSG::DEBUG) eflowStream << MSG::DEBUG << "This TrackParticle element link is not valid" << endreq;
  }

  m_navTrackCnv.transToPers(dummyTrackVector, &persObj->m_eflowTrack, eflowStream);

  ElementLinkVector<CaloClusterContainer> *dummyClusterVector = new ElementLinkVector<CaloClusterContainer>();

  for (int i = 0; i < transObj->numClus(); i++) {
    if (transObj->m_eflowClus.m_store[i].isValid()) dummyClusterVector->push_back(transObj->m_eflowClus.m_store[i]);
    else if (eflowStream.level() <= MSG::DEBUG) eflowStream << MSG::DEBUG << "This CaloTopoCluster element link is not valid" << endreq;
  }
  
  m_navClusCnv.transToPers(dummyClusterVector, &persObj->m_eflowClus, eflowStream);

  delete dummyTrackVector;
  delete dummyClusterVector;

}
