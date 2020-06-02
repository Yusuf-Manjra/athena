/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_nTracks.cxx
 * @author shaun roe
 **/

#include "InDetPerfPlot_nTracks.h"


InDetPerfPlot_nTracks::InDetPerfPlot_nTracks(InDetPlotBase* pParent, const std::string& sDir) : InDetPlotBase(pParent,
                                                                                                              sDir),
  m_counters{0} {
  // nop
}

void
InDetPerfPlot_nTracks::initializePlots() {
  const bool prependDirectory(false);
  SingleHistogramDefinition hd = retrieveDefinition("ntrack");

  m_counters[ALLRECO] = Book1D(hd.name, hd.allTitles, hd.nBinsX, hd.xAxis.first, hd.xAxis.second, prependDirectory);
  hd = retrieveDefinition("ntracksel");
  m_counters[SELECTEDRECO] = Book1D(hd.name, hd.allTitles, hd.nBinsX, hd.xAxis.first, hd.xAxis.second, prependDirectory);
  hd = retrieveDefinition("nparticle");
  m_counters[ALLTRUTH] = Book1D(hd.name, hd.allTitles, hd.nBinsX, hd.xAxis.first, hd.xAxis.second, prependDirectory);
  hd = retrieveDefinition("num_truthmatch_match");
  m_counters[MATCHEDRECO] =
    Book1D(hd.name, hd.allTitles, hd.nBinsX, hd.xAxis.first, hd.xAxis.second, prependDirectory);
}

void
InDetPerfPlot_nTracks::fill(const unsigned int freq, const CounterCategory counter) {
  if (counter < N_COUNTERS) {
    fillHisto((m_counters[counter]), freq);
  }
}
