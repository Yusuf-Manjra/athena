/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: tabstop=2:shiftwidth=2:expandtab
// -*- C++ -*-//
/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigEFBMuMuXFex.cxx
 **
 **   Description: EF hypothesis algorithms for
 **                B^0_{s,d},\Lambda_b \to X \mu^+ \mu^-
 **
 **
 **   Author: J.Kirk
 **   Author: Semen Turchikhin <Semen.Turchikhin@cern.ch>
 **
 **   Created:   12.09.2007
 **   Modified:  08.04.2012
 **
 **   Modified:  26.09.2012 : "B_c -> D_s(*) (->Phi(->K+K-)) mu+mu-" added (Leonid Gladilin <gladilin@mail.cern.ch>)
 **
 **
 ***************************************************************************/

#include "TrigMuonEvent/CombinedMuonFeature.h"

#include "TrigEFBMuMuXFex.h"

#include "TrigParticle/TrigEFBphysContainer.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include <math.h>
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "AthenaKernel/Timeout.h"

#include "MuidEvent/MuidTrackContainer.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "Particle/TrackParticleContainer.h"
#include "VxVertex/VxCandidate.h"
//#include "TrigVKalFitter/TrigVKalFitter.h"
//#include "TrigVKalFitter/VKalVrtAtlas.h"

//#include "TrigNavigation/Navigation.h"

//class ISvcLocator;

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"


/*----------------------------------------------------------------------------*/
TrigEFBMuMuXFex::TrigEFBMuMuXFex(const std::string & name, ISvcLocator* pSvcLocator):
HLT::ComboAlgo(name, pSvcLocator),
m_fitterSvc("Trk::TrkVKalVrtFitter/VertexFitterTool",this),
//m_trigBphysColl_b(NULL),
//m_trigBphysColl_X(NULL),
mTrigBphysColl_b(NULL),
mTrigBphysColl_X(NULL)

{
    declareProperty("AcceptAll",    m_acceptAll=true); // Should we just accept all events
    
    // sign & mass cuts
    //   declareProperty("CutMuonTightness", m_cutMuon = 0.01);                               // default = 0.01
    declareProperty("OppositeSign", m_oppositeCharge = true);                            // deafult = true
    declareProperty("LowerMuMuMassCut", m_lowerMuMuMassCut = 100.);                      // default = 100.0 MeV
    declareProperty("UpperMuMuMassCut", m_upperMuMuMassCut = 5500.);                      // default = 5500.0 MeV
    //   declareProperty("LowerMuVtxMassCut", m_lowerMuVtxMassCut =100.);                    // default = 100.0 MeV
    //   declareProperty("UpperMuVtxMassCut", m_upperMuVtxMassCut = 5500.);                    // default = 5500.0 MeV
    declareProperty("MuVtxChi2Cut", m_muVtxChi2Cut = 40.);                              // default = 40.0
    
    // Maximum number of decay candidates (2 mu + 1 or 2 tracks) to test - protection against TimeOuts
    declareProperty("MaxNcombinations", m_maxNcombinations = 50000);
    
    // B+->K+MuMu cuts
    declareProperty("DoB_KMuMuDecay", m_doB_KMuMuDecay = true);                          //Proceed B->KMuMu part of algorithm
    declareProperty("LowerKMuMuMassCut", m_lowerKMuMuMassCut = 4500.);                    // default = 4500.0 MeV
    declareProperty("UpperKMuMuMassCut", m_upperKMuMuMassCut = 5900.);                    // default = 5900.0 MeV
    //   declareProperty("LowerB_KMuMuMassCutVtxOff", m_lowerB_KMuMuMassCutVtxOff = 4500.);  // default = 4500.0 MeV
    //   declareProperty("UpperB_KMuMuMassCutVtxOff", m_upperB_KMuMuMassCutVtxOff= 5900.);  // default = 5900.0 MeV
    //   declareProperty("LowerBVtxMassCut", m_lowerBVtxMassCut = 4500.);                      // default = 4500.0 MeV
    //   declareProperty("UpperBVtxMassCut", m_upperBVtxMassCut = 5900.);                      // default = 5900.0 MeV
    declareProperty("DoB_KMuMuVertexing", m_doB_KMuMuVertexing = true);
    declareProperty("BVtxChi2Cut", m_bVtxChi2Cut = 300.);                                // default = 300.0
    
    // Bd->K*MuMu cuts
    declareProperty("DoBd_KstarMuMuDecay", m_doBd_KstarMuMuDecay = true);                //Proceed Bd->K*MuMu part of algorithm
    declareProperty("LowerKstar_KaonMassCut", m_lowerKstar_KaonMassCut = 600.);          // default = 600.0 MeV
    declareProperty("UpperKstar_KaonMassCut", m_upperKstar_KaonMassCut = 1500.);          // default = 1500.0 MeV
    declareProperty("LowerBd_KstarMuMuMassCut", m_lowerBd_KstarMuMuMassCut= 4300.);      // default = 4600.0 MeV
    declareProperty("UpperBd_KstarMuMuMassCut", m_upperBd_KstarMuMuMassCut =6300.);      // default = 6300.0 MeV
    //   declareProperty("LowerBd_KstarMuMuMassCutVtxOff", m_lowerBd_KstarMuMuMassCutVtxOff = 4600.);  // default = 4600.0 MeV
    //   declareProperty("UpperBd_KstarMuMuMassCutVtxOff", m_upperBd_KstarMuMuMassCutVtxOff= 6300.);  // default = 6300.0 MeV
    //   declareProperty("LowerKstarVtxMassCut", m_lowerKstarVtxMassCut = 600.);              // default = 600.0 MeV
    //   declareProperty("UpperKstarVtxMassCut", m_upperKstarVtxMassCut= 1200.);              // default = 1200.0 MeV
    //   declareProperty("LowerBdVtxMassCut", m_lowerBdVtxMassCut = 4600.);                    // default = 4600.0 MeV
    //   declareProperty("UpperBdVtxMassCut", m_upperBdVtxMassCut = 6000.);                    // default = 6000.0 MeV
    declareProperty("DoKstar_KPiVertexing", m_doKstar_KPiVertexing = true);
    declareProperty("DoBd_KstarMuMuVertexing", m_doBd_KstarMuMuVertexing = true);
    declareProperty("KstarVtxChi2Cut", m_kStarVtxChi2Cut = 60.);                        // default = 60.0
    declareProperty("BdVtxChi2Cut", m_bDVtxChi2Cut = 60.);                              // default = 60.0
    
    // Bs->PhiMuMu cuts
    declareProperty("DoBs_Phi1020MuMuDecay", m_doBs_Phi1020MuMuDecay = true);            //Proceed Bs->PhiMuMu part of algorithm
    declareProperty("LowerPhi1020_KaonMassCut", m_lowerPhi1020_KaonMassCut  = 990.);      // default = 990.0 MeV
    declareProperty("UpperPhi1020_KaonMassCut", m_upperPhi1020_KaonMassCut = 1050.);      // default = 1050.0 MeV
    declareProperty("LowerBs_Phi1020MuMuMassCut", m_lowerBs_Phi1020MuMuMassCut = 5000.);  // default = 5000.0 MeV
    declareProperty("UpperBs_Phi1020MuMuMassCut", m_upperBs_Phi1020MuMuMassCut = 5800.);  // default = 5800.0 MeV
    //   declareProperty("LowerBs_Phi1020MuMuMassCutVtxOff", m_lowerBs_Phi1020MuMuMassCutVtxOff = 5000.);  // default = 5000.0 MeV
    //   declareProperty("UpperBs_Phi1020MuMuMassCutVtxOff", m_upperBs_Phi1020MuMuMassCutVtxOff = 5800.);  // default = 5800.0 MeV
    //   declareProperty("LowerPhi1020VtxMassCut", m_lowerPhi1020VtxMassCut = 990.);          // default = 990.0 MeV
    //   declareProperty("UpperPhi1020VtxMassCut", m_upperPhi1020VtxMassCut = 1060.);          // default = 1060.0 MeV
    //   declareProperty("LowerBsVtxMassCut", m_lowerBsVtxMassCut = 5000.);                    // default = 5000.0 MeV
    //   declareProperty("UpperBsVtxMassCut", m_upperBsVtxMassCut = 5800.);                    // default = 5800.0 MeV
    declareProperty("DoPhi1020_KKVertexing", m_doPhi1020_KKVertexing = true);
    declareProperty("DoBs_Phi1020MuMuVertexing", m_doBs_Phi1020MuMuVertexing = true);
    declareProperty("Phi1020VtxChi2Cut", m_phi1020VtxChi2Cut = 60.);                    // default = 60.0
    declareProperty("BsVtxChi2Cut", m_bSVtxChi2Cut = 60.);                              // default = 60.0
    
    // Lb->LMuMu cuts // cuts optimalization in progress
    declareProperty("DoLb_LambdaMuMuDecay", m_doLb_LambdaMuMuDecay = true);              //Proceed Lb->LMuMu part of algorithm
    declareProperty("LowerLambda_PrPiMassCut", m_lowerLambda_PrPiMassCut = 1040.);           // optimal = 1080.0 MeV
    declareProperty("UpperLambda_PrPiMassCut", m_upperLambda_PrPiMassCut = 1220.);       // default = 2 500.0 MeV
    declareProperty("LowerLb_LambdaMuMuMassCut", m_lowerLb_LambdaMuMuMassCut = 5090.);    // default = 1 000.0 MeV
    declareProperty("UpperLb_LambdaMuMuMassCut", m_upperLb_LambdaMuMuMassCut = 6270.);    // default = 10 000.0 MeV
    //   declareProperty("LowerLb_LambdaMuMuMassCutVtxOff", m_lowerLb_LambdaMuMuMassCutVtxOff = 1080.); // default = 2 000.0 MeV
    //   declareProperty("UpperLb_LambdaMuMuMassCutVtxOff", m_upperLb_LambdaMuMuMassCutVtxOff = 1180.); // default = 8 000.0 MeV
    //   declareProperty("LowerLambdaVtxMassCut", m_lowerLambdaVtxMassCut = 1070.);            // default = 200.0 MeV
    //   declareProperty("UpperLambdaVtxMassCut", m_upperLambdaVtxMassCut = 1160.);            // default = 2 000.0 MeV
    declareProperty("DoLambda_PPiVertexing", m_doLambda_PPiVertexing = true);
    declareProperty("DoLb_LambdaMuMuVertexing", m_doLb_LambdaMuMuVertexing = true);
    declareProperty("LambdaVtxChi2Cut", m_lambdaVtxChi2Cut = 100.);                      // default = 500.0 MeV
    //   declareProperty("LowerLbVtxMassCut", m_lowerLbVtxMassCut = 4800.);                    // default = 1 000.0 MeV
    //   declareProperty("UpperLbVtxMassCut", m_upperLbVtxMassCut = 6100.);                    // default = 10 000.0 MeV
    declareProperty("LbVtxChi2Cut", m_lBVtxChi2Cut = 100.);                              // default = 500.0
    //   declareProperty("LbVtxDistanceCut", m_lBVtxDistanceCut = 0.);                      // default = 0.0
    //   declareProperty("PiImpactCut", m_piImpactCut = 0.);                                // default = 0.0
    //   declareProperty("PrImpactCut", m_prImpactCut = 0.);                                // default = 0.0
    
    // Bc->DsMuMu cuts
    declareProperty("DoBc_DsMuMuDecay", m_doBc_DsMuMuDecay = true);             // Proceed Bc->DsMuMu part of algorithm
    declareProperty("LowerPhiDs_MassCut", m_lowerPhiDs_MassCut  = 980.);        // default =  980.0 MeV
    declareProperty("UpperPhiDs_MassCut", m_upperPhiDs_MassCut = 1080.);        // default = 1080.0 MeV
    declareProperty("LowerDs_MassCut", m_lowerDs_MassCut = 1600.);              // default = 1600.0 MeV
    declareProperty("UpperDs_MassCut", m_upperDs_MassCut = 2400.);              // default = 2400.0 MeV
    declareProperty("LowerBc_DsMuMuMassCut", m_lowerBc_DsMuMuMassCut = 1800.);  // default = 1800.0 MeV
    declareProperty("UpperBc_DsMuMuMassCut", m_upperBc_DsMuMuMassCut = 7050.);  // default = 7050.0 MeV
    declareProperty("DoDs_Vertexing", m_doDs_Vertexing = true);
    declareProperty("DoBc_DsMuMuVertexing", m_doBc_DsMuMuVertexing = true);
    declareProperty("DsVtxChi2Cut", m_DsVtxChi2Cut =  90.);                    // default =  90.0
    declareProperty("BcVtxChi2Cut", m_bCVtxChi2Cut = 120.);                    // default = 120.0
    
    // Monitoring variables
    //   General
    declareMonitoredStdContainer("Errors",     mon_Errors,     AutoClear);
    declareMonitoredStdContainer("Acceptance", mon_Acceptance, AutoClear);
    declareMonitoredVariable("nTriedCombinations", mon_nTriedCombinations );
    //   Timing
    declareMonitoredVariable("TotalRunTime",  mon_TotalRunTime);
    declareMonitoredVariable("VertexingTime", mon_VertexingTime);
    //   RoIs
    declareMonitoredStdContainer("RoI_RoI1Eta", mon_RoI_RoI1Eta, AutoClear);
    declareMonitoredStdContainer("RoI_RoI2Eta", mon_RoI_RoI2Eta, AutoClear);
    declareMonitoredStdContainer("RoI_RoI1Phi", mon_RoI_RoI1Phi, AutoClear);
    declareMonitoredStdContainer("RoI_RoI2Phi", mon_RoI_RoI2Phi, AutoClear);
    declareMonitoredStdContainer("RoI_dEtaRoI", mon_RoI_dEtaRoI, AutoClear);
    declareMonitoredStdContainer("RoI_dPhiRoI", mon_RoI_dPhiRoI, AutoClear);
    //   DiMuon
    declareMonitoredVariable("DiMu_n", mon_DiMu_n);
    declareMonitoredStdContainer("DiMu_Pt_Mu1",       mon_DiMu_Pt_Mu1,       AutoClear);
    declareMonitoredStdContainer("DiMu_Pt_Mu2",       mon_DiMu_Pt_Mu2,       AutoClear);
    declareMonitoredStdContainer("DiMu_Eta_Mu1",      mon_DiMu_Eta_Mu1,      AutoClear);
    declareMonitoredStdContainer("DiMu_Eta_Mu2",      mon_DiMu_Eta_Mu2,      AutoClear);
    declareMonitoredStdContainer("DiMu_Phi_Mu1",      mon_DiMu_Phi_Mu1,      AutoClear);
    declareMonitoredStdContainer("DiMu_Phi_Mu2",      mon_DiMu_Phi_Mu2,      AutoClear);
    declareMonitoredStdContainer("DiMu_dEtaMuMu",     mon_DiMu_dEtaMuMu,     AutoClear);
    declareMonitoredStdContainer("DiMu_dPhiMuMu",     mon_DiMu_dPhiMuMu,     AutoClear);
    declareMonitoredStdContainer("DiMu_pTsumMuMu",    mon_DiMu_pTsumMuMu,    AutoClear);
    declareMonitoredStdContainer("DiMu_InvMassMuMu",  mon_DiMu_InvMassMuMu, AutoClear);
    declareMonitoredStdContainer("DiMu_VtxMassMuMu",  mon_DiMu_VtxMassMuMu, AutoClear);
    declareMonitoredStdContainer("DiMu_Chi2MuMu",     mon_DiMu_Chi2MuMu,    AutoClear);
    //   Tracks
    declareMonitoredVariable("Tracks_n", mon_Tracks_n);
    declareMonitoredStdContainer("Tracks_Eta", mon_Tracks_Eta, AutoClear);
    declareMonitoredStdContainer("Tracks_Pt",  mon_Tracks_Pt,  AutoClear);
    declareMonitoredStdContainer("Tracks_Phi", mon_Tracks_Phi, AutoClear);
    //   B+
    declareMonitoredVariable("BMuMuK_n", mon_BMuMuK_n);
    declareMonitoredStdContainer("BMuMuK_Pt_K",      mon_BMuMuK_Pt_K,      AutoClear);
    declareMonitoredStdContainer("BMuMuK_Eta_K",     mon_BMuMuK_Eta_K,     AutoClear);
    declareMonitoredStdContainer("BMuMuK_Phi_K",     mon_BMuMuK_Phi_K,     AutoClear);
    declareMonitoredStdContainer("BMuMuK_InvMass_B", mon_BMuMuK_InvMass_B, AutoClear);
    declareMonitoredStdContainer("BMuMuK_VtxMass_B", mon_BMuMuK_VtxMass_B, AutoClear);
    declareMonitoredStdContainer("BMuMuK_Chi2_B",    mon_BMuMuK_Chi2_B,    AutoClear);
    //   Bd
    declareMonitoredVariable("BdMuMuKs_n", mon_BdMuMuKs_n);
    declareMonitoredStdContainer("BdMuMuKs_Pt_K",          mon_BdMuMuKs_Pt_K,          AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Eta_K",         mon_BdMuMuKs_Eta_K,         AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Phi_K",         mon_BdMuMuKs_Phi_K,         AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Pt_Pi",         mon_BdMuMuKs_Pt_Pi,         AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Eta_Pi",        mon_BdMuMuKs_Eta_Pi,        AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Phi_Pi",        mon_BdMuMuKs_Phi_Pi,        AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_InvMass_Kstar", mon_BdMuMuKs_InvMass_Kstar, AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_VtxMass_Kstar", mon_BdMuMuKs_VtxMass_Kstar, AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Chi2_Kstar",    mon_BdMuMuKs_Chi2_Kstar,    AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_InvMass_Bd",    mon_BdMuMuKs_InvMass_Bd,    AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_VtxMass_Bd",    mon_BdMuMuKs_VtxMass_Bd,    AutoClear);
    declareMonitoredStdContainer("BdMuMuKs_Chi2_Bd",       mon_BdMuMuKs_Chi2_Bd,       AutoClear);
    //   Bs
    declareMonitoredVariable("BsMuMuPhi_n", mon_BsMuMuPhi_n);
    declareMonitoredStdContainer("BsMuMuPhi_Pt_K1",           mon_BsMuMuPhi_Pt_K1,           AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Eta_K1",          mon_BsMuMuPhi_Eta_K1,          AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Phi_K1",          mon_BsMuMuPhi_Phi_K1,          AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Pt_K2",           mon_BsMuMuPhi_Pt_K2,           AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Eta_K2",          mon_BsMuMuPhi_Eta_K2,          AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Phi_K2",          mon_BsMuMuPhi_Phi_K2,          AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_InvMass_Phi1020", mon_BsMuMuPhi_InvMass_Phi1020, AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_VtxMass_Phi1020", mon_BsMuMuPhi_VtxMass_Phi1020, AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Chi2_Phi1020",    mon_BsMuMuPhi_Chi2_Phi1020,    AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_InvMass_Bs",      mon_BsMuMuPhi_InvMass_Bs,      AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_VtxMass_Bs",      mon_BsMuMuPhi_VtxMass_Bs,      AutoClear);
    declareMonitoredStdContainer("BsMuMuPhi_Chi2_Bs",         mon_BsMuMuPhi_Chi2_Bs,         AutoClear);
    //   Lambda_b
    declareMonitoredVariable("LbMuMuLambda_n", mon_LbMuMuLambda_n);
    declareMonitoredStdContainer("LbMuMuLambda_Pt_P",           mon_LbMuMuLambda_Pt_P,           AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Eta_P",          mon_LbMuMuLambda_Eta_P,          AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Phi_P",          mon_LbMuMuLambda_Phi_P,          AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Pt_Pi",          mon_LbMuMuLambda_Pt_Pi,          AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Eta_Pi",         mon_LbMuMuLambda_Eta_Pi,         AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Phi_Pi",         mon_LbMuMuLambda_Phi_Pi,         AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_InvMass_Lambda", mon_LbMuMuLambda_InvMass_Lambda, AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_VtxMass_Lambda", mon_LbMuMuLambda_VtxMass_Lambda, AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Chi2_Lambda",    mon_LbMuMuLambda_Chi2_Lambda,    AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_InvMass_Lb",     mon_LbMuMuLambda_InvMass_Lb,     AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_VtxMass_Lb",     mon_LbMuMuLambda_VtxMass_Lb,     AutoClear);
    declareMonitoredStdContainer("LbMuMuLambda_Chi2_Lb",        mon_LbMuMuLambda_Chi2_Lb,        AutoClear);
    //   Bc
    declareMonitoredVariable("BcMuMuDs_n", mon_BcMuMuDs_n);
    declareMonitoredStdContainer("BcMuMuDs_Pt_K1",           mon_BcMuMuDs_Pt_K1,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Eta_K1",          mon_BcMuMuDs_Eta_K1,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Phi_K1",          mon_BcMuMuDs_Phi_K1,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Pt_K2",           mon_BcMuMuDs_Pt_K2,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Eta_K2",          mon_BcMuMuDs_Eta_K2,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Phi_K2",          mon_BcMuMuDs_Phi_K2,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Pt_pi",           mon_BcMuMuDs_Pt_pi,           AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Eta_pi",          mon_BcMuMuDs_Eta_pi,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Phi_pi",          mon_BcMuMuDs_Phi_pi,          AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_InvMass_PhiDs",   mon_BcMuMuDs_InvMass_PhiDs,   AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_InvMass_Ds",      mon_BcMuMuDs_InvMass_Ds,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_VtxMass_Ds",      mon_BcMuMuDs_VtxMass_Ds,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Chi2_Ds",         mon_BcMuMuDs_Chi2_Ds,         AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_InvMass_Bc",      mon_BcMuMuDs_InvMass_Bc,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_VtxMass_Bc",      mon_BcMuMuDs_VtxMass_Bc,      AutoClear);
    declareMonitoredStdContainer("BcMuMuDs_Chi2_Bc",         mon_BcMuMuDs_Chi2_Bc,         AutoClear);
    
    
}


/*----------------------------------------------------------------------------*/
TrigEFBMuMuXFex::~TrigEFBMuMuXFex()
{ }


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBMuMuXFex::hltInitialize()
{
    msg() << MSG::INFO << "Running TrigEFBMuMuXFex::hltInitialize" << endreq;
    
    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "Initialization completed successfully:" << endreq;
        msg() << MSG::DEBUG << "AcceptAll          = "
        << (m_acceptAll==true ? "True" : "False") << endreq;
        msg() << MSG::INFO << "MaxNcombinations            = " << m_maxNcombinations << endreq;
        
        msg() << MSG::DEBUG << "Activated decays:" << endreq;
        msg() << MSG::DEBUG << "    B+ -> mu mu K+ : " << (m_doB_KMuMuDecay==true ? "True" : "False") << endreq;
        msg() << MSG::DEBUG << "    Bd -> mu mu K*(K+ Pi-) : " << (m_doBd_KstarMuMuDecay==true ? "True" : "False") << endreq;
        msg() << MSG::DEBUG << "    Bs -> mu mu Phi(K+ K-) : " << (m_doBs_Phi1020MuMuDecay==true ? "True" : "False") << endreq;
        msg() << MSG::DEBUG << "    Lambda_b -> mu mu Lambda(P Pi) : " << (m_doLb_LambdaMuMuDecay==true ? "True" : "False") << endreq;
        msg() << MSG::DEBUG << "    Bc -> mu mu Ds(Phi pi) : " << (m_doBc_DsMuMuDecay==true ? "True" : "False") << endreq;
        
        msg() << MSG::DEBUG << "OppositeCharge     = "
        << (m_oppositeCharge==true ? "True" : "False") << endreq;
        
    }
    
    // setting up timers
    if ( timerSvc() ) {
        m_TotTimer    = addTimer("EFBMuMuXFexTot");
        m_VtxFitTimer = addTimer("EFBMuMuXFexVtxFit");
    }
    
    // retrieving the vertex fitting tool
    if (m_fitterSvc.retrieve().isFailure()) {
        msg() << MSG::ERROR << "Can't find Trk::TrkVKalVrtFitter" << endreq;
        return StatusCode::SUCCESS;
    } else {
        if (msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << "Trk::TrkVKalVrtFitter found" << endreq;
        }
        m_VKVFitter = dynamic_cast< Trk::TrkVKalVrtFitter* > (&(*m_fitterSvc));
    }
    
    // set counters
    m_lastEvent=-1;
    m_lastEventPassed=-1;
    m_countTotalEvents=0;
    m_countTotalRoI=0;
    m_countPassedEvents=0;
    m_countPassedRoIs=0;
    
    m_lastEventPassedBplus=-1;
    m_lastEventPassedBd=-1;
    m_lastEventPassedBs=-1;
    m_lastEventPassedLb=-1;
    
    m_countPassedEventsBplus=0;
    m_countPassedEventsBs=0;
    m_countPassedEventsBd=0;
    m_countPassedEventsLb=0;
    m_countPassedEventsBc=0;
    
    m_countPassedMuMuID=0;
    m_countPassedMuMuOS=0;
    m_countPassedMuMuMass=0;
    m_countPassedMuMuVtx=0;
    m_countPassedMuMuVtxChi2=0;
    
    m_countPassedBplusMass=0;
    m_countPassedBplusVtx=0;
    m_countPassedBplusVtxChi2=0;
    
    m_countPassedKstarMass=0;
    m_countPassedBdMass=0;
    m_countPassedKstarVtx=0;
    m_countPassedKstarVtxChi2=0;
    m_countPassedBdVtx=0;
    m_countPassedBdVtxChi2=0;
    
    m_countPassedPhi1020Mass=0;
    m_countPassedBsMass=0;
    m_countPassedPhi1020Vtx=0;
    m_countPassedPhi1020VtxChi2=0;
    m_countPassedBsVtx=0;
    m_countPassedBsVtxChi2=0;
    
    m_countPassedLambdaMass=0;
    m_countPassedLbMass=0;
    m_countPassedLambdaVtx=0;
    m_countPassedLambdaVtxChi2=0;
    m_countPassedLbVtx=0;
    m_countPassedLbVtxChi2=0;
    
    m_countPassedPhiDsMass=0;
    m_countPassedDsMass=0;
    m_countPassedBcMass=0;
    m_countPassedDsVtx=0;
    m_countPassedDsVtxChi2=0;
    m_countPassedBcVtx=0;
    m_countPassedBcVtxChi2=0;
    
    
    return HLT::OK;
}


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBMuMuXFex::hltFinalize()
{
    
    msg() << MSG::INFO << "Running TrigEFBMuMuXFex::hltFinalize" << endreq;
    
    msg() << MSG::INFO << "|----------------------- SUMMARY FROM TrigEFBMuMuXFex -------------|" << endreq;
    msg() << MSG::INFO << "Run on events/RoIs      " << m_countTotalEvents << "/" << m_countTotalRoI <<  endreq;
    msg() << MSG::INFO << "Passed events/RoIs      " << m_countPassedEvents << "/" << m_countPassedRoIs <<  endreq;
    msg() << MSG::INFO << "Evts Passed B+:         " << m_countPassedEventsBplus << endreq;
    msg() << MSG::INFO << "Evts Passed Bd:         " << m_countPassedEventsBd << endreq;
    msg() << MSG::INFO << "Evts Passed Bs:         " << m_countPassedEventsBs << endreq;
    msg() << MSG::INFO << "Evts Passed Lambda_b:   " << m_countPassedEventsLb << endreq;
    msg() << MSG::INFO << "Evts Passed Bc:         " << m_countPassedEventsBc << endreq;
    msg() << MSG::INFO << std::endl << endreq;
    msg() << MSG::INFO << "PassedMuMuID:           " << m_countPassedMuMuID << endreq;
    msg() << MSG::INFO << "PassedMuMuOS:           " << m_countPassedMuMuOS << endreq;
    msg() << MSG::INFO << "PassedMuMuMass:         " << m_countPassedMuMuMass << endreq;
    msg() << MSG::INFO << "PassedMuMuVtx:          " << m_countPassedMuMuVtx << endreq;
    msg() << MSG::INFO << "PassedMuMuVtxChi2:      " << m_countPassedMuMuVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedBplusMass:        " << m_countPassedBplusMass << endreq;
    msg() << MSG::INFO << "PassedBplusVtx:         " << m_countPassedBplusVtx << endreq;
    msg() << MSG::INFO << "PassedBplusVtxChi2:     " << m_countPassedBplusVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedKstarMass:        " << m_countPassedKstarMass << endreq;
    msg() << MSG::INFO << "PassedKstarVtx:         " << m_countPassedKstarVtx << endreq;
    msg() << MSG::INFO << "PassedKstarVtxChi2:     " << m_countPassedKstarVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedBdMass:           " << m_countPassedBdMass << endreq;
    msg() << MSG::INFO << "PassedBdVtx:            " << m_countPassedBdVtx << endreq;
    msg() << MSG::INFO << "PassedBdVtxChi2:        " << m_countPassedBdVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedPhi1020Mass:      " << m_countPassedPhi1020Mass << endreq;
    msg() << MSG::INFO << "PassedPhi1020Vtx:       " << m_countPassedPhi1020Vtx << endreq;
    msg() << MSG::INFO << "PassedPhi1020VtxChi2:   " << m_countPassedPhi1020VtxChi2 << endreq;
    msg() << MSG::INFO << "PassedBsMass:           " << m_countPassedBsMass << endreq;
    msg() << MSG::INFO << "PassedBsVtx:            " << m_countPassedBsVtx << endreq;
    msg() << MSG::INFO << "PassedBsVtxChi2:        " << m_countPassedBsVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedLambdaMass:       " << m_countPassedLambdaMass << endreq;
    msg() << MSG::INFO << "PassedLambdaVtx:        " << m_countPassedLambdaVtx << endreq;
    msg() << MSG::INFO << "PassedLambdaVtxChi2:    " << m_countPassedLambdaVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedLbMass:           " << m_countPassedLbMass << endreq;
    msg() << MSG::INFO << "PassedLbVtx:            " << m_countPassedLbVtx << endreq;
    msg() << MSG::INFO << "PassedLbVtxChi2:        " << m_countPassedLbVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedPhiDsMass:        " << m_countPassedPhiDsMass << endreq;
    msg() << MSG::INFO << "PassedDsMass:           " << m_countPassedDsMass << endreq;
    msg() << MSG::INFO << "PassedDsVtx:            " << m_countPassedDsVtx << endreq;
    msg() << MSG::INFO << "PassedDsVtxChi2:        " << m_countPassedDsVtxChi2 << endreq;
    msg() << MSG::INFO << "PassedBcMass:           " << m_countPassedBcMass << endreq;
    msg() << MSG::INFO << "PassedBcVtx:            " << m_countPassedBcVtx << endreq;
    msg() << MSG::INFO << "PassedBcVtxChi2:        " << m_countPassedBcVtxChi2 << endreq;
    
    return HLT::OK;
}


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBMuMuXFex::acceptInputs(HLT::TEConstVec& , bool& pass)
{
    if ( msgLvl() <= MSG::DEBUG )
        msg() << MSG::DEBUG << "Running TrigEFBMuMuXFex::acceptInputs" << endreq;
    
    pass = true;
    
    return HLT::OK;
}


/*----------------------------------------------------------------------------*/
HLT::ErrorCode TrigEFBMuMuXFex::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement*  outputTE )
{
    if ( msgLvl() <= MSG::DEBUG )
        msg() << MSG::DEBUG << "Running TrigEFBMuMuXFex::hltExecute" << endreq;
    
    if ( timerSvc() ) m_TotTimer->start();
    
    if ( timerSvc() ) {
        m_VtxFitTimer->start();
        m_VtxFitTimer->pause();
    }
    
    bool result(false);
    
    // Set monitoring counters to zeroes
    mon_nTriedCombinations = 0;
    mon_DiMu_n         = 0;
    mon_Tracks_n       = 0;
    mon_BMuMuK_n       = 0;
    mon_BdMuMuKs_n     = 0;
    mon_BsMuMuPhi_n    = 0;
    mon_LbMuMuLambda_n = 0;
    mon_BcMuMuDs_n     = 0;
    mon_TotalRunTime   = 0.;
    mon_VertexingTime  = 0.;
    
    if(msgLvl() <= MSG::VERBOSE) {
        if (m_acceptAll) {
            msg() << MSG::VERBOSE << "AcceptAll property is set: taking all events" << endreq;
            result = true;
        } else {
            msg() << MSG::VERBOSE << "AcceptAll property not set: applying selection" << endreq;
        }
    }
    
    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( store()->retrieve(evtInfo).isFailure() ) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get xAOD::EventInfo " << endreq;
        // now try the old event ifo
        if ( store()->retrieve(pEventInfo).isFailure() ) {
            if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
            mon_Errors.push_back( ERROR_No_EventInfo );
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent << " using algo " << "m_muonAlgo"<<  endreq;
        }// found old event info
    }else { // found the xAOD event info
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << evtInfo->runNumber()
            << " Event " << evtInfo->eventNumber() << " using algo " << "m_muonAlgo"<< endreq;
        IdRun   = evtInfo->runNumber();
        IdEvent = evtInfo->eventNumber();
    } // get event ifo
    
    
    //Check that we got 2 input TEs
    if ( inputTE.size() != 2 ) {
        msg() << MSG::ERROR << "Got different than 2 number of input TEs" << endreq;
        mon_Errors.push_back(ERROR_Not_2_InputTEs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::BAD_JOB_SETUP;
    }
    
    if(IdEvent!=m_lastEvent) {
        m_countTotalEvents++;
        m_lastEvent = IdEvent;
    }
    m_countTotalRoI++;
    
    
    //Retrieve ROIs
    const TrigRoiDescriptor *roiDescriptor1(0);
    const TrigRoiDescriptor *roiDescriptor2(0);
    
    // get them from the navigation
    if ( getFeature(inputTE.front(), roiDescriptor1) != HLT::OK ) {
        msg() << MSG::ERROR << "Navigation error while getting RoI descriptor 1" << endreq;
        mon_Errors.push_back(ERROR_No_RoIs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::NAV_ERROR;
    }
    
    if ( getFeature(inputTE.back(), roiDescriptor2) != HLT::OK ) {
        msg() << MSG::ERROR << "Navigation error while getting RoI descriptor 2" << endreq;
        mon_Errors.push_back(ERROR_No_RoIs);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::NAV_ERROR;
    }
    
    if ( msgLvl() <= MSG::DEBUG ){
        msg() << MSG::DEBUG
        << "Using inputTEs: "<< inputTE.front() <<  " and "  << inputTE.back() << " with Ids " << inputTE.front()->getId()<<" AND "<<inputTE.back()->getId() << std::endl
        << "; RoI IDs = "   << roiDescriptor1->roiId()<< " AND   " <<roiDescriptor2->roiId() << std::endl
        << ": Eta1 =    "   << roiDescriptor1->eta() << " Eta2= " <<roiDescriptor2->eta() << std::endl
        << ", Phi1 =    "   << roiDescriptor1->phi() << " Phi2= " <<roiDescriptor2->phi()
        << endreq;
    }
    
    // Fill RoIs monitoring containers
    mon_RoI_RoI1Eta.push_back(roiDescriptor1->eta());
    mon_RoI_RoI1Phi.push_back(roiDescriptor1->phi());
    mon_RoI_RoI2Eta.push_back(roiDescriptor2->eta());
    mon_RoI_RoI2Phi.push_back(roiDescriptor2->phi());
    mon_RoI_dEtaRoI.push_back( fabs(roiDescriptor1->eta() - roiDescriptor2->eta()) );
    float tmp_RoI_dPhiRoI = roiDescriptor1->phi() - roiDescriptor2->phi();
    while (tmp_RoI_dPhiRoI >  M_PI) tmp_RoI_dPhiRoI -= 2*M_PI;
    while (tmp_RoI_dPhiRoI < -M_PI) tmp_RoI_dPhiRoI += 2*M_PI;
    mon_RoI_dPhiRoI.push_back( fabs(tmp_RoI_dPhiRoI) );
    
    // Retrieve muons
    std::vector<const Trk::Track*> muidIDtracks1;
    std::vector<const Trk::Track*> muidIDtracks2;
    //   const DataHandle<MuidTrackContainer> MuidTracksEF1, MuidTracksEF2;
    //const MuidTrackContainer* MuEFTracksEF1;
    //const MuidTrackContainer* MuEFTracksEF2;
    
    // Get the muon container from the outputTE
    std::vector<const xAOD::MuonContainer*> muonContainerEF1;
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of muon 1" << endreq;
    if(getFeatures(inputTE.front(), muonContainerEF1)!=HLT::OK ) {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to get EFInfo feature of muon 1, exiting" << endreq;
        mon_Errors.push_back(ERROR_No_MuonEFInfoContainer);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::OK; // FIXME should be HLT::MISSING_FEATURE ??
    }
    if(msgLvl() <= MSG::DEBUG) ATH_MSG_DEBUG("Found MuonContainer, Got MuonEF (1) Feature, size = " << muonContainerEF1.size());
    
    for ( unsigned int i_mu=0; i_mu<muonContainerEF1.size(); i_mu++ ) {
        std::vector<const Trk::Track*> idTrks;
        HLT::ErrorCode status = GetxAODMuonTracks(muonContainerEF1[i_mu], idTrks, msg());
        if ( status != HLT::OK ) {
            if ( timerSvc() ) m_TotTimer->stop();
            return status;
        } // if bad
        for (unsigned int i_trk=0; i_trk<idTrks.size(); i_trk++ ) {
            addUnique(muidIDtracks1, idTrks[i_trk]);
            const Trk::Perigee* perigee = idTrks[i_trk]->perigeeParameters();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
                << " Comb muon 1 pt/eta/phi " << perigee->pT()
                << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi]
                << endreq;
        } // loop over any tracks
    } // for
    
    
    //  std::vector<const TrigMuonEFInfoContainer*> MuEFTracksEF1;
    //  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of muon 1" << endreq;
    //  HLT::ErrorCode status = getFeatures(inputTE.front(), MuEFTracksEF1);
    //  if ( status != HLT::OK ) {
    //    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to get EFInfo feature of muon 1, exiting" << endreq;
    //    mon_Errors.push_back(ERROR_No_MuonEFInfoContainer);
    //    if ( timerSvc() ) m_TotTimer->stop();
    //    return HLT::OK;
    //  }
    //  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Got MuonEF (1) Feature, size = " << MuEFTracksEF1.size() << endreq;
    //  for ( unsigned int i_mu=0; i_mu<MuEFTracksEF1.size(); i_mu++ ) {
    //    std::vector<const Trk::Track*> idTrks;
    //    status = GetTrigMuonEFInfoTracks(MuEFTracksEF1[i_mu], idTrks, msg());
    //    if ( status != HLT::OK ) {
    //      if ( timerSvc() ) m_TotTimer->stop();
    //      return status;
    //    }
    //    for ( unsigned int i_trk=0; i_trk<idTrks.size(); i_trk++ ) {
    //      addUnique(muidIDtracks1, idTrks[i_trk]);
    //      const Trk::Perigee* perigee = idTrks[i_trk]->perigeeParameters();
    //      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
    //                                          << " Comb muon 1 " << idTrks[i_trk] << " pt/eta/phi " << perigee->pT()
    //                                          << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi]
    //                                          << endreq;
    //    }
    //  }
    
    // second
    
    std::vector<const xAOD::MuonContainer*> muonContainerEF2;
    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of muon 2" << endreq;
    if(getFeatures(inputTE.back(), muonContainerEF2)!=HLT::OK ) {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Failed to get EFInfo feature of muon 2, exiting" << endreq;
        mon_Errors.push_back(ERROR_No_MuonEFInfoContainer);
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::OK; // FIXME - should be
        // return HLT::MISSING_FEATURE; // was HLT::OK
    }
    if(msgLvl() <= MSG::DEBUG) ATH_MSG_DEBUG("Found MuonContainer, Got MuonEF (2) Feature, size = " << muonContainerEF2.size());
    
    for ( unsigned int i_mu=0; i_mu<muonContainerEF2.size(); i_mu++ ) {
        std::vector<const Trk::Track*> idTrks;
        HLT::ErrorCode status = GetxAODMuonTracks(muonContainerEF2[i_mu], idTrks, msg());
        if ( status != HLT::OK ) return status;
        for (unsigned int i_trk=0; i_trk<idTrks.size(); i_trk++ ) {
            addUnique(muidIDtracks2, idTrks[i_trk]);
            const Trk::Perigee* perigee = idTrks[i_trk]->perigeeParameters();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
                << " Comb muon 2 pt/eta/phi " << perigee->pT()
                << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi]
                << endreq;
        } // loop over any tracks
    } // for loop over muons
    
    
    //    std::vector<const TrigMuonEFInfoContainer*> MuEFTracksEF2;
    //  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Try to retrieve EFInfo container of muon 2" << endreq;
    //  status = getFeatures(inputTE.back(), MuEFTracksEF2);
    //  if ( status != HLT::OK ) {
    //    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Failed to get EFInfo feature of muon 2, exiting" << endreq;
    //    mon_Errors.push_back(ERROR_No_MuonEFInfoContainer);
    //    if ( timerSvc() ) m_TotTimer->stop();
    //    return HLT::OK;
    //  }
    //  if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "Got MuonEF (2) Feature, size = " << MuEFTracksEF2.size() << endreq;
    //  for ( unsigned int i_mu=0; i_mu<MuEFTracksEF2.size(); i_mu++ ) {
    //    std::vector<const Trk::Track*> idTrks;
    //    HLT::ErrorCode status = GetTrigMuonEFInfoTracks(MuEFTracksEF2[i_mu], idTrks, msg());
    //    if ( status != HLT::OK ) {
    //      if ( timerSvc() ) m_TotTimer->stop();
    //      return status;
    //    }
    //    for ( unsigned int i_trk=0; i_trk<idTrks.size(); i_trk++ ) {
    //      addUnique(muidIDtracks2, idTrks[i_trk]);
    //      const Trk::Perigee* perigee = idTrks[i_trk]->perigeeParameters();
    //      if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG
    //                                          << " Comb muon 2 " << idTrks[i_trk] << " pt/eta/phi " << perigee->pT()
    //                                          << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi]
    //                                          << endreq;
    //    }
    //  }
    
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "muidIDtracks1.size()= " << muidIDtracks1.size()<<endreq;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "muidIDtracks2.size()= " << muidIDtracks2.size()<<endreq;
    
    Amg::Vector3D vtx ( 0.,0.,0. );
    
    //   TrigEFBphys* trigPartBMuMuX (NULL);
    // FIXME - remove these 'new's
    //m_trigBphysColl_b = new TrigEFBphysContainer();
    //m_trigBphysColl_X = new TrigEFBphysContainer();
    
    mTrigBphysColl_b = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl_b;
    mTrigBphysColl_b->setStore(&xAODTrigBphysAuxColl_b);
    
    mTrigBphysColl_X = new xAOD::TrigBphysContainer();
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl_X;
    mTrigBphysColl_X->setStore(&xAODTrigBphysAuxColl_X);
    
    // Tried combinations counter - protection against timeouts
    int nTriedCombinations(0);
    
    if( !(muidIDtracks1.size()> 0 && muidIDtracks2.size() > 0) )
    {
        if ( msgLvl() <= MSG::DEBUG )
            msg() << MSG::DEBUG << "No muon candidate found for one or both TEs" << endreq;
        mon_Errors.push_back(ERROR_No_MuonCandidate);
        //delete m_trigBphysColl_b;
        //delete m_trigBphysColl_X;
        delete mTrigBphysColl_b;
        delete mTrigBphysColl_X;
        mTrigBphysColl_b = 0;
        mTrigBphysColl_X = 0;
        if ( timerSvc() ) m_TotTimer->stop();
        return HLT::OK;
    } else {
        
        // Loop over muons
        std::vector<const Trk::Track*>::iterator pItr=muidIDtracks1.begin();
        std::vector<const Trk::Track*>::iterator mItr=muidIDtracks2.begin();
        
        for(pItr=muidIDtracks1.begin(); pItr != muidIDtracks1.end(); pItr++) {
            for(mItr=muidIDtracks2.begin(); mItr != muidIDtracks2.end(); mItr++) {
                
                ATH_MSG(DEBUG) << "Try to build muon pair from mu1 " << (*pItr) << ", mu2 " << (*mItr) << endreq;
                
                // check that we have two different muon tracks
                if (*mItr==*pItr) {
                    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Muon tracks are same" << endreq;
                    mon_Errors.push_back(ERROR_SameMuon);
                    continue;
                }
                m_countPassedMuMuID++;
                
                // check if muons have opposite signs
                if( m_oppositeCharge && ((*pItr)->perigeeParameters()->charge())*((*mItr)->perigeeParameters()->charge()) > 0) {
                    ATH_MSG(DEBUG) << "Muon pair rejected by opposite change check: mu1 " << (*pItr)->perigeeParameters()->charge() << ", mu2 " << (*mItr)->perigeeParameters()->charge() << endreq;
                    continue;
                }
                m_countPassedMuMuOS++;
                
                // check if dimuon mass within the range
                float massMuMu = XMass(*pItr,*mItr,di_to_muons);
                if( massMuMu < m_lowerMuMuMassCut || massMuMu > m_upperMuMuMassCut ) {
                    ATH_MSG(DEBUG) << "Muon pair rejected by mass cut: m = " << massMuMu << endreq;
                    continue;
                }
                m_countPassedMuMuMass++;
                
                if ( timerSvc() ) m_VtxFitTimer->resume();
                Trk::Vertex vertex ( vtx );
                std::vector<const Trk::Track*> pair;
                pair.push_back(*pItr);
                pair.push_back(*mItr);
                Trk::VxCandidate * muMuVxCandidate ( 0 );
                muMuVxCandidate = m_fitterSvc->fit(pair,vertex);
                if ( timerSvc() ) m_VtxFitTimer->pause();
                if(!muMuVxCandidate) {
                    ATH_MSG(DEBUG) << "Failed to fit dimuon vertex" << endreq;
                    mon_Errors.push_back(ERROR_DiMuVtxFit_Fails);
                } else {
                    
                    m_countPassedMuMuVtx++;
                    
                    float chi2MuMu = muMuVxCandidate->recVertex().fitQuality().chiSquared();
                    if(chi2MuMu > m_muVtxChi2Cut) {
                        ATH_MSG(DEBUG) << "Muon pair rejected by chi2 cut: chi2 = " << chi2MuMu << endreq;
                        delete muMuVxCandidate;
                        continue;
                    }
                    m_countPassedMuMuVtxChi2++;
                    
                    // calculate MuMu mass from vertexing
                    double massVtxMuMu(-1.), massVtxErrorMuMu(-1.);
                    std::vector<int> trkIndices;
                    for (int i=0;i<(int)pair.size();++i) {trkIndices.push_back(1);}
                    if( !(m_VKVFitter->VKalGetMassError(trkIndices,massVtxMuMu,massVtxErrorMuMu).isSuccess()) ) {
                        ATH_MSG(DEBUG) << "Warning from VKalVrt - cannot calculate fitmass and error for DiMuon vertex!" << endreq;
                        mon_Errors.push_back(ERROR_DiMuVtxMass_Fails);
                    }
                    
                    // Fill muons monitoring containers
                    mon_DiMu_Pt_Mu1.push_back((*pItr)->perigeeParameters()->pT()/1000.);
                    mon_DiMu_Pt_Mu2.push_back((*mItr)->perigeeParameters()->pT()/1000.);
                    mon_DiMu_Eta_Mu1.push_back((*pItr)->perigeeParameters()->eta());
                    mon_DiMu_Eta_Mu2.push_back((*mItr)->perigeeParameters()->eta());
                    mon_DiMu_Phi_Mu1.push_back((*pItr)->perigeeParameters()->parameters()[Trk::phi0]);
                    mon_DiMu_Phi_Mu2.push_back((*mItr)->perigeeParameters()->parameters()[Trk::phi0]);
                    mon_DiMu_dEtaMuMu.push_back( fabs((*pItr)->perigeeParameters()->eta() - (*mItr)->perigeeParameters()->eta()) );
                    float tmp_DiMu_dPhiMuMu = roiDescriptor1->phi() - roiDescriptor2->phi();
                    while (tmp_DiMu_dPhiMuMu >  M_PI) tmp_DiMu_dPhiMuMu -= 2*M_PI;
                    while (tmp_DiMu_dPhiMuMu < -M_PI) tmp_DiMu_dPhiMuMu += 2*M_PI;
                    mon_DiMu_dPhiMuMu.push_back( fabs(tmp_DiMu_dPhiMuMu) );
                    mon_DiMu_pTsumMuMu.push_back( ((*pItr)->perigeeParameters()->pT() + (*mItr)->perigeeParameters()->pT())/1000. );
                    mon_DiMu_InvMassMuMu.push_back(massMuMu/1000.);
                    mon_DiMu_VtxMassMuMu.push_back(massVtxMuMu/1000.);
                    mon_DiMu_Chi2MuMu.push_back(chi2MuMu);
                    
                    mon_DiMu_n++;
                    
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Dimuon Sec Vtx at ("
                        << muMuVxCandidate->recVertex().position() [0]
                        << muMuVxCandidate->recVertex().position() [1]
                        << muMuVxCandidate->recVertex().position() [2] <<  ") with chi2 = "
                        << muMuVxCandidate->recVertex().fitQuality().chiSquared() << " ("
                        << muMuVxCandidate->vxTrackAtVertex()->size() << " tracks)"
                        << " Mass= "<< massMuMu << endreq;
                    
                    // JK look for tracks in TrackParticle containers and add to TrigEFBphys
                    //
                    // TODO
                    // Doesn't work now: track pointers *pItr, *mItr are not the same as in vectorTrkParticleCollections elements
                    //
                    bool Found1Track=false;
                    bool Found2Track=false;
                    
                    ///////////////// Get vector of tracks /////////////////
                    //std::vector<const Rec::TrackParticleContainer *> vectorTrkParticleCollections;
                    std::vector<const xAOD::TrackParticleContainer *> vectorxAODTrkParticleCont;
                    
                    HLT::ErrorCode status = getFeatures(outputTE, vectorxAODTrkParticleCont);
                    //HLT::ErrorCode status = getFeatures(inputTE.front(), vectorTrkParticleCollections);
                    if(status != HLT::OK/* or vectorTrkParticleCont==NULL*/) {
                        msg() << MSG::WARNING << "Failed to get xAOD::TrackParticleCollections from the trigger element" << endreq;
                        mon_Errors.push_back(ERROR_No_TrackColl);
                        if ( timerSvc() ) m_TotTimer->stop();
                        // delete m_trigBphysColl_b;
                        // delete m_trigBphysColl_X;
                        delete mTrigBphysColl_b;
                        delete mTrigBphysColl_X;
                        mTrigBphysColl_b = NULL;
                        mTrigBphysColl_X = NULL;
                        delete muMuVxCandidate;
                        return HLT::MISSING_FEATURE;
                    } else {
                        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Got " << vectorxAODTrkParticleCont.size()
                            << " TrackParticle Collections" << endreq;
                    }
                    //const Rec::TrackParticleContainer* TrkParticleCont = vectorTrkParticleCollections.front();
                    const xAOD::TrackParticleContainer* xAODTrkParticleCont = vectorxAODTrkParticleCont.front();
                    
                    // JK If more than 1 track collection then this is FullScan instance. Find collection with most tracks
                    unsigned int Ntracks=0;
                    for (std::vector<const xAOD::TrackParticleContainer*>::const_iterator pTrackColl = vectorxAODTrkParticleCont.begin();
                         pTrackColl != vectorxAODTrkParticleCont.end();
                         ++pTrackColl) {
                        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "  InDetTrackCollections size, " << (*pTrackColl)->size() << endreq;
                        if ((*pTrackColl)->size() > Ntracks) {
                            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Largest collection so far, assume it is FullScan " << endreq;
                            Ntracks= (*pTrackColl)->size();
                            xAODTrkParticleCont = *pTrackColl;
                        }
                    }
                    
                    // Now prepare to outer loop over tracks
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Now loop over TrackParticles to find tracks " << endreq;
                    xAOD::TrackParticleContainer::const_iterator trkIt =  xAODTrkParticleCont->begin();
                    xAOD::TrackParticleContainer::const_iterator lastTrkIt = xAODTrkParticleCont->end();
                    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Found tracks, ntrack= " << xAODTrkParticleCont->size() << endreq;
                    
                    //           ElementLink<Rec::TrackParticleContainer> trackELmu1;
                    //           ElementLink<Rec::TrackParticleContainer> trackELmu2;
                    
                    int piTrk(-1),miTrk(-1);
                    // First - a little loop to get muon trackELs to store in trigPartBMuMuX
                    for (int itrk=0 ; trkIt != lastTrkIt; itrk++, trkIt++)
                    {
                        const Trk::Track* track = (*trkIt)->track();
                        const Trk::Perigee* perigee = track->perigeeParameters();
                        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "original track " << track << ", pt/eta/phi = " << perigee->pT() << " / " << perigee->eta() << " / " << perigee->parameters()[Trk::phi0] << endreq;
                        //             if (track == *pItr && !Found1Track)
                        //FIXME - correct the dphi problem, and pT?
                        if ( fabs(perigee->pT() - (*pItr)->perigeeParameters()->pT())<100. && fabs(perigee->eta() - (*pItr)->perigeeParameters()->eta())<0.02 && fabs(perigee->parameters()[Trk::phi0] - (*pItr)->perigeeParameters()->parameters()[Trk::phi0])<0.02 && !Found1Track)
                        {
                            //                 ElementLink<Rec::TrackParticleContainer> trackELmu1(*TrkParticleCont,itrk);
                            piTrk = itrk;
                            //                 trigPartBMuMuX->addTrack(trackEL);
                            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "track matches muon track 1, will store in EFBPhys " << endreq;
                            Found1Track=true;
                        }
                        //             if (track == *mItr && !Found2Track)
                        //FIXME - correct the dphi problem, and pT?
                        if ( fabs(perigee->pT() - (*mItr)->perigeeParameters()->pT())<100. && fabs(perigee->eta() - (*mItr)->perigeeParameters()->eta())<0.02 && fabs(perigee->parameters()[Trk::phi0] - (*mItr)->perigeeParameters()->parameters()[Trk::phi0])<0.02 && !Found2Track)
                        {
                            //                 ElementLink<Rec::TrackParticleContainer> trackELmu2(*TrkParticleCont,itrk);
                            miTrk = itrk;
                            //                 trigPartBMuMuX->addTrack(trackEL);
                            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "track matches muon track 2, will store in EFBPhys " << endreq;
                            Found2Track=true;
                        }
                    } // end of a little loop over tracks
                    
                    // TODO
                    // Doesn't work now - the check commented out until solved...
                    if(!Found1Track || !Found2Track) {
                        ATH_MSG(DEBUG) << "No ID track found for one or both muons. Found1Track = " << Found1Track << ", Found2Track = " << Found2Track << endreq;
                        mon_Errors.push_back(ERROR_No_MuonTrackMatch);
                        //             delete m_trigBphysColl_b;
                        //             delete m_trigBphysColl_X;
                        //             delete muMuVxCandidate;
                        //             if ( timerSvc() ) m_TotTimer->stop();
                        //             return(HLT::OK);
                    }
                    ElementLink<xAOD::TrackParticleContainer> trackELmu1(*xAODTrkParticleCont,piTrk);
                    ElementLink<xAOD::TrackParticleContainer> trackELmu2(*xAODTrkParticleCont,miTrk);
                    
                    // Main outer loop over tracks
                    xAOD::TrackParticleContainer::const_iterator trkIt1 =  xAODTrkParticleCont->begin();
                    for (int itrk1=0 ; trkIt1 != lastTrkIt; itrk1++, trkIt1++)
                    {
                        const Trk::Track* track1 = (*trkIt1)->track();
                        
                        // Check that it is not muon track
                        if(itrk1==piTrk || itrk1==miTrk) {
                            ATH_MSG(DEBUG) << "Track " << track1 << " was matched to a muon, skip it" << endreq;
                            continue;
                        }
                        
                        // Fill tracks monitoring containers
                        mon_Tracks_Pt.push_back(track1->perigeeParameters()->pT()/1000.);
                        mon_Tracks_Eta.push_back(track1->perigeeParameters()->eta());
                        mon_Tracks_Phi.push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                        
                        mon_Tracks_n++;
                        
                        //
                        // B+ -> mu mu K+ part
                        //
                        if(m_doB_KMuMuDecay) {
                            
                            TrigEFBphys* trigPartBplusMuMuKplus = checkBplusMuMuKplus(*mItr,*pItr,track1);
                            nTriedCombinations++;
                            if (trigPartBplusMuMuKplus) {
                                //now make the xaod object
                                xAOD::TrigBphys* xaodObj = new xAOD::TrigBphys();
                                mTrigBphysColl_b->push_back( xaodObj );
                                xaodObj->initialise(0, 0., 0., xAOD::TrigBphys::BKMUMU, trigPartBplusMuMuKplus->mass(), xAOD::TrigBphys::EF );
                                
                                //xaodObj->setEta         (trigPartBplusMuMuKplus->eta());
                                //xaodObj->setPhi         (trigPartBplusMuMuKplus->phi());
                                //xaodObj->setMass        (trigPartBplusMuMuKplus->mass());
                                //xaodObj->setRoiId       (trigPartBplusMuMuKplus->roiId());
                                //xaodObj->setParticleType((xAOD::TrigBphys::pType)aod->particleType());
                                //xaodObj->setLevel       (xAOD::TrigBphys::levelType::EF);
                                xaodObj->setFitmass     (trigPartBplusMuMuKplus->fitmass());
                                xaodObj->setFitchi2     (trigPartBplusMuMuKplus->fitchi2());
                                xaodObj->setFitndof     (trigPartBplusMuMuKplus->fitndof());
                                xaodObj->setFitx        (trigPartBplusMuMuKplus->fitx());
                                xaodObj->setFity        (trigPartBplusMuMuKplus->fity());
                                xaodObj->setFitz        (trigPartBplusMuMuKplus->fitz());
                                
                                delete trigPartBplusMuMuKplus; // done with the old object now.
                                trigPartBplusMuMuKplus =0;
                                
                                ElementLink<xAOD::TrackParticleContainer> trackEL3(*xAODTrkParticleCont,itrk1);
                                xaodObj->addTrackParticleLink(trackEL3);
                                if(Found1Track) xaodObj->addTrackParticleLink(trackELmu1);
                                if(Found2Track) xaodObj->addTrackParticleLink(trackELmu2);
                                //m_trigBphysColl_b->push_back(trigPartBplusMuMuKplus);
                                
                                result=true;
                                
                                mon_BMuMuK_n++;
                                
                                if(IdEvent!=m_lastEventPassedBplus) {
                                    m_countPassedEventsBplus++;
                                    m_lastEventPassedBplus = IdEvent;
                                }
                                
                                //                 delete trigPartBplusMuMuKplus;
                            }
                        } // end if(m_doB_KMuMuDecay)
                        
                        // Protection
                        if( nTriedCombinations > m_maxNcombinations ) {
                            ATH_MSG(DEBUG) << "Too many track combinations: " << endreq;
                            ATH_MSG(DEBUG) << "  nTriedCombinations = " << nTriedCombinations
                            << ", while MaxNcombinations = " << m_maxNcombinations << endreq;
                            ATH_MSG(DEBUG) << "Terminate the loop" << endreq;
                            ATH_MSG(DEBUG) << "  Bphys Collection size is " << mTrigBphysColl_b->size() << endreq;
                            if(mTrigBphysColl_b->size() > 0)
                                mon_Errors.push_back(ERROR_TooManyComb_Acc);
                            else
                                mon_Errors.push_back(ERROR_TooManyComb_Rej);
                            break;
                        }
                        
                        // Inner loop over tracks
                        xAOD::TrackParticleContainer::const_iterator trkIt2 = trkIt1 + 1;
                        for (int itrk2=itrk1+1 ; trkIt2 != lastTrkIt; itrk2++, trkIt2++)
                        {
                            //If timeout has happened in spite of the protection
                            if ( Athena::Timeout::instance().reached() ) {
                                return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::TIMEOUT);
                            }
                            
                            if(m_doBd_KstarMuMuDecay || m_doBs_Phi1020MuMuDecay || m_doLb_LambdaMuMuDecay || m_doBc_DsMuMuDecay) {
                                const Trk::Track* track1 = (*trkIt1)->track();
                                const Trk::Track* track2 = (*trkIt2)->track();
                                
                                // Check that it is not muon track
                                if(itrk2==piTrk || itrk2==miTrk) {
                                    ATH_MSG(DEBUG) << "Track " << track2 << " was matched to a muon, skip it" << endreq;
                                    continue;
                                }
                                
                                // check that track1 and track2 have opposite charges
                                if( (track1->perigeeParameters()->charge())*(track2->perigeeParameters()->charge()) > 0) {
                                    ATH_MSG(DEBUG) << "Track pair rejected by opposite charge check: "
                                    << track1 << " = " << track1->perigeeParameters()->charge() << ",  "
                                    << track2 << " = " << track2->perigeeParameters()->charge() << endreq;
                                    continue;
                                }
                                
                                //
                                // Bd -> mu mu K0* (K+ pi-) part
                                //
                                if(m_doBd_KstarMuMuDecay) {
                                    TrigEFBphys* trigPartKstar(0);
                                    
                                    // Mass hypothesis 1: track1 = kaon, track2 = pion
                                    TrigEFBphys* trigPartBdMuMuKstar = checkBMuMu2X(*mItr,*pItr,track1,track2,bD_to_Kstar,trigPartKstar);
                                    nTriedCombinations++;
                                    if (trigPartBdMuMuKstar) {
                                        //                     trigPartBdMuMuKstar->addTrack(trackELmu1);
                                        //                     trigPartBdMuMuKstar->addTrack(trackELmu2);
                                        //now make the xaod object
                                        xAOD::TrigBphys* xaod_trigPartBdMuMuKstar = new xAOD::TrigBphys();
                                        mTrigBphysColl_b->push_back( xaod_trigPartBdMuMuKstar );
                                        xaod_trigPartBdMuMuKstar->initialise(0, 0., 0.,
                                                                             (xAOD::TrigBphys::pType)trigPartBdMuMuKstar->particleType(),
                                                                             trigPartBdMuMuKstar->mass(),
                                                                             xAOD::TrigBphys::EF );
                                        
                                        xAOD::TrigBphys* xaod_trigPartKstar = new xAOD::TrigBphys();
                                        mTrigBphysColl_X->push_back( xaod_trigPartKstar );
                                        xaod_trigPartKstar->initialise(0, 0., 0.,
                                                                       (xAOD::TrigBphys::pType)trigPartKstar->particleType(),
                                                                       trigPartKstar->mass(),
                                                                       xAOD::TrigBphys::EF );
                                        
                                        
                                        xaod_trigPartBdMuMuKstar->setFitmass     (trigPartBdMuMuKstar->fitmass());
                                        xaod_trigPartBdMuMuKstar->setFitchi2     (trigPartBdMuMuKstar->fitchi2());
                                        xaod_trigPartBdMuMuKstar->setFitndof     (trigPartBdMuMuKstar->fitndof());
                                        xaod_trigPartBdMuMuKstar->setFitx        (trigPartBdMuMuKstar->fitx());
                                        xaod_trigPartBdMuMuKstar->setFity        (trigPartBdMuMuKstar->fity());
                                        xaod_trigPartBdMuMuKstar->setFitz        (trigPartBdMuMuKstar->fitz());
                                        
                                        delete trigPartBdMuMuKstar; // done with the old object now.
                                        trigPartBdMuMuKstar =0;
                                        
                                        xaod_trigPartKstar->setFitmass     (trigPartKstar->fitmass());
                                        xaod_trigPartKstar->setFitchi2     (trigPartKstar->fitchi2());
                                        xaod_trigPartKstar->setFitndof     (trigPartKstar->fitndof());
                                        xaod_trigPartKstar->setFitx        (trigPartKstar->fitx());
                                        xaod_trigPartKstar->setFity        (trigPartKstar->fity());
                                        xaod_trigPartKstar->setFitz        (trigPartKstar->fitz());
                                        
                                        delete trigPartKstar; // done with the old object now.
                                        trigPartKstar =0;
                                        
                                        
                                        ElementLink<xAOD::TrackParticleContainer> trackEL3(*xAODTrkParticleCont,itrk1);
                                        ElementLink<xAOD::TrackParticleContainer> trackEL4(*xAODTrkParticleCont,itrk2);
                                        xaod_trigPartKstar->addTrackParticleLink(trackEL3);
                                        xaod_trigPartKstar->addTrackParticleLink(trackEL4);
                                        //m_trigBphysColl_X->push_back(trigPartKstar);
                                        int iKstar = mTrigBphysColl_X->size() - 1;
                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iKstar);
                                        xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackEL3);
                                        xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackEL4);
                                        if(Found1Track) xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackELmu1);
                                        if(Found2Track) xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackELmu2);
                                        xaod_trigPartBdMuMuKstar->setSecondaryDecayLink(trigPartXEL);
                                        //m_trigBphysColl_b->push_back(trigPartBdMuMuKstar);
                                        
                                        result=true;
                                        
                                        mon_BdMuMuKs_n++;
                                        
                                        if(IdEvent!=m_lastEventPassedBd) {
                                            m_countPassedEventsBd++;
                                            m_lastEventPassedBd = IdEvent;
                                        }
                                        
                                        //                     delete trigPartBdMuMuKstar;
                                    }
                                    
                                    // Mass hypothesis 2: track1 = pion, track2 = kaon
                                    trigPartBdMuMuKstar = checkBMuMu2X(*mItr,*pItr,track2,track1,bD_to_Kstar,trigPartKstar);
                                    nTriedCombinations++;
                                    if (trigPartBdMuMuKstar) {
                                        //now make the xaod object
                                        xAOD::TrigBphys* xaod_trigPartBdMuMuKstar = new xAOD::TrigBphys();
                                        mTrigBphysColl_b->push_back( xaod_trigPartBdMuMuKstar );
                                        xaod_trigPartBdMuMuKstar->initialise(0, 0., 0.,
                                                                             (xAOD::TrigBphys::pType)trigPartBdMuMuKstar->particleType(),
                                                                             trigPartBdMuMuKstar->mass(),
                                                                             xAOD::TrigBphys::EF );
                                        
                                        xAOD::TrigBphys* xaod_trigPartKstar = new xAOD::TrigBphys();
                                        mTrigBphysColl_X->push_back( xaod_trigPartKstar );
                                        xaod_trigPartKstar->initialise(0, 0., 0.,
                                                                       (xAOD::TrigBphys::pType)trigPartKstar->particleType(),
                                                                       trigPartKstar->mass(),
                                                                       xAOD::TrigBphys::EF );
                                        
                                        
                                        xaod_trigPartBdMuMuKstar->setFitmass     (trigPartBdMuMuKstar->fitmass());
                                        xaod_trigPartBdMuMuKstar->setFitchi2     (trigPartBdMuMuKstar->fitchi2());
                                        xaod_trigPartBdMuMuKstar->setFitndof     (trigPartBdMuMuKstar->fitndof());
                                        xaod_trigPartBdMuMuKstar->setFitx        (trigPartBdMuMuKstar->fitx());
                                        xaod_trigPartBdMuMuKstar->setFity        (trigPartBdMuMuKstar->fity());
                                        xaod_trigPartBdMuMuKstar->setFitz        (trigPartBdMuMuKstar->fitz());
                                        
                                        delete trigPartBdMuMuKstar; // done with the old object now.
                                        trigPartBdMuMuKstar =0;
                                        
                                        xaod_trigPartKstar->setFitmass     (trigPartKstar->fitmass());
                                        xaod_trigPartKstar->setFitchi2     (trigPartKstar->fitchi2());
                                        xaod_trigPartKstar->setFitndof     (trigPartKstar->fitndof());
                                        xaod_trigPartKstar->setFitx        (trigPartKstar->fitx());
                                        xaod_trigPartKstar->setFity        (trigPartKstar->fity());
                                        xaod_trigPartKstar->setFitz        (trigPartKstar->fitz());
                                        
                                        delete trigPartKstar; // done with the old object now.
                                        trigPartKstar =0;

                                        //                     trigPartBdMuMuKstar->addTrack(trackELmu1);
                                        //                     trigPartBdMuMuKstar->addTrack(trackELmu2);
                                        ElementLink<xAOD::TrackParticleContainer> trackEL3(*xAODTrkParticleCont,itrk2);
                                        ElementLink<xAOD::TrackParticleContainer> trackEL4(*xAODTrkParticleCont,itrk1);
                                        xaod_trigPartKstar->addTrackParticleLink(trackEL3);
                                        xaod_trigPartKstar->addTrackParticleLink(trackEL4);
                                        //m_trigBphysColl_X->push_back(trigPartKstar);
                                        int iKstar = mTrigBphysColl_X->size() - 1;
                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iKstar);
                                        xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackEL3);
                                        xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackEL4);
                                        if(Found1Track) xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackELmu1);
                                        if(Found2Track) xaod_trigPartBdMuMuKstar->addTrackParticleLink(trackELmu2);
                                        xaod_trigPartBdMuMuKstar->setSecondaryDecayLink(trigPartXEL);
                                        //m_trigBphysColl_b->push_back(trigPartBdMuMuKstar);
                                        
                                        result=true;
                                        
                                        mon_BdMuMuKs_n++;
                                        
                                        if(IdEvent!=m_lastEventPassedBd) {
                                            m_countPassedEventsBd++;
                                            m_lastEventPassedBd = IdEvent;
                                        }
                                        
                                        //                     delete trigPartBdMuMuKstar;
                                    }
                                } // end if(m_doBd_KstarMuMuDecay)
                                
                                //
                                // Bs -> mu mu Phi (K K) part
                                //
                                if(m_doBs_Phi1020MuMuDecay) {
                                    TrigEFBphys* trigPartPhi(0);
                                    
                                    TrigEFBphys* trigPartBsMuMuPhi = checkBMuMu2X(*mItr,*pItr,track1,track2,bS_to_Phi,trigPartPhi);
                                    nTriedCombinations++;
                                    if (trigPartBsMuMuPhi) {
                                        //now make the xaod object
                                        xAOD::TrigBphys* xaod_trigPartBsMuMuPhi = new xAOD::TrigBphys();
                                        mTrigBphysColl_b->push_back( xaod_trigPartBsMuMuPhi );
                                        xaod_trigPartBsMuMuPhi->initialise(0, 0., 0.,
                                                                             (xAOD::TrigBphys::pType)trigPartBsMuMuPhi->particleType(),
                                                                             trigPartBsMuMuPhi->mass(),
                                                                             xAOD::TrigBphys::EF );
                                        
                                        xAOD::TrigBphys* xaod_trigPartPhi = new xAOD::TrigBphys();
                                        mTrigBphysColl_X->push_back( xaod_trigPartPhi );
                                        xaod_trigPartPhi->initialise(0, 0., 0.,
                                                                       (xAOD::TrigBphys::pType)trigPartPhi->particleType(),
                                                                       trigPartPhi->mass(),
                                                                       xAOD::TrigBphys::EF );
                                        
                                        
                                        xaod_trigPartBsMuMuPhi->setFitmass     (trigPartBsMuMuPhi->fitmass());
                                        xaod_trigPartBsMuMuPhi->setFitchi2     (trigPartBsMuMuPhi->fitchi2());
                                        xaod_trigPartBsMuMuPhi->setFitndof     (trigPartBsMuMuPhi->fitndof());
                                        xaod_trigPartBsMuMuPhi->setFitx        (trigPartBsMuMuPhi->fitx());
                                        xaod_trigPartBsMuMuPhi->setFity        (trigPartBsMuMuPhi->fity());
                                        xaod_trigPartBsMuMuPhi->setFitz        (trigPartBsMuMuPhi->fitz());
                                        
                                        delete trigPartBsMuMuPhi; // done with the old object now.
                                        trigPartBsMuMuPhi =0;
                                        
                                        xaod_trigPartPhi->setFitmass     (trigPartPhi->fitmass());
                                        xaod_trigPartPhi->setFitchi2     (trigPartPhi->fitchi2());
                                        xaod_trigPartPhi->setFitndof     (trigPartPhi->fitndof());
                                        xaod_trigPartPhi->setFitx        (trigPartPhi->fitx());
                                        xaod_trigPartPhi->setFity        (trigPartPhi->fity());
                                        xaod_trigPartPhi->setFitz        (trigPartPhi->fitz());
                                        
                                        delete trigPartPhi; // done with the old object now.
                                        trigPartPhi =0;

                                        ElementLink<xAOD::TrackParticleContainer> trackEL3(*xAODTrkParticleCont,itrk1);
                                        ElementLink<xAOD::TrackParticleContainer> trackEL4(*xAODTrkParticleCont,itrk2);
                                        xaod_trigPartPhi->addTrackParticleLink(trackEL3);
                                        xaod_trigPartPhi->addTrackParticleLink(trackEL4);
                                        //m_trigBphysColl_X->push_back(trigPartPhi);
                                        int iPhi = mTrigBphysColl_X->size() - 1;
                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iPhi);
                                        xaod_trigPartBsMuMuPhi->addTrackParticleLink(trackEL3);
                                        xaod_trigPartBsMuMuPhi->addTrackParticleLink(trackEL4);
                                        if(Found1Track) xaod_trigPartBsMuMuPhi->addTrackParticleLink(trackELmu1);
                                        if(Found2Track) xaod_trigPartBsMuMuPhi->addTrackParticleLink(trackELmu2);
                                        xaod_trigPartBsMuMuPhi->setSecondaryDecayLink(trigPartXEL);
                                        //m_trigBphysColl_b->push_back(trigPartBsMuMuPhi);
                                        
                                        result=true;
                                        
                                        mon_BsMuMuPhi_n++;
                                        
                                        if(IdEvent!=m_lastEventPassedBs) {
                                            m_countPassedEventsBs++;
                                            m_lastEventPassedBs = IdEvent;
                                        }
                                        
                                        //                     delete trigPartBsMuMuPhi;
                                    }
                                } // end if(m_doBs_Phi1020MuMuDecay)
                                
                                //
                                // Lambda_b -> mu mu Lambda part
                                //
                                if(m_doLb_LambdaMuMuDecay) {
                                    // TODO
                                    // Now L_b uses simple 4-prong vertex fit instead of cascade vertexing
                                    TrigEFBphys* trigPartLambda(0);
                                    
                                    // Mass hypothesis 1: track1 = proton, track2 = pion
                                    TrigEFBphys* trigPartLbMuMuLambda = checkBMuMu2X(*mItr,*pItr,track1,track2,lB_to_L,trigPartLambda);
                                    nTriedCombinations++;
                                    if (trigPartLbMuMuLambda) {
                                        //now make the xaod object
                                        xAOD::TrigBphys* xaod_trigPartLbMuMuLambda = new xAOD::TrigBphys();
                                        mTrigBphysColl_b->push_back( xaod_trigPartLbMuMuLambda );
                                        xaod_trigPartLbMuMuLambda->initialise(0, 0., 0.,
                                                                           (xAOD::TrigBphys::pType)trigPartLbMuMuLambda->particleType(),
                                                                           trigPartLbMuMuLambda->mass(),
                                                                           xAOD::TrigBphys::EF );
                                        
                                        xAOD::TrigBphys* xaod_trigPartLambda = new xAOD::TrigBphys();
                                        mTrigBphysColl_X->push_back( xaod_trigPartLambda );
                                        xaod_trigPartLambda->initialise(0, 0., 0.,
                                                                     (xAOD::TrigBphys::pType)trigPartLambda->particleType(),
                                                                     trigPartLambda->mass(),
                                                                     xAOD::TrigBphys::EF );
                                        
                                        
                                        xaod_trigPartLbMuMuLambda->setFitmass     (trigPartLbMuMuLambda->fitmass());
                                        xaod_trigPartLbMuMuLambda->setFitchi2     (trigPartLbMuMuLambda->fitchi2());
                                        xaod_trigPartLbMuMuLambda->setFitndof     (trigPartLbMuMuLambda->fitndof());
                                        xaod_trigPartLbMuMuLambda->setFitx        (trigPartLbMuMuLambda->fitx());
                                        xaod_trigPartLbMuMuLambda->setFity        (trigPartLbMuMuLambda->fity());
                                        xaod_trigPartLbMuMuLambda->setFitz        (trigPartLbMuMuLambda->fitz());
                                        
                                        delete trigPartLbMuMuLambda; // done with the old object now.
                                        trigPartLbMuMuLambda =0;
                                        
                                        xaod_trigPartLambda->setFitmass     (trigPartLambda->fitmass());
                                        xaod_trigPartLambda->setFitchi2     (trigPartLambda->fitchi2());
                                        xaod_trigPartLambda->setFitndof     (trigPartLambda->fitndof());
                                        xaod_trigPartLambda->setFitx        (trigPartLambda->fitx());
                                        xaod_trigPartLambda->setFity        (trigPartLambda->fity());
                                        xaod_trigPartLambda->setFitz        (trigPartLambda->fitz());
                                        
                                        delete trigPartLambda; // done with the old object now.
                                        trigPartLambda =0;

                                        
                                        ElementLink<xAOD::TrackParticleContainer> trackEL3(*xAODTrkParticleCont,itrk1);
                                        ElementLink<xAOD::TrackParticleContainer> trackEL4(*xAODTrkParticleCont,itrk2);
                                        xaod_trigPartLambda->addTrackParticleLink(trackEL3);
                                        xaod_trigPartLambda->addTrackParticleLink(trackEL4);
                                        // m_trigBphysColl_X->push_back(trigPartLambda);
                                        int iLambda = mTrigBphysColl_X->size() - 1;
                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iLambda);
                                        xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackEL3);
                                        xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackEL4);
                                        if(Found1Track) xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackELmu1);
                                        if(Found2Track) xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackELmu2);
                                        xaod_trigPartLbMuMuLambda->setSecondaryDecayLink(trigPartXEL);
                                        // m_trigBphysColl_b->push_back(trigPartLbMuMuLambda);
                                        
                                        result = true;
                                        
                                        mon_LbMuMuLambda_n++;
                                        
                                        if(IdEvent!=m_lastEventPassedLb) {
                                            m_countPassedEventsLb++;
                                            m_lastEventPassedLb = IdEvent;
                                        }
                                        
                                        //                     delete trigPartLbMuMuLambda;
                                    }
                                    
                                    // Mass hypothesis 2: track1 = pion, track2 = proton
                                    trigPartLbMuMuLambda = checkBMuMu2X(*mItr,*pItr,track2,track1,lB_to_L,trigPartLambda);
                                    nTriedCombinations++;
                                    if (trigPartLbMuMuLambda) {
                                        //now make the xaod object
                                        xAOD::TrigBphys* xaod_trigPartLbMuMuLambda = new xAOD::TrigBphys();
                                        mTrigBphysColl_b->push_back( xaod_trigPartLbMuMuLambda );
                                        xaod_trigPartLbMuMuLambda->initialise(0, 0., 0.,
                                                                              (xAOD::TrigBphys::pType)trigPartLbMuMuLambda->particleType(),
                                                                              trigPartLbMuMuLambda->mass(),
                                                                              xAOD::TrigBphys::EF );
                                        
                                        xAOD::TrigBphys* xaod_trigPartLambda = new xAOD::TrigBphys();
                                        mTrigBphysColl_X->push_back( xaod_trigPartLambda );
                                        xaod_trigPartLambda->initialise(0, 0., 0.,
                                                                        (xAOD::TrigBphys::pType)trigPartLambda->particleType(),
                                                                        trigPartLambda->mass(),
                                                                        xAOD::TrigBphys::EF );
                                        
                                        
                                        xaod_trigPartLbMuMuLambda->setFitmass     (trigPartLbMuMuLambda->fitmass());
                                        xaod_trigPartLbMuMuLambda->setFitchi2     (trigPartLbMuMuLambda->fitchi2());
                                        xaod_trigPartLbMuMuLambda->setFitndof     (trigPartLbMuMuLambda->fitndof());
                                        xaod_trigPartLbMuMuLambda->setFitx        (trigPartLbMuMuLambda->fitx());
                                        xaod_trigPartLbMuMuLambda->setFity        (trigPartLbMuMuLambda->fity());
                                        xaod_trigPartLbMuMuLambda->setFitz        (trigPartLbMuMuLambda->fitz());
                                        
                                        delete trigPartLbMuMuLambda; // done with the old object now.
                                        trigPartLbMuMuLambda =0;
                                        
                                        xaod_trigPartLambda->setFitmass     (trigPartLambda->fitmass());
                                        xaod_trigPartLambda->setFitchi2     (trigPartLambda->fitchi2());
                                        xaod_trigPartLambda->setFitndof     (trigPartLambda->fitndof());
                                        xaod_trigPartLambda->setFitx        (trigPartLambda->fitx());
                                        xaod_trigPartLambda->setFity        (trigPartLambda->fity());
                                        xaod_trigPartLambda->setFitz        (trigPartLambda->fitz());
                                        
                                        delete trigPartLambda; // done with the old object now.
                                        trigPartLambda =0;

                                        
                                        ElementLink<xAOD::TrackParticleContainer> trackEL3(*xAODTrkParticleCont,itrk2);
                                        ElementLink<xAOD::TrackParticleContainer> trackEL4(*xAODTrkParticleCont,itrk1);
                                        xaod_trigPartLambda->addTrackParticleLink(trackEL3);
                                        xaod_trigPartLambda->addTrackParticleLink(trackEL4);
                                        // m_trigBphysColl_X->push_back(trigPartLambda);
                                        int iLambda = mTrigBphysColl_X->size() - 1;
                                        ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iLambda);
                                        xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackEL3);
                                        xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackEL4);
                                        if(Found1Track) xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackELmu1);
                                        if(Found2Track) xaod_trigPartLbMuMuLambda->addTrackParticleLink(trackELmu2);
                                        xaod_trigPartLbMuMuLambda->setSecondaryDecayLink(trigPartXEL);
                                        // m_trigBphysColl_b->push_back(trigPartLbMuMuLambda);
                                        
                                        result = true;
                                        
                                        mon_LbMuMuLambda_n++;
                                        
                                        if(IdEvent!=m_lastEventPassedLb) {
                                            m_countPassedEventsLb++;
                                            m_lastEventPassedLb = IdEvent;
                                        }
                                        
                                        //                     delete trigPartLbMuMuLambda;
                                    }
                                } // end if(m_doLb_LambdaMuMuDecay)
                                
                                //
                                // Bc -> mu mu Ds (Phi pi) part
                                //
                                if(m_doBc_DsMuMuDecay) {
                                    
                                    double xPhiMass = XMass(track1,track2,bS_to_Phi); // decay = bS_to_Phi to actually calculate Phi->KK mass
                                    
                                    if ( xPhiMass > m_lowerPhiDs_MassCut && xPhiMass < m_upperPhiDs_MassCut ) {
                                        
                                        double xPhiMuMuMass = XMuMuMass (*mItr,*pItr,track1,track2,bS_to_Phi);
                                        
                                        if ( xPhiMuMuMass < m_upperBc_DsMuMuMassCut - PIMASS ) {
                                            
                                            // 3rd loop over tracks
                                            xAOD::TrackParticleContainer::const_iterator trkIt3 =  xAODTrkParticleCont->begin();
                                            for (int itrk3=0 ; trkIt3 != lastTrkIt; itrk3++, trkIt3++)
                                            {
                                                const Trk::Track* track3 = (*trkIt3)->track();
                                                
                                                if(itrk3==itrk1 || itrk3==itrk2) {
                                                    ATH_MSG(DEBUG) << "Track " << track3 << " is the same as another" << endreq;
                                                    continue;
                                                }
                                                
                                                // Check that it is not muon track
                                                if(itrk3==piTrk || itrk3==miTrk) {
                                                    ATH_MSG(DEBUG) << "Track " << track3 << " was matched to a muon, skip it" << endreq;
                                                    continue;
                                                }
                                                
                                                TrigEFBphys* trigPartDs;
                                                
                                                TrigEFBphys* trigPartBcMuMuDs = checkBcMuMuDs(*mItr,*pItr,track1,track2, xPhiMass, track3,trigPartDs);
                                                nTriedCombinations++;
                                                if (trigPartBcMuMuDs) {
                                                    
                                                    //now make the xaod object
                                                    xAOD::TrigBphys* xaod_trigPartBcMuMuDs = new xAOD::TrigBphys();
                                                    mTrigBphysColl_b->push_back( xaod_trigPartBcMuMuDs );
                                                    xaod_trigPartBcMuMuDs->initialise(0, 0., 0.,
                                                                                          (xAOD::TrigBphys::pType)trigPartBcMuMuDs->particleType(),
                                                                                          trigPartBcMuMuDs->mass(),
                                                                                          xAOD::TrigBphys::EF );
                                                    
                                                    xAOD::TrigBphys* xaod_trigPartDs = new xAOD::TrigBphys();
                                                    mTrigBphysColl_X->push_back( xaod_trigPartDs );
                                                    xaod_trigPartDs->initialise(0, 0., 0.,
                                                                                    (xAOD::TrigBphys::pType)trigPartDs->particleType(),
                                                                                    trigPartDs->mass(),
                                                                                    xAOD::TrigBphys::EF );
                                                    
                                                    
                                                    xaod_trigPartBcMuMuDs->setFitmass     (trigPartBcMuMuDs->fitmass());
                                                    xaod_trigPartBcMuMuDs->setFitchi2     (trigPartBcMuMuDs->fitchi2());
                                                    xaod_trigPartBcMuMuDs->setFitndof     (trigPartBcMuMuDs->fitndof());
                                                    xaod_trigPartBcMuMuDs->setFitx        (trigPartBcMuMuDs->fitx());
                                                    xaod_trigPartBcMuMuDs->setFity        (trigPartBcMuMuDs->fity());
                                                    xaod_trigPartBcMuMuDs->setFitz        (trigPartBcMuMuDs->fitz());
                                                    
                                                    delete trigPartBcMuMuDs; // done with the old object now.
                                                    trigPartBcMuMuDs =0;
                                                    
                                                    xaod_trigPartDs->setFitmass     (trigPartDs->fitmass());
                                                    xaod_trigPartDs->setFitchi2     (trigPartDs->fitchi2());
                                                    xaod_trigPartDs->setFitndof     (trigPartDs->fitndof());
                                                    xaod_trigPartDs->setFitx        (trigPartDs->fitx());
                                                    xaod_trigPartDs->setFity        (trigPartDs->fity());
                                                    xaod_trigPartDs->setFitz        (trigPartDs->fitz());
                                                    
                                                    delete trigPartDs; // done with the old object now.
                                                    trigPartDs =0;

                                                    
                                                    
                                                    ElementLink<xAOD::TrackParticleContainer> trackEL3(*xAODTrkParticleCont,itrk1);
                                                    ElementLink<xAOD::TrackParticleContainer> trackEL4(*xAODTrkParticleCont,itrk2);
                                                    ElementLink<xAOD::TrackParticleContainer> trackEL5(*xAODTrkParticleCont,itrk3);
                                                    xaod_trigPartDs->addTrackParticleLink(trackEL3);
                                                    xaod_trigPartDs->addTrackParticleLink(trackEL4);
                                                    xaod_trigPartDs->addTrackParticleLink(trackEL5);
                                                    //m_trigBphysColl_X->push_back(trigPartDs);
                                                    int iDs = mTrigBphysColl_X->size() - 1;
                                                    ElementLink<xAOD::TrigBphysContainer> trigPartXEL(*mTrigBphysColl_X,iDs);
                                                    xaod_trigPartBcMuMuDs->addTrackParticleLink(trackEL3);
                                                    xaod_trigPartBcMuMuDs->addTrackParticleLink(trackEL4);
                                                    xaod_trigPartBcMuMuDs->addTrackParticleLink(trackEL5);
                                                    if(Found1Track) xaod_trigPartBcMuMuDs->addTrackParticleLink(trackELmu1);
                                                    if(Found2Track) xaod_trigPartBcMuMuDs->addTrackParticleLink(trackELmu2);
                                                    xaod_trigPartBcMuMuDs->setSecondaryDecayLink(trigPartXEL);
                                                    //m_trigBphysColl_b->push_back(trigPartBcMuMuDs);
                                                    
                                                    result=true;
                                                    
                                                    mon_BcMuMuDs_n++;
                                                    
                                                    if(IdEvent!=m_lastEventPassedBc) {
                                                        m_countPassedEventsBc++;
                                                        m_lastEventPassedBc = IdEvent;
                                                    }
                                                }
                                            }// end of 3rd track loop
                                        } // end if ( xPhiMuMuMass < m_upperBc_DsMuMuMassCut - PIMASS )
                                    } // end if ( xPhiMass > m_lowerPhiDs_MassCut && xPhiMass < m_upperPhiDs_MassCut )
                                } // end if(m_doBc_DsMuMuDecay)
                            } // end if(m_doBd_KstarMuMuDecay || m_doBs_Phi1020MuMuDecay || m_doLb_LambdaMuMuDecay)
                        } // end of inner loop over tracks
                        
                    } // end of outer loop over tracks
                } // end if muMuVxCandidate
                delete muMuVxCandidate;
                
            } // end of inner muon loop
        } // end of outer muon loop
    } // end if muidIDtracks1,2 sizes >= 0
    
    if(msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "Totally tried to build " << nTriedCombinations << " tracks cobinations" << endreq;
    mon_nTriedCombinations = nTriedCombinations;
    
    if(mTrigBphysColl_b!=0 && mTrigBphysColl_b->size()>0) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: Store Bphys Collection " << mTrigBphysColl_b << " size: " << mTrigBphysColl_b->size() << endreq;
        HLT::ErrorCode sc = attachFeature(outputTE, mTrigBphysColl_b, "EFBMuMuXFex" );
        if(sc != HLT::OK) {
            msg() << MSG::WARNING << "Failed to store trigBphys Collection in outputTE" << endreq;
            mon_Errors.push_back(ERROR_BphysCollStore_Fails);
            delete mTrigBphysColl_b;
            delete mTrigBphysColl_X;
            if ( timerSvc() ) m_TotTimer->stop();
            return HLT::ERROR;
        }
        sc = attachFeature(outputTE, mTrigBphysColl_X, "EFBMuMuXFex_X" );
        if(sc != HLT::OK) {
            msg() << MSG::WARNING << "Failed to store trigBphys_X Collection in outputTE" << endreq;
            mon_Errors.push_back(ERROR_BphysCollStore_Fails);
            delete mTrigBphysColl_b;
            delete mTrigBphysColl_X;
            if ( timerSvc() ) m_TotTimer->stop();
            return HLT::ERROR;
        }
    } else {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "REGTEST: no bphys collection to store "  << endreq;
        delete mTrigBphysColl_b;
        delete mTrigBphysColl_X;
    }
    
    if(result) {
        if(IdEvent!=m_lastEventPassed) {
            m_countPassedEvents++;
            m_lastEventPassed = IdEvent;
        }
        m_countPassedRoIs++;
    }
    
    if ( timerSvc() )  {
        m_TotTimer->stop();
        m_VtxFitTimer->resume();
        m_VtxFitTimer->stop();
        mon_TotalRunTime   = m_TotTimer->elapsed();
        mon_VertexingTime  = m_VtxFitTimer->elapsed();
    }
    
    if ( timerSvc() ) {
        ATH_MSG(DEBUG) << "m_TotTimer->elapsed()    = " << m_TotTimer->elapsed() << endreq;
        ATH_MSG(DEBUG) << "m_VtxFitTimer->elapsed() = " << m_VtxFitTimer->elapsed() << endreq;
    }
    
    return HLT::OK;
    
}

/*----------------------------------------------------------------------------*/
TrigEFBphys* TrigEFBMuMuXFex::checkBMuMu2X(const Trk::Track* mu1, const Trk::Track* mu2, const Trk::Track* track1, const Trk::Track* track2, const int decay, TrigEFBphys* & trigPartX)
{
    TrigEFBphys* trigPartXMuMu(0);
    
    double lowerXMassCut, upperXMassCut;
    double lowerXMuMuMassCut, upperXMuMuMassCut;
    double chi2XCut;
    double chi2XMuMuCut;
    std::string decayName;
    TrigEFBphys::pType decayType;
    unsigned int *countPassedXMass;
    unsigned int *countPassedXMuMuMass;
    unsigned int *countPassedXVtx;
    unsigned int *countPassedXVtxChi2;
    unsigned int *countPassedXMuMuVtx;
    unsigned int *countPassedXMuMuVtxChi2;
    
    bool do2XVertexing;
    bool doBMuMu2XVertexing;
    
    std::vector<float> *pMon_BMuMu2X_Pt_X1;
    std::vector<float> *pMon_BMuMu2X_Eta_X1;
    std::vector<float> *pMon_BMuMu2X_Phi_X1;
    std::vector<float> *pMon_BMuMu2X_Pt_X2;
    std::vector<float> *pMon_BMuMu2X_Eta_X2;
    std::vector<float> *pMon_BMuMu2X_Phi_X2;
    std::vector<float> *pMon_BMuMu2X_InvMass_2X;
    std::vector<float> *pMon_BMuMu2X_VtxMass_2X;
    std::vector<float> *pMon_BMuMu2X_Chi2_2X;
    std::vector<float> *pMon_BMuMu2X_InvMass_B;
    std::vector<float> *pMon_BMuMu2X_VtxMass_B;
    std::vector<float> *pMon_BMuMu2X_Chi2_B;
    
    // Fix silly warning messages
    (void)pMon_BMuMu2X_VtxMass_2X;
    (void)pMon_BMuMu2X_VtxMass_B;
    
    if(decay == bD_to_Kstar) {
        lowerXMassCut = m_lowerKstar_KaonMassCut;
        upperXMassCut = m_upperKstar_KaonMassCut;
        chi2XCut = m_kStarVtxChi2Cut;
        lowerXMuMuMassCut = m_lowerBd_KstarMuMuMassCut;
        upperXMuMuMassCut = m_upperBd_KstarMuMuMassCut;
        chi2XMuMuCut = m_bDVtxChi2Cut;
        decayName = "Bd -> mu mu K*";
        decayType = TrigEFBphys::BDKSTMUMU;
        countPassedXMass        = &m_countPassedKstarMass;
        countPassedXMuMuMass    = &m_countPassedBdMass;
        countPassedXVtx         = &m_countPassedKstarVtx;
        countPassedXVtxChi2     = &m_countPassedKstarVtxChi2;
        countPassedXMuMuVtx     = &m_countPassedBdVtx;
        countPassedXMuMuVtxChi2 = &m_countPassedBdVtxChi2;
        do2XVertexing      = m_doKstar_KPiVertexing;
        doBMuMu2XVertexing = m_doBd_KstarMuMuVertexing;
        pMon_BMuMu2X_Pt_X1      = &mon_BdMuMuKs_Pt_K;
        pMon_BMuMu2X_Eta_X1     = &mon_BdMuMuKs_Eta_K;
        pMon_BMuMu2X_Phi_X1     = &mon_BdMuMuKs_Phi_K;
        pMon_BMuMu2X_Pt_X2      = &mon_BdMuMuKs_Pt_Pi;
        pMon_BMuMu2X_Eta_X2     = &mon_BdMuMuKs_Eta_Pi;
        pMon_BMuMu2X_Phi_X2     = &mon_BdMuMuKs_Phi_Pi;
        pMon_BMuMu2X_InvMass_2X = &mon_BdMuMuKs_InvMass_Kstar;
        pMon_BMuMu2X_VtxMass_2X = &mon_BdMuMuKs_VtxMass_Kstar;
        pMon_BMuMu2X_Chi2_2X    = &mon_BdMuMuKs_Chi2_Kstar;
        pMon_BMuMu2X_InvMass_B  = &mon_BdMuMuKs_InvMass_Bd;
        pMon_BMuMu2X_VtxMass_B  = &mon_BdMuMuKs_VtxMass_Bd;
        pMon_BMuMu2X_Chi2_B     = &mon_BdMuMuKs_Chi2_Bd;
    } else if(decay == bS_to_Phi) {
        lowerXMassCut = m_lowerPhi1020_KaonMassCut;
        upperXMassCut = m_upperPhi1020_KaonMassCut;
        chi2XCut = m_phi1020VtxChi2Cut;
        lowerXMuMuMassCut = m_lowerBs_Phi1020MuMuMassCut;
        upperXMuMuMassCut = m_upperBs_Phi1020MuMuMassCut;
        chi2XMuMuCut = m_bSVtxChi2Cut;
        decayName = "Bs -> mu mu phi";
        decayType = TrigEFBphys::BSPHIMUMU;
        countPassedXMass        = &m_countPassedPhi1020Mass;
        countPassedXMuMuMass    = &m_countPassedBsMass;
        countPassedXVtx         = &m_countPassedPhi1020Vtx;
        countPassedXVtxChi2     = &m_countPassedPhi1020VtxChi2;
        countPassedXMuMuVtx     = &m_countPassedBsVtx;
        countPassedXMuMuVtxChi2 = &m_countPassedBsVtxChi2;
        do2XVertexing      = m_doPhi1020_KKVertexing;
        doBMuMu2XVertexing = m_doBs_Phi1020MuMuVertexing;
        pMon_BMuMu2X_Pt_X1      = &mon_BsMuMuPhi_Pt_K1;
        pMon_BMuMu2X_Eta_X1     = &mon_BsMuMuPhi_Eta_K1;
        pMon_BMuMu2X_Phi_X1     = &mon_BsMuMuPhi_Phi_K1;
        pMon_BMuMu2X_Pt_X2      = &mon_BsMuMuPhi_Pt_K2;
        pMon_BMuMu2X_Eta_X2     = &mon_BsMuMuPhi_Eta_K2;
        pMon_BMuMu2X_Phi_X2     = &mon_BsMuMuPhi_Phi_K2;
        pMon_BMuMu2X_InvMass_2X = &mon_BsMuMuPhi_InvMass_Phi1020;
        pMon_BMuMu2X_VtxMass_2X = &mon_BsMuMuPhi_VtxMass_Phi1020;
        pMon_BMuMu2X_Chi2_2X    = &mon_BsMuMuPhi_Chi2_Phi1020;
        pMon_BMuMu2X_InvMass_B  = &mon_BsMuMuPhi_InvMass_Bs;
        pMon_BMuMu2X_VtxMass_B  = &mon_BsMuMuPhi_VtxMass_Bs;
        pMon_BMuMu2X_Chi2_B     = &mon_BsMuMuPhi_Chi2_Bs;
    } else if(decay == lB_to_L) {
        lowerXMassCut = m_lowerLambda_PrPiMassCut;
        upperXMassCut = m_upperLambda_PrPiMassCut;
        chi2XCut = m_lambdaVtxChi2Cut;
        lowerXMuMuMassCut = m_lowerLb_LambdaMuMuMassCut;
        upperXMuMuMassCut = m_upperLb_LambdaMuMuMassCut;
        chi2XMuMuCut = m_lBVtxChi2Cut;
        decayName = "Lambda_b -> mu mu Lambda";
        decayType = TrigEFBphys::LBLMUMU;
        countPassedXMass        = &m_countPassedLambdaMass;
        countPassedXMuMuMass    = &m_countPassedLbMass;
        countPassedXVtx         = &m_countPassedLambdaVtx;
        countPassedXVtxChi2     = &m_countPassedLambdaVtxChi2;
        countPassedXMuMuVtx     = &m_countPassedLbVtx;
        countPassedXMuMuVtxChi2 = &m_countPassedLbVtxChi2;
        do2XVertexing      = m_doLambda_PPiVertexing;
        doBMuMu2XVertexing = m_doLb_LambdaMuMuVertexing;
        pMon_BMuMu2X_Pt_X1      = &mon_LbMuMuLambda_Pt_P;
        pMon_BMuMu2X_Eta_X1     = &mon_LbMuMuLambda_Eta_P;
        pMon_BMuMu2X_Phi_X1     = &mon_LbMuMuLambda_Phi_P;
        pMon_BMuMu2X_Pt_X2      = &mon_LbMuMuLambda_Pt_Pi;
        pMon_BMuMu2X_Eta_X2     = &mon_LbMuMuLambda_Eta_Pi;
        pMon_BMuMu2X_Phi_X2     = &mon_LbMuMuLambda_Phi_Pi;
        pMon_BMuMu2X_InvMass_2X = &mon_LbMuMuLambda_InvMass_Lambda;
        pMon_BMuMu2X_VtxMass_2X = &mon_LbMuMuLambda_VtxMass_Lambda;
        pMon_BMuMu2X_Chi2_2X    = &mon_LbMuMuLambda_Chi2_Lambda;
        pMon_BMuMu2X_InvMass_B  = &mon_LbMuMuLambda_InvMass_Lb;
        pMon_BMuMu2X_VtxMass_B  = &mon_LbMuMuLambda_VtxMass_Lb;
        pMon_BMuMu2X_Chi2_B     = &mon_LbMuMuLambda_Chi2_Lb;
    } else {
        ATH_MSG(DEBUG) << "Wrong decay identifier passed to checkBMuMu2X: decay = " << decay << endreq;
        mon_Errors.push_back(ERROR_WrongDecayID);
        return trigPartXMuMu;
    }
    
    ATH_MSG(DEBUG) << "Try to build " << decayName << " with tracks " << track1 << ", " << track2 << endreq;
    
    float massX = XMass(track1, track2, decay);
    if( !(massX > lowerXMassCut && massX < upperXMassCut) ) {
        ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by X mass cut: m = " << massX << endreq;
    } else {
        (*countPassedXMass)++;
        
        float massXMuMu = XMuMuMass(mu1, mu2, track1, track2, decay);
        if( !(massXMuMu > lowerXMuMuMassCut && massXMuMu < upperXMuMuMassCut) ) {
            ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by XMuMu mass cut: m = " << massXMuMu << endreq;
        } else {
            (*countPassedXMuMuMass)++;
            
            if(do2XVertexing) {
                // Try to fit X -> track1 track2 vertex
                if ( timerSvc() ) m_VtxFitTimer->resume();
                Amg::Vector3D vtx ( 0.,0.,0. );
                Trk::Vertex vertex ( vtx );
                std::vector<const Trk::Track*> trackPair;
                trackPair.push_back(track1);
                trackPair.push_back(track2);
                Trk::VxCandidate * XVxCandidate ( 0 );
                XVxCandidate = m_fitterSvc->fit(trackPair,vertex);
                if ( timerSvc() ) m_VtxFitTimer->pause();
                
                if( !XVxCandidate ) {
                    ATH_MSG(DEBUG) << " Failed to fit X vertex for " << decayName << endreq;
                    mon_Errors.push_back(ERROR_XVtxFit_Fails);
                } else {
                    (*countPassedXVtx)++;
                    
                    float chi2X = XVxCandidate->recVertex().fitQuality().chiSquared();
                    if( !(chi2X < chi2XCut) ) {
                        ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by X vertex chi2 cut: chi2 = " << chi2X << endreq;
                    } else {
                        (*countPassedXVtxChi2)++;
                        
                        // calculate X mass from vertexing
                        double vtxMassX(-1.), vtxMassErrorX(-1.);
                        std::vector<int> trkIndicesX;
                        for (int i=0;i<(int)trackPair.size();++i) {trkIndicesX.push_back(1);}
                        if( !(m_VKVFitter->VKalGetMassError(trkIndicesX,vtxMassX,vtxMassErrorX).isSuccess()) ) {
                            ATH_MSG(DEBUG) << "Warning from VKalVrt - cannot calculate fitmass and error for X in " << decayName << "!" << endreq;
                            mon_Errors.push_back(ERROR_XVtxMass_Fails);
                        }
                        
                        if(doBMuMu2XVertexing) {
                            // Try to fit XMuMu -> mu mu X vertex
                            if ( timerSvc() ) m_VtxFitTimer->resume();
                            std::vector<const Trk::Track*> quartet;
                            quartet.push_back(mu1);
                            quartet.push_back(mu2);
                            quartet.push_back(track1);
                            quartet.push_back(track2);
                            Trk::VxCandidate * XMuMuVxCandidate ( 0 );
                            XMuMuVxCandidate = m_fitterSvc->fit(quartet,vertex);
                            if ( timerSvc() ) m_VtxFitTimer->pause();
                            
                            if( !XMuMuVxCandidate ) {
                                ATH_MSG(DEBUG) << " Failed to fit XMuMu vertex for " << decayName << endreq;
                                mon_Errors.push_back(ERROR_XMuMuVtxFit_Fails);
                            } else {
                                (*countPassedXMuMuVtx)++;
                                
                                float chi2XMuMu = XMuMuVxCandidate->recVertex().fitQuality().chiSquared();
                                if( !(chi2XMuMu < chi2XMuMuCut) ) {
                                    ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu << endreq;
                                } else {
                                    (*countPassedXMuMuVtxChi2)++;
                                    
                                    //                   // calculate XMuMu mass from vertexing
                                    //                   double vtxMassXMuMu(-1.), vtxMassErrorXMuMu(-1.);
                                    //                   std::vector<int> trkIndicesXMuMu;
                                    //                   for (int i=0;i<(int)quartet.size();++i) {trkIndicesXMuMu.push_back(1);}
                                    //                   if( !(m_VKVFitter->VKalGetMassError(trkIndicesXMuMu,vtxMassXMuMu,vtxMassErrorXMuMu).isSuccess()) ) {
                                    //                     ATH_MSG(DEBUG) << "Warning from VKalVrt - cannot calculate fitmass and error for XMuMu in " << decayName << "!" << endreq;
                                    //                     mon_Errors.push_back(ERROR_XMuMuVtxMass_Fails);
                                    //                   }
                                    
                                    trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, massX);
                                    trigPartX->fitchi2(XVxCandidate->recVertex().fitQuality().chiSquared());
                                    //                   trigPartX->fitmass(vtxMassX);
                                    trigPartX->fitx(XVxCandidate->recVertex().position() [0]);
                                    trigPartX->fity(XVxCandidate->recVertex().position() [1]);
                                    trigPartX->fitz(XVxCandidate->recVertex().position() [2]);
                                    
                                    //                   trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massXMuMu);
                                    trigPartXMuMu = new TrigEFBphys( 0, 0., 0., decayType, massXMuMu);
                                    trigPartXMuMu->fitchi2(XMuMuVxCandidate->recVertex().fitQuality().chiSquared());
                                    //                   trigPartXMuMu->fitmass(vtxMassXMuMu);
                                    trigPartXMuMu->fitx(XMuMuVxCandidate->recVertex().position() [0]);
                                    trigPartXMuMu->fity(XMuMuVxCandidate->recVertex().position() [1]);
                                    trigPartXMuMu->fitz(XMuMuVxCandidate->recVertex().position() [2]);
                                    
                                    // Fill BMuMu2X monitoring containers
                                    pMon_BMuMu2X_Pt_X1->push_back(track1->perigeeParameters()->pT()/1000.);
                                    pMon_BMuMu2X_Eta_X1->push_back(track1->perigeeParameters()->eta());
                                    pMon_BMuMu2X_Phi_X1->push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                                    pMon_BMuMu2X_Pt_X2->push_back(track2->perigeeParameters()->pT()/1000.);
                                    pMon_BMuMu2X_Eta_X2->push_back(track2->perigeeParameters()->eta());
                                    pMon_BMuMu2X_Phi_X2->push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                                    pMon_BMuMu2X_InvMass_2X->push_back(massX/1000.);
                                    //                   pMon_BMuMu2X_VtxMass_2X->push_back(vtxMassX/1000.);
                                    pMon_BMuMu2X_Chi2_2X->push_back(chi2X);
                                    pMon_BMuMu2X_InvMass_B->push_back(massXMuMu/1000.);
                                    //                   pMon_BMuMu2X_VtxMass_B->push_back(vtxMassXMuMu/1000.);
                                    pMon_BMuMu2X_Chi2_B->push_back(chi2XMuMu);
                                    
                                    if(msgLvl() <= MSG::DEBUG)
                                        msg() << MSG::DEBUG << " Good " << decayName << " found!" << std::endl
                                        << "  m = " << trigPartXMuMu->mass() << ", "
                                        << "chi2 = " << trigPartXMuMu->fitchi2() << ", vertex (" << trigPartXMuMu->fitx() << ", "
                                        << trigPartXMuMu->fity() << ", " << trigPartXMuMu->fitz() << ")" << endreq;
                                } // end XMuMu chi2 cut
                            } // end if(XMuMuVxCandidate)
                            delete XMuMuVxCandidate;
                        } // end if(doBMuMu2XVertexing), do2XVertexing
                        else {
                            trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, massX);
                            trigPartX->fitchi2(XVxCandidate->recVertex().fitQuality().chiSquared());
                            //               trigPartX->fitmass(vtxMassX);
                            trigPartX->fitx(XVxCandidate->recVertex().position() [0]);
                            trigPartX->fity(XVxCandidate->recVertex().position() [1]);
                            trigPartX->fitz(XVxCandidate->recVertex().position() [2]);
                            
                            //               trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massXMuMu);
                            trigPartXMuMu = new TrigEFBphys( 0, 0., 0., decayType, massXMuMu);
                            //               trigPartXMuMu->fitchi2(XMuMuVxCandidate->recVertex().fitQuality().chiSquared());
                            //               trigPartXMuMu->fitmass(vtxMassXMuMu);
                            //               trigPartXMuMu->fitx(XMuMuVxCandidate->recVertex().position() [0]);
                            //               trigPartXMuMu->fity(XMuMuVxCandidate->recVertex().position() [1]);
                            //               trigPartXMuMu->fitz(XMuMuVxCandidate->recVertex().position() [2]);
                            
                            // Fill BMuMu2X monitoring containers
                            pMon_BMuMu2X_Pt_X1->push_back(track1->perigeeParameters()->pT()/1000.);
                            pMon_BMuMu2X_Eta_X1->push_back(track1->perigeeParameters()->eta());
                            pMon_BMuMu2X_Phi_X1->push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                            pMon_BMuMu2X_Pt_X2->push_back(track2->perigeeParameters()->pT()/1000.);
                            pMon_BMuMu2X_Eta_X2->push_back(track2->perigeeParameters()->eta());
                            pMon_BMuMu2X_Phi_X2->push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                            pMon_BMuMu2X_InvMass_2X->push_back(massX/1000.);
                            //               pMon_BMuMu2X_VtxMass_2X->push_back(vtxMassX/1000.);
                            pMon_BMuMu2X_Chi2_2X->push_back(chi2X);
                            pMon_BMuMu2X_InvMass_B->push_back(massXMuMu/1000.);
                            //               pMon_BMuMu2X_VtxMass_B->push_back(vtxMassXMuMu/1000.);
                            //               pMon_BMuMu2X_Chi2_B->push_back(chi2XMuMu);
                            
                            if(msgLvl() <= MSG::DEBUG)
                                msg() << MSG::DEBUG << " Good " << decayName << " found (no BMuMu2X vertexing)!" << std::endl
                                << "  m = " << trigPartXMuMu->mass() << endreq;
                        } // end if(!doBMuMu2XVertexing), do2XVertexing
                        
                    } // end X chi2 cut
                } // end if(XVxCandidate)
                delete XVxCandidate;
            } // end if(do2XVertexing)
            else {
                if(doBMuMu2XVertexing) { // !do2XVertexing, STUPID CONFIGURATION
                    // Try to fit XMuMu -> mu mu X vertex
                    Amg::Vector3D vtx ( 0.,0.,0. );
                    Trk::Vertex vertex ( vtx );
                    if ( timerSvc() ) m_VtxFitTimer->resume();
                    std::vector<const Trk::Track*> quartet;
                    quartet.push_back(mu1);
                    quartet.push_back(mu2);
                    quartet.push_back(track1);
                    quartet.push_back(track2);
                    Trk::VxCandidate * XMuMuVxCandidate ( 0 );
                    XMuMuVxCandidate = m_fitterSvc->fit(quartet,vertex);
                    if ( timerSvc() ) m_VtxFitTimer->pause();
                    
                    if( !XMuMuVxCandidate ) {
                        ATH_MSG(DEBUG) << " Failed to fit XMuMu vertex for " << decayName << endreq;
                        mon_Errors.push_back(ERROR_XMuMuVtxFit_Fails);
                    } else {
                        (*countPassedXMuMuVtx)++;
                        
                        float chi2XMuMu = XMuMuVxCandidate->recVertex().fitQuality().chiSquared();
                        if( !(chi2XMuMu < chi2XMuMuCut) ) {
                            ATH_MSG(DEBUG) << " " << decayName << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu << endreq;
                        } else {
                            (*countPassedXMuMuVtxChi2)++;
                            
                            //               // calculate XMuMu mass from vertexing
                            //               double vtxMassXMuMu(-1.), vtxMassErrorXMuMu(-1.);
                            //               std::vector<int> trkIndicesXMuMu;
                            //               for (int i=0;i<(int)quartet.size();++i) {trkIndicesXMuMu.push_back(1);}
                            //               if( !(m_VKVFitter->VKalGetMassError(trkIndicesXMuMu,vtxMassXMuMu,vtxMassErrorXMuMu).isSuccess()) ) {
                            //                 ATH_MSG(DEBUG) << "Warning from VKalVrt - cannot calculate fitmass and error for XMuMu in " << decayName << "!" << endreq;
                            //                 mon_Errors.push_back(ERROR_XMuMuVtxMass_Fails);
                            //               }
                            
                            trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, massX);
                            
                            //               trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massXMuMu);
                            trigPartXMuMu = new TrigEFBphys( 0, 0., 0., decayType, massXMuMu);
                            trigPartXMuMu->fitchi2(XMuMuVxCandidate->recVertex().fitQuality().chiSquared());
                            //               trigPartXMuMu->fitmass(vtxMassXMuMu);
                            trigPartXMuMu->fitx(XMuMuVxCandidate->recVertex().position() [0]);
                            trigPartXMuMu->fity(XMuMuVxCandidate->recVertex().position() [1]);
                            trigPartXMuMu->fitz(XMuMuVxCandidate->recVertex().position() [2]);
                            
                            // Fill BMuMu2X monitoring containers
                            pMon_BMuMu2X_Pt_X1->push_back(track1->perigeeParameters()->pT()/1000.);
                            pMon_BMuMu2X_Eta_X1->push_back(track1->perigeeParameters()->eta());
                            pMon_BMuMu2X_Phi_X1->push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                            pMon_BMuMu2X_Pt_X2->push_back(track2->perigeeParameters()->pT()/1000.);
                            pMon_BMuMu2X_Eta_X2->push_back(track2->perigeeParameters()->eta());
                            pMon_BMuMu2X_Phi_X2->push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                            pMon_BMuMu2X_InvMass_2X->push_back(massX/1000.);
                            //               pMon_BMuMu2X_VtxMass_2X->push_back(vtxMassX/1000.);
                            //               pMon_BMuMu2X_Chi2_2X->push_back(chi2X);
                            pMon_BMuMu2X_InvMass_B->push_back(massXMuMu/1000.);
                            //               pMon_BMuMu2X_VtxMass_B->push_back(vtxMassXMuMu/1000.);
                            pMon_BMuMu2X_Chi2_B->push_back(chi2XMuMu);
                            
                            if(msgLvl() <= MSG::DEBUG)
                                msg() << MSG::DEBUG << " Good " << decayName << " found (no 2X vertexing)!" << std::endl
                                << "  m = " << trigPartXMuMu->mass() << ", "
                                << "chi2 = " << trigPartXMuMu->fitchi2() << ", vertex (" << trigPartXMuMu->fitx() << ", "
                                << trigPartXMuMu->fity() << ", " << trigPartXMuMu->fitz() << ")" << endreq;
                        } // end XMuMu chi2 cut
                    } // end if(XMuMuVxCandidate)
                    delete XMuMuVxCandidate;
                } // end if(doBMuMu2XVertexing), !do2XVertexing
                else {
                    trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, massX);
                    
                    //           trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massXMuMu);
                    trigPartXMuMu = new TrigEFBphys( 0, 0., 0., decayType, massXMuMu);
                    
                    // Fill BMuMu2X monitoring containers
                    pMon_BMuMu2X_Pt_X1->push_back(track1->perigeeParameters()->pT()/1000.);
                    pMon_BMuMu2X_Eta_X1->push_back(track1->perigeeParameters()->eta());
                    pMon_BMuMu2X_Phi_X1->push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                    pMon_BMuMu2X_Pt_X2->push_back(track2->perigeeParameters()->pT()/1000.);
                    pMon_BMuMu2X_Eta_X2->push_back(track2->perigeeParameters()->eta());
                    pMon_BMuMu2X_Phi_X2->push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                    pMon_BMuMu2X_InvMass_2X->push_back(massX/1000.);
                    //               pMon_BMuMu2X_VtxMass_2X->push_back(vtxMassX/1000.);
                    //               pMon_BMuMu2X_Chi2_2X->push_back(chi2X);
                    pMon_BMuMu2X_InvMass_B->push_back(massXMuMu/1000.);
                    //           pMon_BMuMu2X_VtxMass_B->push_back(vtxMassXMuMu/1000.);
                    //           pMon_BMuMu2X_Chi2_B->push_back(chi2XMuMu);
                    
                    if(msgLvl() <= MSG::DEBUG)
                        msg() << MSG::DEBUG << " Good " << decayName << " found (no 2X vertexing, no BMuMu2X vertexing)!" << std::endl
                        << "  m = " << trigPartXMuMu->mass() << endreq;
                }// end if(!doBMuMu2XVertexing), !do2XVertexing
                
            } // end if(!do2XVertexing)
            
        } // end if XMuMu mass cut
    } // end if X mass cut
    
    return trigPartXMuMu;
    
}

/*----------------------------------------------------------------------------*/
TrigEFBphys* TrigEFBMuMuXFex::checkBcMuMuDs(const Trk::Track* mu1, const Trk::Track* mu2, const Trk::Track* track1, const Trk::Track* track2, double xPhiMass, const Trk::Track* track3, TrigEFBphys* & trigPartX)
{
    TrigEFBphys* trigPartXMuMu(0);
    
    m_countPassedPhiDsMass++;
    
    ATH_MSG(DEBUG) << "Try to build " << "D_s" << " with tracks " << track1 << ", " << track2 << ", " << track3 << endreq;
    
    float massX = X3Mass(track1, track2, track3);
    if( !(massX > m_lowerDs_MassCut && massX < m_upperDs_MassCut) ) {
        ATH_MSG(DEBUG) << " " << "D_s" << " candidate rejected by the mass cut: m = " << massX << endreq;
    } else {
        m_countPassedDsMass++;
        
        float massXMuMu = X3MuMuMass(mu1, mu2, track1, track2, track3);
        if( !(massXMuMu > m_lowerBc_DsMuMuMassCut && massXMuMu < m_upperBc_DsMuMuMassCut) ) {
            ATH_MSG(DEBUG) << " " << "B_c" << " candidate rejected by the mass cut: m = " << massXMuMu << endreq;
        } else {
            m_countPassedBcMass++;
            
            if(m_doDs_Vertexing) {
                // Try to fit X -> track1 track2 track3 vertex
                if ( timerSvc() ) m_VtxFitTimer->resume();
                Amg::Vector3D vtx ( 0.,0.,0. );
                Trk::Vertex vertex ( vtx );
                std::vector<const Trk::Track*> trackTroika;
                trackTroika.push_back(track1);
                trackTroika.push_back(track2);
                trackTroika.push_back(track3);
                Trk::VxCandidate * XVxCandidate ( 0 );
                XVxCandidate = m_fitterSvc->fit(trackTroika,vertex);
                if ( timerSvc() ) m_VtxFitTimer->pause();
                
                if( !XVxCandidate ) {
                    ATH_MSG(DEBUG) << " Failed to fit X vertex for " << "D_s" << endreq;
                    mon_Errors.push_back(ERROR_XVtxFit_Fails);
                } else {
                    m_countPassedDsVtx++;
                    
                    float chi2X = XVxCandidate->recVertex().fitQuality().chiSquared();
                    if( !(chi2X < m_DsVtxChi2Cut) ) {
                        ATH_MSG(DEBUG) << " " << "D_s" << " candidate rejected by X vertex chi2 cut: chi2 = " << chi2X << endreq;
                    } else {
                        m_countPassedDsVtxChi2++;
                        
                        // calculate X mass from vertexing
                        double vtxMassX(-1.), vtxMassErrorX(-1.);
                        std::vector<int> trkIndicesX;
                        for (int i=0;i<(int)trackTroika.size();++i) {trkIndicesX.push_back(1);}
                        if( !(m_VKVFitter->VKalGetMassError(trkIndicesX,vtxMassX,vtxMassErrorX).isSuccess()) ) {
                            ATH_MSG(DEBUG) << "Warning from VKalVrt - cannot calculate fitmass and error for X in " << "D_s" << "!" << endreq;
                            mon_Errors.push_back(ERROR_XVtxMass_Fails);
                        }
                        
                        if(m_doBc_DsMuMuVertexing) {
                            // Try to fit XMuMu -> mu mu X vertex
                            if ( timerSvc() ) m_VtxFitTimer->resume();
                            std::vector<const Trk::Track*> quintet;
                            quintet.push_back(mu1);
                            quintet.push_back(mu2);
                            quintet.push_back(track1);
                            quintet.push_back(track2);
                            quintet.push_back(track3);
                            Trk::VxCandidate * XMuMuVxCandidate ( 0 );
                            XMuMuVxCandidate = m_fitterSvc->fit(quintet,vertex);
                            if ( timerSvc() ) m_VtxFitTimer->pause();
                            
                            if( !XMuMuVxCandidate ) {
                                ATH_MSG(DEBUG) << " Failed to fit XMuMu vertex for " << "B_c" << endreq;
                                mon_Errors.push_back(ERROR_XMuMuVtxFit_Fails);
                            } else {
                                m_countPassedBcVtx++;
                                
                                float chi2XMuMu = XMuMuVxCandidate->recVertex().fitQuality().chiSquared();
                                if( !(chi2XMuMu < m_bCVtxChi2Cut) ) {
                                    ATH_MSG(DEBUG) << " " << "B_c" << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu << endreq;
                                } else {
                                    m_countPassedBcVtxChi2++;
                                    
                                    //                  trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, xPhiMass);
                                    trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::DSPHIPI, massX);
                                    trigPartX->fitchi2(XVxCandidate->recVertex().fitQuality().chiSquared());
                                    //                   trigPartX->fitmass(vtxMassX);
                                    trigPartX->fitx(XVxCandidate->recVertex().position() [0]);
                                    trigPartX->fity(XVxCandidate->recVertex().position() [1]);
                                    trigPartX->fitz(XVxCandidate->recVertex().position() [2]);
                                    
                                    trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BCDSMUMU, massXMuMu);
                                    trigPartXMuMu->fitchi2(XMuMuVxCandidate->recVertex().fitQuality().chiSquared());
                                    //                   trigPartXMuMu->fitmass(vtxMassXMuMu);
                                    trigPartXMuMu->fitx(XMuMuVxCandidate->recVertex().position() [0]);
                                    trigPartXMuMu->fity(XMuMuVxCandidate->recVertex().position() [1]);
                                    trigPartXMuMu->fitz(XMuMuVxCandidate->recVertex().position() [2]);
                                    
                                    // Fill BMuMu2X monitoring containers
                                    mon_BcMuMuDs_Pt_K1.push_back(track1->perigeeParameters()->pT()/1000.);
                                    mon_BcMuMuDs_Eta_K1.push_back(track1->perigeeParameters()->eta());
                                    mon_BcMuMuDs_Phi_K1.push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                                    mon_BcMuMuDs_Pt_K2.push_back(track2->perigeeParameters()->pT()/1000.);
                                    mon_BcMuMuDs_Eta_K2.push_back(track2->perigeeParameters()->eta());
                                    mon_BcMuMuDs_Phi_K2.push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                                    mon_BcMuMuDs_Pt_pi.push_back(track3->perigeeParameters()->pT()/1000.);
                                    mon_BcMuMuDs_Eta_pi.push_back(track3->perigeeParameters()->eta());
                                    mon_BcMuMuDs_Phi_pi.push_back(track3->perigeeParameters()->parameters()[Trk::phi0]);
                                    mon_BcMuMuDs_InvMass_PhiDs.push_back(xPhiMass/1000.);
                                    mon_BcMuMuDs_InvMass_Ds.push_back(massX/1000.);
                                    //                   mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
                                    mon_BcMuMuDs_Chi2_Ds.push_back(chi2X);
                                    mon_BcMuMuDs_InvMass_Bc.push_back(massXMuMu/1000.);
                                    //                   mon_BcMuMuDs_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
                                    mon_BcMuMuDs_Chi2_Bc.push_back(chi2XMuMu);
                                    
                                    if(msgLvl() <= MSG::DEBUG)
                                        msg() << MSG::DEBUG << " Good " << "B_c" << " found!" << std::endl
                                        << "  m = " << trigPartXMuMu->mass() << ", "
                                        << "chi2 = " << trigPartXMuMu->fitchi2() << ", vertex (" << trigPartXMuMu->fitx() << ", "
                                        << trigPartXMuMu->fity() << ", " << trigPartXMuMu->fitz() << ")" << endreq;
                                } // end XMuMu chi2 cut
                            } // end if(XMuMuVxCandidate)
                            delete XMuMuVxCandidate;
                        } // end if(doBMuMu2XVertexing), do2XVertexing
                        else {
                            //              trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, xPhiMass);
                            trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::DSPHIPI, massX);
                            trigPartX->fitchi2(XVxCandidate->recVertex().fitQuality().chiSquared());
                            //               trigPartX->fitmass(vtxMassX);
                            trigPartX->fitx(XVxCandidate->recVertex().position() [0]);
                            trigPartX->fity(XVxCandidate->recVertex().position() [1]);
                            trigPartX->fitz(XVxCandidate->recVertex().position() [2]);
                            
                            //               trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BCDSMUMU, massXMuMu);
                            trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BCDSMUMU, massXMuMu);
                            //               trigPartXMuMu->fitchi2(XMuMuVxCandidate->recVertex().fitQuality().chiSquared());
                            //               trigPartXMuMu->fitmass(vtxMassXMuMu);
                            //               trigPartXMuMu->fitx(XMuMuVxCandidate->recVertex().position() [0]);
                            //               trigPartXMuMu->fity(XMuMuVxCandidate->recVertex().position() [1]);
                            //               trigPartXMuMu->fitz(XMuMuVxCandidate->recVertex().position() [2]);
                            
                            // Fill BMuMu2X monitoring containers
                            mon_BcMuMuDs_Pt_K1.push_back(track1->perigeeParameters()->pT()/1000.);
                            mon_BcMuMuDs_Eta_K1.push_back(track1->perigeeParameters()->eta());
                            mon_BcMuMuDs_Phi_K1.push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                            mon_BcMuMuDs_Pt_K2.push_back(track2->perigeeParameters()->pT()/1000.);
                            mon_BcMuMuDs_Eta_K2.push_back(track2->perigeeParameters()->eta());
                            mon_BcMuMuDs_Phi_K2.push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                            mon_BcMuMuDs_Pt_pi.push_back(track3->perigeeParameters()->pT()/1000.);
                            mon_BcMuMuDs_Eta_pi.push_back(track3->perigeeParameters()->eta());
                            mon_BcMuMuDs_Phi_pi.push_back(track3->perigeeParameters()->parameters()[Trk::phi0]);
                            mon_BcMuMuDs_InvMass_PhiDs.push_back(xPhiMass/1000.);
                            mon_BcMuMuDs_InvMass_Ds.push_back(massX/1000.);
                            //               mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
                            mon_BcMuMuDs_Chi2_Ds.push_back(chi2X);
                            mon_BcMuMuDs_InvMass_Bc.push_back(massXMuMu/1000.);
                            //               mon_BcMuMuDs_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
                            //               mon_BcMuMuDs_Chi2_Bc.push_back(chi2XMuMu);
                            
                            if(msgLvl() <= MSG::DEBUG) 
                                msg() << MSG::DEBUG << " Good " << "B_c" << " found (no BMuMu2X vertexing)!" << std::endl
                                << "  m = " << trigPartXMuMu->mass() << endreq;
                        } // end if(!doBMuMu2XVertexing), do2XVertexing
                        
                    } // end X chi2 cut
                } // end if(XVxCandidate)
                delete XVxCandidate;
            } // end if(do2XVertexing)
            else {
                if(m_doBc_DsMuMuVertexing) { // !do2XVertexing, STUPID CONFIGURATION
                    // Try to fit XMuMu -> mu mu X vertex
                    Amg::Vector3D vtx ( 0.,0.,0. );
                    Trk::Vertex vertex ( vtx );
                    if ( timerSvc() ) m_VtxFitTimer->resume();
                    std::vector<const Trk::Track*> quintet;
                    quintet.push_back(mu1);
                    quintet.push_back(mu2);
                    quintet.push_back(track1);
                    quintet.push_back(track2);
                    quintet.push_back(track3);
                    Trk::VxCandidate * XMuMuVxCandidate ( 0 );
                    XMuMuVxCandidate = m_fitterSvc->fit(quintet,vertex);
                    if ( timerSvc() ) m_VtxFitTimer->pause();
                    
                    if( !XMuMuVxCandidate ) {
                        ATH_MSG(DEBUG) << " Failed to fit XMuMu vertex for " << "B_c" << endreq;
                        mon_Errors.push_back(ERROR_XMuMuVtxFit_Fails);
                    } else {
                        m_countPassedBcVtx++;
                        
                        float chi2XMuMu = XMuMuVxCandidate->recVertex().fitQuality().chiSquared();
                        if( !(chi2XMuMu < m_bCVtxChi2Cut) ) {
                            ATH_MSG(DEBUG) << " " << "B_c" << " candidate rejected by XMuMu vertex chi2 cut: chi2 = " << chi2XMuMu << endreq;
                        } else {
                            m_countPassedBcVtxChi2++;
                            
                            //               // calculate XMuMu mass from vertexing
                            //               double vtxMassXMuMu(-1.), vtxMassErrorXMuMu(-1.);
                            //               std::vector<int> trkIndicesXMuMu;
                            //               for (int i=0;i<(int)quartet.size();++i) {trkIndicesXMuMu.push_back(1);} 
                            //               if( !(m_VKVFitter->VKalGetMassError(trkIndicesXMuMu,vtxMassXMuMu,vtxMassErrorXMuMu).isSuccess()) ) {
                            //                 ATH_MSG(DEBUG) << "Warning from VKalVrt - cannot calculate fitmass and error for XMuMu in " << decayName << "!" << endreq;
                            //                 mon_Errors.push_back(ERROR_XMuMuVtxMass_Fails);
                            //               }
                            
                            //              trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, xPhiMass);
                            trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::DSPHIPI, massX);
                            
                            //               trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massXMuMu);
                            trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BCDSMUMU, massXMuMu);
                            trigPartXMuMu->fitchi2(XMuMuVxCandidate->recVertex().fitQuality().chiSquared());
                            //               trigPartXMuMu->fitmass(vtxMassXMuMu);
                            trigPartXMuMu->fitx(XMuMuVxCandidate->recVertex().position() [0]);
                            trigPartXMuMu->fity(XMuMuVxCandidate->recVertex().position() [1]);
                            trigPartXMuMu->fitz(XMuMuVxCandidate->recVertex().position() [2]);
                            
                            // Fill BMuMu2X monitoring containers
                            mon_BcMuMuDs_Pt_K1.push_back(track1->perigeeParameters()->pT()/1000.);
                            mon_BcMuMuDs_Eta_K1.push_back(track1->perigeeParameters()->eta());
                            mon_BcMuMuDs_Phi_K1.push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                            mon_BcMuMuDs_Pt_K2.push_back(track2->perigeeParameters()->pT()/1000.);
                            mon_BcMuMuDs_Eta_K2.push_back(track2->perigeeParameters()->eta());
                            mon_BcMuMuDs_Phi_K2.push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                            mon_BcMuMuDs_Pt_pi.push_back(track3->perigeeParameters()->pT()/1000.);
                            mon_BcMuMuDs_Eta_pi.push_back(track3->perigeeParameters()->eta());
                            mon_BcMuMuDs_Phi_pi.push_back(track3->perigeeParameters()->parameters()[Trk::phi0]);
                            mon_BcMuMuDs_InvMass_PhiDs.push_back(xPhiMass/1000.);
                            mon_BcMuMuDs_InvMass_Ds.push_back(massX/1000.);
                            //               mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
                            //               mon_BcMuMuDs_Chi2_Ds.push_back(chi2X);
                            mon_BcMuMuDs_InvMass_Bc.push_back(massXMuMu/1000.);
                            //               mon_BcMuMuDs_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
                            mon_BcMuMuDs_Chi2_Bc.push_back(chi2XMuMu);
                            
                            if(msgLvl() <= MSG::DEBUG) 
                                msg() << MSG::DEBUG << " Good " << "B_c" << " found (no 2X vertexing)!" << std::endl
                                << "  m = " << trigPartXMuMu->mass() << ", "
                                << "chi2 = " << trigPartXMuMu->fitchi2() << ", vertex (" << trigPartXMuMu->fitx() << ", " 
                                << trigPartXMuMu->fity() << ", " << trigPartXMuMu->fitz() << ")" << endreq;
                        } // end XMuMu chi2 cut
                    } // end if(XMuMuVxCandidate)
                    delete XMuMuVxCandidate;
                } // end if(doBMuMu2XVertexing), !do2XVertexing
                else {
                    //              trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::PHIKK, xPhiMass);
                    trigPartX = new TrigEFBphys( 0, 0., 0., TrigEFBphys::DSPHIPI, massX);
                    
                    //           trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massXMuMu);
                    trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BCDSMUMU, massXMuMu);
                    
                    // Fill BMuMu2X monitoring containers
                    mon_BcMuMuDs_Pt_K1.push_back(track1->perigeeParameters()->pT()/1000.);
                    mon_BcMuMuDs_Eta_K1.push_back(track1->perigeeParameters()->eta());
                    mon_BcMuMuDs_Phi_K1.push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                    mon_BcMuMuDs_Pt_K2.push_back(track2->perigeeParameters()->pT()/1000.);
                    mon_BcMuMuDs_Eta_K2.push_back(track2->perigeeParameters()->eta());
                    mon_BcMuMuDs_Phi_K2.push_back(track2->perigeeParameters()->parameters()[Trk::phi0]);
                    mon_BcMuMuDs_Pt_pi.push_back(track3->perigeeParameters()->pT()/1000.);
                    mon_BcMuMuDs_Eta_pi.push_back(track3->perigeeParameters()->eta());
                    mon_BcMuMuDs_Phi_pi.push_back(track3->perigeeParameters()->parameters()[Trk::phi0]);
                    mon_BcMuMuDs_InvMass_PhiDs.push_back(xPhiMass/1000.);
                    mon_BcMuMuDs_InvMass_Ds.push_back(massX/1000.);
                    //           mon_BcMuMuDs_VtxMass_Ds.push_back(vtxMassX/1000.);
                    //           mon_BcMuMuDs_Chi2_Ds.push_back(chi2X);
                    mon_BcMuMuDs_InvMass_Bc.push_back(massXMuMu/1000.);
                    //           mon_BcMuMuDs_VtxMass_Bc.push_back(vtxMassXMuMu/1000.);
                    //           mon_BcMuMuDs_Chi2_Bc.push_back(chi2XMuMu);
                    
                    if(msgLvl() <= MSG::DEBUG) 
                        msg() << MSG::DEBUG << " Good " << "B_c" << " found (no 2X vertexing, no BMuMu2X vertexing)!" << std::endl
                        << "  m = " << trigPartXMuMu->mass() << endreq;
                }// end if(!doBMuMu2XVertexing), !do2XVertexing
                
            } // end if(!do2XVertexing)
            
        } // end if XMuMu mass cut
    } // end if X mass cut
    
    return trigPartXMuMu;
    
}



/*----------------------------------------------------------------------------*/
TrigEFBphys* TrigEFBMuMuXFex::checkBplusMuMuKplus(const Trk::Track* mu1, const Trk::Track* mu2, const Trk::Track* track1)
{
    TrigEFBphys* trigPartXMuMu(0);
    
    ATH_MSG(DEBUG) << "Try to build B+ -> mu mu K+ with track " << track1 << endreq;
    
    float massKMuMu = KMuMuMass(mu1,mu2,track1);
    if( !(massKMuMu > m_lowerKMuMuMassCut && massKMuMu < m_upperKMuMuMassCut) ) {
        ATH_MSG(DEBUG) << " B+ -> mu mu K+ candidate rejected by mass cut: m = " << massKMuMu << endreq;
    } else {
        m_countPassedBplusMass++;
        
        if(m_doB_KMuMuVertexing) {
            // Try to fit B+ -> mu mu K+ vertex
            if ( timerSvc() ) m_VtxFitTimer->resume();
            Amg::Vector3D vtx ( 0.,0.,0. );
            Trk::Vertex vertex ( vtx );
            std::vector<const Trk::Track*> trio;
            trio.push_back(mu1);
            trio.push_back(mu2);
            trio.push_back(track1);
            Trk::VxCandidate * bPlusVxCandidate ( 0 );
            bPlusVxCandidate = m_fitterSvc->fit(trio,vertex);
            if ( timerSvc() ) m_VtxFitTimer->pause();
            
            if( !bPlusVxCandidate ) {
                ATH_MSG(DEBUG) << " Failed to fit B+ -> mu mu K+ vertex" << endreq;
                mon_Errors.push_back(ERROR_BplusVtxFit_Fails);
            } else {
                m_countPassedBplusVtx++;
                
                float chi2KMuMu = bPlusVxCandidate->recVertex().fitQuality().chiSquared();
                if( !(chi2KMuMu < m_bVtxChi2Cut) ) {
                    ATH_MSG(DEBUG) << " B+ -> mu mu K+ candidate rejected by chi2 cut: chi2 = " << chi2KMuMu << endreq;
                } else {
                    m_countPassedBplusVtxChi2++;
                    
                    //           // calculate KMuMu mass from vertexing
                    //           double vtxMassKMuMu(-1.), vtxMassErrorKMuMu(-1.);
                    //           std::vector<int> trkIndices;
                    //           for (int i=0;i<(int)trio.size();++i) {trkIndices.push_back(1);} 
                    //           if( !(m_VKVFitter->VKalGetMassError(trkIndices,vtxMassKMuMu,vtxMassErrorKMuMu).isSuccess()) ) {
                    //             ATH_MSG(DEBUG) << "Warning from VKalVrt - cannot calculate fitmass and error for B+ -> mu mu K+!" << endreq;
                    //             mon_Errors.push_back(ERROR_BplusVtxMass_Fails);
                    //           }
                    
                    //           trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massKMuMu);
                    trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BKMUMU, massKMuMu);
                    trigPartXMuMu->fitchi2(bPlusVxCandidate->recVertex().fitQuality().chiSquared());
                    //           trigPartXMuMu->fitmass(vtxMassKMuMu);
                    trigPartXMuMu->fitx(bPlusVxCandidate->recVertex().position() [0]);
                    trigPartXMuMu->fity(bPlusVxCandidate->recVertex().position() [1]);
                    trigPartXMuMu->fitz(bPlusVxCandidate->recVertex().position() [2]);
                    
                    // Fill BMuMuK monitoring containers
                    mon_BMuMuK_Pt_K.push_back(track1->perigeeParameters()->pT()/1000.);
                    mon_BMuMuK_Eta_K.push_back(track1->perigeeParameters()->eta());
                    mon_BMuMuK_Phi_K.push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
                    mon_BMuMuK_InvMass_B.push_back(massKMuMu/1000.);
                    //           mon_BMuMuK_VtxMass_B.push_back(vtxMassKMuMu/1000.);
                    mon_BMuMuK_Chi2_B.push_back(chi2KMuMu);
                    
                    if(msgLvl() <= MSG::DEBUG) 
                        msg() << MSG::DEBUG << " Good B+ -> mu mu K+ found!" << std::endl
                        << "  m = " << trigPartXMuMu->mass() << ", "
                        << "chi2 = " << trigPartXMuMu->fitchi2() << ", vertex (" << trigPartXMuMu->fitx() << ", " 
                        << trigPartXMuMu->fity() << ", " << trigPartXMuMu->fitz() << ")" << endreq;
                } // end KMuMu chi2 cut
            } // end if(bPlusVxCandidate)
            delete bPlusVxCandidate;
        }
        else { // no vertexing
            //       trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BMUMUX, massKMuMu);
            trigPartXMuMu = new TrigEFBphys( 0, 0., 0., TrigEFBphys::BKMUMU, massKMuMu);
            
            // Fill BMuMuK monitoring containers
            mon_BMuMuK_Pt_K.push_back(track1->perigeeParameters()->pT()/1000.);
            mon_BMuMuK_Eta_K.push_back(track1->perigeeParameters()->eta());
            mon_BMuMuK_Phi_K.push_back(track1->perigeeParameters()->parameters()[Trk::phi0]);
            mon_BMuMuK_InvMass_B.push_back(massKMuMu/1000.);
            //       mon_BMuMuK_VtxMass_B.push_back(vtxMassKMuMu/1000.);
            //       mon_BMuMuK_Chi2_B.push_back(chi2KMuMu);
            
            if(msgLvl() <= MSG::DEBUG) 
                msg() << MSG::DEBUG << " Good B+ -> mu mu K+ found (no vertexing)!" << std::endl
                << "  m = " << trigPartXMuMu->mass() << endreq;
        }
        
    } // end if KMuMu mass cut
    
    return trigPartXMuMu;
}

/*----------------------------------------------------------------------------*/
void TrigEFBMuMuXFex::addUnique(std::vector<const Trk::Track*>& tracks, const Trk::Track* trkIn)
{
    //  std::cout<<" in addUnique : trkIn pT= "<<trkIn->perigeeParameters()->pT()<<std::endl;
    std::vector<const Trk::Track*>::iterator tItr;
    for( tItr = tracks.begin(); tItr != tracks.end(); tItr++)
    {
        double dPhi=fabs((*tItr)->perigeeParameters()->parameters()[Trk::phi] -
                         trkIn->perigeeParameters()->parameters()[Trk::phi]);
        if (dPhi > M_PI) dPhi = 2.*M_PI - dPhi;
        
        if( fabs(dPhi) < 0.02 &&
           fabs((*tItr)->perigeeParameters()->eta() -
                trkIn->perigeeParameters()->eta()) < 0.02 ) 
        { //std::cout<<" TrigEFBMuMuFex addUnique: the SAME tracks! pT= "<<
            //trkIn->perigeeParameters()->pT()<<" and "<<
            //(*tItr)->perigeeParameters()->pT()<<std::endl;
            return;
        }
    } 
    tracks.push_back(trkIn);       
}


/*----------------------------------------------------------------------------*/
double TrigEFBMuMuXFex::XMass(const Trk::Track* particle1, const Trk::Track* particle2, int decay)
{
    std::vector<double> massHypo;
    massHypo.clear();
    if(decay == di_to_muons){
        massHypo.push_back(MUMASS);
        massHypo.push_back(MUMASS);
    }
    if(decay == bD_to_Kstar){  
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(PIMASS);
    }
    if(decay == bS_to_Phi){  
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(KPLUSMASS);    
    }
    if(decay == lB_to_L){  
        massHypo.push_back(PROTONMASS);
        massHypo.push_back(PIMASS);    
    }
    std::vector<const Trk::Track*> bTracks;
    bTracks.clear();
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    return InvMass(bTracks, massHypo);                                            
}


/*----------------------------------------------------------------------------*/
double TrigEFBMuMuXFex::X3Mass(const Trk::Track* particle1, const Trk::Track* particle2, const Trk::Track* particle3)
{
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(KPLUSMASS);    
    massHypo.push_back(PIMASS);    
    std::vector<const Trk::Track*> bTracks;
    bTracks.clear();
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);
    bTracks.push_back(particle3);
    return InvMass(bTracks, massHypo);                                            
}


/*----------------------------------------------------------------------------*/
double TrigEFBMuMuXFex::KMuMuMass(const Trk::Track* mu1, const Trk::Track* mu2,
                                       const Trk::Track* kaon)
{
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(MUMASS);
    massHypo.push_back(MUMASS);
    massHypo.push_back(KPLUSMASS);  //K
    std::vector<const Trk::Track*> bTracks;
    bTracks.clear();
    bTracks.push_back(mu1);
    bTracks.push_back(mu2);
    bTracks.push_back(kaon);
    return InvMass(bTracks, massHypo);                                            
}


/*----------------------------------------------------------------------------*/
double TrigEFBMuMuXFex::XMuMuMass(const Trk::Track* mu1, const Trk::Track* mu2,
                                       const Trk::Track* particle1, const Trk::Track* particle2, int decay)
{
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(MUMASS);
    massHypo.push_back(MUMASS);
    if(decay == bD_to_Kstar){  
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(PIMASS);
    }
    if(decay == bS_to_Phi){  
        massHypo.push_back(KPLUSMASS);
        massHypo.push_back(KPLUSMASS);
    }
    if(decay == lB_to_L){  
        massHypo.push_back(PROTONMASS);
        massHypo.push_back(PIMASS);    
    }  
    std::vector<const Trk::Track*> bTracks;
    bTracks.clear();
    bTracks.push_back(mu1);
    bTracks.push_back(mu2);
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);  
    return InvMass(bTracks, massHypo);
}

/*----------------------------------------------------------------------------*/
double TrigEFBMuMuXFex::X3MuMuMass(const Trk::Track* mu1, const Trk::Track* mu2,
                                        const Trk::Track* particle1, const Trk::Track* particle2, const Trk::Track* particle3)
{
    std::vector<double> massHypo;
    massHypo.clear();
    massHypo.push_back(MUMASS);
    massHypo.push_back(MUMASS);
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(KPLUSMASS);
    massHypo.push_back(PIMASS);    
    std::vector<const Trk::Track*> bTracks;
    bTracks.clear();
    bTracks.push_back(mu1);
    bTracks.push_back(mu2);
    bTracks.push_back(particle1);
    bTracks.push_back(particle2);  
    bTracks.push_back(particle3);  
    return InvMass(bTracks, massHypo);
}
