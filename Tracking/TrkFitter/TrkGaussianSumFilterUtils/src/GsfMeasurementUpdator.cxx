/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file   GsfMeasurementUpdator.cxx
 * @date   Friday 25th February 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief  Implementation code for GsfMeasurementUpdator class
 */

#include "TrkGaussianSumFilterUtils/GsfMeasurementUpdator.h"
#include "TrkGaussianSumFilterUtils/MultiComponentStateAssembler.h"
#include "TrkGaussianSumFilterUtils/PosteriorWeightsCalculator.h"
//
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include <memory>

namespace {
bool
invalidComponent(const Trk::TrackParameters* trackParameters)
{
  const auto* measuredCov = trackParameters->covariance();
  bool rebuildCov = false;
  if (!measuredCov) {
    rebuildCov = true;
  } else {
    for (int i(0); i < 5; ++i) {
      if ((*measuredCov)(i, i) <= 0.) {
        rebuildCov = true;
      }
    }
  }
  return rebuildCov;
}

Trk::MultiComponentState
rebuildState(Trk::MultiComponentState&& stateBeforeUpdate)
{
  Trk::MultiComponentState stateWithInsertedErrors =
    std::move(stateBeforeUpdate);
  // We need to loop checking for invalid componets i.e negative covariance
  // diagonal elements and update them with a large covariance matrix
  for (auto& component : stateWithInsertedErrors) {
    const Trk::TrackParameters* trackParameters = component.first.get();
    const bool rebuildCov = invalidComponent(trackParameters);
    if (rebuildCov) {
      AmgSymMatrix(5) bigNewCovarianceMatrix = AmgSymMatrix(5)::Zero();
      const double covarianceScaler = 1.;
      bigNewCovarianceMatrix(0, 0) = 250. * covarianceScaler;
      bigNewCovarianceMatrix(1, 1) = 250. * covarianceScaler;
      bigNewCovarianceMatrix(2, 2) = 0.25;
      bigNewCovarianceMatrix(3, 3) = 0.25;
      bigNewCovarianceMatrix(4, 4) = 0.001 * 0.001;
      component.first->updateParameters(
        trackParameters->parameters(), bigNewCovarianceMatrix);
    }
  }
  return stateWithInsertedErrors;
}

} // end of anonymous namespace

Trk::MultiComponentState
Trk::GsfMeasurementUpdator::update(
  Trk::MultiComponentState&& stateBeforeUpdate,
  const Trk::MeasurementBase& measurement) const
{
  // Check all components have associated error matricies
  Trk::MultiComponentState::iterator component = stateBeforeUpdate.begin();
  bool rebuildStateWithErrors = false;
  // Perform initial check of state awaiting update. If all states have
  // associated error matricies then no need to perform the rebuild
  for (; component != stateBeforeUpdate.end(); ++component) {
    rebuildStateWithErrors =
      rebuildStateWithErrors || invalidComponent(component->first.get());
  }

  if (rebuildStateWithErrors) {
    Trk::MultiComponentState stateWithInsertedErrors =
      rebuildState(std::move(stateBeforeUpdate));
    // Perform the measurement update with the modified state
    return calculateFilterStep(
      std::move(stateWithInsertedErrors), measurement, 1);
  }

  // Perform the measurement update
  return calculateFilterStep(std::move(stateBeforeUpdate), measurement, 1);
}

Trk::MultiComponentState
Trk::GsfMeasurementUpdator::update(
  Trk::MultiComponentState&& stateBeforeUpdate,
  const Trk::MeasurementBase& measurement,
  FitQualityOnSurface& fitQoS) const
{

  // Check all components have associated error matricies
  Trk::MultiComponentState::iterator component = stateBeforeUpdate.begin();

  bool rebuildStateWithErrors = false;

  // Perform initial check of state awaiting update. If all states have
  // associated error matricies then no need to perform the rebuild
  for (; component != stateBeforeUpdate.end(); ++component) {
    rebuildStateWithErrors =
      rebuildStateWithErrors || invalidComponent(component->first.get());
  }

  if (rebuildStateWithErrors) {
    Trk::MultiComponentState stateWithInsertedErrors =
      rebuildState(std::move(stateBeforeUpdate));
    // Perform the measurement update with the modified state

    Trk::MultiComponentState updatedState = calculateFilterStep(
      std::move(stateWithInsertedErrors), measurement, fitQoS);
    if (updatedState.empty()) {
      return {};
    }
    return updatedState;
  }

  // Perform the measurement update
  Trk::MultiComponentState updatedState =
    calculateFilterStep(std::move(stateBeforeUpdate), measurement, fitQoS);

  if (updatedState.empty()) {
    return {};
  }
  return updatedState;
}

Trk::FitQualityOnSurface
Trk::GsfMeasurementUpdator::fitQuality(
  const MultiComponentState& updatedState,
  const MeasurementBase& measurement) const
{
  double chi2 = 0;
  int degreesOfFreedom = 0;
  Trk::MultiComponentState::const_iterator component = updatedState.begin();

  for (; component != updatedState.end(); ++component) {
    const Trk::TrackParameters* trackParameters = component->first.get();
    Trk::FitQualityOnSurface componentFitQuality;
    m_updator.stateFitQuality(
      componentFitQuality,
      *trackParameters,
      measurement.localParameters(),
      measurement.localCovariance(),
      1);

    double componentChi2 = componentFitQuality.chiSquared();
    chi2 += component->second * componentChi2;
    // The same measurement is included in each update
    if (component == updatedState.begin()) {
      degreesOfFreedom = componentFitQuality.numberDoF();
    }
  }
  return {chi2, degreesOfFreedom};
}

Trk::MultiComponentState
Trk::GsfMeasurementUpdator::calculateFilterStep(
  Trk::MultiComponentState&& stateBeforeUpdate,
  const Trk::MeasurementBase& measurement,
  int addRemoveFlag) const
{
  if (stateBeforeUpdate.empty()) {
    return {};
  }

  // state Assembler cache
  MultiComponentStateAssembler::Cache cache;

  // Calculate the weight of each component after the measurement
  MultiComponentState stateWithNewWeights = PosteriorWeightsCalculator::weights(
    std::move(stateBeforeUpdate), measurement);

  if (stateWithNewWeights.empty()) {
    return {};
  }

  for (Trk::ComponentParameters& component : stateWithNewWeights) {

    Trk::FitQualityOnSurface fitQuality{};
    /// Update the component in place
    bool updateSuccess = m_updator.filterStep(
      *(component.first),
      fitQuality,
      measurement.localParameters(),
      measurement.localCovariance(),
      addRemoveFlag);

    // If we fail we need to erase the element
    if (!updateSuccess || fitQuality.chiSquared() <= 0.) {
      continue;
    }
    // Move component to state being prepared for assembly
    MultiComponentStateAssembler::addComponent(cache, std::move(component));
  }

  Trk::MultiComponentState assembledUpdatedState =
    MultiComponentStateAssembler::assembledState(std::move(cache));

  if (assembledUpdatedState.empty()) {
    return {};
  }
  // Renormalise state
  MultiComponentStateHelpers::renormaliseState(assembledUpdatedState);
  return assembledUpdatedState;
}

Trk::MultiComponentState
Trk::GsfMeasurementUpdator::calculateFilterStep(
  Trk::MultiComponentState&& stateBeforeUpdate,
  const Trk::MeasurementBase& measurement,
  FitQualityOnSurface& fitQoS) const
{
  // state Assembler cache
  MultiComponentStateAssembler::Cache cache;
  if (stateBeforeUpdate.empty()) {
    return {};
  }

  // Calculate the weight of each component after the measurement
  MultiComponentState stateWithNewWeights = PosteriorWeightsCalculator::weights(
    std::move(stateBeforeUpdate), measurement);

  if (stateWithNewWeights.empty()) {
    return {};
  }

  double chiSquared = 0;
  int degreesOfFreedom = 0;
  for (Trk::ComponentParameters& component : stateWithNewWeights) {
    if (
      stateWithNewWeights.size() > 1 &&
      std::abs(component.first->parameters()[Trk::qOverP]) > 0.033333) {
      continue;
    }
    Trk::FitQualityOnSurface componentFitQuality;
    /// Update the component in place
    bool updateSuccess = m_updator.filterStep(
      *(component.first),
      componentFitQuality,
      measurement.localParameters(),
      measurement.localCovariance(),
      1);
    if (!updateSuccess) {
      continue;
    }

    if (invalidComponent(component.first.get())) {
      continue;
    }

    if (componentFitQuality.chiSquared() <= 0.) {
      continue;
    }

    double componentChi2 = componentFitQuality.chiSquared();
    chiSquared += component.second * componentChi2;

    // The same measurement is included in each update
    // so we can update the degree of freedom only
    if (degreesOfFreedom == 0.0) {
      degreesOfFreedom = componentFitQuality.numberDoF();
    }

    // Add component to state being prepared for assembly
    MultiComponentStateAssembler::addComponent(cache, std::move(component));
  }

  Trk::MultiComponentState assembledUpdatedState =
    MultiComponentStateAssembler::assembledState(std::move(cache));

  if (assembledUpdatedState.empty()) {
    return {};
  }

  fitQoS.setChiSquared(chiSquared);
  fitQoS.setNumberDoF(degreesOfFreedom);
  // Renormalise state
  MultiComponentStateHelpers::renormaliseState(assembledUpdatedState);
  return assembledUpdatedState;
}

