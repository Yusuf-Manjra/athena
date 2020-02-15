/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      QuickCloseComponentsMultiStateMerger.cxx  -  description
      ---------------------------------------------------
begin                : Wednesday 3rd September 2008
author               : amorley
email                : Anthony.Morley@cern.ch
decription           : Implementation code for QuickCloseComponentsMultiStateMerger
                       class
*********************************************************************************/

#include "TrkGaussianSumFilter/QuickCloseComponentsMultiStateMerger.h"
#include "GaudiKernel/Chrono.h"
#include "TrkGaussianSumFilter/AllignedDynArray.h"
#include "TrkGaussianSumFilter/KLGaussianMixtureReduction.h"
#include "TrkGaussianSumFilter/MultiComponentStateCombiner.h"
#include "TrkParameters/TrackParameters.h"
#include <cstdint>
#include <limits>
using namespace GSFUtils;

Trk::QuickCloseComponentsMultiStateMerger::QuickCloseComponentsMultiStateMerger(const std::string& type,
                                                                                const std::string& name,
                                                                                const IInterface* parent)
  : AthAlgTool(type, name, parent)
  , m_chronoSvc("ChronoStatSvc", name)
{

  declareInterface<IMultiComponentStateMerger>(this);
}

Trk::QuickCloseComponentsMultiStateMerger::~QuickCloseComponentsMultiStateMerger() = default;

StatusCode
Trk::QuickCloseComponentsMultiStateMerger::initialize()
{

  // Request the Chrono Service
  if (m_chronoSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve service " << m_chronoSvc);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved service " << m_chronoSvc);

  if (m_maximumNumberOfComponents <= 0) {
    ATH_MSG_FATAL("Attempting to merge multi-state into zero components... stop being silly!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Initialisation of " << type() << " under instance " << name() << " was successful");
  return StatusCode::SUCCESS;
}

StatusCode
Trk::QuickCloseComponentsMultiStateMerger::finalize()
{
  ATH_MSG_INFO("Finalisation of " << type() << " under instance " << name() << " was successful");
  return StatusCode::SUCCESS;
}

std::unique_ptr<Trk::MultiComponentState>
Trk::QuickCloseComponentsMultiStateMerger::merge(Trk::MultiComponentState statesToMerge) const
{
  // Assembler Cache
  MultiComponentStateAssembler::Cache cache;

  if (statesToMerge.size() <= m_maximumNumberOfComponents) {
    MultiComponentStateAssembler::addMultiState(cache, std::move(statesToMerge));
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
              [](const ComponentParameters& x, const ComponentParameters& y) { return x.second > y.second; });

    Trk::ComponentParameters dummyCompParams(statesToMerge.begin()->first->clone(), 1.);
    auto returnMultiState = std::make_unique<Trk::MultiComponentState>();
    returnMultiState->push_back(std::move(dummyCompParams));
    return returnMultiState;
  }

  return mergeFullDistArray(cache, statesToMerge);
}

std::unique_ptr<Trk::MultiComponentState>
Trk::QuickCloseComponentsMultiStateMerger::mergeFullDistArray(MultiComponentStateAssembler::Cache& cache,
                                                              Trk::MultiComponentState& statesToMerge) const
{
  /*
   * Allocate, and initialize the needed arrays
   */
  const int32_t n = statesToMerge.size();
  const int32_t nn = (n + 1) * n / 2;
  const int32_t nn2 = (nn & 7) == 0 ? nn : nn + (8 - (nn & 7)); // make sure it is a multiplet of 8

  AlignedDynArray<float, alignment> distances(nn2); // Array to store all of the distances between components
  AlignedDynArray<float, alignment> qonp(n);        // Array of qonp for each component
  AlignedDynArray<float, alignment> qonpCov(n);     // Array of Cov(qonp,qonp) for each component
  AlignedDynArray<float, alignment> qonpG(n);       // Array of 1/Cov(qonp,qonp) for each component

  // Initialize all values
  for (int32_t i = 0; i < n; ++i) {
    qonp[i] = 0;
    qonpCov[i] = 0;
    qonpG[i] = 1e10;
  }
  for (int32_t i = 0; i < nn2; ++i) {
    distances[i] = std::numeric_limits<float>::max();
  }

  // Needed to convert the triangular index to (i,j)
  std::vector<triangularToIJ> convert(nn2, { -1, -1 });
  // Calculate indicies
  for (int32_t i = 0; i < n; ++i) {
    const int indexConst = (i + 1) * i / 2;
    for (int32_t j = 0; j <= i; ++j) {
      int32_t index = indexConst + j;
      convert[index].I = i;
      convert[index].J = j;
    }
  }

  // Create an array of all components to be merged
  for (int32_t i = 0; i < n; ++i) {
    const AmgSymMatrix(5)* measuredCov = statesToMerge[i].first->covariance();
    const AmgVector(5) parameters = statesToMerge[i].first->parameters();
    // Fill in infomation
    qonp[i] = parameters[Trk::qOverP];
    qonpCov[i] = measuredCov ? (*measuredCov)(Trk::qOverP, Trk::qOverP) : -1.;
    qonpG[i] = qonpCov[i] > 0 ? 1. / qonpCov[i] : 1e10;
  }
  // Calculate distances for all pairs
  // This loop can be vectorised
  calculateAllDistances(qonp, qonpCov, qonpG, distances, n);

  /*
   *  Loop over all components until you reach the target amount
   */
  unsigned int numberOfComponents = n;
  int32_t minIndex = -1;
  int32_t nextMinIndex = -1;

  while (numberOfComponents > m_maximumNumberOfComponents) {

    /* 
     * Find the minimum index 
     * Do it only if we do not have a good new guess
     */
    if(nextMinIndex<1){
      minIndex = findMinimumIndex(distances, nn2);
    } else {
      minIndex = nextMinIndex;
    }
    /* reset the nextMinindex*/
    nextMinIndex=-1;
    /*Keep track of the current minimum value*/
    float currentMinValue=distances[minIndex];

    /*convert the index in an (i,j) pair*/
    int32_t mini = convert[minIndex].I;
    int32_t minj = convert[minIndex].J;
    if (mini == minj) {
      ATH_MSG_ERROR("Err keys are equal key1 " << mini << " key2 " << minj);
      break;
    }
    /*
     * Combine the components to be merged
     * statesToMerge[mini] becomes the merged
     * statesToMerge[minj] is set to dummy values
     */
    MultiComponentStateCombiner::combineWithWeight(statesToMerge[mini], statesToMerge[minj]);
    statesToMerge[minj].first.reset();
    statesToMerge[minj].second = 0.;
    /*
     * set relevant distances
     */
    const AmgSymMatrix(5)* measuredCov = statesToMerge[mini].first->covariance();
    const AmgVector(5)& parameters = statesToMerge[mini].first->parameters();
    qonp[mini] = parameters[Trk::qOverP];
    qonpCov[mini] = (*measuredCov)(Trk::qOverP, Trk::qOverP);
    qonpG[mini] = qonpCov[mini] > 0 ? 1. / qonpCov[mini] : 1e10;
    qonp[minj] = 0.;
    qonpCov[minj] = 0.;
    qonpG[minj] = 1e10;

    // Reset old weights wrt to the new component that is not at positioned in mini
    resetDistances(distances, mini, n);
    // re-calculate distances wrt the new component 
    int32_t possibleNextMin = recalculateDistances(qonp, qonpCov, qonpG, distances, mini, n);
    //We might already got something smaller than the previous minimum
    //we can therefore use the new one directly
    if (possibleNextMin > 0 && distances[possibleNextMin] < currentMinValue) {
      nextMinIndex = possibleNextMin;
    }
    // Reset old weights wrt the  minj position
    resetDistances(distances, minj, n);
    // Decrement the number of components
    --numberOfComponents;
  } // end of merge while

  // Build final state containing both merged and unmerged components
  for (auto& state : statesToMerge) {
    // Avoid merge ones
    if (!state.first) {
      continue;
    }
    /*
     * Add componets to the state be prepared for assembly
     * and update the relevant weight
     */
    cache.multiComponentState.push_back(ComponentParameters(state.first.release(), state.second));
    cache.validWeightSum += state.second;
  }
  std::unique_ptr<Trk::MultiComponentState> mergedState = MultiComponentStateAssembler::assembledState(cache);
  // Clear the state vector
  statesToMerge.clear();
  
  return mergedState;
}
