/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/PtFromRadius.h"

#include "GaudiKernel/MsgStream.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "TMath.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtFromRadius::PtFromRadius(MsgStream* msg,
					 BooleanProperty use_mcLUT,
					 const TrigL2MuonSA::PtBarrelLUTSvc* ptBarrelLUTSvc): 
   m_msg(msg),
   m_use_mcLUT(use_mcLUT),
   m_ptBarrelLUT(ptBarrelLUTSvc->ptBarrelLUT())
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtFromRadius::~PtFromRadius() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtFromRadius::setPt(TrigL2MuonSA::TrackPattern& trackPattern)
{
  const LUT&   lut   = m_ptBarrelLUT->lut();
  const LUTsp& lutSP = m_ptBarrelLUT->lutSP();

  TrigL2MuonSA::SuperPoint* superPoints[3];
  for (int i_station=0; i_station<3; i_station++) {
    superPoints[i_station] = &(trackPattern.superPoints[i_station]);
  }

  int etabin,phibin,neweta,newphi,ch,add;
  float phistep,etastep,pstep,dist,distp,disteta,distphi;
  float A0[6]={0.,0.,0.,0.,0.,0.},A1[6]={0.,0.,0.,0.,0.,0.};
  
  if(trackPattern.radius > 0.) {
    add = trackPattern.s_address;
    etabin = (int)((trackPattern.etaMap - lut.EtaMin[add])/lut.EtaStep[add]);
    phibin = (int)((trackPattern.phiMap - lut.PhiMin[add])/lut.PhiStep[add]);
    
    if(etabin<=-1) etabin = 0;
    if(etabin>=lut.NbinEta[add]) etabin = lut.NbinEta[add]-1;
    if(phibin<=-1) phibin = 0;
    if(phibin>=lut.NbinPhi[add]) phibin = lut.NbinPhi[add]-1;
        
    disteta = trackPattern.etaMap - (etabin*lut.EtaStep[add] +
				  lut.EtaStep[add]/2. + lut.EtaMin[add]);
    distphi = trackPattern.phiMap - (phibin*lut.PhiStep[add] + 
				  lut.PhiStep[add]/2. + lut.PhiMin[add]);
    neweta  = (disteta >= 0.) ? etabin+1 : etabin-1;
    newphi  = (distphi >= 0.) ? phibin+1 : phibin-1;
    etastep = (disteta >= 0.) ? lut.EtaStep[add] : -lut.EtaStep[add];
    phistep = (distphi >= 0.) ? lut.PhiStep[add] : -lut.PhiStep[add];

    if(trackPattern.radius!=0.) {

      ch = (trackPattern.charge>=0.)? 1 : 0;

      if( !m_use_mcLUT && add==1 ) {
	// Use special table for Large-SP data

	if( fabs(trackPattern.phi)>90*TMath::DegToRad() ){
	  phibin=29-phibin;
	}
	int iR = ( superPoints[0]->R > 600 )? 1: 0;
	if( superPoints[0]->R < 0.01 ){
	  iR = ( phibin>14 )? 1: 0;
	}

	A0[0] = lutSP.table_LargeSP[iR][ch][etabin][phibin][0];
	A1[0] = lutSP.table_LargeSP[iR][ch][etabin][phibin][1];

	trackPattern.pt = trackPattern.radius*A0[0] + A1[0];

      } else {

	A0[0] = lut.table[add][ch][etabin][phibin][0];
	A1[0] = lut.table[add][ch][etabin][phibin][1];
	if((neweta<0||neweta>=lut.NbinEta[add])&&
	   (newphi<0||newphi>=lut.NbinPhi[add])) {
	  trackPattern.pt = trackPattern.radius*A0[0] + A1[0];
	} else if (neweta<0||neweta>=lut.NbinEta[add]) {
	  A0[1] = lut.table[add][ch][etabin][newphi][0];
	  A1[1] = lut.table[add][ch][etabin][newphi][1];
	  A0[2] = A0[0] + ((A0[1] - A0[0])/phistep)*distphi;
	  A1[2] = A1[0] + ((A1[1] - A1[0])/phistep)*distphi;
	  trackPattern.pt = trackPattern.radius*A0[2] + A1[2];
	} else if (newphi<0||newphi>=lut.NbinPhi[add]) {
	  A0[1] = lut.table[add][ch][neweta][phibin][0];
	  A1[1] = lut.table[add][ch][neweta][phibin][1];
	  A0[2] = A0[0] + ((A0[1] - A0[0])/etastep)*disteta;
	  A1[2] = A1[0] + ((A1[1] - A1[0])/etastep)*disteta;
	  trackPattern.pt = trackPattern.radius*A0[2] + A1[2];
	} else {
	  if(disteta >= distphi*lut.EtaStep[add]/lut.PhiStep[add]) {
	    A0[1] = lut.table[add][ch][neweta][phibin][0];
	    A1[1] = lut.table[add][ch][neweta][phibin][1];
	    A0[2] = lut.table[add][ch][neweta][newphi][0];
	    A1[2] = lut.table[add][ch][neweta][newphi][1];
	    A0[3] = A0[0] + ((A0[1] - A0[0])/etastep)*disteta;
	    A1[3] = A1[0] + ((A1[1] - A1[0])/etastep)*disteta;
	    dist  = sqrt(phistep*phistep + etastep*etastep);
	    distp = sqrt(disteta*disteta + distphi*distphi);
	    A0[4] = A0[0] + ((A0[2] - A0[0])/dist)*distp;
	    A1[4] = A1[0] + ((A1[2] - A1[0])/dist)*distp;
	    pstep = (phistep/dist)*distp;
	    A0[5] = A0[3] + ((A0[4] - A0[3])/pstep)*distphi;
	    A1[5] = A1[3] + ((A1[4] - A1[3])/pstep)*distphi;
	    trackPattern.pt = trackPattern.radius*A0[5] + A1[5];
	  } else {
	    A0[1] = lut.table[add][ch][etabin][newphi][0];
	    A1[1] = lut.table[add][ch][etabin][newphi][1];
	    A0[2] = lut.table[add][ch][neweta][newphi][0];
	    A1[2] = lut.table[add][ch][neweta][newphi][1];
	    A0[3] = A0[0] + ((A0[1] - A0[0])/phistep)*distphi;
	    A1[3] = A1[0] + ((A1[1] - A1[0])/phistep)*distphi;
	    dist  = sqrt(phistep*phistep + etastep*etastep);
	    distp = sqrt(disteta*disteta + distphi*distphi);
	    A0[4] = A0[0] + ((A0[2] - A0[0])/dist)*distp;
	    A1[4] = A1[0] + ((A1[2] - A1[0])/dist)*distp;
	    pstep = (etastep/dist)*distp;
	    A0[5] = A0[3] + ((A0[4] - A0[3])/pstep)*disteta;
	    A1[5] = A1[3] + ((A1[4] - A1[3])/pstep)*disteta;
	    trackPattern.pt = trackPattern.radius*A0[5] + A1[5];
	  }
	}
      }
    }
  }

  msg() << MSG::DEBUG << "pT determined from radius: radius/sagitta/pT/charge/s_address="
	<< trackPattern.radius << "/" << trackPattern.sagitta << "/"
	<< trackPattern.pt << "/" << trackPattern.charge << "/" << trackPattern.s_address << endreq;


  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

