/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
//     TBD: UPDATE THIS
// NAME:     LArNoiseCorrelationMon.cxx
// PACKAGE:  LArMonTools
//
// AUTHOR:   Helary Louis (helary@lapp.in2p3.fr)
//           Originaly From LArOddCellsMonTool.cxx  by Benjamin Trocme
// 
// Monitor a few things in the LArDigit...
//
//	1) Check that the highest value of the LArDigit is contained in an interval. 
//         If it is not the case increment 3 histograms for each subdetector:
//	      a) Out of range histograms
//	      b) The average histograms: give the average value of the highest digit sample 
//	      c) Channel VS FEB histograms: gives wich slot on wich FEB has his highest digit sample ou of the range
//	2) Check if a digits samples are in saturation. If it's the case increment the saturation histograms. 
//
// Available cuts in the jo file:
//
//   a) SampleRangeLow-SampleRangeUp: range to check the digit sample.
//   b) ADCcut : To select Digits Samples with signal.
//   c) ADCsature: lowest value to check if a Digit sample is in saturation.
// ********************************************************************

//Histograms
#include "GaudiKernel/ITHistSvc.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"

#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

//STL:
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>

//LAr infos:
#include "Identifier/HWIdentifier.h"
#include "LArMonTools/LArOnlineIDStrHelper.h"
#include "LArTools/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRecEvent/LArNoisyROSummary.h"

//Events infos:
#include "xAODEventInfo/EventInfo.h"

//trigger dec. tool
//#include "TrigDecisionTool/TrigDecisionTool.h"

//Header:
#include "LArMonTools/LArNoiseCorrelationMon.h"

#include "LArTrigStreamMatching.h"


// BCIDs of the abort gap
const int ABORT_GAP_START = 3446;
const int ABORT_GAP_END   = 3563;


/*---------------------------------------------------------*/
LArNoiseCorrelationMon::LArNoiseCorrelationMon(const std::string& type, 
			 const std::string& name,
			 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent), 
    m_strbisHelper(0),
    m_strHelper(0),
    m_LArOnlineIDHelper(0),
    m_LArEM_IDHelper(0),
    m_badChannelMask("BadLArRawChannelMask"),
    //m_summary(0),
    m_feedthroughID(0),
    m_slot(0),
    m_feedthrough(0),
    m_febHash(0),
    m_channel(0),
    m_PercComputed(false),
    m_Samplenbr(0),
    m_eventsCounter(0),
    m_evtId(0),
    m_thisTrigDecTool("Trig::TrigDecisionTool/TrigDecisionTool")
{	
   /**bool use to mask the bad channels*/
  declareProperty("IgnoreBadChannels", m_ignoreKnownBadChannels=false);
  declareProperty("LArBadChannelMask",m_badChannelMask);
  declareProperty("LArPedestalKey", m_larPedestalKey="Pedestal");
  declareProperty("LArDigitContainerKey", m_LArDigitContainerKey = "FREE");
  /**default cut to select events*/
  //  declareProperty("SigmaCut",               m_SigmaCut=5);
  /**default saturation cuts*/
  //declareProperty("ADCSatureCut",       m_ADCsatureCut=4095);
  
  /**Range to check for the max sample. If min and max=0, the range is set dynamically	*/
  //declareProperty("SampleRangeLow",       m_SampleRangeLow=0);     
  //declareProperty("SampleRangeUp",        m_SampleRangeUp=0);
  
  /**  Allow to compute error on histograms, default is false  */
  //declareProperty("ComputeHistError",m_ComputeHistError=false);
  /** Switch to online/offline mode*/
  declareProperty("IsOnline",            m_IsOnline=false);
  
  /** Give the name of the streams you want to monitor:*/
  //declareProperty("Streams",m_streams);
  
  /** Use the SampleMax expected and the SampleNumber from DB*/
  //declareProperty("ExpectedSampleMax",m_ExpectedSampleMax=0);
  //declareProperty("SampleNumberFromDB",m_SampleNumberFromDB=0);
  
  /** Number of Bad FEBs used to declare an event noisy and exclude it from monitoring*/
  //declareProperty("NumberBadFebs",      m_NumberBadFebs=5); 
  
  /** Treshold to declare a bad event*/
  /*declareProperty("TreshOut",m_TreshOut=5);
  declareProperty("TreshSat",m_TreshSat=5);
  declareProperty("TreshNull",m_TreshNull=5);
  */
  //  declareProperty("TrigDecisionTool", m_trigDecTool );
}

/*---------------------------------------------------------*/
LArNoiseCorrelationMon::~LArNoiseCorrelationMon()
{
}

StatusCode 
LArNoiseCorrelationMon::finalize()
{
  ATH_MSG_INFO( "finalHists LArNoiseCorrelationMon" );
  /*  DeleteHist(m_BarrelA);
  DeleteHist(m_BarrelC);
  DeleteHist(m_EmecA);
  DeleteHist(m_EmecC);
  DeleteHist(m_HecA);
  DeleteHist(m_HecC);
  DeleteHist(m_FcalA);
  DeleteHist(m_FcalC);
  */

  if(m_IsOnline)
    {
      if(m_av)
	{
	  LWHist::safeDelete(m_av);
	  m_av=0;
	} 
      if(m_TMP_sums)
	{
	  LWHist::safeDelete(m_TMP_sums);
	  m_TMP_sums=0;
	} 
    }
  return StatusCode::SUCCESS;
 
}

/*---------------------------------------------------------*/
StatusCode 
LArNoiseCorrelationMon::initialize()
{
  
  ATH_MSG_INFO( "Initialize LArNoiseCorrelationMon" );
  
  StatusCode sc;
  
  /** Get LAr Online Id Helper*/
  if ( detStore()->retrieve( m_LArOnlineIDHelper, "LArOnlineID" ).isSuccess() ) {
    
    ATH_MSG_DEBUG("connected non-tool: LArOnlineID" );
    
  } else {
    
    ATH_MSG_FATAL( "unable to connect non-tool: LArOnlineID" );
    return StatusCode::FAILURE;
    
  }
  
  /*
  // Get LArEM Id Helper, not used now...
  if ( detStore()->retrieve( m_LArEM_IDHelper, "LArEM_ID" ).isSuccess() ) {
    
    ATH_MSG_DEBUG("connected non-tool: LArEM_ID" );
    
  } else {
    
    ATH_MSG_FATAL( "unable to connect non-tool: LArEM_ID" );
    return StatusCode::FAILURE;
    
  }
  */
  
  /** Get LAr Calbling Service*/
  sc=m_LArCablingService.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve LArCablingService" );
    return StatusCode::FAILURE;
  }
  
  /** Get bad-channel mask (only if jO IgnoreBadChannels is true)*/
  if (m_ignoreKnownBadChannels) { 
    sc=m_badChannelMask.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not retrieve BadChannelMask" << m_badChannelMask);
      return StatusCode::FAILURE;
    }
  }
  
  /** Retrieve pedestals container*/
  sc =  detStore()->regHandle(m_larPedestal,m_larPedestalKey);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "could not register handle for pedestal " );
    return StatusCode::FAILURE;
  }
  
  /** Bool used for online*/
  m_PercComputed=false;
  
  /** get trigger decision tool*/ //TO BE ADJUSTED, TEMPORARY 
    if( !m_thisTrigDecTool.empty() ) {
      ATH_CHECK( m_thisTrigDecTool.retrieve() );
     ATH_MSG_INFO ( "  --> Found AlgTool TrigDecisionTool" );
     }


  return ManagedMonitorToolBase::initialize();
}

/*---------------------------------------------------------*/
StatusCode 
LArNoiseCorrelationMon::bookHistograms()
{
    
    /** Reset event counter*/
    m_eventsCounter=0;

    /** lar noise correlation: for now, single group with one histogram */
    MonGroup generalGroup( this, "/LAr/NoiseCorrel", run, ATTRIB_MANAGED );
    std::string  hName = "test";
    std::string hTitle = "LAr Noise Correlation test";
    
    Nchan=128;
    double chan_low=-0.5,chan_up=127.5;
    m_corr = TH2F_LW::create(hName.c_str(), hTitle.c_str(),Nchan,chan_low,chan_up,Nchan,chan_low,chan_up);
    generalGroup.regHist(m_corr).ignore();
    m_TMP_sums = TH2F_LW::create((hName+"_TMP_sum").c_str(),(hTitle+" TMP sum").c_str(),Nchan,chan_low,chan_up,Nchan,chan_low,chan_up);
    generalGroup.regHist(m_TMP_sums).ignore();
    m_av = TProfile_LW::create((hName+"_TMP_av").c_str(),(hTitle+" TMP av").c_str(),Nchan,chan_low,chan_up,"s");
    generalGroup.regHist(m_av).ignore();

    /**Book Histograms of Barrel. to be added (?) see LArDigit (what this was copied from) to copy the template*/
    /*    MonGroup GroupBarrelShift( this, "/LAr/Digits/Barrel", run, ATTRIB_MANAGED );
    MonGroup GroupBarrelExpert( this, "/LAr/Digits/Barrel", run, ATTRIB_MANAGED );
    MonGroup GroupBarrelExpertEff( this, "/LAr/Digits/Barrel", run, ATTRIB_MANAGED ,"","weightedEff");
    
    LArNoiseCorrelationMon::BookPartitions(m_BarrelA,"BarrelA",GroupBarrelShift,GroupBarrelExpert,GroupBarrelExpertEff);
    LArNoiseCorrelationMon::BookPartitions(m_BarrelC,"BarrelC",GroupBarrelShift,GroupBarrelExpert,GroupBarrelExpertEff);
    */

    /**Book Histogram of EMEC*/
    /*    MonGroup GroupEMECShift( this, "/LAr/Digits/EMEC", run, ATTRIB_MANAGED );
    MonGroup GroupEMECExpert( this, "/LAr/Digits/EMEC", run, ATTRIB_MANAGED );
    MonGroup GroupEMECExpertEff( this, "/LAr/Digits/EMEC", run, ATTRIB_MANAGED,"","weightedEff");
    
    LArNoiseCorrelationMon::BookPartitions(m_EmecA,"EmecA",GroupEMECShift,GroupEMECExpert,GroupEMECExpertEff);
    LArNoiseCorrelationMon::BookPartitions(m_EmecC,"EmecC",GroupEMECShift,GroupEMECExpert,GroupEMECExpertEff);
    */
    /**Book Histogram of HEC*/
    /*   MonGroup GroupHECShift( this, "/LAr/Digits/HEC", run, ATTRIB_MANAGED );
    MonGroup GroupHECExpert( this, "/LAr/Digits/HEC", run, ATTRIB_MANAGED );
    MonGroup GroupHECExpertEff( this, "/LAr/Digits/HEC", run, ATTRIB_MANAGED,"","weightedEff");
    
    LArNoiseCorrelationMon::BookPartitions(m_HecA,"HecA",GroupHECShift,GroupHECExpert,GroupHECExpertEff);
    LArNoiseCorrelationMon::BookPartitions(m_HecC,"HecC",GroupHECShift,GroupHECExpert,GroupHECExpertEff);
    */
    /**Book Histogram of FCAL.*/
    /*   MonGroup GroupFCALShift( this, "/LAr/Digits/FCAL", run, ATTRIB_MANAGED );
    MonGroup GroupFCALExpert( this, "/LAr/Digits/FCAL", run, ATTRIB_MANAGED );
    MonGroup GroupFCALExpertEff( this, "/LAr/Digits/FCAL", run, ATTRIB_MANAGED,"","weightedEff");
    
    LArNoiseCorrelationMon::BookPartitions(m_FcalA,"FcalA",GroupFCALShift,GroupFCALExpert,GroupFCALExpertEff);
    LArNoiseCorrelationMon::BookPartitions(m_FcalC,"FcalC",GroupFCALShift,GroupFCALExpert,GroupFCALExpertEff);
    */
    
    /**Book summary histo*/
    /*MonGroup generalGroup( this, "/LAr/Digits", run, ATTRIB_MANAGED );
    const char *  hName = "summary";
    const char * hTitle = "LArDigit Summary";
    
    m_summary = TH2F_LW::create(hName, hTitle,4,0.,4.,8,0.,8.);
    m_summary->GetXaxis()->SetTitle("Status");
    m_summary->GetYaxis()->SetTitle("Partition");
    m_summary->GetYaxis()->SetBinLabel(1,"EMBC");
    m_summary->GetYaxis()->SetBinLabel(2,"EMBA");
    m_summary->GetYaxis()->SetBinLabel(3,"EMECC");
    m_summary->GetYaxis()->SetBinLabel(4,"EMECA");
    m_summary->GetYaxis()->SetBinLabel(5,"HECC");
    m_summary->GetYaxis()->SetBinLabel(6,"HECA");
    m_summary->GetYaxis()->SetBinLabel(7,"FCalC");
    m_summary->GetYaxis()->SetBinLabel(8,"FCalA");
    m_summary->GetXaxis()->SetBinLabel(1,"OutOfRange");
    m_summary->GetXaxis()->SetBinLabel(2,"Saturation");
    m_summary->GetXaxis()->SetBinLabel(3,"Null Digits");  
    m_summary->GetXaxis()->SetBinLabel(4,"Mean Time");    
    m_summary->GetXaxis()->SetLabelSize(0.055);
    m_summary->GetYaxis()->SetLabelSize(0.055);            
    generalGroup.regHist(m_summary).ignore();

    hName = "summaryGain";
    hTitle = "Gain per partition (only cells with LArDigit available)";    
    m_summaryGain = TH2F_LW::create(hName, hTitle,3,0.,3.,8,0.,8.);
    m_summaryGain->GetXaxis()->SetTitle("Gain");
    m_summaryGain->GetYaxis()->SetTitle("Sub Detector");
    m_summaryGain->GetYaxis()->SetTitle("Partition");
    m_summaryGain->GetYaxis()->SetBinLabel(1,"EMBC");
    m_summaryGain->GetYaxis()->SetBinLabel(2,"EMBA");
    m_summaryGain->GetYaxis()->SetBinLabel(3,"EMECC");
    m_summaryGain->GetYaxis()->SetBinLabel(4,"EMECA");
    m_summaryGain->GetYaxis()->SetBinLabel(5,"HECC");
    m_summaryGain->GetYaxis()->SetBinLabel(6,"HECA");
    m_summaryGain->GetYaxis()->SetBinLabel(7,"FCalC");
    m_summaryGain->GetYaxis()->SetBinLabel(8,"FCalA");
    m_summaryGain->GetXaxis()->SetBinLabel(1,"HIGH");
    m_summaryGain->GetXaxis()->SetBinLabel(2,"MEDIUM");
    m_summaryGain->GetXaxis()->SetBinLabel(3,"LOW");  
    m_summaryGain->GetXaxis()->SetLabelSize(0.055);
    m_summaryGain->GetYaxis()->SetLabelSize(0.055);            
    generalGroup.regHist(m_summaryGain).ignore();
    */
    //}
  
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode 
LArNoiseCorrelationMon::fillHistograms()
{
  
  ATH_MSG_DEBUG("in fillHists()" );
  
  
  /** Increment event counter */
  m_eventsCounter++;

  /**EventID is a part of EventInfo, search event informations:*/
  //  unsigned long run=0;
  const xAOD::EventInfo* thisEvent;
  if (evtStore()->retrieve(thisEvent).isFailure()) {
    ATH_MSG_ERROR( "Failed to retrieve EventInfo object" );
    return StatusCode::FAILURE;
  }
  
  m_evtId = thisEvent->eventNumber();
  //  unsigned l1Trig = thisEvent->level1TriggerType();
  ATH_MSG_DEBUG("Event nb " << m_evtId );
  unsigned LBN = thisEvent->lumiBlock(); //FORSE SERVE
  ATH_MSG_DEBUG("LB id: " << LBN );
  

  /** check trigger */
  bool passTrig = false; 
  if ( (m_triggerChainProp != "")  && (!m_thisTrigDecTool.empty()) ) {
    passTrig = m_thisTrigDecTool->isPassed(m_triggerChainProp);
    if (m_triggerChainProp == "HLT_noalg_cosmiccalo_L1RD1_EMPTY") passTrig=(passTrig && thisEvent->bcid() >= ABORT_GAP_START && thisEvent->bcid() <= ABORT_GAP_END); //need to manually select abort gap for this trigger
  }
  if (!passTrig) {
    ATH_MSG_DEBUG ( " Failed trigger selection " );
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_DEBUG ( " Pass trigger selection " );
  }



  /** retrieve LArDigitContainer*/
  const LArDigitContainer* pLArDigitContainer;
  StatusCode sc = evtStore()->retrieve(pLArDigitContainer, m_LArDigitContainerKey);
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Can\'t retrieve LArDigitContainer with key " 
		      << m_LArDigitContainerKey );
    return StatusCode::SUCCESS;
  }
  
  
  /** Define iterators to loop over Digits containers*/
  LArDigitContainer::const_iterator itDig = pLArDigitContainer->begin(); 
  LArDigitContainer::const_iterator itDig_2;
  LArDigitContainer::const_iterator itDig_e= pLArDigitContainer->end(); 
  const LArDigit* pLArDigit;
  const LArDigit* pLArDigit2;
  
  
  
  /** First event : define range to look for max sample. 
      If not set by user, depends on number of samples*/
  
  /*
  if(m_eventsCounter==1)
  {	
    if(m_SampleNumberFromDB!=0) 
    {
      m_Samplenbr=m_SampleNumberFromDB;
      ATH_MSG_DEBUG("Retrieve nSamples from DB" );
    }
    
    else 
    {
      if (itDig!=itDig_e) 
      {
	pLArDigit = *itDig;
	m_Samplenbr=pLArDigit->nsamples();      
	ATH_MSG_DEBUG("Retrieve nSamples from first event" );
      }
    }
    
    if(m_SampleRangeLow == m_SampleRangeUp)
    {
      if(m_ExpectedSampleMax!=0)
      {
	m_SampleRangeLow=m_ExpectedSampleMax-1;
	m_SampleRangeUp=m_ExpectedSampleMax+1;
	ATH_MSG_DEBUG("Set Range number according to DB" );
      }
      
      else
      {
	ATH_MSG_DEBUG("Set range by hand " );
	/** Set range to look for the maximum*/
  /*	if(m_Samplenbr == 5)
	{
	  m_SampleRangeLow=2;
	  m_SampleRangeUp=4;	  
	}
	
	if(m_Samplenbr == 7)
	{
	  m_SampleRangeLow=3;//nota has to be checked in real run, none are available now...
	  m_SampleRangeUp=5;
	}
	
	if(m_Samplenbr == 10)
	{
	  m_SampleRangeLow=4;
	  m_SampleRangeUp=6;
	}
	
	if(m_Samplenbr == 32)
	{
	  m_SampleRangeLow=5;
	  m_SampleRangeUp=9;
	}
	
	/** Now if we have an exotic configuration...*/
  /*	if(m_Samplenbr < 10&&m_Samplenbr!=5&&m_Samplenbr!=7)
	{
	  m_SampleRangeLow=2;//default is 4,7
	  m_SampleRangeUp=4;
	}
	
	if(m_Samplenbr <= 20&&m_Samplenbr>10)
	{
	  m_SampleRangeLow=4;//default is 4,7
	  m_SampleRangeUp=7;
	}
	
	if(m_Samplenbr > 20&&m_Samplenbr!=32)
	{
	  m_SampleRangeLow=5;//default is 4,7
	  m_SampleRangeUp=9;
	}
      } 
    }
    
    else ATH_MSG_DEBUG("Range has been set by user" );
  */

    /** Now set histos titles, in order that it contains the range*/
  /*    LArNoiseCorrelationMon::HistTitleSum(m_summary);    
    LArNoiseCorrelationMon::OutHistTitle(m_BarrelA);
    LArNoiseCorrelationMon::OutHistTitle(m_BarrelC); 
    LArNoiseCorrelationMon::OutHistTitle(m_EmecA);
    LArNoiseCorrelationMon::OutHistTitle(m_EmecC);
    LArNoiseCorrelationMon::OutHistTitle(m_HecA);
    LArNoiseCorrelationMon::OutHistTitle(m_HecC);    
    LArNoiseCorrelationMon::OutHistTitle(m_FcalA);
    LArNoiseCorrelationMon::OutHistTitle(m_FcalC);
  }//Range and sample max are known now....
  */
  
  
  
  /** Loop over digits*/
  for ( ; itDig!=itDig_e;++itDig) {
    pLArDigit = *itDig;
    
    /** Retrieve pedestals */
    HWIdentifier id = pLArDigit->hardwareID();
    CaloGain::CaloGain gain = pLArDigit->gain();
    float pedestal = m_larPedestal->pedestal(id,gain);
    //float pedestalRMS= m_larPedestal->pedestalRMS(id,gain);
    
    if(!isGoodChannel(id,pedestal))
	continue;
    
    /** Determine to which partition this channel belongs to*/
    //    LArNoiseCorrelationMon::partition &ThisPartition=WhatPartition(id);

    /** Fill the gain*/
    //m_summaryGain->Fill(gain,ThisPartition.sumpos);
    
    
    /** Retrieve samples*/
    const std::vector<short>* digito = &pLArDigit->samples();
    
    /**retrieve the max sample digit ie digitot.back().*/
    /*std::vector<short>::const_iterator maxSam = std::max_element(digito->begin(), digito->end());
    std::vector<short>::const_iterator minSam = std::min_element(digito->begin(), digito->end());
    int maxPos=(maxSam-digito->begin());*/

    
    /** Retrieve once, all the cell info:*/
    m_feedthroughID = m_LArOnlineIDHelper->feedthrough_Id(id);
    m_channel = m_LArOnlineIDHelper->channel(id);
    m_slot = m_LArOnlineIDHelper->slot(m_LArOnlineIDHelper->feb_Id(id));
    m_feedthrough = m_LArOnlineIDHelper->feedthrough(m_LArOnlineIDHelper->feedthrough_Id(id));
    m_febHash = m_LArOnlineIDHelper->feb_Hash(m_LArOnlineIDHelper->feb_Id(id));
    m_febID = m_LArOnlineIDHelper->feb_Id(id);
    m_ch1 = m_LArOnlineIDHelper->channel(id);

    /** if event pass the sigma cut:*/
    /*if((*maxSam-pedestal)>pedestalRMS*m_SigmaCut)
    {
      /** Loop over the samples and get signal shape:*/
    /*for(int i=0;i<pLArDigit->nsamples();i++,iterSam++)FillSignShape(ThisPartition,i,(*iterSam-pedestal),(*maxSam-pedestal));
      
      /** Then fill histo about max sample:*/
    /*FillAverMaxDig(ThisPartition,maxPos,(*maxSam-pedestal),l1Trig,LBN);
      
      /** Finally fill histo out of range:*/
    /*if(!(maxPos>=m_SampleRangeLow&&maxPos<=m_SampleRangeUp))FillOutOfRange(ThisPartition);
    }
    
    /**Second monitoring of the saturating cells.*/
    /*if ((*maxSam)>=m_ADCsatureCut && (gain==0 || gain==1))FillSaturation(ThisPartition);
    
    /**Third monitoring cells containing one sample==0.*/
    /*if((*minSam)==0)FillNullHisto(ThisPartition);
     */


    /** check if this is the FEB we want DA AGGIUSTARE*/
    //    ATH_MSG_INFO( Form(":) %d && %d && %d && %d",m_LArOnlineIDHelper->isEMBchannel(id),m_LArOnlineIDHelper->pos_neg(id),m_feedthrough,m_slot) );
    if(!(m_LArOnlineIDHelper->isEMBchannel(id) && m_LArOnlineIDHelper->pos_neg(id)==1 && m_feedthrough==9 && m_slot==2))
	continue;
    //    ATH_MSG_INFO( Form(":) first channel: %d",m_ch1) );

    /** HERE GOES THE SECOND LOOP */
    bool av_set=false;
    for(itDig_2 = itDig; itDig_2!=itDig_e;++itDig_2)
      {
	pLArDigit2 = *itDig_2;
	HWIdentifier id2 = pLArDigit2->hardwareID();
	if(m_LArOnlineIDHelper->feb_Id(id2)!=m_febID)
	  continue;
	//	ATH_MSG_INFO( ":) second digi loop: right FEB" );
	/** get the pedestal */
	CaloGain::CaloGain gain2 = pLArDigit2->gain();
	float pedestal2 = m_larPedestal->pedestal(id2,gain2);

	if(!isGoodChannel(id2,pedestal2))
	  continue;

	/** get the channel number */
	m_ch2 = m_LArOnlineIDHelper->channel(id2);
	//	ATH_MSG_INFO( Form(":) second channe: %d",m_ch2) );

	/** get the samples */
	const std::vector<short>* digito2 = &pLArDigit2->samples();
	std::vector<short>::const_iterator iterSam2= digito2->begin();
	std::vector<short>::const_iterator iterSam= digito->begin();

	double part_sum=0;
	int Nsam=pLArDigit->nsamples();
	if(pLArDigit2->nsamples()!=Nsam)
	  {
	    ATH_MSG_INFO( Form(":)b Different number of samples: %d vs %d skipping these two",Nsam,pLArDigit2->nsamples()) );
	    continue; //ATTENZIONE A QUESTO COSO
	  }
	/** Loop over the samples and compute what we need:*/
	for(int i=0;i<Nsam;i++,iterSam++,iterSam2++)
	  {
	    if(!av_set) /** feed in the mean only once for channel 1 */
	      {
		m_av->Fill(m_ch1,(*iterSam-pedestal));
	      }
	    /** now compute sum of squares */
	    part_sum+=((*iterSam-pedestal)*(*iterSam2-pedestal2));
	  }
	av_set=true; /** now the average is set and I won't do this again in next ch2 loop*/
	m_TMP_sums->Fill(m_ch1,m_ch2,part_sum);
	m_TMP_sums->Fill(m_ch2,m_ch1,part_sum);
	//	std::cout << Form(":) Filled h_%d,%d: %f",m_ch1,m_ch2,part_sum) << std::endl;
      }/** End of second loop on LArDigit*/

    
  }/** End of loop on LArDigit*/
 

  /** update the published plot once per LB */
  if(endOfLumiBlockFlag() || endOfEventsBlockFlag())
    {
      double mean1,mean2;
      double sigma1,sigma2,sigma1ii,sigma2ii;
      double sumVar1,sumVar2;
      double N;
      double cor;
      for(int i=1;i<=Nchan;i++)
	{
	  mean1=m_av->GetBinContent(i);
	  sigma1ii=m_av->GetBinError(i);
	  sigma1=TMath::Sqrt(m_TMP_sums->GetBinContent(i,i)/(N));
	  sumVar1=m_TMP_sums->GetBinContent(i,i);
	  N=m_av->GetBinEntries(i);
	  if(N==0) 
	    {
	      std::cout << "Bin " << i << " has N=0" << std::endl;
	      continue;
	    }
	  for(int j=i+1;j<=Nchan;j++)
	    {
	      std::cout << "looking at bin: " << i << "-" << j << std::endl;
	      if(m_av->GetBinEntries(j)!=N)
		std::cout << "HEY! different number of entries here! bin " << i << ": " << N << " vs bin " << j << ": " << m_av->GetBinEntries(j) << std::endl;
		//		ATH_MSG_INFO( Form("HEY! different number of entries here! bin %d: %d vs bin %d: %f",i,N,j,m_av->GetBinEntries(j)) );
	      mean2=m_av->GetBinContent(j);
	      sigma2ii=m_av->GetBinError(j);
	      sigma2=TMath::Sqrt(m_TMP_sums->GetBinContent(j,j)/(N));
	      sumVar2=m_TMP_sums->GetBinContent(j,j);
	      if((sumVar1-N*mean1*mean1)*(sumVar2-N*mean2*mean2)==0) continue;
	      cor=(m_TMP_sums->GetBinContent(i,j)-N*mean1*mean2)/TMath::Sqrt((sumVar1-N*mean1*mean1)*(sumVar2-N*mean2*mean2));
	      //	      std::cout << Form(":) corr=%f TMP_sum(%d,%d)=%f, -Nm1m2=%f, sumVar1=%f, Nmean1mean1=%f, sumVar2=%f, Nmean2mean2=%f",cor,i,j,(m_TMP_sums->GetBinContent(i,j)),-1*N*mean1*mean2,sumVar1,N*mean1*mean1,sumVar2,N*mean2*mean2) << std::endl;
	      m_corr->SetBinContent(i,j,cor);
	      m_corr->SetBinContent(j,i,cor);
	    }
	}
    }
 
  /*  // Loop on all partition histograms to renormalise by the number of channels
  ScalePartition(m_BarrelA);
  ScalePartition(m_BarrelC);
  ScalePartition(m_EmecA);
  ScalePartition(m_EmecC);
  ScalePartition(m_HecA);
  ScalePartition(m_HecC);
  ScalePartition(m_FcalA);
  ScalePartition(m_FcalC);
  */
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode LArNoiseCorrelationMon::procHistograms()
{
  if(endOfRunFlag()) 
   {
     myEndOfRun();
   }
  /** Don't do anything*/
  /*
  if(endOfRunFlag() || endOfEventsBlockFlag() )
  {
    FillSumary(m_BarrelA);
    FillSumary(m_BarrelC);
    FillSumary(m_EmecA);
    FillSumary(m_EmecC);
    FillSumary(m_HecA);
    FillSumary(m_HecC);
    FillSumary(m_FcalA);
    FillSumary(m_FcalC);
    /** Properly Delete the LW hists*/
  /*if(endOfRunFlag()){
      EndOfRun(m_BarrelA);
      EndOfRun(m_BarrelC);
      EndOfRun(m_EmecA);
      EndOfRun(m_EmecC);
      EndOfRun(m_HecA);
      EndOfRun(m_HecC);
      EndOfRun(m_FcalA);
      EndOfRun(m_FcalC);
    }
    
    return StatusCode::SUCCESS;
  }
    */
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
/** check if channel is ok for monitoring */
 bool LArNoiseCorrelationMon::isGoodChannel(HWIdentifier ID,float ped)
 {

    /** Remove problematic channels*/
   if (m_ignoreKnownBadChannels && m_badChannelMask->cellShouldBeMasked(ID))
     return false;

    /**skip cells with no pedestals reference in db.*/
    if(ped <= (1.0+LArElecCalib::ERRORCODE))
      return false;
    
    /**skip disconnected channels:*/
    if(!m_LArCablingService->isOnlineConnected(ID))
      return false;

    return true;
 }

/*---------------------------------------------------------*/
/**Book and defines histogramms for a given partition.*/
void LArNoiseCorrelationMon::BookPartitions(partition& sub, const std::string& PartitionName,MonGroup& ShiftGroup, MonGroup& ExpertGroup, MonGroup& ExpertGroupEff)
  
{ 
  /**give a name to the partition*/
  sub.name=PartitionName;
  
  /**declare a partition number for the summary histogram:*/
  if(sub.name=="BarrelC") sub.sumpos=0;
  if(sub.name=="BarrelA") sub.sumpos=1;
  if(sub.name=="EmecC") sub.sumpos=2;
  if(sub.name=="EmecA") sub.sumpos=3;
  if(sub.name=="HecC") sub.sumpos=4;
  if(sub.name=="HecA" ) sub.sumpos=5;
  if(sub.name=="FcalC") sub.sumpos=6;
  if(sub.name=="FcalA") sub.sumpos=7;
  
  /**declare strings for histograms title*/
  m_strHelper = new LArOnlineIDStrHelper(m_LArOnlineIDHelper);
  m_strHelper->setDefaultNameType(LArOnlineIDStrHelper::LARONLINEID);
  
  std::string titlesat="% chan/FEB/events with max=4095 ADC - ";
  std::string titleout="% chan/FEB/events with max out of ";
  std::string titlemaxdi="Average position of Max Digit - ";
  std::string titlenull="% chan/FEB/events with min=0 ADC - ";
  
  std::string titlesatchan="% events with max=4095 ADC - ";
  std::string titleoutchan="% events with max out of ";
  std::string titlenullchan="% events with min=0 ADC - ";
  
  char cutadc[50];
  sprintf(cutadc," - Cut: %d #sigma ",m_SigmaCut);
  
  char expectedSamp[50];
  if(m_ExpectedSampleMax!=0) sprintf(expectedSamp," - Exp Max =%d ",m_ExpectedSampleMax);
  else sprintf(expectedSamp," - No Exp Max DB ");
  
  /**declare histograms properties
     1)for Endcaps*/
  int chan=128,FEB=25,slot=15,crates=375; 
  double chan_low=-0.5,chan_up=127.5,FEB_low=-0.5,FEB_up=24.5,slot_low=0.5,slot_up=15.5,crates_low=0.5,crates_up=375.5; 
  /**2)for Barrel*/
  if(sub.sumpos==0||sub.sumpos==1)
  {
    chan=128,FEB=32,slot=14,crates=448; 
    chan_low=-0.5,chan_up=127.5,FEB_low=-0.5,FEB_up=31.5,slot_low=0.5,slot_up=14.5,crates_low=0.5,crates_up=448.5; 
  }
  
  
  
  
  /** Book histograms*/

  // For 3 the types of errors (saturation, null, outOfRange), 3 histograms are booked but only 2 saved:
  // m_[Type]Digit : (FT,slot) histogram. Number of errors per FEB. Histograms not saved as it needs to be normalized
  // by the number of channels per FEB (128)
  // m_P[Type]Digit : (FT,slot) histogram. Derived by m_[Type]Digit after normalization by the number of channels
  // m_[Type]DigitChan : (FT+Slot; chan) histogram. Number of errors per channel.

  std::string  hName = "tSaturation_";
  hName =hName+PartitionName;
  std::string hTitle=titlesat+" Med/High Gain - "+PartitionName; 
  sub.m_SatDigit = TH2I_LW::create(hName.c_str(), hTitle.c_str(),slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  hName = "tSaturationLow_";
  hName =hName+PartitionName;
  hTitle=titlesat+" Low Gain - "+PartitionName; 
  sub.m_SatDigitLow = TH2I_LW::create(hName.c_str(), hTitle.c_str(),slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  hName = "Saturation_";
  hName =hName+PartitionName;
  hTitle=titlesat+PartitionName; 
  sub.m_PSatDigit = TProfile2D_LW::create(hName.c_str(),hTitle.c_str(),slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  ExpertGroup.regHist(sub.m_PSatDigit).ignore();
  m_strHelper->definePartitionSummProp(sub.m_PSatDigit).ignore();
  hName = "SaturationLow_";
  hName =hName+PartitionName;
  hTitle=titlesat+PartitionName; 
  sub.m_PSatDigitLow = TProfile2D_LW::create(hName.c_str(),hTitle.c_str(),slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  ExpertGroup.regHist(sub.m_PSatDigitLow).ignore();
  m_strHelper->definePartitionSummProp(sub.m_PSatDigitLow).ignore();
  
  hName = "tNullDigit_";
  hName =hName+PartitionName;
  hTitle=titlenull+PartitionName; 
  sub.m_NullDigit= TH2I_LW::create(hName.c_str(), hTitle.c_str(),slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  hName = "NullDigit_";
  hName =hName+PartitionName;
  hTitle=titlenull+PartitionName;
  sub.m_PNullDigit = TProfile2D_LW::create(hName.c_str(),hTitle.c_str(),slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  ExpertGroup.regHist(sub.m_PNullDigit).ignore();
  m_strHelper->definePartitionSummProp(sub.m_PNullDigit).ignore();
    
  hName = "tOutOfRange_";
  hName =hName+PartitionName;   
  hTitle=titleout;
  sub.m_OutDigit = TH2I_LW::create(hName.c_str(), hTitle.c_str(),slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  hName = "OutOfRange_";
  hName =hName+PartitionName;
  hTitle=titleout;
  sub.m_POutDigit = TProfile2D_LW::create(hName.c_str(),hTitle.c_str(),slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  ExpertGroup.regHist(sub.m_POutDigit).ignore();
  m_strHelper->definePartitionSummProp(sub.m_POutDigit).ignore();
  
  hName = "SaturationChan_";
  hName =hName+PartitionName;
  hTitle=titlesatchan+PartitionName+" - Med/High Gain - All Stream";
  sub.m_SatDigitChan = TH2F_LW::create(hName.c_str(), hTitle.c_str(),crates,crates_low,crates_up,chan,chan_low,chan_up);
  ExpertGroupEff.regHist(sub.m_SatDigitChan).ignore();
  m_strHelper->definePartitionSummProp2(sub.m_SatDigitChan).ignore();  
  hName = "SaturationChanLow_";
  hName =hName+PartitionName;
  hTitle=titlesatchan+PartitionName+" - Low Gain - All Stream";
  sub.m_SatDigitChanLow = TH2F_LW::create(hName.c_str(), hTitle.c_str(),crates,crates_low,crates_up,chan,chan_low,chan_up);
  ExpertGroupEff.regHist(sub.m_SatDigitChanLow).ignore();
  m_strHelper->definePartitionSummProp2(sub.m_SatDigitChanLow).ignore();  
  
  hName = "NullDigitChan_";
  hName =hName+PartitionName;
  hTitle=titlenullchan+PartitionName+" - All Gain - All Stream";
  sub.m_NullDigitChan = TH2F_LW::create(hName.c_str(), hTitle.c_str(),crates,crates_low,crates_up,chan,chan_low,chan_up);
  ExpertGroupEff.regHist(sub.m_NullDigitChan).ignore();
  m_strHelper->definePartitionSummProp2(sub.m_NullDigitChan).ignore();  
  
  hName = "OutDigitChan_";
  hName =hName+PartitionName;
  hTitle=titleoutchan;
  sub.m_OutDigitChan = TH2F_LW::create(hName.c_str(), hTitle.c_str(),crates,crates_low,crates_up,chan,chan_low,chan_up);
  ExpertGroupEff.regHist(sub.m_OutDigitChan).ignore();
  m_strHelper->definePartitionSummProp2(sub.m_OutDigitChan).ignore();  
  
  // Booking of histograms monitoring the highest sample position  
  hName = "AvePosMaxDig_";
  hName =hName+PartitionName;
  hTitle=titlemaxdi+PartitionName+cutadc+expectedSamp;
  sub.m_AverDig = TProfile2D_LW::create(hName.c_str(),hTitle.c_str(),slot,slot_low,slot_up,FEB,FEB_low,FEB_up);
  ExpertGroup.regHist(sub.m_AverDig).ignore();
  m_strHelper->definePartitionSummProp(sub.m_AverDig).ignore();
  
  
  
  hName = "EnVsTime_";
  hName =hName+PartitionName;
  hTitle=" Energy vs max sample - " +PartitionName+cutadc+expectedSamp;
  sub.m_EnTime = TH2F_LW::create(hName.c_str(),hTitle.c_str(),32,-0.5,31.5, 300, 0., 3000.);
  sub.m_EnTime->GetXaxis()->SetTitle("Sample Number");
  sub.m_EnTime->GetYaxis()->SetTitle("Energy [ADC]");
  ExpertGroup.regHist(sub.m_EnTime).ignore();
  
  
  hName = "SignShape_";
  hName =hName+PartitionName;
  hTitle=" Normalized Signal Shape - "+PartitionName+cutadc+expectedSamp;
  sub.m_SignShape = TProfile_LW::create(hName.c_str(),hTitle.c_str(),32,-0.5,31.5);
  sub.m_SignShape->GetXaxis()->SetTitle("Sample Number");
  sub.m_SignShape->GetYaxis()->SetTitle("Normalized Signal Shape");
  //Not supported in LWHists: sub.m_SignShape->GetYaxis()->SetTitleOffset(1.2);
  ExpertGroup.regHist(sub.m_SignShape).ignore();
  
  
  hName = "TriggerWord_";
  hName =hName+PartitionName;
  hTitle=" Position of max sample per L1 trigger word (8 bits) | "+PartitionName+cutadc+expectedSamp;  
  sub.m_TriggerType = TProfile_LW::create(hName.c_str(),hTitle.c_str(), 256, -0.5, 255.5);
  sub.m_TriggerType->GetXaxis()->SetTitle("L1 trigger word");
  sub.m_TriggerType->GetYaxis()->SetTitle("Position of max Sample");
  ExpertGroup.regHist(sub.m_TriggerType).ignore();
  
  hName = "MaxVsTime_";
  hName =hName+PartitionName;
  hTitle="Average Max Sample vs LumiBlock - "+PartitionName+expectedSamp;
  sub.m_MaxVsTime  = TProfile_LW::create(hName.c_str(),hTitle.c_str(), 3000, 0.5, 3000.5);
  sub.m_MaxVsTime->GetXaxis()->SetTitle("Luminosity Block");
  sub.m_MaxVsTime->GetYaxis()->SetTitle("Average Max Sample");
  ShiftGroup.regHist(sub.m_MaxVsTime).ignore(); 
  
  
  int nStreams = m_streams.size();
  hName = "MaxSample_PerStream_";
  hName =hName+PartitionName;
  hTitle="Position of the Max Sample - "+PartitionName+cutadc+expectedSamp;
  sub.m_AverTempPos_PerStream = TH2I_LW::create(hName.c_str(), hTitle.c_str(),32,-0.5,31.5,nStreams+1,-0.5,nStreams+0.5);
  sub.m_AverTempPos_PerStream->GetXaxis()->SetTitle("Sample Number");
  for (int str = 0; str < nStreams; str++) {
    sub.m_AverTempPos_PerStream->GetYaxis()->SetBinLabel(str+1,m_streams[str].c_str());     
  }
  sub.m_AverTempPos_PerStream->GetYaxis()->SetBinLabel(nStreams+1,"others");
  ShiftGroup.regHist(sub.m_AverTempPos_PerStream).ignore();
  
  // In online, we need to have a temp histogram to record the number of errors. And periodically, these histograms are dumped
  // in the regular histograms after normalisation by the current number of events.
  if(m_IsOnline)
  {
    sub.m_Temp_NullDigitChan=TH2I_LW::create(("temp_NullDigit_"+PartitionName).c_str(), "",crates,crates_low,crates_up,chan,chan_low,chan_up);
    sub.m_Temp_SatDigitChan=TH2I_LW::create(("temp_SatDigitChan_"+PartitionName).c_str(), "",crates,crates_low,crates_up,chan,chan_low,chan_up);
    sub.m_Temp_SatDigitChanLow=TH2I_LW::create(("temp_SatDigitChanLow_"+PartitionName).c_str(), "",crates,crates_low,crates_up,chan,chan_low,chan_up);
    sub.m_Temp_OutDigitChan=TH2I_LW::create(("temp_OutDigitChan_"+PartitionName).c_str(), "",crates,crates_low,crates_up,chan,chan_low,chan_up);
  }
  
  
  delete m_strHelper;
}


/*---------------------------------------------------------*/
/** Add ADCCut and range to this histogram name */
void LArNoiseCorrelationMon::HistTitle(LWHist2D* hist,partition& sub){
  std::string hTitle;
  char cutint[128];
  char buf[128];
  
  sprintf(buf,"[%d,%d] - ",m_SampleRangeLow,m_SampleRangeUp);
  
  if(m_ExpectedSampleMax!=0)sprintf(cutint," - Cut: %d #sigma - Exp Max=%d ",m_SigmaCut,m_ExpectedSampleMax);
  else sprintf(cutint," - Cut: %d #sigma - No Exp Max DB ",m_SigmaCut);
  
  hTitle=hist->GetTitle();
  hTitle=hTitle+buf+sub.name+cutint;
  hist->SetTitle(hTitle.c_str());
}
/*---------------------------------------------------------*/
/** Add ADCCut and range to this histogram name */
void LArNoiseCorrelationMon::HistTitle(TProfile2D_LW* hist,partition& sub){
  std::string hTitle;
  char cutint[128];
  char buf[128];
  
  sprintf(buf,"[%d,%d] - ",m_SampleRangeLow,m_SampleRangeUp);
  
  if(m_ExpectedSampleMax!=0)sprintf(cutint," - Cut: %d #sigma - Exp Max=%d ",m_SigmaCut,m_ExpectedSampleMax);
  else sprintf(cutint," - Cut: %d #sigma - No Exp Max DB",m_SigmaCut);
  
  hTitle=hist->GetTitle();
  hTitle=hTitle+buf+sub.name+cutint;
  hist->SetTitle(hTitle.c_str());
}

/*---------------------------------------------------------*/
/** Add ADCCut,range and ADCsature to this histogram name */
void LArNoiseCorrelationMon::HistTitleSum(LWHist2D* hist){
  std::string hTitle;
  char buf[128];
  if(m_ExpectedSampleMax!=0)sprintf(buf," - Cut: %d #sigma - range=[%d,%d] - Exp Max=%d ",m_SigmaCut,m_SampleRangeLow,m_SampleRangeUp,m_ExpectedSampleMax);
  else sprintf(buf," - Cut: %d #sigma - range=[%d,%d] - No Exp Max DB ",m_SigmaCut,m_SampleRangeLow,m_SampleRangeUp);
  
  hTitle=hist->GetTitle();
  hTitle=hTitle+buf;
  hist->SetTitle(hTitle.c_str());
} 
/*---------------------------------------------------------*/
void LArNoiseCorrelationMon::OutHistTitle(partition& sub)
{
  
  HistTitle(sub.m_POutDigit,sub);
  HistTitle(sub.m_OutDigit,sub);
  HistTitle(sub.m_OutDigitChan,sub);
}

/*---------------------------------------------------------*/
/** Say which partition is a channel
LArNoiseCorrelationMon::partition& LArNoiseCorrelationMon::WhatPartition(HWIdentifier id)
{
  
  /** return EM Barrel
    if (m_LArOnlineIDHelper->isEmBarrelOnline(id)) 
  {
    if((m_LArOnlineIDHelper->pos_neg(m_feedthroughID))==0) return m_BarrelC;
    else return m_BarrelA;
  }
  
  /** return EM Endcap
  else if (m_LArOnlineIDHelper-> isEMECchannel(id))
  {
    if((m_LArOnlineIDHelper->pos_neg(m_feedthroughID))==0) return m_EmecC;
    else return m_EmecA;
  }
  
  /** return HEC
  else if (m_LArOnlineIDHelper->isHECchannel(id)) 
  {
    if((m_LArOnlineIDHelper->pos_neg(m_feedthroughID))==0) return m_HecC;
    else return m_HecA;
  }
  
  /**  return FCAL
  else 
  {
    if((m_LArOnlineIDHelper->pos_neg(m_feedthroughID))==0) return m_FcalC;
    else return m_FcalA;
  }
  
  }*/

/*---------------------------------------------------------*/
/** Fill histograms if a channel is out of the given range*/
void LArNoiseCorrelationMon::FillOutOfRange(partition& sub)
{
  
  sub.m_OutDigit->Fill(m_slot,m_feedthrough);
  int numb=15;
  if(sub.sumpos==0||sub.sumpos==1) numb=14;
  if(m_IsOnline) sub.m_Temp_OutDigitChan->Fill(m_slot+(numb*m_feedthrough),m_channel);
  else sub.m_OutDigitChan->Fill(m_slot+(numb*m_feedthrough),m_channel); 
}

/*---------------------------------------------------------*/

void LArNoiseCorrelationMon::FillAverMaxDig(partition& sub, int& i,float nrj, unsigned int& l1Trig,unsigned int& lumiblock)
{
  /** Declare iterator to monitor:*/
  // std::vector<int>::const_iterator streamBeg=m_streamsThisEvent.begin();
  //std::vector<int>::const_iterator streamEnd=m_streamsThisEvent.end();
  //if(int(m_streamsThisEvent.size())!=0)for(;streamBeg!=streamEnd;++streamBeg){
  for (auto stream : m_streamsThisEvent) {
      sub.m_AverTempPos_PerStream->Fill(i,stream);
    }
  
  sub.m_AverDig->Fill(m_slot,m_feedthrough,i);
  sub.m_EnTime->Fill(i,(nrj));  
  sub.m_MaxVsTime->Fill(lumiblock,i);
  
  sub.m_TriggerType->Fill(l1Trig,i);
}
/*---------------------------------------------------------*/

void LArNoiseCorrelationMon::FillSignShape(partition& sub, int& i,float nrj,float sample_max)
{
  sub.m_SignShape->Fill(float(i),nrj/(sample_max),(sample_max));
}


/*---------------------------------------------------------*/
void LArNoiseCorrelationMon::FillSaturation(partition& sub)
{
  sub.m_SatDigit->Fill(m_slot,m_feedthrough);
  int numb=15;
  if(sub.sumpos==0||sub.sumpos==1) numb=14;
  if(m_IsOnline) sub.m_Temp_SatDigitChan->Fill(m_slot+(numb*m_feedthrough),m_channel);
  else sub.m_SatDigitChan->Fill(m_slot+(numb*m_feedthrough),m_channel); 
}
/*---------------------------------------------------------*/
void LArNoiseCorrelationMon::FillSaturationLow(partition& sub)
{
  sub.m_SatDigitLow->Fill(m_slot,m_feedthrough);
  int numb=15;
  if(sub.sumpos==0||sub.sumpos==1) numb=14;
  if(m_IsOnline) sub.m_Temp_SatDigitChanLow->Fill(m_slot+(numb*m_feedthrough),m_channel);
  else sub.m_SatDigitChanLow->Fill(m_slot+(numb*m_feedthrough),m_channel); 
}
/*---------------------------------------------------------*/
void LArNoiseCorrelationMon::FillNullHisto(partition& sub)
{
  sub.m_NullDigit->Fill(m_slot,m_feedthrough);
  int numb=15;
  if(sub.sumpos==0||sub.sumpos==1) numb=14;
  if(m_IsOnline) sub.m_Temp_NullDigitChan->Fill(m_slot+(numb*m_feedthrough),m_channel);
  else sub.m_NullDigitChan->Fill(m_slot+(numb*m_feedthrough),m_channel); 
  
}
/*---------------------------------------------------------*/
void LArNoiseCorrelationMon::FillSumary(partition& sub)
{
  //remove 0.9 because, bin are starting from 1 while sumpos starts at 0..........
  /*
  m_summary->SetBinContent(int(1),int(sub.sumpos+1),GetNumberCells(sub.m_POutDigit,m_TreshOut));
  m_summary->SetBinContent(2,sub.sumpos+1,GetNumberCells(sub.m_PSatDigit,m_TreshSat));
  m_summary->SetBinContent(3,sub.sumpos+1,GetNumberCells(sub.m_PNullDigit,m_TreshNull));
  m_summary->SetBinContent(4,sub.sumpos+1,GetMeanVal(sub.m_EnTime));*/
}
/*---------------------------------------------------------*/
void LArNoiseCorrelationMon::ScalePartition(partition& sub)
// 
{
  //DumpHisto(sub.m_NullDigit,sub.m_PNullDigit);
  //DumpHisto(sub.m_SatDigit,sub.m_PSatDigit);
  //DumpHisto(sub.m_OutDigit,sub.m_POutDigit);
  
  if(m_IsOnline){
    DumpOnlineHisto(sub.m_Temp_NullDigitChan,sub.m_NullDigitChan);
    DumpOnlineHisto(sub.m_Temp_SatDigitChan,sub.m_SatDigitChan);
    DumpOnlineHisto(sub.m_Temp_SatDigitChanLow,sub.m_SatDigitChanLow);
    DumpOnlineHisto(sub.m_Temp_OutDigitChan,sub.m_OutDigitChan);
  }
  // BT on 10/4/2015 : originally the DumpHisto was 4 lines before (see comments above).
  // I have the feeling that the online normalisation was wrong.
  DumpHisto(sub.m_NullDigit,sub.m_PNullDigit);
  DumpHisto(sub.m_SatDigit,sub.m_PSatDigit);
  DumpHisto(sub.m_SatDigitLow,sub.m_PSatDigitLow);
  DumpHisto(sub.m_OutDigit,sub.m_POutDigit);

  ATH_MSG_DEBUG("End of Scale Histograms " );
}

/*---------------------------------------------------------*/
/** Scale histograms to get percentage, compute error, and change the entrie number*/
void LArNoiseCorrelationMon::ScaleHisto(LWHist2D * hist,int& events)
{
  if (m_ComputeHistError)ComputeError(hist,events);
  if(events!=0)
  {
    hist->scaleContentsAndErrors(100./events);
    hist->SetEntries(events);
  }
}

/*---------------------------------------------------------*/
void LArNoiseCorrelationMon::ComputeError(LWHist2D* hist,int& events)
{
  int normFactor=events;
  if (normFactor == 0) return;
  double inv_normFactor2 = 1. / (static_cast<double> (normFactor) * static_cast<double> (normFactor));
  unsigned xbin, ybin;
  double numer, error;
  hist->resetActiveBinLoop();
  while(hist->getNextActiveBin(xbin,ybin,numer,error)) 
  {
    if(numer>normFactor)continue;//protection against sqrt(neg)
    if(numer>0)
    {
      float tabError = 100*sqrt(numer*normFactor*(normFactor-numer))*inv_normFactor2;
      hist->SetBinError(xbin,ybin,tabError);       
    }
  }
}
/*---------------------------------------------------------*/
void LArNoiseCorrelationMon::DumpHisto(LWHist2D* hist1,TProfile2D_LW* hist2)
// At the end of run/processing, renormalize the (FT,Slot) histograms
// by the number of FEB channels (128)
{
  unsigned xbin, ybin;
  
  hist1->resetActiveBinLoop();
  for (xbin=1;xbin<=hist1->GetNbinsX();xbin++){
    for (ybin=1;ybin<=hist1->GetNbinsY();ybin++){
      hist2->Fill(xbin,ybin-1.5,100.*(hist1->GetBinContent(xbin,ybin))/128);
    }
  }
  hist1->Reset();
}
/*---------------------------------------------------------*/
void LArNoiseCorrelationMon::DumpOnlineHisto(LWHist2D* hist1,LWHist2D* hist2)
{
  unsigned xbin, ybin;
  double numer, error;
  hist1->resetActiveBinLoop();
  
  while(hist1->getNextActiveBin(xbin,ybin,numer,error))
  {
    hist2->SetBinContentAndError(xbin,ybin,100*numer/m_eventsCounter,error);
  }
  hist2->SetEntries(m_eventsCounter);
}
/*---------------------------------------------------------*/
double LArNoiseCorrelationMon::GetMeanVal(LWHist2D* hist1)
{
  unsigned xbin, ybin,sum=0,compx,compy,sumx=0;
  double binsize;
  compx=hist1->GetNbinsX();
  compy=hist1->GetNbinsY();
  for (xbin=0;xbin<compx;xbin++){
  binsize=(hist1->getXMax()-hist1->getXMin())/hist1->GetNbinsX();
  sumx=0;
    for (ybin=0;ybin<compy;ybin++){
    sumx+=hist1->GetBinContent(xbin,ybin);
    }
  sum+=(sumx*binsize*(xbin-1));
  }
  if(hist1->GetEntries()!=0)  return double(sum)/double(hist1->GetEntries());
  else return 0;
}
/*---------------------------------------------------------*/
int LArNoiseCorrelationMon::GetNumberCells(TProfile2D_LW* hist1,double treshold)
{
  unsigned xbin, ybin,sum=0,compx,compy;
  compx=hist1->GetNbinsX();
  compy=hist1->GetNbinsY();
  for (xbin=0;xbin<compx;xbin++)
    for (ybin=0;ybin<compy;ybin++)
      if(hist1->GetBinContent(xbin,ybin)>treshold)sum++;
  
  return sum;
}

/*---------------------------------------------------------*/
void LArNoiseCorrelationMon::myEndOfRun()
{
  if(m_IsOnline)
    {
      if(m_av) m_av->Reset(); 
      if(m_TMP_sums) m_TMP_sums->Reset(); 
    }
}

/*---------------------------------------------------------*/
void LArNoiseCorrelationMon::EndOfRun(partition& sub)
// This method was formerly named DeleteHists.
// Renamed by BT on 10/4/2015 as it does more than this, e.g:
// Normalisation by the number of events.
{
  if(sub.m_OutDigit){
    sub.m_OutDigit->Reset();
  }
  
  if(sub.m_NullDigit){
    sub.m_NullDigit->Reset();
  }
  
  if(sub.m_SatDigit){
    sub.m_SatDigit->Reset() ;
  }  
  
  if(sub.m_SatDigitLow){
    sub.m_SatDigitLow->Reset() ;
  }  
  
  if(m_IsOnline)
  {
    if(sub.m_Temp_NullDigitChan){
      sub.m_Temp_NullDigitChan->Reset(); 
    } 
    
    if(sub.m_Temp_SatDigitChan){
      sub.m_Temp_SatDigitChan->Reset();
    } 
    
    if(sub.m_Temp_SatDigitChanLow){
      sub.m_Temp_SatDigitChanLow->Reset();
    } 

    if(sub.m_Temp_OutDigitChan){
      sub.m_Temp_OutDigitChan->Reset();
    } 
  }
  else{
    ScaleHisto(sub.m_NullDigitChan,m_eventsCounter);
    ScaleHisto(sub.m_SatDigitChan,m_eventsCounter);
    ScaleHisto(sub.m_SatDigitChanLow,m_eventsCounter);
    ScaleHisto(sub.m_OutDigitChan,m_eventsCounter);
  }
}


void LArNoiseCorrelationMon::DeleteHist(partition& sub)
// deleting local histograms 
{
  if(sub.m_OutDigit){
    LWHist::safeDelete(sub.m_OutDigit);
    sub.m_OutDigit=0;
  }
  
  if(sub.m_NullDigit){
    LWHist::safeDelete(sub.m_NullDigit);
    sub.m_NullDigit=0;
  }
  
  if(sub.m_SatDigit){
    LWHist::safeDelete(sub.m_SatDigit);
    sub.m_SatDigit=0;
  }  
  
  if(sub.m_SatDigitLow){
    LWHist::safeDelete(sub.m_SatDigitLow);
    sub.m_SatDigitLow=0;
  }  
  
  if(m_IsOnline)
  {
    if(sub.m_Temp_NullDigitChan){
      LWHist::safeDelete(sub.m_Temp_NullDigitChan);
      sub.m_Temp_NullDigitChan=0;
    } 
    
    if(sub.m_Temp_SatDigitChan){
      LWHist::safeDelete(sub.m_Temp_SatDigitChan);
      sub.m_Temp_SatDigitChan=0;
    } 
    
    if(sub.m_Temp_SatDigitChanLow){
      LWHist::safeDelete(sub.m_Temp_SatDigitChanLow);
      sub.m_Temp_SatDigitChanLow=0;
    } 

    if(sub.m_Temp_OutDigitChan){
      LWHist::safeDelete(sub.m_Temp_OutDigitChan);
      sub.m_Temp_OutDigitChan=0;
    } 
  }

}


