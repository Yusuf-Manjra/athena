/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   GsfMeasurementUpdator.h
 * @date   Friday 25th February 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief  Class for performing updates on multi-component states for the
 * gaussian-sum filter.
 */

#ifndef TrkGsfMeasurementUpdator_H
#define TrkGsfMeasurementUpdator_H

#include "TrkGaussianSumFilterUtils/KalmanParameterUpdator.h"
#include "TrkGaussianSumFilterUtils/MultiComponentState.h"
#include "TrkGaussianSumFilterUtils/MultiComponentStateAssembler.h"
//
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"

namespace Trk {

class GsfMeasurementUpdator
{
public:
  /** Method for updating the multi-state with a new measurement */
  MultiComponentState update(MultiComponentState&&, const MeasurementBase&)
    const;

  /** Method for updating the multi-state with a new measurement and calculate
   * the fit qaulity at the same time*/
  MultiComponentState update(
    Trk::MultiComponentState&&,
    const Trk::MeasurementBase&,
    FitQualityOnSurface& fitQoS) const;

  /** Method for determining the chi2 of the multi-component state and the
   * number of degrees of freedom */
  FitQualityOnSurface fitQuality(
    const MultiComponentState&,
    const MeasurementBase&) const;

private:
  MultiComponentState calculateFilterStep(
    MultiComponentState&&,
    const MeasurementBase&,
    int addRemoveFlag) const;

  MultiComponentState calculateFilterStep(
    MultiComponentState&&,
    const MeasurementBase&,
    FitQualityOnSurface& fitQoS) const;

};
}

#endif
