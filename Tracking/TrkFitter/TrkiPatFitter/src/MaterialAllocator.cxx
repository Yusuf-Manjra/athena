/*
   Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
 */

/***************************************************************************
   least squared fit to track hit data => PerigeeParameters with covariance
   and fit quality.
***************************************************************************/

#include "TrkiPatFitter/MaterialAllocator.h"

#include <cmath>
#include <iomanip>

#include "GaudiKernel/SystemOfUnits.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkiPatFitterUtils/ExtrapolationType.h"
#include "TrkiPatFitterUtils/FitMeasurement.h"
#include "TrkiPatFitterUtils/FitParameters.h"
#include "TrkiPatFitterUtils/MessageHelper.h"

namespace Trk {
MaterialAllocator::MaterialAllocator(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
    : AthAlgTool(type, name, parent),
      m_aggregateMaterial(true),
      m_allowReordering(false),
      m_useStepPropagator(1),
      m_maxWarnings(10),
      m_orderingTolerance(1. * Gaudi::Units::mm),
      m_scattererMinGap(100. * Gaudi::Units::mm),
      m_scatteringConstant(13.6 *
                           Gaudi::Units::MeV),  // Coulomb scattering constant
      m_scatteringLogCoeff(0.038),              // Coulomb scattering constant
      m_sectorMaxPhi(0.28),
      m_stationMaxGap(0.6 * Gaudi::Units::meter),
      m_calorimeterVolume(nullptr),
      m_indetVolume(nullptr),
      m_messageHelper(nullptr) {
  m_messageHelper = std::make_unique<MessageHelper>(*this, 6);
  declareInterface<IMaterialAllocator>(this);

  declareProperty("AggregateMaterial", m_aggregateMaterial);
  declareProperty("AllowReordering", m_allowReordering);

  // m_useStepPropagator 0 means not used (so Intersector used)
  // 1 Intersector not used and StepPropagator used with FullField
  // 2 StepPropagator with FastField propagation
  // 99 debug mode where both are ran with FullField

  declareProperty("UseStepPropagator", m_useStepPropagator);
  declareProperty("OrderingTolerance", m_orderingTolerance);
  declareProperty(
      "MaxNumberOfWarnings", m_maxWarnings,
      "Maximum number of permitted WARNING messages per message type.");
}

StatusCode MaterialAllocator::initialize() {
  // fill WARNING messages
  m_messageHelper->setMaxNumberOfMessagesPrinted(m_maxWarnings);
  m_messageHelper->setMessage(
      0,
      "leadingSpectrometerTSOS:  missing TrackingGeometrySvc - no leading "
      "material will be added");
  m_messageHelper->setMessage(
      1, "indetMaterial: extrapolateM finds no material on track");
  m_messageHelper->setMessage(
      2,
      "spectrometerMaterial: missing TrackingGeometrySvc - no spectrometer "
      "material added");
  m_messageHelper->setMessage(3,
                              "spectrometerMaterial: did not find MS entrance "
                              "surface - no MS material taken into account");
  m_messageHelper->setMessage(4, "spectrometerMaterial: failed extrapolation");
  m_messageHelper->setMessage(
      5, "spectrometerMaterial: extrapolateM finds no material on track");

  // retrieve the necessary Extrapolators (muon tracking geometry is very
  // picky!)
  ATH_CHECK(m_extrapolator.retrieve());
  ATH_MSG_DEBUG("Retrieved tool " << m_extrapolator);

  ATH_CHECK(m_intersector.retrieve());
  ATH_MSG_DEBUG("Retrieved tool " << m_intersector);

  // retrieve services
  if (m_trackingGeometryReadKey.empty()) {
    ATH_CHECK(m_trackingGeometrySvc.retrieve());
  } else
    ATH_CHECK(m_trackingGeometryReadKey.initialize());
  // need to create the IndetExit and MuonEntrance TrackingVolumes
  ATH_CHECK(m_trackingVolumesSvc.retrieve());
  ATH_MSG_DEBUG("Retrieved Svc " << m_trackingVolumesSvc);
  m_calorimeterVolume = &(m_trackingVolumesSvc->volume(
      ITrackingVolumesSvc::MuonSpectrometerEntryLayer));
  m_indetVolume = &(
      m_trackingVolumesSvc->volume(ITrackingVolumesSvc::CalorimeterEntryLayer));

  if (m_useStepPropagator > 0) {
    ATH_CHECK(m_stepPropagator.retrieve());
  }

  // Field for StepPropagator
  m_stepField = Trk::MagneticFieldProperties(Trk::FullField);
  if (m_useStepPropagator == 2)
    m_stepField = Trk::MagneticFieldProperties(Trk::FastField);

  return StatusCode::SUCCESS;
}

StatusCode MaterialAllocator::finalize() {
  // summarize WARNINGs
  m_messageHelper->printSummary();
  return StatusCode::SUCCESS;
}

void MaterialAllocator::addLeadingMaterial(
    std::vector<FitMeasurement*>& measurements,
    ParticleHypothesis particleHypothesis, FitParameters& fitParameters,
    Garbage_t& garbage) const {
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // nothing to do if starting with vertex measurement
  if (measurements.front()->isVertex()) {
    return;
  }

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG(" start of addLeadingMaterial: ");
    printMeasurements(measurements);
  }

  // fitted momentum at perigee - ignoring leading material effects
  double charge = 1.;
  double qOverP = fitParameters.qOverP();
  double p = 1. / qOverP;
  if (p < 0.) {
    charge = -1.;
    p = -p;
  }

  // check if leading scatterer(s) already present or need to be added (up to
  // delimiter)
  bool energyGain = false;
  bool haveDelimiter = false;
  const TrackSurfaceIntersection* intersection = nullptr;
  int leadingScatterers = 0;
  Trk::FitMeasurement* leadingScatterer = nullptr;
  for (auto* measurement : measurements) {
    if ((*measurement).isMaterialDelimiter()) {
      haveDelimiter = true;
    } else if ((*measurement).isScatterer()) {
      // count unfitted scatterers
      if (!(*measurement).numberDoF()) {
        ++leadingScatterers;
        leadingScatterer = measurement;
      } else {
        if (std::abs(1. / (*measurement).qOverP()) > p)
          energyGain = true;
        break;
      }
    }
  }

  // need to allocate leading scatterers
  if (haveDelimiter && !leadingScatterers) {
    // find first measurement after delimiter
    haveDelimiter = false;
    Amg::Vector3D endPosition = fitParameters.vertex();
    const Surface* firstMeasurementSurface = nullptr;
    Trk::FitMeasurement* leadingOutlier = nullptr;
    std::vector<Trk::FitMeasurement*> leadingOutliers;
    const Surface* surface = nullptr;
    for (auto* measurement : measurements) {
      if ((*measurement).isMaterialDelimiter()) {
        haveDelimiter = true;
        endPosition = (*measurement).position();
        surface = (*measurement).surface();
      } else if ((*measurement).isPositionMeasurement()) {
        if ((*measurement).isOutlier()) {
          if (!firstMeasurementSurface)
            leadingOutliers.push_back(measurement);
        } else {
          if (!firstMeasurementSurface && !intersection) {
            firstMeasurementSurface = (*measurement).surface();
            intersection = new TrackSurfaceIntersection(
                (*measurement).intersection(FittedTrajectory));
          }
          if (!haveDelimiter)
            continue;
          // surface	= (**m).surface();
        }
      } else if ((*measurement).isScatterer()) {
        if (!surface)
          continue;
        // FIXME: update p for Perigee in case of gain??
        if (std::abs(1. / (*measurement).qOverP()) > p)
          energyGain = true;
        break;
      }
    }

    // leading material identified by outwards extrapolateM from perigee to
    // delimiter
    // FIXME: currently only for indet
    // first create the fitted perigee (ignoring the leading material)
    Perigee* perigee =
        new Perigee(fitParameters.position(), p * fitParameters.direction(),
                    charge, fitParameters.vertex());
    bool haveMaterial = false;
    const std::vector<const TrackStateOnSurface*>* indetMaterial = nullptr;
    if (haveDelimiter && intersection && surface &&
        m_indetVolume->inside(endPosition)) {
      // debug
      if (msgLvl(MSG::VERBOSE)) {
        const Amg::Vector3D& direction = intersection->direction();
        const Amg::Vector3D& startPosition = intersection->position();
        ATH_MSG_VERBOSE(
            " addLeadingMaterial: using extrapolateM from distance "
            << direction.dot(fitParameters.position() - startPosition));
      }

      // extrapolateM from perigee to get leading material
      indetMaterial = extrapolatedMaterial(m_extrapolator, *perigee, *surface,
                                           alongMomentum, false,
                                           particleHypothesis, garbage);

      // check material found (expected at least for leading measurement)
      if (indetMaterial && !indetMaterial->empty()) {
        std::vector<const TrackStateOnSurface*>::const_reverse_iterator r =
            indetMaterial->rbegin();
        for (; r != indetMaterial->rend(); ++r) {
          // ignore trailing material
          if (!(**r).trackParameters() || !(**r).materialEffectsOnTrack() ||
              intersection->direction().dot(
                  (**r).trackParameters()->position() - endPosition) > 0.)
            continue;

          haveMaterial = true;
        }
      }
    } else {
      haveDelimiter = false;
    }

    // try again with back extrapolation if no leading material found
    if (haveDelimiter && !haveMaterial) {
      // debug
      ATH_MSG_VERBOSE(
          "        no leading material found with forward extrapolation"
          << ", try again with back extrapolation ");

      // clean up after previous attempt
      deleteMaterial(indetMaterial, garbage);
      indetMaterial = nullptr;

      std::vector<const TrackStateOnSurface*>* indetMaterialF = nullptr;
      const std::vector<const TrackStateOnSurface*>* indetMaterialR = nullptr;
      CurvilinearUVT uvt(intersection->direction());
      Amg::Vector2D localPos;
      PlaneSurface* plane = new PlaneSurface(intersection->position(), uvt);
      if (plane->globalToLocal(intersection->position(),
                               intersection->direction(), localPos)) {
        AtaPlane parameters(localPos[locR], localPos[locZ],
                            intersection->direction().phi(),
                            intersection->direction().theta(), qOverP, *plane);

        indetMaterialR = extrapolatedMaterial(
            m_extrapolator, parameters, perigee->associatedSurface(),
            oppositeMomentum, false, particleHypothesis, garbage);

        if (indetMaterialR && !indetMaterialR->empty()) {
          indetMaterialF = new std::vector<const TrackStateOnSurface*>;
          indetMaterialF->reserve(indetMaterialR->size());

          std::vector<const TrackStateOnSurface*>::const_reverse_iterator r =
              indetMaterialR->rbegin();
          for (; r != indetMaterialR->rend(); ++r) {
            indetMaterialF->push_back(*r);
          }

          for (r = indetMaterialF->rbegin(); r != indetMaterialF->rend(); ++r) {
            // ignore trailing material
            if (!(**r).trackParameters() || !(**r).materialEffectsOnTrack() ||
                intersection->direction().dot(
                    (**r).trackParameters()->position() - endPosition) > 0.)
              continue;

            haveMaterial = true;
          }
          indetMaterial = indetMaterialF;
          indetMaterialF = nullptr;
        }
      }
      delete indetMaterialR;
      delete plane;
    }

    // debug
    /** Commented out because indetMaterial is NULL at this point (coverity
     25126) if (haveDelimiter && msgLvl(MSG::VERBOSE))
       {
        Amg::Vector3D direction     = intersection->direction();
        Amg::Vector3D startPosition = intersection->position();
        double p1             = perigee->momentum().mag();

        for (std::vector<const TrackStateOnSurface*>::const_iterator s =
     indetMaterial->begin(); s != indetMaterial->end();
     ++s)
        {
        if (! (**s).trackParameters()) continue;
        double distance	= direction.dot((**s).trackParameters()->position() -
     startPosition); double deltaE	= 0.; double thickness= 0.; const
     MaterialEffectsOnTrack* materialEffects = dynamic_cast<const
     MaterialEffectsOnTrack*>((**s).materialEffectsOnTrack()); if
     ((**s).materialEffectsOnTrack())
        {
            if (materialEffects) deltaE =
     materialEffects->energyLoss()->deltaE(); thickness =
     (**s).materialEffectsOnTrack()->thicknessInX0();
        }
        else
        {
            ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed) << " delimiter:
     RZ"
                     << std::setw(9) << std::setprecision(3)
                     << (**s).trackParameters()->position().perp()
                     << std::setw(10) << std::setprecision(3)
                     << (**s).trackParameters()->position().z()
                     << "   distance " << std::setw(10) << std::setprecision(3)
                     << distance
                     << "   pt " << std::setw(8) << std::setprecision(3)
                     <<
     (**s).trackParameters()->momentum().perp()/Gaudi::Units::GeV); continue;
        }

        double p2		= (**s).trackParameters()->momentum().mag();
        ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed) << " material: RZ"
                 << std::setw(9) << std::setprecision(3)
                 << (**s).trackParameters()->position().perp()
                 << std::setw(10) << std::setprecision(3)
                 << (**s).trackParameters()->position().z()
                 << "   distance " << std::setw(10) << std::setprecision(3)
                 << distance
                 << "   pt " << std::setw(8) << std::setprecision(3)
                 << (**s).trackParameters()->momentum().perp()/Gaudi::Units::GeV
                 << "  X0thickness " << std::setw(8) << std::setprecision(4)
                 << thickness
                 << "  deltaE " << std::setw(8) << std::setprecision(4)
                 << deltaE
                 << " diffP " << std::setw(8) << std::setprecision(4) << p2 - p1
     ); p1			= p2;
        }
       }
     **/

    // create scatterer FitMeasurement's corresponding to leading material
    // (intersector running inwards to give parameters with qOverP update)
    FitMeasurement* leadingMeas = nullptr;
    if (indetMaterial && !indetMaterial->empty()) {
      std::vector<const TrackStateOnSurface*>::const_reverse_iterator r =
          indetMaterial->rbegin();
      for (; r != indetMaterial->rend(); ++r) {
        // ignore trailing material
        if (!(**r).trackParameters() || !(**r).materialEffectsOnTrack() ||
            intersection->direction().dot((**r).trackParameters()->position() -
                                          endPosition) > 0.)
          continue;

        // intersect material surface
        double eLoss = 0.;
        const MaterialEffectsOnTrack* materialEffects =
            dynamic_cast<const MaterialEffectsOnTrack*>(
                (**r).materialEffectsOnTrack());
        if (materialEffects) {
          eLoss = std::abs(materialEffects->energyLoss()->deltaE());
          if (energyGain)
            eLoss = -eLoss;
        }

        if (leadingScatterers++ || !firstMeasurementSurface) {
          if (m_useStepPropagator == 99) {
            const TrackSurfaceIntersection* newIntersectionSTEP =
                m_stepPropagator->intersectSurface(
                    ctx, (**r).trackParameters()->associatedSurface(),
                    intersection, qOverP,
                    Trk::MagneticFieldProperties(Trk::FullField), Trk::muon);
            intersection = m_intersector->intersectSurface(
                (**r).trackParameters()->associatedSurface(), intersection,
                qOverP);
            if (newIntersectionSTEP && intersection) {
              delete newIntersectionSTEP;
            }
          } else {
            intersection =
                m_useStepPropagator >= 1
                    ? m_stepPropagator->intersectSurface(
                          ctx, (**r).trackParameters()->associatedSurface(),
                          intersection, qOverP, m_stepField, Trk::muon)
                    : m_intersector->intersectSurface(
                          (**r).trackParameters()->associatedSurface(),
                          intersection, qOverP);
          }

          // quit if tracking problem
          if (!intersection) {
            intersection =
                &measurements.front()->intersection(FittedTrajectory);
            break;
          }
          leadingMeas =
              new FitMeasurement((**r).materialEffectsOnTrack(),
                                 Trk::ParticleMasses::mass[particleHypothesis],
                                 intersection->position());
        } else {
          // remove leadingOutliers - they will be reinserted wrt the
          // leadingScatterers
          for (std::vector<Trk::FitMeasurement*>::const_iterator l =
                   leadingOutliers.begin();
               l != leadingOutliers.end(); ++l) {
            leadingOutlier = leadingOutliers.back();
            measurements.erase(
                std::remove(measurements.begin(), measurements.end(), *l),
                measurements.end());
          }
          leadingMeas = new FitMeasurement(
              (**r).materialEffectsOnTrack()->thicknessInX0(), -eLoss,
              Trk::ParticleMasses::mass[particleHypothesis],
              intersection->position(), intersection->direction(), qOverP,
              firstMeasurementSurface);
          leadingScatterer = leadingMeas;
        }
        leadingMeas->intersection(FittedTrajectory, intersection);
        leadingMeas->qOverP(qOverP);

        // put corresponding scatterer FitMeasurement at front of list,
        // after re-inserting any intermediate leadingOutliers
        if (leadingOutlier) {
          double radius =
              leadingMeas->intersection(FittedTrajectory).position().perp();
          while (
              leadingOutlier &&
              leadingOutlier->intersection(FittedTrajectory).position().perp() >
                  radius) {
            leadingOutliers.pop_back();
            measurements.insert(measurements.begin(), leadingOutlier);
            if (!leadingOutliers.empty()) {
              leadingOutlier = leadingOutliers.back();
            } else {
              leadingOutlier = nullptr;
            }
          }
        }

        ATH_MSG_DEBUG(" push_front(leadingMeas) ");

        measurements.insert(measurements.begin(), leadingMeas);

        // update momentum for energy loss
        if (materialEffects) {
          if (charge > 0.) {
            qOverP = 1. / (1. / qOverP + eLoss);
          } else {
            qOverP = 1. / (1. / qOverP - eLoss);
          }
        }
      }
    }

    // final step to give intersection at perigee surface plus memory management
    if (leadingMeas) {
      if (m_useStepPropagator == 99) {
        const TrackSurfaceIntersection* newIntersectionSTEP =
            m_stepPropagator->intersectSurface(
                ctx, perigee->associatedSurface(), intersection, qOverP,
                Trk::MagneticFieldProperties(Trk::FullField), Trk::muon);
        intersection = m_intersector->intersectSurface(
            perigee->associatedSurface(), intersection, qOverP);
        if (newIntersectionSTEP && intersection) {
          delete newIntersectionSTEP;
        }
      } else {
        intersection =
            m_useStepPropagator >= 1
                ? m_stepPropagator->intersectSurface(
                      ctx, perigee->associatedSurface(), intersection, qOverP,
                      m_stepField, Trk::muon)
                : m_intersector->intersectSurface(perigee->associatedSurface(),
                                                  intersection, qOverP);
      }
    } else {
      delete intersection;
      intersection = nullptr;
    }
    delete perigee;
    deleteMaterial(indetMaterial, garbage);
    indetMaterial = nullptr;
  }

  // integrate X0, energy loss and contribution to covariance (from leading
  // scatterer towards perigee)
  AmgSymMatrix(5) leadingCovariance;
  leadingCovariance.setZero();
  if (leadingScatterers) {
    double leadingScattering = 0.;
    double previousScattering = 0.;
    double leadingX0Integral = 0.;
    std::vector<Trk::FitMeasurement*>::reverse_iterator m =
        measurements.rbegin();
    while (*m != leadingScatterer)
      ++m;
    for (; m != measurements.rend(); ++m) {
      if (!(**m).isScatterer())
        continue;
      const MaterialEffectsOnTrack* materialEffects =
          dynamic_cast<const MaterialEffectsOnTrack*>((**m).materialEffects());
      if (!materialEffects)
        continue;

      // set the scattering angle and X0Integral
      leadingX0Integral += (**m).materialEffects()->thicknessInX0();
      double logTerm = 1.0 + m_scatteringLogCoeff * std::log(leadingX0Integral);
      leadingScattering = leadingX0Integral * logTerm * logTerm;
      double scatteringAngle =
          m_scatteringConstant *
          std::sqrt(leadingScattering - previousScattering);
      previousScattering = leadingScattering;
      (**m).scatteringAngle(scatteringAngle, leadingX0Integral);

      // the scattering contribution to the covariance at perigee
      double angleSquared = 1. / (**m).weight();
      double deltaR = ((**m).intersection(FittedTrajectory).position() -
                       fitParameters.vertex())
                          .perp();
      double sinThetaSquared =
          (**m).intersection(FittedTrajectory).direction().perp2();
      angleSquared *= angleSquared / sinThetaSquared;

      // transverse projection
      leadingCovariance(0, 0) += deltaR * deltaR * angleSquared;
      leadingCovariance(0, 2) -= deltaR * angleSquared;
      leadingCovariance(2, 0) = leadingCovariance(0, 2);
      leadingCovariance(2, 2) += angleSquared;

      // longitudinal projection (to get z: remember dcotTh/dTh = -1/sin*sin)
      leadingCovariance(1, 1) +=
          deltaR * deltaR * angleSquared / sinThetaSquared;
      leadingCovariance(1, 3) += deltaR * angleSquared;
      leadingCovariance(3, 1) = leadingCovariance(1, 3);
      leadingCovariance(3, 3) += angleSquared * sinThetaSquared;
    }
  }

  // if leading material has just been added
  if (intersection) {
    fitParameters.update(intersection->position(), intersection->direction(),
                         qOverP, leadingCovariance);
    delete intersection;
  }
  // or pre-existing leading material
  else {
    fitParameters.update(fitParameters.position(), fitParameters.direction(),
                         qOverP, leadingCovariance);
  }

  // debug
  if (msgLvl(MSG::DEBUG)) {
    if (!haveDelimiter)
      ATH_MSG_VERBOSE(" addLeadingMaterial: ");
    printMeasurements(measurements);
  }
}

void MaterialAllocator::allocateMaterial(
    std::vector<FitMeasurement*>& measurements,
    ParticleHypothesis particleHypothesis, FitParameters& fitParameters,
    const TrackParameters& startParameters, Garbage_t& garbage) const {
  // different strategies used for indet and muon spectrometer
  indetMaterial(measurements, particleHypothesis, startParameters, garbage);
  if (!m_extrapolator.empty())
    spectrometerMaterial(measurements, particleHypothesis, fitParameters,
                         startParameters, garbage);

  // debug
  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE(" allocateMaterial: ");
    printMeasurements(measurements);
  }
}

void MaterialAllocator::initializeScattering(
    std::vector<FitMeasurement*>& measurements) const {
  // loop over scatterers to include log term corresponding to integral
  // thickness
  bool integrate = false;
  double previousScattering = 0.;
  double X0Integral = 0.;

  std::vector<Trk::FitMeasurement*>::iterator m = measurements.begin();

  // start integration after any leading material
  while (!(**m).isPositionMeasurement() || (**m).isOutlier())
    ++m;

  for (; m != measurements.end(); ++m) {
    if (integrate && (**m).isPositionMeasurement() && !(**m).isOutlier()) {
      // restart integration for log term
      integrate = false;
      previousScattering = 0.;
      X0Integral = 0.;
    }
    if ((**m).isScatterer()) {
      if (integrate) {
        // reset if measurement closely following
        std::vector<Trk::FitMeasurement*>::iterator next = m;
        if (++next != measurements.end() && !(**next).hitOnTrack() &&
            (**next).isPositionMeasurement() && !(**next).isOutlier()) {
          Amg::Vector3D position =
              (**next).intersection(FittedTrajectory).position();
          if (((**m).intersection(FittedTrajectory).position() - position)
                  .mag() < 1. * Gaudi::Units::mm)
            integrate = false;
        }

        if (!integrate) {
          // restart integration for log term
          previousScattering = 0.;
          X0Integral = 0.;
        }
      }

      integrate = true;
      double thicknessInX0 = (**m).materialEffects()->thicknessInX0();
      if ((**m).materialEffects()->thicknessInX0() < 0.) {
        ATH_MSG_WARNING("thicknessInX0 smaller or equal to zero "
                        << (**m).materialEffects()->thicknessInX0() << " "
                        << *(**m).materialEffects());
        thicknessInX0 = 1e-6;
      }
      X0Integral += thicknessInX0;
      double logTerm = 1.;
      if (X0Integral > 0.) {
        logTerm = 1.0 + m_scatteringLogCoeff * std::log(X0Integral);
      } else {
        ATH_MSG_WARNING("X0Integral smaller or equal to zero "
                        << X0Integral << " thicknessInX0 "
                        << (**m).materialEffects()->thicknessInX0() << " "
                        << *(**m).materialEffects());
        X0Integral = 1e-6;
      }
      double scattering = X0Integral * logTerm * logTerm;
      double angle =
          m_scatteringConstant * std::sqrt(scattering - previousScattering);
      previousScattering = scattering;
      (**m).numberDoF(2);
      (**m).scatteringAngle(angle, X0Integral);
    }
  }
}

std::vector<const TrackStateOnSurface*>*
MaterialAllocator::leadingSpectrometerTSOS(
    const TrackParameters& spectrometerParameters, Garbage_t& garbage) const {
  const EventContext& ctx = Gaudi::Hive::currentContext();
  const Trk::TrackingVolume* spectrometerEntrance = getSpectrometerEntrance();
  if (!spectrometerEntrance)
    return nullptr;
  // check input parameters are really in the spectrometer
  if (m_calorimeterVolume->inside(spectrometerParameters.position()))
    return nullptr;

  std::unique_ptr<const TrackParameters> entranceParameters(
      m_extrapolator->extrapolateToVolume(ctx, spectrometerParameters,
                                          *spectrometerEntrance, anyDirection,
                                          Trk::nonInteracting));
  if (!entranceParameters) {
    ATH_MSG_VERBOSE(
        std::setiosflags(std::ios::fixed)
        << "leadingSpectrometerTSOS: no material found from RZ" << std::setw(9)
        << std::setprecision(3) << spectrometerParameters.position().perp()
        << std::setw(10) << std::setprecision(3)
        << spectrometerParameters.position().z() << "  with p " << std::setw(8)
        << std::setprecision(3)
        << spectrometerParameters.momentum().mag() / Gaudi::Units::GeV);
    return nullptr;
  }

  const Surface& entranceSurface = entranceParameters->associatedSurface();
  std::unique_ptr<const std::vector<const TrackStateOnSurface*>>
      extrapolatedTSOS(extrapolatedMaterial(
          m_extrapolator, spectrometerParameters, entranceSurface, anyDirection,
          false, Trk::muon, garbage));

  if (!extrapolatedTSOS || extrapolatedTSOS->empty() ||
      !extrapolatedTSOS->front()->trackParameters()) {
    ATH_MSG_VERBOSE(
        std::setiosflags(std::ios::fixed)
        << "leadingSpectrometerTSOS: no material found from RZ" << std::setw(9)
        << std::setprecision(3) << spectrometerParameters.position().perp()
        << std::setw(10) << std::setprecision(3)
        << spectrometerParameters.position().z() << "  with p " << std::setw(8)
        << std::setprecision(3)
        << spectrometerParameters.momentum().mag() / Gaudi::Units::GeV);
    return nullptr;
  }

  std::vector<std::unique_ptr<FitMeasurement>> leadingMeasurements;
  std::unique_ptr<std::vector<const TrackStateOnSurface*>> leadingTSOS =
      std::make_unique<std::vector<const TrackStateOnSurface*>>();
  leadingTSOS->reserve(extrapolatedTSOS->size());
  double outgoingEnergy = spectrometerParameters.momentum().mag();
  double particleMass = Trk::ParticleMasses::mass[Trk::muon];
  for (const auto* s : *extrapolatedTSOS) {
    if (!(*s).trackParameters())
      continue;
    std::unique_ptr<FitMeasurement> measurement(
        measurementFromTSOS(*s, outgoingEnergy, particleMass));
    outgoingEnergy = (*s).trackParameters()->momentum().mag();

    if (measurement) {
      leadingMeasurements.emplace(leadingMeasurements.begin(),
                                  std::move(measurement));
    } else {
      leadingTSOS->push_back((*s).clone());
    }
  }

  // convert back to TSOS
  for (const auto& m : leadingMeasurements)
    leadingTSOS->emplace_back(new TrackStateOnSurface(
        nullptr, nullptr, m->materialEffects()->uniqueClone()));

  deleteMaterial(extrapolatedTSOS.release(), garbage);

  // debug
  if (msgLvl(MSG::VERBOSE) && !leadingTSOS->empty()) {
    ATH_MSG_VERBOSE(
        std::setiosflags(std::ios::fixed)
        << "leadingSpectrometerTSOS: from RZ" << std::setw(9)
        << std::setprecision(3) << spectrometerParameters.position().perp()
        << std::setw(10) << std::setprecision(3)
        << spectrometerParameters.position().z() << "  with p " << std::setw(8)
        << std::setprecision(3)
        << spectrometerParameters.momentum().mag() / Gaudi::Units::GeV);
    // printMeasurements(leadingMeasurements);
  }
  return leadingTSOS.release();
}

void MaterialAllocator::orderMeasurements(
    std::vector<FitMeasurement*>& measurements, Amg::Vector3D startDirection,
    Amg::Vector3D startPosition) const {
  // order measurements
  ATH_MSG_VERBOSE(" measurement ordering with startDirection phi "
                  << startDirection.phi() << "  theta "
                  << startDirection.theta());

  // note: preserve original order for close double measurements (such as
  // crossed eta/phi)
  double previousDistance = -m_orderingTolerance;
  std::vector<std::pair<double, FitMeasurement*>> measurementOrder;
  std::vector<std::pair<double, FitMeasurement*>> originalOrder;
  for (auto* measurement : measurements) {
    double distance = startDirection.dot(
        (*measurement).intersection(FittedTrajectory).position() -
        startPosition);
    if (distance < previousDistance)
      distance += m_orderingTolerance;
    previousDistance = distance - m_orderingTolerance;
    measurementOrder.emplace_back(distance, measurement);
    originalOrder.emplace_back(distance, measurement);
  }
  std::sort(measurementOrder.begin(), measurementOrder.end(),
            compareByDistance());
  std::vector<std::pair<double, FitMeasurement*>>::const_iterator orig =
      originalOrder.begin();
  bool shouldReorder = false;
  if (m_allowReordering)
    measurements.erase(measurements.begin(), measurements.end());

  for (std::vector<std::pair<double, FitMeasurement*>>::const_iterator order =
           measurementOrder.begin();
       order != measurementOrder.end(); ++order, ++orig) {
    if (m_allowReordering) {
      measurements.push_back((*order).second);
    }

    // signal if reordering would help
    // FIXME add tolerance
    if ((*order).second == (*orig).second ||
        std::abs((*order).first - (*orig).first) < m_orderingTolerance)
      continue;
    shouldReorder = true;
    // ATH_MSG_INFO( "  reorder distance " << (*order).first - (*orig).first );
  }

  if (shouldReorder) {
    if (m_allowReordering) {
      ATH_MSG_DEBUG(" measurements have been reordered ");
    } else {
      ATH_MSG_DEBUG(" measurement reordering would improve the track fit ");
    }
  }
}

bool MaterialAllocator::reallocateMaterial(
    std::vector<FitMeasurement*>& measurements, FitParameters& parameters,
    Garbage_t& garbage) const {
  ATH_MSG_DEBUG(" reallocateSpectrometerMaterial ");

  int n = 0;
  for (auto& measurement : measurements) {
    if (!(*measurement).isMaterialDelimiter())
      continue;

    double distance =
        ((*measurement).intersection(FittedTrajectory).position() -
         (*measurement).position())
            .mag();
    ATH_MSG_INFO(
        std::setiosflags(std::ios::fixed)
        << std::setw(5) << ++n << std::setw(10) << std::setprecision(3)
        << distance << "  " << (*measurement).type() << std::setw(10)
        << std::setprecision(1)
        << (*measurement).intersection(FittedTrajectory).position().perp()
        << std::setw(9) << std::setprecision(4)
        << (*measurement).intersection(FittedTrajectory).position().phi()
        << std::setw(10) << std::setprecision(1)
        << (*measurement).intersection(FittedTrajectory).position().z());
  }

  // put iterator at MS entrance
  double qOverP = 0;
  ATH_MSG_INFO("qOverP " << qOverP);

  std::vector<Trk::FitMeasurement*>::iterator m = measurements.begin();
  for (; m != measurements.end(); ++m) {
    if (m_calorimeterVolume->inside((**m).position())) {
      // save qOverP for following use with spectrometer
      if ((**m).materialEffects())
        qOverP = (**m).qOverP();
      if ((**m).isMaterialDelimiter())
        ATH_MSG_INFO(" at material delimiter");
      ATH_MSG_INFO("qOverP " << qOverP);
    } else {
      if ((**m).isMaterialDelimiter())
        ATH_MSG_INFO(" at material delimiter");
      break;
    }
  }

  // allocate material from outside inwards
  double mass = Trk::ParticleMasses::mass[Trk::muon];
  MsgStream log(msgSvc(), name());
  const TrackParameters* trackParameters =
      parameters.trackParameters(log, *measurements.back());

  // protect the momentum to avoid excessive Eloss
  Amg::VectorX parameterVector = trackParameters->parameters();
  double Emax = 50000.;
  if (parameterVector[Trk::qOverP] == 0.) {
    parameterVector[Trk::qOverP] = 1. / Emax;
  } else {
    if (std::abs(parameterVector[Trk::qOverP]) * Emax < 1)
      parameterVector[Trk::qOverP] = trackParameters->charge() / Emax;
  }

  // correct track parameters for high momentum track (otherwise Eloss is too
  // large)
  trackParameters =
      (trackParameters->associatedSurface())
          .createUniqueTrackParameters(
              parameterVector[Trk::loc1], parameterVector[Trk::loc2],
              parameterVector[Trk::phi], parameterVector[Trk::theta],
              parameterVector[Trk::qOverP], std::nullopt)
          .release();

  for (std::vector<Trk::FitMeasurement*>::reverse_iterator r =
           measurements.rbegin();
       r != measurements.rend(); ++r) {
    if (!(**r).isMaterialDelimiter())
      continue;
    const std::vector<const TrackStateOnSurface*>* spectrometerMaterial =
        extrapolatedMaterial(m_extrapolator, *trackParameters, *(**r).surface(),
                             oppositeMomentum, false, Trk::muon, garbage);

    if (spectrometerMaterial && !spectrometerMaterial->empty()) {
      //      for (std::vector<const
      //      TrackStateOnSurface*>::const_reverse_iterator s =
      //           spectrometerMaterial->rbegin();
      //       s != spectrometerMaterial->rend();
      //       ++s)
      //      {
      //      if (! (**s).trackParameters() || ! (**s).materialEffectsOnTrack())
      //      continue; ATH_MSG_INFO( " material " <<
      //      (**s).trackParameters()->position() );
      //      }

      std::pair<FitMeasurement*, FitMeasurement*> fms =
          materialAggregation(*spectrometerMaterial, measurements, mass);
      delete fms.first;
      delete fms.second;
      // ATH_MSG_INFO( " delete TSOS " );

      for (const auto* s : *spectrometerMaterial)
        delete s;
    }
    // ATH_MSG_INFO( " delete material " );
    delete spectrometerMaterial;
    delete trackParameters;

    MsgStream log(msgSvc(), name());
    trackParameters = parameters.trackParameters(log, **r);
  }

  delete trackParameters;

  // erase materialDelimiters
  for (m = measurements.begin(); m != measurements.end(); ++m) {
    if (!(**m).isMaterialDelimiter())
      continue;
    delete *m;
    std::vector<Trk::FitMeasurement*>::iterator n = m;
    --m;
    measurements.erase(n);
  }

  return true;
}

void MaterialAllocator::addSpectrometerDelimiters(
    std::vector<FitMeasurement*>& measurements) const {
  // insert delimiters representing station limits for later material
  // aggregation
  //   preBreak:  put delimiter upstream of measurement
  //   postBreak: put delimiter downstream of previous measurement
  // FIXME: modify aggregation function: no aggregation in EC toroid region
  // (~11m)
  // FIXME: single scatterer in outer delimited pair
  // printMeasurements(measurements);
  FitMeasurement* previous = nullptr;
  double previousDistance = 0.;

  Amg::Vector3D startDirection;
  Amg::Vector3D startPosition;
  Amg::Vector3D referenceDirection;
  Amg::Vector3D referencePosition;
  double referencePhi = 0.;
  int index = 1;
  for (std::vector<FitMeasurement*>::iterator m = measurements.begin();
       m != measurements.end(); ++m, ++index) {
    // skip 'non'-measurements
    if (!(**m).isPositionMeasurement() || (**m).isPseudo())
      continue;

    // material delimiters in MS follow the entrance break which should be
    // already present
    Amg::Vector3D position = (**m).position();
    if (m_calorimeterVolume->inside(position))
      continue;

    // break can be before measurement or after previous measurement
    bool preBreak = false;
    bool postBreak = false;
    double distance = 0.;
    if (!previous) {
      // preBreak at first measurement in MS
      preBreak = true;
      referenceDirection = (**m).intersection(FittedTrajectory).direction();
      referencePosition = (**m).intersection(FittedTrajectory).position();
      referencePhi = position.phi();
      startDirection = referenceDirection;
      startPosition = referencePosition;
    } else {
      // post and pre breaks for cluster/drift transition,
      //                         large gap between measurements,
      //                         sector overlap
      distance = referenceDirection.dot(
          (**m).intersection(FittedTrajectory).position() - referencePosition);
      if (((**m).isDrift() && !previous->isDrift()) ||
          (!(**m).isDrift() && previous->isDrift()) ||
          distance > m_stationMaxGap ||
          ((**m).isDrift() &&
           std::abs(position.phi() - referencePhi) > m_sectorMaxPhi)) {
        preBreak = true;
        if (distance > previousDistance + m_scattererMinGap)
          postBreak = true;
      }
    }

    if (!(preBreak || postBreak)) {
      previous = *m;
      previousDistance = distance;
      continue;
    }

    //  ///////
    //  msg(MSG::INFO) << std::setiosflags(std::ios::fixed)
    //             << " index" << std::setw(3) << index+1
    //             << "  at " << std::setw(10) << std::setprecision(1)
    //             << startDirection.dot(
    //             (**m).intersection(FittedTrajectory).position() -
    //             startPosition)
    //             << std::setw(10) << std::setprecision(1) << distance
    //             << std::setw(9)  << std::setprecision(4)
    //             << std::abs(position.phi() - referencePhi);
    //  if (preBreak) msg() << " preBreak ";
    //  if (postBreak) msg() << " postBreak ";
    //  if ((**m).isDrift()) msg() << " isDrift ";
    //  msg() << endmsg;
    //  ///////

    if (postBreak && previous) {
      // if (distance < offset) offset = 0.5*distance;
      FitMeasurement* current = *m;
      while (*m != previous)
        --m;
      FitMeasurement* delimiter = new FitMeasurement(
          (**m).intersection(FittedTrajectory), 0.5 * m_scattererMinGap);
      m = measurements.insert(++m, delimiter);
      while (*m != current)
        ++m;
    }
    if (preBreak) {
      double offset = -0.5 * m_scattererMinGap;
      if (distance - previousDistance < m_scattererMinGap)
        offset = 0.5 * (previousDistance - distance);

      FitMeasurement* delimiter =
          new FitMeasurement((**m).intersection(FittedTrajectory), offset);
      m = measurements.insert(m, delimiter);
      ++m;
    }
    previous = *m;
    previousDistance = 0.;
    referenceDirection = (**m).intersection(FittedTrajectory).direction();
    referencePosition = (**m).intersection(FittedTrajectory).position();
    referencePhi = position.phi();
  }
  //     orderMeasurements(measurements,startDirection,startPosition);
  //     printMeasurements(measurements);
}

void MaterialAllocator::deleteMaterial(
    const std::vector<const TrackStateOnSurface*>* material,
    Garbage_t& garbage) {
  if (material) {
    for (const TrackStateOnSurface* m : *material) {
      garbage.push_back(std::unique_ptr<const TrackStateOnSurface>(m));
    }
    delete material;
  }
}

const std::vector<const TrackStateOnSurface*>*
MaterialAllocator::extrapolatedMaterial(
    const ToolHandle<IExtrapolator>& extrapolator,
    const TrackParameters& parameters, const Surface& surface,
    PropDirection dir, const BoundaryCheck& boundsCheck,
    ParticleHypothesis particleHypothesis, Garbage_t& garbage) const {
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // fix up material duplication appearing after recent TrackingGeometry
  // speed-up
  const std::vector<const TrackStateOnSurface*>* TGMaterial =
      extrapolator->extrapolateM(ctx, parameters, surface, dir, boundsCheck,
                                 particleHypothesis);

  if (!TGMaterial || TGMaterial->empty())
    return TGMaterial;

  std::vector<const TrackStateOnSurface*>* duplicates = nullptr;
  std::vector<const TrackStateOnSurface*>* material =
      new std::vector<const TrackStateOnSurface*>;
  material->reserve(TGMaterial->size());
  std::vector<const TrackStateOnSurface*>::const_iterator tg =
      TGMaterial->begin();
  material->push_back(*tg);
  ++tg;
  for (; tg != TGMaterial->end(); ++tg) {
    const TrackStateOnSurface* TSOS = material->back();
    double separation = 0.;
    if (TSOS->trackParameters())
      separation = (TSOS->trackParameters()->position() -
                    (**tg).trackParameters()->position())
                       .mag();

    if (separation > 0.001 * Gaudi::Units::mm) {
      material->push_back(*tg);
    } else {
      ATH_MSG_VERBOSE(
          std::setiosflags(std::ios::fixed)
          << "        duplicate: RZ" << std::setw(9) << std::setprecision(3)
          << (**tg).trackParameters()->position().perp() << std::setw(10)
          << std::setprecision(3) << (**tg).trackParameters()->position().z());
      if (!duplicates)
        duplicates = new std::vector<const TrackStateOnSurface*>;
      duplicates->push_back(*tg);
    }
  }

  delete TGMaterial;
  if (duplicates)
    deleteMaterial(duplicates, garbage);
  return material;
}

void MaterialAllocator::indetMaterial(
    std::vector<FitMeasurement*>& measurements,
    ParticleHypothesis particleHypothesis,
    const TrackParameters& startParameters, Garbage_t& garbage) const {
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // gather material between first and last measurements inside indet volume
  // allow a few mm radial tolerance around first&last measurements for their
  // associated material
  double tolerance =
      10. * Gaudi::Units::mm / startParameters.momentum().unit().perp();

  // loop over measurements to define portions of track needing indet material
  double endIndetDistance = 0.;
  FitMeasurement* endIndetMeasurement = nullptr;
  double qOverP = startParameters.charge() / startParameters.momentum().mag();

  Amg::Vector3D startDirection = startParameters.momentum().unit();
  Amg::Vector3D startPosition = startParameters.position();
  const TrackParameters* parameters = &startParameters;

  std::vector<Trk::FitMeasurement*>::iterator m = measurements.begin();
  if ((**m).isVertex())
    ++m;
  for (; m != measurements.end(); ++m) {
    if ((**m).isOutlier())
      continue;
    if (m_indetVolume->inside((**m).position())) {
      // quit if pre-existing indet material
      if ((**m).isScatterer()) {
        if (parameters != &startParameters)
          delete parameters;  // parameters was 'newed'
        return;
      }

      // use first measurement at a plane surface to create starting parameters
      if (!(**m).isPositionMeasurement())
        continue;
      if (!endIndetMeasurement && (**m).hasIntersection(FittedTrajectory) &&
          ((**m).surface()->type() == Trk::SurfaceType::Plane ||
           (**m).surface()->type() == Trk::SurfaceType::Disc)) {
        const TrackSurfaceIntersection* intersection =
            &(**m).intersection(FittedTrajectory);
        Amg::Vector3D offset = intersection->direction() * tolerance;
        CurvilinearUVT uvt(intersection->direction());
        PlaneSurface plane(intersection->position() - offset, uvt);

        if (m_useStepPropagator == 99) {
          const TrackSurfaceIntersection* newIntersectionSTEP =
              m_stepPropagator->intersectSurface(
                  ctx, plane, intersection, qOverP,
                  Trk::MagneticFieldProperties(Trk::FullField), Trk::muon);
          intersection =
              m_intersector->intersectSurface(plane, intersection, qOverP);
          if (newIntersectionSTEP && intersection) {
            //                    double dist =
            //                    1000.*(newIntersectionSTEP->position()-intersection->position()).mag();
            //                    std::cout << " iMat 3 distance STEP and
            //                    Intersector " << dist << std::endl;
            //                    if(dist>10.) std::cout << " iMat 3 ALARM
            //                    distance STEP and Intersector " << dist <<
            //                    std::endl;
            delete newIntersectionSTEP;
          } else {
            //                    if(intersection) std::cout << " iMat 3 ALARM
            //                    STEP did not intersect! " << std::endl;
          }
        } else {
          intersection = m_useStepPropagator >= 1
                             ? m_stepPropagator->intersectSurface(
                                   ctx, plane, intersection, qOverP,
                                   m_stepField, Trk::muon)
                             : m_intersector->intersectSurface(
                                   plane, intersection, qOverP);
        }
        Amg::Vector2D localPos;
        if (intersection &&
            plane.globalToLocal(intersection->position(),
                                intersection->direction(), localPos)) {
          if (parameters != &startParameters)
            delete parameters;  // parameters was 'newed' already
          parameters = new AtaPlane(
              localPos[locR], localPos[locZ], intersection->direction().phi(),
              intersection->direction().theta(), qOverP, plane);
          startDirection = intersection->direction();
          startPosition = intersection->position();
        }
        delete intersection;
      }

      // save the last indet measurement, signal any out-of-order meas
      double distance = startDirection.dot(
          (**m).intersection(FittedTrajectory).position() - startPosition);
      if (!endIndetMeasurement || distance > endIndetDistance) {
        endIndetDistance = distance;
        endIndetMeasurement = *m;
      }
    } else {  // outside indet
      break;
    }
  }
  if (!endIndetMeasurement) {
    if (parameters != &startParameters)
      delete parameters;
    return;
  }

  ATH_MSG_DEBUG(" indetMaterial: ALARM no material found on track");

  // allocate indet material from TrackingGeometry
  Amg::Vector3D endPosition =
      endIndetMeasurement->intersection(FittedTrajectory).position();
  startDirection = (endPosition - startPosition).unit();
  endIndetDistance =
      startDirection.dot(endPosition - startPosition) + tolerance;
  ATH_MSG_VERBOSE(" indetMaterial: using extrapolateM out to distance "
                  << endIndetDistance);
  const std::vector<const TrackStateOnSurface*>* indetMaterial =
      extrapolatedMaterial(m_extrapolator, *parameters,
                           *endIndetMeasurement->surface(), alongMomentum,
                           false, particleHypothesis, garbage);

  if (parameters != &startParameters)
    delete parameters;
  if (!indetMaterial || indetMaterial->empty()) {
    deleteMaterial(indetMaterial, garbage);
    return;
  }

  // insert the material into the measurement list
  // ignore trailing material - with a few mm radial tolerance
  std::vector<const Surface*> surfaces;
  surfaces.reserve(indetMaterial->size());
  std::vector<const TrackStateOnSurface*>::const_iterator indetMaterialEnd =
      indetMaterial->begin();
  int trailing = indetMaterial->size();
  for (std::vector<const TrackStateOnSurface*>::const_iterator s =
           indetMaterial->begin();
       s != indetMaterial->end(); ++s, --trailing) {
    if ((**s).trackParameters()) {
      if (startDirection.dot((**s).trackParameters()->position() -
                             startPosition) < endIndetDistance) {
        indetMaterialEnd = s;
        ++indetMaterialEnd;
      } else {
        ATH_MSG_VERBOSE(" indetMaterial: "
                        << trailing
                        << " trailing TSOS (after last measurement)");
        break;
      }
    }
  }

  // return in case of extrapolateM problem
  if (indetMaterialEnd == indetMaterial->begin()) {
    // extrapolateM finds no material on track !!
    m_messageHelper->printWarning(1);
    deleteMaterial(indetMaterial, garbage);
    return;
  }

  // debug
  if (msgLvl(MSG::VERBOSE)) {
    double p1 = indetMaterial->front()->trackParameters()->momentum().mag();

    for (std::vector<const TrackStateOnSurface*>::const_iterator s =
             indetMaterial->begin();
         s != indetMaterialEnd; ++s) {
      if (!(**s).trackParameters())
        continue;
      double distance = startDirection.dot((**s).trackParameters()->position() -
                                           startPosition);
      double deltaE = 0.;
      double thickness = 0.;
      const MaterialEffectsOnTrack* materialEffects =
          dynamic_cast<const MaterialEffectsOnTrack*>(
              (**s).materialEffectsOnTrack());
      if ((**s).materialEffectsOnTrack()) {
        if (materialEffects)
          deltaE = materialEffects->energyLoss()->deltaE();
        thickness = (**s).materialEffectsOnTrack()->thicknessInX0();
      }
      double p2 = (**s).trackParameters()->momentum().mag();
      ATH_MSG_VERBOSE(
          std::setiosflags(std::ios::fixed)
          << "         material: RZ" << std::setw(9) << std::setprecision(3)
          << (**s).trackParameters()->position().perp() << std::setw(10)
          << std::setprecision(3) << (**s).trackParameters()->position().z()
          << "   distance " << std::setw(10) << std::setprecision(3) << distance
          << "   pt " << std::setw(8) << std::setprecision(3)
          << (**s).trackParameters()->momentum().perp() / Gaudi::Units::GeV
          << "  X0thickness " << std::setw(8) << std::setprecision(4)
          << thickness << "  deltaE " << std::setw(8) << std::setprecision(4)
          << deltaE << " diffP " << std::setw(8) << std::setprecision(4)
          << p2 - p1);
      p1 = p2;
    }
  }

  // firstly: add the material belonging to each measurement layer (and skip
  // leading material)
  FitMeasurement* leadingDelimiter = nullptr;
  Amg::Vector3D nextMomentum(0., 0., 0.);
  Amg::Vector3D nextPosition(0., 0., 0.);
  m = measurements.begin();
  std::vector<const TrackStateOnSurface*>::const_iterator s =
      indetMaterial->begin();
  for (; m != measurements.end(); ++m) {
    if (*m == endIndetMeasurement || s == indetMaterialEnd)
      break;
    if (!leadingDelimiter && (**m).isOutlier())
      continue;

    Amg::Vector3D direction = (**m).intersection(FittedTrajectory).direction();
    Amg::Vector3D position = (**m).intersection(FittedTrajectory).position();
    double closestDistance = direction.dot(position - startPosition);
    const MaterialEffectsOnTrack* materialEffects = nullptr;
    const TrackParameters* materialParameters = nullptr;
    const Surface* materialSurface = nullptr;

    // find the closest material TSOS (inside radial tolerance)
    for (; s != indetMaterialEnd; ++s) {
      if (!dynamic_cast<const MaterialEffectsOnTrack*>(
              (**s).materialEffectsOnTrack()) ||
          !(**s).trackParameters())
        continue;
      nextMomentum = (**s).trackParameters()->momentum();
      nextPosition = (**s).trackParameters()->position();
      double distance = direction.dot(nextPosition - position);

      // increasing distance - break when past minimum
      if (distance > closestDistance)
        break;

      // downstream material - check not significantly closer to following
      // measurement
      //  (material too early is better than too late)
      if (distance > 0.) {
        ++m;
        double nextDistance = direction.dot(
            (**s).trackParameters()->position() - (**m).position());
        --m;
        if (std::abs(nextDistance) < distance && distance > tolerance) {
          if (s != indetMaterial->begin())
            --s;
          materialSurface = nullptr;
          break;
        }
        closestDistance = distance;
      } else {
        closestDistance = -distance;
      }

      // when upstream of leading break any previous surface is going to be
      // ignored
      if (!leadingDelimiter && materialSurface)
        surfaces.push_back(materialSurface);

      materialEffects = dynamic_cast<const MaterialEffectsOnTrack*>(
          (**s).materialEffectsOnTrack());
      materialParameters = (**s).trackParameters();
      materialSurface = &materialParameters->associatedSurface();
    }

    // skip if the material has not been allocated to a measurement surface
    if (!materialSurface)
      continue;

    // or if it's already been allocated upstream
    if (!surfaces.empty() && materialSurface == surfaces.back())
      continue;

    // skip leading material during the fit (up to and including first
    // measurement) insert an materialDelimiter so the leading material can be
    // allocated after the fit converges
    if (!leadingDelimiter) {
      position = 0.5 * (materialParameters->position() + nextPosition);
      direction = (materialParameters->momentum() + nextMomentum).unit();
      TrackSurfaceIntersection breakIntersection(position, direction, 0.);
      leadingDelimiter = new FitMeasurement(breakIntersection, 0.);
      while (*m != endIndetMeasurement &&
             direction.dot((**m).intersection(FittedTrajectory).position() -
                           position) < 0.)
        ++m;
      m = measurements.insert(m, leadingDelimiter);
      surfaces.push_back(materialSurface);
      continue;
    }

    // check inside tolerance
    if (closestDistance > tolerance)
      continue;

    // insert material at measurement surface
    const std::bitset<MaterialEffectsBase::NumberOfMaterialEffectsTypes>
        typePattern;
    std::unique_ptr<Trk::EnergyLoss> energyLoss = nullptr;
    if (materialEffects->energyLoss()) {
      energyLoss = std::unique_ptr<Trk::EnergyLoss>(
          materialEffects->energyLoss()->clone());
    }
    MaterialEffectsOnTrack* meot = new MaterialEffectsOnTrack(
        materialEffects->thicknessInX0(), std::move(energyLoss),
        *(**m).surface(), typePattern);
    const TrackSurfaceIntersection* intersection =
        new TrackSurfaceIntersection((**m).intersection(FittedTrajectory));
    if (++m == measurements.end())
      --m;
    m = measurements.insert(
        m,
        new FitMeasurement(meot, Trk::ParticleMasses::mass[particleHypothesis],
                           intersection->position()));
    (**m).intersection(FittedTrajectory, intersection);
    (**m).qOverP(materialParameters->parameters()[Trk::qOverP]);
    (**m).setMaterialEffectsOwner();
    surfaces.push_back(materialSurface);
  }

  // secondly: insert remaining material between measurement layers
  m = measurements.begin();
  int im = 0;
  ATH_MSG_VERBOSE(" measurements.size() "
                  << measurements.size() << " surfaces.size() "
                  << surfaces.size() << " indetMaterial->size() "
                  << indetMaterial->size());
  std::vector<const Surface*>::const_iterator r = surfaces.begin();
  for (s = indetMaterial->begin(); s != indetMaterialEnd; ++s) {
    if (!(**s).trackParameters() || !(**s).materialEffectsOnTrack())
      continue;

    if (r != surfaces.end() &&
        **r == (**s).trackParameters()->associatedSurface()) {
      ++r;
      continue;
    }

    double distance =
        startDirection.dot((**s).trackParameters()->position() - startPosition);

    ATH_MSG_VERBOSE("	startPosition " << startPosition.perp() << " z "
                                        << startPosition.z());
    ATH_MSG_VERBOSE(
        "	(**m).intersection(FittedTrajectory).position() measurement "
        "position r "
        << (**m).intersection(FittedTrajectory).position().perp() << " z "
        << (**m).intersection(FittedTrajectory).position().z());
    ATH_MSG_VERBOSE(
        "	(**s).trackParameters()->position() material surface position "
        "r "
        << (**s).trackParameters()->position().perp() << " z "
        << (**s).trackParameters()->position().z());
    ATH_MSG_VERBOSE(" distance material surface " << distance);

    bool endIndet = false;
    while (distance >
           startDirection.dot((**m).intersection(FittedTrajectory).position() -
                              startPosition)) {
      if (*m == endIndetMeasurement) {
        if (*m != measurements.back()) {
          ++m;
          ++im;
          ATH_MSG_VERBOSE(" measurements.back() im " << im);
        }
        ATH_MSG_VERBOSE(" break im " << im);
        endIndet = true;
        break;
      }
      if (*m != measurements.back()) {
        ++m;
        ++im;
        ATH_MSG_VERBOSE(
            " loop im "
            << im
            << "	(**m).intersection(FittedTrajectory).position() "
               "measurement position r "
            << (**m).intersection(FittedTrajectory).position().perp() << " z "
            << (**m).intersection(FittedTrajectory).position().z());
      } else {
        break;
      }
    }
    ATH_MSG_VERBOSE(
        " im " << im << " distance measurement "
               << startDirection.dot(
                      (**m).intersection(FittedTrajectory).position() -
                      startPosition));
    ATH_MSG_VERBOSE(
        " (**m).intersection(FittedTrajectory).position() measurement position "
        "r "
        << (**m).intersection(FittedTrajectory).position().perp() << " z "
        << (**m).intersection(FittedTrajectory).position().z());

    m = measurements.insert(
        m, new FitMeasurement((**s).materialEffectsOnTrack(),
                              Trk::ParticleMasses::mass[particleHypothesis],
                              (**s).trackParameters()->position()));
    const TrackSurfaceIntersection* intersection = new TrackSurfaceIntersection(
        (**s).trackParameters()->position(),
        (**s).trackParameters()->momentum().unit(), 0.);
    (**m).intersection(FittedTrajectory, intersection);
    (**m).qOverP((**s).trackParameters()->parameters()[Trk::qOverP]);
    ATH_MSG_VERBOSE(" successfull insertion ");
    if (endIndet)
      --m;
  }

  m = measurements.begin();
  im = 0;
  for (; m != measurements.end(); ++m) {
    if (!leadingDelimiter && (**m).isOutlier())
      continue;

    Amg::Vector3D position = (**m).intersection(FittedTrajectory).position();
    ++im;
    ATH_MSG_VERBOSE(" im " << im << " position R " << position.perp() << " z "
                           << position.z());
  }

  // memory management
  deleteMaterial(indetMaterial, garbage);

  ATH_MSG_VERBOSE(" finished indetMaterial ");
}

std::pair<FitMeasurement*, FitMeasurement*>
MaterialAllocator::materialAggregation(
    const std::vector<const TrackStateOnSurface*>& material,
    std::vector<FitMeasurement*>& /*measurements*/,
    double /*particleMass*/) const {
  // aggregation possible in indet and MS. Frequent occurrence in MS
  ATH_MSG_INFO("segment material aggregation " << material.size());
  FitMeasurement* measurement1 = nullptr;
  FitMeasurement* measurement2 = nullptr;
  if (material.empty())
    return std::pair<FitMeasurement*, FitMeasurement*>(measurement1,
                                                       measurement2);

  Amg::Vector3D* referencePosition = nullptr;

  int adjacentScatterers = 0;
  std::vector<FitMeasurement*> aggregateScatterers;
  bool haveAggregation = false;
  //     bool makeAggregation		= false;
  //     double maxDistance			= 0.;
  for (std::vector<const TrackStateOnSurface*>::const_reverse_iterator tsos =
           material.rbegin();
       tsos != material.rend(); ++tsos) {
    if (!(**tsos).trackParameters() || !(**tsos).materialEffectsOnTrack())
      continue;
    ++adjacentScatterers;
    if (!referencePosition)
      referencePosition =
          new Amg::Vector3D((**tsos).trackParameters()->position());
    double distance =
        ((**tsos).trackParameters()->position() - *referencePosition).mag();
    double weight = (**tsos).materialEffectsOnTrack()->thicknessInX0();

    ATH_MSG_INFO(" material position " << (**tsos).trackParameters()->position()
                                       << "   distance " << distance
                                       << "   thickness " << 100. * weight);
  }

  // if 2 or less selected TSOS: just set scatterers on TSOS
  if (adjacentScatterers < 3) {
  }

  // in case of aggregation: insert aggregateScatterers onto track
  if (haveAggregation) {
  }

  delete referencePosition;
  return std::pair<FitMeasurement*, FitMeasurement*>(measurement1,
                                                     measurement2);
}

void MaterialAllocator::materialAggregation(
    std::vector<FitMeasurement*>& measurements, double particleMass) const {
  // Aggregate when at least 2 scatterers exist between delimiter pair.
  //
  // First loop over measurements to create aggregateScatterer vector.
  // This contains any aggregated scatterers along with sparse existing
  // scatterers which haven't been aggregated. The remaining existing scatterers
  // (those which have been aggregated) are flagged for discard (via the outlier
  // flag).

  // currently aggregation only performed for MS:
  Amg::Vector3D referencePosition =
      measurements.back()->intersection(FittedTrajectory).position();
  if (m_calorimeterVolume->inside(referencePosition))
    return;

  Amg::Vector3D referenceDirection =
      measurements.back()->intersection(FittedTrajectory).direction();
  int adjacentScatterers = 0;
  std::vector<FitMeasurement*> aggregateScatterers;
  bool haveAggregation = false;
  bool makeAggregation = false;
  double maxDistance = 0.;
  FitMeasurement* measurement1 = nullptr;
  FitMeasurement* measurement2 = nullptr;
  double totalDistance = 0.;
  double totalDistanceSq = 0.;
  double totalEnergyDeposit = 0.;
  double totalThickness = 0.;
  std::vector<FitMeasurement*>::reverse_iterator start;
  std::vector<FitMeasurement*>::reverse_iterator previous =
      measurements.rbegin();
  for (std::vector<FitMeasurement*>::reverse_iterator m = measurements.rbegin();
       m != measurements.rend(); ++m) {
    if ((**m).isScatterer())
      aggregateScatterers.push_back(*m);
    if (m_calorimeterVolume->inside((**m).position())) {
      if (!adjacentScatterers)
        continue;
      makeAggregation = true;
    }
    //    if (m_calorimeterVolume->inside((**m).position())
    //        && ! m_indetVolume->inside((**m).position())) continue;
    // look for adjacent scatterers
    else if (adjacentScatterers) {
      if ((**m).isScatterer()) {
        Amg::Vector3D position =
            (**m).intersection(FittedTrajectory).position();
        double distance =
            std::abs(referenceDirection.dot(position - referencePosition));
        if (distance < maxDistance) {
          ++adjacentScatterers;
          double weight = (**m).radiationThickness();
          totalDistance += weight * distance;
          totalDistanceSq += weight * distance * distance;
          totalEnergyDeposit += (**m).energyLoss();
          totalThickness += weight;
          if (*m == measurements.front())
            makeAggregation = true;
          //          ATH_MSG_INFO(std::setiosflags(std::ios::fixed)
          //               << "         distance "
          //               << std::setw(8) << std::setprecision(0)
          //               << distance
          //               << "  adjacentScatterers " << adjacentScatterers );
        } else if (adjacentScatterers > 1) {
          makeAggregation = true;
        } else {
          adjacentScatterers = 0;
        }
      } else if (!(**m).isMaterialDelimiter()) {
        previous = m;
        continue;
      } else if (adjacentScatterers > 1) {
        makeAggregation = true;
      } else {
        adjacentScatterers = 0;
      }
    }

    if (makeAggregation) {
      //      double dist =
      //          ((**m).intersection(FittedTrajectory).position() -
      //          referencePosition).mag();
      //      ATH_MSG_INFO(std::setiosflags(std::ios::fixed)
      //           << " makeAggregation: reference R,Z "
      //           << std::setw(8) << std::setprecision(0)
      //           << referencePosition.perp()
      //           << std::setw(8) << std::setprecision(0)
      //           << referencePosition.z()
      //           << "  current R,Z "
      //           << std::setw(8) << std::setprecision(0)
      //           << (**m).intersection(FittedTrajectory).position().perp()
      //           << std::setw(8) << std::setprecision(0)
      //           << (**m).intersection(FittedTrajectory).position().z()
      //           << "  adjacentScatterers " << std::setw(2)
      //           << adjacentScatterers
      //           << "   distance "
      //           << std::setw(8) << std::setprecision(0)
      //           << dist );
      double meanDistance = totalDistance / totalThickness;
      double rmsDistance = 0.;
      double meanSquare =
          totalDistanceSq / totalThickness - meanDistance * meanDistance;
      if (meanSquare > 0.)
        rmsDistance = std::sqrt(meanSquare);
      double gap = 2. * rmsDistance;
      if (adjacentScatterers > 2 || gap < m_scattererMinGap) {
        double distance1 = meanDistance - rmsDistance;
        double distance2 = meanDistance + rmsDistance;
        if (gap < m_scattererMinGap)
          distance2 = meanDistance;
        Amg::Vector3D position =
            (**m).intersection(FittedTrajectory).position();
        double distance =
            std::abs(referenceDirection.dot(position - referencePosition));
        //      ATH_MSG_INFO(std::setiosflags(std::ios::fixed)
        //               << "     distance1 "
        //               << std::setw(8) << std::setprecision(0)
        //               << distance1
        //               << "   distance2 "
        //               << std::setw(8) << std::setprecision(0)
        //               << distance2
        //               << "   distance "
        //               << std::setw(8) << std::setprecision(0)
        //               << distance );
        if (distance2 > distance || distance1 < 0.) {
          //          msg() << "  distance out of bounds: range " << distance
          //            << " to " << 0. << endmsg;
        } else {
          FitMeasurement* after = nullptr;
          FitMeasurement* before = *start;
          double previousDistance = 0.;
          haveAggregation = true;

          for (std::vector<Trk::FitMeasurement*>::reverse_iterator s = start;
               s != measurements.rend(); ++s) {
            if (!(**s).isScatterer())
              continue;
            Amg::Vector3D position =
                (**s).intersection(FittedTrajectory).position();
            double distance =
                std::abs(referenceDirection.dot(position - referencePosition));
            if (!measurement1 && distance > distance1 &&
                gap > m_scattererMinGap) {
              after = *s;
              double separation = distance - previousDistance;
              double fractionAfter =
                  (distance1 - previousDistance) / separation;
              double fractionBefore = (distance - distance1) / separation;
              //              ATH_MSG_INFO( std::setiosflags(std::ios::fixed)
              //                    << "         distance "
              //                    << std::setw(8) << std::setprecision(0)
              //                    << distance<< "   fraction before "
              //                    << std::setw(6) << std::setprecision(2)
              //                    << fractionBefore
              //                    << "   fraction after "
              //                    << std::setw(6) << std::setprecision(2)
              //                    << fractionAfter );
              position = fractionBefore *
                             before->intersection(FittedTrajectory).position() +
                         fractionAfter *
                             after->intersection(FittedTrajectory).position();
              Amg::Vector3D direction =
                  fractionBefore *
                      before->intersection(FittedTrajectory).direction() +
                  fractionAfter *
                      after->intersection(FittedTrajectory).direction();
              double qOverP = fractionBefore * before->qOverP() +
                              fractionAfter * after->qOverP();
              measurement1 = new FitMeasurement(
                  0.5 * totalThickness, -0.5 * totalEnergyDeposit, particleMass,
                  position, direction, qOverP);
            }

            if (distance > distance2) {
              after = *s;
              double separation = distance - previousDistance;
              double fractionAfter =
                  (distance2 - previousDistance) / separation;
              double fractionBefore = (distance - distance2) / separation;
              //              ATH_MSG_INFO( std::setiosflags(std::ios::fixed)
              //                    << "         distance "
              //                    << std::setw(8) << std::setprecision(0)
              //                    << distance<< "   fraction before "
              //                    << std::setw(6) << std::setprecision(2)
              //                    << fractionBefore
              //                    << "   fraction after "
              //                    << std::setw(6) << std::setprecision(2)
              //                    << fractionAfter << endmsg );
              position = fractionBefore *
                             before->intersection(FittedTrajectory).position() +
                         fractionAfter *
                             after->intersection(FittedTrajectory).position();
              Amg::Vector3D direction =
                  fractionBefore *
                      before->intersection(FittedTrajectory).direction() +
                  fractionAfter *
                      after->intersection(FittedTrajectory).direction();
              double qOverP = fractionBefore * before->qOverP() +
                              fractionAfter * after->qOverP();
              if (measurement1) {
                measurement2 = new FitMeasurement(
                    0.5 * totalThickness, -0.5 * totalEnergyDeposit,
                    particleMass, position, direction, qOverP);
              } else {
                measurement2 = new FitMeasurement(
                    totalThickness, -totalEnergyDeposit, particleMass, position,
                    direction, qOverP);
              }
              bool keepCurrentMeas = false;
              if ((**m).isScatterer() && *m != measurements.front()) {
                keepCurrentMeas = true;
                aggregateScatterers.pop_back();
              }
              while (adjacentScatterers--) {
                aggregateScatterers.back()->setOutlier();
                aggregateScatterers.pop_back();
              }
              if (measurement1)
                aggregateScatterers.push_back(measurement1);
              if (measurement2)
                aggregateScatterers.push_back(measurement2);
              if (keepCurrentMeas)
                aggregateScatterers.push_back(*m);
              measurement1 = nullptr;
              measurement2 = nullptr;
              break;
            }
            before = *s;
            previousDistance = distance;
          }
        }
      }
      adjacentScatterers = 0;
      makeAggregation = false;
    }

    // new candidate for merging
    if ((**m).isScatterer() && !adjacentScatterers &&
        !m_calorimeterVolume->inside((**m).position())) {
      adjacentScatterers = 1;
      double weight = (**m).radiationThickness();
      referencePosition =
          (**previous).intersection(FittedTrajectory).position();
      referenceDirection =
          (**previous).intersection(FittedTrajectory).direction();
      Amg::Vector3D position = (**m).intersection(FittedTrajectory).position();
      double distance =
          std::abs(referenceDirection.dot(position - referencePosition));
      maxDistance = distance + 2. * Gaudi::Units::meter;
      start = m;
      totalDistance = weight * distance;
      totalDistanceSq = weight * distance * distance;
      totalEnergyDeposit = (**m).energyLoss();
      totalThickness = weight;
      //      ATH_MSG_INFO(std::setiosflags(std::ios::fixed)
      //           << "         distance "
      //           << std::setw(8) << std::setprecision(0)
      //           << distance
      //           << "  adjacentScatterers " << adjacentScatterers );
    }
    previous = m;
  }

  // avoid possible leak
  delete measurement1;
  // delete measurement2;	// redundant!

  // in case of aggregation: insert the aggregateScatterers into the measurement
  // list (second loop over measurements)
  if (haveAggregation) {
    referencePosition =
        measurements.back()->intersection(FittedTrajectory).position();
    referenceDirection =
        (referencePosition -
         measurements.front()->intersection(FittedTrajectory).position())
            .unit();
    std::vector<Trk::FitMeasurement*>::reverse_iterator s =
        aggregateScatterers.rbegin();
    Amg::Vector3D scattererPosition =
        (**s).intersection(FittedTrajectory).position();
    double scattererDistance =
        std::abs(referenceDirection.dot(scattererPosition - referencePosition));
    for (std::vector<Trk::FitMeasurement*>::iterator m = measurements.begin();
         m != measurements.end(); ) {
      // insert scatterers from aggregrate vector
      Amg::Vector3D position = (**m).intersection(FittedTrajectory).position();
      double distance =
          std::abs(referenceDirection.dot(position - referencePosition));
      while (distance <= scattererDistance && s != aggregateScatterers.rend()) {
        m = measurements.insert(m, *s);
        ++m;
        if (++s != aggregateScatterers.rend()) {
          scattererPosition = (**s).intersection(FittedTrajectory).position();
          scattererDistance = std::abs(
              referenceDirection.dot(scattererPosition - referencePosition));
        }
      }
      if ((**m).isScatterer()) {
        // delete the scatterer if it has been aggregated
        if ((**m).isOutlier())
          delete *m;
        // in any case it must be removed from the list to avoid double counting
        m = measurements.erase(m);
      }
      else {
        ++m;
      }
    }
  }

  // verbose table of fit measurements including material
  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE("  finished material aggregation: ");
    int n = 0;
    Amg::Vector3D startPosition =
        measurements.front()->intersection(FittedTrajectory).position();
    Amg::Vector3D startDirection =
        measurements.front()->intersection(FittedTrajectory).direction();
    for (auto* measurement : measurements) {
      Amg::Vector3D position =
          (*measurement).intersection(FittedTrajectory).position();
      double distance = std::abs(startDirection.dot(position - startPosition));
      msg(MSG::VERBOSE) << std::setiosflags(std::ios::fixed) << std::setw(5)
                        << ++n << std::setw(10) << std::setprecision(3)
                        << distance << "  " << (*measurement).type();
      if ((*measurement).isOutlier()) {
        msg() << "  outlier ";
      } else if ((*measurement).materialEffects()) {
        msg() << std::setw(8) << std::setprecision(3)
              << (*measurement).materialEffects()->thicknessInX0() << "  ";
      } else {
        msg() << "          ";
      }
      if (!(*measurement).isMaterialDelimiter()) {
        msg() << std::setw(10) << std::setprecision(1)
              << (*measurement).position().perp() << std::setw(9)
              << std::setprecision(4) << (*measurement).position().phi()
              << std::setw(10) << std::setprecision(1)
              << (*measurement).position().z();
      }
      msg() << endmsg;
    }
  }

  // loops to erase material delimiters and set energy gain when appropriate
  bool energyGain = false;
  for (auto& measurement : measurements) {
    if ((*measurement).materialEffects() && (*measurement).numberDoF() &&
        (*measurement).energyLoss() < 0.)
      energyGain = true;
  }

  if (energyGain) {
    for (auto& measurement : measurements) {
      if ((*measurement).materialEffects())
        (*measurement).setEnergyGain();
    }
  }
}

FitMeasurement* MaterialAllocator::measurementFromTSOS(
    const TrackStateOnSurface& tsos, double outgoingEnergy,
    double particleMass) {
  if (!tsos.trackParameters() || !tsos.materialEffectsOnTrack())
    return nullptr;

  double deltaE = outgoingEnergy - tsos.trackParameters()->momentum().mag();
  double thicknessInX0 = tsos.materialEffectsOnTrack()->thicknessInX0();
  Amg::Vector3D position = tsos.trackParameters()->position();
  Amg::Vector3D direction = tsos.trackParameters()->momentum().unit();
  double qOverP = 1. / outgoingEnergy;
  if (tsos.trackParameters()->charge() < 0)
    qOverP = -qOverP;

  return new FitMeasurement(thicknessInX0, deltaE, particleMass, position,
                            direction, qOverP);
}

void MaterialAllocator::printMeasurements(
    std::vector<FitMeasurement*>& measurements) const {
  ATH_MSG_VERBOSE(
      "measurements and material:  distance        X0   deltaE            E    "
      "    pT"
      << "           R      phi         Z  DoF      phi    theta");

  if (measurements.empty())
    return;

  std::vector<Trk::FitMeasurement*>::iterator m = measurements.begin();
  while (m != measurements.end() && !(**m).isPositionMeasurement())
    ++m;
  if (m == measurements.end())
    m = measurements.begin();

  Amg::Vector3D direction = (**m).intersection(FittedTrajectory).direction();
  Amg::Vector3D startPosition = (**m).intersection(FittedTrajectory).position();
  int scatterers = 0;
  int leadingMaterial = 0;
  double leadingX0 = 0.;
  double sumX0 = 0.;
  double leadingELoss = 0.;
  double sumELoss = 0.;
  int n = 0;
  for (auto& measurement : measurements) {
    double distance =
        direction.dot((*measurement).intersection(FittedTrajectory).position() -
                      startPosition);
    msg(MSG::VERBOSE) << std::setiosflags(std::ios::fixed) << std::setw(5)
                      << ++n << "  " << (*measurement).type() << std::setw(11)
                      << std::setprecision(3) << distance;
    if ((*measurement).isOutlier()) {
      msg() << "  outlier " << std::setw(44);
    } else if ((*measurement).materialEffects()) {
      double deltaE = 0.;
      const MaterialEffectsOnTrack* materialEffects =
          dynamic_cast<const MaterialEffectsOnTrack*>(
              (*measurement).materialEffects());
      if (materialEffects && materialEffects->energyLoss())
        deltaE = materialEffects->energyLoss()->deltaE();
      if ((*measurement).isEnergyDeposit())
        deltaE = -deltaE;
      msg() << std::setw(10) << std::setprecision(3)
            << (*measurement).materialEffects()->thicknessInX0() << std::setw(9)
            << std::setprecision(1) << deltaE << "  ";
      if (distance > 0.) {
        ++scatterers;
        sumX0 += (*measurement).materialEffects()->thicknessInX0();
        sumELoss -= deltaE;
      } else {
        ++leadingMaterial;
        leadingX0 += (*measurement).materialEffects()->thicknessInX0();
        leadingELoss -= deltaE;
      }

      if ((*measurement).qOverP()) {
        msg() << std::setw(11) << std::setprecision(4)
              << 1. / std::abs((*measurement).qOverP() * Gaudi::Units::GeV)
              << std::setw(10) << std::setprecision(3)
              << (*measurement)
                         .intersection(FittedTrajectory)
                         .direction()
                         .perp() /
                     ((*measurement).qOverP() * Gaudi::Units::GeV)
              << std::setw(12);
      }
    } else {
      msg() << std::setw(54);
    }
    if ((*measurement).isMaterialDelimiter()) {
      msg() << std::setprecision(1)
            << (*measurement).intersection(FittedTrajectory).position().perp()
            << std::setw(9) << std::setprecision(4)
            << (*measurement).intersection(FittedTrajectory).position().phi()
            << std::setw(10) << std::setprecision(1)
            << (*measurement).intersection(FittedTrajectory).position().z()
            << std::setw(14) << std::setprecision(4)
            << (*measurement).intersection(FittedTrajectory).direction().phi()
            << std::setw(9) << std::setprecision(4)
            << (*measurement).intersection(FittedTrajectory).direction().theta()
            << endmsg;
    } else {
      msg() << std::setprecision(1) << (*measurement).position().perp()
            << std::setw(9) << std::setprecision(4)
            << (*measurement).position().phi() << std::setw(10)
            << std::setprecision(1) << (*measurement).position().z()
            << std::setw(5) << (*measurement).numberDoF() << endmsg;
    }
  }

  // fix counting at allocation stage
  if (!scatterers) {
    scatterers = leadingMaterial;
    leadingMaterial = 0;
  }

  ATH_MSG_DEBUG(
      " material: "
      << scatterers << " (" << leadingMaterial
      << ") fitted scattering centres (leading material centres) giving "
      << std::setiosflags(std::ios::fixed) << std::setw(8)
      << std::setprecision(3) << sumX0 << " (" << std::setw(8)
      << std::setprecision(3) << leadingX0 << ")"
      << " X0 and " << std::setw(8) << std::setprecision(3)
      << sumELoss / Gaudi::Units::GeV << " (" << std::setw(8)
      << std::setprecision(3) << leadingELoss / Gaudi::Units::GeV << ")"
      << " GeV Eloss");
}

void MaterialAllocator::spectrometerMaterial(
    std::vector<FitMeasurement*>& measurements,
    ParticleHypothesis particleHypothesis, FitParameters& fitParameters,
    const TrackParameters& startParameters, Garbage_t& garbage) const {
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // return if no MS measurement
  if (m_calorimeterVolume->inside(measurements.back()->position()))
    return;

  // check that the spectrometer measurements are ordered and that material
  // allocation is required
  Amg::Vector3D startDirection = startParameters.momentum().unit();
  Amg::Vector3D startPosition = startParameters.position();
  bool haveMaterial = false;
  bool haveLeadingMaterial = false;
  bool reorderMS = false;
  bool reorderID = false;
  bool firstMSHit = false;
  double previousDistance = 0.;
  double previousDistanceR = 0.;
  double previousDistanceZ = 0.;
  double minDistanceID = 0.;
  double minDistanceMS = 0.;
  double minRDistanceMS = 0.;
  double minZDistanceMS = 0.;
  std::vector<Trk::FitMeasurement*>::iterator m = measurements.begin();
  for (; m != measurements.end(); ++m) {
    Amg::Vector3D position = (**m).intersection(FittedTrajectory).position();
    Amg::Vector3D positionSurf = (**m).surface()->center();
    Amg::Vector3D positionMst = startPosition;
    if ((**m).measurementBase())
      positionMst = (**m).measurementBase()->globalPosition();
    double distance = startDirection.dot(position - startPosition);
    double distanceR = std::hypot(positionMst.x() - startPosition.x(),
                                  positionMst.y() - startPosition.y());
    double distanceZ = (positionMst.z() - startPosition.z());
    if (startDirection.z() < 0)
      distanceZ = -distanceZ;
    if (!m_calorimeterVolume->inside(position) ||
        !m_calorimeterVolume->inside(positionSurf)) {
      if (distance - previousDistance < -m_orderingTolerance) {
        reorderMS = true;
        if (distance - previousDistance < minDistanceMS) {
          minDistanceMS = distance - previousDistance;
          minRDistanceMS = distanceR - previousDistanceR;
          minZDistanceMS = distanceZ - previousDistanceZ;
        }
      }
      if ((**m).isScatterer())
        haveMaterial = true;
      if ((**m).measurementBase() && !firstMSHit) {
        firstMSHit = true;
      }
      if ((**m).isScatterer() && !firstMSHit)
        haveLeadingMaterial = true;
    } else {
      if (distance - previousDistance < -m_orderingTolerance) {
        reorderID = true;
        if (distance - previousDistance < minDistanceID)
          minDistanceID = distance - previousDistance;
      }
    }
    previousDistance = distance;
    previousDistanceZ = distanceZ;
    previousDistanceR = distanceR;
  }

  if (reorderMS && (minRDistanceMS > -m_orderingTolerance ||
                    minZDistanceMS > -m_orderingTolerance)) {
    //    3D distance of the intersection is problematic but the R or Z distance
    //    of the measurementBase is fine we should not reorder

    reorderMS = false;
  }

  //    if(!m_allowReordering) {
  if (reorderMS && fabs(minDistanceMS) > -2.)
    ATH_MSG_DEBUG(" reorder MS part of track with minimum distance "
                  << minDistanceMS << " minRDistanceMS " << minRDistanceMS
                  << " minZDistanceMS " << minZDistanceMS);
  if (reorderID && fabs(minDistanceID) > -2.)
    ATH_MSG_DEBUG(" reorder ID part of track with minimum distance "
                  << minDistanceID);
  //    }

  if (reorderMS || reorderID) {
    if (msgLvl(MSG::DEBUG))
      printMeasurements(measurements);
  }

  if (!haveLeadingMaterial && haveMaterial) {
    ATH_MSG_WARNING(
        " MS part of track has no leading material in front of first MS hit ");
  }

  if (reorderMS)
    orderMeasurements(measurements, startDirection, startPosition);

  // nothing to do if spectrometer material already exists
  if (haveMaterial)
    return;

  ATH_MSG_DEBUG(
      " spectrometerMaterial: ALARM no material found on track can happen for "
      "MuGirl");

  // material has to be added: need inner and outer TrackParameters
  FitMeasurement* innerMeasurement = nullptr;
  FitMeasurement* outerMeasurement = nullptr;
  for (m = measurements.begin(); m != measurements.end(); ++m) {
    if (!(**m).isPositionMeasurement() || (**m).isOutlier())
      continue;
    Amg::Vector3D position = (**m).intersection(FittedTrajectory).position();
    if (m_calorimeterVolume->inside(position))
      continue;
    if (innerMeasurement) {
      outerMeasurement = *m;
    } else {
      innerMeasurement = *m;
    }
  }
  if (!outerMeasurement)
    return;

  // insert delimiters
  addSpectrometerDelimiters(measurements);

  const Trk::TrackingVolume* spectrometerEntrance = getSpectrometerEntrance();
  if (!spectrometerEntrance)
    return;

  // entranceParameters are at the MS entrance surface (0 if perigee downstream)
  TrackSurfaceIntersection* entranceIntersection = nullptr;
  std::unique_ptr<const TrackParameters> entranceParameters;
  MsgStream log(msgSvc(), name());
  if (m_calorimeterVolume->inside(startParameters.position())) {
    std::unique_ptr<const TrackParameters> innerParameters(
        fitParameters.trackParameters(log, *innerMeasurement, false));
    if (!innerParameters)
      innerParameters.reset(startParameters.clone());
    entranceParameters = m_extrapolator->extrapolateToVolume(
        ctx, *innerParameters, *spectrometerEntrance, anyDirection,
        Trk::nonInteracting);
    if (entranceParameters) {
      startDirection = entranceParameters->momentum().unit();
      startPosition = entranceParameters->position();
      entranceIntersection = new TrackSurfaceIntersection(
          entranceParameters->position(), entranceParameters->momentum().unit(),
          0.);
      std::vector<Trk::FitMeasurement*>::iterator e = measurements.begin();
      FitMeasurement* entranceDelimiter =
          new FitMeasurement(*entranceIntersection, 0.);
      for (m = measurements.begin(); m != measurements.end(); ++m) {
        if (!m_calorimeterVolume->inside((**m).position()))
          break;
        e = m;
      }

      // insert a material delimiter at the start of the spectrometer (or at
      // perigee if in MS)
      e = measurements.insert(++e, entranceDelimiter);
      delete entranceIntersection;
    } else {
      // did not find MS entrance surface - no MS material taken into account
      m_messageHelper->printWarning(3);
      return;
    }
  }

  // insert a material delimiter after the last measurement (endParameters)
  std::unique_ptr<const TrackParameters> outerParameters(
      fitParameters.trackParameters(log, *outerMeasurement, false));
  if (!outerParameters)
    outerParameters.reset(startParameters.clone());
  const Surface& endSurface = *measurements.back()->surface();
  std::unique_ptr<const TrackParameters> endParameters(
      m_extrapolator->extrapolate(ctx, *outerParameters, endSurface,
                                  anyDirection, false, particleHypothesis));

  if (!endParameters) {
    endParameters =
        m_extrapolator->extrapolate(ctx, *outerParameters, endSurface,
                                    anyDirection, false, Trk::nonInteracting);

    if (!endParameters) {
      // failed extrapolation
      m_messageHelper->printWarning(4);
      endParameters = std::move(outerParameters);
    }
  }
  // insert delimiter
  const TrackSurfaceIntersection endIntersection(
      endParameters->position(), endParameters->momentum().unit(), 0.);
  FitMeasurement* endBreak =
      new FitMeasurement(endIntersection, 20. * Gaudi::Units::mm);
  measurements.push_back(endBreak);

  double endSpectrometerDistance = startDirection.dot(
      measurements.back()->intersection(FittedTrajectory).position() -
      startPosition);
  const std::vector<const TrackStateOnSurface*>* spectrometerMaterial = nullptr;

  // protect the momentum to avoid excessive Eloss

  Amg::VectorX parameterVector = endParameters->parameters();
  double Emax = 50000.;
  if (parameterVector[Trk::qOverP] == 0.) {
    parameterVector[Trk::qOverP] = 1. / Emax;
  } else {
    if (std::abs(parameterVector[Trk::qOverP]) * Emax < 1)
      parameterVector[Trk::qOverP] = endParameters->charge() / Emax;
  }

  // correct track parameters for high momentum track (otherwise Eloss is too
  // large)
  endParameters =
      endParameters->associatedSurface().createUniqueTrackParameters(
          parameterVector[Trk::loc1], parameterVector[Trk::loc2],
          parameterVector[Trk::phi], parameterVector[Trk::theta],
          parameterVector[Trk::qOverP], std::nullopt);

  if (entranceParameters) {
    const Surface& entranceSurface = entranceParameters->associatedSurface();
    spectrometerMaterial =
        extrapolatedMaterial(m_extrapolator, *endParameters, entranceSurface,
                             anyDirection, false, Trk::muon, garbage);
  } else {
    const Surface& entranceSurface = startParameters.associatedSurface();
    spectrometerMaterial =
        extrapolatedMaterial(m_extrapolator, *endParameters, entranceSurface,
                             anyDirection, false, Trk::muon, garbage);
  }

  // debug
  if (msgLvl(MSG::VERBOSE) && spectrometerMaterial &&
      !spectrometerMaterial->empty()) {
    ATH_MSG_VERBOSE(" spectrometerMaterial: "
                    << "using extrapolateM inwards from outermost measurement");
    double p1 = 0.;
    if (spectrometerMaterial->front()->trackParameters())
      p1 = spectrometerMaterial->front()->trackParameters()->momentum().mag();
    for (const auto* ss : *spectrometerMaterial) {
      if (!(*ss).trackParameters() || !(*ss).materialEffectsOnTrack())
        continue;
      double distance = startDirection.dot((*ss).trackParameters()->position() -
                                           startPosition);
      double deltaE = 0.;
      double thickness = (*ss).materialEffectsOnTrack()->thicknessInX0();
      const MaterialEffectsOnTrack* materialEffects =
          dynamic_cast<const MaterialEffectsOnTrack*>(
              (*ss).materialEffectsOnTrack());
      if (materialEffects && materialEffects->energyLoss())
        deltaE = materialEffects->energyLoss()->deltaE();
      double p2 = (*ss).trackParameters()->momentum().mag();
      ATH_MSG_VERBOSE(
          std::setiosflags(std::ios::fixed)
          << "         material: RZ" << std::setw(9) << std::setprecision(3)
          << (*ss).trackParameters()->position().perp() << std::setw(10)
          << std::setprecision(3) << (*ss).trackParameters()->position().z()
          << "   distance " << std::setw(10) << std::setprecision(3) << distance
          << "   pt " << std::setw(8) << std::setprecision(3)
          << (*ss).trackParameters()->momentum().perp() / Gaudi::Units::GeV
          << "  X0thickness " << std::setw(8) << std::setprecision(4)
          << thickness << "  deltaE " << std::setw(8) << std::setprecision(4)
          << deltaE << " diffP " << std::setw(8) << std::setprecision(4)
          << p2 - p1);
      p1 = p2;
    }
  }

  // insert the material into the measurement list
  if (!spectrometerMaterial || spectrometerMaterial->empty()) {
    m_messageHelper->printWarning(5);
    delete spectrometerMaterial;
    spectrometerMaterial = nullptr;
  } else {
    std::vector<const TrackStateOnSurface*>::const_reverse_iterator s =
        spectrometerMaterial->rbegin();
    std::vector<FitMeasurement*> material;
    double particleMass = Trk::ParticleMasses::mass[particleHypothesis];
    material.reserve(spectrometerMaterial->size());
    std::vector<FitMeasurement*>::iterator m = measurements.begin();
    for (; s != spectrometerMaterial->rend();) {
      const TrackStateOnSurface& tsos = **s;
      while (++s != spectrometerMaterial->rend() && !(**s).trackParameters())
        ;

      double outgoingEnergy = 0.;
      if (s != spectrometerMaterial->rend()) {
        outgoingEnergy = (**s).trackParameters()->momentum().mag();
      } else {
        outgoingEnergy = endParameters->momentum().mag();
      }

      FitMeasurement* measurement =
          measurementFromTSOS(tsos, outgoingEnergy, particleMass);
      if (!measurement)
        continue;

      // insert next to adjacent measurement
      material.push_back(measurement);
      double distance = startDirection.dot(tsos.trackParameters()->position() -
                                           startPosition);
      if (distance > endSpectrometerDistance) {
        delete measurement;
        break;
      }
      while (m != measurements.end() &&
             distance > startDirection.dot(
                            (**m).intersection(FittedTrajectory).position() -
                            startPosition)) {
        ++m;
      }
      if (m == measurements.end()) {
        delete measurement;
        break;
      }

      m = measurements.insert(m, material.back());
    }
  }

  //     // check sign and order here
  //     printMeasurements(measurements);

  // memory management
  ATH_MSG_VERBOSE(" spectrometer: mem management");
  deleteMaterial(spectrometerMaterial, garbage);

  materialAggregation(measurements,
                      Trk::ParticleMasses::mass[particleHypothesis]);
}
}  // namespace Trk
