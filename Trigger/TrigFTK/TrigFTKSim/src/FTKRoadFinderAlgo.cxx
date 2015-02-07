/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKRoadFinderAlgo.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKRoadFileOutput.h"
#include "TrigFTKSim/FTK_SGRoadOutput.h"
#include "TrigFTKSim/FTK_RegionalRawInput.h"

#include "GaudiKernel/MsgStream.h"

#include <sstream>
#include <iostream>
#include <fstream>

#include "TrigFTKSim/FTK_AMBank.h"
#include "TrigFTKSim/FTK_CompressedAMBank.h"
#include "TrigFTKSim/tsp/FTKTSPBank.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
FTKRoadFinderAlgo::FTKRoadFinderAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_rfobj(), 
  m_hitInputTool("FTK_SGHitInput/FTK_SGHitInput"),
  m_roadOutputTool("FTK_SGRoadOutput/FTK_SGRoadOutput"),
  m_roadMarketTool("FTK_RoadMarketTool/FTK_RoadMarketTool"),
  m_nbanks(0), m_nsubregions(1),
  m_verbosity(0),
  m_BarrelOnly(false), m_EnableFTKSim(true),
  m_MaxMissingPlanes(1), m_RoadWarrior(0), m_KeepRemoved(0),
  m_MaxMissingSCTPairs(0), m_RestrictSctPairModule(false), m_RestrictSctPairLayer(false),
  m_IBLMode(0),
  m_ss_offset_fraction(0),
  m_PixelClusteringMode(0),
  m_SctClustering(0),
  m_SCTTrkMode(false), m_scttrk_tracklist(""),
  m_scttrk_roadlist(""), m_scttrk_sectormap_path(""),
  m_scttrk_nlayers(0), m_scttrk_nsubs(0), m_scttrk_lastlayer(0),
  m_useTSPBank(0), m_useCompressedBank(0), 
  m_BankLevel(1), m_doTSPSim(0), m_minTSPCoverage(0),
  m_setAMSize(0),
  m_doMakeCache(0), m_CachePath("bankcache.root"),
  m_SaveAllRoads(0),
  m_pmap(0x0), m_pmap_unused(0x0),
  m_rmap(0x0), m_rmap_unused(0x0), m_CUR_REGION(-1),
  m_ssmap(0x0), m_ssmap_unused(0x0), m_ssmap_tsp(0x0),
  m_require_first(false),
  m_scttrk_sectormap(0x0),
  m_HWModeSS(0),
  m_pmap_path(), m_pmapunused_path(),
  m_rmap_path(), m_rmapunused_path(),
  m_ssmap_path(), m_ssmapunused_path(), m_ssmaptsp_path(), m_badmap_path(),m_badmap_path2(),
  m_modulelut_path(),
  m_CachedBank(false),
  m_InputFromWrapper(true), m_RegionalWrapper(false),
  m_doroadfile(false), m_roadfilepath("ftkroads.root"),
  m_roadfilesdir("."),
  m_roadmarket(true),
  m_saveroads(true),
  m_useMinimalAMIN(false),
  m_SectorAsPatterns(0),
  m_DCMatchMethod(0),
  m_AutoDisable(false)
{
  // number of banks
  declareProperty("NBanks",m_nbanks);
  declareProperty("NSubRegions",m_nsubregions);
  declareProperty("BarrelOnly",m_BarrelOnly);
  
  declareProperty("pmap_path",m_pmap_path);
  declareProperty("pmapunused_path",m_pmapunused_path);
  declareProperty("rmap_path",m_rmap_path);
  declareProperty("rmapunused_path",m_rmapunused_path);
  declareProperty("ssmap_path",m_ssmap_path);
  declareProperty("ssmapunused_path",m_ssmapunused_path);
  declareProperty("ssmaptsp_path",m_ssmaptsp_path);
  declareProperty("badmap_path",m_badmap_path);
  declareProperty("badmap_path_for_hit",m_badmap_path2);

  declareProperty("ModuleLUTPath",m_modulelut_path);
  
  declareProperty("IBLMode",m_IBLMode);

  declareProperty("PixelClusteringMode",m_PixelClusteringMode,"Pixel clustering correction: 0 simple default, 1 channel center and linear ToT interpolation and account for different pixel lengths");
  declareProperty("SctClustering",m_SctClustering,"Enable SCT clustering: 0 disabled, 1 enabled");

  declareProperty("CachedBank",m_CachedBank);
  declareProperty("patternbankpath",m_patternbankpath,"Array with bank paths");
  declareProperty("bankregion",m_bankregion,"Array of region IDs");
  declareProperty("banksubregion",m_banksubreg,"Array of sub-region IDs");
  declareProperty("bankpatterns",m_bankpatterns,"Max patterns for each bank, -1 means all patterns");

  declareProperty("UseTSPBank",m_useTSPBank,"If true the AM bank is the DB format");
  declareProperty("UseCompressedBank",m_useCompressedBank,"If true the AM bank is the compressed format");
  declareProperty("TSPSimulationLevel",m_doTSPSim,"Level of the TSP simulation: 0 AM, 1 DC, 2 TSP");
  declareProperty("DBBankLevel",m_BankLevel,"ID of the bank, if simulation level is 0");
  declareProperty("TSPMinCoverage",m_minTSPCoverage,"Minimum coverage of the TSP patterns");
  declareProperty("SetAMSize",m_setAMSize,"If 1 or 2 the flag in a TSP simulation decide to set the AM size");
  declareProperty("RoadWarrior",m_RoadWarrior);
  declareProperty("MakeCache",m_doMakeCache,"Enable the cache creation for the TSP bank");
  declareProperty("CachePath",m_CachePath,"Path of the cache file");
  declareProperty("InputFromWrapper",m_InputFromWrapper,"Use the standalone input file format, a.k.a. wrapper files");
  declareProperty("RegionalWrapper", m_RegionalWrapper, "Use wrapper files with regions applied");
  declareProperty("WrapperFiles",m_wrapperpaths,"Paths of the input files in the wrapper format");
  declareProperty("DoRoadFile",m_doroadfile,"Enable the extrnal road output file");
  declareProperty("RoadFile",m_roadfilepath,"Path of the road output file, old format");
  declareProperty("RoadFilesDir",m_roadfilesdir,"Path of the road output directory, old format");
  declareProperty("RoadMarket",m_roadmarket,"Enable the road exchange tool");
  declareProperty("SaveRoads",m_saveroads,"If true (default) the output roads are saved into the SG");

  declareProperty("UseMinimalAMIN",m_useMinimalAMIN);

  declareProperty("HWModeSS",m_HWModeSS,"Enable the HW-like encoding for the SS");

  declareProperty("MaxMissingSCTPairs",m_MaxMissingSCTPairs,"Increase the number of missing SCT modules, enable the transition region fix");
  declareProperty("RestrictSctPairModule",m_RestrictSctPairModule, "Restrict SCT pairs modules in the transition region");
  declareProperty("RestrictSctPairLayer",m_RestrictSctPairLayer,"Restrict SCT layers in the transition region");

  declareProperty("SectorsAsPatterns",m_SectorAsPatterns,"When 1 allows to use a list of sectors as pattern bank");

  declareProperty("DCMatchMethod",m_DCMatchMethod,"Set the DC matching method: 0 through TSP SS organization, 1 direct");
}

FTKRoadFinderAlgo::~FTKRoadFinderAlgo()
{
  if (m_ssmap) delete m_ssmap;
  if (m_ssmap_unused) delete m_ssmap_unused;
  if (m_ssmap_tsp) delete m_ssmap_tsp;
  if (m_rmap) delete m_rmap;
  if (m_rmap_unused) delete m_rmap_unused;
  if (m_pmap) delete m_pmap;
  if (m_pmap_unused) delete m_pmap_unused;

  // the objects derived from the standalone simulation has to be destroyed byt the algorithm itself, no Athena algorithm will take care of that
  if (m_InputFromWrapper) delete m_rfobj.getDataInputModule();
  if (m_doroadfile) delete m_rfobj.getRoadOutputModule();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKRoadFinderAlgo::initialize(){
  // get the reference to the central standalone instance
  FTKSetup &ftkset = FTKSetup::getFTKSetup();

  MsgStream log(msgSvc(), name());
  FTKSetup::getFTKSetup().setMsgStream(&log);
  FTKLogger::SetLogger(this);

  log << MSG::INFO << "FTKRoadFinderAlgo::initialize()" << endreq;
  

  ftkset.setSectorsAsPatterns(m_SectorAsPatterns);
  if (m_SectorAsPatterns) {
      log << MSG::INFO << "SectorsAsPatterns mode enabled with value " << m_SectorAsPatterns << endreq;
  }

  log << MSG::INFO << "IBL mode value: " << m_IBLMode << endreq;
  ftkset.setIBLMode(m_IBLMode);

  log << MSG::INFO << "HWModeSS value: " << m_HWModeSS << endreq;
  ftkset.setHWModeSS(m_HWModeSS);

  log << MSG::INFO << "Read the logical layer definitions" << endreq;
  // Look for the main plane-map
  if (m_pmap_path.empty()) {
    log << MSG::FATAL << "Main plane map definition missing" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    m_pmap = new FTKPlaneMap(m_pmap_path.c_str());
    if (!(*m_pmap)) {
      log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endreq;
      return StatusCode::FAILURE;
    }
  }

  if (!m_pmapunused_path.empty()) {
    log << MSG::INFO << "Read the logical layer definitions for the unused layers" << endreq;
    m_pmap_unused = new FTKPlaneMap(m_pmapunused_path.c_str());
    if (!(*m_pmap_unused)) {
      log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endreq;
      return StatusCode::FAILURE;
    }

  }
  

  // Create the region map objects
  if (m_rmap_path.empty()) {
    log << MSG::FATAL << "Region map file needed" << endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::INFO << "Creating region map" << endreq;
  m_rmap = new FTKRegionMap(m_pmap, m_rmap_path.c_str());
  if (!(*m_rmap)) {
    log << MSG::FATAL << "Error creating region map from: " << m_rmap_path.c_str() << endreq;
    return StatusCode::FAILURE;
  }

  //In case the HWModeSS=2 the LUT to map global->local module ID is required
  if (m_HWModeSS==2) {
    if (m_modulelut_path.empty()) {
      log << MSG::FATAL << "A module LUT is required when HW SS calculation is required" << m_rmap_path.c_str() << endreq;
      return StatusCode::FAILURE;
    }
    else {
      log << MSG::INFO << "Loading module map from: " << m_modulelut_path << endreq;
      m_rmap->loadModuleIDLUT(m_modulelut_path.c_str());
    }
  }

  if (m_pmap_unused) {
    log << MSG::INFO << "Creating region map for the unused layers" << endreq;
    m_rmap_unused = new FTKRegionMap(m_pmap_unused, m_rmap_path.c_str());
  }


  log << MSG::INFO << "Read SS configurations" << endreq;
  log << MSG::INFO << "Read AM SS configuration" << endreq;
  m_ssmap = new FTKSSMap(m_rmap, m_ssmap_path.c_str(), false);
  if (!m_ssmapunused_path.empty()) {
    log << MSG::INFO << "Read SS configuration for layer not used in AM" << endreq;
    m_ssmap_unused = new FTKSSMap(m_rmap_unused, m_ssmapunused_path.c_str(), false);
  }
  if (!m_ssmaptsp_path.empty()) {
    log << MSG::INFO << "Read SS configuration for TSP/DC" << endreq;
    m_ssmap_tsp = new FTKSSMap(m_rmap, m_ssmaptsp_path.c_str(), false);
  }
  

  // select how the input is obtained
  if (!m_InputFromWrapper) {
    // Use the SG to retrieve the hits, this also means other Athena tools can be used
    StatusCode schit = m_hitInputTool.retrieve();
    if (schit.isFailure()) {
      log << MSG::FATAL << "Could not retrieve FTK_SGHitInput tool" << endreq;
      return StatusCode::FAILURE;
    }
    else {
      log << MSG::INFO << "Setting FTK_SGHitInput tool" << endreq;
      // set the pmap address to FTKDataInput to use in processEvent
      m_hitInputTool->reference()->setPlaneMaps(m_pmap,m_pmap_unused);
    }
    m_rfobj.setDataInputModule(m_hitInputTool->reference());
  }
  else if (m_RegionalWrapper) {
    // the input comes from a wrapper file's format, prepare the standlone like input
    log << MSG::INFO << "Setting FTK_RegionalRawInput as input module" << endreq;
    FTK_RegionalRawInput *ftkrawinput = new FTK_RegionalRawInput(m_pmap,m_pmap_unused);
    if (m_pmap_unused) {
      // enable all the flags used for the 2nd stage fitting
      ftkrawinput->setSaveUnused(true);
      m_rfobj.setSSSearchUnused(true);
    }

    // add the input files
    log << MSG::INFO << "Loading " << m_wrapperpaths.size() << " files" << endreq;
    for (unsigned ifile=0;ifile!=m_wrapperpaths.size();++ifile) {
      log << MSG::INFO << "Loading " << m_wrapperpaths[ifile] << endreq;
      ftkrawinput->addFile(m_wrapperpaths[ifile].c_str());
    }
    m_rfobj.setDataInputModule(ftkrawinput);
  }
  else {
    // the input comes from a wrapper file's format, prepare the standlone like input
    log << MSG::INFO << "Setting FTK_RawInput as input module" << endreq;
    FTK_RawInput *ftkrawinput = new FTK_RawInput(m_pmap,m_pmap_unused);
    if (m_pmap_unused) {
      // enable all the flags used for the 2nd stage fitting
      ftkrawinput->setSaveUnused(true);
      m_rfobj.setSSSearchUnused(true);
    }

    // add the input files
    log << MSG::INFO << "Loading " << m_wrapperpaths.size() << " files" << endreq;
    for (unsigned ifile=0;ifile!=m_wrapperpaths.size();++ifile) {
      log << MSG::INFO << "Loading " << m_wrapperpaths[ifile] << endreq;
      ftkrawinput->addFile(m_wrapperpaths[ifile].c_str());
    }
    m_rfobj.setDataInputModule(ftkrawinput);
  }

  // Set options related to the input: clustering or other features
  m_rfobj.getDataInputModule()->setPixelClusteringMode(m_PixelClusteringMode);
  m_rfobj.getDataInputModule()->setSctClustering(m_SctClustering);
    
  // Set Option (temporaly way for checking standalone ver. -> should change to better way) //
  ftkset.setBarrelOnly(m_BarrelOnly);
  ftkset.setRawMode(!m_pmap_unused ? 1 : 2);
  ftkset.setEnableFTKSim(m_EnableFTKSim);
  ftkset.setMaxMissingPlanes(m_MaxMissingPlanes);

  // Transition region fix. Works allowing extra missing modules
  ftkset.setMaxMissingSctPairs(m_MaxMissingSCTPairs);
  if (m_RestrictSctPairModule) {
      // Asking missing extra module implies missing extra layer
      ftkset.setRestrictSctPairModule(1);
      ftkset.setRestrictSctPairLayer(1);
  }
  else {
      // possible to have not synched flags
      ftkset.setRestrictSctPairModule(0);
      ftkset.setRestrictSctPairLayer(m_RestrictSctPairLayer ? 1 : 0);
  }

  ftkset.setRoadWarrior(m_RoadWarrior);
  m_pmap->setEnablePlanePair(0);

  ftkset.setKeepRemoved(m_KeepRemoved);
  ftkset.setVerbosity(m_verbosity);
  
  if (m_roadmarket ) { // enable the tool to exchange the roads with the FTKTrackFitterAlgo
    // ensure other colliding options are not true
    if (m_doroadfile) {
      log << MSG::WARNING << "Use of FTK_RoadMarketTool disable the road file" << endreq;
      m_doroadfile = false;
    }

    StatusCode scmrk = m_roadMarketTool.retrieve();
    if (scmrk.isFailure()) {
      log << MSG::FATAL << "Could not retrieve FTK_RoadMarketTool" << endreq;
      return StatusCode::FAILURE;
    }
    else {
      log << MSG::INFO << "Setting FTK_RoadMarketTool" << endreq;
    }

    log << MSG::INFO << "Roads are not saved but only kept in memory and moved to the track fitter" << endreq;
    FTKRoadOutput *ftkoutmodule = m_roadMarketTool->outputReference();
    //m_roadMarketTool->SaveRoads(m_saveroads);
    m_rfobj.setRoadOutputModule(ftkoutmodule); 
    m_roadMarketTool->setRoadFinderReference(&m_rfobj);
  }
  else if (!m_doroadfile && m_saveroads) { // enable the Athena tool for the road output
    StatusCode scout = m_roadOutputTool.retrieve();
    if (scout.isFailure()) {
      log << MSG::FATAL << "Could not retrieve FTK_SGRoadOutput tool" << endreq;
      return StatusCode::FAILURE;
    }
    else {
      log << MSG::INFO << "Setting FTK_SGRoadOutput tool" << endreq;
    }

    log << MSG::INFO << "Write the AM simulation results into the SG" << endreq;
    // prepare an object that interact with the SG to save the found roads
    FTKRoadOutput *ftkoutmodule = m_roadOutputTool->reference();
    m_rfobj.setRoadOutputModule(ftkoutmodule); 
  }
  // create the road output module, module have to be created before the setNBanks call
  else if (m_doroadfile) {
    // Make an external output file, compatible with the
    // standalone version, to store the found roads
    FTKRoadFileOutput *ftkoutmodule = new FTKRoadFileOutput();
    if (m_InputFromWrapper) {
      log << MSG::INFO << "Write the AM simulation results in an external directory: " << m_roadfilesdir << endreq;
      // if the input format is composed by wrapper files the multi-out option is on
      ftkoutmodule->setMultiOut(true);
      ftkoutmodule->setOutDir(m_roadfilesdir.c_str());  // test name -> should be changed for any root file name we want  
    }
    else {
      log << MSG::INFO << "Write the AM simulation results in an external file: " << m_roadfilepath << endreq;
      ftkoutmodule->setMultiOut(false);
      ftkoutmodule->setFileName(m_roadfilepath.c_str());  // test name -> should be changed for any root file name we want  
    }
    m_rfobj.setRoadOutputModule(ftkoutmodule); 
  }
  else {
    log << MSG::FATAL << "At least an output method has to be specified" << endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::INFO << "Load banks" << endreq;

  // Set the bank segmentation
  m_rfobj.setNBanks(m_nbanks);
  m_rfobj.setNsubregions(m_nsubregions);

  // get the bank files and information from the JO
  unsigned int nbanks = m_patternbankpath.size();
  log << MSG::INFO << "Loading " << nbanks << " banks"  << endreq;      
  if (nbanks!=m_bankregion.size() || nbanks!=m_bankpatterns.size() || nbanks!=m_banksubreg.size()) {
    log << MSG::FATAL << "Different array property length in bank definitions: " << nbanks << ' ' << m_bankregion.size() << ' ' << m_bankpatterns.size() << endreq;
    return StatusCode::FAILURE;
  }

  for (unsigned int ib=0;ib!=nbanks;++ib) { // configuration banks loop
    FTK_AMsimulation_base *curbank;
    int regid(m_bankregion[ib]);
    int subid(m_banksubreg[ib]);

    if (m_useTSPBank) { 
      // configura a TSP ready bank
      FTKTSPBank *tspbank = new FTKTSPBank(regid,subid);
      tspbank->setSimulateTSP(m_doTSPSim);
      tspbank->setSSMapTSP(m_ssmap_tsp);
      tspbank->setTSPMinCoverage(m_minTSPCoverage);
      tspbank->setMakeCache(m_doMakeCache);
      tspbank->setCachePath(m_CachePath.c_str());
      tspbank->setAMSize(m_setAMSize);
      tspbank->setDCMatchMethod(m_DCMatchMethod);
      curbank = tspbank;
    }  else if(m_useCompressedBank) {
       FTK_CompressedAMBank *compressedBank=
          new FTK_CompressedAMBank(regid,subid);
       compressedBank->setSSMapTSP(m_ssmap_tsp);
       curbank=compressedBank;
    } else {
      // configure a base AM bank
      curbank = new FTK_AMBank(regid,subid);
    }


    /* if this is set in a TSP simulation also the roads rejected 
       by the DC mechanism or by the TSP are recorded. In this case
       the number of hits is correctly updated to the number of verified
       layers */
    curbank->setSaveAllRoads(m_SaveAllRoads);
    curbank->setRequireFirst(m_require_first);
    curbank->setUseMinimalAMIN(m_useMinimalAMIN);

    // additional adjustments for SCTtrk mode
    if (ftkset.getSCTtrkMode()) {
      // always require SCTtrk layer in AM matching
      curbank->setRequireLast(true);
    }

    m_ssmap->setOffsetFraction(m_ss_offset_fraction);
    curbank->setSSMap(m_ssmap);
    curbank->setSSMapUnused(m_ssmap_unused);

    // set the bad module map path
    curbank->setBadSSMapPath(m_badmap_path);
    curbank->setBadSSMapPath2(m_badmap_path2);

    // set the reference to the current bank
    const string &bankpath = m_patternbankpath[ib];
   
    string bankext("");

    // extract the extension removing eventual trailing number, often added by grid, e.g. file.root.3
    unsigned exlen(0); // start from 0
    for (unsigned ipos=bankpath.size();ipos!=0;--ipos) { // reverse loop over the bank name
      if (bankpath[ipos-1]=='.') {
	if (exlen>1) {
	  bankext =  bankpath.substr(ipos,exlen);

	  log << "Extension: " << bankext << flush;

	  // check if this is compatible with a number
	  bool isValidExt(false);

	  for (unsigned jpos=0;jpos!=exlen;++jpos) {
	    if (! isdigit(bankext[jpos])) {
	      isValidExt = true;
	      break;
	    }
	  }

	  // true if at least 1 character is not a digit
	  if (isValidExt) {
	    break;
	  }
	  else {
	    log << " INVALID" << endreq;
	  }
	}

	bankext = ""; // reset extension
	exlen = 0; // the fragment doesn't represent a valid extension, move further
	ipos -= 1; // skip one step
      }
      
      // increment the size of the sub-string
      exlen += 1;
    } // end reverse loop over the bank name

    if (bankext == string("db")) {
      if (curbank->readDBBank(bankpath.c_str(), m_bankpatterns[ib], m_BankLevel) < 0) {
	delete curbank;
	return StatusCode::FAILURE;
      }
    } else if (bankext == string("root") && !m_CachedBank) {
      // use the ROOT routine      
      if (curbank->readROOTBank(bankpath.c_str(), m_bankpatterns[ib]) < 0) {
	// error reading the file
	delete curbank;
	return StatusCode::FAILURE;
      }
    } else if (bankext == string("root") && m_CachedBank) {
      // use the ROOT routine      
      if (curbank->readROOTBankCache(bankpath.c_str()) < 0) {
	// error reading the file
	delete curbank;
	return StatusCode::FAILURE;
      }
    } else {
      // if the previous check failed try with ASCII file
      if (curbank->readASCIIBank(bankpath.c_str(), m_bankpatterns[ib]) < 0) {
	// error reading the file
	delete curbank;
	return StatusCode::FAILURE;
      }
    }

    // Set the bank this region
    m_rfobj.setAMBank(regid, curbank);
  } // end configuration banks loop
  
  log << MSG::INFO << "Initialize the RoadFinder parameters" << endreq;
  m_rfobj.init(); // initialize roadfinder object

  //if (sc.isFailure()) {
  //  log << MSG::FATAL << "Unexpected problem during the initialization stage" << endreq;
  //  return StatusCode::FAILURE;
  //}

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKRoadFinderAlgo::execute() {
  
  MsgStream log(msgSvc(), name());
  FTKSetup::getFTKSetup().setMsgStream(&log);
  FTKLogger::SetLogger(this);

  log << MSG::INFO << "FTKRoadFinderAlgo::execute() start" << endreq;
  
  if (m_AutoDisable) {
    // if AutoDisable feature is found all the execution driven by road-finder
    log << MSG::INFO << "FTKRoadFinderAlgo auto-disable mode, nothing to do" << endreq;
  }
  else {
    int res = m_rfobj.nextEvent();   
    if (res<0)  {
      log << MSG::WARNING << "No more events to process" << endreq;
      // when all the event are evaluated the auto-disable is enabled
      m_AutoDisable = true;
    }
  }
  
  log << MSG::INFO << "FTKRoadFinderAlgo::execute() end" << endreq;
  return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKRoadFinderAlgo::finalize() {
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "finalize()" << endreq;

    for (unsigned int ib=0;ib!=m_patternbankpath.size();++ib) {
        int regid(m_bankregion[ib]);
        float meanroads = m_rfobj.getAMBank(regid)->getTotRoads()/m_rfobj.getAMBank(regid)->getTotEvents();
        log << MSG::INFO << "Average number of roads bank (" << regid << "," << m_banksubreg[ib] << ") = " << meanroads << endreq;
    }
    // ensure the output is closed
    m_rfobj.getRoadOutputModule()->endFile();

    // detach data input and output modules from RoadFinder to avoid its deletion
    return StatusCode::SUCCESS;
}

void FTKRoadFinderAlgo::PostMessage() {
   if(fType==0)  ATH_MSG_FATAL(fBuffer->str());
   else if(fType==1)  ATH_MSG_ERROR(fBuffer->str());
   else if(fType==2)  ATH_MSG_WARNING(fBuffer->str());
   else if(fType==3)  ATH_MSG_INFO(fBuffer->str());
   else if(fType==4)  ATH_MSG_DEBUG(fBuffer->str());
}

