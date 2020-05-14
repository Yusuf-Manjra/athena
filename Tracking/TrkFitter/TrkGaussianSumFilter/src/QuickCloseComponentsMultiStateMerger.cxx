/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      QuickCloseComponentsMultiStateMerger.cxx  -  description
      ---------------------------------------------------
begin                : Wednesday 3rd September 2008
author               : amorley
email                : Anthony.Morley@cern.ch
decription           : Implementation code for
QuickCloseComponentsMultiStateMerger class
*********************************************************************************/

#include "TrkGaussianSumFilter/QuickCloseComponentsMultiStateMerger.h"
#include "TrkGaussianSumFilter/AlignedDynArray.h"
#include "TrkGaussianSumFilter/KLGaussianMixtureReduction.h"
#include "TrkGaussianSumFilter/MultiComponentStateCombiner.h"
#include "TrkParameters/TrackParameters.h"
#include <cstdint>
#include <limits>

using namespace GSFUtils;

Trk::MultiComponentState
Trk::QuickCloseComponentsMultiStateMerger::merge(
  Trk::MultiComponentState&& statesToMerge,
  const unsigned int maximumNumberOfComponents)
{
  // Assembler Cache
  MultiComponentStateAssembler::Cache cache;
  if (statesToMerge.size() <= maximumNumberOfComponents) {
    MultiComponentStateAssembler::addMultiState(cache,
                                                std::move(statesToMerge));
    return MultiComponentStateAssembler::assembledState(cache);
  }

  // Scan all components for covariance matrices. If one or more component
  // is missing an error matrix, component reduction is impossible.
  bool componentWithoutMeasurement = false;
  Trk::MultiComponentState::const_iterator component = statesToMerge.cbegin();
  for (; component != statesToMerge.cend(); ++component) {
    const AmgSymMatrix(5)* measuredCov = component->first->covariance();
    if (!measuredCov) {
      componentWithoutMeasurement = true;
      break;
    }
  }
  if (componentWithoutMeasurement) {
    // Sort to select the one with the largest weight
    std::sort(statesToMerge.begin(),
              statesToMerge.end(),
              [](const ComponentParameters& x, const ComponentParameters& y) {
                return x.second > y.second;
              });

    Trk::ComponentParameters dummyCompParams(
      statesToMerge.begin()->first->clone(), 1.);
    Trk::MultiComponentState returnMultiState;
    returnMultiState.push_back(std::move(dummyCompParams));
    return returnMultiState;
  }

  return mergeFullDistArray(cache, statesToMerge, maximumNumberOfComponents);
}

Trk::MultiComponentState
Trk::QuickCloseComponentsMultiStateMerger::mergeFullDistArray(
  MultiComponentStateAssembler::Cache& cache,
  Trk::MultiComponentState& statesToMerge,
  const unsigned int maximumNumberOfComponents)
{
  const int32_t n = statesToMerge.size();
  AlignedDynArray<Component1D, alignment> components(n);
  for (int32_t i = 0; i < n; ++i) {
    const AmgSymMatrix(5)* measuredCov = statesToMerge[i].first->covariance();
    const AmgVector(5)& parameters = statesToMerge[i].first->parameters();
    // Fill in infomation
    const double cov =
      measuredCov ? (*measuredCov)(Trk::qOverP, Trk::qOverP) : -1.;
    components[i].mean = parameters[Trk::qOverP];
    components[i].cov = cov;
    components[i].invCov = cov > 0 ? 1. / cov : 1e10;
    components[i].weight = statesToMerge[i].second;
  }

  // Gather the merges
  const std::vector<std::pair<int32_t, int32_t>> merges =
    findMerges(components, n, maximumNumberOfComponents);

  // Do the full 5D calculations of the merge
  for (const auto& mergePair : merges) {
    const int32_t mini = mergePair.first;
    const int32_t minj = mergePair.second;
    MultiComponentStateCombiner::combineWithWeight(statesToMerge[mini],
                                                   statesToMerge[minj]);
    statesToMerge[minj].first.reset();
    statesToMerge[minj].second = 0.;
  }
  // Assemble the final result
  for (auto& state : statesToMerge) {
    // Avoid merge ones
    if (!state.first) {
      continue;
    }
    cache.multiComponentState.push_back(
      ComponentParameters(state.first.release(), state.second));
    cache.validWeightSum += state.second;
  }
  Trk::MultiComponentState mergedState =
    MultiComponentStateAssembler::assembledState(cache);
  // Clear the state vector
  statesToMerge.clear();
  return mergedState;
}
