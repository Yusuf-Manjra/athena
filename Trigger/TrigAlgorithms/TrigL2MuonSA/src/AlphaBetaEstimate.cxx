/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/AlphaBetaEstimate.h"

#include "GaudiKernel/MsgStream.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "xAODTrigMuon/TrigMuonDefs.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::AlphaBetaEstimate::AlphaBetaEstimate(MsgStream* msg,
						   const TrigL2MuonSA::PtEndcapLUTSvc* ptEndcapLUTSvc): 
  m_msg(msg),
  m_ptEndcapLUT(ptEndcapLUTSvc->ptEndcapLUT())
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::AlphaBetaEstimate::~AlphaBetaEstimate() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::AlphaBetaEstimate::setAlphaBeta(const LVL1::RecMuonRoI*   p_roi,
							 TrigL2MuonSA::TgcFitResult& tgcFitResult,
							 TrigL2MuonSA::TrackPattern& trackPattern,
                                                         const TrigL2MuonSA::MuonRoad& muonRoad)
{
  const int MAX_STATION = 5;
  const double PHI_RANGE = 12./(CLHEP::pi/8.);
  
  // computing ALPHA, BETA and RADIUS
  float InnerSlope      = 0;
  float MiddleSlope     = 0;
  float MiddleIntercept = 0;
  float InnerR          = 0;
  float InnerZ          = 0;
  float EER          = 0;//ee
  float EEZ          = 0;//ee
  float MiddleR         = 0; 
  float MiddleZ         = 0;
  float OuterR          = 0;
  float OuterZ          = 0;
  float EBIR          = 0;//endcap barrel inner
  float EBIZ          = 0;//endcap barrel inner

  // set etaMap and phi
  double phi  = 0.;
  double phim = 0.;
      
  TrigL2MuonSA::SuperPoint* superPoint;

  for (int i_station=0; i_station<MAX_STATION; i_station++) {

    int chamber = 0;
    if ( i_station == 0 ) chamber = xAOD::L2MuonParameters::Chamber::EndcapInner;
    if ( i_station == 1 ) chamber = xAOD::L2MuonParameters::Chamber::EndcapMiddle;
    if ( i_station == 2 ) chamber = xAOD::L2MuonParameters::Chamber::EndcapOuter;
    if ( i_station == 3 ) chamber = xAOD::L2MuonParameters::Chamber::EndcapExtra;
    if ( i_station == 4 ) chamber = xAOD::L2MuonParameters::Chamber::BarrelInner;
    superPoint = &(trackPattern.superPoints[chamber]);

    if ( superPoint->Npoint > 2 && superPoint->R > 0.) {
      if (chamber==3) { 
	InnerSlope = superPoint->Alin;
	InnerR = superPoint->R;
	InnerZ = superPoint->Z;	
      } if ( chamber==4 ) { 
	MiddleSlope = superPoint->Alin;
	MiddleIntercept = superPoint->R - MiddleSlope*superPoint->Z;
	MiddleR = superPoint->R;
	MiddleZ = superPoint->Z;
      } if ( chamber==5 ) {
	OuterR = superPoint->R;
	OuterZ = superPoint->Z;
      } if ( chamber==6 ) {
        EER = superPoint->R;
        EEZ = superPoint->Z;
      } if ( chamber==0 ) {//barrel inner
        EBIR = superPoint->R;
        EBIZ = superPoint->Z;
      }
      phim = superPoint->Phim;
    }
  } // end loop for stations

  if (tgcFitResult.isSuccess) {
    // TGC data properly read

    double phi1 = tgcFitResult.tgcMid1[1];
    double phi2 = tgcFitResult.tgcMid2[1];
    if ( tgcFitResult.tgcMid1[3]==0. || tgcFitResult.tgcMid2[3]==0. ) {
      if ( tgcFitResult.tgcMid1[3] != 0. ) phi = phi1;
      if ( tgcFitResult.tgcMid2[3] != 0. ) phi = phi2;
    } else if( phi1*phi2 < 0 && fabsf(phi1)>(CLHEP::pi/2.) ) {
      double tmp1 = (phi1>0)? phi1 - CLHEP::pi : phi1 + CLHEP::pi;
      double tmp2 = (phi2>0)? phi2 - CLHEP::pi : phi2 + CLHEP::pi;
      double tmp  = (tmp1+tmp2)/2.;
      phi  = (tmp>0.)? tmp - CLHEP::pi : tmp + CLHEP::pi;
    } else {      
      phi  = (phi2+phi1)/2.;     
    }
    
    if ( MiddleZ==0. ) {
      if ( tgcFitResult.tgcMid1[0]!=0 && tgcFitResult.tgcMid2[0]!=0 ) {
	trackPattern.etaMap = (tgcFitResult.tgcMid1[0]+tgcFitResult.tgcMid2[0])/2;
      } else {
	trackPattern.etaMap = (tgcFitResult.tgcMid1[0]!=0)? 
	  tgcFitResult.tgcMid1[0] : tgcFitResult.tgcMid2[0];
      }
    }
  
    double etaInner = 0.;
    double etaMiddle = 0.;
    if (MiddleZ!=0.) {
      double theta = atan(MiddleR/fabsf(MiddleZ));
      etaMiddle = -log(tan(theta/2.))*MiddleZ/fabsf(MiddleZ);
    }

    if (InnerZ!=0.) {
      double theta = atan(InnerR/fabsf(InnerZ));
      etaInner = -log(tan(theta/2.))*InnerZ/fabsf(InnerZ);
    }

    if (InnerZ!=0.) {
      if (tgcFitResult.tgcInn[3]!=0.) phi = tgcFitResult.tgcInn[1];

      trackPattern.etaMap = etaInner;      
      // correction by KN, implemented by MI
      if (MiddleZ!=0. && fabs(etaInner-etaMiddle)>0.03 &&
	  fabs(p_roi->eta()) > 1.9 && fabs(p_roi->eta()) < 2.1) {
	trackPattern.etaMap = etaMiddle;  
      }
    } else if (MiddleZ!=0.) {
      trackPattern.etaMap = etaMiddle;  
    }

    if ( phim > CLHEP::pi+0.1 ) phim = phim - 2*CLHEP::pi;
    if ( phim >= 0 ) trackPattern.phiMap = (phi>=0.)? phi - phim : phim - fabs(phi);
    else trackPattern.phiMap = phi - phim;
    
    int Octant = (int)(tgcFitResult.tgcMid1[1] / (CLHEP::pi/4.));
    double PhiInOctant = fabs(tgcFitResult.tgcMid1[1] - Octant * (CLHEP::pi/4.));
    if (PhiInOctant > (CLHEP::pi/8.)) PhiInOctant = (CLHEP::pi/4.) - PhiInOctant;
    
    trackPattern.endcapBeta = 0.0;
    trackPattern.phiMS      = phi;
    trackPattern.pt         = fabs(tgcFitResult.tgcPT);
    trackPattern.charge     = (tgcFitResult.tgcPT!=0.0)? tgcFitResult.tgcPT/fabs(tgcFitResult.tgcPT) : +1;
    
    trackPattern.phiBin = static_cast<int>(PhiInOctant * PHI_RANGE);
    trackPattern.etaBin = static_cast<int>((fabs(tgcFitResult.tgcMid1[0])-1.)/0.05);
    
    if ( muonRoad.LargeSmall==1 ){//Small
      int OctantSmall = Octant;
      double PhiInOctantSmall = PhiInOctant;
      if(tgcFitResult.tgcMid1[1]<0) PhiInOctantSmall = fabs(tgcFitResult.tgcMid1[1] - (OctantSmall-1)*(CLHEP::pi/4.));
      trackPattern.phiBin24 = PhiInOctantSmall * PHI_RANGE;
      trackPattern.smallLarge = 1;
    } 
    else {//Large
      double tgcPhi = tgcFitResult.tgcMid1[1] + CLHEP::pi/8;
      int OctantLarge = (int)(tgcPhi / (CLHEP::pi/4.));
      double PhiInOctantLarge = fabs(tgcPhi - OctantLarge * (CLHEP::pi/4));
      if (tgcPhi<0) PhiInOctantLarge = fabs(tgcPhi - (OctantLarge-1)*(CLHEP::pi/4.));
      trackPattern.phiBin24 = PhiInOctantLarge * PHI_RANGE;
      trackPattern.smallLarge = 0;
    }

  } else {
    // TGC data readout problem -> use RoI (eta, phi) and assume straight track

    trackPattern.etaMap = p_roi->eta();
    phi = p_roi->phi();

    if ( phim > CLHEP::pi+0.1 ) phim = phim - 2*CLHEP::pi;
    if ( phim >= 0 ) trackPattern.phiMap = (phi>=0.)? phi - phim : phim - fabs(phi);
    else trackPattern.phiMap = phi - phim;

    int Octant = (int)(p_roi->phi() / (CLHEP::pi/4.));
    double PhiInOctant = fabs(p_roi->phi() - Octant * (CLHEP::pi/4.));
    if (PhiInOctant > (CLHEP::pi/8.)) PhiInOctant = (CLHEP::pi/4.) - PhiInOctant;

    trackPattern.endcapBeta = 0.0;
    trackPattern.phiMS      = phi;
    trackPattern.pt         = 0;
    trackPattern.charge     = 0;
    
    trackPattern.phiBin = static_cast<int>(PhiInOctant * PHI_RANGE);
    trackPattern.etaBin = static_cast<int>((fabs(p_roi->eta())-1.)/0.05);
    
    if ( muonRoad.LargeSmall==1){//Small
      int OctantSmall = Octant;
      double PhiInOctantSmall = PhiInOctant;
      if(tgcFitResult.tgcMid1[1]<0) PhiInOctantSmall = fabs(tgcFitResult.tgcMid1[1] - (OctantSmall-1)*(CLHEP::pi/4.));
      trackPattern.phiBin24 = PhiInOctantSmall * PHI_RANGE;
      trackPattern.smallLarge = 1;
    }
    else {//Large
      double tgcPhi = tgcFitResult.tgcMid1[1] + CLHEP::pi/8;
      int OctantLarge = (int)(tgcPhi / (CLHEP::pi/4.));
      double PhiInOctantLarge = fabs(tgcPhi - OctantLarge * (CLHEP::pi/4));
      if (tgcPhi<0) PhiInOctantLarge = fabs(tgcPhi - (OctantLarge-1)*(CLHEP::pi/4.));
      trackPattern.phiBin24 = PhiInOctantLarge * PHI_RANGE;
      trackPattern.smallLarge = 0;
    }
  }

  if (MiddleZ && OuterZ) {
    double slope = (OuterR-MiddleR)/(OuterZ-MiddleZ);
    double inter = MiddleR - slope*MiddleZ;    
    
    trackPattern.endcapAlpha = m_ptEndcapLUT->alpha(MiddleZ,MiddleR,OuterZ,OuterR);
    trackPattern.slope       = slope; 
    trackPattern.intercept   = inter;    
    if (InnerR) {
      trackPattern.endcapBeta   = fabsf( atan(InnerSlope) - atan(slope) ); 
      trackPattern.deltaR       = slope * InnerZ + MiddleIntercept - InnerR;
      double sign               = trackPattern.deltaR / fabs(trackPattern.deltaR);
      trackPattern.endcapRadius = computeRadius(InnerSlope, InnerR,  InnerZ,
		                              slope,      MiddleR, MiddleZ,
					      sign);
    }
  } else {    
    if( trackPattern.pt >= 8. || !tgcFitResult.isSuccess) {
      if(MiddleZ) {
	double Ze = MiddleZ+(fabsf(MiddleZ)/MiddleZ)*1000.;
	double Re = MiddleSlope*(Ze) + MiddleIntercept;

	trackPattern.endcapAlpha = m_ptEndcapLUT->alpha(MiddleZ,MiddleR,Ze,Re);
	trackPattern.slope       = MiddleSlope;
	trackPattern.intercept   = MiddleIntercept;
      }      
    } 
    
    if (MiddleZ && InnerZ) {
      trackPattern.endcapBeta   = fabsf( atan(InnerSlope) - atan(MiddleSlope) );
      trackPattern.deltaR       = MiddleSlope*InnerZ + MiddleIntercept - InnerR;
      double sign               = trackPattern.deltaR / fabs(trackPattern.deltaR);
      trackPattern.endcapRadius = computeRadius(InnerSlope,  InnerR,  InnerZ,
					   MiddleSlope, MiddleR, MiddleZ,
					   sign);
    }
  }

  if ( (InnerZ && EEZ) && MiddleZ ){
    trackPattern.endcapRadius3P = computeRadius3Points(InnerZ, InnerR, EEZ, EER, MiddleZ, MiddleR);
  }
  
  if ( (EBIZ && EEZ) && MiddleZ ){
    trackPattern.endcapRadius3P = computeRadius3Points(EBIZ, EBIR, EEZ, EER, MiddleZ, MiddleR);
  }

  msg() << MSG::DEBUG << "... alpha/beta/endcapRadius/charge/s_address="
	<< trackPattern.endcapAlpha << "/" << trackPattern.endcapBeta << "/" << trackPattern.endcapRadius3P << "/" 
	<< trackPattern.charge << "/" << trackPattern.s_address << endreq;
  // 
  return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonSA::AlphaBetaEstimate::computeRadius(double InnerSlope, double InnerR, double InnerZ,
						       double MiddleSlope, double MiddleR, double MiddleZ,
						       double sign)
{
  double cr1 = 0.080/400;
  double cr2 = cr1;
  double x1 = InnerZ;
  if (fabs(x1)>=0.1) {
    double x2 = MiddleZ;
    double y1 = InnerR;
    double y2 = MiddleR;
    double A1 = InnerSlope;
    double A2 = MiddleSlope;
    if (!(fabs(MiddleSlope+999)<0.1)) {
      A2 = MiddleSlope;
      cr2 = cr1/10;
    }
    
    // find centre of circonference
    double xm = (x1+x2)/2.;
    double ym = (y1+y2)/2.;
    double c1 = (x2-x1)*xm+(y2-y1)*ym;
    double c2_1 = -x1-A1*y1;
    double c2_2 = -x2-A2*y2;
    double yR1 = (-c1-c2_1*(x2-x1))/(A1*(x2-x1)-(y2-y1));
    double yR2 = (-c1-c2_2*(x2-x1))/(A2*(x2-x1)-(y2-y1));
    double xR1 = -A1*yR1-c2_1;
    double xR2 = -A2*yR2-c2_2;
    
    double xR = ((1./cr1)*xR1+(1./cr2)*xR2)/((1./cr1)+(1./cr2));
    double yR = ((1./cr1)*yR1+(1./cr2)*yR2)/((1./cr1)+(1./cr2));
    double radius = 0.5*(sqrt((xR-x1)*(xR-x1)+(yR-y1)*(yR-y1))+sqrt((xR-x2)*(xR-x2)+(yR-y2)*(yR-y2)));
    return sign * radius;    
  }
  
  return 0.;
}

double TrigL2MuonSA::AlphaBetaEstimate::computeRadius3Points(double InnerZ, double InnerR, 
                                                               double EEZ, double EER,
                                                               double MiddleZ, double MiddleR)
{
  double radius_EE;

  double a3;
  
  double m = 0.;
  double cost = 0.;
  double x0 = 0., y0 = 0., x2 = 0., y2 = 0., x3 = 0., y3 = 0.;
  double tm = 0.;
  
  a3 = ( MiddleZ - InnerZ ) / ( MiddleR - InnerR );
    
  m = a3;
  cost = cos(atan(m));
  x2 = EER - InnerR;
  y2 = EEZ - InnerZ;
  x3 = MiddleR - InnerR;
  y3 = MiddleZ - InnerZ;

  tm = x2;
  x2 = ( x2   + y2*m)*cost;
  y2 = (-tm*m + y2  )*cost;
    
  tm = x3;
  x3 = ( x3   + y3*m)*cost;
  y3 = (-tm*m + y3  )*cost;
    
  x0 = x3/2.;
  y0 = (y2*y2 + x2*x2 -x2*x3)/(2*y2);
    
  radius_EE = sqrt(x0*x0 + y0*y0);
  return radius_EE;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
