/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   MultiComponentStateCombiner.h
 * @date   Monday 20th December 2004
 * @author Atkinson,Anthony Morley, Christos Anastopoulos
 *
 * Methods that take a multi-component state and collapses
 * all components returning a single set of track
 * parameters with single mean and covariance matrix.
 */

#ifndef MultiComponentStateCombiner_H
#define MultiComponentStateCombiner_H

#include "TrkGaussianSumFilterUtils/MultiComponentState.h"
namespace Trk {

namespace MultiComponentStateCombiner {

/** @bried Calculate combined state of many components */
std::unique_ptr<Trk::TrackParameters>
combine(const MultiComponentState&,
        const bool useMode = false,
        const double fractionPDFused = 1.0);

/** @brief Combined/merge a component to another one */
void
combineWithWeight(Trk::ComponentParameters& mergeTo,
                  const Trk::ComponentParameters& addThis);

/** @brief Update parameters */
void
combineParametersWithWeight(AmgVector(5) & firstParameters,
                            double& firstWeight,
                            const AmgVector(5) & secondParameters,
                            const double secondWeight);

/** @brief Update cov matrix */
void
combineCovWithWeight(const AmgVector(5) & firstParameters,
                     AmgSymMatrix(5) & firstMeasuredCov,
                     const double firstWeight,
                     const AmgVector(5) & secondParameters,
                     const AmgSymMatrix(5) & secondMeasuredCov,
                     const double secondWeight);

}
} // end Trk namespace
#endif
