/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_TRIGT1CALOCALIBCONDITIONSDICT_H
#define TRIGT1CALOCALIBCONDITIONS_TRIGT1CALOCALIBCONDITIONSDICT_H

#include "TrigT1CaloCalibConditions/L1CaloPedestalContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprLutContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloRampDataContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloEnergyScanResultsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloEnergyScanRunInfoContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditions.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsContainerRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPprConditionsRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannelContainerRun2.h"
#include "TrigT1CaloCalibConditions/L1CaloPprDisabledChannel.h"

struct TrigT1CaloCalibConditions_DUMMY_Instantiation {
  // we need to instantiate templated objects to get them into the dict
  std::map<unsigned int, L1CaloRampData> L1CaloRampDataMap;
  std::pair<L1CaloRampStep, L1CaloRampStep> L1CaloRampDataPoint;
  std::vector<std::pair<L1CaloRampStep, L1CaloRampStep> > L1CaloRampDataVector;
  std::pair<const unsigned int, L1CaloRampData> L1CaloRampDataMapValueType;
  std::map<unsigned int, L1CaloRampData>::const_iterator L1CaloRampDataMapConstIterator;
  L1CaloEnergyScanResultsContainer::const_iterator L1CaloEnergyScanResultsMapConstInterator;
  std::pair<const unsigned int, L1CaloEnergyScanResults> L1CaloEnergyScanResultsMapValueType;
  std::map<unsigned int, L1CaloPprDisabledChannel> L1CaloPprDisabledChannelMap;
  std::map<unsigned int, L1CaloPprDisabledChannel>::const_iterator L1CaloPprDisabledChannelMapConstIterator;
};

#endif
