/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/CalibrationMethods/InsituDataCorrection.h"
#include "PathResolver/PathResolver.h"

InsituDataCorrection::InsituDataCorrection()
  : JetCalibrationToolBase::JetCalibrationToolBase("InsituDataCorrection::InsituDataCorrection"),
    m_config(nullptr),
    m_jetAlgo(""),
    m_calibAreaTag(""),
    m_dev(false),
    m_insituCorr(nullptr),
    m_insituCorr_JMS(nullptr),
    m_insituCorr_ResidualMCbased(nullptr)
{ }

InsituDataCorrection::InsituDataCorrection(const std::string& name)
  : JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(nullptr),
    m_jetAlgo(""),
    m_calibAreaTag(""),
    m_dev(false),
    m_insituCorr(nullptr),
    m_insituCorr_JMS(nullptr),
    m_insituCorr_ResidualMCbased(nullptr)
{ }

InsituDataCorrection::InsituDataCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool dev)
  : JetCalibrationToolBase::JetCalibrationToolBase( name ),
    m_config(config),
    m_jetAlgo(jetAlgo),
    m_calibAreaTag(calibAreaTag),
    m_dev(dev),
    m_insituCorr(nullptr),
    m_insituCorr_JMS(nullptr),
    m_insituCorr_ResidualMCbased(nullptr)
{ }

InsituDataCorrection::~InsituDataCorrection() {

}

StatusCode InsituDataCorrection::initializeTool(const std::string&) {

  m_jetStartScale = m_config->GetValue("InsituStartingScale","JetGSCScaleMomentum");

  //Retrieve the name of root file containing the in-situ calibration histograms from the config
  TString insitu_filename = m_config->GetValue("InsituCalibrationFile","None");
  //Should be the Relative and Abosolute Insitu correction applied?
  m_applyRelativeandAbsoluteInsitu = m_config->GetValue("ApplyRelativeandAbsoluteInsitu", true);
  //Should be a Eta restriction in the Relative and Absolute Insitu correction?
  m_applyEtaRestrictionRelativeandAbsolute = m_config->GetValue("ApplyEtaRestrictionRelativeandAbsolute", false);
  //Should be applied the Residual MC based Insitu correction?
  m_applyResidualMCbasedInsitu = m_config->GetValue("ApplyResidualMCbasedInsitu", false);
  //Should be a Eta restriction in the Residual MC based Insitu correction?
  m_applyEtaRestrictionResidualMCbased = m_config->GetValue("ApplyEtaRestrictionResidualMCbased", false);
  //Retrieve the name of the histogram for the relative in-situ calibration
  TString rel_histoname = m_config->GetValue("RelativeInsituCalibrationHistogram","");
  //Retrieve the name of the histogram for the absolute in-situ calibration 
  TString abs_histoname = m_config->GetValue("AbsoluteInsituCalibrationHistogram","");
  //Retrieve the name of the histogram for the absolute in-situ calibration 
  TString residualmcbased_histoname = m_config->GetValue("ResidualMCbasedInsituCalibrationHistogram","");
  //Retrieve the description/name of the in-situ calibration
  TString insitu_desc = m_config->GetValue("InsituCalibrationDescription","");
  //Retrieve the Eta restriction on the Residual MC based insitu calibration
  double insitu_etarestriction_residualmcbased = m_config->GetValue("InsituEtaRestrictionResidualMCbased",0.8);
  //Retrieve the Eta restriction on the Relative and Absolute insitu calibration
  double insitu_etarestriction_relativeandabsolute = m_config->GetValue("InsituEtaRestrictionRelativeandAbsolute",0.8);
  // Apply Insitu only to calo and TA mass calibrated jets (only for large jets)
  m_applyInsituCaloTAjets = m_config->GetValue("ApplyInsituCaloTAJets", false);
  // Apply in situ JMS:
  m_applyInsituJMS = m_config->GetValue("ApplyInsituJMS", false);

  //Find the absolute path to the insitu root file
  if ( !insitu_filename.EqualTo("None") ){
    if(m_dev){
      insitu_filename.Remove(0,32);
      insitu_filename.Insert(0,"JetCalibTools/");
    }
    else{insitu_filename.Insert(14,m_calibAreaTag);}
    insitu_filename=PathResolverFindCalibFile(insitu_filename.Data());
  }

  TFile *insitu_file = TFile::Open(insitu_filename);
  if ( !insitu_file ) { ATH_MSG_FATAL( "Cannot open InsituCalibrationFile: " << insitu_filename ); return StatusCode::FAILURE; }

  ATH_MSG_INFO("Reading In-situ correction factors from: " << insitu_filename);

  rel_histoname.ReplaceAll("JETALGO",m_jetAlgo); abs_histoname.ReplaceAll("JETALGO",m_jetAlgo);
  if(m_applyRelativeandAbsoluteInsitu){
    TH2D * rel_histo = (TH2D*)JetCalibUtils::GetHisto2(insitu_file,rel_histoname);
    TH1D * abs_histo = (TH1D*)JetCalibUtils::GetHisto(insitu_file,abs_histoname);
    if ( !rel_histo || !abs_histo ) {
      ATH_MSG_FATAL( "\n  Tool configured for data, but no residual in-situ histograms could be retrieved. Aborting..." );
      return StatusCode::FAILURE;
    }
    else {
      gROOT->cd();
      // save pTmax of the relative and absolute in situ calibrations
      m_relhistoPtMax = rel_histo->GetXaxis()->GetBinLowEdge(rel_histo->GetNbinsX()+1);
      m_abshistoPtMax = abs_histo->GetBinLowEdge(abs_histo->GetNbinsX()+1);
      // combine in situ calibrations
      m_insituCorr = std::unique_ptr<TH2D>(combineCalibration(rel_histo,abs_histo));
      m_insituEtaMax = m_insituCorr->GetYaxis()->GetBinLowEdge(m_insituCorr->GetNbinsY()+1);
      m_insituPtMin = m_insituCorr->GetXaxis()->GetBinLowEdge(1);
      m_insituPtMax = m_insituCorr->GetXaxis()->GetBinLowEdge(m_insituCorr->GetNbinsX()+1);
    }
    if(m_applyEtaRestrictionRelativeandAbsolute) m_insituEtaMax = insitu_etarestriction_relativeandabsolute;
  }
  if(m_applyResidualMCbasedInsitu){
    m_insituCorr_ResidualMCbased = std::unique_ptr<TH2D>((TH2D*)JetCalibUtils::GetHisto2(insitu_file,residualmcbased_histoname));
    if ( !m_insituCorr_ResidualMCbased ) {
      ATH_MSG_FATAL( "\n  Tool configured for the Residual MC based correction, but no residualmcbased in-situ histograms could be retrieved. Aborting..." );
      return StatusCode::FAILURE;
    }
    else{
      gROOT->cd();
      m_insituEtaMax_ResidualMCbased = m_insituCorr_ResidualMCbased->GetYaxis()->GetBinLowEdge(m_insituCorr_ResidualMCbased->GetNbinsY()+1);
      m_insituPtMin_ResidualMCbased = m_insituCorr_ResidualMCbased->GetXaxis()->GetBinLowEdge(1);
      m_insituPtMax_ResidualMCbased = m_insituCorr_ResidualMCbased->GetXaxis()->GetBinLowEdge(m_insituCorr_ResidualMCbased->GetNbinsX()+1);
    }
    if(m_applyEtaRestrictionResidualMCbased) m_insituEtaMax_ResidualMCbased = insitu_etarestriction_residualmcbased;
  }
  if(!m_applyRelativeandAbsoluteInsitu && !m_applyResidualMCbasedInsitu){
    ATH_MSG_FATAL( "\n  Tool configured for Insitu correction, but no in-situ histograms could be retrieved. Aborting..." );
    return StatusCode::FAILURE;
  }

  //Large-R in situ JMS calibration
  if(m_applyInsituJMS){
    //Retrieve the name of root files containing the in-situ calibration histograms from the config
    TString insituJMS_filename = m_config->GetValue("InsituCalibrationFile_JMS","None");
    //Retrieve the name of the histogram for the absolute in-situ calibration
    TString abs_histoname_JMS = m_config->GetValue("AbsoluteInsituCalibrationHistogram_JMS","");
    //Retrieve the eta range for the in-situ JMS calibration
    double insitu_etarestriction_JMS = m_config->GetValue("InsituEtaRestriction_JMS",2.0);

    //Find the absolute path to the insitu root file Low and High Mass
    if ( !insituJMS_filename.EqualTo("None")){
      if(m_dev){
        insituJMS_filename.Remove(0,32);
        insituJMS_filename.Insert(0,"JetCalibTools/");
      }
      else{
        insituJMS_filename.Insert(14,m_calibAreaTag);
      }
      insituJMS_filename=PathResolverFindCalibFile(insituJMS_filename.Data());
    }

    TFile *insituJMS_file = TFile::Open(insituJMS_filename);
    if ( !insituJMS_file ) { ATH_MSG_FATAL( "Cannot open InsituJMSCalibrationFile: " << insituJMS_filename ); return StatusCode::FAILURE; }

    ATH_MSG_INFO("Reading In-situ correction factors from: " << insituJMS_filename);

    abs_histoname_JMS.ReplaceAll("JETALGO",m_jetAlgo);

    if(m_applyRelativeandAbsoluteInsitu){

      TH2D * abs_histo_JMS = (TH2D*)JetCalibUtils::GetHisto2(insituJMS_file,abs_histoname_JMS);

      if ( !abs_histo_JMS ) {
        ATH_MSG_FATAL( "\n  Tool configured for data, but no in-situ JMS histogram could be retrieved. Aborting..." );
        return StatusCode::FAILURE;
      }
      else {
        gROOT->cd();
        // combine in situ calibrations
        m_insituCorr_JMS    = std::unique_ptr<TH2D>(invertHistogram(abs_histo_JMS));
	m_insituEtaMax_JMS  = insitu_etarestriction_JMS;
        m_insituPtMin_JMS   = m_insituCorr_JMS->GetXaxis()->GetBinLowEdge(1);
        m_insituPtMax_JMS   = m_insituCorr_JMS->GetXaxis()->GetBinLowEdge(m_insituCorr_JMS->GetNbinsX()+1);
        m_insituMassMin_JMS = m_insituCorr_JMS->GetYaxis()->GetBinLowEdge(1);
        m_insituMassMax_JMS = m_insituCorr_JMS->GetYaxis()->GetBinLowEdge((m_insituCorr_JMS->GetNbinsY()+1));
      }
    }
    if(!m_applyRelativeandAbsoluteInsitu){
      ATH_MSG_FATAL( "\n  Tool configured for Insitu correction, but no in-situ histograms could be retrieved. Aborting..." );
      return StatusCode::FAILURE;
    }
  }

  //insitu_file->Close();
  ATH_MSG_INFO("Tool configured to calibrate data");
  ATH_MSG_INFO("In-situ correction to be applied: " << insitu_desc);
  return StatusCode::SUCCESS;

}

StatusCode InsituDataCorrection::calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const {

  float detectorEta = jet.getAttribute<float>("DetectorEta");

  // For small R jets or large-R jets without calo-TA combination
  if(!m_applyInsituCaloTAjets){
    xAOD::JetFourMom_t jetStartP4;
    ATH_CHECK( setStartP4(jet) );
    jetStartP4 = jet.jetP4();

    xAOD::JetFourMom_t calibP4=jetStartP4;
  
    if(m_applyResidualMCbasedInsitu) calibP4=calibP4*getInsituCorr( calibP4.pt(), fabs(detectorEta), "ResidualMCbased" );

    if(m_dev){
      float insituFactor = getInsituCorr( jetStartP4.pt(), detectorEta, "RelativeAbs" );
      jet.setAttribute<float>("JetRelativeAbsInsituCalibFactor",insituFactor);
    }

    if(m_applyRelativeandAbsoluteInsitu) calibP4=calibP4*getInsituCorr( jetStartP4.pt(), detectorEta, "RelativeAbs" );

    // Only for large R jets with insitu JMS but no combination
    if(m_applyInsituJMS){
      xAOD::JetFourMom_t calibP4_JMS;
      calibP4_JMS = calibP4;

      calibP4_JMS=calibP4*getInsituCorr_JMS( calibP4.pt(), calibP4.M(), detectorEta, "RelativeAbs" );

      // pT doesn't change while applying in situ JMS
      TLorentzVector TLVjet;
      TLVjet.SetPtEtaPhiM( calibP4.pt(), calibP4.eta(), calibP4.phi(), calibP4_JMS.M() );
      calibP4.SetPxPyPzE( TLVjet.Px(), TLVjet.Py(), TLVjet.Pz(), TLVjet.E() );
    }

    //Transfer calibrated jet properties to the Jet object
    jet.setAttribute<xAOD::JetFourMom_t>("JetInsituScaleMomentum",calibP4);
    jet.setJetP4( calibP4 );
  }

  // For Large R jets: insitu needs to be apply also to calo mass calibrated jets and TA mass calibrated jets (by default it's only applied to combined mass calibrated jets)
  if(m_applyInsituCaloTAjets){

	// calo mass calibrated jets
        xAOD::JetFourMom_t jetStartP4_calo;
  	xAOD::JetFourMom_t calibP4_calo;
        if(jet.getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumCalo",jetStartP4_calo)){
          calibP4_calo=jetStartP4_calo;
        }else{
          ATH_MSG_FATAL( "Cannot retrieve JetJMSScaleMomentumCalo jets" ); 
          return StatusCode::FAILURE; 
        }

  	if(m_applyResidualMCbasedInsitu) calibP4_calo=calibP4_calo*getInsituCorr( calibP4_calo.pt(), fabs(detectorEta), "ResidualMCbased" );

	if(m_dev){
    	  float insituFactor_calo = getInsituCorr( jetStartP4_calo.pt(), detectorEta, "RelativeAbs" );
    	  jet.setAttribute<float>("JetRelativeAbsInsituCalibFactor_calo",insituFactor_calo);
 	}

        if(m_applyRelativeandAbsoluteInsitu) calibP4_calo=calibP4_calo*getInsituCorr( jetStartP4_calo.pt(), detectorEta, "RelativeAbs" );

	if(m_applyInsituJMS){
	  xAOD::JetFourMom_t calibP4_calo_JMS;
          calibP4_calo_JMS = calibP4_calo;

          calibP4_calo_JMS=calibP4_calo*getInsituCorr_JMS( calibP4_calo.pt(), calibP4_calo.M(), detectorEta, "RelativeAbs" );

	  // pT doesn't change while applying in situ JMS
	  TLorentzVector TLVjet_calo;
	  TLVjet_calo.SetPtEtaPhiM( calibP4_calo.pt(), calibP4_calo.eta(), calibP4_calo.phi(), calibP4_calo_JMS.M() );
	  calibP4_calo.SetPxPyPzE( TLVjet_calo.Px(), TLVjet_calo.Py(), TLVjet_calo.Pz(), TLVjet_calo.E() );
        }

        //Transfer calibrated jet properties to the Jet object
        jet.setAttribute<xAOD::JetFourMom_t>("JetInsituScaleMomentumCalo",calibP4_calo);
        jet.setJetP4( calibP4_calo );	

        // TA mass calibrated jets
        xAOD::JetFourMom_t jetStartP4_ta;
        xAOD::JetFourMom_t calibP4_ta;
        if(jet.getAttribute<xAOD::JetFourMom_t>("JetJMSScaleMomentumTA", jetStartP4_ta)){
          calibP4_ta=jetStartP4_ta;
        }else{
          ATH_MSG_FATAL( "Cannot retrieve JetJMSScaleMomentumTA jets" );
          return StatusCode::FAILURE;
        }

        if(m_applyResidualMCbasedInsitu) calibP4_ta=calibP4_ta*getInsituCorr( calibP4_ta.pt(), fabs(detectorEta), "ResidualMCbased" );

        if(m_dev){
          float insituFactor_ta = getInsituCorr( jetStartP4_ta.pt(), detectorEta, "RelativeAbs" );
          jet.setAttribute<float>("JetRelativeAbsInsituCalibFactor_ta",insituFactor_ta);
        }

        if(m_applyRelativeandAbsoluteInsitu) calibP4_ta=calibP4_ta*getInsituCorr( jetStartP4_ta.pt(), detectorEta, "RelativeAbs" );

        if(m_applyInsituJMS){
	  xAOD::JetFourMom_t calibP4_ta_JMS;
          calibP4_ta_JMS = calibP4_ta;

          calibP4_ta_JMS=calibP4_ta*getInsituCorr_JMS( calibP4_ta.pt(), calibP4_ta.M(), detectorEta, "RelativeAbs" );

          // pT doesn't change while applying in situ JMS
          TLorentzVector TLVjet_ta;
          TLVjet_ta.SetPtEtaPhiM( calibP4_ta.pt(), calibP4_ta.eta(), calibP4_ta.phi(), calibP4_ta_JMS.M() );
          calibP4_ta.SetPxPyPzE( TLVjet_ta.Px(), TLVjet_ta.Py(), TLVjet_ta.Pz(), TLVjet_ta.E() );
        }

        //Transfer calibrated jet properties to the Jet object
        jet.setAttribute<xAOD::JetFourMom_t>("JetInsituScaleMomentumTA",calibP4_ta);
        jet.setJetP4( calibP4_ta );

  }

  return StatusCode::SUCCESS;
}

double InsituDataCorrection::getInsituCorr(double pt, double eta, std::string calibstep) const {
  if (!m_insituCorr && !m_insituCorr_ResidualMCbased) return 1.0;
  double myEta = eta, myPt = pt/m_GeV;

  //eta and pt ranges depends on the insitu calibration
  double etaMax = m_insituEtaMax;
  double ptMin  = m_insituPtMin;
  double ptMax  = m_insituPtMax;
  if (calibstep == "ResidualMCbased"){
    etaMax = m_insituEtaMax_ResidualMCbased;
    ptMin  = m_insituPtMin_ResidualMCbased;
    ptMax  = m_insituPtMax_ResidualMCbased;
  }

  //protection against values outside the histogram range, snap back to the lowest/highest bin edge
  if ( myPt <= ptMin ) myPt = ptMin + 1e-6;
  else if ( myPt >= ptMax ) myPt = ptMax - 1e-6;
  if (calibstep == "ResidualMCbased" && m_applyEtaRestrictionResidualMCbased) {
    if(myEta>=etaMax) return 1.0;
    return m_insituCorr_ResidualMCbased->Interpolate(myPt,myEta);
  }
  if (calibstep == "RelativeAbs" && m_applyEtaRestrictionRelativeandAbsolute) {
    if(myEta>=etaMax) return 1.0;
    else if(myEta<=-etaMax) return 1.0;
  }
  if (myEta <= -etaMax) myEta = 1e-6 - etaMax;
  else if (myEta >= etaMax) myEta = etaMax - 1e-6;
  if (calibstep == "ResidualMCbased" && !m_applyEtaRestrictionResidualMCbased){
    return m_insituCorr_ResidualMCbased->Interpolate(myPt,myEta);
  }
  return m_insituCorr->Interpolate(myPt,myEta);
}

double InsituDataCorrection::getInsituCorr_JMS(double pt, double mass, double eta, std::string calibstep) const {

  if (!m_insituCorr_JMS) return 1.0;

  double myEta = eta, myPt = pt/m_GeV, myMass = mass/m_GeV;

  //eta and pt ranges depends on the insitu calibration
  double etaMax  = m_insituEtaMax_JMS;
  double ptMin   = m_insituPtMin_JMS;
  double ptMax   = m_insituPtMax_JMS;
  double massMin = m_insituMassMin_JMS;
  double massMax = m_insituMassMax_JMS;

  //protection against values outside the histogram range, snap back to the lowest/highest bin edge
  if ( myPt <= ptMin ) myPt = ptMin + 1e-6;
  else if ( myPt >= ptMax ) myPt = ptMax - 1e-6;
  if (calibstep == "RelativeAbs" && m_applyEtaRestrictionRelativeandAbsolute) {
    if(myEta>=etaMax) return 1.0;
    else if(myEta<=-etaMax) return 1.0;
  }
  if (myEta <= -etaMax) myEta = 1e-6 - etaMax;
  else if (myEta >= etaMax) myEta = etaMax - 1e-6;
  if (myMass <= massMin ) myMass = massMin + 1e-6;
  else if (myMass >= massMax ) myMass = massMax - 1e-6;
  return m_insituCorr_JMS->Interpolate(myPt,myMass);
}

TH2D * InsituDataCorrection::combineCalibration(TH2D *h2d, TH1D *h) {
  TH2D *prod = (TH2D*)h2d->Clone();
  for (int xi=1;xi<=prod->GetNbinsX();xi++) {
    double pt=prod->GetXaxis()->GetBinCenter(xi);
    const double R_abs=h->Interpolate(pt); // Rdata/RMC for the absolute scale
    const double inv_R_abs = 1. / R_abs;
    //printf("pT = %7.1f GeV, abs calib: %.4f\n",pt,abs);
    for (int yi=1;yi<=prod->GetNbinsY();yi++) {
      double c_rel = h2d->GetBinContent(xi,yi); // 1/Rrel = RMC/Rdata
      prod->SetBinContent(xi,yi,c_rel*inv_R_abs);
    }
  }
  return prod;
}

TH2D * InsituDataCorrection::invertHistogram(TH2D *h2d){
  TH2D *inv = (TH2D*)h2d->Clone();
  for (int xi=1;xi<=inv->GetNbinsX();xi++) {
    for (int yi=1;yi<=inv->GetNbinsY();yi++) {
      inv->SetBinContent(xi,yi,1./h2d->GetBinContent(xi,yi));

    }
  }
  return inv;
}
