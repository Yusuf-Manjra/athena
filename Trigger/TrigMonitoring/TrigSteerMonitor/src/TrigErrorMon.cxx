/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigSteering/TrigSteer.h"

#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/Navigation.icc"
#include "TrigNavigation/NavigationCore.icc"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include <vector>
#include <algorithm>
#include <TH1I.h>
#include <TH2I.h>
#include "TrigSteerMonitor/TrigErrorMon.h"

#include "TrigConfHLTData/HLTTriggerElement.h"
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

using namespace std;
using namespace HLT;

TrigErrorMon::TrigErrorMon(const std::string & type, const std::string & name,
			 const IInterface* parent)
  :  TrigMonitorToolBase(type, name, parent),
     m_parentAlg(0),
     m_histo_reason(0),
     m_histo_action(0),
     m_histo_steeringInternalReason(0),
     m_histo2d_reason(0),
     m_histo2d_action(0),
     m_log(0),
     m_logLvl(0),
     m_trigLvl("") {
  declareInterface<IMonitorToolBase>(this);
  declareProperty("expertMode", m_expertMode=false, "If set to 'true' includes also errors when HLT::Action == CONTINUE and errors eta/phi");

  // set up axis labels for TE_Errors_Slices histogram
  m_te_names.push_back("L2tau");
  m_te_names.push_back("L2trk");
  m_te_names.push_back("L2_e");
  m_te_names.push_back("L2_g");
  m_te_names.push_back("L2_j");
  m_te_names.push_back("L2_b");
  m_te_names.push_back("L2_mu");
  m_te_names.push_back("L2_xe");


}


/* ******************************** finalize ************************************** */
StatusCode TrigErrorMon::finalize()
{
  delete m_log; m_log = 0;
  return StatusCode::SUCCESS;
}


/* ******************************** initialize ************************************** */
StatusCode TrigErrorMon::initialize()
{
  m_log = new MsgStream(msgSvc(), name());
  m_logLvl = m_log->level();

  if ( TrigMonitorToolBase::initialize().isFailure() ) {
    if (m_logLvl <= MSG::ERROR) (*m_log) << MSG::ERROR << " Unable to initialize base class !"
					 << endreq;
    return StatusCode::FAILURE;
  }
  
  m_parentAlg = dynamic_cast<const HLT::TrigSteer*>(parent());
  if ( !m_parentAlg ) {
    if (m_logLvl <= MSG::ERROR)(*m_log) << MSG::ERROR << " Unable to cast the parent algorithm to HLT::TrigSteer !"
	     << endreq;
    return StatusCode::FAILURE;
  }

  m_trigLvl = m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 ? "L2" : m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::EF ? "EF" : "HLT" ;
  
  if (m_logLvl <= MSG::INFO)(*m_log) << MSG::INFO << "Finished initialize() of TrigErrorMon"
	   << endreq;
  
  return StatusCode::SUCCESS;
}


/* ******************************** bookHists  ************************************** */

StatusCode TrigErrorMon::bookHists()
{ 
  if ( bookHistograms( false, false, true ).isFailure() ) { 
    if (m_logLvl <= MSG::ERROR)(*m_log) << MSG::ERROR << "Failure" << endreq;
    return StatusCode::FAILURE;
    
  }
  return StatusCode::SUCCESS;
}




/* ******************************** book Histograms ************************************* */
StatusCode TrigErrorMon::bookHistograms( bool/* isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun*/ )
{
  /*
  if (m_logLvl <= MSG::DEBUG) {
    (*m_log) << MSG::DEBUG << "bookHistograms" << endreq;
  }
  */
  TrigMonGroup expertHistograms( this, boost::replace_all_copy(name(), ".", "/" ), expert );
  
  //retrieve chain names from configuredChains
  std::vector<const HLT::SteeringChain*> cc = m_parentAlg->getConfiguredChains();
  unsigned int totalNChains = cc.size();

  // store chainnames and binnumber in maps
  std::vector<std::string> chainname(totalNChains);
  int binnr=0;
  foreach(const HLT::SteeringChain* chain, cc) {
    unsigned int chaincounter = chain->getChainCounter();
    // fill a map for consecutive numbering, this avoids empty bins, m_binmap starts at 0 !
    // fill array with ChainNames  - chainname starts at 0 !
    chainname[binnr] = chain->getChainName();
    m_binmap[chaincounter] = binnr++;
  }
  
  //resete private members 
  m_histo_reason=0;
  m_histo_action=0;
  m_histo_steeringInternalReason=0;
  m_histo2d_reason=0;
  m_histo2d_action=0;
  
  //get number of error codes
  ErrorCode eclast=HLT::LAST_ERROR;
  int nmaxcode_reason=eclast.reason();
  int nmaxcode_action=eclast.action();
  int nmaxcode_sir=eclast.steeringInternalReason();
  
  // reason and ateeringInternalReason ec=0 => "", start from ec=1
  // action "CONTINUE" start from 0 
  
  //book 3 1D histos  for the 3  types of ErrorCode namely for action, reason and steeringinternalreason


  TString htitle = TString("Errors reason total count ")+TString(m_trigLvl); 

  m_histo_reason =  new TrigLBNHist<TH1I>(TH1I( "ErrorsReasonTotalCount",htitle.Data(),nmaxcode_reason+1, -0.5, nmaxcode_reason+0.5));
  

  htitle=TString("Errors action total count ")+TString(m_trigLvl);
  m_histo_action = new TrigLBNHist<TH1I>(TH1I( "ErrorsActionTotalCount",htitle.Data(),nmaxcode_action+1, -0.5, nmaxcode_action+0.5));

  htitle=TString("Errors steering internal reason  total count ")+TString(m_trigLvl);
  m_histo_steeringInternalReason = new TrigLBNHist<TH1I>(TH1I( "ErrorsSteeringInternalReasonTotalCount",htitle.Data(),nmaxcode_sir+1, -0.5, nmaxcode_sir+0.5));
  
  //book 2 2d histos for  ErrorCode vs chain for action and reason
 
  
  htitle=TString("Errors reason per chain ")+TString(m_trigLvl);
  m_histo2d_reason =  new TH2I( "ErrorsReasonPerChain",htitle.Data(),
				 totalNChains, -0.5, totalNChains-0.5,
				 nmaxcode_reason+1, -0.5, nmaxcode_reason+0.5);
  
  htitle=TString("Errors action per chain ")+TString(m_trigLvl);
  m_histo2d_action =  new TH2I( "ErrorsActionPerChain",htitle.Data(),
				 totalNChains, -0.5, totalNChains-0.5,
				 nmaxcode_action+1, -0.5, nmaxcode_action+0.5);
  

  
  //add axis labels
  m_histo2d_reason->GetYaxis()->SetTitle("ErrorCode");
  m_histo2d_reason->GetXaxis()->SetTitle("chains");
  m_histo2d_action->GetYaxis()->SetTitle("ErrorCode");
  m_histo2d_action->GetXaxis()->SetTitle("chains");
  //make histos nice 
  m_histo2d_reason->GetYaxis()->SetTitleOffset(3);
  m_histo2d_action->GetYaxis()->SetTitleOffset(3);

  
  // labeling the ErrorCode bins
  // loop errorcodes 
  for(int i=0;i<= max(max(nmaxcode_reason,nmaxcode_sir),nmaxcode_action);i++){
    //retrieve ErrorCodes
    ErrorCode ec=HLT::ErrorCode((HLT::Action::Code) min(i,nmaxcode_action),
				(HLT::Reason::Code) min(i,nmaxcode_reason),
				(HLT::SteeringInternalReason::Code) min(i,nmaxcode_sir));
    //get strings
    std::string sirName=strErrorCode(ec);
    std::string actionName,reasonName;
    string::size_type loc1=strErrorCode(ec).find(" ");
    string::size_type loc2=strErrorCode(ec).find_last_of(" ");
    //retrieve action, reason and steeringinternalreason code 
    if (loc1 ==std::string::npos  || loc1==loc2){
      if (m_logLvl <= MSG::WARNING)(*m_log)<<MSG::WARNING<<"  can not identify Action, Reason and SteeringInternalReason from strErrorCode "<<endreq;
    }else{
      actionName=sirName.substr(0,loc1);
      reasonName=sirName.substr(loc2);
      sirName=sirName.substr(loc1,loc2-loc1);
    }

    // set bin labels, the first bin is UNKNOWN. (the zeroth bin is the underflow bin)
    // and from the second bin on, the knowns
    if(i>=1 && i<=nmaxcode_reason){
      if ( i==1 ){
	m_histo_reason->  GetXaxis()->SetBinLabel(i, "UNKNOWN");
	m_histo2d_reason->GetYaxis()->SetBinLabel(i, "UNKNOWN");
      }
      m_histo_reason->  GetXaxis()->SetBinLabel(i+1, reasonName.c_str());
      m_histo2d_reason->GetYaxis()->SetBinLabel(i+1, reasonName.c_str());
    }
    if(i>=1 && i<=nmaxcode_action){
      if ( i==1 ){
	m_histo_action->GetXaxis()->SetBinLabel(i, "CONTINUE");
	m_histo2d_action->GetYaxis()->SetBinLabel(i, "CONTINUE");
      }
      m_histo_action->GetXaxis()->SetBinLabel(i+1, actionName.c_str());
      m_histo2d_action->GetYaxis()->SetBinLabel(i+1, actionName.c_str());
    }
    if(i>=1 && i<=nmaxcode_sir){
      if ( i==1 ){
	m_histo_steeringInternalReason->GetXaxis()->SetBinLabel(i, "UNKNOWN");
      }
      m_histo_steeringInternalReason->GetXaxis()->SetBinLabel(i+1, sirName.c_str());
    }
  }
  
  // naming the chain bins of 2D histos  using map chain name
  for (unsigned int tmpbin=1;tmpbin<totalNChains+1;tmpbin++) {
    m_histo2d_action->GetXaxis()->SetBinLabel(tmpbin  , chainname[tmpbin-1].c_str());
    m_histo2d_reason->GetXaxis()->SetBinLabel(tmpbin  , chainname[tmpbin-1].c_str());
  }

  
  if ( expertHistograms.regHist((ITrigLBNHist*)m_histo_reason).isFailure())
    (*m_log) << MSG::WARNING << "Can't book "
	     <<  m_histo_reason->GetName() << endreq;

  if ( expertHistograms.regHist((ITrigLBNHist*)m_histo_action).isFailure())
    (*m_log) << MSG::WARNING << "Can't book "
	     << m_histo_action->GetName() << endreq;

  if ( expertHistograms.regHist((ITrigLBNHist*)m_histo_steeringInternalReason).isFailure())
    (*m_log) << MSG::WARNING << "Can't book "
	     << m_histo_steeringInternalReason->GetName() << endreq;
  
  if ( expertHistograms.regHist(m_histo2d_reason).isFailure())
    (*m_log) << MSG::WARNING << "Can't book "
	     <<  m_histo2d_reason->GetName() << endreq;
  
  if ( expertHistograms.regHist(m_histo2d_action).isFailure())
    (*m_log) << MSG::WARNING << "Can't book "
	     << m_histo2d_action->GetName() << endreq;


  if ( ! m_expertMode )
    return StatusCode::SUCCESS;

  // book histo with eta-phi map of TEs errors
  m_histo_te_errors_etaphi = new TH2I("TE_Errors_All",
      "#eta vs #varphi of TEs errors", 64, -4.0, 4.0, 64,  -M_PI*(1.-1./64.), M_PI*(1.+1./64.));
  m_histo_te_errors_etaphi->GetXaxis()->SetTitle("#eta_{ROI}");
  m_histo_te_errors_etaphi->GetYaxis()->SetTitle("#varphi_{ROI} / rad");

  // 
  m_histo_te_errors_names = new TH1I("TE_Errors_Slices",
      "Entries", m_te_names.size(), -0.5, m_te_names.size() - 0.5 );

  vector<string>::const_iterator te_names_itr;
  int bin;
  for (bin = 1, te_names_itr = m_te_names.begin(); te_names_itr != m_te_names.end(); ++te_names_itr, ++bin ) {
    m_histo_te_errors_names -> GetXaxis()->SetBinLabel( bin, (*te_names_itr).c_str());
  }
  
  m_histo_tau_errors_etaphi = new TH2I("TE_Errors_tau", 
      "#eta vs #varphi of tau TE errors", 51, -2.55, 2.55, 64, -M_PI*(1.-1./64.), M_PI*(1.+1./64.));
  m_histo_tau_errors_etaphi->GetXaxis()->SetTitle("#eta_{ROI}");
  m_histo_tau_errors_etaphi->GetYaxis()->SetTitle("#varphi_{ROI} / rad");

  m_histo_trk_errors_etaphi = new TH2I("TE_Errors_trk", 
      "#eta vs #varphi of track TE errors", 51, -2.55, 2.55, 64, -M_PI*(1.-1./64.), M_PI*(1.+1./64.));
  m_histo_trk_errors_etaphi->GetXaxis()->SetTitle("#eta_{ROI}");
  m_histo_trk_errors_etaphi->GetYaxis()->SetTitle("#varphi_{ROI} / rad");

  m_histo_e_errors_etaphi = new TH2I("TE_Errors_e", 
      "#eta vs #varphi of electron TE errors", 51, -2.55, 2.55, 64, -M_PI*(1.-1./64.), M_PI*(1.+1./64.));
  m_histo_e_errors_etaphi->GetXaxis()->SetTitle("#eta_{ROI}");
  m_histo_e_errors_etaphi->GetYaxis()->SetTitle("#varphi_{ROI} / rad");

  m_histo_g_errors_etaphi = new TH2I("TE_Errors_g", 
      "#eta vs #varphi of photon TE errors", 51, -2.55, 2.55, 64, -M_PI*(1.-1./64.), M_PI*(1.+1./64.));
  m_histo_g_errors_etaphi->GetXaxis()->SetTitle("#eta_{ROI}");
  m_histo_g_errors_etaphi->GetYaxis()->SetTitle("#varphi_{ROI} / rad");

  m_histo_j_errors_etaphi = new TH2I("TE_Errors_j", 
      "#eta vs #varphi of jet TE errors", 31, -3.1, 3.1, 32,  -M_PI*(1.-1./32.), M_PI*(1.+1./32.) );
  m_histo_j_errors_etaphi->GetXaxis()->SetTitle("#eta_{ROI}");
  m_histo_j_errors_etaphi->GetYaxis()->SetTitle("#varphi_{ROI} / rad");

  m_histo_b_errors_etaphi = new TH2I("TE_Errors_b", 
      "#eta vs #varphi of b tagged TE errors", 51, -2.55, 2.55, 64, -M_PI*(1.-1./64.), M_PI*(1.+1./64.));
  m_histo_b_errors_etaphi->GetXaxis()->SetTitle("#eta_{ROI}");
  m_histo_b_errors_etaphi->GetYaxis()->SetTitle("#varphi_{ROI} / rad");

  m_histo_mu_errors_etaphi = new TH2I("TE_Errors_mu", 
      "#eta vs #varphi of muon TE errors", 50, -2.5, 2.5, 64, -M_PI, M_PI);
  m_histo_mu_errors_etaphi->GetXaxis()->SetTitle("#eta_{ROI}");
  m_histo_mu_errors_etaphi->GetYaxis()->SetTitle("#varphi_{ROI} / rad");

  m_histo_xe_errors_etaphi = new TH2I("TE_Errors_xe", 
      "#eta vs #varphi of missing energy TE errors", 51, -2.55, 2.55, 64, -M_PI*(1.-1./64.), M_PI*(1.+1./64.));
  m_histo_xe_errors_etaphi->GetXaxis()->SetTitle("#eta_{ROI}");
  m_histo_xe_errors_etaphi->GetYaxis()->SetTitle("#varphi_{ROI} / rad");

  

  if ( expertHistograms.regHist(m_histo_te_errors_etaphi).isFailure()) {
    (*m_log) << MSG::WARNING << "Can't book "
	     <<  m_histo_te_errors_etaphi->GetName() << endreq;
  }

  if ( expertHistograms.regHist(m_histo_tau_errors_etaphi).isFailure()) {
    (*m_log) << MSG::WARNING << "Can't book "
	     <<  m_histo_tau_errors_etaphi->GetName() << endreq;
  }

  if ( expertHistograms.regHist(m_histo_trk_errors_etaphi).isFailure()) {
    (*m_log) << MSG::WARNING << "Can't book "
	     <<  m_histo_trk_errors_etaphi->GetName() << endreq;
  }

  if ( expertHistograms.regHist(m_histo_e_errors_etaphi).isFailure()) {
    (*m_log) << MSG::WARNING << "Can't book "
	     <<  m_histo_e_errors_etaphi->GetName() << endreq;
  }

  if ( expertHistograms.regHist(m_histo_g_errors_etaphi).isFailure()) {
    (*m_log) << MSG::WARNING << "Can't book "
	     <<  m_histo_g_errors_etaphi->GetName() << endreq;
  }

  if ( expertHistograms.regHist(m_histo_j_errors_etaphi).isFailure()) {
    (*m_log) << MSG::WARNING << "Can't book "
	     <<  m_histo_j_errors_etaphi->GetName() << endreq;
  }

  if ( expertHistograms.regHist(m_histo_b_errors_etaphi).isFailure()) {
    (*m_log) << MSG::WARNING << "Can't book "
	     <<  m_histo_b_errors_etaphi->GetName() << endreq;
  }

  if ( expertHistograms.regHist(m_histo_mu_errors_etaphi).isFailure()) {
    (*m_log) << MSG::WARNING << "Can't book "
	     <<  m_histo_mu_errors_etaphi->GetName() << endreq;
  }

  if ( expertHistograms.regHist(m_histo_xe_errors_etaphi).isFailure()) {
    (*m_log) << MSG::WARNING << "Can't book "
	     <<  m_histo_xe_errors_etaphi->GetName() << endreq;
  }

  if ( expertHistograms.regHist(m_histo_te_errors_names).isFailure()) {
    (*m_log) << MSG::WARNING << "Can't book "
	     <<  m_histo_te_errors_names->GetName() << endreq;
  }

  //if (m_logLvl <= MSG::DEBUG)   (*m_log) << MSG::DEBUG << "bookHists() done" << endreq;
  
  return StatusCode::SUCCESS;
}


bool need_fill(const HLT::ErrorCode& ec, bool includeall) {
  if (includeall)
    return true;
  else 
    return ec.action() > HLT::Action::CONTINUE;
}

/* ******************************** fill Histograms ************************************** */
StatusCode TrigErrorMon::fillHists()
{
  
  ErrorCode ec ;
  ec = m_parentAlg->getAlgoConfig()->getHltStatus() ;
  
  if( !m_histo_action || !m_histo_reason || !m_histo_steeringInternalReason 
      || !m_histo2d_reason || !m_histo2d_action ){
    (*m_log)<<MSG::WARNING<<" pointers to histograms not ok, dont Fill ! "<<endreq;
    return StatusCode::FAILURE;
  }


  if ( ec == HLT::OK ) return StatusCode::SUCCESS;

  if ( need_fill(ec, m_expertMode) ) { 
    m_histo_action->Fill((int) ec.action() );
    m_histo_reason->Fill((int) ec.reason() );
    m_histo_steeringInternalReason->Fill((int) ec.steeringInternalReason() );
  } else {
    return StatusCode::SUCCESS;
  }

  const std::vector<const HLT::SteeringChain*>& activeChains = m_parentAlg->getActiveChains();
 
  for (std::vector<const HLT::SteeringChain*>::const_iterator chain = activeChains.begin();
       chain != activeChains.end(); ++chain) {
    Int_t chaincounter = (Int_t)(*chain)->getChainCounter();
    ErrorCode ecchain=(ErrorCode) (*chain)->getErrorCode();
    if ( ecchain != HLT::OK ) {
      if ( need_fill(ecchain, m_expertMode) ){
	     m_histo2d_reason->Fill(m_binmap[chaincounter],(int) ecchain.reason());
	     m_histo2d_action->Fill(m_binmap[chaincounter],(int) ecchain.action());	
      }
    }
  }

  if ( ! m_expertMode )
    return StatusCode::SUCCESS;

  // Get TEs
  std::vector<HLT::TriggerElement*> TEs;
  m_parentAlg->getAlgoConfig()->getNavigation()->getAll(TEs, true);

  std::vector<HLT::TriggerElement*>::const_iterator te;

  // Loop over TEs
  for (te = TEs.begin(); te != TEs.end(); ++te) {

    // If TE contains errros fill histogram with eta and phi of this TE 
    std::string label; 

    if ( (*te)->getErrorState() ) {

      // Get trigger ROI descriptor
      std::vector<const TrigRoiDescriptor*> roiDescriptor;
      if (  m_parentAlg->getAlgoConfig()->getNavigation()->getRecentFeatures( 
            (*te), roiDescriptor, "initialRoI")  == false ) {
        continue;
      }

      std::vector<const TrigRoiDescriptor*>::const_iterator roiDescriptorIt;
      for ( roiDescriptorIt = roiDescriptor.begin(); roiDescriptorIt != roiDescriptor.end(); ++roiDescriptorIt){
        if (*roiDescriptorIt == 0)  {
          continue;
        }

        float eta = (*roiDescriptorIt)->eta();
        float phi = (*roiDescriptorIt)->phi();

        m_histo_te_errors_etaphi->Fill( eta, phi );

        // get TE lable
        TrigConf::HLTTriggerElement::getLabel ( (*te)->getId(), label );

        if ( label.find("L2tau") !=  string::npos ) {
          m_histo_tau_errors_etaphi->Fill( eta, phi );

        } else if ( label.find("L2trk") !=  string::npos ) {
          m_histo_trk_errors_etaphi->Fill( eta, phi );

        } else if ( label.find("L2_e") !=  string::npos ) {
          m_histo_e_errors_etaphi->Fill( eta, phi );

        } else if ( label.find("L2_g") !=  string::npos ) {
          m_histo_g_errors_etaphi->Fill( eta, phi );

        } else if ( label.find("L2_j") !=  string::npos ) {
          m_histo_j_errors_etaphi->Fill( eta, phi );

        } else if ( label.find("L2_b") !=  string::npos ) {
          m_histo_b_errors_etaphi->Fill( eta, phi );

        } else if ( label.find("L2_mu") !=  string::npos ) {
          m_histo_mu_errors_etaphi->Fill( eta, phi );

        } else if ( label.find("L2_xe") !=  string::npos ) {
          m_histo_xe_errors_etaphi->Fill( eta, phi );
        }

        vector<string>::const_iterator te_names_itr;
        int bin;
        for (bin = 0, te_names_itr = m_te_names.begin(); te_names_itr != m_te_names.end(); ++te_names_itr, ++bin ) {
          if ( label.find( (*te_names_itr).c_str() ) !=  string::npos ) {
            m_histo_te_errors_names ->Fill( bin );
            break;
          }
        }

      }
    }
  }

  return StatusCode::SUCCESS;
}



