/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   GsfCombinedMaterialEffects.h
 * @date   Friday 11th January 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * Class definition for consideration of multiple scatter and energy
 * loss effects from material simultaneously
 */

#ifndef TrkGsfCombinedMaterialEffects_H
#define TrkGsfCombinedMaterialEffects_H

#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"
//
#include "TrkExInterfaces/IEnergyLossUpdator.h"
//
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class GsfCombinedMaterialEffects
  : public AthAlgTool
  , virtual public IMultiStateMaterialEffects
{
public:
  /** Helper Struct for multiple Gaussian componets*/
  struct ComponentValues
  {
    double weight;
    double mean;
    double variance;
  };
  using MixtureParameters =
    std::array<ComponentValues, GSFConstants::maxNumberofBHComponents>;

  /** Helper struct for construction and evaluation of polynomial */
  struct Polynomial
  {
    // Evaluation of the polynomial for given material thickness (t)
    double operator()(const double& t) const
    {
      double sum(0.);
      for (size_t i = 0; i < GSFConstants::polynomialCoefficients; ++i) {
        sum = t * sum + coefficients[i];
      }
      return sum;
    }
    std::array<double, GSFConstants::polynomialCoefficients> coefficients;
  };

  /** Constructor with AlgTool parameters*/
  GsfCombinedMaterialEffects(
    const std::string&,
    const std::string&,
    const IInterface*);

  /** Virtual destructor */
  virtual ~GsfCombinedMaterialEffects() override;

  /** AlgTool initialise method */
  virtual StatusCode initialize() override final;

  virtual void compute(
    GsfMaterial::Combined&,
    const ComponentParameters&,
    const MaterialProperties&,
    double,
    PropDirection = anyDirection,
    ParticleHypothesis = nonInteracting) const override final;

private:
  // Multipe scattering
  void scattering(
    GsfMaterial::Scattering&,
    const ComponentParameters& componentParameters,
    const MaterialProperties& materialProperties,
    double pathLength) const;

  // Non brem (non-electron) energy loss
  void energyLoss(
    GsfMaterial::EnergyLoss&,
    const ComponentParameters&,
    const MaterialProperties&,
    double,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  // Electron enrrgy loss due to Bremsstrahlung
  void BetheHeitler(
    GsfMaterial::EnergyLoss& cache,
    const ComponentParameters& componentParameters,
    const MaterialProperties& materialProperties,
    double pathLenght,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  // Read polynomial fit parameters from a specified file
  bool readBHParameters();

  int m_BHnumberOfComponents;
  int m_BHtransformationCode;
  int m_BHnumberOfComponentsHighX0;
  int m_BHtransformationCodeHighX0;

  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_BHpolynomialWeights;
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_BHpolynomialMeans;
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_BHpolynomialVariances;
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_BHpolynomialWeightsHighX0;
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_BHpolynomialMeansHighX0;
  std::array<Polynomial, GSFConstants::maxNumberofBHComponents>
    m_BHpolynomialVariancesHighX0;

  Gaudi::Property<bool> m_useHighX0{ this,
                                     "UseHighX0",
                                     true,
                                     "Use the high X0 parametrization" };

  Gaudi::Property<std::string> m_parameterisationFileName{
    this,
    "BetheHeitlerParameterisationFileName",
    "GeantSim_LT01_cdf_nC6_O5.par",
    "Parametrization of Bethe Heitler material effects"
  };

  Gaudi::Property<std::string> m_parameterisationFileNameHighX0{
    this,
    "BetheHeitlerParameterisationFileNameHighX0",
    "GeantSim_GT01_cdf_nC6_O5.par",
    "Parametrization of Bethe Heitler material effects for high X0"
  };
};
} // end Trk namespace

#endif
