/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define TPhotonMCShifterTool_cxx

#include "ElectronPhotonShowerShapeFudgeTool/TPhotonMCShifterTool.h"
#include "PathResolver/PathResolver.h"
#include "TMath.h"
#include "TFile.h"
#include <iostream>
#include <memory>
#include <stdexcept>

static const double GeV = 1.e+3;

using namespace IDVAR;

void TPhotonMCShifterTool::FudgeShowers( double  pt     ,
					 double  eta2   ,
					 double& rhad1  ,
					 double& rhad   ,
					 double& e277   ,
					 double& reta   ,
					 double& rphi   ,
					 double& weta2  ,
					 double& f1     ,
					 double& fside  ,
					 double& wtot   ,
					 double& w1     ,
					 double& deltae ,
					 double& eratio ,
					 int     isConv   ,
					 int     preselection)
{
  // only load new FF histograms if the tune changes
  if (preselection<0) preselection = m_preselection;
  if (preselection>0 && preselection!=m_preselection) {
    m_preselection = preselection;
    this->LoadFFs(preselection);
  }

  //fudge showers
  rhad1  = Fudge_Rhad1(rhad1, pt, eta2, isConv);
  rhad   = Fudge_Rhad(rhad, pt, eta2, isConv);
  e277   = Fudge_E277(e277, pt, eta2, isConv);
  reta   = Fudge_Reta(reta, pt, eta2, isConv);
  rphi   = Fudge_Rphi(rphi, pt, eta2, isConv);
  weta2  = Fudge_Weta2(weta2, pt, eta2, isConv);
  f1     = Fudge_F1(f1, pt, eta2, isConv);
  deltae = Fudge_DE(deltae, pt, eta2, isConv);
  wtot   = Fudge_Wtot(wtot, pt, eta2, isConv);
  fside  = Fudge_Fside(fside, pt, eta2, isConv);
  w1     = Fudge_W1(w1, pt, eta2, isConv);
  eratio = Fudge_Eratio(eratio, pt, eta2, isConv);
}


void TPhotonMCShifterTool::FudgeShowers( float  pt     ,
					 float  eta2   ,
					 float& rhad1  ,
					 float& rhad   ,
					 float& e277   ,
					 float& reta   ,
					 float& rphi   ,
					 float& weta2  ,
					 float& f1     ,
					 float& fside  ,
					 float& wtot   ,
					 float& w1     ,
					 float& deltae ,
					 float& eratio ,
					 int     isConv   ,
					 int     preselection)
{
  // only load new FF histograms if the tune changes
  if (preselection<0) preselection = m_preselection;
  if (preselection>0 && preselection!=m_preselection) {
    m_preselection = preselection;
    this->LoadFFs(preselection);
  }

  //fudge showers
  rhad1  = Fudge_Rhad1(rhad1, pt, eta2, isConv);
  rhad   = Fudge_Rhad(rhad, pt, eta2, isConv);
  e277   = Fudge_E277(e277, pt, eta2, isConv);
  reta   = Fudge_Reta(reta, pt, eta2, isConv);
  rphi   = Fudge_Rphi(rphi, pt, eta2, isConv);
  weta2  = Fudge_Weta2(weta2, pt, eta2, isConv);
  f1     = Fudge_F1(f1, pt, eta2, isConv);
  deltae = Fudge_DE(deltae, pt, eta2, isConv);
  wtot   = Fudge_Wtot(wtot, pt, eta2, isConv);
  fside  = Fudge_Fside(fside, pt, eta2, isConv);
  w1     = Fudge_W1(w1, pt, eta2, isConv);
  eratio = Fudge_Eratio(eratio, pt, eta2, isConv);
}

void TPhotonMCShifterTool::FudgeShowers(  std::vector<float> clE,
					  std::vector<float> eta2   ,
					  std::vector<float>& rhad1  ,
					  std::vector<float>& rhad   ,
					  std::vector<float>& e277   ,
					  std::vector<float>& reta   ,
					  std::vector<float>& rphi   ,
					  std::vector<float>& weta2  ,
					  std::vector<float>& f1     ,
					  std::vector<float>& fside  ,
					  std::vector<float>& wtot   ,
					  std::vector<float>& w1     ,
					  std::vector<float>& deltae ,
					  std::vector<float>& eratio ,
					  std::vector<int> isConv  ,
					  int    preselection)
{
  // Assume all vectors have the same size
  for (unsigned int i = 0; i < clE.size(); ++i)
    FudgeShowers(clE[i]/TMath::CosH(eta2[i]),
		 eta2  [i],
		 rhad1 [i],
		 rhad  [i],
		 e277  [i],
		 reta  [i],
		 rphi  [i],
		 weta2 [i],
		 f1    [i],
		 fside [i],
		 wtot  [i],
		 w1    [i],
		 deltae[i] ,
		 eratio[i] ,
		 isConv[i],
		 preselection);
}


void TPhotonMCShifterTool::LoadFFs(int preselection)
{
  if (preselection == m_preselection) return;
  m_preselection = preselection;

  static const std::string FILE_NAME =
     "ElectronPhotonShowerShapeFudgeTool/PhotonFudgeFactors.root";
  const std::string filePath = PathResolverFindCalibFile( FILE_NAME );
  std::unique_ptr< TFile > f( TFile::Open( filePath.c_str(), "READ" ) );
  if( ( ! f.get() ) || f->IsZombie() ) {
     throw std::runtime_error( "Couldn't open file: " + FILE_NAME );
  }
  if (!f->FindKey(Form("TUNE%d",preselection))) {
    std::cout << "Directory TUNE" << preselection << " does not exist in fudge factor file. Aborting" << std::endl;
    exit(-1);
  }
  
  h_u_rhad1 = (TH2D*) f->Get(Form("TUNE%d/FF_RHAD1_UNCONV",preselection));
  h_u_rhad1->SetDirectory(0);
 
  h_u_rhad = (TH2D*) f->Get(Form("TUNE%d/FF_RHAD_UNCONV",preselection));
  h_u_rhad->SetDirectory(0);
    
  h_u_e277 = (TH2D*) f->Get(Form("TUNE%d/FF_E277_UNCONV",preselection));
  h_u_e277->SetDirectory(0);
    
  h_u_reta = (TH2D*) f->Get(Form("TUNE%d/FF_RETA_UNCONV",preselection));
  h_u_reta->SetDirectory(0);
    
  h_u_rphi = (TH2D*) f->Get(Form("TUNE%d/FF_RPHI_UNCONV",preselection));
  h_u_rphi->SetDirectory(0);
    
  h_u_weta2 =(TH2D*) f->Get(Form("TUNE%d/FF_WETA2_UNCONV",preselection));
  h_u_weta2->SetDirectory(0);
    
  h_u_f1 = (TH2D*) f->Get(Form("TUNE%d/FF_F1_UNCONV",preselection));
  h_u_f1->SetDirectory(0);
    
  h_u_fside = (TH2D*) f->Get(Form("TUNE%d/FF_FSIDE_UNCONV",preselection));
  h_u_fside->SetDirectory(0);
    
  h_u_wtot = (TH2D*) f->Get(Form("TUNE%d/FF_WTOT_UNCONV",preselection));
  h_u_wtot->SetDirectory(0);
    
  h_u_w1 = (TH2D*) f->Get(Form("TUNE%d/FF_W1_UNCONV",preselection));
  h_u_w1->SetDirectory(0);
    
  h_u_de = (TH2D*) f->Get(Form("TUNE%d/FF_DE_UNCONV",preselection));
  h_u_de->SetDirectory(0);
    
  h_u_eratio = (TH2D*) f->Get(Form("TUNE%d/FF_ERATIO_UNCONV",preselection));
  h_u_eratio->SetDirectory(0);

  h_c_rhad1 = (TH2D*) f->Get(Form("TUNE%d/FF_RHAD1_CONV",preselection));
  h_c_rhad1->SetDirectory(0);
    
  h_c_rhad = (TH2D*) f->Get(Form("TUNE%d/FF_RHAD_CONV",preselection));
  h_c_rhad->SetDirectory(0);
    
  h_c_e277 = (TH2D*) f->Get(Form("TUNE%d/FF_E277_CONV",preselection));
  h_c_e277->SetDirectory(0);
    
  h_c_reta = (TH2D*) f->Get(Form("TUNE%d/FF_RETA_CONV",preselection));
  h_c_reta->SetDirectory(0);
    
  h_c_rphi = (TH2D*) f->Get(Form("TUNE%d/FF_RPHI_CONV",preselection));
  h_c_rphi->SetDirectory(0);
    
  h_c_weta2 = (TH2D*) f->Get(Form("TUNE%d/FF_WETA2_CONV",preselection));
  h_c_weta2->SetDirectory(0);
    
  h_c_f1 = (TH2D*) f->Get(Form("TUNE%d/FF_F1_CONV",preselection));
  h_c_f1->SetDirectory(0);
    
  h_c_fside = (TH2D*) f->Get(Form("TUNE%d/FF_FSIDE_CONV",preselection));
  h_c_fside->SetDirectory(0);
    
  h_c_wtot = (TH2D*) f->Get(Form("TUNE%d/FF_WTOT_CONV",preselection));
  h_c_wtot->SetDirectory(0);
    
  h_c_w1 = (TH2D*) f->Get(Form("TUNE%d/FF_W1_CONV",preselection));
  h_c_w1->SetDirectory(0);
    
  h_c_de = (TH2D*) f->Get(Form("TUNE%d/FF_DE_CONV",preselection));
  h_c_de->SetDirectory(0);
    
  h_c_eratio = (TH2D*) f->Get(Form("TUNE%d/FF_ERATIO_CONV",preselection));
  h_c_eratio->SetDirectory(0);

  f->Close();
}

/*
TH2D* TPhotonMCShifterTool::GetFFTH2D(int var, int isConv, int preselection){

  // read the FFs
  if (preselection>=0 && preselection != m_preselection)
    this->LoadFFs(preselection);

  // create the histogram
  const int ptbins = m_rhad1_ff.GetPtBins()+1;
  double *ptarray = m_rhad1_ff.GetPtArray();
  const int etabins = m_rhad1_ff.GetEtaBins()+1;
  double *etaarray = m_rhad1_ff.GetEtaArray();
  //std::cout << ptbins << " " << etabins << std::endl;
  double x[100];
  double y[100];
  
  for (int i=0; i<ptbins; i++){
    if(i==0)
      x[0]=0.;
    else
      x[i] = ptarray[i-1];
    //std::cout << i << " " << x[i] << std::endl;
  }    
  for (int i=0; i<etabins; i++){
    if(i==0)
      y[0]=0.;
    else
      y[i] = etaarray[i-1];
    //std::cout << i << " " << y[i] << std::endl;
  }    


  
  TH2D* hff = new TH2D("hff","hff",ptbins-1,x,etabins-1,y);
  hff->GetXaxis()->SetTitle("p_{T} [MeV]");
  hff->GetYaxis()->SetTitle("|#eta_{S2}|");
  
  // fill the histogram
  double pt, eta;
  for (int i=1; i<ptbins; i++){
    pt = hff->GetXaxis()->GetBinCenter(i);
    for (int j=1; j<etabins; j++){
      eta = hff->GetYaxis()->GetBinCenter(j);      
      //std::cout << "pT = " << pt << ", eta = " << eta << std::endl;

      double ff, fferr;
      
      switch (var) {
      case IDVAR::RHAD1:
	ff    = GetFF_Rhad1( pt, eta );
	fferr = GetFFerr_Rhad1( pt, eta );
	break;
      case IDVAR::RHAD:
	ff    = GetFF_Rhad( pt, eta );
	fferr = GetFFerr_Rhad( pt, eta );
	break;
      case IDVAR::E277:
	ff    = GetFF_E277( pt, eta );
	fferr = GetFFerr_E277( pt, eta );
	break;
      case IDVAR::RETA:
	ff    = GetFF_Reta( pt, eta );
	fferr = GetFFerr_Reta( pt, eta );
	break;
      case IDVAR::RPHI:
	ff    = GetFF_Rphi( pt, eta );
	fferr = GetFFerr_Rphi( pt, eta );
	break;
      case IDVAR::WETA2:
	ff    = GetFF_Weta2( pt, eta );
	fferr = GetFFerr_Weta2( pt, eta );
	break;
      case IDVAR::F1:
	ff    = GetFF_F1( pt, eta );
	fferr = GetFFerr_F1( pt, eta );
	break;
      case IDVAR::FSIDE:
	ff    = GetFF_Fside( pt, eta );
	fferr = GetFFerr_Fside( pt, eta );
	break;
      case IDVAR::WTOT:
	ff    = GetFF_Wtot( pt, eta );
	fferr = GetFFerr_Wtot( pt, eta );
	break;
      case IDVAR::W1:
	ff    = GetFF_W1( pt, eta );
	fferr = GetFFerr_W1( pt, eta );
	break;
      case IDVAR::DE:
	ff    = GetFF_DE( pt, eta );
	fferr = GetFFerr_DE( pt, eta );
	break;
      case IDVAR::ERATIO:
	ff    = GetFF_Eratio( pt, eta );
	fferr = GetFFerr_Eratio( pt, eta );
	break;
      default:
	ff = 0.;
	fferr = 0.;
	break;
      }
      //std::cout << "FF = " << ff << " +- " << fferr << std::endl;
      hff->SetBinContent(i,j,ff);
      hff->SetBinError(i,j,fferr);
    }
  }
  
  // return the histogram
  return hff;
}
*/


//Get graph of FFs
TGraphErrors* TPhotonMCShifterTool::GetFFmap(int var, double eta, int isConv, int preselection ){
  if (preselection>=0 && preselection != m_preselection)
    this->LoadFFs(preselection);

  if (h_u_rhad1==0) return 0;
  
  const int ptbins = h_u_rhad1->GetNbinsX();

  // Strict C++ compiler in Athena does not allow variable widht arrays.
  // reserve enough space, use only first fraction to create TGraphErrors
  //double x[ptbins]; double xerr[ptbins];
  //double y[ptbins]; double yerr[ptbins];
  double x[100];	double xerr[100];
  double y[100];	double yerr[100];

  for (int i=0; i<ptbins; i++){

    x[i] = h_u_rhad1->GetXaxis()->GetBinCenter(i+1);
    xerr[i] = h_u_rhad1->GetXaxis()->GetBinWidth(i+1)/2.0;
    switch (var) {
    case IDVAR::RHAD1:
      y[i]    = GetFF_Rhad1( x[i], eta, isConv );
      yerr[i] = GetFFerr_Rhad1( x[i], eta, isConv );
      break;
    case IDVAR::RHAD:
      y[i]    = GetFF_Rhad( x[i], eta, isConv );
      yerr[i] = GetFFerr_Rhad( x[i], eta, isConv );
      break;
    case IDVAR::E277:
      y[i]    = GetFF_E277( x[i], eta, isConv );
      yerr[i] = GetFFerr_E277( x[i], eta, isConv );
      break;
    case IDVAR::RETA:
      y[i]    = GetFF_Reta( x[i], eta, isConv );
      yerr[i] = GetFFerr_Reta( x[i], eta, isConv );
      break;
    case IDVAR::RPHI:
      y[i]    = GetFF_Rphi( x[i], eta, isConv );
      yerr[i] = GetFFerr_Rphi( x[i], eta, isConv );
      break;
    case IDVAR::WETA2:
      y[i]    = GetFF_Weta2( x[i], eta, isConv );
      yerr[i] = GetFFerr_Weta2( x[i], eta, isConv );
      break;
    case IDVAR::F1:
      y[i]    = GetFF_F1( x[i], eta, isConv );
      yerr[i] = GetFFerr_F1( x[i], eta, isConv );
      break;
    case IDVAR::FSIDE:
      y[i]    = GetFF_Fside( x[i], eta, isConv );
      yerr[i] = GetFFerr_Fside( x[i], eta, isConv );
      break;
    case IDVAR::WTOT:
      y[i]    = GetFF_Wtot( x[i], eta, isConv );
      yerr[i] = GetFFerr_Wtot( x[i], eta, isConv );
      break;
    case IDVAR::W1:
      y[i]    = GetFF_W1( x[i], eta, isConv );
      yerr[i] = GetFFerr_W1( x[i], eta, isConv );
      break;
    case IDVAR::DE:
      y[i]    = GetFF_DE( x[i], eta, isConv );
      yerr[i] = GetFFerr_DE( x[i], eta, isConv );
      break;
    case IDVAR::ERATIO:
      y[i]    = GetFF_Eratio( x[i], eta, isConv );
      yerr[i] = GetFFerr_Eratio( x[i], eta, isConv );
      break;
    default:
      break;
    }
  }
  
  //rescale to GeV
  for (int i=0; i<ptbins; i++){
    x[i] = x[i]/GeV;
    xerr[i] = xerr[i]/GeV;
  }
  
  TGraphErrors* graph = new TGraphErrors(ptbins,x,y,xerr,yerr);
  graph->GetXaxis()->SetTitle("p_{T} [GeV]");
  graph->GetYaxis()->SetTitle("FF");
  
  return graph;
}

TGraphErrors* TPhotonMCShifterTool::GetFFmap_Rhad1(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::RHAD1 , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Rhad(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::RHAD , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_E277(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::E277 , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Reta(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::RETA , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Rphi(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::RPHI , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Weta2(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::WETA2 , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_F1(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::F1 , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Fside(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::FSIDE , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Wtot(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::WTOT , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_W1(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::W1 , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_DE(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::DE , eta, isConv, preselection);
}
TGraphErrors* TPhotonMCShifterTool::GetFFmap_Eratio(double eta, int isConv, int preselection ){
	return  this->GetFFmap( IDVAR::ERATIO , eta, isConv, preselection);
}

