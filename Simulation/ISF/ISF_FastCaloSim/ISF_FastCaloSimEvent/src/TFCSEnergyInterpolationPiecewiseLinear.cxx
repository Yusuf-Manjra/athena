/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSEnergyInterpolationPiecewiseLinear.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"

#ifdef __FastCaloSimStandAlone__
namespace Gaudi {
  namespace Units {
    constexpr double megaelectronvolt = 1.;
    constexpr double kiloelectronvolt = 1.e-3 * megaelectronvolt;
    constexpr double keV = kiloelectronvolt;
  }
}
#else
#include "GaudiKernel/SystemOfUnits.h"
#endif

//========================================================
//======= TFCSEnergyInterpolationPiecewiseLinear =========
//========================================================

TFCSEnergyInterpolationPiecewiseLinear::TFCSEnergyInterpolationPiecewiseLinear(const char* name, const char* title): TFCSParametrization(name, title), m_linInterpol(0, ROOT::Math::Interpolation::kLINEAR)
{
}

void TFCSEnergyInterpolationPiecewiseLinear::InitFromArrayInLogEkin(Int_t np, const Double_t logEkin[], const Double_t response[])
{
  m_linInterpol.SetData(np, logEkin, response);
  m_minXValue = *std::min_element(logEkin, logEkin + np);
  m_maxXValue = *std::max_element(logEkin, logEkin + np);
  m_nPoints = np;
}

void TFCSEnergyInterpolationPiecewiseLinear::InitFromArrayInEkin(Int_t np, const Double_t Ekin[], const Double_t response[])
{
  std::vector<Double_t> logEkin(np);
  for(int i=0; i<np; i++) logEkin[i] = TMath::Log(Ekin[i]);
  InitFromArrayInLogEkin(np, logEkin.data(), response);
}

FCSReturnCode TFCSEnergyInterpolationPiecewiseLinear::simulate(TFCSSimulationState& simulstate, const TFCSTruthState* truth, const TFCSExtrapolationState*) const
{

  const float Ekin  = truth->Ekin();
  const float Einit = OnlyScaleEnergy() ? simulstate.E() : Ekin;
  
  //catch very small values of Ekin (use 1 keV here) and fix the interpolation lookup to the 1keV value
  const float logEkin = Ekin > Gaudi::Units::keV ? TMath::Log(Ekin) : TMath::Log(Gaudi::Units::keV);
  
  float Emean;
  if(logEkin < m_minXValue){
    Emean = m_linInterpol.Eval(m_minXValue)*Einit;
  }
  else if(logEkin > m_maxXValue){
    Emean = (m_linInterpol.Eval(m_maxXValue) + m_linInterpol.Deriv(m_maxXValue) * (logEkin - m_maxXValue))*Einit;
  }
  else{
    Emean = m_linInterpol.Eval(logEkin)*Einit;
  }

  if(OnlyScaleEnergy()) ATH_MSG_DEBUG("set E="<<Emean<<" for true Ekin="<<Ekin<<" and E="<<Einit);
  else                  ATH_MSG_DEBUG("set E="<<Emean<<" for true Ekin="<<Ekin);

  //set mean energy of simulstate
  simulstate.set_E(Emean);

  return FCSSuccess;
}

void TFCSEnergyInterpolationPiecewiseLinear::Print(Option_t *option) const
{
  TString opt(option);
  bool shortprint = opt.Index("short")>=0;
  bool longprint  = msgLvl(MSG::DEBUG) || (msgLvl(MSG::INFO) && !shortprint);
  TString optprint = opt;
  optprint.ReplaceAll("short","");
  TFCSParametrization::Print(option);

  if(longprint) ATH_MSG_INFO(optprint <<(OnlyScaleEnergy()?"  E()*":"  Ekin()*")<<"linInterpol N="<<m_nPoints
                           <<" "<<m_minXValue<<"<=log(Ekin)<="<<m_maxXValue
                           <<" "<<TMath::Exp(m_minXValue)<<"<=Ekin<="<<TMath::Exp(m_maxXValue));
}


void TFCSEnergyInterpolationPiecewiseLinear::unit_test(TFCSSimulationState* simulstate, TFCSTruthState* truth, const TFCSExtrapolationState* extrapol, TGraph* grlinear)
{
  if(!simulstate) simulstate  = new TFCSSimulationState();
  if(!truth)      truth       = new TFCSTruthState();
  if(!extrapol)   extrapol    = new TFCSExtrapolationState();

  if(!grlinear) {
    const int Graph0_n=9;
    Double_t Graph0_fx1001[Graph0_n] = {1.024,
                                        2.048,
                                        4.094,
                                        8.192,
                                        16.384,
                                        32.768,
                                        65.536,
                                        131.072,
                                        262.144};
    
    for(int i=0; i<Graph0_n; ++i) Graph0_fx1001[i]*=1000;
    Double_t Graph0_fy1001[Graph0_n] = {0.6535402,
                                        0.6571529,
                                        0.6843001,
                                        0.7172835,
                                        0.7708416,
                                        0.798819,
                                        0.8187628,
                                        0.8332745,
                                        0.8443931};
    grlinear = new TGraph(Graph0_n,Graph0_fx1001,Graph0_fy1001);
  }  

  TGraph* grdraw=(TGraph*)grlinear->Clone();
  grdraw->SetMarkerColor(46);
  grdraw->SetMarkerStyle(8);

  TFCSEnergyInterpolationPiecewiseLinear test("testTFCSEnergyInterpolationPieceWiseLinear", "test TFCSEnergyInterpolationPiecewiseLinear");
  test.set_pdgid(22);
  test.set_Ekin_nominal(0.5*(grdraw->GetX()[0]+grdraw->GetX()[grdraw->GetN()-1]));
  test.set_Ekin_min(grdraw->GetX()[0]);
  test.set_Ekin_max(grdraw->GetX()[grdraw->GetN()-1]);
  test.set_eta_nominal(0.225);
  test.set_eta_min(0.2);
  test.set_eta_max(0.25);
  test.InitFromArrayInEkin(grlinear->GetN(),grlinear->GetX(),grlinear->GetY());
  //test.set_OnlyScaleEnergy();
  test.Print();

  truth->set_pdgid(22);

  TGraph* gr=new TGraph();
  gr->SetNameTitle("testTFCSEnergyInterpolationPiecewiseLogX","test TFCSEnergyInterpolationPiecewiseLinear log x-axis");
  gr->GetXaxis()->SetTitle("Ekin [MeV]");
  gr->GetYaxis()->SetTitle("<E(reco)>/Ekin(true)");

  int ip=0;
  for(float Ekin=test.Ekin_min()*0.25;Ekin<=test.Ekin_max()*4;Ekin*=1.05) {
    //Init LorentzVector for truth. For photon Ekin=E
    truth->SetPxPyPzE(Ekin, 0, 0, Ekin);
    simulstate->set_E(Ekin);
    if (test.simulate(*simulstate,truth,extrapol) != FCSSuccess) {
      return;
    }
    gr->SetPoint(ip,Ekin,simulstate->E()/Ekin);
    ++ip;
  }  

  //Drawing doesn't make sense inside athena and necessary libraries not linked by default
  #if defined(__FastCaloSimStandAlone__)
    TCanvas* c = new TCanvas(gr->GetName(),gr->GetTitle());
    gr->Draw("APL");
    grdraw->Draw("Psame");
    c->SetLogx();
  #endif
}