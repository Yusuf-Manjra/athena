/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     EgammaShowerDepth.cxx
// PACKAGE:  Trigger/TrigTools/EgammaHitsShowerDepth.cxx
//
// AUTHOR:  Werner Spolidoro Freund (wsfreund@cern.ch)
//
// REFERENCES: Auxiliar Class for Hits Calibration (BASED ON OFFLINE)
//
// Updated: January 14, 2011
//  Modifications on EgammaHitsCalibration updated for offline rev 30069
//
// ********************************************************************


#include "EgammaHitsCalibration.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "GaudiKernel/MsgStream.h"


const CaloSampling::CaloSample EgammaHitsShowerDepth::m_samps[2][4] = 
{ { CaloSampling::PreSamplerB,
    CaloSampling::EMB1,
    CaloSampling::EMB2,
    CaloSampling::EMB3 },
  { CaloSampling::PreSamplerE,
    CaloSampling::EME1,
    CaloSampling::EME2,
    CaloSampling::EME3 }
};


/**
 * @brief Calculate the depth of the cluster.
 * @param aeta abs(eta) of the cluster.
 * @param cluster Cluster for which to calculate the depth.
 * @param log Stream for debug messages.
 */
float EgammaHitsShowerDepth::depth (const float &aeta,
                                    const float start_crack,
                                    const float end_crack,
                                    const CxxUtils::Array<2>& sampling_depth,
                                    const float etamax,
                                    const xAOD::TrigEMCluster* cluster) const
{

  float R[4];
  int si;

  if (aeta < start_crack) {
    barrelCoefs (aeta, R);
    si = 0;
  }
  else if (aeta > end_crack) {
    if (!endcapCoefs (aeta, sampling_depth, etamax, R))
      return 0;
    si = 1;
  }
  else
    return 0;

  double rsum = 0;
  double esum = 0;
  for (int i=0; i < 4; i++) {
    float e = cluster->energy (m_samps[si][i]);
    rsum += R[i] * e;
    esum += e;
  }
  
  if (esum == 0)
    return 0;

  float depth = rsum / esum;

  return depth;
}


/**
 * @brief Calculate the sampling depth coefficients for the barrel.
 * @param aeta abs(eta) of the cluster.
 * @param R[out] The set of coefficients per layer.
 */
void EgammaHitsShowerDepth::barrelCoefs (const float &aeta, float R[4]) const
{
//----------------------------------------------------------------------
//     APPROXIMATE Longitudinal segmentation of the EM Barrel
//     Inspired from M.Seman's former ACCOMPA.
//     - PURPOSE Computes the shower longitudinal barycenter
//     - Inputs: shower eta and energies in the compartments
//----------------------------------------------------------------------

  // Constants:                                                      
  static const float                                                 
   X0IN = 1.6,          // 1.7 X0 before strips                      
   X0S1 = 6.0,          //  6  X0 till end of strips                 
//   X0S2 = 24.0,         // 24  X0 till end of S2                   
//   X0S3 = 2.0,          //  2  X0 at least in S3                   
//   RIN  = 150.0024,     // where strips start (cm)                 
//   ROUT = 197.0482,     // where S3 ends (cm)                      
//   RCLR = 0.7,          // clearance for resistive ink (cm)        
   //BEND1 = 152.1000,    // first 3 bends radii (cm)                  
   //BEND2 = 155.9659,                                                 
   //BEND3 = 159.7202,                                                 
   ETASTEP = 0.8,       // where lead changes                        
   //DENSHI = 2.137,      // cm/X0 high density                        
   //DENSLO = 2.695,      // cm/X0  low density                        
   X0THI = 23.74,       // total depth high density part (not indep?)
   X0TLO = 19.177;      // total depth  low density part (not indep?)
                                                                     
   float /*DENS,*/ X0T/*, RLO, RHI*/ ;
   float R12, R23;

   if ( aeta < ETASTEP ) {
     //DENS = DENSHI;
     X0T = X0THI;
     //RLO = BEND2;             // break strips between bends 2 and 3
     //RHI = BEND3;
    }else{
     //DENS = DENSLO;
     X0T = X0TLO;
     //RLO = BEND1;             // break strips between bends 1 and 2
     //RHI = BEND2;
   }

   R[0] = (X0IN) * cosh(aeta);

   //     R12 is radius at transition strips->Sampling2		
   //     R23 is radius at transition Sampling2->Sampling3	
	
   R12 = X0S1;

   //..   total depth of calo at given eta

   float RADLTOT = X0T*cosh(aeta);

   //..   radius where S2 ends. Make sure there are 2X0 left for S3

   if (aeta < 0.5){
     R23 = 22 * cosh(aeta) ;
   }else{
     R23 = 25;
   }

   R[1] = R[0]  + (R12 - R[0])/2 ;
   R[2] = R12 + (R23 - R12)/2;
   R[3] = R23 + (RADLTOT - R23)/2;
}


/**
 * @brief Calculate the sampling depth coefficients for the endcap.
 * @param aeta abs(eta) of the cluster.
 * @param sampling_depth Array of sampling depths per bin/sampling in the EC.
 * @param etamax Maximum eta value in @a sampling_depth.
 * @param R[out] The set of coefficients per layer.
 */
bool EgammaHitsShowerDepth::endcapCoefs (const float &aeta,
                                         const CxxUtils::Array<2>& sampling_depth,
                                         const float etamax,
                                         float R[4]) const
{
  unsigned int ibin = (static_cast<unsigned int> (aeta / etamax * 100)) ;
  if (ibin >= sampling_depth.size())
    return false;

  for (int i=0; i < 4; i++)
    R[i] = sampling_depth[ibin][i+1];
  return true;
}

