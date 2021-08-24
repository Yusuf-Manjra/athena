/*
  Copyright (C) 2020-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file GsfMaterialMixtureConvolution.cxx
 * @begin         Julay 20 2020
 * @author        Anthony Morley
 * @brief         Implementation code for GSF material mixture convolution
 */

#include "TrkGaussianSumFilter/GsfMaterialMixtureConvolution.h"
#include "TrkGaussianSumFilter/IMultiStateMaterialEffectsUpdator.h"
#include "TrkGaussianSumFilterUtils/KLGaussianMixtureReduction.h"
#include "TrkGaussianSumFilterUtils/MultiComponentState.h"
#include "TrkGaussianSumFilterUtils/MultiComponentStateAssembler.h"
#include "TrkGaussianSumFilterUtils/MultiComponentStateCombiner.h"
#include "TrkGaussianSumFilterUtils/QuickCloseComponentsMultiStateMerger.h"
//
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkSurfaces/PerigeeSurface.h"

namespace {
bool
updateP(double& qOverP, double deltaP)
{
  double p = 1. / std::abs(qOverP);
  p += deltaP;
  if (p <= 0.) {
    return false;
  }
  qOverP = qOverP > 0. ? 1. / p : -1. / p;
  return true;
}
}

Trk::GsfMaterialMixtureConvolution::GsfMaterialMixtureConvolution(
  const std::string& type,
  const std::string& name,
  const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<IMaterialMixtureConvolution>(this);
}

Trk::GsfMaterialMixtureConvolution::~GsfMaterialMixtureConvolution() = default;

StatusCode
Trk::GsfMaterialMixtureConvolution::initialize()
{
  if (m_maximumNumberOfComponents > 16) {
    ATH_MSG_FATAL("Requested MaximumNumberOfComponents > 16");
    return StatusCode::FAILURE;
  }
  m_materialEffects = GsfCombinedMaterialEffects(
    m_parameterisationFileName, m_parameterisationFileNameHighX0);
  return StatusCode::SUCCESS;
}

/* ==========================================
   Update with full material effects
   ========================================== */

Trk::MultiComponentState
Trk::GsfMaterialMixtureConvolution::update(
  std::vector<GsfMaterial::Combined>& caches,
  const Trk::MultiComponentState& multiComponentState,
  const Trk::Layer& layer,
  Trk::PropDirection direction,
  Trk::ParticleHypothesis particleHypothesis) const
{

  const Trk::MaterialProperties* materialProperties =
    layer.fullUpdateMaterialProperties(*(multiComponentState.begin()->first));

  if (!materialProperties) {
    ATH_MSG_DEBUG("UPDATE but no material properties!!!");
  }

  Trk::MultiComponentState updatedMergedState = update(
    caches, multiComponentState, layer, direction, particleHypothesis, Normal);
  ATH_MSG_DEBUG("UPDATE update N in: " << multiComponentState.size()
                                       << " N out: "
                                       << updatedMergedState.size());
  if (updatedMergedState.empty()) {
    return {};
  }
  // Renormalise state
  MultiComponentStateHelpers::renormaliseState(updatedMergedState);

  return updatedMergedState;
}

/* ==========================================
   Update with pre-update material effects
========================================== */

Trk::MultiComponentState
Trk::GsfMaterialMixtureConvolution::preUpdate(
  std::vector<GsfMaterial::Combined>& caches,
  const Trk::MultiComponentState& multiComponentState,
  const Trk::Layer& layer,
  Trk::PropDirection direction,
  Trk::ParticleHypothesis particleHypothesis) const
{
  const Trk::MaterialProperties* materialProperties =
    layer.fullUpdateMaterialProperties(*(multiComponentState.begin()->first));
  if (!materialProperties) {
    ATH_MSG_DEBUG("PREUPDATE but no material properties!!!");
  }
  /* -------------------------------------
     Preliminary checks
     ------------------------------------- */
  Trk::MultiComponentState updatedMergedState = update(caches,
                                                       multiComponentState,
                                                       layer,
                                                       direction,
                                                       particleHypothesis,
                                                       Preupdate);
  ATH_MSG_DEBUG("PREUPDATE update N in: " << multiComponentState.size()
                                          << " N out: "
                                          << updatedMergedState.size());
  if (updatedMergedState.empty()) {
    return {};
  }
  // Renormalise state
  MultiComponentStateHelpers::renormaliseState(updatedMergedState);

  return updatedMergedState;
}

/* ==========================================
   Update with post-update material effects
   ========================================== */

Trk::MultiComponentState
Trk::GsfMaterialMixtureConvolution::postUpdate(
  std::vector<GsfMaterial::Combined>& caches,
  const Trk::MultiComponentState& multiComponentState,
  const Trk::Layer& layer,
  Trk::PropDirection direction,
  Trk::ParticleHypothesis particleHypothesis) const
{

  const Trk::MaterialProperties* materialProperties =
    layer.fullUpdateMaterialProperties(*(multiComponentState.begin()->first));
  if (!materialProperties) {
    ATH_MSG_DEBUG("POSTUPDATE but no material properties!!!");
  }
  /* -------------------------------------
     Preliminary checks
     ------------------------------------- */

  Trk::MultiComponentState updatedMergedState = update(caches,
                                                       multiComponentState,
                                                       layer,
                                                       direction,
                                                       particleHypothesis,
                                                       Postupdate);

  ATH_MSG_DEBUG("POSTUPDATE update N in: " << multiComponentState.size()
                                           << " N out: "
                                           << updatedMergedState.size());
  if (updatedMergedState.empty()) {
    return {};
  }
  // Renormalise state
  MultiComponentStateHelpers::renormaliseState(updatedMergedState);

  return updatedMergedState;
}

Trk::MultiComponentState
Trk::GsfMaterialMixtureConvolution::update(
  std::vector<GsfMaterial::Combined>& caches,
  const Trk::MultiComponentState& inputState,
  const Trk::Layer& layer,
  Trk::PropDirection direction,
  Trk::ParticleHypothesis particleHypothesis,
  MaterialUpdateType updateType) const
{

  // Check the multi-component state is populated
  if (inputState.empty()) {
    ATH_MSG_DEBUG("Multi component state passed to update is "
                  "not populated... returning 0");
    return {};
  }

  double updateFactor(1.);
  // Full method does this for each component which i don't think this is needed
  if (updateType == Preupdate) {
    updateFactor =
      layer.preUpdateMaterialFactor(*inputState.front().first, direction);
    ATH_MSG_DEBUG("Material effects update prior to propagation using layer "
                  "information and particle hypothesis: "
                  << particleHypothesis);
  } else if (updateType == Postupdate) {
    updateFactor =
      layer.postUpdateMaterialFactor(*inputState.front().first, direction);
    ATH_MSG_DEBUG("Material effects update after propagation using layer "
                  "information and particle hypothesis: "
                  << particleHypothesis);
  }

  if (updateFactor < 0.01) {
    // Bail out as factor is too small to bother about
    return {};
  }

  caches.resize(inputState.size());

  // Fill cache and work out how many final components there should be
  size_t n(0);
  for (size_t i(0); i < inputState.size(); ++i) {
    const AmgSymMatrix(5)* measuredCov = inputState[i].first->covariance();
    // If the momentum is too dont apply material effects
    if (inputState[i].first->momentum().mag() <= m_momentumCut) {
      ATH_MSG_DEBUG("Ignoring material effects... Momentum too low");
      caches[i].resetAndAddDummyValues();
      caches[i].deltaParameters[0] = inputState[i].first->parameters();
      caches[i].weights[0] = inputState[i].second;
      if (measuredCov) {
        caches[i].deltaCovariances[0] += *measuredCov;
      }
      n += caches[i].numWeights;
      continue;
    }

    // Get the material effects and store them in the cache
    std::pair<const Trk::MaterialProperties*, double> matPropPair =
      getMaterialProperties(inputState[i].first.get(), layer);

    if (!matPropPair.first) {
      ATH_MSG_DEBUG("No material properties .. dont apply material effects");
      caches[i].resetAndAddDummyValues();
      caches[i].deltaParameters[0] = inputState[i].first->parameters();
      caches[i].weights[0] = inputState[i].second;
      if (measuredCov) {
        caches[i].deltaCovariances[0] += *measuredCov;
      }
      n += caches[i].numWeights;
      continue;
    }

    // Apply the update factor
    matPropPair.second *= updateFactor;

    m_materialEffects.compute(caches[i],
                              inputState[i],
                              *matPropPair.first,
                              matPropPair.second,
                              direction,
                              particleHypothesis);

    // check vectors have the same size
    if (caches[i].numWeights != caches[i].numDeltaPs) {
      ATH_MSG_WARNING("Inconsistent number of components in the updator!!! no "
                      "material effect will be applied");
      caches[i].resetAndAddDummyValues();
    }

    // Apply material effects to input state and store results in cache
    for (size_t j(0); j < caches[i].numWeights; ++j) {
      if (measuredCov) {
        caches[i].deltaCovariances[j] += *measuredCov;
      } else {
        caches[i].deltaCovariances[j].setZero();
      }
      caches[i].deltaParameters[j] = inputState[i].first->parameters();
      // Adjust the momentum of the component's parameters vector here. Check to
      // make sure update is good.
      if (!updateP(caches[i].deltaParameters[j][Trk::qOverP],
                   caches[i].deltaPs[j])) {
        ATH_MSG_ERROR("Cannot update state vector momentum!!! return nullptr");
        return {};
      }
      // Store component weight
      caches[i].weights[j] *= inputState[i].second;
      // Ensure weight of component is not too small to save us from potential
      // FPE's Value. Weights are double so the min of float should
      // be small enough and should be handled
      if (caches[i].weights[j] < std::numeric_limits<float>::min()) {
        caches[i].weights[j] = std::numeric_limits<float>::min();
      }
    }
    n += caches[i].numWeights;
  }

  // Fill information for to calculate which components to merge
  // In addition scan all components for covariance matrices. If one or more
  // component is missing an error matrix, component reduction is impossible.
  bool componentWithoutMeasurement = false;

  GSFUtils::Component1DArray componentsArray;
  componentsArray.numComponents = n;
  std::vector<std::pair<size_t, size_t>> indices(n);
  size_t k(0);
  for (size_t i(0); i < inputState.size(); ++i) {
    for (size_t j(0); j < caches[i].numWeights; ++j) {
      const AmgSymMatrix(5)* measuredCov = inputState[i].first->covariance();
      // Fill in infomation
      const double cov =
        measuredCov ? caches[i].deltaCovariances[j](Trk::qOverP, Trk::qOverP)
                    : -1.;
      if (!measuredCov) {
        componentWithoutMeasurement = true;
      }

      componentsArray.components[k].mean =
        caches[i].deltaParameters[j][Trk::qOverP];
      componentsArray.components[k].cov = cov;
      componentsArray.components[k].invCov = cov > 0 ? 1. / cov : 1e10;
      componentsArray.components[k].weight = caches[i].weights[j];
      indices[k] = { i, j };
      ++k;
    }
  }

  if (componentWithoutMeasurement) {
    auto* result = std::max_element(
      componentsArray.components.data(),
      componentsArray.components.data() + componentsArray.numComponents,
      [](const auto& a, const auto& b) { return a.weight < b.weight; });
    auto index = std::distance(componentsArray.components.data(), result);

    // Build the first TP
    size_t stateIndex = indices[index].first;
    size_t materialIndex = indices[index].second;

    AmgVector(5)& updatedStateVector =
      caches[stateIndex].deltaParameters[materialIndex];
    const AmgSymMatrix(5)* measuredCov =
      inputState[stateIndex].first->covariance();
    std::optional<AmgSymMatrix(5)> updatedCovariance = std::nullopt;
    if (measuredCov &&
        caches[stateIndex].deltaCovariances.size() > materialIndex) {
      updatedCovariance =
        AmgSymMatrix(5)(caches[stateIndex].deltaCovariances[materialIndex]);
    }
    std::unique_ptr<Trk::TrackParameters> updatedTrackParameters =
      inputState[stateIndex]
        .first->associatedSurface()
        .createUniqueTrackParameters(updatedStateVector[Trk::loc1],
                                     updatedStateVector[Trk::loc2],
                                     updatedStateVector[Trk::phi],
                                     updatedStateVector[Trk::theta],
                                     updatedStateVector[Trk::qOverP],
                                     std::move(updatedCovariance));

    Trk::ComponentParameters dummyCompParams(std::move(updatedTrackParameters),
                                             1.);
    Trk::MultiComponentState returnMultiState;
    returnMultiState.push_back(std::move(dummyCompParams));
    return returnMultiState;
  }

  // Gather the merges -- order is important -- RHS is smaller than LHS
  GSFUtils::MergeArray KL;
  if (n > m_maximumNumberOfComponents) {
    KL = findMerges(componentsArray, m_maximumNumberOfComponents);
  }
  // Merge components
  MultiComponentStateAssembler::Cache assemblerCache;
  int nMerges(0);
  GSFUtils::IsMergedArray isMerged = {};
  int32_t returnedMerges = KL.numMerges;

  for (int32_t i = 0; i < returnedMerges; ++i) {
    const int8_t mini = KL.merges[i].To;
    const int8_t minj = KL.merges[i].From;
    if (isMerged[minj]) {
      ATH_MSG_WARNING("Component is already merged " << minj);
      continue;
    }
    // Get the first TP
    size_t stateIndex = indices[mini].first;
    size_t materialIndex = indices[mini].second;
    // Copy weight and first parameters as they are needed later on
    // for updating the covariance
    AmgVector(5) firstParameters =
      caches[stateIndex].deltaParameters[materialIndex];
    double firstWeight = caches[stateIndex].weights[materialIndex];

    // Get the second TP
    size_t stateIndex2 = indices[minj].first;
    size_t materialIndex2 = indices[minj].second;

    // Some values for sanity checks
    ++nMerges;
    isMerged[minj] = true;

    // Update first parameters and weight
    Trk::MultiComponentStateCombiner::combineParametersWithWeight(
      caches[stateIndex].deltaParameters[materialIndex],
      caches[stateIndex].weights[materialIndex],
      caches[stateIndex2].deltaParameters[materialIndex2],
      caches[stateIndex2].weights[materialIndex2]);

    // Update covariance
    Trk::MultiComponentStateCombiner::combineCovWithWeight(
      firstParameters,
      caches[stateIndex].deltaCovariances[materialIndex],
      firstWeight,
      caches[stateIndex2].deltaParameters[materialIndex2],
      caches[stateIndex2].deltaCovariances[materialIndex2],
      caches[stateIndex2].weights[materialIndex2]);

    // Reset 2nd parameters values just for clarity
    caches[stateIndex2].deltaParameters[materialIndex2].setZero();
    caches[stateIndex2].deltaCovariances[materialIndex2].setZero();
  }

  for (size_t i(0); i < n; ++i) {
    if (isMerged[i]) {
      continue;
    }

    // Build the TP
    size_t stateIndex = indices[i].first;
    size_t materialIndex = indices[i].second;
    AmgVector(5)& stateVector =
      caches[stateIndex].deltaParameters[materialIndex];
    AmgSymMatrix(5)& measuredCov =
      caches[stateIndex].deltaCovariances[materialIndex];

    std::unique_ptr<Trk::TrackParameters> updatedTrackParameters =
      inputState[stateIndex]
        .first->associatedSurface()
        .createUniqueTrackParameters(stateVector[Trk::loc1],
                                     stateVector[Trk::loc2],
                                     stateVector[Trk::phi],
                                     stateVector[Trk::theta],
                                     stateVector[Trk::qOverP],
                                     measuredCov);

    double updatedWeight = caches[stateIndex].weights[materialIndex];

    assemblerCache.multiComponentState.emplace_back(
      std::move(updatedTrackParameters), updatedWeight);
    assemblerCache.validWeightSum += updatedWeight;
  }

  if (nMerges + assemblerCache.multiComponentState.size() != n) {
    ATH_MSG_WARNING("Combining complete but merger size is incompatible: "
                    << n << "  " << nMerges << " "
                    << assemblerCache.multiComponentState.size());
  }

  // Check all weights
  Trk::MultiComponentState mergedState =
    MultiComponentStateAssembler::assembledState(std::move(assemblerCache));

  if (mergedState.size() > m_maximumNumberOfComponents)
    ATH_MSG_ERROR("Merging failed, target size: " << m_maximumNumberOfComponents
                                                  << " final size: "
                                                  << mergedState.size());
  return mergedState;
}

std::pair<const Trk::MaterialProperties*, double>
Trk::GsfMaterialMixtureConvolution::getMaterialProperties(
  const Trk::TrackParameters* trackParameters,
  const Trk::Layer& layer) const
{

  const Trk::MaterialProperties* materialProperties(nullptr);
  double pathCorrection(0.);

  // Incorporate the reference material

  if (m_useReferenceMaterial) {

    // Get the surface associated with the parameters
    const Trk::Surface* surface = &(trackParameters->associatedSurface());

    // Only utilise the reference material if an associated detector element
    // exists
    if (surface && surface->associatedDetectorElement()) {

      // Get the layer material properties
      const Trk::LayerMaterialProperties* layerMaterial =
        layer.layerMaterialProperties();

      // Assign the material properties
      materialProperties =
        layerMaterial ? layerMaterial->fullMaterial(trackParameters->position())
                      : nullptr;

      // Determine the pathCorrection if the material properties exist
      pathCorrection = materialProperties
                         ? 1. / std::abs(surface->normal().dot(
                                  trackParameters->momentum().unit()))
                         : 0.;
    }
  }

  // Check that the material properties have been defined - if not define them
  // from the layer information
  materialProperties = materialProperties
                         ? materialProperties
                         : layer.fullUpdateMaterialProperties(*trackParameters);

  // Bail out if still no material properties can be found
  if (!materialProperties) {
    return { nullptr, 0 };
  }

  // Define the path correction
  pathCorrection =
    pathCorrection > 0.
      ? pathCorrection
      : layer.surfaceRepresentation().pathCorrection(
          trackParameters->position(), trackParameters->momentum());

  // The pathlength ( in mm ) is the path correction * the thickness of the
  // material
  double pathLength = pathCorrection * materialProperties->thickness();

  return { materialProperties, pathLength };
}
