/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATLCLUSTERINGLNF_H
#define ATLCLUSTERINGLNF_H

/*
 * atlClusteringLNF
 * ---------------
 *
 * Routines to perform clustering in the pixels.
 *
 * 21-11-2007 Alberto Annovi (alberto.annovi@lnf.infn.it)
 *
 */

#include "TrigFTKSim/ftktypes.h"
#include "TrigFTKSim/FTK_RawInput.h"
//#include "TrigFTKSim/atlhit_rd.h"

#define GRID_ROW_MAX 21
#define GRID_COL_MAX 8 
#define hitRowInGrid(gridCntRow, hit) ((hit->getPhiSide() <= gridCntRow + GRID_ROW_MAX/2) && \
        (hit->getPhiSide() >= gridCntRow - GRID_ROW_MAX / 2))? true : false
#define hitColInGrid(gridStrCol, hit) (hit->getEtaStrip() < gridStrCol + GRID_COL_MAX)? true : false

#ifdef __cplusplus
#include <vector>
#include <set>
#include <map>
class FTKRawHit_cmp {
 public:
  bool operator() (const FTKRawHit *hit1, const FTKRawHit *hit2) const;
};
typedef std::vector<FTKRawHit*> hitVector;
typedef std::map<unsigned int, hitVector> hitsByModuleMap;
class cluster {
 public:
  hitVector hitlist; // list of hits that make the cluster
  FTKRawHit clusterEquiv; // hit equivalent to the cluster
  FTKRawHit* seed; // hit equivalent to the cluster
  bool isSafe; // confirmed by ganged pattern recognition 
  bool isKilled; // ganged pattern recognition suggest to remove this cluster

  ~cluster();
};
typedef std::vector<cluster> cluList;
typedef std::map<unsigned int, cluList*> clustersByModuleMap;
int hitToModuleId(const FTKRawHit &hit);

extern bool SAVE_CLUSTER_CONTENT;
extern bool DIAG_CLUSTERING;
extern int PIXEL_CLUSTERING_MODE;
const int PIXEL_CLUSTERING_MODE_APRIL2014_FIX=2;
const int REALISTIC_CLUSTERING_MODE=100;
extern bool DUPLICATE_GANGED;
extern bool GANGED_PATTERN_RECOGNITION;
extern bool SPLIT_BLAYER_MODULES;
extern bool SCT_CLUSTERING;

extern "C" {
#endif
  void atlClusteringBlayer(std::vector<FTKRawHit> &);
  void atlClusteringLNF(std::vector<FTKRawHit> &);
  //  void atlClusteringTrack(atltrack *track);
  int moduleIdToLayer(int modId);
  int moduleIdToIsPixel(int modId);
  int moduleIdIsEndcap(int modId);
  double eta(const FTKRawHit &);
  int hitIsGanged(const FTKRawHit &);
  int gangedPartner(const FTKRawHit &);

#ifdef __cplusplus
}
#endif

#endif // ATLCLUSTERINGLNF_H
