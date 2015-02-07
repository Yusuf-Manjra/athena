/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigInDetTracklet.h"
#include "TrigInDetPattRecoTools/TrigTrackSeedGenerator.h"
#include "IRegionSelector/IRoiDescriptor.h"

TrigTrackSeedGenerator::TrigTrackSeedGenerator(const TrigCombinatorialSettings& tcs) 
  : m_settings(tcs), 
    m_maxSoaSize(1000),
    m_maxOuterRadius(550.0), 
    m_minRadius(10.0),
    m_maxRadius(600.0), 
    m_radBinWidth(2.0),
    m_minDeltaRadius(10.0), 
    m_maxDeltaRadius(270.0),
    m_zTol(3.0), 
    m_pStore(NULL)
{
  m_radLayers.clear();
  m_radLayers.resize(1+(int)(m_maxRadius/m_radBinWidth));
  m_phiSliceWidth = 2*M_PI/m_settings.m_nMaxPhiSlice;
  m_phiSlices.resize(m_settings.m_nMaxPhiSlice);
  m_nMaxRadBin = 1+(int)((m_maxRadius-m_minRadius)/m_radBinWidth);
  m_pStore = new PHI_R_STORAGE(m_settings.m_nMaxPhiSlice, m_nMaxRadBin);

  //mult scatt. variance for doublet matching
  const double radLen = 0.036;
  m_CovMS = std::pow((13.6/m_settings.m_tripletPtMin),2)*radLen;
  m_ptCoeff = 0.29997*m_settings.m_magFieldZ/2;// ~0.3
  m_minPt2 = m_settings.m_tripletPtMin*m_settings.m_tripletPtMin;
}

TrigTrackSeedGenerator::~TrigTrackSeedGenerator() {
  for(INTERNAL_TRIPLET_BUFFER::iterator it=m_triplets.begin();it!=m_triplets.end();++it) {
    delete (*it).second;
  }
  m_triplets.clear();
  delete m_pStore;
}

void TrigTrackSeedGenerator::loadSpacePoints(const std::vector<TrigSiSpacePointBase>& vSP) {

  m_pStore->reset();

  for(auto l : m_radLayers) {
    l.clear();
  }
  for(std::vector<TrigSiSpacePointBase>::const_iterator it = vSP.begin();it != vSP.end();++it) {
    if((*it).r()>m_maxRadius) continue;
    unsigned int idx = (*it).r()/m_radBinWidth;
    m_radLayers[idx].push_back(&(*it));
  }
  for(auto s : m_phiSlices) {
    s.clear();
  }
  for(std::vector<std::vector<const TrigSiSpacePointBase*> >::iterator lIt=m_radLayers.begin();lIt!=m_radLayers.end();++lIt) {
    if((*lIt).empty()) continue;
    for(std::vector<const TrigSiSpacePointBase*>::iterator rIt=(*lIt).begin();rIt!=(*lIt).end();++rIt) {
      int idx = ((*rIt)->phi()+M_PI)/m_phiSliceWidth;
      if(idx<0 || idx>=m_settings.m_nMaxPhiSlice) continue; 
      m_phiSlices[idx].push_back(*rIt);
    }
  }
  for(std::vector<TrigSiSpacePointBase>::const_iterator it = vSP.begin();it != vSP.end();++it) {
    if((*it).r()>m_maxRadius || (*it).r() < m_minRadius) continue;
    int rIdx = ((*it).r()-m_minRadius)/m_radBinWidth;
    int phiIdx = ((*it).phi()+M_PI)/m_phiSliceWidth;
    m_pStore->addSpacePoint(phiIdx, rIdx, &(*it));
  }
  m_pStore->sortSpacePoints();

  m_SoA.reserveSpacePoints(vSP.size());
}

void TrigTrackSeedGenerator::createSeeds() {


  float zMinus = m_settings.roiDescriptor->zedMinus() - m_zTol;
  float zPlus  = m_settings.roiDescriptor->zedPlus() + m_zTol;
  
  for(INTERNAL_TRIPLET_BUFFER::reverse_iterator it=m_triplets.rbegin();it!=m_triplets.rend();++it) {
    delete (*it).second;
  }
  m_triplets.clear();

  for(int phiIdx=0;phiIdx<m_settings.m_nMaxPhiSlice;phiIdx++) {

    const PHI_SECTOR& S = m_pStore->m_phiSectors[phiIdx];

    if(S.m_nSP==0) continue;

    std::vector<int> phiVec;
    phiVec.reserve(4);

    phiVec.push_back(phiIdx);

    if(phiIdx>0) phiVec.push_back(phiIdx-1);
    else phiVec.push_back(m_settings.m_nMaxPhiSlice-1);

    if(phiIdx<m_settings.m_nMaxPhiSlice-1) phiVec.push_back(phiIdx+1);
    else phiVec.push_back(0);
    
    //Remove duplicates
    std::sort(phiVec.begin(), phiVec.end());
    phiVec.erase(std::unique(phiVec.begin(), phiVec.end()), phiVec.end());


    for(int radIdx=0;radIdx<m_nMaxRadBin;radIdx++) {

      if(S.m_radBins[radIdx].empty()) continue;

      for(auto& spm : S.m_radBins[radIdx]) {//loop over middle spacepoint

        float zm = spm->z();
        float rm = spm->r();
        bool isSct = (spm->isSCT());

        const std::pair<IdentifierHash, IdentifierHash>& deIds = spm->offlineSpacePoint()->elementIdList();

        int nSP=0;
        int nInner=0;
        int nOuter=0;

        for(int innRadIdx=radIdx-1;innRadIdx>=0;innRadIdx--) {

          float refRad = m_minRadius +  m_radBinWidth*(0.5+innRadIdx);

          float dR = rm - refRad;

          if(dR<m_minDeltaRadius) continue;
          if(dR>m_maxDeltaRadius) break;

          float minZ = zMinus + refRad*(zm - zMinus)/rm;
          float maxZ = zPlus + refRad*(zm - zPlus)/rm;

          for(auto& innPhiIdx : phiVec) {

            const std::vector<const TrigSiSpacePointBase*>& spVec = m_pStore->m_phiSectors[innPhiIdx].m_radBins[innRadIdx];

            if(spVec.empty()) continue;

            std::vector<const TrigSiSpacePointBase*>::const_iterator it1 = std::lower_bound(spVec.begin(), spVec.end(), minZ, PHI_SECTOR::smallerThanZ());
            std::vector<const TrigSiSpacePointBase*>::const_iterator it2 = std::upper_bound(spVec.begin(), spVec.end(), maxZ, PHI_SECTOR::greaterThanZ());

            if(std::distance(it1, it2)==0) continue;

            for(std::vector<const TrigSiSpacePointBase*>::const_iterator spIt= it1; spIt!=it2; ++spIt) {

              if(deIds == (*spIt)->offlineSpacePoint()->elementIdList()) continue;

              if (!m_settings.m_tripletDoPSS) {//Allow PSS seeds?
                if(isSct && ((*spIt)->isPixel())) continue;
              }

              double tau = (zm - (*spIt)->z())/(rm - (*spIt)->r());
              double z0  = zm - rm*tau;

              if (!m_settings.roiDescriptor->contains(z0, tau)) {
                continue;
              }

              m_SoA.m_sp.push_back(*spIt);
              nSP++;
            }
          }
        }

        nInner = nSP;

        //std::cout<<"middle sp : r="<<rm<<" phi="<<spm->phi()<<" z="<<zm<<" has "<<innerSPs.size()<<" inner neighbours"<<std::endl;

        for(int outRadIdx=radIdx+1;(outRadIdx<m_nMaxRadBin) && (nSP<m_maxSoaSize);outRadIdx++) {

          float refRad = m_minRadius +  m_radBinWidth*(0.5+outRadIdx);

          float dR = refRad - rm;

          if(dR<m_minDeltaRadius) continue;
          if(dR>m_maxDeltaRadius) break;

          float maxZ = zMinus + refRad*(zm - zMinus)/rm;
          float minZ = zPlus + refRad*(zm - zPlus)/rm;

          for(auto& outPhiIdx : phiVec) {

            const std::vector<const TrigSiSpacePointBase*>& spVec = m_pStore->m_phiSectors[outPhiIdx].m_radBins[outRadIdx];

            if(spVec.empty()) continue;

            std::vector<const TrigSiSpacePointBase*>::const_iterator it1 = std::lower_bound(spVec.begin(), spVec.end(), minZ, PHI_SECTOR::smallerThanZ());
            std::vector<const TrigSiSpacePointBase*>::const_iterator it2 = std::upper_bound(spVec.begin(), spVec.end(), maxZ, PHI_SECTOR::greaterThanZ());

            if(std::distance(it1, it2)==0) continue;

            for(std::vector<const TrigSiSpacePointBase*>::const_iterator spIt= it1; spIt!=it2; ++spIt) {

              if(deIds == (*spIt)->offlineSpacePoint()->elementIdList()) continue;

              double tau = (zm - (*spIt)->z())/(rm - (*spIt)->r());
              double z0  = zm - rm*tau;

              if (!m_settings.roiDescriptor->contains(z0, tau)) {
                continue;
              }

              m_SoA.m_sp.push_back(*spIt);
              nSP++;
            }
          }
        }

        nOuter = nSP-nInner;

        //std::cout<<"middle sp : r="<<rm<<" phi="<<spm->phi()<<" z="<<zm<<" has "<<outerSPs.size()<<" outer neighbours"<<std::endl;

        if(nInner != 0 && nOuter != 0) {
          INTERNAL_TRIPLET_BUFFER tripletMap;

          createTriplets(spm, nInner, nOuter, tripletMap);

          if(!tripletMap.empty()) storeTriplets(tripletMap);	
          tripletMap.clear();
        }
        else {
          m_SoA.m_sp.clear(); 
          continue;
        }


      }//loop over spacepoints in phi-r bin
    }
  }

  /*
  for(auto& tm : m_triplets) {
    std::cout<<"Q="<<tm.first<<" barcodes: "<<tm.second->s1().barCode()<<" "<<
      tm.second->s2().barCode()<<" "<<tm.second->s3().barCode()<<std::endl;
  }
  */
}


void TrigTrackSeedGenerator::createTriplets(const TrigSiSpacePointBase* pS, int nInner, int nOuter,
					    INTERNAL_TRIPLET_BUFFER& output) {

  

  output.clear();
  if(nInner==0 || nOuter==0) return;

  int nSP = nInner + nOuter;

  double cosA = pS->x()/pS->r();
  double sinA = pS->y()/pS->r();
  double covZ = pS->dz()*pS->dz();
  double covR = pS->dr()*pS->dr();

  m_SoA.resizeComponents();//Resize m_r, mu etc to m_sp size

  for(int idx=0;idx<nSP;idx++) {
    const TrigSiSpacePointBase* pSP = m_SoA.m_sp[idx];
    
    //1. transform in the pS-centric c.s

    double dx = pSP->x() - pS->x();
    double dy = pSP->y() - pS->y();
    double R2inv = 1.0/(dx*dx+dy*dy);
    double Rinv = sqrt(R2inv);
    double xn = dx*cosA + dy*sinA;
    double yn =-dx*sinA + dy*cosA;
    double dz = (idx<nInner) ? pSP->z() - pS->z() : -pSP->z() + pS->z(); 
    double t = Rinv*dz;

    //2. Conformal mapping

    m_SoA.m_r[idx] = Rinv;
    m_SoA.m_u[idx] = xn*R2inv;
    m_SoA.m_v[idx] = yn*R2inv;
    
    //3. cot(theta) estimation for the doublet

    double covZP = pSP->dz()*pSP->dz();
    double covRP = pSP->dr()*pSP->dr();
    
    m_SoA.m_t[idx] = t;
    m_SoA.m_tCov[idx] = R2inv*(covZ + covZP + t*t*(covR+covRP));
  }

  bool isPixel = (pS->isPixel());

  //double loop

  for(int innIdx=0;innIdx<nInner;innIdx++) {

    //mult. scatt contribution due to the layer with middle SP
    
    double dCov = m_CovMS*(1+m_SoA.m_t[innIdx]*m_SoA.m_t[innIdx]);

    double z0 = pS->z() - m_SoA.m_t[innIdx]*pS->r();

    for(int outIdx=nInner;outIdx<nSP;outIdx++) {
      /*
      std::cout<<"Tr: "<<m_SoA.m_sp[innIdx]->offlineSpacePoint()->r()<<" "<<m_SoA.m_sp[innIdx]->offlineSpacePoint()->phi()
	       <<" "<<m_SoA.m_sp[innIdx]->offlineSpacePoint()->globalPosition().z()<<" | ";
      std::cout<<pS->offlineSpacePoint()->r()<<" "<<pS->offlineSpacePoint()->phi()<<" "<<pS->offlineSpacePoint()->globalPosition().z()<<" | ";
      std::cout<<m_SoA.m_sp[outIdx]->offlineSpacePoint()->r()<<" "<<m_SoA.m_sp[outIdx]->offlineSpacePoint()->phi()<<" "
	       <<m_SoA.m_sp[outIdx]->offlineSpacePoint()->globalPosition().z()<<std::endl;
      */

      //1. rz doublet matching

      double dt = m_SoA.m_t[innIdx] - m_SoA.m_t[outIdx];

      double covdt = m_SoA.m_tCov[innIdx] + m_SoA.m_tCov[outIdx];
      covdt += 2*m_SoA.m_r[innIdx]*m_SoA.m_r[outIdx]*(m_SoA.m_t[innIdx]*m_SoA.m_t[outIdx]*covR + covZ);

      double dt2 = dt*dt/9.0;//i.e. 3-sigma cut

      if(dt2 > covdt+dCov) continue;

      //2. pT estimate

      double du = m_SoA.m_u[outIdx] - m_SoA.m_u[innIdx];
      if(du==0.0) continue;
      double A = (m_SoA.m_v[outIdx] - m_SoA.m_v[innIdx])/du;
      double B = m_SoA.m_v[innIdx] - A*m_SoA.m_u[innIdx];
      double pT2 = m_ptCoeff*m_ptCoeff*(1+A*A)/(B*B);

      if(pT2 < m_minPt2) continue;

      //3. the 3-sigma cut with estimated pT

      double frac = m_minPt2/pT2;
      if(dt2 > covdt+frac*dCov) continue;

      //4. d0 cut

      double d0 = pS->r()*(B*pS->r()-A);

      if(fabs(d0) > m_settings.m_tripletD0Max) continue;
      
      if (m_SoA.m_sp[outIdx]->isSCT() && isPixel) {
        if(fabs(d0) > m_settings.m_tripletD0_PPS_Max) continue;
      }

      //5. phi0 cut

      double uc = 2*B*pS->r() - A;
      double phi0 = atan2(sinA - uc*cosA, cosA + uc*sinA);

      if(!m_settings.roiDescriptor->containsPhi(phi0)) {
        continue;
      }

      //6. add new triplet

      double Q = d0*d0;
      if(output.size()>=m_settings.m_maxTripletBufferLength) {
        INTERNAL_TRIPLET_BUFFER::iterator it = output.begin();
        if( Q >= (*it).first) continue;
        delete (*it).second;
        output.erase(it);
      }

      TrigInDetTriplet* t = new TrigInDetTriplet(*m_SoA.m_sp[innIdx], *pS, *m_SoA.m_sp[outIdx], 
						 m_SoA.m_t[innIdx], phi0, z0, d0, sqrt(pT2), Q);


      output.insert(std::pair<double, TrigInDetTriplet*>(Q,t));
    }
  }
  m_SoA.clear();
}


void TrigTrackSeedGenerator::storeTriplets(INTERNAL_TRIPLET_BUFFER& tripletMap) {
  for(INTERNAL_TRIPLET_BUFFER::iterator it=tripletMap.begin();it!=tripletMap.end();++it) {
    double Q = (*it).first;
    if((*it).second->s3().isSCT()) {
      Q += (*it).second->s1().isSCT() ? 1000.0 : 10000.0;
    }
    m_triplets.insert(std::pair<double, TrigInDetTriplet*>(Q, (*it).second));
  }
}

void TrigTrackSeedGenerator::getSeeds(std::vector<TrigInDetTriplet*>& vs) {
  vs.clear();
  vs.reserve(m_triplets.size());
  for(INTERNAL_TRIPLET_BUFFER::reverse_iterator it=m_triplets.rbegin();it!=m_triplets.rend();++it) {
    vs.push_back((*it).second);
    (*it).second = NULL;//ownership transferred
  }
  m_triplets.clear();
}
