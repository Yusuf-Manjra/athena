/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

/*
 * FPGATrackSimRoad_test.cxx: Unit tests for FPGATrackSimRoad and FPGATrackSimHit
 */

#include <cstdio>
#include <iostream>
#include <cassert>
#include "FPGATrackSimObjects/FPGATrackSimRoad.h"

int main(int, char**)
{

  const int roadid(8675309);

  std::vector<std::vector<const FPGATrackSimHit*>> hitVec;
  FPGATrackSimHit hits[8];
  for (unsigned i = 0; i < 8; i++) {
    hits[i].setPhiIndex(i*i);
    hits[i].setEtaIndex(i+1);
    hits[i].setLayer(i);
    hits[i].setHitType(HitType::mapped);
    const FPGATrackSimHit* hitP = &hits[i];
    std::vector<const FPGATrackSimHit*> hitPVec;
    hitPVec.push_back(hitP);
    hitVec.push_back(hitPVec);
  }
  
  FPGATrackSimRoad road;
  road.setHits(hitVec);
  road.setRoadID(roadid);
  std::cout << "road id = " << road.getRoadID() << std::endl;
  for (unsigned i = 0; i < 8; i++) {
    std::vector<const FPGATrackSimHit*> returned_hits = road.getHits(i);
    std::cout << "layer = " << returned_hits[0]->getLayer() << " and phi = " << returned_hits[0]->getPhiIndex() << 
      " and eta = " << returned_hits[0]->getEtaIndex() << std::endl;
  }

  return 0;

}

