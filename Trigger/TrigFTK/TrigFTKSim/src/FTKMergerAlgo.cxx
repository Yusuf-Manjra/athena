/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKTrack.h"

#include "TrigFTKToolInterfaces/ITrigFTKTrackConverter.h"
#include "TrigFTKToolInterfaces/ITrigFTKUncertiantyTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "IdDictDetDescr/IdDictManager.h"

#include "TrigFTKSim/FTKMergerAlgo.h"
#include "TrigFTKPool/FTKAthTrack.h"
#include "TrigFTKPool/FTKAthTrackContainer.h"

#include "GaudiKernel/MsgStream.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "PileUpTools/PileUpMergeSvc.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

#include "NavFourMom/INavigable4MomentumCollection.h"

#include "TBranch.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"

#include <list>
#include <iostream>
#include <sstream>
#include <fstream>

#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))

using namespace std;

/////////////////////////////////////////////////////////////////////////////
FTKMergerAlgo::FTKMergerAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_pmap_path(""), m_pmap(0x0),
  m_useStandalone(true),
  m_singleProces(false),
  m_doMerging(false),
  m_MergeRoads(false),
  m_nregions(64), m_nsubregions(4),
  m_neventsUnmergedFiles(0),
  m_neventsMerged(0),
  m_outputTree(0),
  m_outputFile(0),
  m_merged_bank(0),
  m_HW_level(2),
  m_HW_ndiff(6),
  m_NCoords(0),
  m_HW_path(""),
  m_keep_rejected(0),
  m_SCTCoordOffset(0),
  m_ftktrack_tomerge_paths(),
  m_ftktrack_tomerge_firstregion(0),
  m_ftktrack_tomerge_firstsubreg(0),
  m_ftrtrack_tomerge_bfname("FTKTracksStream%u."),
  m_MergeRegion(-1),
  m_ftktrack_mergeoutput("ftkmerged.root"),
  m_ftktrack_mergeInputPath(""),
  m_ftktrack_mergeFileRoot(""),
  m_forceAllInput(true),
  m_doGrid(false),
  m_ftktrack_paths_merged(),
  m_mergedtracks_chain(0x0),
  m_mergedtracks_stream(0x0),
  m_mergedtracks_bname("FTKBankMerged"), /* Old standalone name is FTKBankMerged, prdosys FTKMergedTracksStream */
  m_force_merge(false),
  m_out_indettrack_Name("FTK_LVL2_Tracks"), // of the collection to store RAW TrigInDetTracks
  m_out_indettrack(0x0),
  m_FTKPxlClu_CollName("FTK_Pixel_Clusters"), // default name for the FTK pixel clusters collection
  m_FTKPxlCluContainer(0x0), 
  m_FTKSCTClu_CollName("FTK_SCT_Cluster"), // default name for the FTK SCT cluster collection
  m_FTKSCTCluContainer(0x0),
  m_out_trktrack_Name("FTK_Trk_Tracks"), // name of the collection used to store RAW Trk::Tracks
  m_out_trktrack(0x0),
  m_GenerateRDO(false),
  m_ftk_raw_trackcollection_Name("FTK_RDO_Tracks"),
  m_ftk_raw_trackcollection(0x0), // JWH Added to test FTK RDO
  m_out_ftktrackconv_Name("FTK_Trk_Tracks_Refit"), // name of the collection used to store converted Trk::Tracks
  m_out_ftktrackconv(0x0), // collection used to store converted TrkTrack
  m_out_ftktrackparticleconv_Name("FTK_TrackParticles_Refit"), // name of the collection used to store converted TrackParticle
  m_out_ftktrackparticleconv(0x0), // collection used to store converted TrackParticle
  m_out_indettrackconv_Name("FTK_LVL2_Tracks_Refit"), // name of the collection to store converted TrigInDetTracks
  m_out_indettrackconv(0x0), // output collection for converted TrigInDetTracks
  m_trackConverterTool("TrigFTKTrackConverter"),
  m_uncertiantyTool("TrigFTKUncertiantyTool"),
  m_trackSumTool("Trk::TrackSummaryTool/InDetTrackSummaryTool"),

  //  m_particleCreatorTool("Trk::ParticleCreatorTool"),
  //  m_particleCreatorTool(0x0),
  m_out_trackPC_Name("FTK_TrackParticles"),
  m_out_trackPC(0x0),
  m_vxCandidatesPrimaryName("VxPrimaryCandidate"),
  m_truthFileNames(),
  m_truthTrackTreeName(""),
  m_evtinfoTreeName(""),
  m_saveTruthTree(1)
{
  declareProperty("useStandalone",m_useStandalone,"Use tracks produced from the standalone version");
  declareProperty("SingleProces",m_singleProces,"Assume in the same process with the TF");
  declareProperty("doMerging",m_doMerging,"Enable the merging");
  declareProperty("NBanks",m_nregions,"Number of regions");
  declareProperty("NSubRegions",m_nsubregions,"Number of sub-regions");
  declareProperty("mergedTrackPaths",m_ftktrack_paths_merged,
		  "Paths of the merged tracks");
  declareProperty("mergeTrackBName",m_mergedtracks_bname,"Branch name for the merged tracks");
  declareProperty("FTKConvTrackParticles",m_out_ftktrackconv_Name,"Name for the converted TrackParticle");
  //  declareProperty("ParticleCreatorTool", m_particleCreatorTool);
  declareProperty("pmap_path",m_pmap_path,
		  "FTK layer configuration");
  declareProperty("force_merge",m_force_merge,"Force the merging disabling any check");
 
  declareProperty("HitWarrior",m_HW_level);
  declareProperty("HWNDiff",m_HW_ndiff);
  declareProperty("loadHWConf_path",m_HW_path); 
  declareProperty("KeepRejected",m_keep_rejected); 

  declareProperty("FTKToMergePaths",m_ftktrack_tomerge_paths);
  declareProperty("FirstRegion",m_ftktrack_tomerge_firstregion);
  declareProperty("FirstSubreg",m_ftktrack_tomerge_firstsubreg);
  declareProperty("FTKMergedOutput",m_ftktrack_mergeoutput);
  declareProperty("FTKUnmergedInputPath",m_ftktrack_mergeInputPath);
  declareProperty("FTKUnmergedFileRoot",m_ftktrack_mergeFileRoot);
  declareProperty("FTKForceAllInput",m_forceAllInput,"Require all inputFiles to be present");
  declareProperty("FTKDoGrid",m_doGrid,"Assume Naming used on grid");
  declareProperty("MergeRegion",m_MergeRegion,"If >=0 enables the merging for a single region, -1 means global merging");
  declareProperty("FTKUnmergedFormatName",m_ftrtrack_tomerge_bfname,"Format for the branch-name of the unmerged tracks");
  declareProperty("FTKPixelClustersCollName",m_FTKPxlClu_CollName,"FTK pixel clusters collection");
  declareProperty("FTKSCTClusterCollName",m_FTKSCTClu_CollName,"FTK SCT clusters collection");
  declareProperty("TrackConverter",m_trackConverterTool);
  declareProperty("UncertiantyTool",m_uncertiantyTool);
  declareProperty("TrackSummaryTool",m_trackSumTool);

  declareProperty("FTKLVL2Container",m_out_indettrack_Name,
      "Container name for the FTK tracks used by LVL2");
  declareProperty("FTKTrackParticleContainer",m_out_trackPC_Name,
                  "Track Particle Container name for the FTK tracks ");
  declareProperty("FTKTrkTrack",m_out_trktrack_Name,"Name for the FTK Trk::Track container");
  declareProperty("FTKTrackParticleContainerRefit",m_out_ftktrackparticleconv_Name);
  declareProperty("FTKLVL2ContainerRefit",m_out_indettrackconv_Name);
  declareProperty("FTKTrkTrackContainerRefit",m_out_ftktrackconv_Name);

  declareProperty("GenerateRDO",m_GenerateRDO);
  declareProperty("TruthFileNames",m_truthFileNames);
  declareProperty("TruthTrackTreeName",m_truthTrackTreeName);
  declareProperty("EvtInfoTreeName",m_evtinfoTreeName);
  declareProperty("SaveTruthTree",m_saveTruthTree);

  declareProperty("MergeRoads",m_MergeRoads,"if True the roads will be merged, if no roads are found an error will be raised");
}

FTKMergerAlgo::~FTKMergerAlgo()
{
  // destroy configuration
  if (m_mergedtracks_stream) delete m_mergedtracks_stream;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKMergerAlgo::initialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "FTKMergerAlgo::initialize()" << endreq;
  
  /*TODO: this part is required only if the FTKMerger algo is reading from SG (in conversion mode)
   * or writing in the SG (not currently supported). This check can be relaxed
   */
  StatusCode scSG = service( "StoreGateSvc", m_StoreGate );
  if (scSG.isFailure()) {
    log << MSG::FATAL << "Unable to retrieve StoreGate service" << endreq;
    return scSG;
  }


  if (m_useStandalone && !m_doMerging) {
    // prepare the input from the FTK tracks, merged in an external simulation
    m_mergedtracks_chain = new TChain("ftkdata","Merged tracks chain");
    // add the file to the chain
    log << MSG::INFO << "Loading " << m_ftktrack_paths_merged.size() << " files with FTK merged tracks" << endreq;
    vector<string>::const_iterator imtp = m_ftktrack_paths_merged.begin();
    for (;imtp!=m_ftktrack_paths_merged.end();++imtp) {
      Int_t addres = m_mergedtracks_chain->Add((*imtp).c_str());
      log << MSG::DEBUG << "Added: " << *imtp << '[' << addres << ']' <<endreq;
    }
    m_mergedtracks_stream = new FTKTrackStream();
    TBranch *mergedtracks_branch;
    Int_t res = m_mergedtracks_chain->SetBranchAddress(m_mergedtracks_bname.c_str(),&m_mergedtracks_stream,&mergedtracks_branch);
    if (res<0) {
      log << MSG::FATAL << "Branch \"" << m_mergedtracks_bname << "\" with merged tracks not found" << endreq;
      return StatusCode::FAILURE;
    }


    // reset the counters
    m_mergedtracks_iev = 0;

    // cleanup the map between event number and vector of FTK tracks
    m_trackVectorMap.clear();

    Long64_t nEntries = m_mergedtracks_chain->GetEntries();

    int prevEventNumber(-1); // variable to check repetition of events, special warning condition
    bool hasRepeatedEvents(false); // flag to verify if the repetition of the event condition has been found

    for(Long64_t iEntry=0; iEntry<nEntries; iEntry++) {

      m_mergedtracks_chain->GetEntry(iEntry);
      int eventNumber = m_mergedtracks_stream->eventNumber();

      std::map<int, Long64_t >::iterator mapIt = m_trackVectorMap.find(eventNumber);

      if(mapIt==m_trackVectorMap.end()) {//new event
        if (!hasRepeatedEvents) {
          m_trackVectorMap.insert(std::pair<int, Long64_t >(eventNumber, iEntry ));
          prevEventNumber = eventNumber;
        }
        else {
          log << MSG::FATAL << "A duplicated event was found before the end of file, the error cannot be recoverd" << endreq;
          return StatusCode::FAILURE;
        }
      }
      else if (eventNumber==prevEventNumber) {
        log << MSG::WARNING << "Event " << eventNumber << " found in the previous event, failure condition masked if this happens at the end of the file" << endreq;
        hasRepeatedEvents = true;
      }
      else {
        // Duplicate events are a condition error at the moment, this
        // can be fixed using a runNumber and eventNumber paier as key
        log << MSG::FATAL << "Tracks for the current event (" << eventNumber << ") already exist. Duplication not allowed" << endreq;
        return StatusCode::FAILURE;
      }

    }

    log << MSG::DEBUG << "Tracks from " << m_trackVectorMap.size() << " events loaded"<<endreq;

    /* This wroking mode interacts with the Detector objects, for
     * this reason generic identifiers are retrieved */
    StatusCode sc = service("DetectorStore", m_detStore);
    if ( sc.isFailure() ) { 
      log << MSG::FATAL << "DetStore service not found" << endreq; 
      return StatusCode::FAILURE;
    }

    // Get the Track Converter Tool
    if (m_trackConverterTool.retrieve().isFailure() ) {
      log << MSG::ERROR << "Failed to retrieve tool " << m_trackConverterTool << endreq;
      return StatusCode::FAILURE;
    }

    // Get the Uncertianty Tool
    if (m_uncertiantyTool.retrieve().isFailure() ) {
      log << MSG::ERROR << "Failed to retrieve tool " << m_uncertiantyTool << endreq;
      return StatusCode::FAILURE;
    }

    // Get the Track Sum Tool
    if (m_trackSumTool.retrieve().isFailure() ) {
      log << MSG::ERROR << "Failed to retrieve tool " << m_trackSumTool << endreq;
      return StatusCode::FAILURE;
    }


    // ID helpers
    m_idHelper = new AtlasDetectorID;
    const IdDictManager* idDictMgr( 0 );
    if(m_detStore->retrieve(idDictMgr, "IdDict").isFailure() || !idDictMgr ) {
      log << MSG::ERROR << "Could not get IdDictManager !" << endreq;
      return StatusCode::FAILURE;
    }
    if( m_detStore->retrieve(m_PIX_mgr, "Pixel").isFailure() ) {
      log << MSG::ERROR << "Unable to retrieve Pixel manager from DetectorStore" << endreq;
      return StatusCode::FAILURE;
    }
    if (m_detStore->retrieve(m_pixel_id, "PixelID").isFailure()) {
      log << MSG::FATAL << "Could not get Pixel ID helper" << endreq;
      return StatusCode::FAILURE;
    }
    if( m_detStore->retrieve(m_SCT_mgr, "SCT").isFailure() ) {
      log << MSG::ERROR << "Unable to retrieve SCT manager from DetectorStore" << endreq;
      return StatusCode::FAILURE;
    }
    if (m_detStore->retrieve(m_sct_id, "SCT_ID").isFailure()) {
      log << MSG::FATAL << "Could not get SCT ID helper" << endreq;
      return StatusCode::FAILURE;
    }

    /* Creating the SG entry points fro the FTK clusters */

    // Creating collection for pixel clusters
    m_FTKPxlCluContainer = new InDet::PixelClusterContainer(m_pixel_id->wafer_hash_max());
    m_FTKPxlCluContainer->addRef();
    sc = m_StoreGate->record(m_FTKPxlCluContainer,m_FTKPxlClu_CollName);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Error registering the FTK pixel container in the SG" << endreq;
      return StatusCode::FAILURE;
    }
    // Generic format link for the pixel clusters
    const InDet::SiClusterContainer *symSiContainerPxl(0x0);
    sc = m_StoreGate->symLink(m_FTKPxlCluContainer,symSiContainerPxl);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Error creating the sym-link to the Pixel clusters" << endreq;
      return StatusCode::FAILURE;
    }

    // Creating collection for the SCT clusters
    m_FTKSCTCluContainer = new InDet::SCT_ClusterContainer(m_sct_id->wafer_hash_max());
    m_FTKSCTCluContainer->addRef();
    sc = m_StoreGate->record(m_FTKSCTCluContainer,m_FTKSCTClu_CollName);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Error registering the FTK SCT container in the SG" << endreq;
      return StatusCode::FAILURE;
    }
    // Generic format link for the pixel clusters
    const InDet::SiClusterContainer *symSiContainerSCT(0x0);
    sc = m_StoreGate->symLink(m_FTKSCTCluContainer,symSiContainerSCT);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Error creating the sym-link to the SCT clusters" << endreq;
      return StatusCode::FAILURE;
    }

  } // preparation for standalone track conversion block done
  else if (m_useStandalone && m_doMerging) {
    StatusCode sc = initStandaloneTracks();
    if (sc.isFailure()) {
      log << MSG::FATAL << "Unable initialize merged tracks" << endreq;
      return sc;
    }
  }
  else if (!m_singleProces) {
    /* merge the input from several POOL files, the PileUpMergeSvc is used to 
       make this task */
    if (!(service("PileUpMergeSvc", m_mergeSvc, true)).isSuccess() || 
	0 == m_mergeSvc) {
      log << MSG::ERROR << "Could not find PileUpMergeSvc" << endreq;
      return StatusCode::FAILURE;
    }
  }
  else {
    // The merger will assume the SG contains the results from NRegions and 1 sub-region
    if (m_nsubregions!=1) {
      log << MSG::ERROR << "The current code supports only 1 sub-region" << endreq;
      return StatusCode::FAILURE;
    }
  }

  // set the plane-map for the track merger
  if (m_pmap_path.empty()) {
    log << MSG::FATAL << "No FTK plane-map file is set" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    m_pmap = new FTKPlaneMap(m_pmap_path.c_str());
    if (!(*m_pmap)) {
      log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endreq;
      return StatusCode::FAILURE;
    }

    // extract some general information from the PMAP file

    // get the total total dimension = total number of independent coordinates for a track
    m_NCoords = m_pmap->getTotalDim();

    // get the total number of pixel layers (assumed to be the first layers)
    for (int ip=0;ip!=m_pmap->getNPlanes();++ip) {
        if (!m_pmap->isPixel(ip)) {
            m_SCTCoordOffset = ip*2;
            break;
        }
    }
  }
  
  // Set Hit Warrior configuration when using mergeSGContent//
  if (m_doMerging){
    if (m_HW_path.empty()) {
      log << MSG::FATAL << "HWSDEV_FILE Nothing" << endreq;
      return StatusCode::FAILURE;
    }
    else {
      m_HW_dev = new float[m_NCoords];
      for (int i=0; i!=m_NCoords ;++i) {
	m_HW_dev[i] = 0.f;
      }
      
      ifstream fin(m_HW_path.c_str());
      if (!fin) {   
	log << MSG::FATAL << "HWSDEV_FILE Nothing" << endreq;
	return StatusCode::FAILURE;
      }
      
      log << MSG::INFO << "Load Hit Warrior Configuration" << endreq;
      
      string line;
      // first line represents version, not used now
      getline(fin,line);
      // loop until an N in the first position is found,
      // this should change in the future, as in other map files
      while(getline(fin,line)) {
	if (line[0]=='N') break;
      }
      // start the real read of the file
      char flag;
      unsigned int nvals;
      int pos;
      float fval;
      fin >> flag >> nvals;
      for (unsigned int i=0;i<nvals;++i) {
	fin >> pos >> fval;
	m_HW_dev[pos] = fval;
      }
    }
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKMergerAlgo::execute() {
   MsgStream log(msgSvc(), name());
   log << MSG::INFO << "FTKMergerAlgo::execute() start" << endreq;

   if (m_useStandalone && !m_doMerging) {
     StatusCode sc = convertMergedTracks();
     if (sc.isFailure()) {
       log << MSG::FATAL << "Unable convert merged tracks" << endreq;
       return sc;
     }
   }
   else if (m_useStandalone && m_doMerging) {
     StatusCode sc = mergeStandaloneTracks();
     if (sc.isFailure()) {
       log << MSG::FATAL << "Unable convert merged tracks" << endreq;
       return sc;
     }
   }
   else if (m_singleProces && m_doMerging) {
     StatusCode sc = mergeSGContent();
     if (sc.isFailure()) {
       log << MSG::FATAL << "Unable merge the SG content" << endreq;
       return sc;
     }
   }
   else if (!m_useStandalone && m_doMerging) {
     StatusCode sc = mergePoolFiles();
     if (sc.isFailure()) {
       log << MSG::FATAL << "Unable merge the SG content from multiple POOL files" << endreq;
       return sc;
     }
   }
   else {
     log << MSG::ERROR << "Unmanaged merging/conversion combination" << endreq;
     return StatusCode::FAILURE;
   }

   
   log << MSG::DEBUG << "FTKMergerAlgo::execute() end" << endreq;

   return StatusCode::SUCCESS;
}

//
//  Initialize inputput when merging standalone input
//   this mode reads unmerged tracks from a previous FTKTrackFitterAlgo 
//
StatusCode FTKMergerAlgo::initStandaloneTracks()
{
  MsgStream log(msgSvc(), name());

  //
  // reset the internal event counter
  //
  m_mergedtracks_iev = 0;
    
  // prepare the input from the FTK tracks, merged in an external simulation
  m_mergedtracks_tree = new TTree("ftkdata","Merged tracks chain");
  log << MSG::DEBUG << "Will Merge:  " << m_nregions << " region and "  << m_nsubregions << " subregions " <<endreq;
  log << MSG::DEBUG << "Starting with region " << m_ftktrack_tomerge_firstregion << " and subregion " << m_ftktrack_tomerge_firstsubreg << endreq;
	  

  //
  //  The output stream
  //
  m_mergedtracks_stream = new FTKTrackStream;
  m_mergedtracks_tree->Branch("FTKBankMerged",&m_mergedtracks_stream);

  //
  // allocate memory
  //
  m_ftktrack_tomerge_file   = new TFile**[m_nregions];
  m_ftktrack_tomerge_tree   = new TTree**[m_nregions];
  m_ftktrack_tomerge_branch = new TBranch**[m_nregions];
  m_ftktrack_tomerge_stream = new FTKTrackStream**[m_nregions]; 
  for (unsigned int ir=0;ir!=m_nregions;++ir) { // region loop
    m_ftktrack_tomerge_file[ir]   = new TFile*[m_nsubregions];
    m_ftktrack_tomerge_tree[ir]   = new TTree*[m_nsubregions];
    m_ftktrack_tomerge_branch[ir] = new TBranch*[m_nsubregions];
    m_ftktrack_tomerge_stream[ir] = new FTKTrackStream*[m_nsubregions];
  }

  //
  // Assume that the files are ordered, so all we need is:
  //   - the path
  //   - the first region
  //   - the number of regions
  //   - the number of subregions
  //
  if (!m_ftktrack_tomerge_paths.empty()) {
    // set the input files using a list of input files

    // initialize the merging structure   
    for (  unsigned int ireg=0; ireg!=m_nregions;   ++ireg) { // region loop
      for (unsigned int isub=0; isub!=m_nsubregions; ++isub) { // subregion loop

	//
	//  init memory
	//
	m_ftktrack_tomerge_file  [ireg][isub] = 0;
	m_ftktrack_tomerge_tree  [ireg][isub] = 0;
	m_ftktrack_tomerge_branch[ireg][isub] = 0;
	m_ftktrack_tomerge_stream[ireg][isub] = 0;
      } // end subreg loop
    } // end region loop
    // end merge structure initialization

    // calculate the offset of the file number
    const unsigned int ifile_offset = m_ftktrack_tomerge_firstregion*m_nsubregions+m_ftktrack_tomerge_firstsubreg;

    for (unsigned int ifile=0;ifile!=m_ftktrack_tomerge_paths.size();++ifile) { // file loop
      // get the current path
      const string &curfilepath = m_ftktrack_tomerge_paths[ifile];

      // identify the current region and sub-region ID, using the global file ID
      const unsigned int global_ifile = (ifile+ifile_offset);
      const unsigned int regionID(global_ifile/m_nsubregions);
      const unsigned int subregID(global_ifile%m_nsubregions);

      // associate the current file to the current (region,sub-region
      TFile *curfile  = TFile::Open(curfilepath.c_str());
      if (!curfile){
	log << MSG::FATAL << " Error opening the file: " << curfilepath << endreq;
	return StatusCode::FAILURE;
      }

      m_ftktrack_tomerge_file[regionID][subregID] = curfile;
      m_ftktrack_tomerge_tree[regionID][subregID] = (TTree*) curfile->Get("ftkdata");
      if (!m_ftktrack_tomerge_tree[regionID][subregID]) {
	log << MSG::FATAL << "TTree \"ftkdata\" not found in the file: " << curfilepath << endreq;
	return StatusCode::FAILURE;
      }

      m_ftktrack_tomerge_stream[regionID][subregID] = new FTKTrackStream();
      //m_ftktrack_tomerge_tree[regionID][subregID]->Print();
      Int_t res = m_ftktrack_tomerge_tree[regionID][subregID]->SetBranchAddress(Form(m_ftrtrack_tomerge_bfname.c_str(),regionID),&m_ftktrack_tomerge_stream[regionID][subregID],&m_ftktrack_tomerge_branch[regionID][subregID]);
      if (res<0) {
	// negative reeturn values are errors, no specific action taken, just exit
	log << MSG::FATAL << "Error connecting branch: " << Form(m_ftrtrack_tomerge_bfname.c_str(),regionID) << endreq;
	return StatusCode::FAILURE;
      }

      //
      // Set Number of events and the file name (from the first TTree)
      //
      if (m_neventsUnmergedFiles==0) {
	m_neventsUnmergedFiles = m_ftktrack_tomerge_tree[regionID][subregID]->GetEntries();
	log << MSG::DEBUG << "Setting nevents to " << m_neventsUnmergedFiles << " (with regionNum:" << regionID << " subregionNum: " << subregID << " ) " << endreq;
      }
      
    } // end file loop
  } // end merge using a list of input files
  else { 
    // Merge all the regions (correct for offset)
    //
    for (  unsigned int ireg=0; ireg!=m_nregions;   ++ireg) { // region loop

      //
      //  Get the region number including the offset
      //
      unsigned int regNum = ireg + m_ftktrack_tomerge_firstregion;

      // 
      // Merge all the subregions
      //
      for (unsigned int isub=0; isub!=m_nsubregions; ++isub) { // subregion loop

	//
	//  Get the subregion number including the offset
	//
	unsigned int subNum = isub + m_ftktrack_tomerge_firstsubreg;

	//
	//  init memory
	//
	m_ftktrack_tomerge_file  [ireg][isub] = 0;
	m_ftktrack_tomerge_tree  [ireg][isub] = 0;
	m_ftktrack_tomerge_branch[ireg][isub] = 0;
	m_ftktrack_tomerge_stream[ireg][isub] = 0;

	//
	// Path to files being merged
	//
	//m_ftktrack_mergeInputPath = "/afs/cern.ch/work/g/gvolpi/ftksimoutput20120918/";
	std::string thisPath = GetPathName(regNum,subNum);
      
	if(!m_doGrid){
	  //
	  //  Check that the intput file exists
	  //
	  ifstream testStream(thisPath.c_str());
	  if(!testStream.good()){
      
	    // 
	    // If not file doesnt exist try with only one digit
	    //
	    log << MSG::VERBOSE << " Could not read " << thisPath << endreq;
	    thisPath = m_ftktrack_mergeInputPath + "/"+m_ftktrack_mergeFileRoot + Form("_reg%d_sub%d.root",regNum,subNum);
	    log << MSG::VERBOSE << " Trying  " << thisPath << endreq;

	    testStream.close();
	    testStream.open(thisPath.c_str());
	
	    if(!testStream.good()){
	      log << MSG::VERBOSE << thisPath << " also failed" << endreq;
	      log << MSG::WARNING << " Could not read " << GetPathName(regNum,subNum)  
		  << " or " << thisPath << endreq;

	      if(m_forceAllInput){
		log << MSG::FATAL << " Missing inputFile, " << thisPath << " Failing... " << endreq;
		return StatusCode::FAILURE;
	      }else{
		log << MSG::WARNING << " Missing inputFile, " << thisPath << " Skipping... " << endreq;
		continue;
	      }
	    }else{
	      log << MSG::VERBOSE << " InputFile, " << thisPath << " Found " << endreq;
	    }
	  }
	  testStream.close();
	}

	//
	// this will become equal the filename of the first valid region
	//
	string merged_fname("merged_file.root");	

	if (thisPath.empty()) continue;
	log << MSG::INFO << "Opening: " << thisPath << endreq;
	
	//
	//  Get the TFile and the TTree
	//
	m_ftktrack_tomerge_file[ireg][isub] = TFile::Open(thisPath.c_str());
	m_ftktrack_tomerge_tree[ireg][isub] = (TTree*) m_ftktrack_tomerge_file[ireg][isub]->Get("ftkdata");
	if (!m_ftktrack_tomerge_tree[ireg][isub]) {
	  log << MSG::ERROR << "*** Error: TTree in file \"" << thisPath << "\" not found" << endreq;
	  return StatusCode::FAILURE;
	}
	
	//
	//  Create the Stream
	//
	m_ftktrack_tomerge_stream[ireg][isub] = new FTKTrackStream();
	
	// JOHNDA: Not sure which one we want here.
	//   assume output of track fitter for now
	bool input_tree_merged = false;
	if(input_tree_merged){ // if merging output of track_merger
	  m_ftktrack_tomerge_tree[ireg][isub]->Print();
	  m_ftktrack_tomerge_tree[ireg][isub]->SetBranchAddress("FTKBankMerged",&m_ftktrack_tomerge_stream[ireg][isub],&m_ftktrack_tomerge_branch[ireg][isub]);
	}else {                 
	  //
	  // Try to find branches named after teh region
	  //
	  if(m_ftktrack_tomerge_tree[ireg][isub]->FindBranch(Form("FTKTracksStream%d.",regNum))){
	    log << MSG::VERBOSE << "Setting branch with region number: " << regNum << " ireg: " << ireg << " isub: " << isub << endreq;
	    m_ftktrack_tomerge_tree[ireg][isub]->SetBranchAddress(Form("FTKTracksStream%d.",regNum),&m_ftktrack_tomerge_stream[ireg][isub],&m_ftktrack_tomerge_branch[ireg][isub]);

	    //
	    // If not, Try to find branches named after merging has been done
	    //

	  }else{
	    log << MSG::DEBUG << "Setting branch with name: FTKMergedTracksStream" << endreq;
	    m_ftktrack_tomerge_tree[ireg][isub]->SetBranchAddress("FTKMergedTracksStream",&m_ftktrack_tomerge_stream[ireg][isub],&m_ftktrack_tomerge_branch[ireg][isub]);
	  }
	}

	//
	// Set Number of events and the file name (from the first TTree)
	//
	if (m_neventsUnmergedFiles==0) {
	  m_neventsUnmergedFiles = m_ftktrack_tomerge_tree[ireg][isub]->GetEntries();
	  log << MSG::DEBUG << "Setting nevents to " << m_neventsUnmergedFiles << " (with regionNum:" << regNum << " subregionNum: " << subNum << " ) " << endreq;
	  merged_fname = gSystem->BaseName(thisPath.c_str());
	}

      }// subregions
    }// regions
  } // end by position merge

  //
  // Create the merged file
  //
  if (!m_GenerateRDO) {
    string ofname = Form("%s",m_ftktrack_mergeoutput.c_str());
    m_outputFile = TFile::Open(ofname.c_str(),"recreate");
  }
  //
  // create the TTree and the branches
  //
  m_outputTree = new TTree("ftkdata","FTK Simulation Results (merged)");
  m_merged_bank = new FTKTrackStream;
  const int TREE_ROAD_BUFSIZE = 16000;
  if (m_MergeRegion==-1) { 
    // this represent a global merging
    m_outputTree->Branch("FTKMergedTracksStream",&m_merged_bank,TREE_ROAD_BUFSIZE);
  }
  else {
    // in this case only a partial merge, on a single region is assumed
    m_outputTree->Branch(Form("FTKMergedTracksStream%d",m_MergeRegion),&m_merged_bank,TREE_ROAD_BUFSIZE);
  }

  return StatusCode::SUCCESS;
}

//
//  The merging of teh standalone input
//
StatusCode FTKMergerAlgo::mergeStandaloneTracks()
{
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "Merging event " << m_mergedtracks_iev << endreq;
  
  //
  // Check if there are events left to merge
  //
  if(m_neventsMerged > m_neventsUnmergedFiles){
    log << MSG::INFO << "Merged all " << m_neventsUnmergedFiles << " events, nothing left to do." << endreq;
    return StatusCode::SUCCESS;    
  }

  // JWH Testing adding to storegate ///
  if (m_GenerateRDO) {
    m_ftk_raw_trackcollection = new FTK_RawTrackContainer;

    StatusCode scJ = m_StoreGate->record(m_ftk_raw_trackcollection, m_ftk_raw_trackcollection_Name);
    if (scJ.isFailure()) {
      log << MSG::FATAL << "Failure registering FTK_RawTrackContainer" << endreq;
      return StatusCode::FAILURE;
    } else{
      log << MSG::DEBUG << "Setting FTK_RawTrackContainer registered" << endreq;
    }
  }

  //
  //  Set event index
  //
  const unsigned int &iev = m_neventsMerged;

  log << MSG::VERBOSE << "Getting entries " << endreq;
  for (  unsigned int ireg=0; ireg!=m_nregions;   ++ireg) {
    for (unsigned int isub=0; isub!=m_nsubregions;  ++isub) {
      if(!m_ftktrack_tomerge_tree[ireg][isub]){
	log << MSG::VERBOSE << "Tree does not exist. ireg: " << ireg << " isub: " << isub << " iev: " << iev << endreq;
	continue;
      }
      log << MSG::VERBOSE << "ireg: " << ireg << " isub: " << isub << " iev: " << iev << endreq;
      log << MSG::VERBOSE << m_ftktrack_tomerge_branch[ireg][isub] << endreq;
      m_ftktrack_tomerge_branch[ireg][isub]->GetEntry(iev);
    }
  }
  log << MSG::VERBOSE << "Got entries " << endreq;
      
  // clear clones for this bank
  m_merged_bank->clear();
	
  // update roads[ib] summing, after the RW, the original roads
  merge_tracks(m_merged_bank, m_ftktrack_tomerge_stream);
      
  // fill the completed results
  if (m_GenerateRDO) {
      // copy the results in a RDO file
      for (int itrk=0;itrk!=m_merged_bank->getNTracks();++itrk) {
          const FTKTrack *cutrack = m_merged_bank->getTrack(itrk);
          m_ftk_raw_trackcollection->push_back(SimToRaw(*cutrack));
      }
  }
  else {
      m_outputTree->Fill();
  }

  //
  // Increment counters
  //  
  m_neventsMerged += 1;
  m_mergedtracks_iev+=1;

  return StatusCode::SUCCESS;
}

//
//  Cleaning up the standalone input
//
StatusCode FTKMergerAlgo::finalizeStandaloneTracks()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in finalizeStandaloneTracks() " << endreq;

  //
  // write, close, and destroy the output file
  //
  if (!m_GenerateRDO) {
    log << MSG::DEBUG << "writing output " << m_outputFile << " " << m_MergeRegion << " " << m_neventsUnmergedFiles << endreq;
    m_outputFile->Write();
    log << MSG::DEBUG << "closing file " << endreq;
    m_outputFile->Close();
    log << MSG::DEBUG << "deleting output " << endreq;
    delete m_outputFile;
  }

  //
  // close input files
  //
  log << MSG::DEBUG << "Closing input " << endreq;
  set<TFile*> already_deleted;
  for( unsigned int ireg=0; ireg!=m_nregions; ++ireg ) { 
    for( unsigned int isub=0;  isub!=m_nsubregions;  ++isub ) { 

      if( !(m_ftktrack_tomerge_file[ireg][isub]) ) 
	continue; 
	
      if( already_deleted.find( m_ftktrack_tomerge_file[ireg][isub] ) != already_deleted.end() ) {
	m_ftktrack_tomerge_file[ireg][isub] = 0;
	continue; 
      }
	
      already_deleted.insert( m_ftktrack_tomerge_file[ireg][isub] );
      delete m_ftktrack_tomerge_file[ireg][isub];
      m_ftktrack_tomerge_file[ireg][isub] = 0;
    }
  }

  //
  // clean up other used memory;
  //
  log << MSG::DEBUG << "Clean up memory " << endreq;
  if( m_merged_bank ) {
    delete m_merged_bank; 
    m_merged_bank = 0; 
  }
     
  log << MSG::DEBUG << "Clean up track stream " << endreq;
  for (unsigned int ireg=0;ireg!=m_nregions;++ireg) { // region loop
    for (unsigned int isub=0;isub!=m_nsubregions;++isub) { // subregion loop
      if( m_ftktrack_tomerge_stream[ireg][isub] ) { 
	delete m_ftktrack_tomerge_stream[ireg][isub]; 
	m_ftktrack_tomerge_stream[ireg][isub] = 0; 
      }
    }
  }

  log << MSG::DEBUG << "Return success " << endreq;
  return StatusCode::SUCCESS;
}

StatusCode FTKMergerAlgo::mergeSGContent()
{
  MsgStream log(msgSvc(), name());

  // Get information on the events
  const EventInfo* eventInfo(0);
  if( m_StoreGate->retrieve(eventInfo).isFailure() ) {
    log << MSG::ERROR << "Could not retrieve event info" << endreq;
    return StatusCode::FAILURE;
  }
  const EventID* eventID( eventInfo->event_ID() );
  
  log << MSG::DEBUG
      << "entered execution for run " << eventID->run_number()
      << "   event " << eventID->event_number()
      << endreq;

  list<FTKAthTrack> SGMergedList; // list of merged tracks, std::list needed because HW may reject already included tracks

  for (unsigned int ibank=0;ibank!=m_nregions;++ibank) { // banks loop
    // compose the track container name
    ostringstream track_contname;
    track_contname << "FTKTracks" << ibank << ends;
    const char *Ccontname = track_contname.str().c_str();

    if (m_StoreGate->contains<FTKAthTrackContainer>(Ccontname)) {
      const DataHandle<FTKAthTrackContainer> ftktracks_cont;
      StatusCode sc = m_StoreGate->retrieve(ftktracks_cont,Ccontname);
      if (!sc.isSuccess()) {
	log << MSG::ERROR << "Could not get tracks in region " << ibank << endreq;
	return StatusCode::FAILURE;
      }

      log << MSG::DEBUG << "Number of tracks for region " << ibank << " : " << ftktracks_cont->size() << endreq;

      for (FTKAthTrackContainer::const_iterator itrack = ftktracks_cont->begin(); itrack != ftktracks_cont->end(); ++itrack) {

	/// Check you can actually make RDO tracks? ///
	
	/* TODO: before to insert this track in the SGMergedList the hit-warrior needs to be applied.
	   To do that the current track has to be compared with all the tracks in the current list,
	   decided if there is a match. In case of a match only 1 track is accepted: the new one, removing the old one, or the old one, avoiding to add the new one */

	// Hit Warrior //
	FTKAthTrack &track_cur = **itrack;
	// remains 0 if the track has to be added
	// -1 means is worse than an old track (duplicated)
	int accepted(0);
	
	list<FTKAthTrack>::iterator imtrk = SGMergedList.begin();
	// Disable hitwarrior according the flags
	if (m_HW_level==0 || track_cur.getHWRejected()/100!=0)
	  imtrk=SGMergedList.end();
	
	for (;imtrk!=SGMergedList.end();++imtrk) { // loop over tracks of this bank
	  // reference to an old track
	  FTKAthTrack &track_old = *imtrk;
	  if (track_old.getHWRejected()%10 !=0 || track_old.getHWRejected()/100!=0) 
	    // skip HW rejected tracks and tracks with bad chi2,
	    // tracks in removed roads are used, but if a match is found 
	    // will be always rejected or marked as removed, if compared
	    // to a combination in a good road
	    continue; 
	  
	  int HWres = track_cur.HWChoice(track_old,m_HW_dev,m_HW_ndiff,m_HW_level);
  	  
	  if (HWres==-1) {
	    accepted = -1;
	    // passing the end of the list means remove current
	    removeTrack(SGMergedList,SGMergedList.end(),track_cur,track_old,true);
	  }
	  else if (HWres==1) {
	    // return the new current position, it changes if the
	    // rejected track is removed from the list
	    imtrk = removeTrack(SGMergedList,imtrk,track_old,track_cur);
	  }
	} // end loop over tracks of this bank
	
	// create a copy of the track into the merged list
	if (accepted==0) {
	  SGMergedList.push_back(track_cur);
	}
	else if (m_keep_rejected>0) {
	  SGMergedList.push_back(track_cur);	
	}
	//SGMergedList.push_back(**itrack); //old
 
      }
    }
    else {
      log << MSG::DEBUG << "No tracks in region " << ibank  << endreq;
    }
  } // end banks loop


  // prepare the output collection
  m_out_indettrack = new TrigInDetTrackCollection;
  
  StatusCode sc = m_StoreGate->record(m_out_indettrack , m_out_indettrack_Name);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Failure registering FTK LVL2 collection" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::DEBUG << "Setting FTK LVL2 Collection registered" << endreq;
  }

  m_out_trktrack = new TrackCollection;

  StatusCode sc2 = m_StoreGate->record(m_out_trktrack , m_out_trktrack_Name);
  if (sc2.isFailure()) {
    log << MSG::FATAL << "Failure registering FTK Trk::Track" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::DEBUG << "Setting FTK Trk::Track registered" << endreq;
  }

  // JWH Testing adding to storegate ///
  m_ftk_raw_trackcollection = new FTK_RawTrackContainer;

  StatusCode scJ = m_StoreGate->record(m_ftk_raw_trackcollection, m_ftk_raw_trackcollection_Name);
  if (scJ.isFailure()) {
    log << MSG::FATAL << "Failure registering FTK_RawTrackContainer" << endreq;
    return StatusCode::FAILURE;
  } else{
    log << MSG::DEBUG << "Setting FTK_RawTrackContainer registered" << endreq;
  }
  
  m_out_trackPC = new Rec::TrackParticleContainer;
  StatusCode sc1 = m_StoreGate->record(m_out_trackPC , m_out_trackPC_Name);
  if (sc1.isFailure()) {
    log << MSG::FATAL << "Failure registering FTK Track Particle collection" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::DEBUG << "Setting FTK Track Particle Collection registered" << endreq;
  }
  Rec::TrackParticle* aParticle(0);


  FTKAthTrackContainer *SGFinalContainer = new FTKAthTrackContainer;

  list<FTKAthTrack>::iterator imtrack = SGMergedList.begin();
  for (;imtrack!=SGMergedList.end();imtrack = SGMergedList.erase(imtrack)) {
    FTKAthTrack &curtrk = (*imtrack);
    
    // convert the FTKAthTracks in TrigInDetTrack and FTKAthTrackContainer
    // move the content of the FTK track in the TrigInDetTrack format
    TrigInDetTrackFitPar* perigee = new TrigInDetTrackFitPar( curtrk.getIP(),
							      curtrk.getPhi(),
							      curtrk.getZ0(),
							      curtrk.getEta(),
							      curtrk.getPt() );     

    // this will become the pointer to the Id clusters used by the FTK track
    std::vector<const TrigSiSpacePoint*> *ftkclusters = 0x0;
    TrigInDetTrack *trig_track = new TrigInDetTrack( ftkclusters, perigee, NULL, 
						      curtrk.getChi2ndof());

    // set the number of pixel and SCT hits
    unsigned int nPixels(0);
    unsigned int nSCTs(0);

    for (int ip=0;ip!=m_pmap->getNPlanes();++ip) {
      int ipos = m_pmap->getDim(ip,0); // the position in the bitmask
      
      if (curtrk.getBitmask()&(1<<ipos)) {
	// the layer is active, check only the first coordinate is enough
	 
	int ndim = m_pmap->getPlane(ip,0).getNDimension(); // guess the dimension
	switch(ndim) {
	case 1:
	  nSCTs += 1;
	  break;
	case 2:
	  nPixels += 1;
	  break;
	default:
	  log << MSG::FATAL << "Unknown hit format" << endreq;
	  return StatusCode::FAILURE;
	}
      }
    }
    // set the hits information. TODO

    
    // set the FTK track ID, require version 17 or greater
    trig_track->algorithmId(TrigInDetTrack::STRATEGY_F_ID);
    // add the track in the LVL2 container
    m_out_indettrack->push_back(trig_track);

    // convert the FTKAthTracks to the Trk::Track class
    double trkTheta = atan2(1.0,curtrk.getCotTheta());
    double trkQOverP = sin(trkTheta)/curtrk.getPt();

		Trk::PerigeeSurface periSurf;
    Trk::Perigee* trkPerigee = new Trk::Perigee( curtrk.getIP(),
                                                 curtrk.getZ0(),
                                                 curtrk.getPhi(),
                                                 trkTheta,
                                                 trkQOverP,
                                                 periSurf);

    const Trk::TrackStateOnSurface* trkTSoS = new Trk::TrackStateOnSurface( NULL, trkPerigee );
    DataVector<const Trk::TrackStateOnSurface>* trkTSoSVec = new DataVector<const Trk::TrackStateOnSurface>;
    trkTSoSVec->push_back(trkTSoS);

    Trk::TrackInfo* trkTrackInfo = new Trk::TrackInfo(Trk::TrackInfo::Unknown, Trk::pion);

    const Trk::FitQuality* trkFitQuality = new Trk::FitQuality(curtrk.getChi2(), curtrk.getChi2()/curtrk.getChi2ndof());

    Trk::Track* trkTrack = new Trk::Track( *trkTrackInfo, &*trkTSoSVec, &*trkFitQuality);
    
    // add Trk::Track to container
    m_out_trktrack = new TrackCollection;
    m_out_trktrack->push_back(trkTrack);

    //JWH 
    FTK_RawTrack* jay_track = new FTK_RawTrack();
    jay_track->setD0(curtrk.getIP());
    jay_track->setPhi(curtrk.getPhi());
    jay_track->setZ0(curtrk.getZ0());
    m_ftk_raw_trackcollection->push_back(jay_track);

    //TrackParticle container
    if(trkTrack != NULL){
      cout << "FTKMergerAlgo:: Creating the TrackParticle " << endl;
      const Trk::VxCandidate* pVxCandidate = 0 ;
      //      aParticle = TEcreateParticle(trkTrack,pVxCandidate,Trk::NoVtx,trkPerigee,trkFitQuality);
      aParticle = createTrackParticle(trkTrack,pVxCandidate,Trk::NoVtx);
    }

    if (aParticle!=0){
      m_out_trackPC->push_back(aParticle);
      cout << "FTKMergerAlgo:: Non-zero adding " << endl;
    }else{
      cout << "FTKMergerAlgo:: aParticle zero not adding " << endl;
    }
    

    // fill the final list of tracks in the custom format
    // TODO: add a flag to skip this step
    
    SGFinalContainer->push_back(new FTKAthTrack(*imtrack));
    


  }

  sc = m_StoreGate->record(SGFinalContainer,"FTKTracksFinal");
  if (sc.isFailure()) {
    log << MSG::FATAL << "Failure registering final FTK collection" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::DEBUG << "Setting final FTK collection registered" << endreq;
  }

  


  return StatusCode::SUCCESS;


}


/** rejected, or mark as rejected, the track in the list
    against  the second track */
list<FTKAthTrack>::iterator FTKMergerAlgo::removeTrack(list<FTKAthTrack> &tracks_list, list<FTKAthTrack>::iterator itrack,
				     FTKAthTrack &rejtrk, const FTKAthTrack &killer, bool rejnew)
{
  
  if (!rejnew) { // is rejecting a track in the list
    if (!m_keep_rejected) { // remove from the list the rejected
      // save the position before this
      list<FTKAthTrack>::iterator before = itrack;
      // move the to the previous track
      --before;

      // remove the old track from the list
      tracks_list.erase(itrack);

      // assign the itrack to the previous position before, to not invalidate the loop
      itrack = before;
    }
    else if  (rejtrk.getHWRejected()%10==0) { // mark only if not yet marked
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());
      //nfits_rej += 1;  //This is not necessary in StoreGate 
    }

  }
  else {  // is rejecting the new track, not yet in the list
    if (rejtrk.getHWRejected()%10==0) { // mark only the first time
      // the new track is rejected
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());    

      //nfits_rej += 1; //This is not necessary in StoreGate 
    }
  }

  return itrack;
}



StatusCode FTKMergerAlgo::mergePoolFiles()
{
   MsgStream log(msgSvc(), name());

   StatusCode sc;

   // Get information on the events
   const EventInfo* eventInfo(0);
   if( m_StoreGate->retrieve(eventInfo).isFailure() ) {
     log << MSG::ERROR << "Could not retrieve event info" << endreq;
     return StatusCode::FAILURE;
   }
   const EventID* eventID( eventInfo->event_ID() );

   log << MSG::DEBUG
       << "entered execution for run " << eventID->run_number()
       << "   event " << eventID->event_number()
       << endreq;

  //Put all available ExampleClass with key "MyData" in alist
  typedef PileUpMergeSvc::TimedList<FTKAthTrackContainer>::type FTKAthTrackContainerList;
  FTKAthTrackContainerList unmerged_tracks;

  if (!(m_mergeSvc->retrieveSubEvtsData("FTKTracks0", unmerged_tracks).isSuccess())) {
    log << MSG::ERROR << "Could not fill FTKTracks0" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << unmerged_tracks.size() 
	<< " ExampleClassList with key " << "FTKTracks0"
	<< " found" << endreq;
  }


  return StatusCode::SUCCESS;
}


/** This method converts FTKTracks from a merged file in POOL
 * formats: TrigInDetTrack and TrackParticle.
 * The position of the FTKTrack in the input TChain is obtained
 * from a map. */
StatusCode FTKMergerAlgo::convertMergedTracks()
{
  MsgStream log(msgSvc(), name());

  // Get information on the events
  const EventInfo* eventInfo(0);
  if( m_StoreGate->retrieve(eventInfo).isFailure() ) {
    log << MSG::ERROR << "Could not retrieve event info" << endreq;
    return StatusCode::FAILURE;
  }
  const EventID* eventID( eventInfo->event_ID() );
  int eventNumber =  eventID->event_number();

  log << MSG::DEBUG
      << "entered execution for run " << eventID->run_number()
      << "   event " << eventNumber
      << endreq;
  
  //
  //  Get the primary vertex container
  //
  //   const VxContainer* primcontainer(0);
  //   bool primVtxExists = evtStore()->contains<VxContainer>(m_vxCandidatesPrimaryName);
  //   if(!primVtxExists){
  //     log << MSG::INFO << "No VxContainer with key " << m_vxCandidatesPrimaryName << " found!" << endreq;
  //     cout << evtStore()->dump() << endl;;
  //   }
  
  // Extract the vector of tracks found by the FTK for the current event
  std::map<int, Long64_t >::iterator mapIt = m_trackVectorMap.find(eventNumber);
  if(mapIt == m_trackVectorMap.end()) {
    log << MSG::ERROR << "No FTK tracks for event " << eventNumber << " found, possible mismatch between RDO and FTK files"<<endreq;
    return StatusCode::FAILURE;
  }
  
  //Have Tracks
  const Long64_t &FTKEntry = (*mapIt).second;
  if (m_mergedtracks_chain->GetEntry(FTKEntry)==-1) {
   log << MSG::ERROR << "Error reading the FTK entry: " << FTKEntry <<endreq;
    return StatusCode::FAILURE;
  }
 
  int ntracks_merged = m_mergedtracks_stream->getNTracks();
  log << MSG::INFO << "Number of FTK tracks to merge in SG: " << ntracks_merged << endreq;
 
  //
  // Do the InDetTrackCollection First
  //

   // prepare the output collection
   m_out_indettrack = new TrigInDetTrackCollection;
   // m_out_indettrack ->addRef(); // prevent automatic deletion

   /* The SG registration has to be repeated each event because the
      m_out_indetterack is not an IDS container. The memory is freed by
      the SG, no delete call has to be raised */
   StatusCode sc = m_StoreGate->record(m_out_indettrack , m_out_indettrack_Name);
   if (sc.isFailure()) {
     log << MSG::FATAL << "Failure registering FTK LVL2 collection" << endreq;
     return StatusCode::FAILURE;
   }
   else {
     log << MSG::DEBUG << "Setting FTK LVL2 Collection registered" << endreq;
   }

   // registering the TRigInDetTrack collection for the refitted tracks
   m_out_indettrackconv = new TrigInDetTrackCollection;
   StatusCode SCidc = m_StoreGate->record(m_out_indettrackconv, m_out_indettrackconv_Name);
   if (SCidc.isFailure()) {
     log << MSG::FATAL << "Failure registering FTK LVL2 converted collection" << endreq;
     return StatusCode::FAILURE;
   }
    else {
      log << MSG::DEBUG << "Setting FTK LVL2 converted Collection registered" << endreq;
    }

   // create and register the collection for the converted  TrackParticle
   m_out_ftktrackconv = new TrackCollection;
   StatusCode sc3 = m_StoreGate->record(m_out_ftktrackconv , m_out_ftktrackconv_Name);
   if (sc3.isFailure()) {
     log << MSG::FATAL << "Failure registering converted FTK Trk::Track" << endreq;
     return StatusCode::FAILURE;
   }
   else {
     log << MSG::DEBUG << "Setting converted FTK Trk::Track registered" << endreq;
   }

   m_out_ftktrackparticleconv = new Rec::TrackParticleContainer;
   StatusCode SCtpc = m_StoreGate->record(m_out_ftktrackparticleconv, m_out_ftktrackparticleconv_Name);
   if (SCtpc.isFailure()) {
       log << MSG::FATAL << "Failure registering converted FTK TrackParticles" << endreq;
   }
   else {
       log << MSG::DEBUG << "Setting converted FTK TrackParticle registered" << endreq;
   }
   /* Prepare the registration of pixel clusters derived from the FTKHit */
   
   //
   //  Now do the TrackParticles
   //    NOTE we have to record the Trk::Tracks to storegate even if they are not written out 
   //


   m_out_trktrack = new TrackCollection;
   m_out_trackPC  = new Rec::TrackParticleContainer();
   //m_ftk_raw_trackcollection = new FTK_RawTrackContainer();

   sc = m_StoreGate->record(m_out_trktrack , m_out_trktrack_Name);
   if (sc.isFailure()) {
     log << MSG::FATAL << "Failure registering FTK Trk::Track collection" << endreq;
     return StatusCode::FAILURE;
   }
   else {
     log << MSG::DEBUG << "Setting FTK Trk::Track Collection registered" << endreq;
   }


   sc = m_StoreGate->record(m_out_trackPC , m_out_trackPC_Name);
   if (sc.isFailure()) {
     log << MSG::FATAL << "Failure registering FTK TrackParticle collection" << endreq;
     return StatusCode::FAILURE;
   }
   else {
     log << MSG::DEBUG << "Setting FTK TrackParticle Collection registered" << endreq;
   }

   // Check the FTK pixel container
   if (!m_StoreGate->contains<InDet::PixelClusterContainer>(m_FTKPxlClu_CollName)) {
      m_FTKPxlCluContainer->cleanup();
      if (m_StoreGate->record(m_FTKPxlCluContainer,m_FTKPxlClu_CollName,false).isFailure()) {
        log << MSG::FATAL << "Error registering FTK pixel cluster instance" << endreq;
        return StatusCode::FAILURE;
      }
   }

   // check the FTK SCT container
    if (!m_StoreGate->contains<InDet::SCT_ClusterContainer>(m_FTKSCTClu_CollName)) {
      m_FTKSCTCluContainer->cleanup();
      if (m_StoreGate->record(m_FTKSCTCluContainer,m_FTKSCTClu_CollName,false).isFailure()) {
        log << MSG::FATAL << "Error registering FTK SCT cluster instance" << endreq;
        return StatusCode::FAILURE;
      }
   }
 
    // create a vector with the pointer to the tracks selected
    std::vector<FTKTrack*> final_ftktracks(ntracks_merged);
    final_ftktracks.clear();

     //
     // Loop on merged tracks
     //
     for(int itrack=0;itrack!=ntracks_merged;++itrack) {
       
       // get the current track
       FTKTrack *curtrk = m_mergedtracks_stream->getTrack(itrack);
     
       // check only final tracks are moved in the TrigInDetTrack format
       if (curtrk->getHWRejected()) continue;


       final_ftktracks.push_back(curtrk);

     //
     // TrigInDetTrack First
     //

     // build the covariance matrix using parameterized uncertainties
     vector<double> *covmtx = new vector<double>(15,0.);
     (*covmtx)[0]  =  m_uncertiantyTool->getParamCovMtx(curtrk,  FTKTrackParam::d0,   FTKTrackParam::d0);
     (*covmtx)[5]  =  m_uncertiantyTool->getParamCovMtx(curtrk,  FTKTrackParam::phi,  FTKTrackParam::phi);
     (*covmtx)[9]  =  m_uncertiantyTool->getParamCovMtx(curtrk,  FTKTrackParam::z0,   FTKTrackParam::z0);
     (*covmtx)[12] =  m_uncertiantyTool->getParamCovMtx(curtrk,  FTKTrackParam::eta,  FTKTrackParam::eta);
     (*covmtx)[14] =  m_uncertiantyTool->getParamCovMtx(curtrk,  FTKTrackParam::pt,   FTKTrackParam::pt);


     // move the content of the FTK track in the TrigInDetTrack format
     // TODO: fix any possible discrpancy with the standard LVL2 perigee definitions
     TrigInDetTrackFitPar* perigee = new TrigInDetTrackFitPar( curtrk->getIP(),
         curtrk->getPhi(),
         curtrk->getZ0(),
         curtrk->getEta(),
         curtrk->getPt(), covmtx );     

     // this will become the pointer to the Id clusters used by the FTK track
     std::vector<const TrigSiSpacePoint*> *ftkclusters = 0x0;
     TrigInDetTrack *trig_track = new TrigInDetTrack( ftkclusters, perigee, NULL, 
         curtrk->getChi2ndof());


     // set the FTK track ID, require version 17 or greater
     trig_track->algorithmId(TrigInDetTrack::STRATEGY_F_ID);

     // call the conversion tool to connect SP to the TrigInDetTrack in order to
     StatusCode SHRes = m_trackConverterTool->addSiHitInfo(curtrk,trig_track);
     if (SHRes.isFailure()) {
         log << MSG::FATAL << "Error adding SiHitInfo" << endreq;
         return StatusCode::FAILURE;
     }
     //
    // add the TrigInDetTrack to the output collection
     m_out_indettrack->push_back(trig_track);


     //
     // Now do the Trk::Tracks
     //

     //
     // Build the covariance matrix
     //

     AmgSymMatrix(5) *trkTrackCovm = new AmgSymMatrix(5);
     trkTrackCovm->setZero();
     (*trkTrackCovm)(0,0)= m_uncertiantyTool->getParamCovMtx(curtrk,   FTKTrackParam::d0,     FTKTrackParam::d0);
     (*trkTrackCovm)(1,1)= m_uncertiantyTool->getParamCovMtx(curtrk,   FTKTrackParam::z0,     FTKTrackParam::z0);
     (*trkTrackCovm)(2,2)= m_uncertiantyTool->getParamCovMtx(curtrk,   FTKTrackParam::phi,    FTKTrackParam::phi);
     (*trkTrackCovm)(3,3)= m_uncertiantyTool->getParamCovMtx(curtrk,   FTKTrackParam::theta,  FTKTrackParam::theta);
     (*trkTrackCovm)(4,4)= m_uncertiantyTool->getParamCovMtx(curtrk,   FTKTrackParam::qOp,    FTKTrackParam::qOp);
     //
     // Construct the measured Perigee Parameters
     //
     double trkTheta = atan2(1.0,curtrk->getCotTheta());
     double trkQOverP = sin(trkTheta)/curtrk->getPt();
     Trk::PerigeeSurface periSurf;
			
     const Trk::Perigee* trkPerigee = new Trk::Perigee( curtrk->getIP(),
         curtrk->getZ0(),
         curtrk->getPhi(),
         trkTheta,
         trkQOverP,
         periSurf,
         trkTrackCovm);

     //
     // Build the TSOS
     //
     DataVector<const Trk::TrackStateOnSurface>* trkTSoSVec = new DataVector<const Trk::TrackStateOnSurface>;
     trkTSoSVec->push_back( new Trk::TrackStateOnSurface(NULL, trkPerigee));

     //
     // Build the TrackInfo
     //
     Trk::TrackInfo trkTrackInfo = Trk::TrackInfo(Trk::TrackInfo::Unknown, Trk::pion);

     //
     // Build the FitQuality
     //
     const Trk::FitQuality* trkFitQuality = new Trk::FitQuality(curtrk->getChi2(), curtrk->getChi2()/curtrk->getChi2ndof());

     //
     // And finally the Trk::Track
     //
     Trk::Track* trkTrack = new Trk::Track( trkTrackInfo, trkTSoSVec, trkFitQuality);
     //     m_out_trktrack->push_back(trkTrack);

     //     FTK_RawTrack* jay_track = new FTK_RawTrack();
     //     m_ftk_raw_trackcollection->push_back(jay_track);
     //
     //   Now we do the TrackParticles
     //
     Rec::TrackParticle* aParticle(0);

     if(trkTrack != NULL)
       {
	 const Trk::VxCandidate* pVxCandidate = 0 ;

	 unsigned int bitmask=curtrk->getBitmask();
	 
	 aParticle = createTrackParticle(trkTrack, pVxCandidate,Trk::NoVtx, bitmask);
	 
	 if (aParticle!=0) 
	   {
	     //
	     // This magic is needed for persistency 
	     //
	     aParticle->setStorableObject(m_out_trktrack);	  
	     
	     //
	     // Add TrackParticle 
	     //
	     m_out_trackPC->push_back(aParticle);
	   }
       }

     } // end merged tracks loop

     StatusCode result = m_trackConverterTool->convert(final_ftktracks, m_FTKPxlCluContainer,
						       m_FTKSCTCluContainer, m_out_ftktrackconv);

     if(result.isFailure()) {
       log << MSG::DEBUG << "Track converter failure..." << endreq;
     }

     //
     //  Loop on trk tracks:
     //
     for (TrackCollection::iterator nomFTKTrack = m_out_ftktrackconv->begin(); nomFTKTrack != m_out_ftktrackconv->end(); ++nomFTKTrack) {
       Trk::Track* trkTrack_conv = *nomFTKTrack;
       if(trkTrack_conv == NULL) continue;
       
       //
       //  Create Track Summary 
       //
       //cout << "trackSum Before: " << trkTrack_conv->trackSummary() << endl; ;
       const Trk::TrackSummary* summary = m_trackSumTool->createSummaryNoHoleSearch(*trkTrack_conv);
       m_trackSumTool->updateTrack(*trkTrack_conv);

       //cout << "TrackSum After: " << trkTrack_conv->trackSummary() << endl; ;
       //       if(trkTrack_conv->trackSummary()){
       //cout << " NPix " << trkTrack_conv->trackSummary()->get(Trk::numberOfPixelHits) <<endl;
       //}

       Rec::TrackParticle* convParticle(0);

       const Trk::VxCandidate* pVxCandidate = 0 ;

       if(!summary)  summary = new Trk::TrackSummary;
    
       convParticle = createTrackParticle(trkTrack_conv,pVxCandidate,Trk::NoVtx,summary);
       if (convParticle!=0){
	 convParticle->setStorableObject(m_out_ftktrackconv);  
	 m_out_ftktrackparticleconv->push_back(convParticle);

	 //cout << "FTKMergerAlgo:: Non-zero adding conv Track " << endl;

	 //cout << "TrackSum TrackSummary: " << convParticle->trackSummary() << endl; ;
	 // if(convParticle->trackSummary()){
	 //   cout << " NPix " << convParticle->trackSummary()->get(Trk::numberOfPixelHits) <<endl;
	 // }

       }else{
	 cout << "FTKMergerAlgo:: aParticle zero not adding conv Track  " << endl;
       }
       
     }





     result = m_trackConverterTool->convert(m_out_ftktrackconv,m_out_indettrackconv);
     if (result.isFailure()) {
       log << MSG::DEBUG << "Track convertion to TrigInDetTrack failure" << endreq;
     }

   /*

      This is what worked

   //
   // Now Do the TrackPartilces
   //

   const TrackCollection *ftkTracksFromStoreGate = m_out_trktrack;

   // Now prepare for particle creation
   Rec::TrackParticle* aParticle(0);

   for (TrackCollection::const_iterator ftktr  = (*ftkTracksFromStoreGate).begin() ;
   ftktr != (*ftkTracksFromStoreGate).end()   ; ++ftktr)
   {
   const Trk::VxCandidate* pVxCandidate = 0 ;
   aParticle = TEcreateParticle(*ftktr, pVxCandidate,Trk::NoVtx);
   if (aParticle!=0) {
   aParticle->setStorableObject(m_out_trktrack);	  
   m_out_trackPC->push_back(aParticle);
   }
   } */


   // prepare to move to the next event
   m_mergedtracks_iev += 1;  

   return StatusCode::SUCCESS;
}

StatusCode FTKMergerAlgo::finalizeCopyTruthTree() {

   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "about to copy truth tree " << endreq;

   TChain *chain_truthtrack(0), *chain_evt(0);
   if (m_truthTrackTreeName != "") {
      chain_truthtrack = new TChain(m_truthTrackTreeName.c_str());
   }
   if (m_evtinfoTreeName != "") {
      chain_evt = new TChain(m_evtinfoTreeName.c_str());
   }

   for (unsigned int ifile=0;ifile!=m_truthFileNames.size(); ++ifile) { // file loop
      // get the current path
      const string &curfilepath = m_truthFileNames[ifile];
      if (m_truthTrackTreeName != "") {
         chain_truthtrack->Add(curfilepath.c_str());
      }
      if (m_evtinfoTreeName != "") {
         chain_evt->Add(curfilepath.c_str());
      }
   }

   if (m_truthTrackTreeName != "") {
      m_outputFile->cd();
      chain_truthtrack->CloneTree()->Write();
      delete chain_truthtrack;
   }
   if (m_evtinfoTreeName != "") {
      m_outputFile->cd();
      chain_evt->CloneTree()->Write();
      delete chain_evt;
   }
   return StatusCode::SUCCESS;
}
 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKMergerAlgo::finalize() {
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "finalize()" << endreq;

    log << MSG::DEBUG << " truth tree = " << m_truthTrackTreeName << " and evtinfo tree = " << m_evtinfoTreeName << endreq;

    // Copy over the output truth trees, if required
    if (m_truthFileNames.size() > 0 && ((m_truthTrackTreeName != "") || (m_evtinfoTreeName != ""))) {
       StatusCode sc = finalizeCopyTruthTree();
       if (sc.isFailure()) {
          log << MSG::FATAL << "Unable copy truth trees " <<  m_truthTrackTreeName << " and evtinfotree = " << m_evtinfoTreeName << endreq;
          return sc;
       }
    }

    //
    //  Clean up output and input merged files 
    //
    if (m_useStandalone && m_doMerging && !m_GenerateRDO) {
      log << MSG::DEBUG << "finalize standalone tracks" << endreq;
      StatusCode sc = finalizeStandaloneTracks();
      if (sc.isFailure()) {
	log << MSG::FATAL << "Unable finalize merged tracks" << endreq;
	return sc;
      }
    }

    // the cluster container has to be explictly released
    if (m_useStandalone && !m_doMerging) {
      m_FTKPxlCluContainer->cleanup();
      m_FTKPxlCluContainer->release();
    }

    // cleanup the map containing the list of FTK tracks found in each event
    log << MSG::DEBUG << "Dump Track map" << endreq;
    m_trackVectorMap.clear();

    log << MSG::DEBUG << "end Finalize() " << endreq;
    return StatusCode::SUCCESS;
}


//
Rec::TrackParticle* FTKMergerAlgo::createTrackParticle(const Trk::Track* track,
						       const Trk::VxCandidate* vxCandidate,
						       Trk::TrackParticleOrigin prtOrigin,
						       unsigned int bitmask)
{
  //routine to recreate CreateParticleTool since the Tool does not seem to work

  if (track == 0) return 0;
  const Trk::Perigee* aMeasPer = track->perigeeParameters();
  if (aMeasPer->covariance()==0) return 0; 
  aMeasPer = aMeasPer->clone();

  //Creating TrackParticle with a dummy TrackSummary
  const Trk::TrackSummary* summary;

  //
  //  If bitmask, set the number of hits
  //
  if(bitmask){

    std::vector<int> tracksum;
    for (int i = 0; i < Trk::numberOfTrackSummaryTypes; i++)
      tracksum.push_back(-1);

    //
    // pixels have hit for each coordinate, so only need to count 1
    //
    int npixel(0);
    int npixelholes(0);
    for (int ix=0;ix!=m_SCTCoordOffset;ix+=2) { // loop over the PXL coordinates, jump of 2
       npixel += bool(bitmask&(1<<ix));
       npixelholes += !bool(bitmask&(1<<ix));
    }
    
    int nsct(0);
    int nsctholes(0);
    for (int ix=m_SCTCoordOffset;ix!=m_NCoords;++ix) { // Loop over hte SCT coordinates
        nsct += bool(bitmask&(1<<ix));
        nsctholes += !bool(bitmask&(1<<ix));
    }
            

    tracksum[Trk::numberOfBLayerHits]  = bitmask&(1<<0);
    tracksum[Trk::numberOfPixelHits]   = npixel;
    tracksum[Trk::numberOfPixelHoles]  = npixelholes;
    tracksum[Trk::numberOfSCTHits]     = nsct;
    tracksum[Trk::numberOfSCTHoles]    = nsctholes;
    tracksum[Trk::expectBLayerHit]     =  1.0;


    tracksum[Trk::numberOfBLayerOutliers          ] = 0;
    tracksum[Trk::numberOfBLayerSharedHits        ] = 0;
    tracksum[Trk::numberOfPixelOutliers           ] = 0;
    tracksum[Trk::numberOfPixelSharedHits         ] = 0;
    tracksum[Trk::numberOfGangedPixels            ] = 0;
    tracksum[Trk::numberOfGangedFlaggedFakes      ] = 0;
    tracksum[Trk::numberOfPixelDeadSensors        ] = 0;
    tracksum[Trk::numberOfPixelSpoiltHits         ] = 0;
    tracksum[Trk::numberOfSCTOutliers             ] = 0;
    tracksum[Trk::numberOfSCTDoubleHoles          ] = 0;
    tracksum[Trk::numberOfSCTSharedHits           ] = 0;
    tracksum[Trk::numberOfSCTDeadSensors          ] = 0;
    tracksum[Trk::numberOfSCTSpoiltHits           ] = 0;
    tracksum[Trk::numberOfTRTHits                 ] = 0;
    tracksum[Trk::numberOfTRTOutliers             ] = 0;
    tracksum[Trk::numberOfTRTHoles                ] = 0;
    tracksum[Trk::numberOfTRTHighThresholdHits    ] = 0;
    tracksum[Trk::numberOfTRTHighThresholdOutliers] = 0;
    tracksum[Trk::numberOfTRTDeadStraws           ] = 0;
    tracksum[Trk::numberOfTRTTubeHits             ] = 0;

    std::vector<float> prob;
    for( int i = 0; i < Trk::numberOfeProbabilityTypes; i++)
      prob.push_back(-1);
    
    std::bitset<Trk::numberOfDetectorTypes> dettype = 0;
    summary = new Trk::TrackSummary(tracksum,prob,dettype);
    
  }else{ 

    //
    // no bit mask, create dummy TrackSummary
    //
    summary = new Trk::TrackSummary;

  }

  // find the first and the last hit in track
  // we do that the same way as in the track slimming tool!
  // that way it is also ok on not slimmed tracks!
  std::vector<const Trk::TrackParameters*> parameters;
   {
     const DataVector<const Trk::TrackStateOnSurface>* trackStates = track->trackStateOnSurfaces();
     
     const Trk::TrackParameters* first(0) ;
     // search first valid TSOS first
     for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator itTSoS = trackStates->begin(); 
	   itTSoS != trackStates->end(); ++itTSoS)
       {
	 if ( (*itTSoS)->type(Trk::TrackStateOnSurface::Measurement) && (*itTSoS)->trackParameters()!=0 && (*itTSoS)->measurementOnTrack()!=0 )
	   {
	     first = (*itTSoS)->trackParameters();
	     parameters.push_back((*itTSoS)->trackParameters()->clone());
	     break;
	   }
	}
     
     // search last valid TSOS first
     for ( DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator rItTSoS = trackStates->rbegin(); 
	   rItTSoS != trackStates->rend(); ++rItTSoS)
       {
	 if ( (*rItTSoS)->type(Trk::TrackStateOnSurface::Measurement) && (*rItTSoS)->trackParameters()!=0 && (*rItTSoS)->measurementOnTrack()!=0 )
	   {
	     if (!(first == (*rItTSoS)->trackParameters())) {
	       parameters.push_back((*rItTSoS)->trackParameters()->clone());
	       break;
	     }
	   }
       }
     


     // security check:
     if (parameters.size() > 2) msg() << MSG::WARNING << "More than two additional track parameters to be stored in TrackParticle!" << endreq;
   }   

   const Trk::FitQuality* fitQuality = new Trk::FitQuality( (*track->fitQuality()) );
   return new Rec::TrackParticle(track, prtOrigin, vxCandidate, summary, parameters, aMeasPer,  fitQuality);

}

//
Rec::TrackParticle* FTKMergerAlgo::createTrackParticle(const Trk::Track* track,
						       const Trk::VxCandidate* vxCandidate,
						       Trk::TrackParticleOrigin prtOrigin,
						       const Trk::TrackSummary* summary)
{
  //routine to recreate CreateParticleTool since the Tool does not seem to work
 
  if (track == 0) return 0;
  const Trk::Perigee* aMeasPer(0);
  aMeasPer = dynamic_cast<const Trk::Perigee*>( track->perigeeParameters() );
  if (aMeasPer==0) return 0; 
  aMeasPer = aMeasPer->clone();

  // find the first and the last hit in track
  // we do that the same way as in the track slimming tool!
  // that way it is also ok on not slimmed tracks!
  std::vector<const Trk::TrackParameters*> parameters;
  {
    const DataVector<const Trk::TrackStateOnSurface>* trackStates = track->trackStateOnSurfaces();
  
    const Trk::TrackParameters* first(0) ;
    // search first valid TSOS first
    for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator itTSoS = trackStates->begin(); 
	  itTSoS != trackStates->end(); ++itTSoS)
      {
	if ( (*itTSoS)->type(Trk::TrackStateOnSurface::Measurement) && (*itTSoS)->trackParameters()!=0 && (*itTSoS)->measurementOnTrack()!=0 )
	  {
	    first = (*itTSoS)->trackParameters();
	    parameters.push_back((*itTSoS)->trackParameters()->clone());
	    break;
	  }
      }
   
    // search last valid TSOS first
    for ( DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator rItTSoS = trackStates->rbegin(); 
	  rItTSoS != trackStates->rend(); ++rItTSoS)
      {
	if ( (*rItTSoS)->type(Trk::TrackStateOnSurface::Measurement) && (*rItTSoS)->trackParameters()!=0 && (*rItTSoS)->measurementOnTrack()!=0 )
	  {
	    if (!(first == (*rItTSoS)->trackParameters())) {
	      parameters.push_back((*rItTSoS)->trackParameters()->clone());
	      break;
	    }
	  }
      }
   


    // security check:
    if (parameters.size() > 2) msg() << MSG::WARNING << "More than two additional track parameters to be stored in TrackParticle!" << endreq;
  }   

  const Trk::FitQuality* fitQuality = new Trk::FitQuality( (*track->fitQuality()) );
 
  return new Rec::TrackParticle(track, prtOrigin, vxCandidate, summary, parameters, aMeasPer,  fitQuality);
}






/** rejected, or mark as rejected, the track in the list
    against  the second track */
list<FTKTrack>::iterator FTKMergerAlgo::removeTrack(list<FTKTrack> &tracks_list, list<FTKTrack>::iterator itrack,
				     FTKTrack &rejtrk, const FTKTrack &killer, bool rejnew)
{
  
  if (!rejnew) { // is rejecting a track in the list
    if (!m_keep_rejected) { // remove from the list the rejected
      // save the position before this
      list<FTKTrack>::iterator before = itrack;
      // move the to the previous track
      --before;

      // remove the old track from the list
      tracks_list.erase(itrack);

      // assign the itrack to the previous position before, to not invalidate the loop
      itrack = before;
    }
    else if  (rejtrk.getHWRejected()%10==0) { // mark only if not yet marked
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());
      m_nfits_rej += 1;
    }

  }
  else {  // is rejecting the new track, not yet in the list
    if (rejtrk.getHWRejected()%10==0) { // mark only the first time
      // the new track is rejected
      // mark the old track as rejected
      rejtrk.setHWRejected( rejtrk.getHWRejected()+1 );
      // assign the ID of the track rejected this
      rejtrk.setHWTrackID(killer.getTrackID());    

      m_nfits_rej += 1;
    }
  }

  return itrack;
}


/** this function merges the tracks coming from the banks produced
    fitting the roads in a single banks, the output will have tracks
    from roads found in all the detectors*/
void FTKMergerAlgo::merge_tracks(FTKTrackStream* &merged_tracks, FTKTrackStream ***reg_tracks)
{
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "In merged_tracks " << endreq;

  list<FTKTrack> tracks_list;
  
  // reset rejected fits counter
  m_nfits_rej = 0;

  //
  // copy run and event number to the output, and check that the
  // banks are in sync.
  //
  unsigned long bank_run = 0ul;
  unsigned long bank_event = 0ul;
  for (unsigned int ireg=0; ireg!=m_nregions;++ireg) { // banks loop
    for ( unsigned int isub=0; isub!=m_nsubregions;++isub) { // subregions loop
      if(!reg_tracks[ireg][isub]) continue;
      unsigned long this_run = reg_tracks[ireg][isub]->runNumber();
      unsigned long this_event = reg_tracks[ireg][isub]->eventNumber();
      //if( this_run==0 && this_event==0 ) { continue; }
      bank_run = this_run;
      bank_event = this_event;
      break;
    }
  }
  merged_tracks->setRunNumber( bank_run );
  merged_tracks->setEventNumber( bank_event );
  //if( bank_run!=0 || bank_event!=0 ) {
  bool event_sync = true;
  for(unsigned int ireg=0; ireg!=m_nregions; ++ireg) {
    for (unsigned int isub=0; isub!=m_nsubregions;++isub) {
      if(!reg_tracks[ireg][isub]) continue;
      unsigned long this_run = reg_tracks[ireg][isub]->runNumber();
      unsigned long this_event = reg_tracks[ireg][isub]->eventNumber();
      if( this_run != bank_run || this_event != bank_event ) {
	event_sync = false;
	break;
      }
    }
  }
  if( !event_sync ) {
    std::cout << "BANKS ARE OUT OF SYNC!!!" << std::endl;
    std::cout << "Bank\t\tSubregion\t\tRun\t\tEvent!" << std::endl;
    for(unsigned int ireg=0; ireg!=m_nregions; ++ireg) {
      for(unsigned int isub=0; isub!=m_nsubregions; ++isub) {
	std::cout << ireg << "\t\t" << isub << "\t\t" << reg_tracks[ireg][isub]->runNumber() << "\t\t" << reg_tracks[ireg][isub]->eventNumber() << std::endl;
      }
    }
    return;
  }

  //
  //  The Merging 
  //
  for (unsigned int ireg=0;ireg!=m_nregions;++ireg) { // bank loop
    for (unsigned int isub=0; isub!=m_nsubregions;++isub) { // subregions loop
      if(!reg_tracks[ireg][isub]) continue;
      
      unsigned int ntracks = reg_tracks[ireg][isub]->getNTracks();
      
      merged_tracks->addNCombs(reg_tracks[ireg][isub]->getNCombs());
      merged_tracks->addNFits( reg_tracks[ireg][isub]->getNFits());
      merged_tracks->addNFitsMajority(reg_tracks[ireg][isub]->getNFitsMajority());
      merged_tracks->addNFitsRecovery(reg_tracks[ireg][isub]->getNFitsRecovery());
      merged_tracks->addNAddFitsRecovery(reg_tracks[ireg][isub]->getNAddFitsRecovery());
      merged_tracks->addNFitsBad(reg_tracks[ireg][isub]->getNFitsBad());
      merged_tracks->addNFitsHWRejected(reg_tracks[ireg][isub]->getNFitsHWRejected());
      merged_tracks->addNFitsBadMajority(reg_tracks[ireg][isub]->getNFitsBadMajority());
      merged_tracks->addNFitsHWRejectedMajority(reg_tracks[ireg][isub]->getNFitsHWRejectedMajority());


      unsigned int ntracksI = reg_tracks[ireg][isub]->getNTracksI();
      merged_tracks->addNCombsI(reg_tracks[ireg][isub]->getNCombsI());
      merged_tracks->addNFitsI(reg_tracks[ireg][isub]->getNFitsI());
      merged_tracks->addNFitsMajorityI(reg_tracks[ireg][isub]->getNFitsMajorityI());
      merged_tracks->addNFitsRecoveryI(reg_tracks[ireg][isub]->getNFitsRecoveryI());
      merged_tracks->addNAddFitsRecoveryI(reg_tracks[ireg][isub]->getNAddFitsRecoveryI());
      merged_tracks->addNFitsBadI(reg_tracks[ireg][isub]->getNFitsBadI());
      merged_tracks->addNFitsHWRejectedI(reg_tracks[ireg][isub]->getNFitsHWRejectedI());
      merged_tracks->addNFitsBadMajorityI(reg_tracks[ireg][isub]->getNFitsBadMajorityI());
      merged_tracks->addNFitsHWRejectedMajorityI(reg_tracks[ireg][isub]->getNFitsHWRejectedMajorityI());
      
      for (unsigned int itr=0;itr!=ntracks;++itr) { // track loop

	// get the track from the bank
	FTKTrack &newtrack = *(reg_tracks[ireg][isub]->getTrack(itr));

	//	FTK_RawTrack* test;
	//	std::cout << "JWH TEST TRACK BEFORE 2 " << std::endl;	
	//	std::cout << "Track Collection =  " << m_ftk_raw_trackcollection << std::endl;	
	//	m_ftk_raw_trackcollection->push_back(test);
	//	std::cout << "JWH TEST TRACK AFTER 2" << std::endl;

	// remains 0 if the track has to be added
	// -1 means is worse than an old track (duplicated)
	int accepted(0);
	
	list<FTKTrack>::iterator itrack = tracks_list.begin();
	// Disable hitwarrior according the flags
	if (m_HW_level==0 || newtrack.getHWRejected()/100!=0)
	  itrack=tracks_list.end();
	
	for (;itrack!=tracks_list.end();++itrack) { // loop over tracks of this bank
	  // reference to an old track
	  FTKTrack &track_old = *itrack;
	  if (track_old.getHWRejected()%10 !=0 || track_old.getHWRejected()/100!=0) 
	    // skip HW rejected tracks and tracks with bad chi2,
	    // tracks in removed roads are used, but if a match is found 
	    // will be always rejected or marked as removed, if compared
	    // to a combination in a good road
	    continue; 
	  
	  int HWres = newtrack.HWChoice(track_old,m_HW_dev,m_HW_ndiff,m_HW_level);
	  
	  if (HWres==-1) {
	    accepted = -1;
	    // passing the end of the list means remove current
	    removeTrack(tracks_list,tracks_list.end(),newtrack,track_old,true);
	  }
	  else if (HWres==1) {
	    // return the new current position, it changes if the
	    // rejected track is removed from the list
	    itrack = removeTrack(tracks_list,itrack,track_old,newtrack);
	  }
	  
	} // end loop over tracks of this bank

     
	// create a copy of the track into the merged list
	if (accepted==0) {
	  tracks_list.push_back(newtrack);
	}
	else if (m_keep_rejected>0) {
	  tracks_list.push_back(newtrack);	
	}
      } // end track loop



      for (unsigned int itrI=0;itrI!=ntracksI;++itrI) { // intermediate track loop
	// get the track from the bank
	FTKTrack &newtrack = *(reg_tracks[ireg][isub]->getTrackI(itrI));
	
	// the intermediate tracks are not filtered by HW
	merged_tracks->addTrackI(newtrack);
      } // end intermediate track loop

    } // end subregion loop
  } // end bank loop
  
  // synchronize the rejected counter
  merged_tracks->addNFitsHWRejected(m_nfits_rej);
  
  // push the tracks into the output stream
  list<FTKTrack>::iterator itrack = tracks_list.begin();
  for (;itrack!=tracks_list.end();itrack = tracks_list.erase(itrack)) { // loop over merged tracks
    merged_tracks->addTrack(*itrack);
  } // end loop over merged tracks
}


//
//  Format the input file names
//
std::string FTKMergerAlgo::GetPathName(unsigned int regNum, unsigned int subNum)
{
  if(m_doGrid){
    unsigned int fileIndex = m_nsubregions * regNum + subNum;
    std::string thisPath = m_ftktrack_mergeFileRoot + Form(".%06d.root.1",fileIndex);
    return thisPath;
  }

  std::string thisPath = m_ftktrack_mergeInputPath + "/"+ m_ftktrack_mergeFileRoot + Form("_reg%d_sub%02d.root",regNum,subNum);

  return thisPath;
}

//
// Get sigma(Q/P) from sigma(1/2pT) and sigma(eta) 
//    (NOTE: invpt = 1/2pT)
double FTKMergerAlgo::getSigmaQoverP(double invpt, double sigmaTwoInvPt, double eta,  double sigmaEta)
{
  //
  // (Might not be necessary, but lets to it right.)
  //  
  //   We have paramaterized the uncertianty of 1/2xPt and eta
  //    (NOTE: invpt = 1/2pT)
  //
  //
  // But we want sigma q/p := "f"
  //   define sigmaTwoInvPt := "g"
  //
  //   f = 2g/cosh(eta)
  //  
  // (After calculus) 
  //
  //  sigma(f) = 2/cosh(eta) x sqrt( sigma(g)^2 + (gxtanh(eta))^2 )
  //
  
  double secondTerm = invpt*tanh(eta)*sigmaEta;
  double coshEta    = cosh(eta); 
  double sigmaQoverP = coshEta ? 2./cosh(eta) * sqrt( sigmaTwoInvPt*sigmaTwoInvPt + secondTerm*secondTerm) : 10 ;
  return sigmaQoverP;
}

double FTKMergerAlgo::getSigmaTheta(double eta, double sigmaEta)
{
  //
  //  sigma(theta) = d(theta)/d(eta) x sigma(eta)
  //
  //  and,   d(theta)/d(eta) = 1./cosh(eta)
  //
  double coshEta    = cosh(eta); 
  double sigmaTheta = coshEta ? sigmaEta/coshEta : 10;
  return sigmaTheta;
}

FTK_RawTrack* FTKMergerAlgo::SimToRaw(const FTKTrack &track)
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "FTKMergerAlgo::SimToRaw()" << endreq;

  // Words for Track Parameters and cluster objects//
  uint32_t word_1(0), 
    word_2(0), 
    word_3(0), 
    word_4(0), 
    word_5(0);
  /*  FTK_RawPixelCluster* pixel_cluster_1 = new FTK_RawPixelCluster(0);
  FTK_RawPixelCluster* pixel_cluster_2 = new FTK_RawPixelCluster(1);
  FTK_RawPixelCluster* pixel_cluster_3 = new FTK_RawPixelCluster(2);
  FTK_RawPixelCluster* pixel_cluster_4 = new FTK_RawPixelCluster(3);*/
  FTK_RawPixelCluster pixel_cluster_1(0), pixel_cluster_2(1), pixel_cluster_3(2), pixel_cluster_4(3);

  FTK_RawSCT_Cluster  sct_cluster_1(4), 
    sct_cluster_2(5), 
    sct_cluster_3(6), 
    sct_cluster_4(7), 
    sct_cluster_5(8), 
    sct_cluster_6(9), 
    sct_cluster_7(10), 
    sct_cluster_8(11);

  //  std::vector<FTK_RawPixelCluster*> pixel_clusters;
  std::vector<FTK_RawPixelCluster> pixel_clusters;
  std::vector<FTK_RawSCT_Cluster>  sct_clusters;
  bool p1(0), p2(0), p3(0), p4(0);
  bool f1(0), f2(0), f3(0), f4(0), f5(0), f6(0), f7(0), f8(0);
  for (int i = 0; i<track.getNPlanes(); ++i){
    const FTKHit& hit = track.getFTKHit(i);
    int type = hit.getDim();
    if ( type == 1){ // SCT
      if( i == 4 ){
	f1 = true;
	sct_cluster_1.setModuleID(hit.getIdentifierHash());
	sct_cluster_1.setHitCoord(hit.getCoord(0)); // loc pos
	sct_cluster_1.setHitWidth(hit.getNStrips()); // width
      } else if( i == 5 ){
	f2 = true;
	sct_cluster_2.setModuleID(hit.getIdentifierHash());
	sct_cluster_2.setHitCoord(hit.getCoord(0));
	sct_cluster_2.setHitWidth(hit.getNStrips());
      } else if( i == 6 ){
	f3 = true;
	sct_cluster_3.setModuleID(hit.getIdentifierHash());
	sct_cluster_3.setHitCoord(hit.getCoord(0));
	sct_cluster_3.setHitWidth(hit.getNStrips());
      } else if( i == 7 ){
	f4 = true;
	sct_cluster_4.setModuleID(hit.getIdentifierHash());
	sct_cluster_4.setHitCoord(hit.getCoord(0));
	sct_cluster_4.setHitWidth(hit.getNStrips());
      } else if( i == 8 ){
	f5 = true;
	sct_cluster_5.setModuleID(hit.getIdentifierHash());
	sct_cluster_5.setHitCoord(hit.getCoord(0));
	sct_cluster_5.setHitWidth(hit.getNStrips());
      } else if( i == 9 ){
	f6 = true;
	sct_cluster_6.setModuleID(hit.getIdentifierHash());
	sct_cluster_6.setHitCoord(hit.getCoord(0));
	sct_cluster_6.setHitWidth(hit.getNStrips());
      } else if( i == 10 ){
	f7 = true;
	sct_cluster_7.setModuleID(hit.getIdentifierHash());
	sct_cluster_7.setHitCoord(hit.getCoord(0));
	sct_cluster_7.setHitWidth(hit.getNStrips());
      } else if( i == 11 ){
	f8 = true;
	sct_cluster_8.setModuleID(hit.getIdentifierHash());
	sct_cluster_8.setHitCoord(hit.getCoord(0));
	sct_cluster_8.setHitWidth(hit.getNStrips());
      }
    } else if ( type == 2){ // PIXEL //
      if ( i == 0 ){
	p1 = true;
	pixel_cluster_1.setModuleID(hit.getIdentifierHash());
	pixel_cluster_1.setRowCoord(hit.getCoord(0));
	pixel_cluster_1.setRowWidth(hit.getPhiWidth());
	pixel_cluster_1.setColCoord(hit.getCoord(1)); 
	pixel_cluster_1.setColWidth(hit.getEtaWidth());
      } else if (i == 1){
	p2 = true;
	pixel_cluster_2.setModuleID(hit.getIdentifierHash());
	pixel_cluster_2.setRowCoord(hit.getCoord(0));
	pixel_cluster_2.setRowWidth(hit.getPhiWidth());
	pixel_cluster_2.setColCoord(hit.getCoord(1)); 
	pixel_cluster_2.setColWidth(hit.getEtaWidth());
      } else if (i == 2){
	p3 = true;
	pixel_cluster_3.setModuleID(hit.getIdentifierHash());
	pixel_cluster_3.setRowCoord(hit.getCoord(0));
	pixel_cluster_3.setRowWidth(hit.getPhiWidth());
	pixel_cluster_3.setColCoord(hit.getCoord(1)); 
	pixel_cluster_3.setColWidth(hit.getEtaWidth());
      } else if (i == 3){
	p4 = true;
	pixel_cluster_4.setModuleID(hit.getIdentifierHash());
	pixel_cluster_4.setRowCoord(hit.getCoord(0));
	pixel_cluster_4.setRowWidth(hit.getPhiWidth());
	pixel_cluster_4.setColCoord(hit.getCoord(1)); 
	pixel_cluster_4.setColWidth(hit.getEtaWidth());
      }
    }
  } // end of hit loop
  
  pixel_clusters.push_back(pixel_cluster_1);
  pixel_clusters.push_back(pixel_cluster_2);
  pixel_clusters.push_back(pixel_cluster_3);
  pixel_clusters.push_back(pixel_cluster_4);
  sct_clusters.push_back(sct_cluster_1);
  sct_clusters.push_back(sct_cluster_2);
  sct_clusters.push_back(sct_cluster_3);
  sct_clusters.push_back(sct_cluster_4);
  sct_clusters.push_back(sct_cluster_5);
  sct_clusters.push_back(sct_cluster_6);
  sct_clusters.push_back(sct_cluster_7);
  sct_clusters.push_back(sct_cluster_8);

  /// Road ID convert to 32 bit ///
  unsigned int road_id = (unsigned int)track.getRoadID();
  word_1 = road_id; 

  /// Pack D0 and Z0 ///
  uint32_t d0   = 0;
  uint32_t z0   = 0;
  uint32_t phi  = 0;
  uint32_t cot  = 0;
  uint32_t chi  = 0;
  uint32_t curv = 0;

  if(!(fabs(track.getIP())>32.767))
    d0   = NINT( 10e2*track.getIP() + 32767);
  if(!(fabs(track.getZ0())>327.67))
    z0   = NINT( 100*track.getZ0() + 32767);
  if(!(fabs(track.getPhi()>TMath::Pi())))
    phi  = NINT( 10000*track.getPhi() + 32767);
  if(!(fabs(track.getCotTheta())>6.5))
    cot  = NINT( 5000*track.getCotTheta() + 32767);
  if(!(fabs(track.getInvPt())>66))
    curv = NINT( 5*10e6*track.getInvPt() + 32767);

  d0 = d0 << 16;
  d0 = d0 | z0;
  word_2 = d0;

  cot = cot << 16;
  cot = cot | phi;
  word_3 = cot;

  chi = chi << 16;
  chi = chi | curv;
  word_4 = chi;

  FTK_RawTrack *raw_track = new FTK_RawTrack(word_1, word_2, word_3, word_4, word_5, pixel_clusters, sct_clusters);
//  FTK_RawTrack *raw_track = new FTK_RawTrack(word_1, word_2, word_3, word_4, word_5);
  raw_track->setZ0(track.getZ0());
  raw_track->setPhi(track.getPhi());
  raw_track->setCotTh(track.getCotTheta());
  raw_track->setCurv(track.getInvPt());
  raw_track->setBarcode(track.getBarcode());

  log << MSG::DEBUG << "FTK_RawTrack Parameter Summary: " << endreq;
  log << MSG::DEBUG << " - Road ID = " << raw_track->getRoadID() << " " << track.getRoadID() << endreq;
  log << MSG::DEBUG << " - Road ID = " << raw_track->getTH1() << " " << track.getRoadID() << endreq;
  log << MSG::DEBUG << " - d0      = " << raw_track->getD0()     << " " << track.getIP() << endreq;
  log << MSG::DEBUG << " - z0      = " << raw_track->getZ0()     << " " << track.getZ0() << endreq;
  log << MSG::DEBUG << " - phi     = " << raw_track->getPhi()    << " " << track.getPhi() << endreq;
  log << MSG::DEBUG << " - cot     = " << raw_track->getCotTh()  << " " << track.getCotTheta() << endreq;
  log << MSG::DEBUG << " - ipt     = " << raw_track->getCurv()   << " " << track.getInvPt() << endreq;
  log << MSG::DEBUG << " - barcode = " << raw_track->getBarcode()<< " " << track.getBarcode() << endreq;
  for( unsigned int i = 0; i < raw_track->getPixelClusters().size(); ++i){
    log << MSG::DEBUG << " PIXEL Cluster " << i << endreq;
    log << MSG::DEBUG << "       Layer      = " << raw_track->getPixelClusters()[i].getLayer() << endreq;
    log << MSG::DEBUG << "       Col Coord  = " << raw_track->getPixelClusters()[i].getColCoord() << endreq;
    log << MSG::DEBUG << "       Col Width  = " << raw_track->getPixelClusters()[i].getColWidth() << endreq;
    log << MSG::DEBUG << "       Row Coord  = " << raw_track->getPixelClusters()[i].getRowCoord() << endreq;
    log << MSG::DEBUG << "       Row Width  = " << raw_track->getPixelClusters()[i].getRowWidth() << endreq;
    log << MSG::DEBUG << "       Hash ID    = " << raw_track->getPixelClusters()[i].getModuleID() << endreq;
    const FTKHit& hit = track.getFTKHit(i);
    //    if (!raw_track->getPixelClusters()[i].getLayer() ==-1){
    if ( (p1 && i == 0) ||
	 (p2 && i == 1) ||
	 (p3 && i == 2) ||
	 (p4 && i == 3)){
      log << MSG::DEBUG << "       t Col Coord    = " << hit.getCoord(1)   << endreq;
      log << MSG::DEBUG << "       t Col Width    = " << hit.getEtaWidth() << endreq;
      log << MSG::DEBUG << "       t Row Coord    = " << hit.getCoord(0)   << endreq;
      log << MSG::DEBUG << "       t Row Width    = " << hit.getPhiWidth() << endreq;
      log << MSG::DEBUG << "       t Hash ID      = " << hit.getIdentifierHash() << endreq;
    }
  }
  for( unsigned int i = 0; i < raw_track->getSCTClusters().size(); ++i){
    log << MSG::DEBUG << " SCT Cluster " << i << endreq;
    log << MSG::DEBUG << "       Layer        = " << raw_track->getSCTClusters()[i].getLayer() << endreq;
    log << MSG::DEBUG << "       Hit Coord    = " << raw_track->getSCTClusters()[i].getHitCoord() << endreq;
    log << MSG::DEBUG << "       Hit Width    = " << raw_track->getSCTClusters()[i].getHitWidth() << endreq;
    log << MSG::DEBUG << "       Module ID    = " << raw_track->getSCTClusters()[i].getModuleID() << endreq;
    if ( (f1 && i == 0) ||
	 (f2 && i == 1) ||
	 (f3 && i == 2) ||
	 (f4 && i == 3) ||
	 (f5 && i == 4) ||
	 (f6 && i == 5) ||
	 (f7 && i == 6) ||
	 (f8 && i == 7)){//!raw_track->getSCTClusters()[i].getLayer() ==-1){
      const FTKHit& hit = track.getFTKHit(i+4);
      log << MSG::DEBUG << "       a Coord     = " << hit.getCoord(0)  << endreq;
      log << MSG::DEBUG << "       a NStrips   = " << hit.getNStrips() << endreq;
      log << MSG::DEBUG << "       a Hash ID   = " << hit.getIdentifierHash() << endreq;
    } 
  }
  log << MSG::DEBUG << endreq;
  return raw_track;
}

void FTKMergerAlgo::printBits(unsigned int num, unsigned int length){
  for(unsigned int bit=0;bit<(sizeof(unsigned int) * 8) && bit < length; bit++){
    std::cout << int(num & 0x01);
    num = num >> 1;
  }
  std::cout << std::endl;
}


void FTKMergerAlgo::merge_roads(FTKRoadStream * &newbank,FTKRoadStream **oldbanks, int nelements)
{
  // copy run and event number to the output, and check that the
  // subregions are in sync.
  unsigned long sr_run = 0ul;
  unsigned long sr_event = 0ul;
  for (int isr=0;isr!=nelements;++isr) { // subregions loop
    unsigned long this_run = oldbanks[isr]->runNumber();
    unsigned long this_event = oldbanks[isr]->eventNumber();
    if( this_run==0 && this_event==0 ) { continue; }
    sr_run = this_run;
    sr_event = this_event;
    break;
  }
  newbank->setRunNumber( sr_run );
  newbank->setEventNumber( sr_event );
  if( sr_run!=0 || sr_event!=0 ) {
    bool event_sync = true;
    for(int isr=0; isr!=nelements; ++isr) {
      unsigned long this_run = oldbanks[isr]->runNumber();
      unsigned long this_event = oldbanks[isr]->eventNumber();
      if( this_run != sr_run || this_event != sr_event ) {
        event_sync = false;
        break;
      }
    }
    if( !event_sync ) {
      std::cout << "SUBREGION BANKS ARE OUT OF SYNC!!!" << std::endl;
      std::cout << "Subregion\t\tRun\t\tEvent!" << std::endl;
      for(int isr=0; isr!=nelements; ++isr) {
        std::cout << isr << "\t\t" << oldbanks[isr]->runNumber() << "\t\t" << oldbanks[isr]->eventNumber() << std::endl;
      }
      return;
    }
  }

  // merge together the roads from different regions or subregions
  for (int isr=0;isr<nelements;++isr) { // subregions loop

    // merge statistical informations
    if(isr==0) {
      // total # of hits and clusters is the same in each subregion, so we do it once
      newbank->naoSetNhitsTot(oldbanks[isr]->naoGetNhitsTot());
      newbank->naoSetNclusTot(oldbanks[isr]->naoGetNclusTot());
      // initialize counters from the first subregion
      newbank->naoSetNclus(oldbanks[isr]->naoGetNclus());
      newbank->naoSetNss(oldbanks[isr]->naoGetNss());
      newbank->naoSetNroadsAM(oldbanks[isr]->naoGetNroadsAM());
      newbank->naoSetNroadsMOD(oldbanks[isr]->naoGetNroadsMOD());
    }
    else {
      // increment counters from other subregions
      newbank->naoAddNroadsAM(oldbanks[isr]->naoGetNroadsAM());
      newbank->naoAddNroadsMOD(oldbanks[isr]->naoGetNroadsMOD());
    }

    int nroads = oldbanks[isr]->getNRoads();
    for (int iroad=0;iroad<nroads;++iroad) {
      FTKRoad *cur_road = oldbanks[isr]->getRoad(iroad);
      FTKRoad *new_road = newbank->addRoad(*cur_road);
      // the sub-region ID is hidden in this formula sub*100+reg
      new_road->setBankID(/*100*isr*ENCODE_SUBREGION+*/cur_road->getBankID());
    }
    newbank->inc4LRoad(oldbanks[isr]->getN4LRoads());

    // add the SS for all the layers
    const FTKSS_container_t &strips_cont = oldbanks[isr]->getSSContainer();
    int cont_nplanes = strips_cont.size();

    for (int ipl=0;ipl!=cont_nplanes;++ipl) { // plane loop
      const FTKSS_map_t &ssmap = strips_cont[ipl];

      FTKSS_map_t::const_iterator iss = ssmap.begin();
      for (;iss!=ssmap.end();++iss) { // SS loop
        newbank->addSS(ipl,(*iss).first,(*iss).second);
      } // end SS loop
    } // end plane loop

    // add te SS strips in the layers ignored in the RF step
    const map< int, map<int,FTKSS> > &unusedStrips_cont = oldbanks[isr]->getUnusedSSMap();
    map< int, map<int,FTKSS> >::const_iterator uSS_iplane = unusedStrips_cont.begin();
    for (;uSS_iplane!=unusedStrips_cont.end();++uSS_iplane) { // unused plane loop
      const int &plane_id = (*uSS_iplane).first;
      map<int,FTKSS>::const_iterator uSS_iss = (*uSS_iplane).second.begin();
      for (;uSS_iss!=(*uSS_iplane).second.end();++uSS_iss) { // SS loop
        newbank->addUnusedSS(plane_id,(*uSS_iss).first,(*uSS_iss).second);
      } // end SS loop
    } // end unused planes loop
  } // end subregions loop
}
