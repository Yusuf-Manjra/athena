/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBPHYSHYPO_CONSTANTS_H
#define TRIGBPHYSHYPO_CONSTANTS_H

constexpr double EPSILON     =      1.0E-6;
constexpr double PROTONMASS  =    938.272;    //MeV
constexpr double EMASS       =      0.511003;//MeV
constexpr double MUMASS      =    105.65837; //MeV
constexpr double PIMASS      =    139.57;    //MeV
constexpr double JPSIMASS    =   3096.916;   //MeV
constexpr double KMASS       =    493.677;   //MeV
constexpr double PHIMASS     =   1020.;      //MeV
constexpr double DSMASS      =   1968.2;     //MeV
constexpr double KPLUSMASS   =    493.67700; //MeV
constexpr double BPLUSMASS   =   5279.13;    //MeV
constexpr double BSMASS      =   5369.3;     //MeV
constexpr double KSTARMASS   =    891.66;    //MeV
constexpr double BDMASS      =   5279.5;     //MeV
constexpr double LAMBDAMASS  =   1115.683;   //MeV
constexpr double LAMBDABMASS =   5620.2;     //MeV
constexpr double BCMASS      =   6277.0;     //MeV

/// list of decay constexprants BMuMuX
constexpr int di_to_muons = 0;
constexpr int b_to_K = 1;           // to recognize type of decay B+/- ->K+/-      mu+ mu-
constexpr int bD_to_Kstar = 2; 		// to recognize type of decay Bd   ->K*(K+Pi-) mu+ mu-
constexpr int bS_to_Phi = 3; 		// to recognize type of decay Bs   ->Phi(K+K-) mu+ mu-
constexpr int lB_to_L = 4; 		    // to recognize type of decay Lb   ->L(PPi-)   mu+ mu-
constexpr int bC_to_PiPi = 5; 	// to recognize type of decay Bc   ->D+(K-pi+pi+) mu+ mu-
constexpr int di_to_electrons = 6;

// PDG'2020
struct PDG20 {
  static constexpr double
    mElectron   =    0.5109989461,
    mMuon       =  105.6583745,
    mPion       =  139.57039,
    mPion0      =  134.9768,
    mKaon       =  493.677,
    mK_S0       =  497.611,
    mPhi1020    = 1019.461,
    mProton     =  938.2720813,
    mLambda0    = 1115.683,
    mJpsi       = 3096.900,
    mPsi2S      = 3686.097,
    mB          = 5279.32,
    mB0         = 5279.63,
    mB_s0       = 5366.89,
    mB_c        = 6274.9,
    mLambda_b0  = 5619.60;
};

#endif
