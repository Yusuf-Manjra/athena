/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   MultiComponentStateCombiner.cxx
 * @date   Monday 20th December 2004
 * @author Atkinson,Anthony Morley, Christos Anastopoulos
 *
 * Implementation code for MultiComponentStateCombiner
 */

#include "TrkGaussianSumFilterUtils/MultiComponentStateCombiner.h"
#include "TrkGaussianSumFilterUtils/MultiComponentStateModeCalculator.h"
#include "TrkGaussianSumFilterUtils/QuickCloseComponentsMultiStateMerger.h"
//
#include "CxxUtils/phihelper.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"

namespace {

// Actual implementation method for combining
// a multi component state.
std::unique_ptr<Trk::ComponentParameters>
combineToSingleImpl(const Trk::MultiComponentState* uncombinedState,
                    const bool useMode,
                    const double fractionPDFused)
{
  if (uncombinedState->empty()) {
    return nullptr;
  }

  const Trk::TrackParameters* firstParameters =
    uncombinedState->front().first.get();

  // Check to see if first track parameters are measured or not
  const AmgSymMatrix(5)* firstMeasuredCov = firstParameters->covariance();

  if (uncombinedState->size() == 1) {
    return std::make_unique<Trk::ComponentParameters>(
      uncombinedState->front().first->clone(), uncombinedState->front().second);
  }

  double sumW(0.);
  const int dimension = (uncombinedState->front()).first->parameters().rows();
  if (dimension != 5) {
    return nullptr;
  }

  AmgVector(5) mean;
  mean.setZero();
  AmgSymMatrix(5) covariance;
  AmgSymMatrix(5) covariancePart1;
  covariancePart1.setZero();
  AmgSymMatrix(5) covariancePart2;
  covariancePart2.setZero();

  Trk::MultiComponentState::const_iterator component = uncombinedState->begin();
  double totalWeight(0.);
  for (; component != uncombinedState->end(); ++component) {
    double weight = (*component).second;
    totalWeight += weight;
  }

  component = uncombinedState->begin();

  for (; component != uncombinedState->end(); ++component) {

    const Trk::TrackParameters* trackParameters = (*component).first.get();
    double weight = (*component).second;

    AmgVector(5) parameters = trackParameters->parameters();

    // Ensure that we don't have any problems with the cyclical nature of phi
    // Use first state as reference poin
    // Ensure that we don't have any problems with the cyclical nature of phi
    // Use first state as reference poin
    double deltaPhi =
        (*uncombinedState->begin()).first->parameters()[2] - parameters[2];
    if (deltaPhi > M_PI) {
      parameters[2] += 2 * M_PI;
    } else if (deltaPhi < -M_PI) {
      parameters[2] -= 2 * M_PI;
    }

    sumW += weight;
    mean += weight * parameters;

    // Extract local error matrix: Must make sure track parameters are measured,
    // ie have an associated error matrix.
    const AmgSymMatrix(5)* measuredCov = trackParameters->covariance();

    // Calculate the combined covariance matrix
    // \sigma = \Sum_{m=1}^{M} w_{m}(\sigma_m + (\mu_m-\mu)(\mu_m-\mu)^{T})
    if (measuredCov) {
      // Changed from errorMatrixInMeasurementFrame
      covariancePart1 += weight * (*measuredCov);
      // Loop over all remaining components to find the second part of the
      // covariance
      Trk::MultiComponentState::const_iterator remainingComponentIterator =
        component;

      for (; remainingComponentIterator != uncombinedState->end();
           ++remainingComponentIterator) {

        if (remainingComponentIterator == component) {
          continue;
        }

        AmgVector(5) parameterDifference =
          parameters - ((*remainingComponentIterator).first)->parameters();

        double remainingComponentIteratorWeight =
          (*remainingComponentIterator).second;

        covariancePart2 += weight * remainingComponentIteratorWeight *
                           parameterDifference *
                           parameterDifference.transpose();

      } // end loop over remaining components
    }   // end clause if errors are involved

    if (weight / totalWeight > fractionPDFused) {
      break;
    }
  } // end loop over all components

  mean /= sumW;

  // Ensure that phi is between -pi and pi
  //
  mean[2] = CxxUtils::wrapToPi(mean[2]);

  covariance = covariancePart1 / sumW + covariancePart2 / (sumW * sumW);

  if (useMode && dimension == 5) {

    // Calculate the mode of the q/p distribution
    std::array<double, 10> modes =
      Trk::MultiComponentStateModeCalculator::calculateMode(*uncombinedState);

    //  Replace mean with mode if qOverP mode is not 0
    if (modes[4] != 0) {
      mean[0] = modes[0];
      mean[1] = modes[1];
      mean[2] = modes[2];
      mean[3] = modes[3];
      mean[4] = modes[4];

      if (modes[5 + 0] > 0) {
        double currentErr = sqrt((covariance)(0, 0));
        currentErr = modes[5 + 0] / currentErr;
        (covariance)(0, 0) = modes[5 + 0] * modes[5 + 0];
        covariance.fillSymmetric(1, 0, (covariance)(1, 0) * currentErr);
        covariance.fillSymmetric(2, 0, (covariance)(2, 0) * currentErr);
        covariance.fillSymmetric(3, 0, (covariance)(3, 0) * currentErr);
        covariance.fillSymmetric(4, 0, (covariance)(4, 0) * currentErr);
      }
      if (modes[5 + 1] > 0) {
        double currentErr = sqrt((covariance)(1, 1));
        currentErr = modes[5 + 1] / currentErr;
        covariance.fillSymmetric(1, 0, (covariance)(1, 0) * currentErr);
        (covariance)(1, 1) = modes[5 + 1] * modes[5 + 1];
        covariance.fillSymmetric(2, 1, (covariance)(2, 1) * currentErr);
        covariance.fillSymmetric(3, 1, (covariance)(3, 1) * currentErr);
        covariance.fillSymmetric(4, 1, (covariance)(4, 1) * currentErr);
      }
      if (modes[5 + 2] > 0) {
        double currentErr = sqrt((covariance)(2, 2));
        currentErr = modes[5 + 2] / currentErr;
        covariance.fillSymmetric(2, 0, (covariance)(2, 0) * currentErr);
        covariance.fillSymmetric(2, 1, (covariance)(2, 1) * currentErr);
        (covariance)(2, 2) = modes[5 + 2] * modes[5 + 2];
        covariance.fillSymmetric(3, 2, (covariance)(3, 2) * currentErr);
        covariance.fillSymmetric(4, 2, (covariance)(4, 2) * currentErr);
      }
      if (modes[5 + 3] > 0) {
        double currentErr = sqrt((covariance)(3, 3));
        currentErr = modes[5 + 3] / currentErr;
        covariance.fillSymmetric(3, 0, (covariance)(3, 0) * currentErr);
        covariance.fillSymmetric(3, 1, (covariance)(3, 1) * currentErr);
        covariance.fillSymmetric(3, 2, (covariance)(3, 2) * currentErr);
        (covariance)(3, 3) = modes[5 + 3] * modes[5 + 3];
        covariance.fillSymmetric(4, 3, (covariance)(4, 3) * currentErr);
      }
      if (modes[5 + 4] > 0) {
        double currentErr = sqrt((covariance)(4, 4));
        currentErr = modes[5 + 4] / currentErr;
        covariance.fillSymmetric(4, 0, (covariance)(4, 0) * currentErr);
        covariance.fillSymmetric(4, 1, (covariance)(4, 1) * currentErr);
        covariance.fillSymmetric(4, 2, (covariance)(4, 2) * currentErr);
        covariance.fillSymmetric(4, 3, (covariance)(4, 3) * currentErr);
        (covariance)(4, 4) = modes[5 + 4] * modes[5 + 4];
      }

    } // modes[4]!=0
  }   // useMode && dimensions==5

  std::unique_ptr<Trk::TrackParameters> combinedTrackParameters = nullptr;
  double loc1 = mean[Trk::loc1];
  double loc2 = mean[Trk::loc2];
  double phi = mean[Trk::phi];
  double theta = mean[Trk::theta];
  double qoverp = mean[Trk::qOverP];
  if (firstMeasuredCov) {
    combinedTrackParameters =
      firstParameters->associatedSurface().createUniqueTrackParameters(
        loc1, loc2, phi, theta, qoverp, std::move(covariance));
  } else {
    combinedTrackParameters =
      firstParameters->associatedSurface().createUniqueTrackParameters(
        loc1, loc2, phi, theta, qoverp, std::nullopt);
  }

  return std::make_unique<Trk::ComponentParameters>(
    std::move(combinedTrackParameters), totalWeight);
}

} // end anonymous namespace

std::unique_ptr<Trk::TrackParameters>
Trk::MultiComponentStateCombiner::combineToSingle(
  const Trk::MultiComponentState& uncombinedState,
  const bool useMode,
  const double fractionPDFused)
{
  std::unique_ptr<Trk::ComponentParameters> combinedComponent =
    combineToSingleImpl(&uncombinedState, useMode, fractionPDFused);
  return std::move(combinedComponent->first);
}

void
Trk::MultiComponentStateCombiner::combineWithWeight(
  Trk::ComponentParameters& mergeTo,
  const Trk::ComponentParameters& addThis)
{
  const Trk::TrackParameters* firstTrackParameters = mergeTo.first.get();
  const AmgVector(5)& firstParameters = firstTrackParameters->parameters();
  double firstWeight = mergeTo.second;

  const Trk::TrackParameters* secondTrackParameters = addThis.first.get();
  const AmgVector(5)& secondParameters = secondTrackParameters->parameters();
  double secondWeight = addThis.second;

  // copy over the first
  AmgVector(5) finalParameters(firstParameters);
  double finalWeight = firstWeight;
  combineParametersWithWeight(
    finalParameters, finalWeight, secondParameters, secondWeight);

  const AmgSymMatrix(5)* firstMeasuredCov = firstTrackParameters->covariance();
  const AmgSymMatrix(5)* secondMeasuredCov = secondTrackParameters->covariance();
  // Check to see if first track parameters are measured or not
  if (firstMeasuredCov && secondMeasuredCov) {
    AmgSymMatrix(5) finalMeasuredCov(*firstMeasuredCov);
    combineCovWithWeight(firstParameters, finalMeasuredCov, firstWeight,
                         secondParameters, *secondMeasuredCov, secondWeight);
    mergeTo.first->updateParameters(finalParameters, finalMeasuredCov);
    mergeTo.second = finalWeight;
  } else {
    mergeTo.first->updateParameters(finalParameters);
    mergeTo.second = finalWeight;
  }
}
/**
 * Moment-preserving merge of two 5D components
 * for example see
 * Runnalls, Andrew R.(2007)
 * Kullback-Leibler approach to Gaussian mixture reduction
 * equations (2),(3),(4)
 */

// The following does heave use of Eigen
// for covariance. Avoid out-of-line calls
// to Eigen
#if defined(__GNUC__)
[[gnu::flatten]]
#endif
void
Trk::MultiComponentStateCombiner::combineParametersWithWeight(
  AmgVector(5) & firstParameters,
  double& firstWeight,
  const AmgVector(5) & secondParameters,
  const double secondWeight)
{
  double totalWeight = firstWeight + secondWeight;
  double deltaPhi = firstParameters[2] - secondParameters[2];
  if (deltaPhi > M_PI) {
    firstParameters[2] -= 2 * M_PI;
  } else if (deltaPhi < -M_PI) {
    firstParameters[2] += 2 * M_PI;
  }
  firstParameters =
      (firstWeight * firstParameters + secondWeight * secondParameters) /
      totalWeight;
  // Ensure that phi is between -pi and pi
  firstParameters[2] = CxxUtils::wrapToPi(firstParameters[2]);
  firstWeight = totalWeight;
}

// The following does heave use of Eigen
// for covariance. Avoid out-of-line calls
// to Eigen
#if defined(__GNUC__)
[[gnu::flatten]]
#endif
void
Trk::MultiComponentStateCombiner::combineCovWithWeight(
  const AmgVector(5) & firstParameters,
  AmgSymMatrix(5) & firstMeasuredCov,
  const double firstWeight,
  const AmgVector(5) & secondParameters,
  const AmgSymMatrix(5) & secondMeasuredCov,
  const double secondWeight)
{
  double totalWeight = firstWeight + secondWeight;
  AmgVector(5) parameterDifference = firstParameters - secondParameters;
  parameterDifference[2] = CxxUtils::wrapToPi(parameterDifference[2]);
  parameterDifference /= totalWeight;
  firstMeasuredCov =
      (firstWeight * firstMeasuredCov + secondWeight * secondMeasuredCov) /
      totalWeight;
  firstMeasuredCov += firstWeight * secondWeight * parameterDifference *
                      parameterDifference.transpose();
}

// The following does heave use of Eigen
// for covariance. Avoid out-of-line calls
// to Eigen
#if defined(__GNUC__)
[[gnu::flatten]]
#endif
Trk::MultiComponentState
Trk::MultiComponentStateCombiner::combineWithSmoother(
  const Trk::MultiComponentState& forwardsMultiState,
  const Trk::MultiComponentState& smootherMultiState,
  unsigned int maximumNumberOfComponents)
{

  std::unique_ptr<Trk::MultiComponentState> combinedMultiState =
    std::make_unique<Trk::MultiComponentState>();

  // Loop over all components in forwards multi-state
  for (const auto& forwardsComponent : forwardsMultiState) {
    // Need to check that all components have associated weight matricies
    const AmgSymMatrix(5)* forwardMeasuredCov =
      forwardsComponent.first->covariance();
    // Loop over all components in the smoother multi-state
    for (const auto& smootherComponent : smootherMultiState) {
      // Need to check that all components have associated weight matricies
      const AmgSymMatrix(5)* smootherMeasuredCov =
        smootherComponent.first->covariance();
      if (!smootherMeasuredCov && !forwardMeasuredCov) {
        return {};
      }

      if (!forwardMeasuredCov) {
        Trk::ComponentParameters smootherComponentOnly(
          smootherComponent.first->clone(), smootherComponent.second);
        combinedMultiState->push_back(std::move(smootherComponentOnly));
        continue;
      }

      if (!smootherMeasuredCov) {
        Trk::ComponentParameters forwardComponentOnly(
          forwardsComponent.first->clone(), forwardsComponent.second);
        combinedMultiState->push_back(std::move(forwardComponentOnly));
        continue;
      }

      const AmgSymMatrix(5) summedCovariance =
        *forwardMeasuredCov + *smootherMeasuredCov;
      const AmgSymMatrix(5) K =
        *forwardMeasuredCov * summedCovariance.inverse();
      const AmgVector(5) newParameters =
        forwardsComponent.first->parameters() +
        K * (smootherComponent.first->parameters() -
             forwardsComponent.first->parameters());
      const AmgVector(5) parametersDiff =
        forwardsComponent.first->parameters() -
        smootherComponent.first->parameters();

      AmgSymMatrix(5) covarianceOfNewParameters =
        AmgSymMatrix(5)(K * *smootherMeasuredCov);

      std::unique_ptr<Trk::TrackParameters> combinedTrackParameters =
        (forwardsComponent.first)
          ->associatedSurface()
          .createUniqueTrackParameters(newParameters[Trk::loc1],
                                       newParameters[Trk::loc2],
                                       newParameters[Trk::phi],
                                       newParameters[Trk::theta],
                                       newParameters[Trk::qOverP],
                                       std::move(covarianceOfNewParameters));
      const AmgSymMatrix(5) invertedSummedCovariance =
        summedCovariance.inverse();
      // Determine the scaling factor for the new weighting. Determined from the
      // PDF of the many-dimensional gaussian
      double exponent =
        parametersDiff.transpose() * invertedSummedCovariance * parametersDiff;
      double weightScalingFactor = exp(-0.5 * exponent);
      double combinedWeight = smootherComponent.second *
                              forwardsComponent.second * weightScalingFactor;
      Trk::ComponentParameters combinedComponent(
        std::move(combinedTrackParameters), combinedWeight);
      combinedMultiState->push_back(std::move(combinedComponent));
    }
  }
  // Component reduction on the combined state
  Trk::MultiComponentState mergedState =
    Trk::QuickCloseComponentsMultiStateMerger::merge(
      std::move(*combinedMultiState), maximumNumberOfComponents);
  // Before return the weights of the states need to be renormalised to one.
  Trk::MultiComponentStateHelpers::renormaliseState(mergedState);

  return mergedState;
}
