/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* *********************************************************************************
                        IMultiStateExtrapolator.h  -  description
                        ------------------------------------------
begin                : Tuesday 25th January 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Extrapolation of a MultiComponentState to a destination
                       surface
================================================================================= */

#ifndef TrkIMultiStateExtrapolator_H
#define TrkIMultiStateExtrapolator_H

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/BoundaryCheck.h"

#include "GaudiKernel/IAlgTool.h"
#include <vector>
#include <memory>
namespace Trk {

class IPropagator;
class Surface;
class Track;
class TrackingVolume;
class TrackStateOnSurface;

static const InterfaceID IID_IMultiStateExtrapolator("IMultiStateExtrapolator", 1, 0);

class IMultiStateExtrapolator : virtual public IAlgTool
{
public:
  /** Virtual destructor */
  virtual ~IMultiStateExtrapolator()= default;

  /** AlgTool interface method */
  static const InterfaceID& interfaceID() { return IID_IMultiStateExtrapolator; };

  /** Extrapolation of a MutiComponentState to a destination surface (1) */
  virtual std::unique_ptr<MultiComponentState> extrapolate(
    const IPropagator&,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    BoundaryCheck boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;

  /** - Extrapolation of a MultiComponentState to destination surface without material effects (2) */
  virtual std::unique_ptr<MultiComponentState> extrapolateDirectly(
    const IPropagator&,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    BoundaryCheck boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;

  /** Configured AlgTool extrapolation method (1) */
  virtual std::unique_ptr<MultiComponentState> extrapolate(
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    BoundaryCheck boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;

  /** Configured AlgTool extrapolation without material effects method (2) */
  virtual std::unique_ptr<MultiComponentState> extrapolateDirectly(
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    BoundaryCheck boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;

  virtual std::unique_ptr<std::vector<const Trk::TrackStateOnSurface*>> extrapolateM( 
    const MultiComponentState&,
    const Surface&,
    PropDirection dir = anyDirection,
    BoundaryCheck bcheck = true,
    ParticleHypothesis particle = nonInteracting) const = 0;
};

} // end trk namespace

#endif
