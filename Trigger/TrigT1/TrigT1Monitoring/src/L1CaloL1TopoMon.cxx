/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        L1CaloL1TopoMon.cxx
// PACKAGE:     TrigT1CaloMonitoring
//
// AUTHOR:      Joergen Sjoelin (sjolin@fysik.su.se)
//
// DESCRIPTION: Monitoring of L1Calo --> L1Topo transmission
//
// ********************************************************************

#include <map>
#include <utility>

#include <algorithm>
#include <vector>
#include <iomanip>
#include <sstream>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/FrontPanelCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

#include "xAODTrigL1Calo/CMXJetTobContainer.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_Decoder.h"
#include "TrigT1Result/RoIBResult.h"
#include "L1TopoRDO/L1TopoRDOCollection.h"
#include "L1TopoRDO/Helpers.h"
#include "L1TopoRDO/Header.h"
#include "L1TopoRDO/Fibre.h"
#include "L1TopoRDO/Status.h"
#include "L1TopoRDO/L1TopoTOB.h"
#include "L1TopoRDO/BlockTypes.h"
#include "L1CaloL1TopoMon.h"

// ============================================================================
// To be moved to L1TopoRDO Helpers.cxx
namespace L1Topo {
// ============================================================================
const std::string formatVecHex8(const std::vector<uint32_t>& vec)
{
  std::ostringstream s;
  s << "[ ";
  for (auto elem : vec) {
    s << std::hex << std::showbase << std::setfill('0') << std::setw(8)
      << elem << " " << std::dec << std::noshowbase;
  }
  s << "]";
  return s.str();
}
// ============================================================================
}  // end namespace
// ============================================================================

// ============================================================================
namespace LVL1 {
// ============================================================================

// *********************************************************************
// Public Methods
// *********************************************************************

/*---------------------------------------------------------*/
L1CaloL1TopoMon::L1CaloL1TopoMon( const std::string & type,
                                  const std::string & name,
                                  const IInterface* parent )
  : ManagedMonitorToolBase( type, name, parent ),
    m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_debug(false), m_histBooked(false),
    m_h_l1topo_1d_CMXTobs(0),
    m_h_l1topo_1d_Simulation(0),
    m_h_l1topo_1d_JetTobs_EnergyLg(0),
    m_h_l1topo_2d_JetTobs_Hitmap_mismatch(0),
    m_h_l1topo_2d_JetTobs_Hitmap_match(0),
    m_h_l1topo_1d_Errors(0),
    m_h_l1topo_1d_DAQTobs(0),
    m_h_l1topo_1d_DAQJetTobs(0),
    m_h_l1topo_1d_DAQTauTobs(0),
    m_h_l1topo_1d_DAQEMTobs(0),
    m_h_l1topo_1d_DAQMuonTobs(0),
    m_h_l1topo_1d_DAQTriggerBits(0),
    m_h_l1topo_1d_DAQMismatchTriggerBits(0),
    m_h_l1topo_1d_DAQOverflowBits(0),
    m_h_l1topo_1d_ROITobs(0)


    /*---------------------------------------------------------*/
{
  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file
  
  declareProperty( "TrigConfigSvc", m_configSvc, "Trig Config Service");
  declareProperty( "PathInRootFile",
		   m_PathInRootFile= "LVL1_Interfaces/L1Topo");
  declareProperty( "CMXJetTobLocation", m_CMXJetTobLocation
		   = LVL1::TrigT1CaloDefs::CMXJetTobLocation);
  declareProperty( "TopoCTPLocation", m_topoCTPLoc
		   = LVL1::DEFAULT_L1TopoCTPLocation, 
		   "StoreGate location of topo inputs" );
}
  
/*---------------------------------------------------------*/
L1CaloL1TopoMon::~L1CaloL1TopoMon()
/*---------------------------------------------------------*/
{
}
  
#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode L1CaloL1TopoMon::initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endreq;
  m_debug = msgLvl(MSG::DEBUG);

  StatusCode sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Couldn't connect to " << m_configSvc.typeAndName()
                    << endreq;
    return sc;
  } else {
    msg(MSG::INFO) << "Connected to " << m_configSvc.typeAndName() << endreq;
  }

  sc = m_errorTool.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool"
                  << endreq;
    return sc;
  }

  sc = m_histTool.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool"
                    << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloL1TopoMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{


  msg(MSG::DEBUG) << "in L1CaloL1TopoMon::bookHistograms" << endreq;

  if ( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  if ( newLumiBlock ) {}

  if ( newRun ) {

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    MonGroup L1CaloL1Topo( this, m_PathInRootFile, run, attr );
    MonGroup L1CaloL1TopoEvents( this, m_PathInRootFile, run, attr,
                                 "", "eventSample" );

    m_histTool->setMonGroup(&L1CaloL1Topo);

    m_h_l1topo_1d_Errors =
      m_histTool->book1F("l1topo_1d_Errors",
			 "L1Topo error summary",
			 ERROR_BIT::NUMBEROFBITS, 0, ERROR_BIT::NUMBEROFBITS);
    for (int i=0; i<ERROR_BIT::NUMBEROFBITS;++i)
      m_h_l1topo_1d_Errors->GetXaxis()->
	SetBinLabel(i+1,ERROR_LABELS[i].c_str());

  

    m_h_l1topo_1d_CMXTobs =
      m_histTool->book1F("l1topo_1d_CMXTobs",
			 "Number of CMX TOBs",
			 MAXTOBS, 0, MAXTOBS);

    m_h_l1topo_1d_Simulation =
      m_histTool->book1F("l1topo_1d_Simulation",
			 "Simulated L1Topo trigger bits", 128, 0, 128);

    m_h_l1topo_2d_JetTobs_Hitmap_mismatch = m_histTool->
      bookJEMCrateModuleVsFrameLoc("l1topo_2d_JetTobs_Hitmap_mismatch",
				   "Mismatched L1Topo-Jet TOBs Hit Map");
    m_h_l1topo_2d_JetTobs_Hitmap_match = m_histTool->
      bookJEMCrateModuleVsFrameLoc("l1topo_2d_JetTobs_Hitmap_match",
				   "CMX-matched L1Topo-Jet TOBs Hit Map");

    m_h_l1topo_1d_JetTobs_EnergyLg = m_histTool->
      book1F("l1topo_1d_JetTobs_EnergyLg",
	     "L1Topo-Jet TOB Energy Large Window Size", 256, 0., 1024);
    m_h_l1topo_1d_JetTobs_EnergyLg->SetOption("HIST");

    m_h_l1topo_1d_DAQTobs =
      m_histTool->book1F("l1topo_1d_DAQTobs",
                         "Number of L1Topo DAQ L1Topo TOBs",
                         20, 0, 20);
    m_h_l1topo_1d_DAQJetTobs =
      m_histTool->book1F("l1topo_1d_DAQJetTobs",
                         "Number of L1Topo DAQ Jet TOBs",
                         MAXTOBS, 0, MAXTOBS);
    m_h_l1topo_1d_DAQTauTobs =
      m_histTool->book1F("l1topo_1d_DAQTauTobs",
			 "Number of L1Topo DAQ Tau TOBs",
			 MAXTOBS, 0, MAXTOBS);
    m_h_l1topo_1d_DAQEMTobs =
      m_histTool->book1F("l1topo_1d_DAQEMTobs",
			 "Number of L1Topo DAQ EM TOBs",
			 MAXTOBS, 0, MAXTOBS);
    m_h_l1topo_1d_DAQMuonTobs =
      m_histTool->book1F("l1topo_1d_DAQMuonTobs",
			 "Number of L1Topo DAQ Muon TOBs",
			 MAXTOBS, 0, MAXTOBS);
    m_h_l1topo_1d_DAQTriggerBits =
      m_histTool->book1F("l1topo_1d_DAQTriggerBits",
			 "L1Topo DAQ trigger bits",
			 128, 0, 128);
    m_h_l1topo_1d_DAQMismatchTriggerBits =
      m_histTool->book1F("l1topo_1d_DAQMismatchTriggerBits",
			 "L1Topo DAQ-Simulation trigger bits mismatch",
			 128, 0, 128);
    m_h_l1topo_1d_DAQOverflowBits =
      m_histTool->book1F("l1topo_1d_DAQOverflowBits",
			 "L1Topo DAQ overflow bits",
			 128, 0, 128);
    m_h_l1topo_1d_ROITobs =
      m_histTool->book1F("l1topo_1d_ROITobs",
			 "Number of L1Topo ROI L1Topo TOBs",
			 20, 0, 20);
    for (int i=0; i<4; ++i) {
      const std::string textFPGA[4]=
	{"L1Topo_00_U1","L1Topo_00_U2","L1Topo_01_U1","L1Topo_01_U2"};
      m_h_l1topo_2d_ItemsBC[i] =
	m_histTool->bookTH2F(std::string("l1topo_2d_ItemsBC")+
			     std::to_string(i),
			     std::string("Bunch Crossing vs Algorithm Number ")+textFPGA[i],
			     32, i*32, (i+1)*32, 3, -1.5, 1.5);
    }

    // Get L1Topo output bit names from the LVL1 menu thresholds
    
    // KW COMMENTING OUT
    // LABELS ARE CROWDED AND ILLEGIBLE
    // ALPHANUMERIC LABELS BREAK TIER-0 MERGING (ATR-13604)

    /*std::map<unsigned int, std::string> topoCounterToName;
    const std::vector<TrigConf::TriggerThreshold*>& thrVec = 
      m_configSvc->thresholdConfig()->
      getThresholdVector(TrigConf::L1DataDef::TOPO);
    ATH_MSG_INFO("L1Topo bits found in the LVL1 menu = " << thrVec.size());
    
    if (thrVec.size()>0) {
      for (auto thr : thrVec) {
	if (thr) {
	  ATH_MSG_DEBUG("Looping L1Topo mapping " << thr->mapping());
	  ATH_MSG_DEBUG("Looping L1Topo name " << thr->name());
	  topoCounterToName.emplace(thr->mapping(),thr->name());
	}
	else
	  ATH_MSG_DEBUG("Looping L1Topo bit inactive");
      }
      ATH_MSG_INFO("L1Topo bits configured in the LVL1 menu = "
		   << topoCounterToName.size());
      for (unsigned int binIndex=0; binIndex<128; ++binIndex){ 
	auto it = topoCounterToName.find(binIndex);
	std::string label;
	if (it != topoCounterToName.end()){ // KW commenting out (labels are crowded & illegible)
	  label=it->second;
	}
	else {
	  label=std::to_string(binIndex);
          cout << (binIndex%32+1);
	}
	m_h_l1topo_2d_ItemsBC[binIndex/32]->GetXaxis()->
	  SetBinLabel(binIndex%32+1,label.c_str());
      }
    }*/

    m_histTool->unsetMonGroup();
    m_histBooked = true;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloL1TopoMon::fillHistograms()
/*---------------------------------------------------------*/
{

  if (m_debug) msg(MSG::DEBUG) << "in L1CaloL1TopoMon::fillHistograms"
			       << endreq;

  if (!m_histBooked) {
    if (m_debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode sc = StatusCode::SUCCESS;

  std::vector<xAOD::CMXJetTob*> cmxtobs;  

  // Validate properly unpacked input from L1Calo
  if (m_errorTool->corrupt() || m_errorTool->robOrUnpackingError()) {
    if (m_debug) msg(MSG::DEBUG) << "Corrupt L1Calo event" << endreq;
    m_h_l1topo_1d_Errors->Fill(CALO_CONV);
  }

  const DataHandle<CTP_RDO> ctpRDO = 0;
  sc = evtStore()->retrieve(ctpRDO,"CTP_RDO");
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Could not find CTP_RDO in StoreGate");
  }
  else {
    CTP_Decoder ctp;
    ctp.setRDO(ctpRDO);
    const uint16_t l1aPos = ctpRDO->getL1AcceptBunchPosition();
    const uint32_t bx=ctp.getBunchCrossings().size();
    if (l1aPos >= bx) {
      ATH_MSG_INFO( "CTP_RDO gave invalid l1aPos." );
    }
    else {
      ATH_MSG_DEBUG( "CTP l1aPos, size : " << l1aPos << ", " << bx );
      if ( bx>=3 && l1aPos>0) {
	for (int bc=-1; bc<2; ++bc) {
	  const CTP_BC& ctp_bc = ctp.getBunchCrossings().at(l1aPos+bc);
	  std::bitset<512> tip = ctp_bc.getTIP();
	  ATH_MSG_VERBOSE( "got CTP TIP bits: " << tip.to_string() );
	  const unsigned int topoTipStart(384);
	  const unsigned int nTopoCTPOutputs(128);
	  for (unsigned int item=0; item<nTopoCTPOutputs; ++item){
      int h = (int)item/32;
      int binx = m_h_l1topo_2d_ItemsBC[h]->GetXaxis()->FindBin(item);
      int biny = m_h_l1topo_2d_ItemsBC[h]->GetYaxis()->FindBin(bc);
	    if (item == 8 || item == 29 || item == 30 || item == 31 || item == 46 || item == 47 || 
           item == 48 || item == 49 || item == 50 || item == 51 || item == 52 || item == 53 || 
           item == 54 || item == 55 || item == 56 || item == 57 || item == 58 || item == 59 || 
           item == 60 || item == 61 || item == 62 || item == 63 || item == 88 || item == 89 || 
           item == 90 || item == 91 || item == 92 || item == 93 || item == 94 || item == 95 || 
           item == 107 || item == 108) { 
          m_h_l1topo_2d_ItemsBC[item/32]->SetBinContent(binx, biny,-200); //Sets bin value for algorithms not used in the run
          } 
      else {
           if (tip.test(item+topoTipStart)) { //Checks if algorithms fired on event-by-event basis
             m_h_l1topo_2d_ItemsBC[item/32]->Fill(item,bc);
              }
           }
        }
      }
    }
  }
}

  // Retrieve CMX tobs
  bool cmx_ematch=true;
  const DataHandle<xAOD::CMXJetTobContainer> cmxtob = 0;
  sc = evtStore()->retrieve(cmxtob);
  if (sc.isFailure() || !cmxtob) {
    ATH_MSG_DEBUG ("No CMX tobs found in TES");
    m_h_l1topo_1d_Errors->Fill(NO_CMX);
  }   
  else {
    ATH_MSG_DEBUG( "Found CMXJetTobCollection, looping on TOBs ..." );
    for (auto & tob : *cmxtob) {
      if (tob->energyLarge()) {
	cmxtobs.push_back(tob);
      }
    }
    m_h_l1topo_1d_CMXTobs->Fill(std::min((int)cmxtobs.size(),MAXTOBS-1));
    
  }

  // Retrieve L1Topo CTP simulted decision if present
  if (!evtStore()->contains<LVL1::FrontPanelCTP>(m_topoCTPLoc.value())){
    ATH_MSG_INFO("Could not retrieve LVL1::FrontPanelCTP with key "
		 << m_topoCTPLoc.value());
  }
  else {
    const DataHandle< LVL1::FrontPanelCTP > topoCTP;
    CHECK_RECOVERABLE(evtStore()->retrieve(topoCTP,m_topoCTPLoc.value()));
    if (!topoCTP){
      ATH_MSG_INFO( "Retrieve of LVL1::FrontPanelCTP failed." );
    }
    else {
      for(unsigned int i=0; i<32; ++i) {
	// old scheme
	//uint32_t mask = 0x1; mask <<= i;
	//if( (m_topoCTP->cableWord0(0) & mask) != 0 )
	//	m_h_l1topo_1d_Simulation->Fill(i); // cable 0, clock 0
	//if( (m_topoCTP->cableWord0(1) & mask) != 0 )
	//	m_h_l1topo_1d_Simulation->Fill(32 + i); // cable 0, clock 1
	//if( (m_topoCTP->cableWord1(0) & mask) != 0 )
	//	m_h_l1topo_1d_Simulation->Fill(64 + i); // cable 1, clock 0
	//if( (m_topoCTP->cableWord1(1) & mask) != 0 )
	//	m_h_l1topo_1d_Simulation->Fill(96 + i); // cable 1, clock 1
	uint64_t topores0=topoCTP->cableWord0(1);
	topores0<<=32;
	topores0+=topoCTP->cableWord0(0);
	uint64_t topores1=topoCTP->cableWord1(1);
	topores1<<=32;
	topores1+=topoCTP->cableWord1(0);
	if( (topores0 & (0x1UL << (2*i+0))))
	  m_h_l1topo_1d_Simulation->Fill(i); // cable 0, clock 0
	if( (topores0 & (0x1UL << (2*i+1))))
	  m_h_l1topo_1d_Simulation->Fill(32 + i); // cable 0, clock 1
	if( (topores1 & (0x1UL << (2*i+0))))
	  m_h_l1topo_1d_Simulation->Fill(64 + i); // cable 1, clock 0
	if( (topores1 & (0x1UL << (2*i+1))))
	  m_h_l1topo_1d_Simulation->Fill(96 + i); // cable 1, clock 1
      }
      ATH_MSG_DEBUG("Simulated output from L1Topo from StoreGate with key "
		    << m_topoCTPLoc);
      ATH_MSG_DEBUG("L1Topo word 1 at clock 0 is: 0x"
		    << std::hex << std::setw( 8 ) << std::setfill( '0' )
		    << topoCTP->cableWord0(0));
      ATH_MSG_DEBUG("L1Topo word 2 at clock 0 is: 0x"
		    << std::hex << std::setw( 8 ) << std::setfill( '0' )
		    << topoCTP->cableWord1(0));
      ATH_MSG_DEBUG("L1Topo word 1 at clock 1 is: 0x"
		    << std::hex << std::setw( 8 ) << std::setfill( '0' )
		    << topoCTP->cableWord0(1));
      ATH_MSG_DEBUG("L1Topo word 2 at clock 1 is: 0x"
		    << std::hex << std::setw( 8 ) << std::setfill( '0' )
		    << topoCTP->cableWord1(1));
    }       
  }

  // Retrieve the L1Topo RDOs from the DAQ RODs
  const int NFPGA=4;
  const DataHandle<L1TopoRDOCollection> rdos = 0;
  sc = evtStore()->retrieve(rdos);
  if (sc.isFailure() or 0 == rdos) {
    m_h_l1topo_1d_Errors->Fill(NO_DAQ);
    ATH_MSG_DEBUG ( "Could not retrieve L1Topo DAQ RDO collection "
		    "from StoreGate" );
  }
  else {
    // initialise collections filled for all blocks
    std::vector<L1Topo::L1TopoTOB> daqTobs;
    std::vector<uint32_t> daqJetTobs;
    std::vector<uint32_t> daqTauTobs;
    std::vector<uint32_t> daqEMTobs;
    std::vector<uint32_t> daqMuonTobs;
    std::vector<uint32_t> vFibreSizes;
    std::vector<uint32_t> vFibreStatus;
    
    for (auto & rdo : *rdos){
      ATH_MSG_VERBOSE( *rdo );
      ATH_MSG_DEBUG( "Found DAQ RDO with source ID "
                     << L1Topo::formatHex8(rdo->getSourceID()) );
      auto errors = rdo->getErrors();
      if (! errors.empty()){
	ATH_MSG_WARNING( "Converter errors reported: " << errors );
	m_h_l1topo_1d_Errors->Fill(DAQ_CONV);
      }
      const std::vector<uint32_t> cDataWords = rdo->getDataWords();

      if ( cDataWords.size() == 0 ) {
        ATH_MSG_DEBUG ( "L1TopoRDO DAQ is empty" );
	m_h_l1topo_1d_Errors->Fill(NO_DAQ);
      }

      // initialise header: beware, this can make a valid-looking header
      //   set version 15, BCN -7, which is unlikely:
      L1Topo::Header header(0xf, 0, 0, 0, 0, 1, 0x7);
      for (auto word : cDataWords){
	switch (L1Topo::blockType(word)){
	case L1Topo::BlockTypes::HEADER:
	  {
	    header = L1Topo::Header(word);
	    if (header.payload_crc()!=0) {
	      m_h_l1topo_1d_Errors->Fill(PAYL_CRC);
	    }
	    break;
	  }
	case L1Topo::BlockTypes::FIBRE:
	  {
	    auto fibreBlock = L1Topo::Fibre(word);
	    for (auto fsize: fibreBlock.count()){
	      vFibreSizes.push_back(fsize);
	    }
	    for (auto fstatus: fibreBlock.status()){
	      vFibreStatus.push_back(fstatus);
	    }
	    break;
	  }
	case L1Topo::BlockTypes::STATUS:
	  {
	    auto status = L1Topo::Status(word);
	    ATH_MSG_WARNING( "fibre overflow: " << status.overflow()
			     << " fibre crc: " << status.crc() );
	    if (status.overflow()) m_h_l1topo_1d_Errors->Fill(F_OVERFLOW);
	    if (status.crc()) m_h_l1topo_1d_Errors->Fill(F_CRC);
	    break;
	  }
	case L1Topo::BlockTypes::L1TOPO_TOB:
	  {
	    if (header.bcn_offset()==0){
	      auto tob = L1Topo::L1TopoTOB(word);
	      ATH_MSG_DEBUG(tob);
	      daqTobs.push_back(tob);
	      auto index = L1Topo::triggerBitIndex(rdo->getSourceID(),tob);
	      for (unsigned int i=0; i<8; ++i){
		if ((tob.trigger_bits() >>i)&1)
		  m_h_l1topo_1d_DAQTriggerBits->Fill(index+i);
		if ((tob.overflow_bits()>>i)&1)
		  m_h_l1topo_1d_DAQOverflowBits->Fill(index+i);
	      }
	    }
	    break;
	  }
	case L1Topo::BlockTypes::JET1_TOB:
	case L1Topo::BlockTypes::JET2_TOB:
	  {
	    if (header.bcn_offset()==0){
	      const int crate    = (word >> 28) & 0x1;
	      const int jem      = (word >> 24) & 0xF;
	      const int frame    = (word >> 21) & 0x7;
	      const int location = (word >> 19) & 0x3;
	      const int energyL  = (word & 0x2FF);
	      const int x = crate*16 + jem;
	      const int y = frame*4 + location;
	      //auto tob = L1Topo::JetTOB(word);
	      //daqJetTobs.push_back(tob);
	      int tob = 1; // Fake object until defined
	      if (energyL) {
		daqJetTobs.push_back(tob);
		m_h_l1topo_1d_JetTobs_EnergyLg->Fill(energyL,1./NFPGA);
		bool match=false;
		bool ematch=false;
		for (auto & t : cmxtobs) {
		  const int cmx_x = t->crate()*16 + t->jem();
		  const int cmx_y = t->frame()*4 + t->location();
		  if (x==cmx_x && y==cmx_y && energyL==t->energyLarge())
		    match=true;
		  if (energyL==t->energyLarge())
		    ematch=true;
		}
		if (!ematch) cmx_ematch=false;
		if (match)
		  m_h_l1topo_2d_JetTobs_Hitmap_match->Fill(x, y, 1./NFPGA);
		else
		  m_h_l1topo_2d_JetTobs_Hitmap_mismatch->Fill(x, y, 1./NFPGA);
	      }
	    }
	    break;
	  }


	case L1Topo::BlockTypes::TAU_TOB:
    {
      if (header.bcn_offset()==0){
        int tob = 1; // Fake object until defined
        daqTauTobs.push_back(tob);
      }
      break;
    }

  case L1Topo::BlockTypes::EM_TOB:
    {
      if (header.bcn_offset()==0){
        int tob = 1; // Fake object until defined
        daqEMTobs.push_back(tob);
      }
      break;
    }

	case L1Topo::BlockTypes::MUON_TOB:
	  {
	    if (header.bcn_offset()==0){
	      int tob = 1; // Fake object until defined
	      daqMuonTobs.push_back(tob);
	    }
	    break;
	  }
	case L1Topo::BlockTypes::ENERGY_TOB:
	  {
	    if (header.bcn_offset()==0){
	    }
	    break;
	  }
	default:
	  {
	    break;
	  }
	}
      } // for word
    }
    m_h_l1topo_1d_DAQTobs->Fill(std::min((int)daqTobs.size()/NFPGA,19));
    m_h_l1topo_1d_DAQJetTobs->Fill(std::min((int)daqJetTobs.size()/NFPGA,
					    MAXTOBS-1));
    m_h_l1topo_1d_DAQTauTobs->Fill(std::min((int)daqTauTobs.size()/NFPGA,
					    MAXTOBS-1));
    m_h_l1topo_1d_DAQMuonTobs->Fill(std::min((int)daqMuonTobs.size()/NFPGA,
					     MAXTOBS-1));
    m_h_l1topo_1d_DAQEMTobs->Fill(std::min((int)daqEMTobs.size()/NFPGA,
					   MAXTOBS-1));
  }
  
  // Retrieve and print the L1Topo RDOs from the ROI RODs
  std::vector<L1Topo::L1TopoTOB> roiTobs;
  const ROIB::RoIBResult* roibresult = 0;
  CHECK (evtStore()->retrieve(roibresult) );
  const std::vector< ROIB::L1TopoResult > l1TopoResults =
    roibresult->l1TopoResult();
  ATH_MSG_DEBUG( "Number of L1Topo ROI RODs found: " << l1TopoResults.size() );
  for (auto & r : l1TopoResults) {
    //ATH_MSG_VERBOSE( r.dump() );
    auto rdo = r.rdo();
    ATH_MSG_DEBUG( "Found ROI RDO with source ID "
		   << L1Topo::formatHex8(rdo.getSourceID()) );
    auto errors = rdo.getErrors();
    if (! errors.empty()) {
      ATH_MSG_WARNING( "Converter errors reported: " << errors );
      m_h_l1topo_1d_Errors->Fill(ROI_CONV);
    }
    const std::vector<uint32_t> cDataWords = rdo.getDataWords();
    if ( cDataWords.size() == 0 ) {
      ATH_MSG_DEBUG ( "L1TopoRDO ROI is empty" );
      m_h_l1topo_1d_Errors->Fill(NO_ROI);
    }
    for (auto word : cDataWords) {
      switch (L1Topo::blockType(word)) {
      case L1Topo::BlockTypes::L1TOPO_TOB:
      {
        auto tob = L1Topo::L1TopoTOB(word);
        ATH_MSG_DEBUG( tob );
        roiTobs.push_back(tob);
        //auto index = L1Topo::triggerBitIndex(rdo.getSourceID(),tob);
        for (unsigned int i = 0; i < 8; ++i) {
          //m_histTriggerBitsFromROIConv->Fill (index+i,(tob.trigger_bits() >>i)&1);
          //m_histOverflowBitsFromROIConv->Fill(index+i,(tob.overflow_bits()>>i)&1);
        }
        // histogram trigger and overflow bits
        break;
      }
      default:
      {
        ATH_MSG_WARNING( "unexpected TOB type in ROI: "
			 <<  L1Topo::formatHex8(word) );
        break;
      }
      }
    }
  }
  m_h_l1topo_1d_ROITobs->Fill(std::min((int)roiTobs.size(),19));

  for (unsigned int i=1; i<=128;++i) {
    float diff=fabs(m_h_l1topo_1d_DAQTriggerBits->GetBinContent(i)-
		    m_h_l1topo_1d_Simulation->GetBinContent(i));
    if (diff>0.1)
      m_h_l1topo_1d_DAQMismatchTriggerBits->SetBinContent(i,diff);
  }

  if (!cmx_ematch) m_h_l1topo_1d_Errors->Fill(CMX_EMATCH);

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloL1TopoMon::procHistograms()
/*---------------------------------------------------------*/
{
  if ( endOfLumiBlock ) { }

  if ( endOfRun ) { }

  return StatusCode::SUCCESS;
}

// *********************************************************************
// Private Methods
// *********************************************************************

}
