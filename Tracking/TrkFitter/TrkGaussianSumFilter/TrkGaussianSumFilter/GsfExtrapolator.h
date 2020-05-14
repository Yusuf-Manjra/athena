/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/*********************************************************************************
  GsfExtrapolator.h  -  description
------------------------------------------
begin                : Tuesday 25th January 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Extrapolation of MultiComponentState class based on
Andi's single component state extrapolator. It is an
AlgTool inheriting from the IMultiStateExtrapolator class
*********************************************************************************/

#ifndef TrkGsfExtrapolator_H
#define TrkGsfExtrapolator_H

#include <Gaudi/Accumulators.h>

#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkExInterfaces/INavigator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkParameters/TrackParameters.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include "TrkGeometry/MagneticFieldProperties.h"
#include <memory>
#include <string>
#include <vector>

namespace Trk {

class Layer;
class Surface;
class TrackingVolume;
class TrackingGeometry;
class TrackStateOnSurface;
class MaterialProperties;
class IMultiComponentStateMerger;
class IMaterialMixtureConvolution;
class IMultipleScatteringUpdator;
/** @struct StateAtBoundarySurface
  - Structure to contain information about a state at the interface between
  tracking volumes
  */

struct StateAtBoundarySurface
{

  /** Data members */
  const MultiComponentState* stateAtBoundary;
  const TrackParameters* navigationParameters;
  const TrackingVolume* trackingVolume;

  /** Default constructor  */
  StateAtBoundarySurface()
    : stateAtBoundary(nullptr)
    , navigationParameters(nullptr)
    , trackingVolume(nullptr)
  {}

  /** Update State at Boundary Surface Information */
  void updateBoundaryInformation(const MultiComponentState* boundaryState,
                                 const TrackParameters* navParameters,
                                 const TrackingVolume* nextVolume)
  {
    stateAtBoundary = boundaryState;
    navigationParameters = navParameters;
    trackingVolume = nextVolume;
  }
};

/** @class GsfExtrapolator */

class GsfExtrapolator
  : public AthAlgTool
  , virtual public IMultiStateExtrapolator
{

public:
  /** Constructor with AlgTool parameters */
  GsfExtrapolator(const std::string&, const std::string&, const IInterface*);

  /** Destructor */
  virtual ~GsfExtrapolator() override final;

  /** AlgTool initialise method */
  virtual StatusCode initialize() override final;

  /** AlgTool finalise method */
  virtual StatusCode finalize() override final;

  /** Configured AlgTool extrapolation method (1) */
  virtual MultiComponentState extrapolate(
    const EventContext& ctx,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

  /** Configured AlgTool extrapolation without material effects method (2) */
  virtual MultiComponentState extrapolateDirectly(
    const EventContext& ctx,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

  virtual std::unique_ptr<std::vector<const Trk::TrackStateOnSurface*>>
  extrapolateM(const EventContext& ctx,
               const MultiComponentState&,
               const Surface& sf,
               PropDirection dir,
               const BoundaryCheck& bcheck,
               ParticleHypothesis particle) const override final;

private:
  struct Cache
  {
    bool m_recall;                  //!< Flag the recall solution
    const Surface* m_recallSurface; //!< Surface for recall
    const Layer* m_recallLayer;     //!< Layer for recall
    const TrackingVolume*
      m_recallTrackingVolume; //!< Tracking volume for recall
    StateAtBoundarySurface
      m_stateAtBoundarySurface; //!< Instance of structure describing the state
                                //!< at a boundary of tracking volumes
    std::unique_ptr<std::vector<const Trk::TrackStateOnSurface*>> m_matstates;
    std::vector<std::unique_ptr<const MultiComponentState>>
      m_mcsGarbageBin; //!< Garbage bin for MultiComponentState objects
    std::vector<std::unique_ptr<const TrackParameters>>
      m_tpGarbageBin; //!< Garbage bin for TrackParameter objects

    Cache()
      : m_recall(false)
      , m_recallSurface(nullptr)
      , m_recallLayer(nullptr)
      , m_recallTrackingVolume(nullptr)
      , m_stateAtBoundarySurface()
      , m_matstates(nullptr)
      , m_mcsGarbageBin()
      , m_tpGarbageBin()
    {}
  };

  /** These are the methods that do the actual heavy lifting when extrapolating
   * with a cache */
  MultiComponentState extrapolateImpl(
    const EventContext& ctx,
    Cache& cache,
    const IPropagator&,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Configured AlgTool extrapolation without material effects method (2) */
  MultiComponentState extrapolateDirectlyImpl(
    const EventContext& ctx,
    const IPropagator&,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Two primary private extrapolation methods
    - extrapolateToVolumeBoundary - extrapolates to the exit of the destination
    tracking volume
    - Exit layer surface will be hit in this method.
    - extrapolateInsideVolume     - extrapolates to the destination surface in
    the final tracking volume
    */

  void extrapolateToVolumeBoundary(
    const EventContext& ctx,
    Cache& cache,
    const IPropagator&,
    const MultiComponentState&,
    const Layer*,
    const TrackingVolume&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  MultiComponentState extrapolateInsideVolume(
    const EventContext& ctx,
    Cache& cache,
    const IPropagator&,
    const MultiComponentState&,
    const Surface&,
    const Layer*,
    const TrackingVolume&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Additional private extrapolation methods */

  /** Layer stepping, stopping at the last layer before destination */
  MultiComponentState extrapolateFromLayerToLayer(
    const EventContext& ctx,
    Cache& cache,
    const IPropagator&,
    const MultiComponentState&,
    const TrackingVolume&,
    const Layer* startLayer,
    const Layer* destinationLayer = nullptr,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Single extrapolation step to an intermediate layer */
  MultiComponentState extrapolateToIntermediateLayer(
    const EventContext& ctx,
    Cache& cache,
    const IPropagator&,
    const MultiComponentState&,
    const Layer&,
    const TrackingVolume&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting,
    bool perpendicularCheck = true) const;

  /** Final extrapolation step to a destination layer */
  MultiComponentState extrapolateToDestinationLayer(
    const EventContext& ctx,
    Cache& cache,
    const IPropagator&,
    const MultiComponentState&,
    const Surface&,
    const Layer&,
    const Layer*,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Extrapolation to consider material effects assuming all material on active
   * sensor elements - CTB method */
  Trk::MultiComponentState extrapolateSurfaceBasedMaterialEffects(
    const EventContext& ctx,
    const IPropagator&,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** GSF Method to propagate a number of components simultaneously */
  Trk::MultiComponentState multiStatePropagate(
    const EventContext& ctx,
    const IPropagator&,
    const MultiComponentState&,
    const Surface&,
    PropDirection direction = anyDirection,
    const BoundaryCheck& boundaryCheck = true,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Method to choose propagator type */
  unsigned int propagatorType(const TrackingVolume& trackingVolume) const;

  /** Method to initialise navigation parameters including starting state, layer
   * and volume, and destination volume */
  void initialiseNavigation(const EventContext& ctx,
                            Cache& cache,
                            const IPropagator& propagator,
                            const MultiComponentState& initialState,
                            const Surface& surface,
                            const Layer*& associatedLayer,
                            const TrackingVolume*& currentVolume,
                            const TrackingVolume*& destinationVolume,
                            const TrackParameters*& referenceParameters,
                            PropDirection direction) const;

  bool radialDirectionCheck(const EventContext& ctx,
                            const IPropagator& prop,
                            const MultiComponentState& startParm,
                            const MultiComponentState& parsOnLayer,
                            const TrackingVolume& tvol,
                            PropDirection dir,
                            ParticleHypothesis particle) const;

  /** Method to set the recall information */
  void setRecallInformation(Cache& cache,
                            const Surface&,
                            const Layer&,
                            const TrackingVolume&) const;

  /** Method to reset the recall information */
  void resetRecallInformation(Cache& cache) const;

  /** Private method to throw a multi-component state into the garbage bin */
  void throwIntoGarbageBin(Cache& cache, const MultiComponentState*) const;

  /** Private method to throw a set of track parameters into the garbage bin */
  void throwIntoGarbageBin(Cache& cache, const TrackParameters*) const;

  /** Private method to empty garbage bins */
  void emptyGarbageBins(Cache& cache) const;
  /** Add material to vector*/
  void addMaterialtoVector(
    Cache& cache,
    const Trk::Layer* nextLayer,
    const Trk::TrackParameters* nextPar,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  std::string layerRZoutput(const Trk::Layer* lay) const;

  std::string positionOutput(const Amg::Vector3D& pos) const;

  int radialDirection(const Trk::MultiComponentState& pars,
                      PropDirection dir) const;

  ToolHandleArray<IPropagator> m_propagators{ this, "Propagators", {}, "" };
  ToolHandle<INavigator> m_navigator{ this,
                                      "Navigator",
                                      "Trk::Navigator/Navigator",
                                      "" };
  ToolHandle<IMaterialMixtureConvolution> m_materialUpdator{
    this,
    "GsfMaterialConvolution",
    "Trk::GsfMaterialMixtureConvolution/GsfMaterialMixtureConvolution",
    ""
  };
  ToolHandle<IMultipleScatteringUpdator> m_msupdators{
    this,
    "MultipleScatteringUpdator",
    "Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator",
    ""
  };
  ToolHandle<IEnergyLossUpdator> m_elossupdators{
    this,
    "EnergyLossUpdator",
    "Trk::EnergyLossUpdator/AtlasEnergyLossUpdator",
    ""
  };

  bool m_propagatorStickyConfiguration; //!< Switch between simple and full
                                        //!< configured propagators
  bool m_surfaceBasedMaterialEffects;   //!< Switch to turn on/off surface based
                                        //!< material effects
  bool m_fastField;
  unsigned int
    m_propagatorConfigurationLevel; //!< Configuration level of the propagator
  unsigned int m_propagatorSearchLevel; //!< Search level of the propagator
  Trk::MagneticFieldProperties m_fieldProperties;

  mutable Gaudi::Accumulators::Counter<int,
                                       Gaudi::Accumulators::atomicity::full>
    m_extrapolateCalls; //!< Statistics: Number of calls to the main extrapolate
                        //!< method
  mutable Gaudi::Accumulators::Counter<int,
                                       Gaudi::Accumulators::atomicity::full>
    m_extrapolateDirectlyCalls; //!< Statistics: Number of calls to the
                                //!< extrapolate directly method
  mutable Gaudi::Accumulators::Counter<int,
                                       Gaudi::Accumulators::atomicity::full>
    m_extrapolateDirectlyFallbacks; //!< Statistics: Number of calls to the
                                    //!< extrapolate directly fallback
  mutable Gaudi::Accumulators::Counter<int,
                                       Gaudi::Accumulators::atomicity::full>
    m_navigationDistanceIncreaseBreaks; //!< Statistics: Number of times
                                        //!< navigation stepping fails to go the
                                        //!< right way
  mutable Gaudi::Accumulators::Counter<int,
                                       Gaudi::Accumulators::atomicity::full>
    m_oscillationBreaks; //!< Statistics: Number of times a tracking volume
                         //!< oscillation is detected
  mutable Gaudi::Accumulators::Counter<int,
                                       Gaudi::Accumulators::atomicity::full>
    m_missedVolumeBoundary; //!< Statistics: Number of times the volume boundary
                            //!< is missed
};

} // end namespace Trk

// for the inline implementations
#include "GsfExtrapolator.icc"
#endif
