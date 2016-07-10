/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKTRUTHSELECTOR_H
#define TRACKTRUTHSELECTOR_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthData/TrackTruthCollection.h"

class TrackTruthSelector: public AthAlgorithm {
public:
  TrackTruthSelector(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
private:
  // DetailedTrackTruthCollection input name
  std::string m_detailedTrackTruthName;

  // TrackTruthCollection output name
  std::string m_outputName;

  // Subdetector weights
  std::vector<double> m_subDetWeights;

  void fillOutput(TrackTruthCollection *out, const DetailedTrackTruthCollection *in);
  double getProbability(const DetailedTrackTruth& dt) const;
};

#endif/*TRACKTRUTHSELECTOR_H*/
