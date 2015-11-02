/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_RDO_CreatorAlgo_h
#define FTK_RDO_CreatorAlgo_h

#include "PileUpTools/PileUpMergeSvc.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include "TChain.h"

#include "TrigFTKSim/FTKTrackStream.h"
#include "TrigFTKSim/FTKPMap.h"

#include "TrigFTKPool/FTKAthTrack.h" 

#include "TrigFTK_RawData/FTK_RawTrack.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"

#include <vector>
#include <string>
#include <map>


/////////////////////////////////////////////////////////////////////////////
class FTK_RDO_CreatorAlgo: public AthAlgorithm {
public:
  FTK_RDO_CreatorAlgo (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FTK_RDO_CreatorAlgo ();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  StoreGateSvc* m_StoreGate;
  StoreGateSvc* m_detStore;

  PileUpMergeSvc *m_mergeSvc; // Pile up service, used to merge sub-regions output

  // path for the plane-map information
  std::string m_pmap_path;
  FTKPlaneMap *m_pmap;

  bool m_useStandalone; // use input from standalone version instead of Athena
  bool m_singleProces; // assume the merger runs in a single process, after the trackfitter
  bool m_doMerging; // if "true" input is expected from regions/sub-regions and has to be merged

  unsigned int m_nregions; // number of regions
  unsigned int m_nsubregions; // number of sub-regions (in each region)
  unsigned int m_neventsUnmergedFiles;// number of events in unmerged files 
  unsigned int m_neventsMerged;// number of events already unmerged 

  //
  //  Merged output
  //
  TTree *m_outputTree;
  TFile *m_outputFile;
  FTKTrackStream *m_merged_bank;
  
  //HW parameter 
  int m_HW_level; // hit-warrior level
  unsigned int m_HW_ndiff; // maximum number of different points
  int m_NCoords; // NCoords
  float *m_HW_dev; //HW
  std::string m_HW_path; // HW configuration path
  int m_keep_rejected; // >0 keep rejected roads (1 HW rej, 2 bad quality)

  int m_SCTCoordOffset; // offset for the SCT pixel coordinates

  // vectors with information for the merging
  // paths with the merged tracks
  std::vector< std::string > m_ftktrack_tomerge_paths;
  int m_ftktrack_tomerge_firstregion;
  int m_ftktrack_tomerge_firstsubreg;
  std::string m_ftrtrack_tomerge_bfname;
  int m_MergeRegion;
  unsigned int m_nStreamsToMerge;
  TTree ***m_ftktrack_tomerge_tree;  
  TFile ***m_ftktrack_tomerge_file;
  TBranch ***m_ftktrack_tomerge_branch;
  FTKTrackStream ***m_ftktrack_tomerge_stream;  
  std::string m_ftktrack_mergeoutput;
  std::string m_ftktrack_mergeInputPath;
  std::string m_ftktrack_mergeFileRoot;

  //
  //  Require all inputs files to be present
  //    if missing one:
  //       - fail if true
  //       - print warning continue if false
  bool m_forceAllInput;

  //
  //   Use naming as done on the grid 
  //
  bool m_doGrid;

  // paths with the merged tracks
  std::vector< std::string > m_ftktrack_paths_merged;
  // chain  of ftk merged tracks
  TChain *m_mergedtracks_chain;
  TTree *m_mergedtracks_tree;
  // streams with the merged roads
  FTKTrackStream *m_mergedtracks_stream;
  std::string m_mergedtracks_bname; // name to be used for the merged tracks' branch name

  // merged roads counter
  unsigned long m_mergedtracks_iev;

  StatusCode convertMergedTracks();

  StatusCode mergeSGContent();

  StatusCode mergePoolFiles();

  StatusCode initStandaloneTracks();
  StatusCode mergeStandaloneTracks();
  StatusCode finalizeStandaloneTracks();

  //remove track function
  std::list<FTKAthTrack>::iterator removeAthTrack(std::list<FTKAthTrack>&, std::list<FTKAthTrack>::iterator, FTKAthTrack&, const FTKAthTrack&,bool isnew=false);

  // merging functions for the standalong tracks 
  int m_nfits_rej;
  void merge_tracks(FTKTrackStream *&, FTKTrackStream ***);
  std::list<FTKTrack>::iterator removeFTKTrack(std::list<FTKTrack>&, std::list<FTKTrack>::iterator, FTKTrack&, const FTKTrack&,bool isnew=false);

  //
  // Format the input file names
  //
  std::string GetPathName(unsigned int ireg, unsigned int isub);

  // disable the event synchronization and other checks
  bool m_force_merge;




  bool m_GenerateRDO; // when true the raw track collection is generated during the merge
  std::string m_ftk_raw_trackcollection_Name;
  FTK_RawTrackContainer *m_ftk_raw_trackcollection;


  // map containing for each event the entry where the tracks 
  // found by the FTK (the event number is used as a key to 
  // address differences in the event sequence)
  std::map< int, Long64_t > m_trackVectorMap;


  // Convertor for RDO objects 
  FTK_RawTrack SimToRaw(FTKAthTrack);
  FTK_RawTrack* SimToRaw(const FTKTrack&);
  void printBits(unsigned int num, unsigned int length);
};

#endif // FTK_RDO_CreatorAlgo_h
