/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     SoftMuonTag.cxx
PACKAGE:  offline/PhysicsAnalysis/JetTagging/JetTagTools

PURPOSE:  b-tagging based on soft muon identification
********************************************************************/
#include "JetTagTools/SoftMuonTag.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <limits>
#include <cmath>

#include "GaudiKernel/IToolSvc.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODBTagging/BTagging.h"
#include "xAODJet/Jet.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODBTagging/SecVtxHelper.h"

#include "GaudiKernel/MsgStream.h"
#include "Navigation/NavigationToken.h"
#include "GaudiKernel/IToolSvc.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h" 

#include "JetTagInfo/TruthInfo.h"
#include "JetTagInfo/SoftMuonInfo.h"
#include "JetTagInfo/SoftLeptonTruthInfo.h"
#include "JetTagInfo/SLTrueInfo.h"

#include "JetTagTools/NewLikelihoodTool.h"
#include "JetTagTools/HistoHelperRoot.h"
#include "GaudiKernel/ITHistSvc.h"

#include "xAODMuon/MuonContainer.h"

#include "AthenaKernel/Units.h"

#include <fstream>
#include <algorithm>
#include <utility>
#include <vector>
#include <map>
#include <list>
#include <math.h> 


namespace Analysis
{

  SoftMuonTag::SoftMuonTag(const std::string& t, const std::string& n, const IInterface* p)
    : base_class(t,n,p),
      m_trackToVertexIPEstimator(this),
      m_muonSelectorTool("JVC_MuonSelectorTool", this),
      m_likelihoodTool("Analysis::NewLikelihoodTool", this),
      m_SVmuonFinder("InDet::InDetSVWithMuonTool/findSVwithMuon", this),
      m_histoHelper(0)
  {
    /** ANDREA **/
    // which calibration folder to use
    declareProperty("taggerNameBase", m_taggerNameBase = "SMT");
    declareProperty("taggerName", m_taggerName = "SMT");
    declareProperty("decTagName", m_decTagName = "SMT_inputs");
    declareProperty("inputSoftMuonSourceName", m_softmuon_infosource = "SMT");
    declareProperty("xAODBaseName",      m_xAODBaseName);
    /** ANDREA **/

    declareProperty("Runmodus",       m_runModus               = "analysis");
    declareProperty("RefType",        m_refType                = "ALL");
    declareProperty("TaggingAlgType", m_algMode                = "L1D");
    declareProperty("BTagJetPTmin",   m_pTjetmin               = 15.*Gaudi::Units::GeV);
    declareProperty("BTagJetEtamin",  m_etajetmin              = 2.7);
    declareProperty("LikelihoodTool", m_likelihoodTool);
    declareProperty("TrackToVertexIPEstimator", m_trackToVertexIPEstimator);   
    declareProperty("muonSelectorTool", 	m_muonSelectorTool);
    declareProperty("checkOverflows", m_checkOverflows         = false);
    declareProperty("purificationDeltaR", m_purificationDeltaR = 0.8);
    declareProperty("muonIsolDeltaR", m_muonIsolDeltaR         = 0.7);
    declareProperty("UseBinInterpol", m_UseBinInterpol         = true);
    
    declareProperty("jetCollectionList"    , m_jetCollectionList);
    declareProperty("useForcedCalibration" , m_doForcedCalib   = false);
    declareProperty("ForcedCalibrationName", m_ForcedCalibName = "Cone4H1Tower");

    // VD: not sure which of these is needed ....
    declareProperty("RecAlgorithm",   m_alg                    = 1);
    declareProperty("CutD0",          m_d0cut                  = 4.*Gaudi::Units::mm);
    declareProperty("CutPT",          m_pTcut                  = 4.*Gaudi::Units::GeV);
    declareProperty("CutDR",          m_DRcut                  = 0.5);
    declareProperty("CutMatchChi2",   m_MatchChi2cut           = 10);

    declareProperty("writeInfoPlus",  m_writeInfoPlus          = true);

    declareProperty("originalMuCollectionName", m_originalMuCollectionName = "StacoMuonCollection");
    declareProperty("MuonAssociationName"     , m_muonAssociationName="Muons");

    /** number of hypotheses = 3 : b,l,c */
    m_hypothese.push_back("SoftMb");
    m_hypothese.push_back("SoftMl");
    m_hypothese.push_back("SoftMc");

    // List of histogram names
    m_histoname.push_back("pT");
    m_histoname.push_back("pTrel");
    m_histoname.push_back("pTLowPt");
    m_histoname.push_back("pTrelLowPt");
    m_histoname.push_back("pTpTrel");
    m_histoname.push_back("pTpTrelLowPt");
    m_histoname.push_back("JetETEffL1D");
    m_histoname.push_back("JetETLowPtEffL1D");
    m_histoname.push_back("JetETNormL1D");
    m_histoname.push_back("JetETLowPtNormL1D");
    m_histoname.push_back("JetETEffL1DL1D");
    m_histoname.push_back("JetETLowPtEffL1DL1D");
    m_histoname.push_back("JetETNormL1DL1D");
    m_histoname.push_back("JetETLowPtNormL1DL1D");
    m_histoname.push_back("JetETEffL2D");
    m_histoname.push_back("JetETLowPtEffL2D");
    m_histoname.push_back("JetETNormL2D");
    m_histoname.push_back("JetETLowPtNormL2D");
    
    
    declareProperty("MuonQuality",		m_muonQualityCut = xAOD::Muon::Medium ); 
  }

  SoftMuonTag::~SoftMuonTag()
  {
  }

  StatusCode SoftMuonTag::initialize()
  {
  
    ATH_MSG_INFO("#BTAG# Initializing..."); 
    printParameterSettings();

    if (m_SVmuonFinder.retrieve().isFailure()) {
    ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_SVmuonFinder);
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_SVmuonFinder);
    }

    /** ANDREA **/
    m_warnCounter=0;
    m_disableAlgo=false;
    m_treeName = "BDT";
    m_varStrName = "variables";
    
    // prepare readKey for calibration data:
    ATH_CHECK(m_readKey.initialize());
    
    /** ANDREA **/

    /** retrieving ToolSvc: */
    IToolSvc* toolSvc;
    StatusCode sc = service("ToolSvc", toolSvc);
    if (StatusCode::SUCCESS != sc) {
      ATH_MSG_ERROR( "#BTAG# Can't get ToolSvc");
      return StatusCode::FAILURE;
    }
  
    //Retrieve the Muon Selectot tool
    sc = m_muonSelectorTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_muonSelectorTool);
      return sc;
    } else {
      ATH_MSG_INFO("#BTAG# Retrieved tool " << m_muonSelectorTool);  
    }

    ATH_CHECK( m_TrackParticles.initialize() );

    if (m_trackToVertexIPEstimator.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_trackToVertexIPEstimator);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_trackToVertexIPEstimator);
    }

    // If the jet author is not known 
    // (or one wants a calibration not corresponding to the author), can force the calibration. 
    // Check that this calibration has been loaded
    if (m_doForcedCalib) {
      if (std::find( m_jetCollectionList.begin(), 
		     m_jetCollectionList.end(), 
		     m_ForcedCalibName ) == m_jetCollectionList.end()) {
	ATH_MSG_ERROR( "#BTAG# Error, forced calibration to an unloaded one");
	return StatusCode::FAILURE;
      }
    }

    m_likelihoodTool.disable(); //WL Disable this tools since it's not used as long as the section below is commented out


    /* ----------------------------------------------------------------------------------- */
    /*                         BOOK HISTOS IF IN REFERENCE MODE                            */
    /* ----------------------------------------------------------------------------------- */
    if (m_runModus=="reference") {
      //
      // Book the histos
      // 
      ITHistSvc* myHistoSvc;
      if( service( "THistSvc", myHistoSvc ).isSuccess() ) {
	ATH_MSG_DEBUG( "#BTAG# "<< name() << ": HistoSvc loaded successfully.");
	m_histoHelper = new HistoHelperRoot(myHistoSvc);
	m_histoHelper->setCheckOverflows(m_checkOverflows);
      } else ATH_MSG_ERROR( "#BTAG# " << name() << ": HistoSvc could NOT bo loaded.");
      //
      ATH_MSG_INFO( "#BTAG# Booking histos...");
      for(uint ijc=0;ijc<m_jetCollectionList.size();ijc++) {
	for(uint ih=0;ih<m_hypothese.size();ih++) {
	  if(ih==0) {
	    // Control:
	    std::string hDir = "/RefFile/SoftMu/"+m_jetCollectionList[ijc]+"/controlSoftMu/";
	    m_histoHelper->bookHisto(hDir+"eta","eta",60,-3.,3.);
	    m_histoHelper->bookHisto(hDir+"phi","phi",64,-3.2,3.2);
	    m_histoHelper->bookHisto(hDir+"pt","pt",50,0.,300.);
	    m_histoHelper->bookHisto(hDir+"smpt","Soft Muon pT",100,0.,100.);
	  }
	  std::string hDir = "/RefFile/SoftMu/"+m_jetCollectionList[ijc]+"/"+m_hypothese[ih]+"/";
	  // variables:
	  m_histoHelper->bookHisto(hDir+"pT",           "pT/(pT+5)",100,0.,1.);
	  m_histoHelper->bookHisto(hDir+"pTrel",        "pTrel/(pTrel+0.5)",100,0.,1.);
	  m_histoHelper->bookHisto(hDir+"pTLowPt",      "pT/(pT+5)",100,0.,1.);
	  m_histoHelper->bookHisto(hDir+"pTrelLowPt",   "pTrel/(pTrel+0.5)",100,0.,1.);
	  m_histoHelper->bookHisto(hDir+"pTpTrel",      "pT/(pT+5) vs pTrel/(pTrel+0.5)" ,100,0.,1.,100,0.,1.);
	  m_histoHelper->bookHisto(hDir+"pTpTrelLowPt", "pT/(pT+5) vs pTrel/(pTrel+0.5)" ,100,0.,1.,100,0.,1.);
	  // normalization:
	  // 1D:
	  m_histoHelper->bookHisto(hDir+"JetETEffL1D",  "Jet Et",100,0,500);
	  m_histoHelper->bookHisto(hDir+"JetETLowPtEffL1D",  "Jet Et",100,0,500);
	  m_histoHelper->bookHisto(hDir+"JetETNormL1D", "Jet Et",100,0,500);
	  m_histoHelper->bookHisto(hDir+"JetETLowPtNormL1D", "Jet Et",100,0,500);
	  // 1Dx1D:
	  m_histoHelper->bookHisto(hDir+"JetETEffL1DL1D",  "Jet Et",100,0,500);
	  m_histoHelper->bookHisto(hDir+"JetETLowPtEffL1DL1D",  "Jet Et",100,0,500);
	  m_histoHelper->bookHisto(hDir+"JetETNormL1DL1D", "Jet Et",100,0,500);
	  m_histoHelper->bookHisto(hDir+"JetETLowPtNormL1DL1D", "Jet Et",100,0,500);
	  // 2D:
	  m_histoHelper->bookHisto(hDir+"JetETEffL2D",       "Jet Et",100,0,500);
	  m_histoHelper->bookHisto(hDir+"JetETLowPtEffL2D",  "Jet Et",100,0,500);
	  m_histoHelper->bookHisto(hDir+"JetETNormL2D",      "Jet Et",100,0,500);
	  m_histoHelper->bookHisto(hDir+"JetETLowPtNormL2D", "Jet Et",100,0,500);
	}
      }
      m_histoHelper->print();

    }
  
    return StatusCode::SUCCESS;
  }


  StatusCode SoftMuonTag::finalize()
  {
    /** ANDREA **/
    ATH_MSG_DEBUG("#BTAG# Finalizing SoftMuonTag.");
    /** ANDREA **/
    return StatusCode::SUCCESS;
  }


  StatusCode SoftMuonTag::tagJet(const xAOD::Vertex& priVtx,
                                 const xAOD::Jet& jetToTag,
                                 xAOD::BTagging& BTag,
                                 const std::string &jetName) const
  {
    ATH_MSG_DEBUG( "#BTAG# Starting tagJet");

    /** author to know which jet algorithm: */
    std::string author = jetName;
    if (m_doForcedCalib) author = m_ForcedCalibName;
    ATH_MSG_VERBOSE("#BTAG# Using jet type " << author << " for calibrations.");

    /* The jet */
    double jeteta = jetToTag.eta(), jetphi = jetToTag.phi(), jetpt = jetToTag.pt();
    ATH_MSG_DEBUG( "#BTAG# Jet properties : eta = "<<jeteta
		   <<" phi = "<<jetphi
		   <<" pT  = "<<jetpt/1.e3 
		   <<" author = " <<author);

    /** ANDREA **/
    /*
     * #index for this function
     * #1: Preparation of MVA instance using MVAUtils BDT
     * #2: set input variables from MultivariateTagManager inputs map
     * #3: Calculation of MVA output variable(s)
     * #4: Fill MVA output variable(s) into xAOD
     */

    // #0: Check if disabled or not
    if ( m_disableAlgo ) {
      if (m_warnCounter<20) {
	ATH_MSG_WARNING("#BTAG# .... algorithm was disabled due to mis-configuration problems: no output will be written ... PLEASE WATCH OUT" );
	m_warnCounter++;
      }
    }

    // #1: Preparation of MVA instance using BDT
    ATH_MSG_DEBUG("#BTAG# Jet author for SoftMuon: " << author );

    //Retrieval of Calibration Condition Data objects
    SG::ReadCondHandle<JetTagCalibCondData> readCdo(m_readKey);

    ATH_MSG_DEBUG("#BTAG# Booking MVAUtils::BDT for "<<m_taggerNameBase);
    //Retrieve BDT from cond object
    MVAUtils::BDT *bdt(nullptr);
    ATH_MSG_DEBUG("#BTAG# Booking MVAUtils::BDT for "<<m_taggerNameBase);
    bdt = readCdo->retrieveBdt("SoftMu",author);
    if (!bdt) {
      ATH_MSG_WARNING("#BTAG# No BDT for " << m_taggerNameBase<<" exists in the condition object.. Disabling algorithm.");
      m_disableAlgo=true;
      return StatusCode::SUCCESS;
    }

    //Retrieve input variables of BDT from cond object
    std::vector<std::string> inputVars = readCdo->retrieveInputVars(m_taggerNameBase,author, m_taggerNameBase+"Calib/"+m_varStrName);

    std::string alias = readCdo->getChannelAlias(author);
    std::vector<float*>  inputPointers; inputPointers.clear();
    unsigned nConfgVar=0; bool badVariableFound=false;

    Vars vars;
    vars.SetVariableRefs(msg(),inputVars,nConfgVar,badVariableFound,inputPointers);
    ATH_MSG_DEBUG("#BTAG# nConfgVar"<<nConfgVar
		    <<", badVariableFound= "<<badVariableFound <<", inputPointers.size()= "<<inputPointers.size() );

    if ( inputVars.size()!=nConfgVar or badVariableFound ) {
      ATH_MSG_WARNING("#BTAG# Number of expected variables for SoftMu: "<< nConfgVar << "  does not match the number of variables found in the calibration file: " << inputVars.size() << " ... the algorithm will be 'disabled' "<<alias<<" "<<author);
      m_disableAlgo=true;
      return StatusCode::SUCCESS;
    }
 
    bdt->SetPointers(inputPointers);


    // Reference only: Fill control histograms and get jet label
    std::string pref = "";
    std::string label = "N/A";
    std::vector<Amg::Vector3D> hardMus;
    if (m_runModus=="reference") {
      m_histoHelper->fillHisto("/RefFile/SoftMu/"+author+"/controlSoftMu/eta",(double)jeteta);
      if (fabs(jeteta) <= m_etajetmin) {
	m_histoHelper->fillHisto("/RefFile/SoftMu/"+author+"/controlSoftMu/phi",(double)jetphi);
	m_histoHelper->fillHisto("/RefFile/SoftMu/"+author+"/controlSoftMu/pt",(double)jetpt/1.e3);
      }
      else return StatusCode::SUCCESS; // failed eta cut
    }

    std::vector<ElementLink<xAOD::MuonContainer> > assocMuons;
    assocMuons= BTag.auxdata<std::vector<ElementLink<xAOD::MuonContainer> > >(m_muonAssociationName);
    if ( assocMuons.size()==0 ) {
      ATH_MSG_DEBUG( "#BTAG# Found no associated muons to the jet");
      ///return StatusCode::SUCCESS; /// need to go untill the end to decorate
    }

    /// variables used to decorate the Btagging object
    float jet_mu_dRmin_pt=0;
    float jet_mu_dRmin_dR=10;
    float jet_mu_dRmin_qOverPratio    = 0;
    float jet_mu_dRmin_mombalsignif   = 0;
    float jet_mu_dRmin_scatneighsignif= 0;
    float jet_mu_dRmin_pTrel=0;
    float jet_mu_dRmin_d0   =0;
    float jet_mu_dRmin_z0   =0;
    float jet_mu_dRmin_ID_qOverP_var  = 0;   
    //SVMT: combining both SMT and SV-related info
    float jet_mu_sv_mass=-1., jet_mu_sv_efrc=-1., jet_mu_sv_VtxnormDist=-1.;
    float jet_mu_sv_Lxy=-10., jet_mu_sv_L3d=-10., jet_mu_sv_dR=-1.;
    float jet_mu_sv_dmaxPt=0.;
    double d_jet_mu_sv_dmaxPt=0.;
    float jet_mu_sv_ntrk=40., jet_mu_sv_ntrkVtx=40.;

    int muonIndex=-1;

    for (unsigned int iT=0; iT<assocMuons.size(); iT++) {
      if ( !assocMuons.at(iT).isValid() ) continue;
      const xAOD::Muon* tmpMuon= *(assocMuons.at(iT));
      if ( tmpMuon==0 ) continue;
    
      if ( tmpMuon->muonType() != xAOD::Muon::Combined) continue;

    

      // muon selection here:
      float dR = jetToTag.p4().DeltaR(tmpMuon->p4());
      if(dR>=0.4) continue;
    

      const ElementLink< xAOD::TrackParticleContainer >& pMuIDTrack=tmpMuon->inDetTrackParticleLink();
      const ElementLink< xAOD::TrackParticleContainer >& pMuMSTrack=tmpMuon->muonSpectrometerTrackParticleLink();
      if ( !pMuIDTrack.isValid() || !pMuMSTrack.isValid()) continue;

      const std::vector<float>&cov= (*pMuIDTrack)->definingParametersCovMatrixVec();
      float momBalSignif0=0.;
      tmpMuon->parameter(momBalSignif0, xAOD::Muon::momentumBalanceSignificance);
      ATH_MSG_DEBUG("#BTAG# momBalSignif0= "<< momBalSignif0 );
      ATH_MSG_DEBUG("#BTAG# qOverP()= "<< (*pMuMSTrack)->qOverP() );
      if( momBalSignif0==0 )          continue;
      if( (*pMuMSTrack)->qOverP()==0) continue;
     
      // Can be uncommented in case further selection on muon quality is needed
      //ATH_MSG_DEBUG("#BTAG# muon quality= "<< m_muonSelectorTool->getQuality(*tmpMuon) );
      //xAOD::Muon::Quality quality = m_muonSelectorTool->getQuality(*tmpMuon);
      //ATH_MSG_DEBUG("#BTAG# muon quality= "<< quality );
      //just added this cut         
      //if( quality > m_muonQualityCut ) continue; 

      float scatNeighSignif=0.;
      tmpMuon->parameter(scatNeighSignif, xAOD::Muon::scatteringNeighbourSignificance);
      ATH_MSG_DEBUG("#BTAG# scatNeighSignif= "<< scatNeighSignif );
      TLorentzVector myjet, mymu;
      myjet.SetPtEtaPhiM(jetToTag.pt(),jetToTag.eta(),jetToTag.phi(),0);
      mymu.SetPtEtaPhiM(tmpMuon->pt(),tmpMuon->eta(),tmpMuon->phi(),0);
      float pTrel      =myjet.Vect().Perp(mymu.Vect()); // VD: everything MUST be in MeV
      float qOverPratio=(*pMuIDTrack)->qOverP()/(*pMuMSTrack)->qOverP();
    
      float d0 = tmpMuon->primaryTrackParticle()->d0();
      float z0 = tmpMuon->primaryTrackParticle()->z0()+(tmpMuon->primaryTrackParticle()->vz())-(priVtx.z()) ;

      //Finding SV with a muon
      SG::ReadHandle<xAOD::TrackParticleContainer> h_TrackParticles (m_TrackParticles);
      ATH_MSG_DEBUG( " retrieve track particle container with key " << m_TrackParticles.key()  );
      if (!h_TrackParticles.isValid()) {
        ATH_MSG_ERROR( " cannot retrieve track particle container with key " << m_TrackParticles.key()  );
        return StatusCode::FAILURE;
      }

      xAOD::TrackParticleContainer::const_iterator trackItr   = h_TrackParticles->begin();
      xAOD::TrackParticleContainer::const_iterator trackItrE =  h_TrackParticles->end();
      std::vector<const xAOD::TrackParticle*> my_trkparticles(0);
      for( ; trackItr != trackItrE; ++trackItr ) {
	const xAOD::TrackParticle* trackParticle = ( *trackItr );
	my_trkparticles.push_back(trackParticle);
      }
      const xAOD::Vertex *SVtx = m_SVmuonFinder->findSVwithMuon(priVtx,tmpMuon->primaryTrackParticle(),my_trkparticles);
      if(SVtx!=NULL) {
	ATH_MSG_DEBUG("**********ANDREA: Found SV with muon!!!");
	jet_mu_sv_mass        = xAOD::SecVtxHelper::VertexMass     (SVtx);
	jet_mu_sv_efrc        = xAOD::SecVtxHelper::EnergyFraction (SVtx);
	jet_mu_sv_ntrk        = xAOD::SecVtxHelper::VtxNtrk        (SVtx);
	jet_mu_sv_VtxnormDist = xAOD::SecVtxHelper::VtxnormDist    (SVtx);
	jet_mu_sv_ntrkVtx = SVtx->nTrackParticles();
	const Amg::Vector3D PVposition = priVtx.position();
	const Amg::Vector3D position   = SVtx    ->position();
	Amg::Vector3D PvSvDir( position.x() - PVposition.x(),
			       position.y() - PVposition.y(),
			       position.z() - PVposition.z() );
	jet_mu_sv_Lxy=sqrt(pow(PvSvDir(0,0),2)+pow(PvSvDir(1,0),2));
	jet_mu_sv_L3d=sqrt(pow(PvSvDir(0,0),2)+pow(PvSvDir(1,0),2)+pow(PvSvDir(2,0),2));
	TVector3 jetDir;  jetDir .SetPtEtaPhi(jetToTag.pt(),jetToTag.eta(),jetToTag.phi());
	TVector3 PvSvDIR; PvSvDIR.SetXYZ(position.x() - PVposition.x(),position.y() - PVposition.y(),position.z() - PVposition.z());
	jet_mu_sv_dR=deltaR(jetDir.Eta(),PvSvDIR.Eta(),jetDir.Phi(),PvSvDIR.Phi());
	TLorentzVector tlv;
	for(int kt=0; kt<(int)SVtx->nTrackParticles(); kt++) tlv+=SVtx->trackParticle(kt)->p4();
	for(int kt=0; kt<(int)SVtx->nTrackParticles(); kt++) d_jet_mu_sv_dmaxPt=TMath::Max(d_jet_mu_sv_dmaxPt,(SVtx->trackParticle(kt)->p4()).Perp(tlv.Vect()));
	jet_mu_sv_dmaxPt=(float)d_jet_mu_sv_dmaxPt;
	ATH_MSG_DEBUG("**********ANDREA: SV mass= "<< jet_mu_sv_mass <<" efrc= "<< jet_mu_sv_efrc <<" ntrk= "<< jet_mu_sv_ntrk<<" VtxnormDist= "<< jet_mu_sv_VtxnormDist);
    delete SVtx;  
      }   
      

      ATH_MSG_DEBUG( "#BTAG# Found muon " << iT << " with PT= " << tmpMuon->pt() 
		     << " dR= " << dR 
		     << " qOverPratio= " << qOverPratio
		     << " ScatNeiSig= " << scatNeighSignif
		     << " MomBalSig= " <<  momBalSignif0
		     << " ptrel= " << pTrel
		     << " z0= " << z0 
		     << " d0= " << d0
		     );

      if(dR<jet_mu_dRmin_dR){
	jet_mu_dRmin_pt=tmpMuon->pt();
	jet_mu_dRmin_dR=dR;
	jet_mu_dRmin_qOverPratio    = qOverPratio;
	jet_mu_dRmin_mombalsignif   = momBalSignif0;
	jet_mu_dRmin_scatneighsignif= scatNeighSignif;
	jet_mu_dRmin_pTrel=pTrel;
	jet_mu_dRmin_d0   =d0;
	jet_mu_dRmin_z0   =z0;
	jet_mu_dRmin_ID_qOverP_var  = cov[14];  
	muonIndex=iT;
      }




    }
  
    if (muonIndex!=-1) ATH_MSG_DEBUG(" #BTAG: choosing muon: " <<  muonIndex);

    // now decorate the b-tagging object
    std::string xAODBaseName="SMT";
    BTag.setVariable<float>(xAODBaseName, "mu_pt"           , jet_mu_dRmin_pt);
    BTag.setVariable<float>(xAODBaseName, "dR"              , jet_mu_dRmin_dR);
    BTag.setVariable<float>(xAODBaseName, "qOverPratio"     , jet_mu_dRmin_qOverPratio);
    BTag.setVariable<float>(xAODBaseName, "mombalsignif"    , jet_mu_dRmin_mombalsignif);
    BTag.setVariable<float>(xAODBaseName, "scatneighsignif" , jet_mu_dRmin_scatneighsignif);
    BTag.setVariable<float>(xAODBaseName, "pTrel"           , jet_mu_dRmin_pTrel);
    BTag.setVariable<float>(xAODBaseName, "mu_d0"           , jet_mu_dRmin_d0);
    BTag.setVariable<float>(xAODBaseName, "mu_z0"           , jet_mu_dRmin_z0);
    BTag.setVariable<float>(xAODBaseName, "ID_qOverP"       , jet_mu_dRmin_ID_qOverP_var);
  
    ElementLink<xAOD::MuonContainer> theLink; 
    if (muonIndex!=-1) theLink=assocMuons.at(muonIndex);
  
    BTag.auxdata< ElementLink<xAOD::MuonContainer> >("SMT_mu_link")=theLink; 

    // #2: Set necessary MVA-input variables
    vars.m_pt     = jetToTag.pt();
    vars.m_absEta = fabs(jetToTag.eta());

    /*** Retrieving soft muon variables ***/
    vars.m_sm_dR=jet_mu_dRmin_dR;
    vars.m_sm_pTrel=jet_mu_dRmin_pTrel;
    vars.m_sm_qOverPratio=jet_mu_dRmin_qOverPratio;  
    vars.m_sm_mombalsignif=jet_mu_dRmin_mombalsignif;
    vars.m_sm_scatneighsignif=jet_mu_dRmin_scatneighsignif;
    vars.m_sm_mu_d0=jet_mu_dRmin_d0;
    //SVMT
    vars.m_sm_mu_sv_mass=jet_mu_sv_mass;
    vars.m_sm_mu_sv_efrc=jet_mu_sv_efrc;
    vars.m_sm_mu_sv_VtxnormDist=jet_mu_sv_VtxnormDist;
    vars.m_sm_mu_sv_Lxy=jet_mu_sv_Lxy;
    vars.m_sm_mu_sv_L3d=jet_mu_sv_L3d;
    vars.m_sm_mu_sv_dR=jet_mu_sv_dR;
    vars.m_sm_mu_sv_dmaxPt=jet_mu_sv_dmaxPt;
    vars.m_sm_mu_sv_ntrk=jet_mu_sv_ntrk;
    vars.m_sm_mu_sv_ntrkVtx=jet_mu_sv_ntrkVtx;

    //////////////////////////////////
    // End of SMT inputs retrieving //
    //////////////////////////////////

    PrintInputs(vars); 

    // #3: Computation of MVA output variable(s)
    /* compute SMT: */
    double smt = -1.1;

    smt = GetClassResponse(bdt);//this gives back double

    if (vars.m_sm_dR==10.) smt=-1.1;
    ATH_MSG_DEBUG("#BTAG# SMT weight: " << smt <<", "<<alias<<", "<<author);

    // #4: Fill MVA output variable(s) into xAOD
    /** give information to the info class. */
    // Can be uncommented if SVMT evaluation wants to be checked (rather than SMT)
    //if(inputVars.size()<10) BTag.setVariable<double>(xAODBaseName, "discriminant", smt);
    //else            	  BTag.setVariable<double>("SVMT", "discriminant", smt);
    BTag.setVariable<double>(xAODBaseName, "discriminant", smt);
    BTag.setVariable<char>(xAODBaseName, "discriminantIsValid", true);

    /** ANDREA **/

    return  StatusCode::SUCCESS;
  }



  void SoftMuonTag::finalizeHistos() 
  {
    if (m_runModus=="reference") {
      for(uint ijc=0;ijc<m_jetCollectionList.size();ijc++) {
	for(uint ih=0;ih<m_hypothese.size();ih++) {
	  std::string hDir = "/BTAG/CALIB/"+m_hypothese[ih]+"/";
	  m_likelihoodTool
	    ->smoothAndNormalizeHistogram( m_histoHelper->getHisto1D(hDir+"pT") , "" );
	  m_likelihoodTool
	    ->smoothAndNormalizeHistogram( m_histoHelper->getHisto1D(hDir+"pTrel") , "" );
	  m_likelihoodTool
	    ->smoothAndNormalizeHistogram( m_histoHelper->getHisto1D(hDir+"pTLowPt") , "" );
	  m_likelihoodTool
	    ->smoothAndNormalizeHistogram( m_histoHelper->getHisto1D(hDir+"pTrelLowPt") , "" );
	  m_likelihoodTool
	    ->smoothAndNormalizeHistogram( m_histoHelper->getHisto1D(hDir+"pTpTrel") , "" );
	  m_likelihoodTool
	    ->smoothAndNormalizeHistogram( m_histoHelper->getHisto1D(hDir+"pTpTrelLowPt") , "" );
	}
      }
    }
    return;
  }


  void SoftMuonTag::printParameterSettings()
  {
    ATH_MSG_INFO( "#BTAG# " << name() << "Parameter settings " );
    ATH_MSG_INFO( "#BTAG# I am in " << m_runModus << " modus." );
    ATH_MSG_INFO( "#BTAG# The method is "<<m_algMode);
    if (m_runModus == "reference") {
      ATH_MSG_INFO( "#BTAG# Preparing "<< m_refType<< "-jet probability density functions...");
    }
  }
  
  void SoftMuonTag::Vars::SetVariableRefs(MsgStream& msg,
                                          const std::vector<std::string>& inputVars,
                                          unsigned &nConfgVar,
                                          bool &badVariableFound,
                                          std::vector<float*> &inputPointers)
  {
    for (unsigned ivar=0; ivar<inputVars.size(); ivar++) {
      if      (inputVars.at(ivar)=="pt"         	     ) { inputPointers.push_back(&m_pt   ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="abs(eta)"   	     ) { inputPointers.push_back(&m_absEta   ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="j_m_dR"     	     ) { inputPointers.push_back(&m_sm_dR    ) ; nConfgVar++; }  
      else if (inputVars.at(ivar)=="j_m_pTrel"  	     ) { inputPointers.push_back(&m_sm_pTrel    ) ; nConfgVar++; }  
      else if (inputVars.at(ivar)=="j_m_qP"     	     ) { inputPointers.push_back(&m_sm_qOverPratio    ) ; nConfgVar++; }  
      else if (inputVars.at(ivar)=="j_m_mombal" 	     ) { inputPointers.push_back(&m_sm_mombalsignif    ) ; nConfgVar++; }  
      else if (inputVars.at(ivar)=="j_m_scat"   	     ) { inputPointers.push_back(&m_sm_scatneighsignif    ) ; nConfgVar++; }  
      else if (inputVars.at(ivar)=="j_m_d0"     	     ) { inputPointers.push_back(&m_sm_mu_d0    ) ; nConfgVar++; }  
      else if (inputVars.at(ivar)=="j_m_s_mass" 	     ) { inputPointers.push_back(&m_sm_mu_sv_mass ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="j_m_s_efrc" 	     ) { inputPointers.push_back(&m_sm_mu_sv_efrc ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="j_m_s_ntrk" 	     ) { inputPointers.push_back(&m_sm_mu_sv_ntrk ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="j_m_s_VtxnormDist" ) { inputPointers.push_back(&m_sm_mu_sv_VtxnormDist ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="j_m_s_ntrkVtx"     ) { inputPointers.push_back(&m_sm_mu_sv_ntrkVtx ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="j_m_s_dmaxPt"      ) { inputPointers.push_back(&m_sm_mu_sv_dmaxPt ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="j_m_s_Lxy" 	     ) { inputPointers.push_back(&m_sm_mu_sv_Lxy ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="j_m_s_L3d" 	     ) { inputPointers.push_back(&m_sm_mu_sv_L3d ) ; nConfgVar++; }
      else if (inputVars.at(ivar)=="j_m_s_dR"  	     ) { inputPointers.push_back(&m_sm_mu_sv_dR ) ; nConfgVar++; }
      else {
	msg << MSG::WARNING << "#BTAG# \""<<inputVars.at(ivar)<<"\" <- This variable found in xml/calib-file does not match to any variable declared in SMT... CHECK PLEASE!!!" << endmsg;
	badVariableFound=true;
      } 
    }
  }//void SoftMuonTag::SetVariableRefs

  void SoftMuonTag::PrintInputs(const Vars& vars) const {
    ATH_MSG_DEBUG("#BTAG# SMT jet info: " <<
                  "  jet pt= "     << vars.m_pt  <<
                  ", jet eta= "    << vars.m_absEta <<
		  ", dR= "         << vars.m_sm_dR <<
		  ", pTrel= "      << vars.m_sm_pTrel <<
		  ", q/p="         << vars.m_sm_qOverPratio <<
                  ", momb="        << vars.m_sm_mombalsignif <<
		  ", scat="        << vars.m_sm_scatneighsignif <<
		  ", d0 ="         << vars.m_sm_mu_d0);
  }

  float SoftMuonTag:: deltaR(float eta1, float eta2, float phi1, float phi2) const {
    float DEta = fabs(eta1 - eta2);
    float DPhi = acos(cos(fabs(phi1 - phi2)));
    return sqrt(pow(DEta, 2) + pow(DPhi, 2));
  }

  /* ANDREA */

}

