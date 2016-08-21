/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromClustersPUC.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Florian U. Bernlochner, Bob Kowalewski, Kenji Hamano
CREATED:  Nov 28, 2014

PURPOSE:  Pile-up fit

 ********************************************************************/
#include "TrigEFMissingET/EFMissingETFromClustersPUC.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "CxxUtils/sincosf.h"

#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"

#include <TMatrixD.h>

#include <cmath>
#include <string>
using namespace std;

EFMissingETFromClustersPUC::EFMissingETFromClustersPUC(const std::string& type, 
    const std::string& name, 
    const IInterface* parent) :
  EFMissingETBaseTool(type, name, parent)
{
  declareProperty("SaveUncalibrated", m_saveuncalibrated = false ,"save uncalibrated topo. clusters");
  declareProperty("SubtractPileup", m_subtractpileup = true ,"use fit based pileup subtraction");
  declareProperty("towerWidthInput", m_towerwidthinput = 0.7 ," ");
  declareProperty("EtaRange", m_etarange = 5.0 ,"eta cut");
  //  declareProperty("ptmin", m_ptmin = 45000 ,"tower pt threshold in MeV");
  declareProperty("aveEclusPU", m_aveecluspu = 10000.0 ,"sets scale for variance of masked regions in MeV; not to be interpreted literally as the average PU cluster energy");
  declareProperty("resE", m_rese = 15.81 ,"calo energy resoln in sqrt(MeV)");
  declareProperty("nSigma", m_nsigma = 3.2 ,"tower ET significance");
  declareProperty("varRhoScale", m_varrhoscale = 4.0 ,"adjustment factor for weighting rho errors in fit");
  
  // declare configurables
  
  _fextype = FexType::TOPO;
  
  m_methelperposition = 3;

  // Determine number of phi & eta bins, and number of towers
  m_nphibins = (TMath::TwoPi()/m_towerwidthinput/2)*2;
  m_netabins = 2* m_etarange/m_towerwidthinput;
  m_ntowers = m_nphibins*m_netabins;

  //initialization to make coverity happy:
  m_clusterstate = xAOD::CaloCluster_v1::UNCALIBRATED;
 
  
}


EFMissingETFromClustersPUC::~EFMissingETFromClustersPUC()
{
}


StatusCode EFMissingETFromClustersPUC::initialize()
{

  if(msgLvl(MSG::DEBUG)) 
    msg(MSG::DEBUG) << "called EFMissingETFromClustersPUC::initialize()" << endreq;
  
  /// timers
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) 
    msg(MSG::WARNING) << name() << ": Unable to locate TrigTimer Service" << endreq;
  
  if (m_timersvc) {
    // global time
    std::string basename=name()+".TotalTime";
    m_glob_timer = m_timersvc->addItem(basename);
  } // if timing service

  if(m_saveuncalibrated) m_clusterstate = xAOD::CaloCluster::UNCALIBRATED;
   else m_clusterstate = xAOD::CaloCluster::CALIBRATED;

  return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromClustersPUC::execute()
{
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromClustersPUC::finalize()
{
  if(msgLvl(MSG::DEBUG)) 
    msg(MSG::DEBUG) << "called EFMissingETFromClustersPUC::finalize()" << endreq;

  return StatusCode::SUCCESS;
  
}

StatusCode EFMissingETFromClustersPUC::execute(xAOD::TrigMissingET * /* met */ ,
    TrigEFMissingEtHelper *metHelper ,
    const xAOD::CaloClusterContainer *caloCluster, const xAOD::JetContainer * /* jets */)
{

  if(msgLvl(MSG::DEBUG)) 
    msg(MSG::DEBUG) << "called EFMissingETFromClustersPUC::execute()" << endreq;
  
  if(m_timersvc)
    m_glob_timer->start(); // total time

  /// fetching the topo. cluster component
  TrigEFMissingEtComponent* metComp = 0;
  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Cluster component
  if (metComp==0) {
    msg(MSG::ERROR) << "cannot fetch Topo. cluster component!" << endreq;
    return StatusCode::FAILURE;
  }
  if(string(metComp->m_name).substr(0,2)!="TC"){
    msg(MSG::ERROR) << "fetched " << metComp->m_name
	     << " instead of the Clusters component!" << endreq;
    return StatusCode::FAILURE;
  }

  if(msgLvl(MSG::DEBUG)) 
    msg(MSG::DEBUG) << "fetched metHelper component \"" << metComp->m_name << "\"" << endreq;
  

  if ( (metComp->m_status & m_maskProcessed)==0 ){ // not yet processed
    metComp->Reset();  // reset component...
  } else { // skip if processed
    if(m_timersvc) m_glob_timer->stop(); // total time
    return StatusCode::SUCCESS;
  }

  // set status to Processing
  metComp->m_status |= m_maskProcessing;
  
  metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition ); // fetch Cluster component

  if (metComp==0) {  msg(MSG::ERROR) << "cannot fetch Topo. cluster component!" << endreq;  return StatusCode::FAILURE; }
  if(string(metComp->m_name).substr(0,2)!="TC"){ msg(MSG::ERROR) << "fetched " << metComp->m_name << " instead of the Clusters component!" << endreq; return StatusCode::FAILURE; }

  // Variables
  double MExEta = 0, MEyEta = 0, MEzEta = 0, MExFull = 0, MEyFull = 0, MEzFull = 0, METEta = 0, MET = 0;

  double sumEtEta = 0, sumEtFull = 0, sumEEta = 0, sumEFull = 0;
  vector<double> ExTower(m_ntowers,0), EyTower(m_ntowers,0), EtTower(m_ntowers,0);
  vector<double> ExTower1(m_ntowers,0), EyTower1(m_ntowers,0), EtTower1(m_ntowers,0);
  vector<double> ExTower2(m_ntowers,0), EyTower2(m_ntowers,0), EtTower2(m_ntowers,0);
  vector<double> ExTower3(m_ntowers,0), EyTower3(m_ntowers,0), EtTower3(m_ntowers,0);
  vector<double> ExInMask, EyInMask, EtInMask, AreaInMask;
  vector<int> indexOfMask;

  // Calculate initial energy
  for (xAOD::CaloClusterContainer::const_iterator it = caloCluster->begin(); it != caloCluster->end(); ++it ) {
      float phi = (*it)->phi(m_clusterstate), eta = (*it)->eta(m_clusterstate), Et  = (*it)->pt(m_clusterstate), 
            E = (*it)->p4(m_clusterstate).E();
      float cosPhi, sinPhi; sincosf(phi, &sinPhi, &cosPhi);
      float Ex = Et*cosPhi, Ey = Et*sinPhi, Ez = Et*sinhf(eta);
      // calculate full granularity Ex, Ey
      MExFull += Ex; MEyFull += Ey; MEzFull += Ez; sumEtFull += Et; sumEFull += E;
      // collect clusters into fixed-position towers    
      if(eta > (-1) * m_etarange && eta < m_etarange) { // eta cut
        MExEta += Ex; MEyEta += Ey; MEzEta += Ez; sumEtEta += Et; sumEEta += E;
       int binEta = (eta + m_etarange)/(2*m_etarange)*m_netabins, 
           binPhi = (fmod(phi+TMath::TwoPi(),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins,
           index = binEta*m_nphibins + binPhi;
       int binEta1 = (fmod(eta + m_etarange*(1+0.5/m_netabins),2*m_etarange))/(2*m_etarange)*m_netabins, 
	   binPhi1 = (fmod(phi+TMath::TwoPi()*(1+0.5/m_nphibins),TMath::TwoPi()) / TMath::TwoPi())*m_nphibins,
           index1 = binEta1*m_nphibins + binPhi,
           index2 = binEta *m_nphibins + binPhi1,
           index3 = binEta1*m_nphibins + binPhi1;
       ExTower[index] += Ex; EyTower[index] += Ey; EtTower[index] += Et;
       ExTower1[index1] += Ex; EyTower1[index1] += Ey; EtTower1[index1] += Et;
       ExTower2[index2] += Ex; EyTower2[index2] += Ey; EtTower2[index2] += Et;
       ExTower3[index3] += Ex; EyTower3[index3] += Ey; EtTower3[index3] += Et;
      }
   } // end topo. loop.   
  double varEtOneTowerNow = m_aveecluspu*sumEtEta/m_ntowers * 2;
  double threshEtOneTowerNow = sumEtEta/m_ntowers + m_nsigma*sqrt(varEtOneTowerNow);

  // find binning that gives largest sumEt from bins above threshold
  std::vector<double> EtMaxSumT(4,0);
  std::vector<int> aboveNowT(4,0);
  for(uint k=0; k<ExTower.size(); k++) {
    if ( EtTower[k]   > threshEtOneTowerNow ) { EtMaxSumT[0] += EtTower[k];  aboveNowT[0]++; }
    if ( EtTower1[k]  > threshEtOneTowerNow ) { EtMaxSumT[1] += EtTower1[k]; aboveNowT[1]++; }
    if ( EtTower2[k]  > threshEtOneTowerNow ) { EtMaxSumT[2] += EtTower2[k]; aboveNowT[2]++; }
    if ( EtTower3[k]  > threshEtOneTowerNow ) { EtMaxSumT[3] += EtTower3[k]; aboveNowT[3]++; }
  }
  std::vector<double>::iterator itmax;
  itmax = std::max_element(EtMaxSumT.begin(), EtMaxSumT.end());
  int iTmax = std::distance(EtMaxSumT.begin(), itmax);
  int aboveNow = aboveNowT[iTmax];
  double EtMaxSumNow = EtMaxSumT[iTmax];
  // select Tower bins to correspond to iTmax
  if (iTmax == 1) {
    EtTower = EtTower1; ExTower = ExTower1; EyTower = EyTower1;
  } else if (iTmax == 2) {
    EtTower = EtTower2; ExTower = ExTower2; EyTower = EyTower2;
  } else if (iTmax == 3) {
    EtTower = EtTower3; ExTower = ExTower3; EyTower = EyTower3;
  }

  // remove current bins above ptmin from sumEtEta and recalculate
  // threshold to reduce sensitivity to high_pt
  double varEtOneTower = m_aveecluspu*(sumEtEta-EtMaxSumNow)/(m_ntowers-aboveNow) * 2;
  double threshEtOneTower = (sumEtEta-EtMaxSumNow)/(m_ntowers-aboveNow) + m_nsigma*sqrt(varEtOneTower);
  
  // Missing transverse energy from fixed position tower
  METEta = sqrt(MExEta*MExEta + MEyEta*MEyEta); MET = sqrt(MExFull*MExFull + MEyFull*MEyFull);

  // Perform pile-up fit and subtract contributions
  // --------------------------------------------------------------------------------------
  if (m_subtractpileup) {

   for(unsigned int k=0; k<ExTower.size(); k++)
     //     if (EtTower[k] > m_ptmin) {
     if (EtTower[k] > threshEtOneTower) {
        ExInMask.push_back(ExTower[k]); EyInMask.push_back(EyTower[k]); EtInMask.push_back(EtTower[k]);
        AreaInMask.push_back(2*m_etarange/m_netabins*TMath::TwoPi()/m_nphibins);
     }

   int nummasks = ExInMask.size();
   if(nummasks > 0) {
    // Form sumEtobs and covEtobs from all towers
    double sumEtobs = 0, varsumEtobs = 0;
    TMatrixD Etobs(2,1), covEtobs(2,2);
    for(unsigned int j = 0; j < ExTower.size(); j++) {
      double E1 = sqrt(ExTower[j]*ExTower[j]+EyTower[j]*EyTower[j]);
      if (E1 < 1.0) continue;
      sumEtobs += E1;
      varsumEtobs += m_rese*m_rese*E1;
      double cosphi1 = ExTower[j]/E1, sinphi1 = EyTower[j]/E1;
      Etobs[0][0] += E1*cosphi1; Etobs[1][0] += E1*sinphi1;
      covEtobs[0][0] += m_rese*m_rese*E1*cosphi1*cosphi1;
      covEtobs[1][0] += m_rese*m_rese*E1*cosphi1*sinphi1;
      covEtobs[1][1] += m_rese*m_rese*E1*sinphi1*sinphi1;
    }
    covEtobs[0][1] = covEtobs[1][0];
    // record masks, remove towers that are masked from observed quantities
    vector<double> Emasked; TMatrixD Etmasked(2,1);
    double sumEtmasked = 0, arealost = 0, areatot = (m_etarange*2)*2*M_PI;
    for(int k = 0; k < nummasks; k++) {
        arealost += AreaInMask[k];
        double E1 = sqrt(ExInMask[k]*ExInMask[k]+EyInMask[k]*EyInMask[k]);
        double E1res2 = E1*m_rese*m_rese;
        double cosphi1 = ExInMask[k]/E1, sinphi1 = EyInMask[k]/E1;
        Emasked.push_back(E1);  sumEtmasked += E1;
        Etmasked[0][0] += E1*cosphi1;  Etmasked[1][0] += E1*sinphi1;
        sumEtobs -= E1; varsumEtobs -= E1res2;
        Etobs[0][0] -= E1*cosphi1;  Etobs[1][0] -= E1*sinphi1;
        covEtobs[0][0] -= E1res2*cosphi1*cosphi1;
        covEtobs[1][0] -= E1res2*sinphi1*cosphi1;
        covEtobs[1][1] -= E1res2*sinphi1*sinphi1;
    }  
    covEtobs[0][1] = covEtobs[1][0];
    TMatrixD covEtobsinv(covEtobs); covEtobsinv.Invert();
    double areaobs = areatot - arealost, rhoobs = sumEtobs/areaobs;

    TMatrixD dXdEab(nummasks,nummasks), dXdEa(nummasks,1);
    vector<double> varRhoA;
    for (int k1 = 0; k1<nummasks; k1++) {
        double aratio = AreaInMask[k1]/areaobs;
	//        varRhoA.push_back((varsumEtobs*aratio*aratio + m_aveecluspu*sumEtobs*aratio * 2));
	varRhoA.push_back((varsumEtobs*aratio*aratio + m_varrhoscale*m_aveecluspu*sumEtobs*aratio * 2));
        double ET1inv = 1/EtInMask[k1];
        double cosphi1 = ExInMask[k1]*ET1inv;
        double sinphi1 = EyInMask[k1]*ET1inv;
        dXdEa[k1][0] = -(                                               \
                         Etobs[0][0]*(covEtobsinv[0][0]*cosphi1+covEtobsinv[1][0]*sinphi1) + \
                         Etobs[1][0]*(covEtobsinv[0][1]*cosphi1+covEtobsinv[1][1]*sinphi1) - \
                         rhoobs*AreaInMask[k1]/varRhoA[k1] );
        for (int k2 = 0; k2<nummasks; k2++) {
          double ET2inv = 1/EtInMask[k2];
          double cosphi2 = ExInMask[k2]*ET2inv;
          double sinphi2 = EyInMask[k2]*ET2inv;
          dXdEab[k1][k2] = (                                            \
                            cosphi1*(covEtobsinv[0][0]*cosphi2+covEtobsinv[1][0]*sinphi2) + \
                            sinphi1*(covEtobsinv[0][1]*cosphi2+covEtobsinv[1][1]*sinphi2) );
          if (k1 == k2) dXdEab[k1][k2] += 1/varRhoA[k1];
        }
     }
     TMatrixD covFit(dXdEab); covFit.Invert(); TMatrixD Evals(covFit*dXdEa);
     TMatrixD ETobscor(Etmasked), ETfitcor(Etmasked);
     //     double evalsx = 0, evalsy = 0;
     for (int k=0; k<nummasks; k++) {
       double cosphi1 = ExInMask[k]/EtInMask[k], sinphi1 = EyInMask[k]/EtInMask[k];
        ETfitcor[0][0] -= Evals[k][0]*cosphi1;
        ETfitcor[1][0] -= Evals[k][0]*sinphi1;
	//	evalsx += Evals[k][0]*cosphi1; evalsy += Evals[k][0]*sinphi1;
      }
      double METfitcor = sqrt(ETfitcor[0][0]*ETfitcor[0][0]+ETfitcor[1][0]*ETfitcor[1][0]);
      // // calculate chi2
      // chi2 += covEtobsinv[0][0]*(Etobs[0][0]+evalsx)*(Etobs[0][0]+evalsx)
      // 	    + covEtobsinv[1][0]*(Etobs[0][0]+evalsx)*(Etobs[1][0]+evalsy)
      // 	    + covEtobsinv[0][1]*(Etobs[1][0]+evalsy)*(Etobs[0][0]+evalsx)
      // 	    + covEtobsinv[1][1]*(Etobs[1][0]+evalsy)*(Etobs[1][0]+evalsy);
      // for (int k=0; k<nummasks; k++) {
      // 	chi2 += (AreaInMask[k]*rhoobs-Evals[k][0])*(AreaInMask[k]*rhoobs-Evals[k][0])/varRhoA[k];
      // }

      // //calculate significance of MET in high-pt regions (relative to MET=0)
      // TMatrixD dMETdEval(nummasks,1);  TMatrixD dMETdphi(nummasks,1);
      // for (int k=0; k<nummasks; k++) {
      // 	double cosphi1 = ExInMask[k]/EtInMask[k];  double sinphi1 = EyInMask[k]/EtInMask[k];
      // 	dMETdEval[k][0] = 1/METfitcor*( 
      // 	  ETfitcor[0][0]*(-cosphi1) + ETfitcor[1][0]*(-sinphi1) );
      // 	dMETdphi[k][0] = 1/METfitcor*(					
      // 	  ETfitcor[0][0]*(+Evals[k][0]*sinphi1) + ETfitcor[1][0]*(-Evals[k][0]*cosphi1) );
      // }      
      // double varMET = ((covFit*dMETdEval).T()*dMETdEval)[0][0];
      // double varPhi = TMath::TwoPi()/nPhiBins*TMath::TwoPi()/nPhiBins/12;
      // for (int k=0; k<nummasks; k++) { varMET += dMETdphi[k][0]*dMETdphi[k][0]*varPhi; }
      // signif = sqrt(METfitcor*METfitcor/varMET);

      msg() << MSG::DEBUG << " METEta = " << METEta << "\t METfitcor = " << METfitcor << "\t MET= " << MET << endreq;

      ETobscor[0][0]=MExEta; ETobscor[1][0]=MEyEta;

      metComp->m_ex = -(float) ETfitcor[0][0];
      metComp->m_ey = -(float) ETfitcor[1][0];  
      // For the rest we add in the default quantities 
      metComp->m_ez = -MEzEta;
      metComp->m_sumEt = sumEtEta;
      metComp->m_sumE  = sumEEta;
      metComp->m_usedChannels += 1;

      metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition + 1 ); // fetch first auxiliary component to store uncorrected MET
    
      metComp->m_ex = -(float) ETobscor[0][0];
      metComp->m_ey = -(float) ETobscor[1][0];  
      // For the rest we add in the default quantities 
      metComp->m_ez = -MEzEta;
      metComp->m_sumEt = sumEtEta;
      metComp->m_sumE  = sumEEta;
      metComp->m_usedChannels += 1;     

  } else {
  
      // Just store zero energies for the clusters 
      metComp->m_ex = 0.;
      metComp->m_ey = 0.;
      metComp->m_ey = 0.;
      metComp->m_sumEt = 0.;
      metComp->m_sumE  = 0.;
      metComp->m_usedChannels += 1;
      
      metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition + 1 ); // fetch first auxiliary component to store uncorrected MET
      
      metComp->m_ex = -MExEta;
      metComp->m_ey = -MEyEta;
      metComp->m_ez = -MEzEta;
      metComp->m_sumEt = sumEtEta;
      metComp->m_sumE  = sumEEta;
      metComp->m_usedChannels += 1;

  }
  
  // --------------------------------------------------------------------------------------

  // move from "processing" to "processed" state
  metComp->m_status ^= m_maskProcessing; // switch off bit
  metComp->m_status |= m_maskProcessed;  // switch on bit

 } // end container loop.

  if(m_timersvc)
    m_glob_timer->stop(); // total time

  return StatusCode::SUCCESS;

}
