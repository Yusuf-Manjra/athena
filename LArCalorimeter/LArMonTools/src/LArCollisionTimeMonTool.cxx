/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArCollisionTimeMonTool.cxx
// PACKAGE:  LArMonTools
//
// AUTHOR:   Vikas Bansal
//
// Class for monitoring : EC timing from LArCollisionTime Container 

//
// ********************************************************************


#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <functional>
#include <map>
#include <utility>
#include <algorithm>

#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

#include "LArMonTools/LArCollisionTimeMonTool.h"
#include "LArRecEvent/LArCollisionTime.h"

#include "xAODEventInfo/EventInfo.h"

#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

using namespace std;

/*---------------------------------------------------------*/
LArCollisionTimeMonTool::LArCollisionTimeMonTool(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent), 
    m_lumi_blocks( 1500 ),
    m_ECTimeDiff(0),
    m_ECTimeAvg(0),
    m_timeCut(5.0),
    m_minCells(2),
    m_eWeighted(true),
    newrun(true),
    //m_bunchGroupTool("BunchGroupTool"),
    m_bunchGroupTool("BunchCrossingTool"),
    m_bcid_init(false)
{
  declareProperty( "m_lumi_blocks"	,      m_lumi_blocks = 1000 );
  declareProperty( "timeDiffCut"	,      m_timeCut = 5.0 );
  declareProperty( "nCells"		,      m_minCells = 2 );
  declareProperty( "eWeighted"		,      m_eWeighted = true );
  declareProperty( "Key"                ,      m_key="LArCollisionTime");
  declareProperty( "histPath"           ,      m_histPath="LArCollisionTime"); 
  declareProperty( "BunchCrossingTool"  ,      m_bunchGroupTool); 
  declareProperty( "TrainFrontDistance" ,      m_distance = 30); 
  declareProperty( "IsOnline"           ,      m_IsOnline=false);
  
  m_eventsCounter = 0;  
  
  m_LArCollTime_h			= NULL; 
  m_LArCollTime_lb_h			= NULL;
  m_LArCollTime_lb_timeCut_h		= NULL;
  m_LArCollTime_lb_singlebeam_timeCut_h = NULL;
  m_LArCollTime_vs_LB_h			= NULL; 
  m_LArCollTime_vs_BCID_h		= NULL;
  m_LArCollAvgTime_h			= NULL; 
  m_LArCollAvgTime_vs_LB_h		= NULL; 
}

/*---------------------------------------------------------*/
LArCollisionTimeMonTool::~LArCollisionTimeMonTool()
{
}

/*---------------------------------------------------------*/
StatusCode 
LArCollisionTimeMonTool::initialize() {

  ManagedMonitorToolBase::initialize().ignore();
  CHECK(m_bunchGroupTool.retrieve());
  msg(MSG::DEBUG) << "Successful Initialize LArCollisionTimeMonTool " << endmsg;
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode 
LArCollisionTimeMonTool::bookHistograms() {
  
  if(m_IsOnline)  m_nhist=2; else m_nhist=1;
  // So far 2 histos, all bcid and inside the train
  m_LArCollTime_h=new TH1F_LW*[m_nhist];
  m_LArCollAvgTime_h=new TH1F_LW*[m_nhist];
  m_LArCollTime_lb_h=new TH1F_LW*[m_nhist];
  m_LArCollTime_lb_timeCut_h=new TH1F_LW*[m_nhist];
  m_LArCollTime_lb_singlebeam_timeCut_h=new TH1F_LW*[m_nhist];
  m_LArCollTime_vs_LB_h=new TH2F_LW*[m_nhist];
  m_LArCollTime_vs_BCID_h=new TH2F_LW*[m_nhist];
  m_LArCollAvgTime_vs_LB_h=new TH2F_LW*[m_nhist];
  m_LArCollAvgTime_vs_BCID_h=new TH2F_LW*[m_nhist];

  std::vector<std::string> hnameadd(m_nhist); 
  if(m_IsOnline) hnameadd={"","_intrain"}; else hnameadd={""};
  std::vector<std::string> htitadd(m_nhist); 
  if(m_IsOnline) htitadd={"",", inside the train"}; else htitadd={""};

  MonGroup generalGroupShift( this, "/LAr/"+m_histPath+"/", run, ATTRIB_MANAGED);
  MonGroup generalGroupLB( this, "/LAr/"+m_histPath+"/", run, ATTRIB_X_VS_LB, "", "merge");
  //if(isNewRun ){ // Commented by B.Trocme to comply with new ManagedMonitorToolBase
    newrun=true;
    //
    // Create top folder for histos
    //
  for(unsigned i=0; i<m_nhist; ++i) {   
    m_LArCollTime_h[i] = TH1F_LW::create(("LArCollTime"+hnameadd[i]).data(), 
			       ("LArCollisionTime - difference of avg time from ECC and ECA"+htitadd[i]).data(),
			       101, -50.5, 50.5 );			
    
    m_LArCollTime_h[i]->GetYaxis()->SetTitle("Number of events (weighted by energy/GeV) per ns");
    m_LArCollTime_h[i]->GetXaxis()->SetTitle("<t_{C}> - <t_{A}> (ns)");
    generalGroupShift.regHist(m_LArCollTime_h[i]).ignore();
    
    m_LArCollTime_lb_h[i] = TH1F_LW::create(("LArCollTimeLumiBlock"+hnameadd[i]).data(), 
				     ("LArCollisionTime - difference of avg time from ECC and ECA"+htitadd[i]).data(),
				  101, -50.5, 50.5 );			
    
    m_LArCollTime_lb_h[i]->GetYaxis()->SetTitle("Number of events (weighted by energy/GeV) per ns");
    m_LArCollTime_lb_h[i]->GetXaxis()->SetTitle("<t_{C}> - <t_{A}> (ns)");
    generalGroupShift.regHist(m_LArCollTime_lb_h[i]).ignore();
    
    
    
    m_LArCollTime_vs_LB_h[i] = TH2F_LW::create(("LArCollTime_vs_LB"+hnameadd[i]).data(), 
				        ("LArCollisionTime Vs Luminosity Block - difference of avg time of ECC and ECA as a function of luminosity block"+htitadd[i]).data(),
				     m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5, 101, -50.5, 50.5 );			
    
    m_LArCollTime_vs_LB_h[i]->GetXaxis()->SetTitle("Luminosity Block Number");
    m_LArCollTime_vs_LB_h[i]->GetYaxis()->SetTitle("<t_{C}> - <t_{A}> (ns)");
    m_LArCollTime_vs_LB_h[i]->GetZaxis()->SetTitle("Number of events (weighted by energy/GeV)");
    generalGroupLB.regHist(m_LArCollTime_vs_LB_h[i]).ignore();
    
    m_LArCollTime_vs_BCID_h[i] = TH2F_LW::create(("LArCollTime_vs_BCID"+hnameadd[i]).data(), 
				          ("LArCollisionTime Vs BCID - difference of avg time of ECC and ECA as a function of BCID"+htitadd[i]).data(),
				       3564, 0.5, 3564.5, 101, -50.5, 50.5 );			
    
    m_LArCollTime_vs_BCID_h[i]->GetXaxis()->SetTitle("Bunch Crossing Number");
    m_LArCollTime_vs_BCID_h[i]->GetYaxis()->SetTitle("<t_{C}> - <t_{A}> (ns)");
    m_LArCollTime_vs_BCID_h[i]->GetZaxis()->SetTitle("Number of events (weighted by energy/GeV)");
    generalGroupShift.regHist(m_LArCollTime_vs_BCID_h[i]).ignore();  
    
    // Average Time
    
    m_LArCollAvgTime_h[i] = TH1F_LW::create(("LArCollAvgTime"+hnameadd[i]).data(), 
				     ("LArCollisionAverageTime - avg time of ECC and ECA"+htitadd[i]).data(),
				  320, -40., 40. );			
    
    m_LArCollAvgTime_h[i]->GetYaxis()->SetTitle("Number of events (weighted by energy/GeV) per ns");
    m_LArCollAvgTime_h[i]->GetXaxis()->SetTitle("(<t_{C}> + <t_{A}>) / 2 (ns)");
    generalGroupShift.regHist(m_LArCollAvgTime_h[i]).ignore();
    
    m_LArCollAvgTime_vs_LB_h[i] = TH2F_LW::create(("LArCollAvgTime_vs_LB"+hnameadd[i]).data(), 
					   ("LArCollisionAvgTime Vs Luminosity Block - avg time of ECC and ECA as a function of luminosity block"+htitadd[i]).data(),
					m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5, 320, -40., 40. );			
    
    m_LArCollAvgTime_vs_LB_h[i]->GetXaxis()->SetTitle("Luminosity Block Number");
    m_LArCollAvgTime_vs_LB_h[i]->GetYaxis()->SetTitle("(<t_{C}> + <t_{A}>) / 2 (ns)");
    m_LArCollAvgTime_vs_LB_h[i]->GetZaxis()->SetTitle("Number of events (weighted by energy/GeV)");
    generalGroupLB.regHist(m_LArCollAvgTime_vs_LB_h[i]).ignore();
    
    m_LArCollAvgTime_vs_BCID_h[i] = TH2F_LW::create(("LArCollAvgTime_vs_BCID"+hnameadd[i]).data(), 
					     ("LArCollisionAvgTime Vs BCID - avg time of ECC and ECA as a function of luminosity block"+htitadd[i]).data(),
					m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5, 320, -40., 40. );			
    
    m_LArCollAvgTime_vs_BCID_h[i]->GetXaxis()->SetTitle("Bunch Crossing Number");
    m_LArCollAvgTime_vs_BCID_h[i]->GetYaxis()->SetTitle("(<t_{C}> + <t_{A}>) / 2 (ns)");
    m_LArCollAvgTime_vs_BCID_h[i]->GetZaxis()->SetTitle("Number of events (weighted by energy/GeV)");
    generalGroupShift.regHist(m_LArCollAvgTime_vs_BCID_h[i]).ignore();
    
    //Monitoring events as a function of LB when events lie in a time window of +/-10ns
    m_LArCollTime_lb_timeCut_h[i] = TH1F_LW::create(("LArCollTimeLumiBlockTimeCut"+hnameadd[i]).data(), 
					     ("Events with abs(<t_{C}> - <t_{A}>) < 10ns as a function of LB"+htitadd[i]).data(),
					  m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5 );			
    
    m_LArCollTime_lb_timeCut_h[i]->GetYaxis()->SetTitle("Number of events per LB");
    m_LArCollTime_lb_timeCut_h[i]->GetXaxis()->SetTitle("Luminosity Block Number");
    generalGroupShift.regHist(m_LArCollTime_lb_timeCut_h[i]).ignore();
    
    //Monitoring events as a function of LB when events lie in a time window of +/-[20-30]ns
    m_LArCollTime_lb_singlebeam_timeCut_h[i] = TH1F_LW::create(("LArCollTimeLumiBlockSingleBeamTimeCut"+hnameadd[i]).data(), 
						        ("Events with 20 ns < abs(<t_{C}> - <t_{A}>) < 30ns as a function of LB"+htitadd[i]).data(),
						     m_lumi_blocks, 0.5, double(m_lumi_blocks)+0.5 );			
    
    m_LArCollTime_lb_singlebeam_timeCut_h[i]->GetYaxis()->SetTitle("Number of events per LB");
    m_LArCollTime_lb_singlebeam_timeCut_h[i]->GetXaxis()->SetTitle("Luminosity Block Number");
    generalGroupShift.regHist(m_LArCollTime_lb_singlebeam_timeCut_h[i]).ignore();
    
  }
  return StatusCode::SUCCESS;
    
    //  }// end isNewRun
  
}


/*---------------------------------------------------------*/
StatusCode 
LArCollisionTimeMonTool::fillHistograms()
{
  msg(MSG::DEBUG) << "in fillHists()" << endmsg;
  
  // Increment event counter
  m_eventsCounter++;
  
  // --- retrieve event information ---
  const xAOD::EventInfo* event_info;
  unsigned bunch_crossing_id = 0;
  unsigned lumi_block        = 0;
  //double event_time_minutes = -1;
  if (evtStore()->retrieve( event_info ).isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve EventInfo object" << endmsg;
    return StatusCode::FAILURE;
  }

  // bunch crossing ID:
  bunch_crossing_id = event_info->bcid();
    
  // luminosity block number
  lumi_block = event_info->lumiBlock();
    
  if(m_bunchGroupTool->bcType(bunch_crossing_id) == Trig::IBunchCrossingTool::Empty) {
     //msg(MSG::INFO) <<"BCID: "<<bunch_crossing_id<<" empty, not filling CollTime" <<endmsg;
     msg(MSG::INFO) <<"BCID: "<<bunch_crossing_id<<" empty ? not filling the coll. time" <<endmsg;
     return StatusCode::SUCCESS; // not filling anything in empty bunches
  }

  int bcid_distance = m_bunchGroupTool->distanceFromFront(bunch_crossing_id, Trig::IBunchCrossingTool::BunchCrossings);
  
  // Retrieve LArCollision Timing information
  const LArCollisionTime * larTime;
  if(evtStore()->retrieve(larTime,m_key).isFailure())
  {
    msg(MSG::WARNING) << "Unable to retrieve LArCollisionTime event store" << endmsg;
    return StatusCode::SUCCESS; // Check if failure shd be returned. VB
  } else {
    msg(MSG::DEBUG) << "LArCollisionTime successfully retrieved from event store" << endmsg;
  }

  if (larTime and !(event_info->isEventFlagBitSet(xAOD::EventInfo::LAr,3))) {// Do not fill histo if noise burst suspected
    // Calculate the time diff between ECC and ECA
    m_ECTimeDiff = larTime->timeC() - larTime->timeA();
    m_ECTimeAvg  = (larTime->timeC() + larTime->timeA()) / 2.0;
    if (larTime->ncellA() > m_minCells && larTime->ncellC() > m_minCells && std::fabs(m_ECTimeDiff) < m_timeCut ) { // Only fill histograms if a minimum number of cells were found and time difference was sensible
      double weight = 1;
      if (m_eWeighted) weight = (larTime->energyA()+larTime->energyC())/1000; 
      m_LArCollTime_h[0]->Fill(m_ECTimeDiff,weight);
      m_LArCollTime_lb_h[0]->Fill(m_ECTimeDiff,weight);
      m_LArCollTime_vs_LB_h[0]->Fill(lumi_block, m_ECTimeDiff,weight);
      m_LArCollTime_vs_BCID_h[0]->Fill(bunch_crossing_id, m_ECTimeDiff,weight);
      m_LArCollAvgTime_h[0]->Fill(m_ECTimeAvg,weight);
      m_LArCollAvgTime_vs_LB_h[0]->Fill(lumi_block, m_ECTimeAvg,weight);
      m_LArCollAvgTime_vs_BCID_h[0]->Fill(bunch_crossing_id, m_ECTimeAvg,weight);
      if ( fabs(m_ECTimeDiff) < 10 ) m_LArCollTime_lb_timeCut_h[0]->Fill(lumi_block);
      if ( fabs(m_ECTimeDiff) > 20 && fabs(m_ECTimeDiff) < 30 ) m_LArCollTime_lb_singlebeam_timeCut_h[0]->Fill(lumi_block);
      if(m_IsOnline && bcid_distance > m_distance) { // fill histos inside the train
        msg(MSG::INFO) <<"BCID: "<<bunch_crossing_id<<" distance from Front: "<<bcid_distance<<"Filling in train..."<<endmsg;    
        m_LArCollTime_h[1]->Fill(m_ECTimeDiff,weight);
        m_LArCollTime_lb_h[1]->Fill(m_ECTimeDiff,weight);
        m_LArCollTime_vs_LB_h[1]->Fill(lumi_block, m_ECTimeDiff,weight);
        m_LArCollTime_vs_BCID_h[1]->Fill(bunch_crossing_id, m_ECTimeDiff,weight);
        m_LArCollAvgTime_h[1]->Fill(m_ECTimeAvg,weight);
        m_LArCollAvgTime_vs_LB_h[1]->Fill(lumi_block, m_ECTimeAvg,weight);
        m_LArCollAvgTime_vs_BCID_h[1]->Fill(bunch_crossing_id, m_ECTimeAvg,weight);
        if ( fabs(m_ECTimeDiff) < 10 ) m_LArCollTime_lb_timeCut_h[1]->Fill(lumi_block);
        if ( fabs(m_ECTimeDiff) > 20 && fabs(m_ECTimeDiff) < 30 ) m_LArCollTime_lb_singlebeam_timeCut_h[1]->Fill(lumi_block);
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode LArCollisionTimeMonTool::procHistograms()
{
  
  if(endOfLumiBlockFlag() ){
    // For online monitoring, reset the histogram after Lumi block finishes
    for(unsigned i=0; i<m_nhist; ++i) m_LArCollTime_lb_h[i]->Reset();
  }
  
  msg(MSG::DEBUG) << "End of procHistograms " << endmsg;
  return StatusCode::SUCCESS;
}
