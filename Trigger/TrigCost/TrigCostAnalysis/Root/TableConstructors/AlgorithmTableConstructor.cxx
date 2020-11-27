/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "../../TrigCostAnalysis/TableConstructors/AlgorithmTableConstructor.h"

#include "TH1.h"

AlgorithmTableConstructor::AlgorithmTableConstructor(const std::string& name) : TableConstructorBase(name) {
  addExpectedHistogram("Time_perCall");
  addExpectedHistogram("FirstTime_perEvent");
  addExpectedHistogram("Time_perEvent");
  addExpectedHistogram("Time_perEventFractional");
  addExpectedHistogram("AlgCalls_perEvent");
  addExpectedHistogram("RoIID_perCall");
  addExpectedHistogram("InEventView_perCall");
  addExpectedHistogram("Request_perEvent");
  addExpectedHistogram("NetworkRequest_perEvent");
  addExpectedHistogram("CachedROBSize_perEvent");
  addExpectedHistogram("NetworkROBSize_perEvent");
  addExpectedHistogram("RequestTime_perEvent");


  getBaseEntry().addColumn("name", "Name", "Algorithms name");
  getBaseEntry().addColumn("events", "Raw Active Events", "Raw underlying statistics on number of events processed with the alg active");
  getBaseEntry().addColumn("eventsWeighted", "Active Events", "Total weighted number of events with the alg active");
  getBaseEntry().addColumn("callsPerEvent", "Calls/Event", "Mean number of alg calls in events with one or more calls");
  getBaseEntry().addColumn("callsSlow", "Calls > 1000 ms", "Weighted number of events in which this alg was exceptionally slow");
  getBaseEntry().addColumn("eventRate", "Event Rate [Hz]", "Walltime normalised rate of events with one or more executions of the alg");
  getBaseEntry().addColumn("callRate", "Call Rate [Hz]", "Walltime normalised rate of calls in events with one or more executions of the alg");
  getBaseEntry().addColumn("totalTimeSec", "Alg Total Time [s]", "Total weighted integrated walltime of the alg");
  getBaseEntry().addColumn("totalTimePerc", "Alg Total Time [%]", "Total weighted integrated walltime of the alg as a percentage of all algs");
  getBaseEntry().addColumn("timePerCall", "Alg Total Time/Call [ms]", "Mean weighted alg time. Normalised to all alg calls");
  getBaseEntry().addColumn("timePerEvent", "Alg Total Time/Event [ms]", "Mean weighted alg time. Normalised to all events with one or more alg calls");
  getBaseEntry().addColumn("requestTimePerEvent", "ROS Data Request Time/Event [ms]", "Average time waiting for ROS data per event for  events with at least one execution in this run range");
  getBaseEntry().addColumn("dataRate", "Data Request Rate [Hz]", "Rate of calls to the ROS from this algorithm in this run range");
  getBaseEntry().addColumn("retrievedDataRate", "Retrieved ROB Rate [Hz]", "Rate of ROB retrievals from this algorithm in this run range");
  getBaseEntry().addColumn("cachedDataSizeRate", "Cached ROB Rate [kB/s]", "Average size of cached ROB data fetches for this algorithm in this run range");
  getBaseEntry().addColumn("retrievedDataSizeRate", "Retrieved ROB Rate [kB/s]", "Average size of retrieved ROB data fetches for this algorithm in this run range");
}


TableEntry AlgorithmTableConstructor::getTableEntry(const std::string name) {
  TableEntry tableEntry(getBaseEntry());
  getHistograms(name);

  const float weightedEvents = hist("AlgCalls_perEvent")->Integral(); // Filled once per event with event weight
  const float weightedCalls = histGetXWeightedIntegral("AlgCalls_perEvent", /*isLog*/ false);

  tableEntry.setEntry("name", name);
  tableEntry.setEntry("events", hist("AlgCalls_perEvent")->GetEntries());
  tableEntry.setEntry("eventsWeighted", weightedEvents);
  tableEntry.setEntry("callsPerEvent", hist("AlgCalls_perEvent")->GetMean());
  tableEntry.setEntry("callsSlow", hist("Time_perCall")->Integral( hist("Time_perCall")->FindBin(1000.), hist("Time_perCall")->GetNbinsX()));
  tableEntry.setEntry("eventRate", weightedEvents); // Needs normalising in tablePostProcessing
  tableEntry.setEntry("callRate", weightedCalls); // Needs normalising in tablePostProcessing
  tableEntry.setEntry("totalTimeSec", histGetXWeightedIntegral("Time_perCall", /*isLog*/ true) * 1e-3);
  //"totalTimePerc" is set in post
  tableEntry.setEntry("timePerCall", hist("Time_perCall")->GetMean());
  tableEntry.setEntry("timePerEvent", hist("Time_perEvent")->GetMean());
  tableEntry.setEntry("requestTimePerEvent", hist("RequestTime_perEvent")->GetMean());
  tableEntry.setEntry("dataRate", histGetXWeightedIntegral("Request_perEvent", /*isLog*/ false)); // Needs normalising in tablePostProcessing
  tableEntry.setEntry("retrievedDataRate", histGetXWeightedIntegral("NetworkRequest_perEvent", /*isLog*/ false)); // Needs normalising in tablePostProcessing
  tableEntry.setEntry("retrievedDataSizeRate", histGetXWeightedIntegral("NetworkROBSize_perEvent", /*isLog*/ false)); // Needs normalising in tablePostProcessing
  tableEntry.setEntry("cachedDataSizeRate", histGetXWeightedIntegral("CachedROBSize_perEvent", /*isLog*/ false)); // Needs normalising in tablePostProcessing

  return tableEntry;
}

void AlgorithmTableConstructor::tablePostProcessing(std::vector<TableEntry>& tableEntries, const float walltime) {
  // Get total time
  float totalTime = 0;
  for (const TableEntry& te : tableEntries) {
    totalTime += te.getEntryFloat("totalTimeSec");
  }

  // Set derived
  for (TableEntry& te : tableEntries) {
    te.setEntry("totalTimePerc", 100.0 * (te.getEntryFloat("totalTimeSec") / totalTime));
    te.normaliseEntry("eventRate", walltime);
    te.normaliseEntry("callRate", walltime);
    te.normaliseEntry("dataRate", walltime);
    te.normaliseEntry("retrievedDataRate", walltime);
    te.normaliseEntry("cachedDataSizeRate", walltime);
    te.normaliseEntry("retrievedDataSizeRate", walltime);
  }
}

