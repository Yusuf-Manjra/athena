/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

   NAME: TRTCalibrator.cxx    
PACKAGE: TRTCalibTools 

AUTHORS: Johan Lundquist  
CREATED: 27-03-2009 

PURPOSE: Tool for calibrating the TRT
          
********************************************************************/

#include "TRTCalibrator.h"
#include "Calibrator.h"
#include "TRT_CalibData/TrackInfo.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "EventPrimitives/EventPrimitives.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ConditionsData/RtRelation.h"
#include "TRT_ConditionsData/BasicRtRelation.h"
#include "TRT_ConditionsData/DinesRtRelation.h"
#include "TRT_ConditionsData/BinnedRtRelation.h"
#include "CommissionEvent/ComTime.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"

#include <TNtuple.h>
#include <TFile.h>
#include <fstream>


TRTCalibrator::TRTCalibrator(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_DetID(nullptr),
  m_TRTID(nullptr),
  m_trtmanager(nullptr),
  m_trtcaldbTool("ITRT_CalDbTool", this),
  m_neighbourSvc("ITRT_StrawNeighbourSvc", name),
  m_TRTStrawSummaryTool("InDetTRTStrawStatusSummaryTool",this),
  m_maxDistance(2.8),
  m_maxTimeResidual(150),
  m_minTimebinsOverThreshold(2),
  m_maxTrackChisquarePerDof(10),
  m_numOfHitsTotal(0),
  m_numOfHitsAccepted(0),
  m_numOfProcessedTracks(0),
  m_comTimeName("ComTime"),
  m_minrt(10000),
  m_mint0(200),
  m_nevents(-1),
  m_t0offset(-0.3),
  m_calsub("user"),
  m_rtbinning("t"),
  m_selstring("_*_-_-_-_-_-_-"),
  m_options(""),
  m_hittuple("basic.root"),
  m_rtrel("basic"),
  m_ntrtmanager("TRT"),
  m_SplitBarrel(true),
  m_useP0(true),
  m_floatP3(true),
  m_DoShortStrawCorrection(true),
  m_DoArXenonSep(false),
  m_histfile(nullptr)
{
  declareInterface<ITRTCalibrator>(this);
  declareProperty("TRTCalDbTool",m_trtcaldbTool);
  declareProperty("NeighbourSvc",m_neighbourSvc);
  declareProperty("maxDistance",m_maxDistance) ;
  declareProperty("maxTimeResidual",m_maxTimeResidual) ;
  declareProperty("minTimebinsOverThreshold",m_minTimebinsOverThreshold) ;
  declareProperty("maxTrackChisquarePerDof",m_maxTrackChisquarePerDof) ;
  declareProperty("MinRt",m_minrt);
  declareProperty("MinT0",m_mint0);
  declareProperty("Nevents",m_nevents);
  declareProperty("Selstring",m_selstring);
  declareProperty("SubPart",m_calsub);
  declareProperty("Options",m_options);
  declareProperty("Hittuple",m_hittuple);
  declareProperty("RtRel",m_rtrel);
  declareProperty("TrtManagerLocation",m_ntrtmanager);
  declareProperty("CalibrateRt",m_doRt);
  declareProperty("CalibrateT0",m_doT0);
  declareProperty("RtBinning",m_rtbinning);
  declareProperty("FitResidual",m_doRes);
  declareProperty("NoHistograms",m_beQuiet);
  declareProperty("UseBoardRef",m_useBoardRef);
  declareProperty("SplitBarrel",m_SplitBarrel);
  declareProperty("UseP0",m_useP0);
  declareProperty("FloatP3",m_floatP3);
  declareProperty("PrintLog",m_doLogPrint);
  declareProperty("PrintT0Out",m_doOutPrint);
  declareProperty("PrintRtOut",m_doRtPrint);
  declareProperty("T0Offset",m_t0offset);
  declareProperty("DoShortStrawCorrection",m_DoShortStrawCorrection);
  declareProperty("DoArXenonSep",m_DoArXenonSep);
  declareProperty("TRTStrawSummaryTool",  m_TRTStrawSummaryTool);
}


StatusCode TRTCalibrator::initialize()
{
  msg(MSG::INFO) << "initialize()" << endmsg;
  
  m_trtmanager=nullptr; 
  
  if ((detStore()->retrieve(m_DetID,"AtlasID")).isFailure()) {
    msg(MSG::FATAL) << "Problem retrieving ATLASDetectorID helper" << endmsg;
    return StatusCode::FAILURE;
  }
  if ((detStore()->retrieve(m_trtmanager,m_ntrtmanager)).isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT_DetectorManager" << endmsg;
    return StatusCode::FAILURE;
  }
  if ((detStore()->retrieve(m_TRTID)).isFailure()) {
    msg(MSG::FATAL) << "Problem retrieving TRTID helper" << endmsg;
    return StatusCode::FAILURE;
  }
  if(m_trtcaldbTool.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Could not get TRTCalDbTool !" << endmsg;
    return StatusCode::FAILURE;
  }
  if(StatusCode::SUCCESS!=m_neighbourSvc.retrieve() ) {
    msg(MSG::FATAL) <<"Could not get TRTStrawNeighbourTool !"<<endmsg;
    return StatusCode::FAILURE;
  }


  // The tool to get the argon status:

   if (m_TRTStrawSummaryTool.retrieve().isFailure() ) {
      ATH_MSG_ERROR ("Failed to retrieve StrawStatus Summary " << m_TRTStrawSummaryTool);
      return StatusCode::FAILURE;
   } else {
      msg(MSG::INFO) << "Retrieved tool " << m_TRTStrawSummaryTool << endmsg;
   }


  m_histfile = new TFile("calibout.root","RECREATE");

  /* 
     Pre-define standard calibration configuration

     The string-vectors "CalibrateRt" ("CalibrateT0") contains the names of the levels for 
     which R-t (T0) calibration should be performed

     The string-vector "PrintLog" contains the names of the levels for which a status line
     should be printed to the output log

     The string-vectors "PrintT0Out" and "PrintRtOut" contains the names of the levels for 
     text files containing the calibration constants should be generated

     This standard configuration divides the Barrel in 9 parts: 
     
     "all": a single calibration for all hits in the whole TRT
     "-1" & "1": calibration of the barrel C- & A-side
     "X_Y": calibration of layer Y in barrel X-side
  */ 

  //the whole TRT
  m_config["all"].selections["Detector"].insert(CALIB_NONE); // don't go deeper than TRT level
  m_config["all"].selections["Layer"].insert(CALIB_NONE);
  m_config["all"].selections["Module"].insert(CALIB_NONE);
  m_config["all"].selections["Board"].insert(CALIB_NONE);
  m_config["all"].selections["Chip"].insert(CALIB_NONE);
  m_config["all"].selections["Straw"].insert(CALIB_NONE);
  m_config["all"].CalibrateRt.emplace_back("TRT") ; //only calibrate R-t and T0 on TRT level
  m_config["all"].CalibrateT0.emplace_back("TRT") ;
  m_config["all"].PrintLog.emplace_back("TRT") ;
  m_config["all"].PrintT0Out.emplace_back("TRT") ;
  m_config["all"].PrintRtOut.emplace_back("TRT") ;

  //common for all barrel/ec
  m_config["subdet"].CalibrateRt.emplace_back("TRT") ;  //calibrate R-t and T0 on TRT and barrel/ec level
  m_config["subdet"].CalibrateRt.emplace_back("Detector") ;
  m_config["subdet"].CalibrateT0=m_config["subdet"].CalibrateRt;
  m_config["subdet"].NoHistograms.emplace_back("TRT") ; //don't write root histogram on TRT level
  m_config["subdet"].PrintLog=m_config["subdet"].CalibrateRt;
  m_config["subdet"].PrintT0Out.emplace_back("Detector") ;
  m_config["subdet"].PrintRtOut.emplace_back("Detector") ;

  //individual barrel/ec
  m_config["1"]=m_config["subdet"];
  m_config["1"].selections["Detector"].insert(1);
  m_config["1"].selections["Layer"].insert(CALIB_NONE); // don't go deeper than barrel/ec level
  m_config["1"].selections["Module"].insert(CALIB_NONE);
  m_config["1"].selections["Board"].insert(CALIB_NONE);
  m_config["1"].selections["Chip"].insert(CALIB_NONE);
  m_config["1"].selections["Straw"].insert(CALIB_NONE);
  m_config["-1"]=m_config["subdet"];
  m_config["-1"].selections["Detector"].insert(-1);
  m_config["-1"].selections["Layer"].insert(CALIB_NONE);
  m_config["-1"].selections["Module"].insert(CALIB_NONE);
  m_config["-1"].selections["Board"].insert(CALIB_NONE);
  m_config["-1"].selections["Chip"].insert(CALIB_NONE);
  m_config["-1"].selections["Straw"].insert(CALIB_NONE);

  //common for all layers
  m_config["layer"].CalibrateRt.emplace_back("TRT") ;  //calibrate R-t and T0 on all levels (except Straw level)
  m_config["layer"].CalibrateRt.emplace_back("Detector") ;
  m_config["layer"].CalibrateRt.emplace_back("Layer") ;
  m_config["layer"].CalibrateRt.emplace_back("Module") ;
  m_config["layer"].CalibrateRt.emplace_back("Board") ;
  m_config["layer"].CalibrateRt.emplace_back("Chip") ;
  m_config["layer"].CalibrateT0=m_config["layer"].CalibrateRt;
  m_config["layer"].NoHistograms.emplace_back("TRT") ; //don't write root histogram on TRT, barrel/ec, straw level
  m_config["layer"].NoHistograms.emplace_back("Detector") ;
  m_config["layer"].NoHistograms.emplace_back("Straw") ;
  m_config["layer"].PrintLog=m_config["layer"].CalibrateRt;
  m_config["layer"].PrintT0Out.emplace_back("Layer") ;
  m_config["layer"].PrintT0Out.emplace_back("Module") ;
  m_config["layer"].PrintT0Out.emplace_back("Board") ;
  m_config["layer"].PrintT0Out.emplace_back("Chip") ;
  m_config["layer"].PrintT0Out.emplace_back("Straw") ; //print txt output for each straw
  m_config["layer"].PrintRtOut.emplace_back("Layer") ;
  m_config["layer"].PrintRtOut.emplace_back("Module") ;
  m_config["layer"].PrintRtOut.emplace_back("Board") ;
  m_config["layer"].PrintRtOut.emplace_back("Chip") ;
  m_config["layer"].PrintRtOut.emplace_back("Straw") ; //print txt output for each straw

  //individual layers
  m_config["1_0"]=m_config["layer"];
  m_config["1_0"].selections["Detector"].insert(1);
  m_config["1_0"].selections["Layer"].insert(0);
  m_config["1_1"]=m_config["layer"];
  m_config["1_1"].selections["Detector"].insert(1);
  m_config["1_1"].selections["Layer"].insert(1);
  m_config["1_2"]=m_config["layer"];
  m_config["1_2"].selections["Detector"].insert(1);
  m_config["1_2"].selections["Layer"].insert(2);
  m_config["-1_0"]=m_config["layer"];
  m_config["-1_0"].selections["Detector"].insert(-1);
  m_config["-1_0"].selections["Layer"].insert(0);
  m_config["-1_1"]=m_config["layer"];
  m_config["-1_1"].selections["Detector"].insert(-1);
  m_config["-1_1"].selections["Layer"].insert(1);
  m_config["-1_2"]=m_config["layer"];
  m_config["-1_2"].selections["Detector"].insert(-1);
  m_config["-1_2"].selections["Layer"].insert(2);

  return StatusCode::SUCCESS;
}


StatusCode TRTCalibrator::finalize()
{
  m_histfile->Write();
  m_histfile->Close();
  return StatusCode::SUCCESS;
}


std::string TRTCalibrator::SubLev(std::string pat, int lev){  
  
  std::string sublev;

  for (int i=0;i<lev;i++){
    pat.replace(pat.find('_'),1,"");
    sublev = pat.substr(0,pat.find('_'));
    pat.replace(0,pat.find('_'),"");
  }
  return sublev;
}

void TRTCalibrator::DumpStrawData(int isid){  
  
  std::map<std::string,int> strawmap;
  Identifier ident=(Identifier)(isid);



  std::string strawkey=std::string(Form("%i %i %i %i %i", 
        (int)m_TRTID->barrel_ec(ident), 
        (int)m_TRTID->layer_or_wheel(ident) ,
        (int)m_TRTID->phi_module(ident) ,
        (int)m_TRTID->straw_layer(ident) ,
        (int)m_TRTID->straw(ident)));
  strawmap[strawkey]=isid;
}


bool TRTCalibrator::IsSubLev(const std::string& key, int lev, const std::string& sublev){  
  
  std::string sl=sublev;

  if (sl.compare(SubLev(key,lev))==0) return true;

   while(sl.compare(sl.substr(sl.find(',')+1))!=0){
     if (SubLev(key,lev).compare(sl.substr(0,sl.find(',')))==0) return true;
     sl=sl.substr(sl.find(',')+1);
     if (SubLev(key,lev).compare(sl.substr(0,sl.find(',')))==0) return true;
   }
  return false;

}

int TRTCalibrator::GetSubLevels(const std::string& key, int lev, std::set<int>* levels){ 

  std::string sl=SubLev(key,lev);

  if (sl.compare("-")==0) {
    levels->insert(-4); 
    return 999;
  }
  if (sl.compare("*")==0) {
    levels->insert(-3); 
    return 999;
  }

  if(sl.find(',')!=std::string::npos){
    int value;
    while(sl.find(',')!=std::string::npos){
      sscanf(sl.substr(0,sl.find(',')).data(),"%i",&value);
      levels->insert(value);
      sl=sl.substr(sl.find(',')+1);
    }
    sscanf(sl.data(),"%i",&value);
    levels->insert(value);
    return levels->size();
  }

  if(sl.find('t')!=std::string::npos){
    int min,max;
    sscanf(sl.substr(0,sl.find('t')).data(),"%i",&min);
    std::cout << "min=" << min << std::endl;
    sl=sl.substr(sl.find('t')+1);
    sscanf(sl.data(),"%i",&max);
    std::cout << "max=" << max << std::endl;
    for (int imod=min; imod<=max; imod++){
      levels->insert(imod);
    }
    return levels->size();
  }

  int value;
  sscanf(sl.data(),"%i",&value);
  levels->insert(value);
  return levels->size();

}

void TRTCalibrator::MakeBDKeys(int bec, int low, int mod, int brd, int chp, int strawid){
  //make map keys for the board-chip view

  char ckey[100];
  sprintf(ckey,"_%i_%i_%i_%i_%i_%i",bec,low,mod,brd,chp,strawid); m_Skey=std::string(ckey);
  sprintf(ckey,"_%i_%i_%i_%i_%i",bec,low,mod,brd,chp); m_Ckey=std::string(ckey);
  sprintf(ckey,"_%i_%i_%i_%i",bec,low,mod,brd); m_Bkey=std::string(ckey);
  sprintf(ckey,"_%i_%i_%i",bec,low,mod); m_Mkey=std::string(ckey);
  sprintf(ckey,"_%i_%i",bec,low); m_Lkey=std::string(ckey);
  sprintf(ckey,"_%i",bec); m_Dkey=std::string(ckey);
  sprintf(ckey,"_all"); m_Tkey=std::string(ckey);

  sprintf(ckey,"_%i_%i",abs(bec),low); m_Lkey_acc=std::string(ckey);
  sprintf(ckey,"_%i",abs(bec)); m_Dkey_acc=std::string(ckey);

}

bool TRTCalibrator::calibrate ATLAS_NOT_THREAD_SAFE () {

  float run,evt,trk,r,dr,t,rtrack,ttrack,t0,ephase,sid;
  Identifier ident;
  const InDetDD::TRT_BaseElement* strawelement;
  int nTRThist=0, ndethist=0, nlayhist=0, nmodhist=0, nbrdhist=0, nchphist=0, nstwhist=0;
  int nTRThistAr=0, ndethistAr=0, nlayhistAr=0, nmodhistAr=0, nbrdhistAr=0, nchphistAr=0, nstwhistAr=0;
  databundle hitdata;
  
  std::string infile=m_hittuple;
  std::string outfile="calibout";
  
  std::string detname="Detector";
  std::string layname="Layer";
  if (!m_SplitBarrel) {
    detname="WholeBarrel";
    layname="WholeBarrelLayer";
  }    
 
  //create Calibrator objects for each sub-level
  Calibrator TRT(0,"TRT",m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);
  Calibrator Detector(1,detname.data(),m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);
  Calibrator Layer(2,layname.data(),m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);
  Calibrator Module(3,"Module",m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);
  Calibrator Board(4,"Board",m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);
  Calibrator Chip(5,"Chip",m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);
  Calibrator Straw(6,"Straw",m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);


  // make a map of calibrators (so that they can be looped over)
  std::map<std::string,Calibrator*> calibrators;
  calibrators["TRT"]=&TRT;
  calibrators["Detector"]=&Detector;
  calibrators["Layer"]=&Layer;
  calibrators["Module"]=&Module;
  calibrators["Board"]=&Board;
  calibrators["Chip"]=&Chip;
  calibrators["Straw"]=&Straw;

  //use default configurations unless "user" option is used
  if(m_calsub!="user"){
    m_doRt=m_config[m_calsub].CalibrateRt;
    m_doT0=m_config[m_calsub].CalibrateT0;
    m_doRes=m_config[m_calsub].FitResidual;
    m_beQuiet=m_config[m_calsub].NoHistograms;
    m_doOutPrint=m_config[m_calsub].PrintT0Out;
    m_doRtPrint=m_config[m_calsub].PrintRtOut;
    m_doLogPrint=m_config[m_calsub].PrintLog;    
  }

  //configure the calibrators
  for (const std::string& s : m_doRt)        calibrators[s]->dort=true;
  for (const std::string& s : m_doT0)        calibrators[s]->dot0=true;
  for (const std::string& s : m_doRes)       calibrators[s]->dores=true;
  for (const std::string& s : m_beQuiet)     calibrators[s]->bequiet=true;
  for (const std::string& s : m_doOutPrint)  calibrators[s]->printt0=true;
  for (const std::string& s : m_doRtPrint)   calibrators[s]->printrt=true;
  for (const std::string& s : m_doLogPrint)  calibrators[s]->printlog=true;
  for (const std::string& s : m_useBoardRef) calibrators[s]->usebref=true;
  for (std::pair<const std::string, Calibrator*>& p : calibrators) {
    Calibrator* calib = p.second;
    calib->usep0=m_useP0;
    calib->floatp3=m_floatP3;
    calib->useshortstraws=m_DoShortStrawCorrection;

    if (m_doRt.size()==0) calib->nort=true;
    if (m_doT0.size()==0) calib->not0=true;
    
    std::set<int> selset;
    GetSubLevels(m_selstring,calib->level+1,&selset); //get the set of selections for level "level+1" from the m_selstring ...
    for (std::set<int>::iterator imod=selset.begin(); imod!=selset.end(); ++imod){
      calib->selection = selset;//... and configure the calibrator with them
    }
  }     

  // FOR ARGON:
  Calibrator TRT_Ar     (0,"TRT_Ar"     ,m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);
  Calibrator Detector_Ar(1,"Detector_Ar",m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);
  Calibrator Layer_Ar   (2,"Layer_Ar"   ,m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);
  Calibrator Module_Ar  (3,"Module_Ar"  ,m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);
  Calibrator Board_Ar   (4,"Board_Ar"   ,m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);
  Calibrator Chip_Ar    (5,"Chip_Ar"    ,m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);
  Calibrator Straw_Ar   (6,"Straw_Ar"   ,m_mint0,m_minrt,m_rtrel,m_rtbinning,m_t0offset);

  // make a map of calibrators (so that they can be looped over)
  std::map<std::string,Calibrator*>     calibratorsAr;
  calibratorsAr["TRT"]                  =&TRT_Ar;
  calibratorsAr["Detector"]             =&Detector_Ar;
  calibratorsAr["Layer"]                =&Layer_Ar;
  calibratorsAr["Module"]               =&Module_Ar;
  calibratorsAr["Board"]                =&Board_Ar;
  calibratorsAr["Chip"]                 =&Chip_Ar;
  calibratorsAr["Straw"]                =&Straw_Ar;

  //configure the calibrators
  for (const std::string& s : m_doRt)        calibratorsAr[s]->dort=true;
  for (const std::string& s : m_doT0)        calibratorsAr[s]->dot0=true;
  for (const std::string& s : m_doRes)       calibratorsAr[s]->dores=true;
  for (const std::string& s : m_beQuiet)     calibratorsAr[s]->bequiet=true;
  for (const std::string& s : m_doOutPrint)  calibratorsAr[s]->printt0=true;
  for (const std::string& s : m_doRtPrint)   calibratorsAr[s]->printrt=true;
  for (const std::string& s : m_doLogPrint)  calibratorsAr[s]->printlog=true;
  for (const std::string& s : m_useBoardRef) calibratorsAr[s]->usebref=true;
  for (std::pair<const std::string, Calibrator*>& p : calibratorsAr) {
    Calibrator* calib = p.second;
    calib->usep0=m_useP0;
    calib->floatp3=m_floatP3;
    calib->useshortstraws=m_DoShortStrawCorrection;

    if (m_doRt.size()==0) calib->nort=true;
    if (m_doT0.size()==0) calib->not0=true;

    std::set<int> selset;
    GetSubLevels(m_selstring,calib->level+1,&selset); //get the set of selections for level "level+1" from the m_selstring ... 
    for (std::set<int>::iterator imod=selset.begin(); imod!=selset.end(); ++imod){
      calib->selection = selset;//... and configure the calibrator with them
    }
  }



  m_options = '_' + TRT.GetOptString() + '_' + Detector.GetOptString() + '_' + Layer.GetOptString() + '_' + Module.GetOptString() + '_' + Board.GetOptString() + '_' + Chip.GetOptString() + '_' + Straw.GetOptString();
  if(m_calsub!="user") m_selstring = '_' + TRT.GetSelString() + '_' + Detector.GetSelString() + '_' + Layer.GetSelString() + '_' + Module.GetSelString() + '_' + Board.GetSelString() + '_' + Chip.GetSelString() + '_' + Straw.GetSelString();
  
  //print some info
  msg(MSG::INFO) << endmsg;
  msg(MSG::INFO) << "INPUT FILE       : " << infile << endmsg; 
  msg(MSG::INFO) << "OUTPUT FILE      : " << outfile << endmsg; 
  msg(MSG::INFO) << "SELECTION STRING : " << m_selstring << endmsg; 
  msg(MSG::INFO) << "OPTION STRING    : " << m_options << endmsg; 
  msg(MSG::INFO) << "RT RELATION      : " << m_rtrel << endmsg; 
  msg(MSG::INFO) << "MIN STATISTICS   : RT=" << m_minrt << ", T0=" << m_mint0 << endmsg; 
  msg(MSG::INFO) << "T0 OFFSET        : " << m_t0offset << endmsg; 
  msg(MSG::INFO) << endmsg;
  msg(MSG::INFO) << TRT.PrintInfo() << endmsg;
  msg(MSG::INFO) << Detector.PrintInfo() << endmsg;
  msg(MSG::INFO) << Layer.PrintInfo() << endmsg;
  msg(MSG::INFO) << Module.PrintInfo() << endmsg;
  msg(MSG::INFO) << Board.PrintInfo() << endmsg;
  msg(MSG::INFO) << Chip.PrintInfo() << endmsg;
  msg(MSG::INFO) << Straw.PrintInfo() << endmsg;
  msg(MSG::INFO) << endmsg;

  if (m_DoArXenonSep){
  //print some info
        msg(MSG::INFO) << endmsg;
        msg(MSG::INFO) << "Setup for the ARGON Straws: " <<endmsg;
        msg(MSG::INFO) << "INPUT FILE       : " << infile << endmsg;
        msg(MSG::INFO) << "OUTPUT FILE      : " << outfile << endmsg;
        msg(MSG::INFO) << "SELECTION STRING : " << m_selstring << endmsg;
        msg(MSG::INFO) << "OPTION STRING    : " << m_options << endmsg;
        msg(MSG::INFO) << "RT RELATION      : " << m_rtrel << endmsg;
        msg(MSG::INFO) << "MIN STATISTICS   : RT=" << m_minrt << ", T0=" << m_mint0 << endmsg;
        msg(MSG::INFO) << "T0 OFFSET        : " << m_t0offset << endmsg;
        msg(MSG::INFO) << endmsg;
        msg(MSG::INFO) << TRT_Ar.PrintInfo() << endmsg;
        msg(MSG::INFO) << Detector_Ar.PrintInfo() << endmsg;
        msg(MSG::INFO) << Layer_Ar.PrintInfo() << endmsg;
        msg(MSG::INFO) << Module_Ar.PrintInfo() << endmsg;
        msg(MSG::INFO) << Board_Ar.PrintInfo() << endmsg;
        msg(MSG::INFO) << Chip_Ar.PrintInfo() << endmsg;
        msg(MSG::INFO) << Straw_Ar.PrintInfo() << endmsg;
        msg(MSG::INFO) << endmsg;
  }
  //read the chip reference t0 values from finedelays.txt
  int rbrd, rchp, rdet, dum;
  float rt0;
  std::map<std::string,float> reft0map;
  std::ifstream t0ref("finedelays.txt",std::ios::in);
  for(int iref=0;iref<208;iref++){
    t0ref >> rbrd >> rchp >> rdet >> dum >> rt0;
    reft0map[std::string(Form("_%i_%i_%i",rdet,rbrd,rchp))]=rt0;
  }
  t0ref.close();

  //set type of rt-relation
  bool isdines = m_rtrel.find("dines")!=std::string::npos;
  bool isbinned = m_rtrel.find("binned")!=std::string::npos;
  int rtint;
  if (isdines) rtint=2;
  else if (isbinned) rtint=1;
  else rtint=0;
  
  int npop,isid;
  std::map<std::string,epdata> ephasemap;

  //determine the input file type
  char filetype[5];
  std::ifstream myFile (infile.data(), std::ios::in | std::ios::binary);
  bool isntuple=false;
  if(myFile.is_open()){
    myFile.read ((char*)filetype, 4); filetype[4]=0;
    if (strcmp(filetype,"root")==0) isntuple=true;
    //if (strcmp(filetype,"bina")==0) return 3;
    myFile.close();
  }
  else
    std::cout << "FILE NOT FOUND!" << std::endl;

  if(!isntuple){  // LOOP OVER STRAW HISTOGRAMS IN CASE OF BINARY HISTOGRAMS =====================================
    
    std::ifstream myFile (infile.data(), std::ios::in | std::ios::binary);
    
    int ihist=0;
    int ihistAr=0;
    //for (int itemp=0;itemp<20;itemp++){
    while(true){

      //read a binary histogram
      myFile.read ((char*)&npop,sizeof(int)); //number of populated bins
      if (myFile.eof()) break;
      int* chist=new int[2*npop+2]; //the histogram
      if (npop>0) myFile.read ((char*)(chist+2), sizeof(int)*2*npop);
      myFile.read ((char*)&isid,sizeof(int)); //the straw id 
      sid = (float)isid;

      chist[0]=npop;
      chist[1]=isid;
     
      //get the straw address (barrel/ec, layer, module, ...) based on the straw identifier
      ident=(Identifier)(isid);
      int chip=0,board=-1;
      m_neighbourSvc->getChip(ident,chip); 
      if(abs(m_TRTID->barrel_ec(ident))<2){
        board=m_neighbourSvc->chipToBoardBarrel(chip,m_TRTID->layer_or_wheel(ident));
      }
      else if (chip<12) {
        board=0;
      }
      else {
        chip=chip-20;
        board=1;
      }
      if (m_SplitBarrel) hitdata.det=(int)m_TRTID->barrel_ec(ident); 
      else hitdata.det=abs((int)m_TRTID->barrel_ec(ident)); 
      hitdata.lay=(int)m_TRTID->layer_or_wheel(ident); 
      hitdata.mod=(int)m_TRTID->phi_module(ident); 
      hitdata.brd=board; 
      hitdata.chp=chip; 
      hitdata.stl=(int)m_TRTID->straw_layer(ident); 
      hitdata.stw=(int)m_TRTID->straw(ident);
      hitdata.sid=isid;
      
      //get the old rt parameters based on straw identifier
      const float* pcal ;
      std::vector<float> rvalues ;
      const float defaultpcal[] = {0,0,0,0} ;
      if (isdines){
        const TRTCond::DinesRtRelation* rtr = dynamic_cast<const TRTCond::DinesRtRelation*>(m_trtcaldbTool->getRtRelation(ident)) ;
        pcal = rtr ? rtr->cal() : defaultpcal ;
      }

      else {
        const TRTCond::BasicRtRelation* rtr = dynamic_cast<const TRTCond::BasicRtRelation*>(m_trtcaldbTool->getRtRelation(ident)) ;
        pcal = rtr ? rtr->cal() : defaultpcal ;
      }
      hitdata.rtpar[0]=pcal[0];
      hitdata.rtpar[1]=pcal[1];
      hitdata.rtpar[2]=pcal[2];
      hitdata.rtpar[3]=pcal[3];
      
      //cout "RT= " << tmin << " " << tmax << " " << rvalues.size() << " " << rvalues[0] << " " << rvalues[1] << endl;

      //build map keys
      MakeBDKeys(hitdata.det, hitdata.lay, hitdata.mod, hitdata.brd, hitdata.chp, hitdata.sid);
      
      //make the level hierachy dictionaries
      m_trt.t[m_Tkey].d[m_Dkey].l[m_Lkey].m[m_Mkey].b[m_Bkey].c[m_Ckey].s[m_Skey].z=0;
      m_trt_acc.t[m_Tkey].d[m_Dkey_acc].l[m_Lkey_acc].m[m_Mkey].b[m_Bkey].c[m_Ckey].s[m_Skey].z=0;
      
      //populate the hit data structure to be added 
      strawelement = m_trtmanager->getElement(ident);
      hitdata.ievt=0; 
      hitdata.tres=0; 
      hitdata.weight=1.0;
      hitdata.res=0; 
      hitdata.t=0; 
      hitdata.r=0;
      hitdata.t0=m_trtcaldbTool->getT0(ident);
      hitdata.x=(strawelement->center(ident)).x();
      hitdata.y=(strawelement->center(ident)).y();
      hitdata.z=(strawelement->center(ident)).z();
      
      //in the short straws corrections, autodetect if it was applied on the previous step
      if ( m_DoShortStrawCorrection &&  hitdata.lay==0 && hitdata.stl<9){
        //If correction was done in ctes in db (readed), undo the correction: This fixes problem on t0 averaging....
        double t0test1=m_trtcaldbTool->getT0((Identifier)301998432);
        double t0test2=m_trtcaldbTool->getT0((Identifier)302001504);
        if (t0test1 != t0test2) hitdata.t0+=0.75; //short straw compensation
      }
      
      //the reference t0 from finedelays
      hitdata.rt0=reft0map[std::string(Form("_%i_%i_%i",hitdata.det,hitdata.brd,hitdata.chp))];
     
           // Prepare for Xe-Ar mixed conditions:
            int isArgonStraw = 0;
            if (m_TRTStrawSummaryTool->getStatusHT(ident) != TRTCond::StrawStatus::Good) {
                  isArgonStraw = 1;
            }

 
      //add histogram to the Calibrators (A and C side separated)
   if(!m_DoArXenonSep){
      nTRThist += TRT.AddHit(m_Tkey,hitdata,chist,true);
      if (Detector.CheckSelection(hitdata.det)) { //only add the histogram if it is in the selection
        if (m_SplitBarrel) ndethist += Detector.AddHit(m_Dkey,hitdata,chist,true);
        else ndethist += Detector.AddHit(m_Dkey_acc,hitdata,chist,true);
        if (Layer.CheckSelection(hitdata.lay)) {
          if (m_SplitBarrel) nlayhist += Layer.AddHit(m_Lkey,hitdata,chist,true);
          else { nlayhist += Layer.AddHit(m_Lkey_acc,hitdata,chist,true); continue;}
          if (Module.CheckSelection(hitdata.mod)) {
            nmodhist += Module.AddHit(m_Mkey,hitdata,chist,true);
            if (Board.CheckSelection(hitdata.brd)) {
              nbrdhist += Board.AddHit(m_Bkey,hitdata,chist,true);
              if (Chip.CheckSelection(hitdata.chp)) {
                nchphist += Chip.AddHit(m_Ckey,hitdata,chist,true);
                if (Straw.CheckSelection(hitdata.stw))
                  nstwhist += Straw.AddHit(m_Skey,hitdata,chist,true);
              }
            }
          }
        }
      }                         // Here it closes
    ihist++;
    }

    else{
     if(isArgonStraw==0){
      nTRThist += TRT.AddHit(m_Tkey,hitdata,chist,true);
      if (Detector.CheckSelection(hitdata.det)) { //only add the histogram if it is in the selection
        if (m_SplitBarrel) ndethist += Detector.AddHit(m_Dkey,hitdata,chist,true);
        else ndethist += Detector.AddHit(m_Dkey_acc,hitdata,chist,true);
        if (Layer.CheckSelection(hitdata.lay)) {
          if (m_SplitBarrel) nlayhist += Layer.AddHit(m_Lkey,hitdata,chist,true);
          else { nlayhist += Layer.AddHit(m_Lkey_acc,hitdata,chist,true); continue;}
          if (Module.CheckSelection(hitdata.mod)) {
            nmodhist += Module.AddHit(m_Mkey,hitdata,chist,true);
            if (Board.CheckSelection(hitdata.brd)) {
              nbrdhist += Board.AddHit(m_Bkey,hitdata,chist,true);
              if (Chip.CheckSelection(hitdata.chp)) {
                nchphist += Chip.AddHit(m_Ckey,hitdata,chist,true);
                if (Straw.CheckSelection(hitdata.stw))
                  nstwhist += Straw.AddHit(m_Skey,hitdata,chist,true);
              }
            }
          }
        }
      }                         // Here it closes
      ihist++;
    }

    else{               // ARGON HITS
      nTRThistAr += TRT_Ar.AddHit(m_Tkey,hitdata,chist,true);
      if (Detector_Ar.CheckSelection(hitdata.det)) { //only add the histogram if it is in the selection
        if (m_SplitBarrel) ndethistAr += Detector_Ar.AddHit(m_Dkey,hitdata,chist,true);
        else ndethistAr += Detector_Ar.AddHit(m_Dkey_acc,hitdata,chist,true);
        if (Layer_Ar.CheckSelection(hitdata.lay)) {
          if (m_SplitBarrel) nlayhistAr += Layer_Ar.AddHit(m_Lkey,hitdata,chist,true);
          else { nlayhistAr += Layer_Ar.AddHit(m_Lkey_acc,hitdata,chist,true); continue;}
          if (Module_Ar.CheckSelection(hitdata.mod)) {
            nmodhistAr += Module_Ar.AddHit(m_Mkey,hitdata,chist,true);
            if (Board_Ar.CheckSelection(hitdata.brd)) {
              nbrdhistAr += Board_Ar.AddHit(m_Bkey,hitdata,chist,true);
              if (Chip_Ar.CheckSelection(hitdata.chp)) {
                nchphistAr += Chip_Ar.AddHit(m_Ckey,hitdata,chist,true);
                if (Straw_Ar.CheckSelection(hitdata.stw))
                  nstwhistAr += Straw_Ar.AddHit(m_Skey,hitdata,chist,true);
              }
            }
          }
        }
      }                         // Here it closes
    ihistAr++;
    }                   // END ARGON HITS


    }           //CLOSE THE IF FOR ARXE



    if ((ihist%10000==9999) | (ihist==m_nevents-1)){
      msg(MSG::INFO)  << Form("%7i HISTOGRAMS READ, UNITS ADDED: %i %i %2i %3i %3i %4i %6i",ihist+1,nTRThist, ndethist, nlayhist, nmodhist, nbrdhist, nchphist, nstwhist) << endmsg;
      if(m_DoArXenonSep) msg(MSG::INFO)  << Form("%7i Ar HISTOGRAMS READ, UNITS ADDED: %i %i %2i %3i %3i %4i %6i",ihistAr+1,nTRThistAr, ndethistAr, nlayhistAr, nmodhistAr, nbrdhistAr, nchphistAr, nstwhistAr) << endmsg;
      //nTRThist=ndethist=nlayhist=nmodhist=nbrdhist=nchphist=nstwhist=0;
    }
      
      ihist++;
      delete [] chist;
    } // Finish Straw Loop   
    msg(MSG::INFO)  << Form("%7i HISTOGRAMS READ, UNITS ADDED: %i %i %2i %3i %3i %4i %6i",ihist+1,nTRThist, ndethist, nlayhist, nmodhist, nbrdhist, nchphist, nstwhist) << endmsg;
    if(m_DoArXenonSep) msg(MSG::INFO)  << Form("%7i Ar HISTOGRAMS READ, UNITS ADDED: %i %i %2i %3i %3i %4i %6i",ihistAr+1,nTRThistAr, ndethistAr, nlayhistAr, nmodhistAr, nbrdhistAr, nchphistAr, nstwhistAr) << endmsg;

    TRT.UpdateOldConstants();
    Detector.UpdateOldConstants();
    Layer.UpdateOldConstants();
    Module.UpdateOldConstants();
    Board.UpdateOldConstants();
    Chip.UpdateOldConstants();

    if (ihistAr>0 ){
        TRT_Ar.UpdateOldConstants();
        Detector_Ar.UpdateOldConstants();
        Layer_Ar.UpdateOldConstants();
        Module_Ar.UpdateOldConstants();
        Board_Ar.UpdateOldConstants();
        Chip_Ar.UpdateOldConstants();
    }

    myFile.close();

  } else { // LOOP OVER HITS IN CASE OF ROOT NTUPLE =====================================

    msg(MSG::INFO)  <<  " READING basic NTUPLE is HIGHLY DEPRECATED!!! Do NOT use it!!!" << endmsg;
    
    //open the ntuple
    TFile* ntfile=new TFile(infile.data());
    if (!(ntfile->IsZombie())){
      TNtuple* hittuple=(TNtuple*)ntfile->Get("ntuple");
      
      hittuple->SetBranchAddress("run",&run);
      hittuple->SetBranchAddress("evt",&evt);
      hittuple->SetBranchAddress("trk",&trk);
      hittuple->SetBranchAddress("sid",&sid);
      hittuple->SetBranchAddress("r",&r);
      hittuple->SetBranchAddress("dr",&dr);
      hittuple->SetBranchAddress("t",&t);
      hittuple->SetBranchAddress("rtrack",&rtrack);
      hittuple->SetBranchAddress("ttrack",&ttrack);
      hittuple->SetBranchAddress("t0",&t0);
      hittuple->SetBranchAddress("ephase",&ephase);
      
      if (m_nevents==-1) m_nevents = hittuple->GetEntries();
      
      //loop a 1st time over the hittuple and calculate event phase and other track related data;
      msg(MSG::INFO)  << "CALCULATING TRACK DATA!" << endmsg;
      for (int ievt=0;ievt<m_nevents;ievt++){

        if ((ievt%1000000==999999) | (ievt==m_nevents-1)) {
          msg(MSG::INFO) << ievt+1 << " HITS READ! " << endmsg; 
        }

        hittuple->GetEntry(ievt);

        std::string epkey=std::string(Form("_%i_%i_%i",(int)run,(int)evt,(int)trk));

        if (ephasemap.find(epkey) == ephasemap.end()){
          ephasemap[epkey].run=run;
          ephasemap[epkey].event=evt;
          ephasemap[epkey].track=trk;
          ephasemap[epkey].ephase1=(t+ephase)-t0-ttrack;
          ephasemap[epkey].ephase2=ephase;
          ephasemap[epkey].nhits=1.0;
          ephasemap[epkey].t=t;
          ephasemap[epkey].ttrack=ttrack;
          ephasemap[epkey].t0=t0;
        }
        else {
          ephasemap[epkey].ephase1+=(t+ephase)-t0-ttrack;
          ephasemap[epkey].t+=t;
          ephasemap[epkey].ttrack+=ttrack;
          ephasemap[epkey].t0+=t0;
          ephasemap[epkey].nhits++;
        }
      }
      
      //loop a 2nd time over the hittuple and add hits to the Calibrators
      msg(MSG::INFO)  << "READING AND ADDING HITS!" << endmsg;
      for (int ievt=0;ievt<m_nevents;ievt++){

        if ((ievt%100000==99999) | (ievt==m_nevents-1)){
          msg(MSG::INFO)  << ievt+1 << " HITS READ! " << " " << nTRThist << " " << ndethist << " " << nlayhist << " " << nmodhist << " " << nbrdhist << " " << nchphist << " " << nstwhist << endmsg;
          nTRThist=ndethist=nlayhist=nmodhist=nbrdhist=nchphist=nstwhist=0;
        }

        hittuple->GetEntry(ievt);

        //get the straw address (barrel/ec, layer, module, ...) based on the straw identifier
        ident=(Identifier)((int)sid);
        int chip=0,board=-1;
        m_neighbourSvc->getChip(ident,chip); 
        if(abs(m_TRTID->barrel_ec(ident))<2){
          board=m_neighbourSvc->chipToBoardBarrel(chip,m_TRTID->layer_or_wheel(ident));
        }
        else
          if (chip<12)
            board=0;
          else {
            chip=chip-20;
            board=1;
          }
        hitdata.det=(int)m_TRTID->barrel_ec(ident); 
        hitdata.lay=(int)m_TRTID->layer_or_wheel(ident); 
        hitdata.mod=(int)m_TRTID->phi_module(ident); 
        hitdata.brd=board; 
        hitdata.chp=chip; 
        hitdata.stl=(int)m_TRTID->straw_layer(ident); 
        hitdata.stw=(int)m_TRTID->straw(ident); 
        hitdata.sid=(int)sid;

        //only add the hit if the straw is included in the part of the TRT that is supposed to be calibrated
        if (Detector.CheckSelection(hitdata.det) && Layer.CheckSelection(hitdata.lay)){

          //build map keys
          MakeBDKeys(hitdata.det, hitdata.lay, hitdata.mod, hitdata.brd, hitdata.chp, hitdata.sid);

          //make the level hierachy dictionary
          m_trt.t[m_Tkey].d[m_Dkey].l[m_Lkey].m[m_Mkey].b[m_Bkey].c[m_Ckey].s[m_Skey].z=0;

          //populate the hit data structure to be added 
          strawelement = m_trtmanager->getElement(ident);
          hitdata.ievt=ievt; 
          hitdata.tres=t-ttrack-t0; 
          hitdata.weight=1.0;
          hitdata.res=r-rtrack; 
          //hitdata.t=t-t0+averageT0-20.0; 
          hitdata.t=t-t0; 
          hitdata.r=std::abs(rtrack);
          hitdata.t0=m_trtcaldbTool->getT0(ident);
          hitdata.rt0=reft0map[std::string(Form("_%i_%i_%i",hitdata.det,hitdata.brd,hitdata.chp))];
          hitdata.x=(strawelement->center(ident)).x();
          hitdata.y=(strawelement->center(ident)).y();
          hitdata.z=(strawelement->center(ident)).z();

          //add the hit to the Calibrators on all levelels 
          nTRThist += TRT.AddHit(m_Tkey,hitdata,nullptr,true);
          ndethist += Detector.AddHit(m_Dkey,hitdata,nullptr,true);
          nlayhist += Layer.AddHit(m_Lkey,hitdata,nullptr,true);
          nmodhist += Module.AddHit(m_Mkey,hitdata,nullptr,true);    
          nbrdhist += Board.AddHit(m_Bkey,hitdata,nullptr,true);    
          nchphist += Chip.AddHit(m_Ckey,hitdata,nullptr,true);    
          nstwhist += Straw.AddHit(m_Skey,hitdata,nullptr,true);   

          if (msgLvl(MSG::DEBUG)) msg() << hitdata.sid << endmsg;
          if (msgLvl(MSG::DEBUG)) msg() << m_TRTID->barrel_ec(ident) << endmsg;
          if (msgLvl(MSG::DEBUG)) msg() << m_TRTID->layer_or_wheel(ident) << endmsg;
          if (msgLvl(MSG::DEBUG)) msg() << m_TRTID->phi_module(ident) << endmsg;
          if (msgLvl(MSG::DEBUG)) msg() << m_TRTID->straw_layer(ident) << endmsg;
          if (msgLvl(MSG::DEBUG)) msg() << m_TRTID->straw(ident) << endmsg;
          if (msgLvl(MSG::DEBUG)) msg() << chip << endmsg;
          if (msgLvl(MSG::DEBUG)) msg() << board << endmsg;
          if (msgLvl(MSG::DEBUG)) msg() << (strawelement->center(ident)).x() << endmsg;
          if (msgLvl(MSG::DEBUG)) msg() << (strawelement->center(ident)).y() << endmsg;
          if (msgLvl(MSG::DEBUG)) msg() << (strawelement->center(ident)).z() << endmsg;

        }

      }// for .. event loop
    }// if .. file ok
  }// if .. binhist or ntuple

  msg(MSG::INFO) << endmsg;
  msg(MSG::INFO) << TRT.PrintStat() << endmsg;
  msg(MSG::INFO) << Detector.PrintStat() << endmsg;
  msg(MSG::INFO) << Layer.PrintStat() << endmsg;
  msg(MSG::INFO) << Module.PrintStat() << endmsg;
  msg(MSG::INFO) << Board.PrintStat() << endmsg;
  msg(MSG::INFO) << Chip.PrintStat() << endmsg;
  msg(MSG::INFO) << Straw.PrintStat() << endmsg;
  msg(MSG::INFO) << endmsg;


  if(m_DoArXenonSep){
          msg(MSG::INFO) << endmsg;
          msg(MSG::INFO) << TRT_Ar.PrintStat() << endmsg;
          msg(MSG::INFO) << Detector_Ar.PrintStat() << endmsg;
          msg(MSG::INFO) << Layer_Ar.PrintStat() << endmsg;
          msg(MSG::INFO) << Module_Ar.PrintStat() << endmsg;
          msg(MSG::INFO) << Board_Ar.PrintStat() << endmsg;
          msg(MSG::INFO) << Chip_Ar.PrintStat() << endmsg;
          msg(MSG::INFO) << Straw_Ar.PrintStat() << endmsg;
          msg(MSG::INFO) << endmsg;
  }



  //caldata startdata(true,tbins,rbins);
  caldata startdata(true,55,100);
  //caldata startdata(true,64,64);
  startdata.t0=20.0;
  std::map<std::string,TDirectory*> dirmap;
  TDirectory* trtdir=gDirectory;
  TDirectory* detdir=gDirectory;
  TDirectory* laydir=gDirectory;
  TDirectory* moddir=gDirectory;
  TDirectory* brddir=gDirectory;
  TDirectory* chpdir=gDirectory;

  std::ofstream rtcalfile(Form("%s_rt.txt",outfile.data()),std::ios::out);
  std::ofstream binrtcalfile(Form("%s_binrt.txt",outfile.data()),std::ios::out);
  std::ofstream t0calfile(Form("%s_t0.txt",outfile.data()),std::ios::out);


  //if (m_SplitBarrel){
  
  for (std::pair<const std::string, BDdetector>& pt : m_trt.t) {
    
    if (TRT.Skip()) break;
    if (TRT.HasKey(pt.first)) {
      trtdir = TRT.Calibrate(m_histfile,pt.first,SubLev(m_options,1),&startdata);
      if (TRT.printt0) t0calfile << Form("-3 -1 -1 -1 -1 : %e %e",TRT.data[pt.first].t0,TRT.data[pt.first].t0err) << std::endl;
      if (TRT.printrt) rtcalfile << Form("-3 -1 -1 -1 -1 : %i %e %e %e %e",rtint,TRT.data[pt.first].rtpar[0],TRT.data[pt.first].rtpar[1],TRT.data[pt.first].rtpar[2],TRT.data[pt.first].rtpar[3]) << std::endl;
    }
    for (std::pair<const std::string, BDlayer>& pd : pt.second.d) {
      
      if(Detector.Skip()) break; 
      if(Detector.HasKey(pd.first)){
        detdir = Detector.Calibrate(trtdir,pd.first,SubLev(m_options,2),&TRT.data[pt.first]);
      }
      for (std::pair<const std::string, BDmodule>& pl : pd.second.l) {

        if(Layer.Skip()) break;
        if(Layer.HasKey(pl.first)){
          laydir = Layer.Calibrate(detdir,pl.first,SubLev(m_options,3),&Detector.data[pd.first]);
          if (Layer.printt0) t0calfile << Form("%i %i -1 -1 -1 : %e %e",Layer.data[pl.first].det,Layer.data[pl.first].lay,Layer.data[pl.first].t0,Layer.data[pl.first].t0err) << std::endl;
          if (Layer.printrt) rtcalfile    << Form("%i %i -1 -1 -1 : %i %e %e %e %e",Layer.data[pl.first].det,Layer.data[pl.first].lay,rtint,Layer.data[pl.first].rtpar[0],Layer.data[pl.first].rtpar[1],Layer.data[pl.first].rtpar[2],Layer.data[pl.first].rtpar[3]) << std::endl;
          if (!m_SplitBarrel) {
            if (Layer.printt0) t0calfile    << Form("%i %i -1 -1 -1 : %e %e",-Layer.data[pl.first].det,Layer.data[pl.first].lay,Layer.data[pl.first].t0,Layer.data[pl.first].t0err) << std::endl;
            if (Layer.printrt) rtcalfile    << Form("%i %i -1 -1 -1 : %i %e %e %e %e",-Layer.data[pl.first].det,Layer.data[pl.first].lay,rtint,Layer.data[pl.first].rtpar[0],Layer.data[pl.first].rtpar[1],Layer.data[pl.first].rtpar[2],Layer.data[pl.first].rtpar[3]) << std::endl;
          }
        }
        for (std::pair<const std::string, BDboard>& pm : pl.second.m) {

          if(Module.Skip()) break; 
          if(Module.HasKey(pm.first)){ 
            moddir = Module.Calibrate(laydir,pm.first,SubLev(m_options,4),&Layer.data[pl.first]);
            if (Module.printt0) t0calfile << Form("%i %i %i -1 -1 : %e %e",Module.data[pm.first].det,Module.data[pm.first].lay,Module.data[pm.first].mod,Module.data[pm.first].t0,Module.data[pm.first].t0err) << std::endl;
            if (Module.printrt) rtcalfile    << Form("%i %i %i -1 -1 : %i %e %e %e %e",Module.data[pm.first].det,Module.data[pm.first].lay,Module.data[pm.first].mod,rtint,Module.data[pm.first].rtpar[0],Module.data[pm.first].rtpar[1],Module.data[pm.first].rtpar[2],Module.data[pm.first].rtpar[3]) << std::endl;
          } 
          for (std::pair<const std::string, BDchip>& pb : pm.second.b) {

            if(Board.Skip()) break; 
            if(Board.HasKey(pb.first)){ 
              brddir = Board.Calibrate(moddir,pb.first,SubLev(m_options,5),&Module.data[pm.first]);
            } 
            for (std::pair<const std::string, BDstraw>& pc : pb.second.c) {

              if(Chip.Skip()) break; 
              if(Chip.HasKey(pc.first)){ 
                chpdir = Chip.Calibrate(brddir,pc.first,SubLev(m_options,6),&Board.data[pb.first]);
              }
              for (std::pair<const std::string, BDzero>& ps : pc.second.s) {

                if(Straw.Skip()) break;
                if(Straw.HasKey(ps.first)){ 
                  Straw.Calibrate(chpdir,ps.first,SubLev(m_options,7),&Chip.data[pc.first]);
                  if (Straw.printt0) t0calfile << Form("%i %i %i %i %i : %e %e",Straw.data[ps.first].det,Straw.data[ps.first].lay,Straw.data[ps.first].mod,Straw.data[ps.first].stl,Straw.data[ps.first].stw,Straw.data[ps.first].t0,Straw.data[ps.first].t0err) << std::endl;
                  if (Straw.printrt) rtcalfile << Form("%i %i %i %i %i : %i %e %e %e %e",Straw.data[ps.first].det,Straw.data[ps.first].lay,Straw.data[ps.first].mod,Straw.data[ps.first].stl,Straw.data[ps.first].stw,rtint,Straw.data[ps.first].rtpar[0],Straw.data[ps.first].rtpar[1],Straw.data[ps.first].rtpar[2],Straw.data[ps.first].rtpar[3]) << std::endl;
                

                }
              }
            }
          }
        }
      }
    }
  }

  std::map<std::string,TDirectory*> dirArmap;
  TDirectory* trtdirAr=gDirectory;
  TDirectory* detdirAr=gDirectory;
  TDirectory* laydirAr=gDirectory;
  TDirectory* moddirAr=gDirectory;
  TDirectory* brddirAr=gDirectory;
  TDirectory* chpdirAr=gDirectory;

  if (m_DoArXenonSep){

  for (std::pair<const std::string, BDdetector>& pt : m_trt.t) {

    if (TRT_Ar.Skip()) break;
    if (TRT_Ar.HasKey(pt.first)) {
      trtdirAr = TRT_Ar.Calibrate(m_histfile,pt.first,SubLev(m_options,1),&startdata);
    }
    for (std::pair<const std::string, BDlayer>& pd : pt.second.d) {

      if(Detector_Ar.Skip()) break;
      if(Detector_Ar.HasKey(pd.first)){
        detdirAr = Detector_Ar.Calibrate(trtdirAr,pd.first,SubLev(m_options,2),&TRT_Ar.data[pt.first]);
      }
      for (std::pair<const std::string, BDmodule>& pl : pd.second.l) {

        if(Layer_Ar.Skip()) break;
        if(Layer_Ar.HasKey(pl.first)){
          laydirAr = Layer_Ar.Calibrate(detdirAr,pl.first,SubLev(m_options,3),&Detector_Ar.data[pd.first]);
       
        }
        for (std::pair<const std::string, BDboard>& pm : pl.second.m) {

          if(Module_Ar.Skip()) break;
          if(Module_Ar.HasKey(pm.first)){
            moddirAr = Module_Ar.Calibrate(laydirAr,pm.first,SubLev(m_options,4),&Layer_Ar.data[pl.first]);
            if (Module_Ar.printt0) t0calfile << Form("%i %i %i -1 -1 : %e %e",Module_Ar.data[pm.first].det,Module_Ar.data[pm.first].lay,Module_Ar.data[pm.first].mod,Module_Ar.data[pm.first].t0,Module_Ar.data[pm.first].t0err) << std::endl;
            if (Layer_Ar.printrt) rtcalfile    << Form("%i %i %i -1 -1 : %i %e %e %e %e",Module_Ar.data[pm.first].det,Module_Ar.data[pm.first].lay,Module_Ar.data[pm.first].mod,rtint,Module_Ar.data[pm.first].rtpar[0],Module_Ar.data[pm.first].rtpar[1],Module_Ar.data[pm.first].rtpar[2],Module_Ar.data[pm.first].rtpar[3]) << std::endl;
          }
          for (std::pair<const std::string, BDchip>& pb : pm.second.b) {
            if(Board_Ar.Skip()) break;
            if(Board_Ar.HasKey(pb.first)){
              brddirAr = Board_Ar.Calibrate(moddirAr,pb.first,SubLev(m_options,5),&Module_Ar.data[pm.first]);
            }
            for (std::pair<const std::string, BDstraw>& pc : pb.second.c) {

              if(Chip_Ar.Skip()) break;
              if(Chip_Ar.HasKey(pc.first)){
                chpdirAr = Chip_Ar.Calibrate(brddirAr,pc.first,SubLev(m_options,6),&Board_Ar.data[pb.first]);
              }
              for (std::pair<const std::string, BDzero>& ps : pc.second.s) {

                if(Straw_Ar.Skip()) break;
                if(Straw_Ar.HasKey(ps.first)){
                  Straw_Ar.Calibrate(chpdirAr,ps.first,SubLev(m_options,7),&Chip_Ar.data[pc.first]);
                  if (Straw_Ar.printt0) t0calfile << Form("%i %i %i %i %i : %e %e",Straw_Ar.data[ps.first].det,Straw_Ar.data[ps.first].lay,Straw_Ar.data[ps.first].mod,Straw_Ar.data[ps.first].stl,Straw_Ar.data[ps.first].stw,Straw_Ar.data[ps.first].t0,Straw_Ar.data[ps.first].t0err) << std::endl;
                  if (Straw_Ar.printrt) rtcalfile << Form("%i %i %i %i %i : %i %e %e %e %e",Straw_Ar.data[ps.first].det,Straw_Ar.data[ps.first].lay,Straw_Ar.data[ps.first].mod,Straw_Ar.data[ps.first].stl,Straw_Ar.data[ps.first].stw,rtint,Straw_Ar.data[ps.first].rtpar[0],Straw_Ar.data[ps.first].rtpar[1],Straw_Ar.data[ps.first].rtpar[2],Straw_Ar.data[ps.first].rtpar[3]) << std::endl;
                }
              }
            }
          }
        }
      }
    }
  }

}// Close for ARGON 


  
  
  //make ntuples for each calibrator 
  if (!TRT.bequiet) TRT.WriteStat(m_histfile);
  if (!Detector.bequiet) Detector.WriteStat(m_histfile);
  if (!Layer.bequiet) Layer.WriteStat(m_histfile);
  if (!Module.bequiet) Module.WriteStat(m_histfile);
  if (!Board.bequiet) Board.WriteStat(m_histfile);
  Chip.WriteStat(m_histfile);
  Straw.WriteStat(m_histfile);

  TRT.DumpConstants();
  Detector.DumpConstants();
  Layer.DumpConstants();
  Module.DumpConstants();
  Board.DumpConstants();
  Chip.DumpConstants();
  Straw.DumpConstants();
  
  rtcalfile.close();
  binrtcalfile.close();
  t0calfile.close();

        //AR
  //make ntuples for each calibrator 
  if(m_DoArXenonSep){
   if (!TRT_Ar.bequiet)          TRT_Ar.WriteStat(m_histfile);
   if (!Detector_Ar.bequiet)     Detector_Ar.WriteStat(m_histfile);
   if (!Layer_Ar.bequiet)        Layer_Ar.WriteStat(m_histfile);
   if (!Module_Ar.bequiet)       Module_Ar.WriteStat(m_histfile);
   if (!Board_Ar.bequiet)        Board_Ar.WriteStat(m_histfile);
   Chip_Ar.WriteStat(m_histfile);
   Straw_Ar.WriteStat(m_histfile);

   TRT_Ar.DumpConstants();
   Detector_Ar.DumpConstants();
   Layer_Ar.DumpConstants();
   Module_Ar.DumpConstants();
   Board_Ar.DumpConstants();
   Chip_Ar.DumpConstants();
   Straw_Ar.DumpConstants();
  }

  return true;
  
}

bool TRTCalibrator::fill(const Trk::Track* aTrack, TRT::TrackInfo* output) {
  if (aTrack==nullptr) {};
  if (output==nullptr) {};
  return true;
}


