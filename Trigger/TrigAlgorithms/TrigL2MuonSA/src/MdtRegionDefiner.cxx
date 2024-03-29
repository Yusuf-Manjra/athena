/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtRegionDefiner.h"

#include "MdtRegion.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "xAODTrigMuon/TrigMuonDefs.h"
#include <cmath>

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::MdtRegionDefiner::MdtRegionDefiner(const std::string& type,
						 const std::string& name,
						 const IInterface*  parent):
  AthAlgTool(type, name, parent)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtRegionDefiner::initialize()
{
  ATH_CHECK(m_muDetMgrKey.initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtRegionDefiner::getMdtRegions(const TrigRoiDescriptor* p_roids,
							 const TrigL2MuonSA::RpcFitResult& rpcFitResult,
							 TrigL2MuonSA::MuonRoad& muonRoad,
							 TrigL2MuonSA::MdtRegion& mdtRegion) const
{
  constexpr double ZERO_LIMIT = 1e-5;
  mdtRegion.Clear();
  
  int sectors[2];
    
  sectors[0] = muonRoad.MDT_sector_trigger;
  sectors[1] = muonRoad.MDT_sector_overlap;

  int endcap_inner = xAOD::L2MuonParameters::Chamber::EndcapInner;

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();

  for(int i_station=0; i_station<6; i_station++) {
    int chamber = 0;
    if (i_station==0) chamber = xAOD::L2MuonParameters::Chamber::BarrelInner;
    if (i_station==1) chamber = xAOD::L2MuonParameters::Chamber::BarrelMiddle;
    if (i_station==2) chamber = xAOD::L2MuonParameters::Chamber::BarrelOuter;
    if (i_station==3) chamber = xAOD::L2MuonParameters::Chamber::BME;
    if (i_station==4) chamber = xAOD::L2MuonParameters::Chamber::EndcapInner;
    if (i_station==5) chamber = xAOD::L2MuonParameters::Chamber::Backup; // BMG
    for(int i_sector=0; i_sector<2; i_sector++) { // 0: normal, 1: overlap
      int sector = sectors[i_sector];
      ATH_MSG_DEBUG("--- chamber/sector=" << chamber << "/" << sector);

      if( sector==99 ) continue;
      float zMin   = 0;
      float zMax   = 0;
      float rMin   = 0;
      float rMax   = 0;
      float etaMin = 0;
      float etaMax = 0;
      float phiMin = 0;
      float phiMax = 0;
      int types[2];
      int& ty1 = types[0];
      int& ty2 = types[1];

      float tmp_rMin = 0;
      float tmp_rMax = 0;
      float tmp_zMin = 0;
      float tmp_zMax = 0;
      ty1 = -1;
      ty2 = -1;
      int sign = 1;
      
      for(unsigned int sta_iter=0; sta_iter< muonRoad.stationList.size(); sta_iter++){
	
	Identifier id = muonRoad.stationList[sta_iter];
	int stationPhi = m_idHelperSvc->mdtIdHelper().stationPhi(id);
	std::string name = m_idHelperSvc->mdtIdHelper().stationNameString(m_idHelperSvc->mdtIdHelper().stationName(id));
	int chamber_this = 99;
	int sector_this = 99;
	bool isEndcap;
	find_station_sector(name, stationPhi, isEndcap, chamber_this, sector_this);

	if(chamber_this == chamber && sector_this == sector ){
	  if(ty1 == -1)
	    ty1 = m_idHelperSvc->mdtIdHelper().stationNameIndex(name)+1;
	  else if(ty2 == -1)
	    ty2 = m_idHelperSvc->mdtIdHelper().stationNameIndex(name)+1;
          
          const MuonGM::MdtReadoutElement* mdtReadout = muDetMgr->getMdtReadoutElement(id);
          const MuonGM::MuonStation* muonStation = mdtReadout->parentMuonStation();
	  
	  Amg::Transform3D trans = Amg::CLHEPTransformToEigen(*muonStation->getNominalAmdbLRSToGlobal());
	  
	  Amg::Vector3D OrigOfMdtInAmdbFrame = 
	    Amg::Hep3VectorToEigen( muonStation->getBlineFixedPointInAmdbLRS() );
	  
	  tmp_rMin = (trans*OrigOfMdtInAmdbFrame).perp();
	  tmp_rMax = tmp_rMin+muonStation->Rsize();
	  
	  if(rMin==0 || tmp_rMin < rMin)rMin = tmp_rMin;
	  if(rMax==0 || tmp_rMax > rMax)rMax = tmp_rMax;
	  if ( chamber_this == endcap_inner ){
	    tmp_zMin = (trans*OrigOfMdtInAmdbFrame).z();
	    if(tmp_zMin < 0) sign = -1;
	    else if(tmp_zMin > 0) sign = 1;
	    tmp_zMax = tmp_zMin + sign*muonStation->Zsize();
	    if(zMin==0 || tmp_zMin < zMin)zMin = tmp_zMin;
	    if(zMax==0 || tmp_zMax > zMax)zMax = tmp_zMax;
	  }
	  
	}
      }

      if ( chamber == endcap_inner ){
        find_endcap_road_dim(muonRoad.rWidth[chamber][0],
            muonRoad.aw[chamber][i_sector],
            muonRoad.bw[chamber][i_sector],
            zMin,zMax,
            &rMin,&rMax);
      } else { //// barrel chambers
        float max_road = muonRoad.MaxWidth(chamber);
        find_barrel_road_dim(max_road,
            muonRoad.aw[chamber][i_sector],
            muonRoad.bw[chamber][i_sector],
            rMin,rMax,
            &zMin,&zMax);
      }

      ATH_MSG_DEBUG("...zMin/zMax/ty1/ty2=" << zMin << "/" << zMax << "/" << types[0] << "/" << types[1]);
      ATH_MSG_DEBUG("...rMin/rMax=" << rMin << "/" << rMax);
      
      find_eta_min_max(zMin, rMin, zMax, rMax, etaMin, etaMax);

      find_phi_min_max(muonRoad.phiRoI, phiMin, phiMax);

      ATH_MSG_DEBUG("...etaMin/etaMax/phiMin/phiMax=" << etaMin << "/" << etaMax << "/" << phiMin << "/" << phiMax);

      mdtRegion.zMin[chamber][i_sector] = zMin;
      mdtRegion.zMax[chamber][i_sector] = zMax;
      mdtRegion.rMin[chamber][i_sector] = rMin;
      mdtRegion.rMax[chamber][i_sector] = rMax;
      mdtRegion.etaMin[chamber][i_sector] = etaMin;
      mdtRegion.etaMax[chamber][i_sector] = etaMax;
      mdtRegion.phiMin[chamber][i_sector] = phiMin;
      mdtRegion.phiMax[chamber][i_sector] = phiMax;
      for(int i_type=0; i_type<2; i_type++) {
	int type = types[i_type];
	if( type == -1 ) continue;
	mdtRegion.chamberType[chamber][i_sector][i_type] = type;
      }
    }
  }
 
  if (m_use_rpc && rpcFitResult.isSuccess) {
    // use phi from fit
    for (int i=0; i<6; i++){
      for (int j=0; j<2; j++){
        if (i==4) muonRoad.phi[9][j] = rpcFitResult.phi;
        else if (i==5) muonRoad.phi[10][j] = rpcFitResult.phi;
        else muonRoad.phi[i][j] = rpcFitResult.phi;
      }
    }
  }
  else {
    for (int i=0; i<6; i++){
      for (int j=0; j<2; j++){
        if (std::abs(muonRoad.extFtfMiddlePhi) > ZERO_LIMIT) { // for inside-out
          if (i==4) muonRoad.phi[9][j] = muonRoad.extFtfMiddlePhi;
          else if (i==5) muonRoad.phi[10][j] = muonRoad.extFtfMiddlePhi;
          else muonRoad.phi[i][j] = muonRoad.extFtfMiddlePhi;
        }
        if (i==4) muonRoad.phi[9][j] = p_roids->phi();
        else if (i==5) muonRoad.phi[10][j] = p_roids->phi();
        else muonRoad.phi[i][j] = p_roids->phi();
      }
    }
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtRegionDefiner::getMdtRegions(const TrigRoiDescriptor* p_roids,
							 const TrigL2MuonSA::TgcFitResult& tgcFitResult,
							 TrigL2MuonSA::MuonRoad& muonRoad,
							 TrigL2MuonSA::MdtRegion& mdtRegion) const
{
  mdtRegion.Clear();
  
  int sectors[2];
  
  sectors[0] = muonRoad.MDT_sector_trigger;
  sectors[1] = muonRoad.MDT_sector_overlap;
  
  int endcap_middle = xAOD::L2MuonParameters::Chamber::EndcapMiddle;
  int barrel_inner = xAOD::L2MuonParameters::Chamber::BarrelInner;
  int bee = xAOD::L2MuonParameters::Chamber::BEE;

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();

  for(int i_station=0; i_station<7; i_station++) {
    int chamber = 0;
    if (i_station==0) chamber = xAOD::L2MuonParameters::Chamber::EndcapInner;
    if (i_station==1) chamber = xAOD::L2MuonParameters::Chamber::EndcapMiddle;
    if (i_station==2) chamber = xAOD::L2MuonParameters::Chamber::EndcapOuter;
    if (i_station==3) chamber = xAOD::L2MuonParameters::Chamber::EndcapExtra;
    if (i_station==4) chamber = xAOD::L2MuonParameters::Chamber::BarrelInner;
    if (i_station==5) chamber = xAOD::L2MuonParameters::Chamber::BEE;
    if (i_station==6) chamber = xAOD::L2MuonParameters::Chamber::Backup;
    for(int i_sector=0; i_sector<2; i_sector++) { // 0: normal, 1: overlap
      int sector = sectors[i_sector];
      ATH_MSG_DEBUG("--- chamber/sector=" << chamber << "/" << sector);
      if( sector==99 ) continue;
      float zMin   = 0;
      float zMax   = 0;
      float rMin   = 0;
      float rMax   = 0;
      float etaMin = 0;
      float etaMax = 0;
      float phiMin = 0;
      float phiMax = 0;
      int types[2];
      int& ty1 = types[0];
      int& ty2 = types[1];
      float sta_zMin = 0;
      float sta_zMax = 0;
	
      float tmp_zMin = 0;
      float tmp_zMax = 0;
      float tmp_rMin = 0;
      float tmp_rMax = 0;
      int sign = 1;
      ty1 = -1;
      ty2 = -1;
      
      for(unsigned int sta_iter=0; sta_iter<muonRoad.stationList.size(); sta_iter++){
	Identifier id = muonRoad.stationList[sta_iter];
	int stationPhi = m_idHelperSvc->mdtIdHelper().stationPhi(id);
	std::string name = m_idHelperSvc->mdtIdHelper().stationNameString(m_idHelperSvc->mdtIdHelper().stationName(id));
	int chamber_this = 99;
	int sector_this = 99;
	bool isEndcap;
	find_station_sector(name, stationPhi, isEndcap, chamber_this, sector_this);
        ATH_MSG_DEBUG("name/stationPhi/isEndcap/chamber_this/sector_this=" <<
		      name << "/" << stationPhi << "/" << isEndcap << "/" << chamber_this << "/" << sector_this);
	
	if(chamber_this == chamber && sector_this == sector){
	  if(ty1 == -1)
	    ty1 = m_idHelperSvc->mdtIdHelper().stationNameIndex(name)+1;
	  else if(ty2 == -1)
		ty2 = m_idHelperSvc->mdtIdHelper().stationNameIndex(name)+1;
          
          const MuonGM::MdtReadoutElement* mdtReadout = muDetMgr->getMdtReadoutElement(id);
          const MuonGM::MuonStation* muonStation = mdtReadout->parentMuonStation();
    
	  float scale = 10.;
	  
	  Amg::Transform3D trans = Amg::CLHEPTransformToEigen(*muonStation->getNominalAmdbLRSToGlobal());
	  
	  Amg::Vector3D OrigOfMdtInAmdbFrame = 
	    Amg::Hep3VectorToEigen( muonStation->getBlineFixedPointInAmdbLRS() );
	  
	  tmp_zMin = (trans*OrigOfMdtInAmdbFrame).z();
	  if(tmp_zMin < 0) sign = -1;
	  else if(tmp_zMin > 0) sign = 1;
	  tmp_zMax = tmp_zMin + sign*muonStation->Zsize();
	  
	  if(sta_zMin==0 || tmp_zMin<sta_zMin) sta_zMin = tmp_zMin;
	  if(sta_zMin==0 || tmp_zMax<sta_zMin) sta_zMin = tmp_zMax;
	  if(sta_zMax==0 || tmp_zMax>sta_zMax) sta_zMax = tmp_zMax;
	  if(sta_zMax==0 || tmp_zMin>sta_zMax) sta_zMax = tmp_zMin;
	  
	  if (chamber_this==barrel_inner){//barrel inner
	    tmp_rMin = (trans*OrigOfMdtInAmdbFrame).perp()/scale;
	    tmp_rMax = tmp_rMin+muonStation->Rsize()/scale;
	    if(rMin==0 || tmp_rMin < rMin)rMin = tmp_rMin;
	    if(rMax==0 || tmp_rMax > rMax)rMax = tmp_rMax;
	  }

	  if (chamber_this==bee){//BEE
	    tmp_rMin = (trans*OrigOfMdtInAmdbFrame).perp()/scale;
	    tmp_rMax = tmp_rMin+muonStation->Rsize()/scale;
	    if(rMin==0 || tmp_rMin < rMin)rMin = tmp_rMin;
	    if(rMax==0 || tmp_rMax > rMax)rMax = tmp_rMax;
	  }
	  
	}
      }
      
      if (chamber==endcap_middle) { // endcap middle
	
	if (tgcFitResult.isSuccess) { // TGC data is properly read
	  if (tgcFitResult.tgcMid1[3] < tgcFitResult.tgcMid2[3]) {
	    zMin = tgcFitResult.tgcMid1[3];
	    zMax = tgcFitResult.tgcMid2[3];
	  } else {
	    zMin = tgcFitResult.tgcMid2[3];
	    zMax = tgcFitResult.tgcMid1[3];
	  }
	  
	} else { // Failed to read TGC data 
	  
	  zMin = sta_zMin;
	  zMax = sta_zMax;
	}
      }
      else if (chamber==barrel_inner || chamber==bee) { //barrel inner || BEE
	float max_road = 50 /*muonRoad.MaxWidth(chamber)*/;
	find_barrel_road_dim(max_road,
			     muonRoad.aw[chamber][i_sector],
			     muonRoad.bw[chamber][i_sector],
			     rMin,rMax,
			     &zMin,&zMax);
      }else {// endcap inner,outer,ee

	  zMin = sta_zMin;
	  zMax = sta_zMax;
      }
      
      if (chamber!=barrel_inner && chamber!=bee){//Endcap chamber
        find_endcap_road_dim(muonRoad.rWidth[chamber][0],
			     muonRoad.aw[chamber][i_sector],
			     muonRoad.bw[chamber][i_sector],
			     zMin,zMax,
			     &rMin,&rMax);
      }
      
      ATH_MSG_DEBUG("...zMin/zMax/ty1/ty2=" << zMin << "/" << zMax << "/" << types[0] << "/" << types[1]);
      ATH_MSG_DEBUG("...rMin/rMax=" << rMin << "/" << rMax);
      
      find_eta_min_max(zMin, rMin, zMax, rMax, etaMin, etaMax);
      
      find_phi_min_max(muonRoad.phiRoI, phiMin, phiMax);
      
      ATH_MSG_DEBUG("...etaMin/etaMax/phiMin/phiMax=" << etaMin << "/" << etaMax << "/" << phiMin << "/" << phiMax);
      
      mdtRegion.zMin[chamber][i_sector] = zMin;
      mdtRegion.zMax[chamber][i_sector] = zMax;
      mdtRegion.rMin[chamber][i_sector] = rMin;
      mdtRegion.rMax[chamber][i_sector] = rMax;
      mdtRegion.etaMin[chamber][i_sector] = etaMin;
      mdtRegion.etaMax[chamber][i_sector] = etaMax;
      mdtRegion.phiMin[chamber][i_sector] = phiMin;
      mdtRegion.phiMax[chamber][i_sector] = phiMax;
      for(int i_type=0; i_type<2; i_type++) {
	int type = types[i_type];
	if( type == -1 ) continue;
	mdtRegion.chamberType[chamber][i_sector][i_type] = type;
      }
    } 
  }
  
  StatusCode sc = computePhi(p_roids, tgcFitResult, mdtRegion, muonRoad);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Error in comupting phi");
    return sc;
  }
  
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MdtRegionDefiner::find_station_sector(const std::string& name, int phi, bool& endcap, 
    int& chamber, int& sector)
{   
  if(name[0]=='E' || name[0]=='F' || (name[0]=='B' && name[1]=='E'))
    endcap = true;
  else 
    endcap = false;
  int largeSmall=0;
  if(name[2]=='S' || name[2]=='F' || name[2]=='G') largeSmall = 1;//Small
  if (name[1]=='E' && name[2]=='E') largeSmall=1;//BEE
  sector = (phi-1)*2 + largeSmall;
  if (endcap){
    if(name[1]=='I')
      chamber = 3;
    if(name[1]=='M')
      chamber = 4;
    if(name[1]=='O')
      chamber = 5;
    if(name[1]=='E')
      chamber = 6;
    if(name[1]=='E' && name[2]=='E')//BEE
      chamber = 8;
  }else {
    if(name[1]=='I')
      chamber = 0;
    if(name[1]=='M')
      chamber = 1;
    if(name[1]=='O')
      chamber = 2;
    if(name[1]=='M' && name[2]=='E')//BME
      chamber = 9;
    if(name[1]=='M' && name[2]=='G')//BMG
      chamber = 10;
    if (name[0]=='E'&&name[1]=='I') //EI
      chamber = 3;
  }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MdtRegionDefiner::find_phi_min_max(float phiMiddle, float& phiMin, float& phiMax) const
{   
   phiMin = phiMiddle - 0.1;
   phiMax = phiMiddle + 0.1;
   if ( phiMin < -1.*M_PI ) phiMin += 2.*M_PI;
   if ( phiMax > 1.*M_PI ) phiMax -= 2.*M_PI;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MdtRegionDefiner::find_eta_min_max(float zMin, float rMin, float zMax, float rMax,
						      float& etaMin, float& etaMax) const
{   
   // const bool doEmulateMuFast = true;
   const bool doEmulateMuFast = false;

   etaMin = 0.;
   etaMax = 0.;

   if(rMin > 0 && rMax > 0)
   {
      float eta[4];
      float theta;

      theta  = (std::abs(zMin)>0.1)? std::atan(rMin/std::abs(zMin)): M_PI/2.;
      eta[0] = (zMin>0.)?  -std::log(std::tan(theta/2.)) : std::log(std::tan(theta/2.));

      theta  = (std::abs(zMax)>0.1)? std::atan(rMin/std::abs(zMax)): M_PI/2.;
      eta[1] = (zMax>0.)?  -std::log(std::tan(theta/2.)) : std::log(std::tan(theta/2.));
      if(doEmulateMuFast) eta[1] = eta[0];

      theta  = (std::abs(zMax)>0.1)? std::atan(rMax/std::abs(zMax)): M_PI/2.;
      eta[2] = (zMax>0.)?  -std::log(std::tan(theta/2.)) : std::log(std::tan(theta/2.));

      theta  = (std::abs(zMin)>0.1)? std::atan(rMax/std::abs(zMin)): M_PI/2.;
      eta[3] = (zMin>0.)?  -std::log(std::tan(theta/2.)) : std::log(std::tan(theta/2.));
      if(doEmulateMuFast) eta[3] = eta[2];

      etaMin = eta[0];
      etaMax = eta[0];
      for(int i=1; i<4; i++) {
	 if( eta[i] < etaMin ) etaMin = eta[i];
	 if( eta[i] > etaMax ) etaMax = eta[i];
      }

      if (etaMin > etaMax) {
	 float tmp = etaMax;
	 etaMax = etaMin;
	 etaMin = tmp;
      }
   }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MdtRegionDefiner::find_barrel_road_dim(float max_road, float aw, float bw,
							  float rMin,float rMax,float *zMin,float *zMax) const
{
  float ia,iaq,dz,z,points[4];
  int i;
  
  *zMin =  9999.;
  *zMax = -9999.;
  
  if(aw) {
    ia  = 1/aw;
    iaq = ia*ia;
  } else {
    ia  = 0;
    iaq = 0;
  }
  
  dz = max_road*std::sqrt(1.+iaq);
  z  = (ia)? (rMin-bw)*ia : bw;
  points[0] = z - dz;
  points[1] = z + dz;
  z  = (ia)? (rMax-bw)*ia : bw;
  points[2] = z - dz;
  points[3] = z + dz;
  
  for(i=0;i<4;i++) {
    if(*zMin>=points[i]) *zMin = points[i];
    if(*zMax<=points[i]) *zMax = points[i];
  }

  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::MdtRegionDefiner::find_endcap_road_dim(float road,float aw, float bw, float zMin,
							  float zMax,float *rMin,float *rMax) const
{
   float r,points[4];
   int i;
  
   *rMin = 999999.;
   *rMax = 0.;
    
   if(zMin>=99999. && zMax<=-99999.)
   {
      *rMin = 0.;
      *rMax = 0.;
      return;
   }
    
   r = zMin*aw + bw;
   points[0] = r - road;
   points[1] = r + road;
   r = zMax*aw + bw;
   points[2] = r - road;
   points[3] = r + road;
    
   for(i=0;i<4;i++) {
      if(*rMin>=points[i]) *rMin = points[i];
      if(*rMax<=points[i]) *rMax = points[i];
   }
    
   if(*rMin<0.) *rMin = 0.;
   if(*rMax<0.) *rMax = 0.;
    
   return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtRegionDefiner::prepareTgcPoints(const TrigL2MuonSA::TgcHits& tgcHits)
{
   const double PHI_BOUNDARY = 0.2;

   // loop over TGC digits.
   unsigned int iHit;
   for (iHit = 0; iHit < tgcHits.size(); iHit++)
   {
      // Get the digit point.
      const TrigL2MuonSA::TgcHitData& hit = tgcHits[iHit];

      // reject width=0 hits
      const double ZERO_LIMIT = 1e-5;
      if( std::abs(hit.width) < ZERO_LIMIT ) continue;

      double w = 12.0 / hit.width / hit.width;
      if (hit.isStrip)
      {
	 w *= hit.r * hit.r;
	 double phi = hit.phi;
	 if( phi < 0 && ( (M_PI+phi)<PHI_BOUNDARY) ) phi += M_PI*2;
	 if      ( hit.sta < 3 ) { m_tgcStripMidPoints.push_back(TgcFit::Point(iHit + 1, hit.sta, hit.z, phi, w)); }
	 else if ( hit.sta ==3 ) { m_tgcStripInnPoints.push_back(TgcFit::Point(iHit + 1, hit.sta, hit.z, phi, w)); }
      }
      else
      {
	 if      ( hit.sta < 3 ) { m_tgcWireMidPoints.push_back(TgcFit::Point(iHit + 1, hit.sta, hit.z, hit.r, w)); }
	 else if ( hit.sta ==3 ) { m_tgcWireInnPoints.push_back(TgcFit::Point(iHit + 1, hit.sta, hit.z, hit.r, w)); }
      }
   }

   return StatusCode::SUCCESS;

}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtRegionDefiner::computePhi(const TrigRoiDescriptor* p_roids,
						      const TrigL2MuonSA::RpcFitResult& rpcFitResult,
						      const TrigL2MuonSA::MdtRegion& mdtRegion,
						      TrigL2MuonSA::MuonRoad& muonRoad) const
{
  int barrel_inner = xAOD::L2MuonParameters::Chamber::BarrelInner;
  int barrel_middle = xAOD::L2MuonParameters::Chamber::BarrelMiddle;
  int barrel_outer = xAOD::L2MuonParameters::Chamber::BarrelOuter;

  for(int i_station=0; i_station<3; i_station++) {
    int chamber = 0;
    if (i_station==0) chamber = xAOD::L2MuonParameters::Chamber::BarrelInner;
    if (i_station==1) chamber = xAOD::L2MuonParameters::Chamber::BarrelMiddle;
    if (i_station==2) chamber = xAOD::L2MuonParameters::Chamber::BarrelOuter;
    for( int i_sector=0; i_sector<2; i_sector++) {

      if ( rpcFitResult.isSuccess ) {

	if (chamber==barrel_middle || !rpcFitResult.isPhiDir) {
	  muonRoad.phi[chamber][i_sector] = rpcFitResult.phi;
	  continue;
	}
	
	double dz = 0.;
	
	if (chamber==barrel_outer) {
	  
	  double MiddleR = std::abs(mdtRegion.rMin[barrel_middle][i_sector] + mdtRegion.rMax[barrel_middle][i_sector])/2.;
	  double MiddleZ = MiddleR*muonRoad.aw[barrel_middle][i_sector] + muonRoad.bw[barrel_middle][i_sector];
	  
	  double mm = (muonRoad.aw[barrel_outer][i_sector]!=0)? 1./muonRoad.aw[barrel_outer][i_sector] : 0.;
	  double OuterR  = std::abs(mdtRegion.rMin[barrel_outer][i_sector]+mdtRegion.rMax[barrel_outer][i_sector])/2.;
	  double OuterZ  = (mm)? (OuterR-muonRoad.bw[barrel_outer][i_sector])/mm : muonRoad.bw[barrel_outer][i_sector];
	  double DzOuter = std::abs(OuterZ-MiddleZ);
	  dz = std::sqrt((OuterR-MiddleR)*(OuterR-MiddleR) + DzOuter*DzOuter);
	  dz = (OuterR-MiddleR);
	  
	} else if (chamber==barrel_inner) {
	  
	  double MiddleR = 0;
	  double MiddleZ = 0;
	  
	  double mm = (muonRoad.aw[barrel_inner][i_sector]!=0)? 1./muonRoad.aw[barrel_inner][i_sector] : 0.;
	  double InnerR  = std::abs(mdtRegion.rMin[barrel_inner][i_sector]+mdtRegion.rMax[barrel_inner][i_sector])/2.;
	  double InnerZ  = (mm)? (InnerR-muonRoad.bw[barrel_inner][i_sector])/mm  : muonRoad.bw[barrel_inner][i_sector];
	  double DzInner = std::abs(InnerZ-MiddleZ);
	  dz = -std::sqrt((InnerR-MiddleR)*(InnerR-MiddleR) + DzInner*DzInner);
	  dz = - std::abs(InnerR-MiddleR);
	  
	}
        
	muonRoad.phi[chamber][i_sector] = (dz)* rpcFitResult.dPhidZ + rpcFitResult.phi;
	
	while (muonRoad.phi[chamber][i_sector] > M_PI)
	  muonRoad.phi[chamber][i_sector] -= 2*M_PI;
	
	while (muonRoad.phi[chamber][i_sector] <-M_PI)
	  muonRoad.phi[chamber][i_sector] += 2*M_PI;

      } else {
        // RPC data is not read -> use RoI
        muonRoad.phi[chamber][i_sector] = p_roids->phi();
      }
    }
  }
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::MdtRegionDefiner::computePhi(const TrigRoiDescriptor* p_roids,
						      const TrigL2MuonSA::TgcFitResult& tgcFitResult,
						      const TrigL2MuonSA::MdtRegion& mdtRegion,
						      TrigL2MuonSA::MuonRoad& muonRoad) const
{
  int endcap_inner = xAOD::L2MuonParameters::Chamber::EndcapInner;
  int endcap_middle = xAOD::L2MuonParameters::Chamber::EndcapMiddle;
  int endcap_outer = xAOD::L2MuonParameters::Chamber::EndcapOuter;
  int barrel_inner = xAOD::L2MuonParameters::Chamber::BarrelInner;

  for(int i_station=0; i_station<5; i_station++) {
    int chamber = 0;
    if (i_station==0) chamber = xAOD::L2MuonParameters::Chamber::EndcapInner;
    if (i_station==1) chamber = xAOD::L2MuonParameters::Chamber::EndcapMiddle;
    if (i_station==2) chamber = xAOD::L2MuonParameters::Chamber::EndcapOuter;
    if (i_station==3) chamber = xAOD::L2MuonParameters::Chamber::EndcapExtra;
    if (i_station==4) chamber = xAOD::L2MuonParameters::Chamber::BarrelInner;
    for( int i_sector=0; i_sector<2; i_sector++) {

      if ( tgcFitResult.isSuccess ) {
	if (chamber==endcap_middle || !tgcFitResult.isPhiDir) {
	  muonRoad.phi[chamber][i_sector] = tgcFitResult.phi;
	  continue;
	}
	
	muonRoad.phi[chamber][i_sector] = 0.;
	double dz = 0.;
	
	if (chamber==endcap_outer) {
	  
	  double MiddleZ = std::abs(mdtRegion.zMin[endcap_middle][i_sector] + mdtRegion.zMax[endcap_middle][i_sector])/2.;
	  double MiddleR = MiddleZ*muonRoad.aw[endcap_middle][i_sector] + muonRoad.bw[endcap_middle][i_sector];
	  
	  double OuterZ  = std::abs(mdtRegion.zMin[endcap_outer][i_sector] + mdtRegion.zMax[endcap_outer][i_sector])/2.;
	  double OuterR  = OuterZ*muonRoad.aw[endcap_outer][i_sector] + muonRoad.bw[endcap_outer][i_sector];
	  double DrOuter = std::abs(OuterR-MiddleR);
	  dz = std::sqrt((OuterZ-MiddleZ)*(OuterZ-MiddleZ) + DrOuter*DrOuter);
	  dz = (OuterZ-MiddleZ);
	  
	} if (chamber==endcap_inner || chamber==barrel_inner) {

	  double MiddleZ = 0;
	  double MiddleR = 0;
	  
	  if(tgcFitResult.tgcInn[2] != 0.) {
	    muonRoad.phi[chamber][i_sector] = tgcFitResult.tgcInn[1];
	    continue;
	  }
	  double InnerZ  = std::abs(mdtRegion.zMin[endcap_inner][i_sector] + mdtRegion.zMax[endcap_inner][i_sector])/2.;
	  double InnerR  = InnerZ*muonRoad.aw[endcap_inner][i_sector] + muonRoad.bw[endcap_inner][i_sector];
	  double DrInner = std::abs(InnerR-MiddleR);
	  dz = -std::sqrt((InnerZ-MiddleZ)*(InnerZ-MiddleZ) + DrInner*DrInner);
	  dz = -std::abs(InnerZ-MiddleZ);
	}
        
	muonRoad.phi[chamber][i_sector] = (dz)* tgcFitResult.dPhidZ + tgcFitResult.phi;
	while (muonRoad.phi[chamber][i_sector] > M_PI)  muonRoad.phi[chamber][i_sector] -= 2*M_PI;
	while (muonRoad.phi[chamber][i_sector] <-M_PI)  muonRoad.phi[chamber][i_sector] += 2*M_PI;

      } else {
        // TGC data is not read -> use RoI
        muonRoad.phi[chamber][i_sector] = p_roids->phi();
      }
    }
  }
  return StatusCode::SUCCESS;
}  
