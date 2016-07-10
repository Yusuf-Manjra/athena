/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PFMatcher.h
 *
 *  Created on: 03.04.2014
 *      Author: tlodd
 */

#ifndef PFMATCHER_H_
#define PFMATCHER_H_

#include "eflowRec/PFMatchInterfaces.h"

#include <cmath>

class DistanceProvider;

namespace PFMatch {

struct MatchDistance {
  MatchDistance(const ICluster* cluster, double distance, bool isMatch):
    _cluster(cluster), _distance(distance), _isMatch(isMatch) { }
  const ICluster* _cluster;
  double _distance;
  bool _isMatch;
};

class TrackClusterMatcher {
public:
  TrackClusterMatcher( IDistanceProvider* distanceProvider, double matchCut):
      _distanceProvider(distanceProvider), _matchCut(matchCut) { setDRParameters(); }

  virtual ~TrackClusterMatcher();

  MatchDistance match(const ITrack* track, const ICluster* cluster);

  template<class ClusterType>
  MatchDistance bestMatch(ITrack* track, const std::vector<ClusterType*>& clusters);
  template<class ClusterType>
  MatchDistance bestMatchDRparametrized(ITrack* track, const std::vector<ClusterType*>& clusters);
  template<class ClusterType>
  std::vector<MatchDistance> allMatches(ITrack* track, const std::vector<ClusterType*>& clusters);
  template<class ClusterType>
  double getDRCutSquared(ClusterType* theCluster);
private:
  IDistanceProvider* _distanceProvider;
  double _matchCut;
   /** This stores the parameters to vary DR cut with Pt */
  float m_drcut_par[8][3];

  /** This sets the parameters in the above array */
  void setDRParameters();


};

template<class ClusterType>
MatchDistance TrackClusterMatcher::bestMatch(ITrack* track, const std::vector<ClusterType*>& clusters) {
  ClusterType* bestCluster(0);
  double bestDistance(_matchCut);

  unsigned int nClusters(clusters.size());
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster){

    ClusterType* thisCluster = clusters[iCluster];
    double thisDistance(_distanceProvider->distanceBetween(track, clusters[iCluster]));

    if (thisDistance < bestDistance) {
      bestDistance = thisDistance;
      bestCluster  = thisCluster;
    }
  }
  return MatchDistance(bestCluster, bestDistance, bestCluster != 0);
}

 template<class ClusterType>
   MatchDistance TrackClusterMatcher::bestMatchDRparametrized(ITrack* track, const std::vector<ClusterType*>& clusters) {
   ClusterType* bestCluster(0);
   double bestDistance(_matchCut);
   //double mybestdistance(_matchCut);
   unsigned int nClusters(clusters.size());
   for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster){
     
     ClusterType* thisCluster = clusters[iCluster];
     double thisDistance(_distanceProvider->distanceBetween(track, clusters[iCluster]));
     double  mybestdistance = getDRCutSquared(thisCluster);
     
     if (thisDistance < mybestdistance){
       if (thisDistance < bestDistance) {
	 bestDistance = thisDistance;
	 bestCluster  = thisCluster;
       }
     } 	
   }
   
   return MatchDistance(bestCluster, bestDistance, bestCluster != 0);
 }
 template<class ClusterType>
std::vector<MatchDistance> TrackClusterMatcher::allMatches(ITrack* track, const std::vector<ClusterType*>& clusters) {
  std::vector<MatchDistance> result;

  unsigned int nClusters(clusters.size());
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster){

    ClusterType* thisCluster = clusters[iCluster];
    double thisDistance(_distanceProvider->distanceBetween(track, thisCluster));

    if (thisDistance < _matchCut) {
      result.push_back(MatchDistance(thisCluster, thisDistance, true));
    }
  }

  return result;
 }

template<class ClusterType>
 double TrackClusterMatcher::getDRCutSquared( ClusterType* theCluster) {
    
    double m_coneRSq = 1.64*1.64;
    double coneRSq = m_coneRSq;
  int ieta = -1;
  double clusEta;
  
  clusEta = theCluster->eta();

  if (abs(clusEta)<0.6) ieta=0;
  if (abs(clusEta)>=0.6 && abs(clusEta)<1.6) ieta = 1 + int((abs(clusEta) - 0.6)/0.2) ;
  if (abs(clusEta)>=1.6 && abs(clusEta)<2.0) ieta = 6 ;
  if (abs(clusEta)>=2.0 && abs(clusEta)<2.5) ieta = 7 ;
  if (abs(clusEta)>=2.5) ieta = 7 ; 

  double clusterEnergy = theCluster->e()/1000.0;

  //  std::cout<<"cluene "<<clusterEnergy<<std::endl;
  double drcut = m_drcut_par[ieta][0]+m_drcut_par[ieta][1]*exp(m_drcut_par[ieta][2]*clusterEnergy);
  //  std::cout<<"drparam "<<m_drcut_par[ieta][0]<<" "<<m_drcut_par[ieta][1]<<" "<<m_drcut_par[ieta][2]<<std::endl;
  coneRSq = drcut*drcut;

 if (coneRSq>m_coneRSq) coneRSq = m_coneRSq;
  double conemin = m_drcut_par[ieta][0]+m_drcut_par[ieta][1]*exp(m_drcut_par[ieta][2]*10);
  
  if (coneRSq<conemin*conemin) coneRSq = conemin*conemin; 
  return coneRSq;

  }

}

#endif /* PFMATCHER_H_ */
