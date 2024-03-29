/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigInDetTriplet.h"
#include "TrigInDetPattRecoEvent/TrigInDetSiLayer.h"
#include "TrigInDetPattRecoTools/TrigTrackSeedGenerator.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/RoiUtil.h"
#include "InDetPrepRawData/PixelCluster.h"

TrigTrackSeedGenerator::TrigTrackSeedGenerator(const TrigCombinatorialSettings& tcs) 
  : m_settings(tcs), 
    m_minDeltaRadius(10.0), 
    m_zTol(3.0), 
    m_pStore(NULL)
{
  m_maxDeltaRadius = m_settings.m_doublet_dR_Max;
  m_maxDeltaRadiusConf = m_settings.m_doublet_dR_Max_Confirm;
  m_phiSliceWidth = 2*M_PI/m_settings.m_nMaxPhiSlice;
  m_pStore = new L_PHI_STORAGE(m_settings.m_nMaxPhiSlice, (int)m_settings.m_layerGeometry.size());

  //mult scatt. variance for doublet matching
  const double radLen = 0.036;
  m_CovMS = std::pow((13.6/m_settings.m_tripletPtMin),2)*radLen;
  const double ptCoeff = 0.29997*1.9972/2.0;// ~0.3*B/2 - assumes nominal field of 2*T
  m_minR_squ = m_settings.m_tripletPtMin*m_settings.m_tripletPtMin/std::pow(ptCoeff,2);
  m_dtPreCut = std::tan(2.0*m_settings.m_tripletDtCut*std::sqrt(m_CovMS));
}

TrigTrackSeedGenerator::~TrigTrackSeedGenerator() {
  delete m_pStore;

  m_SoA.clear();

}

void TrigTrackSeedGenerator::loadSpacePoints(const std::vector<TrigSiSpacePointBase>& vSP) {

  m_pStore->reset();

  m_SoA.clear();

  m_spStorage.clear();

  m_spStorage.resize(vSP.size());

  m_minTau.resize(vSP.size(), 0.0);
  m_maxTau.resize(vSP.size(), 100.0);

  int spIndex = 0;
  
  for(std::vector<TrigSiSpacePointBase>::const_iterator it = vSP.begin();it != vSP.end();++it) {
    int layerId = (*it).layer();

    bool isPixel = (m_settings.m_layerGeometry[layerId].m_subdet == 1);
    bool isEndcap = (m_settings.m_layerGeometry[layerId].m_type != 0);

    bool updateTauRange = false;
    float minTau = 0.0;
    float maxTau = 100.0;

    if((m_settings.m_useTrigSeedML > 0) && isPixel) {
      const Trk::SpacePoint* osp = (*it).offlineSpacePoint();
      const InDet::PixelCluster* pCL = dynamic_cast<const InDet::PixelCluster*>(osp->clusterList().first);

      float cluster_width = pCL->width().widthPhiRZ().y();
      if(isEndcap) {
	if(cluster_width > m_settings.m_maxEC_len) continue;
      }
      else {//Barrel
	if(!m_settings.m_vLUT.empty()) {
	  const TrigSeedML_LUT& LUT = (*m_settings.m_vLUT.begin());
	  updateTauRange = LUT.getValidRange(cluster_width, minTau, maxTau);
	}
      }
    }
    
    int phiIdx = ((*it).phi()+M_PI)/m_phiSliceWidth;
    if (phiIdx >= m_settings.m_nMaxPhiSlice) {
      phiIdx %= m_settings.m_nMaxPhiSlice;
    }
    else if (phiIdx < 0) {
      phiIdx += m_settings.m_nMaxPhiSlice;
      phiIdx %= m_settings.m_nMaxPhiSlice;
    }
    m_spStorage[spIndex].set(&(*it), spIndex);
    m_pStore->addSpacePoint(phiIdx, layerId, &m_spStorage[spIndex]);

    if(updateTauRange) {
      m_minTau[spIndex] = minTau;
      m_maxTau[spIndex] = maxTau;
    }
    spIndex++;

  }

  m_pStore->sortSpacePoints2(m_settings.m_layerGeometry);

  m_SoA.resize(vSP.size());


}

void TrigTrackSeedGenerator::createSeeds(const IRoiDescriptor* roiDescriptor) {

  m_zMinus = roiDescriptor->zedMinus() - m_zTol;
  m_zPlus  = roiDescriptor->zedPlus() + m_zTol;
  
  m_triplets.clear();

  int nLayers = (int) m_settings.m_layerGeometry.size();

  
  //TO-DO: create vector of MIDDLE layers and for each middle layer create a list of 
  //ALLOWED layers for doublet making
  //for example coaxial barrel layers should be separated by more than DR_Max
  // no simultaneous +/- or -/+ endcaps

  m_innerMarkers.clear();
  m_outerMarkers.clear();
  
  for(int layerI=1;layerI<nLayers;layerI++) {//skip layer 0 for middle spacepoint search

    const L_PHI_SECTOR& S = m_pStore->m_layers[layerI];
    if(S.m_nSP==0) continue;

    bool isSct = (m_settings.m_layerGeometry[layerI].m_subdet == 2);
    
    for(int phiI=0;phiI<m_settings.m_nMaxPhiSlice;phiI++) {

      for(auto spm : S.m_phiSlices[phiI]) {//loop over middle spacepoints

	float zm = spm->m_pSP->z();
	float rm = spm->m_pSP->r();
	
	m_nInner = 0;
	m_nOuter = 0;

	m_innerMarkers.clear();
	m_outerMarkers.clear();

	for(int layerJ=0;layerJ<nLayers;layerJ++) {

	  bool isPixel2 = (m_settings.m_layerGeometry[layerJ].m_subdet == 1);
	  
	  if(isSct && isPixel2 && (!m_settings.m_tripletDoPSS)) continue;//no mixed PSS seeds allowed

	  if((!isSct) && (!isPixel2)) {// i.e. xPS (or SPx)
	    if((!m_settings.m_tripletDoConfirm) && (!m_settings.m_tripletDoPPS)) {
	      continue;//no mixed PPS seeds allowed
	    }
	    //but if m_tripletDoConfirm is true we will use PPS seeds to confirm PPP seeds
	  }
	  
	  if(!validateLayerPairNew(layerI, layerJ, rm, zm)) continue; 
	    
	  bool checkPSS = (!m_settings.m_tripletDoPSS) && (isSct && isPixel2);
	  
	  const std::vector<const INDEXED_SP*>& spVec = m_pStore->m_layers[layerJ].m_phiThreeSlices.at(phiI);

	  if(spVec.empty()) continue;

	  SP_RANGE delta;

	  if(!getSpacepointRange(layerJ, spVec, delta)) continue;

	  int nI = m_nInner;
	  int nO = m_nOuter;
	      
	  processSpacepointRange(layerJ, spm, checkPSS, delta, roiDescriptor);

	  if(m_nInner > nI) m_innerMarkers.push_back(m_nInner);
	  if(m_nOuter > nO) m_outerMarkers.push_back(m_nOuter);


	}//loop over inner/outer layers
	
	if(m_nInner != 0 && m_nOuter != 0) {
	  std::vector<TrigInDetTriplet> tripletVec;
	  
	  if(m_settings.m_tripletDoConfirm) {
	    if(!isSct) {
	      createConfirmedTriplets(spm->m_pSP, m_nInner, m_nOuter, tripletVec, roiDescriptor);
	    }
	    else createTripletsNew(spm->m_pSP, m_nInner, m_nOuter, tripletVec, roiDescriptor);
	  }
	  else createTripletsNew(spm->m_pSP, m_nInner, m_nOuter, tripletVec, roiDescriptor);
	  
	  if(!tripletVec.empty()) storeTriplets(tripletVec);	
	  tripletVec.clear();
	}
	else continue;
      }
    }
  }

}

void TrigTrackSeedGenerator::createSeeds(const IRoiDescriptor* roiDescriptor, const std::vector<float>& vZv) {
  
  m_triplets.clear();

  if(vZv.empty()) return;

  int nLayers = (int) m_settings.m_layerGeometry.size();

 
  for(int layerI=1;layerI<nLayers;layerI++) {//skip layer 0 for middle spacepoint search

    const L_PHI_SECTOR& S = m_pStore->m_layers[layerI];
    if(S.m_nSP==0) continue;

    bool isSct = (m_settings.m_layerGeometry[layerI].m_subdet == 2);
    bool isBarrel = (m_settings.m_layerGeometry[layerI].m_type == 0);
    
    bool checkWidth = isBarrel && (!isSct) && (m_settings.m_useTrigSeedML > 0);
    
    for(int phiI=0;phiI<m_settings.m_nMaxPhiSlice;phiI++) {

	std::vector<int> phiVec;
	phiVec.reserve(4);

	phiVec.push_back(phiI);

	if(phiI>0) phiVec.push_back(phiI-1);
	else phiVec.push_back(m_settings.m_nMaxPhiSlice-1);

	if(phiI<m_settings.m_nMaxPhiSlice-1) phiVec.push_back(phiI+1);
	else phiVec.push_back(0);
    
	//Remove duplicates
	std::sort(phiVec.begin(), phiVec.end());
	phiVec.erase(std::unique(phiVec.begin(), phiVec.end()), phiVec.end());

	for(auto spm : S.m_phiSlices[phiI]) {//loop over middle spacepoints

	  float zm = spm->m_pSP->z();
	  float rm = spm->m_pSP->r();

	  float minTau = 0.0;
	  float maxTau = 100.0;

	  if(checkWidth) {
	    minTau = m_minTau[spm->m_idx];
	    maxTau = m_maxTau[spm->m_idx];
	  }
	  
	  std::vector<TrigInDetTriplet> tripletVec;

	  for(const auto zVertex : vZv) {//loop over zvertices

	    m_nInner = 0;
	    m_nOuter = 0;

	    m_zMinus = zVertex - m_settings.m_zvError;
	    m_zPlus = zVertex + m_settings.m_zvError;

	    for(int layerJ=0;layerJ<nLayers;layerJ++) {//loop over other layers

	      bool isPixel2 = (m_settings.m_layerGeometry[layerJ].m_subdet == 1);

	      if((!m_settings.m_tripletDoPSS) && (!m_settings.m_tripletDoPPS)) {//no mixed seeds allowed
		if(isSct && isPixel2) continue;//no PSx
		if((!isSct) && (!isPixel2)) continue;//no xPS
	      }

	      if(!validateLayerPairNew(layerI, layerJ, rm, zm)) continue; 
	      
	      bool checkPSS = (!m_settings.m_tripletDoPSS) && (isSct && isPixel2);

	      for(auto phiJ : phiVec) {
		
		const std::vector<const INDEXED_SP*>& spVec = m_pStore->m_layers[layerJ].m_phiSlices.at(phiJ);
		
		if(spVec.empty()) continue;

		SP_RANGE delta;

		if(!getSpacepointRange(layerJ, spVec, delta)) continue;
	      
		processSpacepointRangeZv(spm, checkPSS, delta, checkWidth, minTau, maxTau);
	      }//loop over phi bins

	    }//loop over inner/outer layers
	    if(m_nInner != 0 && m_nOuter != 0) {
	      createTriplets(spm->m_pSP, m_nInner, m_nOuter, tripletVec, roiDescriptor);
	    }
	  }//loop over zvertices
	  if(!tripletVec.empty()) storeTriplets(tripletVec);
	  tripletVec.clear();
	}
    }
  }


}

bool TrigTrackSeedGenerator::validateLayerPairNew(int layerI, int layerJ, float rm, float zm) {

  const float deltaRefCoord = 5.0;

  if(layerJ==layerI) return false;//skip the same layer ???

  if(m_pStore->m_layers[layerJ].m_nSP==0) return false;

  int typeI = m_settings.m_layerGeometry[layerI].m_type;

  float refCoordI = m_settings.m_layerGeometry[layerI].m_refCoord;

  int typeJ = m_settings.m_layerGeometry[layerJ].m_type;

  float refCoordJ = m_settings.m_layerGeometry[layerJ].m_refCoord;

  if((typeI!=0) && (typeJ!=0) && refCoordI*refCoordJ<0.0) return false;//only ++ or -- EC combinations are allowed

  //project beamline interval to the ref. coord of the layer

  bool isBarrel = (typeJ == 0);

  if(isBarrel && std::fabs(refCoordJ-rm)>m_maxDeltaRadius) return false;

  //boundaries for nextLayer

  m_minCoord = 10000.0;
  m_maxCoord =-10000.0;

  if(isBarrel) {

    if(refCoordJ<rm){//inner layer

      m_minCoord = m_zMinus + refCoordJ*(zm-m_zMinus)/rm;//+deltaRefCoord
      m_maxCoord = m_zPlus + refCoordJ*(zm-m_zPlus)/rm;//-deltaRefCoord
      m_minCoord -= deltaRefCoord*std::fabs(zm-m_zMinus)/rm;//corrections due to the layer width
      m_maxCoord += deltaRefCoord*std::fabs(zm-m_zPlus)/rm;
    }
    else {//outer layer

      m_minCoord = m_zPlus + refCoordJ*(zm-m_zPlus)/rm;//+deltaRefCoord
      m_maxCoord = m_zMinus + refCoordJ*(zm-m_zMinus)/rm;//-deltaRefCoord
      m_minCoord -= deltaRefCoord*std::fabs(zm-m_zPlus)/rm;
      m_maxCoord += deltaRefCoord*std::fabs(zm-m_zMinus)/rm;
    }
  }
  else {//endcap
    float maxB =m_settings.m_layerGeometry[layerJ].m_maxBound;
    float minB =m_settings.m_layerGeometry[layerJ].m_minBound;
    if(maxB<=rm) return false;// This currently rejects SP type doublets
                              // Could correct this by retrieving if layers are pix or SCT, and not performing this check for SCt->pix doublets
    if(refCoordJ>0) {//positive EC
      if(refCoordJ > zm) {//outer layer
	
	if(zm < m_zMinus) return false;
	if(zm == m_zPlus) return false;
	float zMax = (zm*maxB-rm*refCoordJ)/(maxB-rm);
	if( m_zMinus > zMax) return false;
	if (rm < minB) {
	  float zMin = (zm*minB-rm*refCoordJ)/(minB-rm);
	  if(m_zPlus<zMin) return false;
	}

	m_minCoord = (refCoordJ-m_zMinus)*rm/(zm-m_zMinus);
	m_maxCoord = (refCoordJ-m_zPlus)*rm/(zm-m_zPlus);
	m_minCoord -= deltaRefCoord*rm/std::fabs(zm-m_zMinus);
	m_maxCoord += deltaRefCoord*rm/std::fabs(zm-m_zPlus);

	if(zm <= m_zPlus) m_maxCoord = maxB;
	
	if(m_minCoord > maxB) return false;
	if(m_maxCoord < minB) return false;
	
      }
      else {//inner layer
        if(minB == rm) return false;
	float zMax = (zm*minB-rm*refCoordJ)/(minB-rm);
	if( m_zMinus > zMax) return false;
	if (rm>maxB) {// otherwise, intersect of line from maxB through middle sp will be on the wrong side of the layer
	  float zMin = (zm*maxB-rm*refCoordJ)/(maxB-rm);
	  if(m_zPlus<zMin) return false;
	}
	if(zm == m_zPlus || zm == m_zMinus) return false;
	m_minCoord = (refCoordJ-m_zPlus)*rm/(zm-m_zPlus);
	m_maxCoord = (refCoordJ-m_zMinus)*rm/(zm-m_zMinus);
	m_minCoord -= deltaRefCoord*rm/std::fabs(zm-m_zPlus);
	m_maxCoord += deltaRefCoord*rm/std::fabs(zm-m_zMinus);
      }
    }
    else {//negative EC
      if(refCoordJ < zm) {//outer layer

	if(zm > m_zPlus) return false;
	if(zm == m_zMinus) return false;
	float zMin = (zm*maxB-rm*refCoordJ)/(maxB-rm);
	if( m_zPlus < zMin) return false;
	if (rm<minB) {// otherwise, intersect of line from minB through middle sp will be on the wrong side of the layer
	  float zMax = (zm*minB-rm*refCoordJ)/(minB-rm);
	  if(m_zMinus>zMax) return false;
	}

	m_minCoord = (refCoordJ-m_zPlus)*rm/(zm-m_zPlus);
	m_maxCoord = (refCoordJ-m_zMinus)*rm/(zm-m_zMinus);
	m_minCoord -= deltaRefCoord*rm/std::fabs(zm-m_zPlus);
	m_maxCoord += deltaRefCoord*rm/std::fabs(zm-m_zMinus);
	if(zm > m_zMinus) m_maxCoord = maxB;	
	if(m_minCoord > maxB) return false;
	if(m_maxCoord < minB) return false;

      }
      else {//inner layer
        if(minB == rm) return false;
	float zMin = (zm*minB-rm*refCoordJ)/(minB-rm);
	if( m_zPlus < zMin) return false;
	if (rm>maxB) {// otherwise, intersect of line from maxB through middle sp will be on the wrong side of the layer
	  float zMax = (zm*maxB-rm*refCoordJ)/(maxB-rm);
	  if(m_zMinus>zMax) return false;
	}
	if(zm == m_zPlus || zm == m_zMinus) return false;
	m_minCoord = (refCoordJ-m_zMinus)*rm/(zm-m_zMinus);
	m_maxCoord = (refCoordJ-m_zPlus)*rm/(zm-m_zPlus);
	m_minCoord -= deltaRefCoord*rm/std::fabs(zm-m_zMinus);
	m_maxCoord += deltaRefCoord*rm/std::fabs(zm-m_zPlus);

      }
    }
  }
  float minBoundJ = m_settings.m_layerGeometry[layerJ].m_minBound;
  float maxBoundJ = m_settings.m_layerGeometry[layerJ].m_maxBound;
  if(maxBoundJ<m_minCoord || minBoundJ>m_maxCoord) return false;
  return true;
}



bool TrigTrackSeedGenerator::getSpacepointRange(int lJ, const std::vector<const INDEXED_SP*>& spVec, SP_RANGE& delta) {
  
  int typeJ = m_settings.m_layerGeometry[lJ].m_type;
  bool isBarrel = (typeJ == 0);
  bool isPositiveEC = m_settings.m_layerGeometry[lJ].m_refCoord > 0; 
  float minSpCoord = (isBarrel) ? (*spVec.begin())->m_pSP->z() : (*spVec.begin())->m_pSP->r();
  float maxSpCoord = (isBarrel) ? (*spVec.rbegin())->m_pSP->z() : (*spVec.rbegin())->m_pSP->r();

  if(!isBarrel && isPositiveEC) {//forward endcap SPs are sorted differently
    float tmp = minSpCoord;minSpCoord = maxSpCoord;maxSpCoord = tmp;
  }

  if(minSpCoord > m_maxCoord) return false;
  if(maxSpCoord < m_minCoord) return false;

  //identify the range of spacepoints in the layer

  std::vector<const INDEXED_SP*>::const_iterator it1 = spVec.end();
  std::vector<const INDEXED_SP*>::const_iterator it2 = spVec.end();
  
  if(isBarrel) {
    it1 = std::lower_bound(spVec.begin(), spVec.end(), m_minCoord, L_PHI_SECTOR::smallerThanZ());
    it2 = std::upper_bound(spVec.begin(), spVec.end(), m_maxCoord, L_PHI_SECTOR::greaterThanZ());
  }
  else {
    if(isPositiveEC) {
      it1 = std::lower_bound(spVec.begin(), spVec.end(), m_maxCoord, L_PHI_SECTOR::greaterThanR_i());
      it2 = std::upper_bound(spVec.begin(), spVec.end(), m_minCoord, L_PHI_SECTOR::smallerThanR_i());
    } 
    else {
      it1 = std::lower_bound(spVec.begin(), spVec.end(), m_minCoord, L_PHI_SECTOR::smallerThanR());
      it2 = std::upper_bound(spVec.begin(), spVec.end(), m_maxCoord, L_PHI_SECTOR::greaterThanR());
    }
  }
  
  if(std::distance(it1, it2)==0) return false;

  delta.first = it1;delta.second = it2;
  return true;
}

int TrigTrackSeedGenerator::processSpacepointRange(int lJ, const INDEXED_SP* spm, bool checkPSS, const SP_RANGE& delta, const IRoiDescriptor* roiDescriptor) {

  int nSP=0;

  bool isBarrel = (m_settings.m_layerGeometry[lJ].m_type==0);
  float refCoord = m_settings.m_layerGeometry[lJ].m_refCoord;
  bool isPixel = spm->m_pSP->isPixel();
  
  float rm = spm->m_pSP->r();
  float zm = spm->m_pSP->z();

  float dZ = refCoord-zm;
  float dR_i = isBarrel ? 1.0/(refCoord-rm) : 1.0;

  int SeedML = m_settings.m_useTrigSeedML;

  for(std::vector<const INDEXED_SP*>::const_iterator spIt=delta.first; spIt!=delta.second; ++spIt) {
    // if(deIds == (*spIt)->offlineSpacePoint()->elementIdList()) continue;
    float zsp = (*spIt)->m_pSP->z();
    float rsp = (*spIt)->m_pSP->r();
    
    float dr = rsp - rm;
    
    if(std::fabs(dr)>m_maxDeltaRadius ) continue;

    if(m_settings.m_tripletDoConfirm) {
      if(isPixel && !(*spIt)->m_pSP->isPixel()) {
	if(std::fabs(dr)>m_maxDeltaRadiusConf ) continue;
      }
    }

    if(std::fabs(dr)<m_minDeltaRadius ) continue;
    
    //inner doublet check
    
    if (dr<0 && checkPSS) continue;//Allow PSS seeds ?  
    
    float dz = zsp - zm;
    float tau = dz/dr;
    float ftau = std::fabs(tau);
    if (ftau > 7.41) continue;
    
    if(isPixel && SeedML > 0) {
      if(ftau < m_minTau[spm->m_idx] || ftau > m_maxTau[spm->m_idx]) continue;
    }

    
    float z0  = zm - rm*tau;
    if (m_settings.m_doubletFilterRZ) {
      if (!RoiUtil::contains( *roiDescriptor, z0, tau)) continue;
    }
    
    float t = isBarrel ? dz*dR_i : dZ/dr;
    
    if(dr<0) {
      if(isBarrel && (*spIt)->m_pSP->isPixel()) {
	if(SeedML == 3 || SeedML == 4) {
	  if(ftau < m_minTau[(*spIt)->m_idx] || ftau > m_maxTau[(*spIt)->m_idx]) continue;
	}
      }
      m_SoA.m_spi[m_nInner] = (*spIt)->m_pSP;
      m_SoA.m_ti[m_nInner] = t;
      m_nInner++;
    }
    else {
      if(isBarrel && (*spIt)->m_pSP->isPixel()) {
	if(SeedML == 2 || SeedML == 4) {
	  if(ftau < m_minTau[(*spIt)->m_idx] || ftau > m_maxTau[(*spIt)->m_idx]) continue;
	}
      }
      m_SoA.m_spo[m_nOuter] = (*spIt)->m_pSP;
      m_SoA.m_to[m_nOuter] = t;
      m_nOuter++;
    }
    nSP++;  
  }

  return nSP;
}

int TrigTrackSeedGenerator::processSpacepointRangeZv(const INDEXED_SP* spm, bool checkPSS, const SP_RANGE& delta, bool checkWidth, const float& minTau, const float& maxTau) {

  int nSP=0;

  float rm = spm->m_pSP->r();
  float zm = spm->m_pSP->z();

  int SeedML = m_settings.m_useTrigSeedML;
  
  for(std::vector<const INDEXED_SP*>::const_iterator spIt=delta.first; spIt!=delta.second; ++spIt) {

    float rsp = (*spIt)->m_pSP->r(); 
    float zsp = (*spIt)->m_pSP->z();  
    float dr = rsp - rm;
	      
    if(std::fabs(dr)>m_maxDeltaRadius ) continue;
    if(std::fabs(dr)<m_minDeltaRadius ) continue;
		
    //inner doublet check

    if (dr<0 && checkPSS) continue;//Allow PSS seeds ? 
 
    float dz = zsp - zm;
    float tau = dz/dr;
    float ftau = std::fabs(tau);
    
    if (ftau > 7.41) continue;//|eta|<2.7

    if(checkWidth) {
      if(ftau < minTau || ftau > maxTau) continue;
    }
    
    if(dr<0) {
      if((*spIt)->m_pSP->isPixel()) {
      	if(SeedML == 3 || SeedML == 4) {
	  if(ftau < m_minTau[(*spIt)->m_idx] || ftau > m_maxTau[(*spIt)->m_idx]) continue;
	}
      }
      m_SoA.m_spi[m_nInner++] = (*spIt)->m_pSP;
    }
    else {
      if((*spIt)->m_pSP->isPixel()) {
	if(SeedML == 2 || SeedML == 4) {
	  if(ftau < m_minTau[(*spIt)->m_idx] || ftau > m_maxTau[(*spIt)->m_idx]) continue;
	}
      }
      m_SoA.m_spo[m_nOuter++] = (*spIt)->m_pSP;
    }

    nSP++;  
  }

  return nSP;
}

void TrigTrackSeedGenerator::createTriplets(const TrigSiSpacePointBase* pS, int nInner, int nOuter,
					    std::vector<TrigInDetTriplet>& output, const IRoiDescriptor* roiDescriptor) {

  if(nInner==0 || nOuter==0) return;

  /// NB: the EDM classes store variuables as floats, therefore the double to float conversion
  ///     looses precision and as float(M_PI))>M_PI in the 7th decimal place we get problems 
  ///     at the pi boundary
  /// prefer std::fabs here for explicit double precision
  bool fullPhi = ( roiDescriptor->isFullscan() || ( std::fabs( roiDescriptor->phiPlus() - roiDescriptor->phiMinus()  - 2*M_PI ) < 1e-6 ) ); 

  int nSP = nInner + nOuter;

  const double pS_r = pS->r();
  const double pS_x = pS->x();
  const double pS_y = pS->y();
  const double pS_dr = pS->dr();
  const double pS_dz = pS->dz();
  const double cosA = pS_x/pS_r;
  const double sinA = pS_y/pS_r;
  const double covZ = pS_dz*pS_dz;
  const double covR = pS_dr*pS_dr;
  const bool isPixel = pS->isPixel();

  int idx = 0;
  for(;idx<nInner;idx++) {
    const TrigSiSpacePointBase* pSP = m_SoA.m_spi[idx];
    
    //1. transform in the pS-centric c.s

    const double dx = pSP->x() - pS_x;
    const double dy = pSP->y() - pS_y;
    const double R2inv = 1.0/(dx*dx+dy*dy);
    const double Rinv = std::sqrt(R2inv);
    const double xn = dx*cosA + dy*sinA;
    const double yn =-dx*sinA + dy*cosA;
    const double dz = pSP->z() - pS->z(); 
    const double t = Rinv*dz;

    //2. Conformal mapping

    m_SoA.m_r[idx] = Rinv;
    m_SoA.m_u[idx] = xn*R2inv;
    m_SoA.m_v[idx] = yn*R2inv;
    
    //3. cot(theta) estimation for the doublet

    const double covZP = pSP->dz()*pSP->dz();
    const double covRP = pSP->dr()*pSP->dr();
    
    m_SoA.m_t[idx] = t;
    m_SoA.m_tCov[idx] = R2inv*(covZ + covZP + t*t*(covR+covRP));
  }

  for(int k=0;k<nOuter;k++,idx++) {
    const TrigSiSpacePointBase* pSP = m_SoA.m_spo[k];
    
    //1. transform in the pS-centric c.s

    const double dx = pSP->x() - pS_x;
    const double dy = pSP->y() - pS_y;
    const double R2inv = 1.0/(dx*dx+dy*dy);
    const double Rinv = std::sqrt(R2inv);
    const double xn = dx*cosA + dy*sinA;
    const double yn =-dx*sinA + dy*cosA;
    const double dz = -pSP->z() + pS->z(); 
    const double t = Rinv*dz;

    //2. Conformal mapping
    
    m_SoA.m_r[idx] = Rinv;
    m_SoA.m_u[idx] = xn*R2inv;
    m_SoA.m_v[idx] = yn*R2inv;
    
    //3. cot(theta) estimation for the doublet

    const double covZP = pSP->dz()*pSP->dz();
    const double covRP = pSP->dr()*pSP->dr();
    
    m_SoA.m_t[idx] = t;
    m_SoA.m_tCov[idx] = R2inv*(covZ + covZP + t*t*(covR+covRP));
  }

  //double loop

  for(int innIdx=0;innIdx<nInner;innIdx++) {

    //mult. scatt contribution due to the layer with middle SP
    
    const double r_inn = m_SoA.m_r[innIdx];
    const double t_inn = m_SoA.m_t[innIdx];
    const double v_inn = m_SoA.m_v[innIdx];
    const double u_inn = m_SoA.m_u[innIdx];
    const double tCov_inn = m_SoA.m_tCov[innIdx];
    const double dCov = m_CovMS*(1+t_inn*t_inn);


    for(int outIdx=nInner;outIdx<nSP;outIdx++) {

      //1. rz doublet matching
      const double t_out = m_SoA.m_t[outIdx];



      const double dt2 = std::pow((t_inn - t_out), 2)/9.0;

      
      double covdt = (t_inn*t_out*covR + covZ);
      covdt       *= 2*r_inn*m_SoA.m_r[outIdx];
      covdt       += tCov_inn + m_SoA.m_tCov[outIdx];

      if(dt2 > covdt+dCov) continue;

      //2. pT estimate

      const double du = m_SoA.m_u[outIdx] - u_inn;
      if(du==0.0) continue;
      const double A = (m_SoA.m_v[outIdx] - v_inn)/du;
      const double B = v_inn - A*u_inn;
      const double R_squ = (1 + A*A)/(B*B);

      if(R_squ < m_minR_squ) continue;

      //3. the 3-sigma cut with estimated pT

      const double frac = m_minR_squ/R_squ;
      if(dt2 > covdt+frac*dCov) continue;

      //4. d0 cut

      const double fabs_d0 = std::fabs(pS_r*(B*pS_r - A));

      if(fabs_d0 > m_settings.m_tripletD0Max) continue;
      
      if (m_SoA.m_spo[outIdx-nInner]->isSCT() && isPixel) {
        if(fabs_d0 > m_settings.m_tripletD0_PPS_Max) continue;
      }

      //5. phi0 cut

      if ( !fullPhi ) { 
        const double uc = 2*B*pS_r - A;
        const double phi0 = atan2(sinA - uc*cosA, cosA + uc*sinA);

        if ( !RoiUtil::containsPhi( *roiDescriptor, phi0 ) ) {
          continue;
        }
      }

      //6. add new triplet

      //  Calculate triplet weight: No weighting in LRT mode, but d0**2 weighting for normal (non LRT) mode. Triplets are then sorted by lowest weight.
      const double Q= (m_settings.m_LRTmode ? 0 : fabs_d0*fabs_d0);
      if(output.size()>=m_settings.m_maxTripletBufferLength) {
	if (m_settings.m_LRTmode) { // take the first m_maxTripletBufferLength triplets
	  continue;
	} else { // choose smallest d0

	  std::sort(output.begin(), output.end(), 
		    [](const TrigInDetTriplet& A, const TrigInDetTriplet& B) {
		      return A.Q() > B.Q();
		    }
		    );

	  std::vector<TrigInDetTriplet>::iterator it = output.begin();
	  if( Q >= (*it).Q()) {
	    continue;
	  }
	  output.erase(it);
	}
      }
      output.emplace_back(*m_SoA.m_spi[innIdx], *pS, *m_SoA.m_spo[outIdx-nInner], Q);
    }
  }
}

void TrigTrackSeedGenerator::createTripletsNew(const TrigSiSpacePointBase* pS, int nInner, int nOuter,
					       std::vector<TrigInDetTriplet>& output, const IRoiDescriptor* roiDescriptor) {

  if(nInner==0 || nOuter==0) return;

  /// prefer std::fabs here for explicit double precision 
  bool fullPhi = ( roiDescriptor->isFullscan() || ( std::fabs( roiDescriptor->phiPlus() - roiDescriptor->phiMinus()  - 2*M_PI ) < 1e-6 ) ); 


  int nSP = nInner + nOuter;
  output.reserve(m_settings.m_maxTripletBufferLength);

  int nL = m_outerMarkers.size() + m_innerMarkers.size();

  int nleft[64];
  int iter[64];
  int dirs[64];
  int type[64];
  double values[64];

  iter[0] = m_innerMarkers[0]-1;
  nleft[0] = m_innerMarkers[0];
  dirs[0] = -1;
  type[0] = 0;//inner sp
  int kL=1;
  for(unsigned int k=1;k<m_innerMarkers.size();k++) {
    iter[kL] = m_innerMarkers[k]-1;
    nleft[kL] = m_innerMarkers[k]-m_innerMarkers[k-1];
    dirs[kL] = -1;
    type[kL] = 0;
    kL++;
  }
  iter[kL] = 0;
  nleft[kL] = m_outerMarkers[0];
  dirs[kL] = 1;
  type[kL] = 1;//outer sp
  kL++;  
  for(unsigned int k=1;k<m_outerMarkers.size();k++) {
    iter[kL] = m_outerMarkers[k-1];
    nleft[kL] = m_outerMarkers[k] - m_outerMarkers[k-1];
    dirs[kL] = 1;
    type[kL] = 1;
    kL++;
  }
  kL--;
  for(int k=0;k<nL;k++) {
    values[k] = (type[k]==0) ? m_SoA.m_ti[iter[k]] : m_SoA.m_to[iter[k]];//initialization
  }

  //merged sort

  int nActive=nL;
  int iSP=0;
  while(nActive!=0) {
    nActive = 0;
    //find min element
    double min_el=1000.0;
    int k_min=-1;
    for(int k=0;k<nL;k++) {
      if(nleft[k]==0) continue;
      nActive++;
      if(values[k]<min_el) {
        min_el = values[k];
        k_min = k;
      }
    }
    if(k_min==-1) break;
    //output

    int i_min = iter[k_min];

    if(type[k_min]==0){//inner 
      m_SoA.m_sorted_sp[iSP] = m_SoA.m_spi[i_min];
    } else {//outer 
      m_SoA.m_sorted_sp[iSP] = m_SoA.m_spo[i_min];
    }

    m_SoA.m_sorted_sp_type[iSP] = type[k_min];
    m_SoA.m_sorted_sp_t[iSP] = min_el;
    iSP++;

    iter[k_min] += dirs[k_min];
    nleft[k_min]--;
    if(nleft[k_min]==0) {
      nActive--;
      continue;
    }
    values[k_min] = (type[k_min]==0) ? m_SoA.m_ti[iter[k_min]] : m_SoA.m_to[iter[k_min]];
  }

  const double pS_r = pS->r();
  const double pS_x = pS->x();
  const double pS_y = pS->y();
  //const double pS_z = pS->z();
  const double pS_dr = pS->dr();
  const double pS_dz = pS->dz();
  const double cosA = pS_x/pS_r;
  const double sinA = pS_y/pS_r;
  const double covZ = pS_dz*pS_dz;
  const double covR = pS_dr*pS_dr;
  const bool isPixel = pS->isPixel();

  for(int idx=0;idx<nSP;idx++) {

    const TrigSiSpacePointBase* pSP = m_SoA.m_sorted_sp[idx];
    
    //1. transform in the pS-centric c.s

    const double dx = pSP->x() - pS_x;
    const double dy = pSP->y() - pS_y;
    const double R2inv = 1.0/(dx*dx+dy*dy);
    const double Rinv = std::sqrt(R2inv);
    const double xn = dx*cosA + dy*sinA;
    const double yn =-dx*sinA + dy*cosA;
    const double dz = (m_SoA.m_sorted_sp_type[idx]==0) ? pSP->z() - pS->z() : -pSP->z() + pS->z();

    const double t = Rinv*dz;

    //2. Conformal mapping

    m_SoA.m_r[idx] = Rinv;
    m_SoA.m_u[idx] = xn*R2inv;
    m_SoA.m_v[idx] = yn*R2inv;
    
    //3. cot(theta) estimation for the doublet

    const double covZP = pSP->dz()*pSP->dz();
    const double covRP = pSP->dr()*pSP->dr();
    
    m_SoA.m_t[idx] = t;
    m_SoA.m_tCov[idx] = R2inv*(covZ + covZP + t*t*(covR+covRP));
  }

  //double loop

  int iter1 = 0;

  while(iter1<nSP-1) {//outer loop

    int type1 = m_SoA.m_sorted_sp_type[iter1];//0 - inner, 1 - outer
    double t1 = m_SoA.m_sorted_sp_t[iter1];
    double tcut = (1 + t1*t1)*m_dtPreCut;

    //mult. scatt contribution due to the layer with middle SP
    
    const double r_inn = m_SoA.m_r[iter1];
    const double t_inn = m_SoA.m_t[iter1];
    const double v_inn = m_SoA.m_v[iter1];
    const double u_inn = m_SoA.m_u[iter1];
    const double tCov_inn = m_SoA.m_tCov[iter1];
    const double dCov = m_CovMS*(1+t_inn*t_inn);

    for(int iter2=iter1+1;iter2<nSP;iter2++) {//inner loop

      if(type1==m_SoA.m_sorted_sp_type[iter2]) continue;

      if(m_SoA.m_sorted_sp_t[iter2]-t1>tcut) break;

      const double t_out = m_SoA.m_t[iter2];

      const double dt2 = std::pow((t_inn - t_out), 2)*(1.0/9.0);

      double covdt = (t_inn*t_out*covR + covZ);
      covdt       *= 2*r_inn*m_SoA.m_r[iter2];
      covdt       += tCov_inn + m_SoA.m_tCov[iter2];

      if(dt2 > covdt+dCov) continue;

      //2. pT estimate

      const double du = m_SoA.m_u[iter2] - u_inn;
      if(du==0.0) continue;

      const double A = (m_SoA.m_v[iter2] - v_inn)/du;
      //Branchless version of (type1==0) ? v_inn - A*u_inn : m_SoA.m_v[iter2] - A*m_SoA.m_u[iter2];
      const double B = (1-type1)*(v_inn - A*u_inn) + type1*(m_SoA.m_v[iter2] - A*m_SoA.m_u[iter2]);
      const double R_squ = (1 + A*A)/(B*B);

      if(R_squ < m_minR_squ) continue;

      //3. the 3-sigma cut with estimated pT

      const double frac = m_minR_squ/R_squ;
      if(dt2 > covdt+frac*dCov) continue;

      //4. d0 cut

      const double d0_partial = B*pS_r - A;//Pre-calculate for use in phi check
      const double fabs_d0 = std::fabs(pS_r*(d0_partial));

      if(fabs_d0 > m_settings.m_tripletD0Max) continue;

      //Branchless version of (type1 == 1) ? m_SoA.m_sorted_sp[iter1]->isSCT() : m_SoA.m_sorted_sp[iter2]->isSCT();
      bool isSCT =  (1-type1)*m_SoA.m_sorted_sp[iter1]->isSCT() + type1*m_SoA.m_sorted_sp[iter2]->isSCT();
      
      if (isSCT && isPixel) {
        if(fabs_d0 > m_settings.m_tripletD0_PPS_Max) continue;
      }

      //5. phi0 cut

      if ( !fullPhi ) {
        const double uc = 2*d0_partial;
        const double phi0 = atan2(sinA - uc*cosA, cosA + uc*sinA);

        if ( !RoiUtil::containsPhi( *roiDescriptor, phi0 ) ) {
          continue;
        }
      }

      //6. add new triplet

      const double Q = fabs_d0*fabs_d0;

      if(output.size()>=m_settings.m_maxTripletBufferLength) {
        std::sort(output.begin(), output.end(), 
          [](const TrigInDetTriplet& A, const TrigInDetTriplet& B) {
            return A.Q() > B.Q();
          }
        );

        std::vector<TrigInDetTriplet>::iterator it = output.begin();
        if( Q >= (*it).Q()) {
          continue;
        }
        output.erase(it);

      }

      const TrigSiSpacePointBase* pSPI = (type1==0) ? m_SoA.m_sorted_sp[iter1] : m_SoA.m_sorted_sp[iter2];
      const TrigSiSpacePointBase* pSPO = (type1==0) ? m_SoA.m_sorted_sp[iter2] : m_SoA.m_sorted_sp[iter1];

      output.emplace_back(*pSPI, *pS, *pSPO, Q);
    }

    iter1++;
  }

}

void TrigTrackSeedGenerator::createConfirmedTriplets(const TrigSiSpacePointBase* pS, int nInner, int nOuter,
						     std::vector<TrigInDetTriplet>& output, const IRoiDescriptor* roiDescriptor) {


  struct ProtoSeed {
  public:
    ProtoSeed(const TrigSiSpacePointBase* s, double c, double Q, bool conf = false) : m_sp(s), m_curv(c), m_Q(Q), m_confirmed(conf) {};
    ProtoSeed(const ProtoSeed& ps) : m_sp(ps.m_sp), m_curv(ps.m_curv), m_Q(ps.m_Q), m_confirmed(ps.m_confirmed) {};
    const TrigSiSpacePointBase* m_sp;
    double m_curv, m_Q;
    bool m_confirmed;
  };

  if(nInner==0 || nOuter==0) return;
  
  /// prefer std::fabs here for explicit double precision
  bool fullPhi = ( roiDescriptor->isFullscan() || ( std::fabs( roiDescriptor->phiPlus() - roiDescriptor->phiMinus()  - 2*M_PI ) < 1e-6 ) ); 


  int nSP = nInner + nOuter;

  const double pS_r = pS->r();
  const double pS_x = pS->x();
  const double pS_y = pS->y();
  const double pS_dr = pS->dr();
  const double pS_dz = pS->dz();
  const double cosA = pS_x/pS_r;
  const double sinA = pS_y/pS_r;
  const double covZ = pS_dz*pS_dz;
  const double covR = pS_dr*pS_dr;
  const bool isPixel = pS->isPixel();


  double bestQ = 1e8;


  int idx = 0;
  for(;idx<nInner;idx++) {
    const TrigSiSpacePointBase* pSP = m_SoA.m_spi[idx];
    
    //1. transform in the pS-centric c.s

    const double dx = pSP->x() - pS_x;
    const double dy = pSP->y() - pS_y;
    const double R2inv = 1.0/(dx*dx+dy*dy);
    const double Rinv = std::sqrt(R2inv);
    const double xn = dx*cosA + dy*sinA;
    const double yn =-dx*sinA + dy*cosA;
    const double dz = pSP->z() - pS->z(); 
    const double t = Rinv*dz;

    //2. Conformal mapping

    m_SoA.m_r[idx] = Rinv;
    m_SoA.m_u[idx] = xn*R2inv;
    m_SoA.m_v[idx] = yn*R2inv;
    
    //3. cot(theta) estimation for the doublet

    const double covZP = pSP->dz()*pSP->dz();
    const double covRP = pSP->dr()*pSP->dr();
    
    m_SoA.m_t[idx] = t;
    m_SoA.m_tCov[idx] = R2inv*(covZ + covZP + t*t*(covR+covRP));
  }

  for(int k=0;k<nOuter;k++,idx++) {
    const TrigSiSpacePointBase* pSP = m_SoA.m_spo[k];
    
    //1. transform in the pS-centric c.s

    const double dx = pSP->x() - pS_x;
    const double dy = pSP->y() - pS_y;
    const double R2inv = 1.0/(dx*dx+dy*dy);
    const double Rinv = std::sqrt(R2inv);
    const double xn = dx*cosA + dy*sinA;
    const double yn =-dx*sinA + dy*cosA;
    const double dz = -pSP->z() + pS->z(); 
    const double t = Rinv*dz;

    //2. Conformal mapping
    
    m_SoA.m_r[idx] = Rinv;
    m_SoA.m_u[idx] = xn*R2inv;
    m_SoA.m_v[idx] = yn*R2inv;
    
    //3. cot(theta) estimation for the doublet

    const double covZP = pSP->dz()*pSP->dz();
    const double covRP = pSP->dr()*pSP->dr();
    
    m_SoA.m_t[idx] = t;
    m_SoA.m_tCov[idx] = R2inv*(covZ + covZP + t*t*(covR+covRP));
  }

  //double loop

  for(int innIdx=0;innIdx<nInner;innIdx++) {//loop over inner spacepoints

    //mult. scatt contribution due to the layer with middle SP
    
    const double r_inn = m_SoA.m_r[innIdx];
    const double t_inn = m_SoA.m_t[innIdx];
    const double v_inn = m_SoA.m_v[innIdx];
    const double u_inn = m_SoA.m_u[innIdx];
    const double tCov_inn = m_SoA.m_tCov[innIdx];
    const double dCov = m_CovMS*(1+t_inn*t_inn);

    std::vector<ProtoSeed> vPro;
    vPro.reserve(100);

    for(int outIdx=nInner;outIdx<nSP;outIdx++) {

      //1. rz doublet matching
      const double t_out = m_SoA.m_t[outIdx];

      const double dt2 = std::pow((t_inn - t_out), 2)/9.0;//i.e. 3-sigma cut


      double covdt = (t_inn*t_out*covR + covZ);
      covdt       *= 2*r_inn*m_SoA.m_r[outIdx];
      covdt       += tCov_inn + m_SoA.m_tCov[outIdx];

      if(dt2 > covdt+dCov) continue;

      //2. pT estimate

      const double du = m_SoA.m_u[outIdx] - u_inn;
      if(du==0.0) continue;
      const double A = (m_SoA.m_v[outIdx] - v_inn)/du;
      const double B = v_inn - A*u_inn;
      const double R_squ = (1 + A*A)/(B*B);

      if(R_squ < m_minR_squ) continue;
      
      
      //3. the 3-sigma cut with estimated pT

      const double frac = m_minR_squ/R_squ;
      if(dt2 > covdt+frac*dCov) continue;

      //4. d0 cut

      const double fabs_d0 = std::fabs(pS_r*(B*pS_r - A));

      if(fabs_d0 > m_settings.m_tripletD0Max) continue;
      
      if (m_SoA.m_spo[outIdx-nInner]->isSCT() && isPixel) {
        if(fabs_d0 > m_settings.m_tripletD0_PPS_Max) continue;
      }

      //  Calculate triplet weight: No weighting in LRT mode, but d0**2 weighting for normal (non LRT) mode. Triplets are then sorted by lowest weight.

      const double Q= (m_settings.m_LRTmode ? 0 : fabs_d0*fabs_d0);

      bool isOuterPixel = m_SoA.m_spo[outIdx-nInner]->isPixel();

      if(!m_settings.m_LRTmode) {
	if (isOuterPixel && (bestQ < Q)) continue;//this PPP seed has worse quality
      }
      
      //5. phi0 cut

      if ( !fullPhi ) {
        const double uc = 2*B*pS_r - A;
        const double phi0 = atan2(sinA - uc*cosA, cosA + uc*sinA);

        if ( !RoiUtil::containsPhi( *roiDescriptor, phi0 ) ) {
          continue;
        }
      }

      //6. add new triplet
      
      const double Curv  = B/std::sqrt(1+A*A);
      
      
      bool isConfirmed = false;
      
      for(auto& ps : vPro) {
	double diffC = 1 - Curv/ps.m_curv;
	if(std::fabs(diffC) < m_settings.m_curv_delta) {
	  ps.m_confirmed = true;
	  isConfirmed = true;
	}
      }
      
      if(!isOuterPixel) {
	continue;//we use PPS seeds only to confirm PPP seeds
      }
      
      vPro.emplace_back(ProtoSeed(m_SoA.m_spo[outIdx-nInner], Curv, Q, isConfirmed));

    }//loop over outer spacepoints
    
    for(const auto& ps : vPro) {
      if(!ps.m_confirmed) continue;
      if(output.size()>=m_settings.m_maxTripletBufferLength) {
	if (m_settings.m_LRTmode) { // take the first m_maxTripletBufferLength triplets
	  continue;
	} 
	else { // choose largest d0

	  std::sort(output.begin(), output.end(), 
		    [](const TrigInDetTriplet& A, const TrigInDetTriplet& B) {
		      return A.Q() < B.Q();
		    }
		    );
	  double QL = output.back().Q();
	  if(bestQ > QL) {
	    bestQ = QL;
	  }
	  if( ps.m_Q >= QL) {//reject this seed
	    continue;
	  }
	  output.pop_back();
	}
      }
      
      output.emplace_back(*m_SoA.m_spi[innIdx], *pS, *ps.m_sp, ps.m_Q);
    }
  }
}



void TrigTrackSeedGenerator::storeTriplets(std::vector<TrigInDetTriplet>& tripletVec) {
  for(std::vector<TrigInDetTriplet>::iterator it=tripletVec.begin();it!=tripletVec.end();++it) {
    float newQ = (*it).Q();

    if((!m_settings.m_LRTmode) && (*it).s3().isSCT()) {
      // In normal (non LRT) mode penalise SSS by 1000, PSS (if enabled) and PPS by 10000
      newQ += (*it).s1().isSCT() ? 1000.0 : 10000.0;
    }
    (*it).Q(newQ);
    m_triplets.push_back((*it));
  }
}

void TrigTrackSeedGenerator::getSeeds(std::vector<TrigInDetTriplet>& vs) {
  vs.clear();
  std::sort(m_triplets.begin(), m_triplets.end(), 
    [](const TrigInDetTriplet& A, const TrigInDetTriplet& B) {
      return A.Q() < B.Q();
    }
  );
  vs = std::move(m_triplets);
}
