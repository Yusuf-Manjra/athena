/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "StripSurfaceChargesGenerator.h"

// DD
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SCT_ReadoutGeometry/SCT_ModuleSideDesign.h"

// Athena
#include "GeneratorObjects/HepMcParticleLink.h"
#include "InDetSimEvent/SiHit.h" // for SiHit, SiHit::::xDep, etc
#include "HitManagement/TimedHitPtr.h" // for TimedHitPtr
#include "GeneratorObjects/McEventCollectionHelper.h"
#include "PathResolver/PathResolver.h"

// ROOT
#include "TH1.h" // for TH1F
#include "TH2.h" // for TH2F
#include "TProfile.h" // for TProfile

// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Units/SystemOfUnits.h"

// STL
#include <cmath>
#include <fstream>
#include <regex>

using InDetDD::SiDetectorElement;
using InDetDD::SCT_ModuleSideDesign;
using InDetDD::SiLocalPosition;

using namespace std;

// constructor
StripSurfaceChargesGenerator::StripSurfaceChargesGenerator(const std::string& type,
                                                           const std::string& name,
                                                           const IInterface* parent)
  // : AthAlgTool(type, name, parent),
  :base_class(type, name, parent),
   m_distortionsTool("SCT_DistortionsTool", this),
   m_siConditionsSvc("SCT_SiliconConditionsSvc", name),
   m_siPropertiesSvc("SCT_SiPropertiesSvc", name),
   m_radDamageSvc("SCT_RadDamageSummarySvc", name)
{
  declareProperty("SCTDistortionsTool", m_distortionsTool,
                  "Tool to retrieve SCT distortions");
  declareProperty("SiConditionsSvc", m_siConditionsSvc);
  declareProperty("SiPropertiesSvc", m_siPropertiesSvc);
  declareProperty("SCT_RadDamageSummarySvc", m_radDamageSvc);

  declareProperty("FixedTime", m_tfix = -999);
  declareProperty("SubtractTime", m_tsubtract = -999);
  declareProperty("SurfaceDriftTime", m_tSurfaceDrift = 10);
  declareProperty("NumberOfCharges", m_numberOfCharges = 1);
  declareProperty("SmallStepLength", m_smallStepLength = 5);
  declareProperty("DepletionVoltage", m_vdepl = 70);
  declareProperty("BiasVoltage", m_vbias = 150);

  declareProperty("isOverlay", m_isOverlay=false);
  declareProperty("UseSiCondDB", m_useSiCondDB,
                  "Usage of SiConditions DB values can be disabled to use setable ones");
  declareProperty("doDistortions", m_doDistortions,
                  "Simulation of module distortions");
  declareProperty("doTrapping", m_doTrapping,
                  "Simulation of charge trapping effect");
  declareProperty("doHistoTrap", m_doHistoTrap,
                  "Histogram the charge trapping effect");
  declareProperty("doRamo", m_doRamo,
                  "Ramo Potential for charge trapping effect");


  declareProperty("UseMcEventCollectionHelper", m_needsMcEventCollHelper = false);

}

// ----------------------------------------------------------------------
// Initialize
// ----------------------------------------------------------------------
StatusCode StripSurfaceChargesGenerator::initialize() {
  ATH_MSG_DEBUG("SCT_SurfaceChargesGenerator::initialize()");

  // Get ISiPropertiesSvc
  ATH_CHECK(m_siPropertiesSvc.retrieve());

  // Get ISiliconConditionsSvc
  ATH_CHECK(m_siConditionsSvc.retrieve());

  if (m_doTrapping) {
    ///////////////////////////////////////////////////
    // -- Get Radiation Damage Service
    ATH_CHECK(m_radDamageSvc.retrieve());
    if(m_doRamo)
      m_radDamageSvc->InitPotentialValue();
  }

  if (m_doHistoTrap) {
    // -- Get Histogram Service
    ATH_CHECK(service("THistSvc", m_thistSvc));

    m_h_efieldz = new TProfile("efieldz", "", 50, 0., 0.4);
    ATH_CHECK(m_thistSvc->regHist("/file1/efieldz", m_h_efieldz));

    m_h_efield = new TH1F("efield", "", 100, 200., 800.);
    ATH_CHECK(m_thistSvc->regHist("/file1/efield", m_h_efield));

    m_h_spess = new TH1F("spess", "", 50, 0., 0.4);
    ATH_CHECK(m_thistSvc->regHist("/file1/spess", m_h_spess));

    m_h_depD = new TH1F("depD", "", 50, -0.3, 0.3);
    ATH_CHECK(m_thistSvc->regHist("/file1/depD", m_h_depD));

    m_h_drift_electrode = new TH2F("drift_electrode", "", 50, 0., 20., 50, 0., 20.);
    ATH_CHECK(m_thistSvc->regHist("/file1/drift_electrode", m_h_drift_electrode));

    m_h_drift_time = new TH1F("drift_time", "", 100, 0., 20.);
    ATH_CHECK(m_thistSvc->regHist("/file1/drift_time", m_h_drift_time));

    m_h_t_electrode = new TH1F("t_electrode", "", 100, 0., 20.);
    ATH_CHECK(m_thistSvc->regHist("/file1/t_electrode", m_h_t_electrode));

    m_h_ztrap = new TH1F("ztrap", "", 100, 0., 0.3);
    ATH_CHECK(m_thistSvc->regHist("/file1/ztrap", m_h_ztrap));

    // More histograms to check what's going on
    m_h_zhit = new TH1F("zhit", "", 50, -0.2, 0.2);
    ATH_CHECK(m_thistSvc->regHist("/file1/zhit", m_h_zhit));

    m_h_ztrap_tot = new TH1F("ztrap_tot", "", 100, 0., 0.5);
    ATH_CHECK(m_thistSvc->regHist("/file1/ztrap_tot", m_h_ztrap_tot));

    m_h_no_ztrap = new TH1F("no_ztrap", "", 100, 0., 0.5);
    ATH_CHECK(m_thistSvc->regHist("/file1/no_ztrap", m_h_no_ztrap));

    m_h_trap_drift_t = new TH1F("trap_drift_t", "", 100, 0., 20.);
    ATH_CHECK(m_thistSvc->regHist("/file1/trap_drift_t", m_h_trap_drift_t));

    m_h_notrap_drift_t = new TH1F("notrap_drift_t", "", 100, 0., 20.);
    ATH_CHECK(m_thistSvc->regHist("/file1/notrap_drift_t", m_h_notrap_drift_t));

    m_h_mob_Char = new TProfile("mob_Char", "", 200, 100., 1000.);
    ATH_CHECK(m_thistSvc->regHist("/file1/mob_Char", m_h_mob_Char));

    m_h_vel = new TProfile("vel", "", 100, 100., 1000.);
    ATH_CHECK(m_thistSvc->regHist("/file1/vel", m_h_vel));

    m_h_drift1 = new TProfile("drift1", "", 50, 0., 0.3);
    ATH_CHECK(m_thistSvc->regHist("/file1/drift1", m_h_drift1));

    m_h_gen = new TProfile("gen", "", 50, 0., 0.3);
    ATH_CHECK(m_thistSvc->regHist("/file1/gen", m_h_gen));

    m_h_gen1 = new TProfile("gen1", "", 50, 0., 0.3);
    ATH_CHECK(m_thistSvc->regHist("/file1/gen1", m_h_gen1));

    m_h_gen2 = new TProfile("gen2", "", 50, 0., 0.3);
    ATH_CHECK(m_thistSvc->regHist("/file1/gen2", m_h_gen2));

    m_h_velocity_trap = new TProfile("velocity_trap", "", 50, 0., 1000.);
    ATH_CHECK(m_thistSvc->regHist("/file1/velocity_trap", m_h_velocity_trap));

    m_h_mobility_trap = new TProfile("mobility_trap", "", 50, 100., 1000.);
    ATH_CHECK(m_thistSvc->regHist("/file1/mobility_trap", m_h_mobility_trap));

    m_h_trap_pos = new TH1F("trap_pos", "", 100, 0., 0.3);
    ATH_CHECK(m_thistSvc->regHist("/file1/trap_pos", m_h_trap_pos));
  }
  ///////////////////////////////////////////////////

  m_smallStepLength.setValue(m_smallStepLength.value() * CLHEP::micrometer);
  m_tSurfaceDrift.setValue(m_tSurfaceDrift.value() * CLHEP::ns);

  // Surface drift time calculation Stuff
  m_tHalfwayDrift = m_tSurfaceDrift * 0.5;
  m_distHalfInterStrip = m_distInterStrip * 0.5;
  if ((m_tSurfaceDrift > m_tHalfwayDrift) and (m_tHalfwayDrift >= 0.0) and
      (m_distHalfInterStrip > 0.0) and (m_distHalfInterStrip < m_distInterStrip)) {
    m_SurfaceDriftFlag = true;
  } else {
    ATH_MSG_INFO("\tsurface drift still not on, wrong params");
  }

  // Make sure these two flags are not set simultaneously
  if (m_tfix>-998. and m_tsubtract>-998.) {
    ATH_MSG_FATAL("\tCannot set both FixedTime and SubtractTime options!");
    ATH_MSG_INFO("\tMake sure the two flags are not set simultaneously in jo");
    return StatusCode::FAILURE;
  }

  if (m_doDistortions) {
    ATH_CHECK(m_distortionsTool.retrieve());
  } else {
    // m_distortionsTool.disable();
  }

  std::string potentialFileName("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/StripDigitization/SCTPotential.csv");
  ATH_MSG_INFO("Load ITK sensor E-potential from file: "+ potentialFileName);
  if(!loadPotenticalFile(potentialFileName, m_pot_deltax, m_pot_deltay, m_pot_nstepx, m_pot_nstepy, m_potentialVec)){
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// finalize
// ----------------------------------------------------------------------
StatusCode StripSurfaceChargesGenerator::finalize() {
  ATH_MSG_DEBUG("SCT_SurfaceChargesGenerator::finalize()");
  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// perpandicular Drift time calculation
// ----------------------------------------------------------------------
float StripSurfaceChargesGenerator::driftTime(float zhit, const SiDetectorElement* element) const {
  if (element==nullptr) {
    ATH_MSG_ERROR("SCT_SurfaceChargesGenerator::process element is nullptr");
    return -2.0;
  }
  const SCT_ModuleSideDesign* design{dynamic_cast<const SCT_ModuleSideDesign*>(&(element->design()))};
  if (design==nullptr) {
    ATH_MSG_ERROR("SCT_SurfaceChargesGenerator::process can not get " << design);
    return -2.0;
  }
  const double thickness{design->thickness()};
  const IdentifierHash hashId{element->identifyHash()};

  if ((zhit < 0.0) or (zhit > thickness)) {
    ATH_MSG_DEBUG("driftTime: hit coordinate zhit=" << zhit / CLHEP::micrometer << " out of range");
    return -2.0;
  }

  float depletionVoltage{0.};
  float biasVoltage{0.};
  if (m_useSiCondDB) {
    depletionVoltage = m_siConditionsSvc->depletionVoltage(hashId) * CLHEP::volt;
    biasVoltage = m_siConditionsSvc->biasVoltage(hashId) * CLHEP::volt;
  } else {
    depletionVoltage = m_vdepl * CLHEP::volt;
    biasVoltage = m_vbias * CLHEP::volt;
  }

  const float denominator{static_cast<float>(depletionVoltage + biasVoltage - (2.0 * zhit * depletionVoltage / thickness))};
  if (denominator <= 0.0) {
    if (biasVoltage >= depletionVoltage) { // Should not happen
      if(not m_isOverlay) {
        ATH_MSG_ERROR("driftTime: negative argument X for log(X) " << zhit);
      }
      return -1.0;
    } else {
      // (m_biasVoltage<m_depletionVoltage) can happen with underdepleted sensors, lose charges in that volume
      return -10.0;
    }
  }

  float t_drift{log((depletionVoltage + biasVoltage) / denominator)};


  t_drift *= thickness * thickness / (2.0 * m_siPropertiesSvc->getSiProperties(hashId).driftMobility(InDetDD::holes) * depletionVoltage);
  return t_drift;
}

// ----------------------------------------------------------------------
// Sigma diffusion calculation
// ----------------------------------------------------------------------
float StripSurfaceChargesGenerator::diffusionSigma(float zhit, const SiDetectorElement* element) const {
  if (element==nullptr) {
    ATH_MSG_ERROR("SCT_SurfaceChargesGenerator::diffusionSigma element is nullptr");
    return 0.0;
  }
  const IdentifierHash hashId{element->identifyHash()};
  const float t{driftTime(zhit, element)}; // in ns

  if (t > 0.0) {
    const float sigma{static_cast<float>(sqrt(2. * m_siPropertiesSvc->getSiProperties(hashId).diffusionConstant(InDetDD::holes) * t))}; // in mm
    return sigma;
  } else {
    return 0.0;
  }
}

// ----------------------------------------------------------------------
// Maximum drift time
// ----------------------------------------------------------------------
float StripSurfaceChargesGenerator::maxDriftTime(const SiDetectorElement* element) const {
  if (element) {
    const float sensorThickness{static_cast<float>(element->thickness())};
    return driftTime(sensorThickness, element);
  } else {
    ATH_MSG_INFO("Error: SiDetectorElement not set!");
    return 0.;
  }
}

// ----------------------------------------------------------------------
// Maximum Sigma difusion
// ----------------------------------------------------------------------
float StripSurfaceChargesGenerator::maxDiffusionSigma(const SiDetectorElement* element) const {
  if (element) {
    const float sensorThickness{static_cast<float>(element->thickness())};
    return diffusionSigma(sensorThickness, element);
  } else {
    ATH_MSG_INFO("Error: SiDetectorElement not set!");
    return 0.;
  }
}

// ----------------------------------------------------------------------
// Calculating the surface drift time but I should confess that
// I haven't found out yet where the calculation come from
// ----------------------------------------------------------------------
float StripSurfaceChargesGenerator::surfaceDriftTime(float ysurf) const {
  if (m_SurfaceDriftFlag) {
    if ((ysurf >= 0.0) and (ysurf <= m_distInterStrip)) {
      float t_surfaceDrift{0.};
      if (ysurf < m_distHalfInterStrip) {
        const float y{ysurf / m_distHalfInterStrip};
        t_surfaceDrift= m_tHalfwayDrift * y * y;
      } else {
        const float y{(m_distInterStrip - ysurf) / (m_distInterStrip - m_distHalfInterStrip)};
        t_surfaceDrift = m_tSurfaceDrift + (m_tHalfwayDrift - m_tSurfaceDrift) * y * y;
      }
      return t_surfaceDrift;
    } else {
      ATH_MSG_WARNING(" ysurf out of range " << ysurf << "(m_distInterStrip = "<<m_distInterStrip<<")");
      return -1.0;
    }
  } else {
    return 0.0;
  }
}

// -------------------------------------------------------------------------------------------
// create a list of surface charges from a hit - called from SCT_Digitization
// AthAlgorithm
// -------------------------------------------------------------------------------------------
void StripSurfaceChargesGenerator::process(const SiDetectorElement* element,
					   const SCT_ModuleSideDesign * motherDesign,
                                           const TimedHitPtr<SiHit>& phit,
                                           const ISiSurfaceChargesInserter& inserter, CLHEP::HepRandomEngine * rndmEngine) const {
  ATH_MSG_VERBOSE("SCT_SurfaceChargesGenerator::process starts");
  processSiHit(element, motherDesign, *phit, inserter, phit.eventTime(), phit.pileupType(), rndmEngine);
  return;
}

// -------------------------------------------------------------------------------------------
// create a list of surface charges from a hit - called from both AthAlgorithm
// and PileUpTool
// -------------------------------------------------------------------------------------------
void StripSurfaceChargesGenerator::processSiHit(const SiDetectorElement* element,
						const SCT_ModuleSideDesign * motherDesign,
                                                const SiHit& phit,
                                                const ISiSurfaceChargesInserter& inserter,
                                                float p_eventTime,
                                                unsigned short p_pileupType, CLHEP::HepRandomEngine * rndmEngine) const {
  const SCT_ModuleSideDesign* design;
  if (motherDesign){
    ATH_MSG_DEBUG("Using Mother Design");
    design = motherDesign;
  }
  else design = dynamic_cast<const SCT_ModuleSideDesign*>(&(element->design()));
  if (design==nullptr) {
    ATH_MSG_ERROR("SCT_SurfaceChargesGenerator::process can not get " << design);
    return;
  }

  const double thickness{design->thickness()};
  const IdentifierHash hashId{element->identifyHash()};
  const double tanLorentz{element->getTanLorentzAnglePhi()};

  // ---**************************************
  //  Time of Flight Calculation - separate method?
  // ---**************************************
  // --- Original calculation of Time of Flight of the particle Time needed by the particle to reach the sensor
  float timeOfFlight{p_eventTime + hitTime(phit)};

  // Kondo 19/09/2007: Use the coordinate of the center of the module to calculate the time of flight
  timeOfFlight -= (element->center().mag()) / CLHEP::c_light;
  // !< extract the distance to the origin of the module to Time of flight

  // !< timing set from jo to adjust (subtract) the timing
  if (m_tsubtract > -998.) {
    timeOfFlight -= m_tsubtract;
  }
  // ---**************************************

  const CLHEP::Hep3Vector pos{phit.localStartPosition()};
  const float xEta{static_cast<float>(pos[SiHit::xEta])};
  const float xPhi{static_cast<float>(pos[SiHit::xPhi])};
  const float xDep{static_cast<float>(pos[SiHit::xDep])};

  const CLHEP::Hep3Vector endPos{phit.localEndPosition()};
  const float cEta{static_cast<float>(endPos[SiHit::xEta]) - xEta};
  const float cPhi{static_cast<float>(endPos[SiHit::xPhi]) - xPhi};
  const float cDep{static_cast<float>(endPos[SiHit::xDep]) - xDep};

  const float LargeStep{sqrt(cEta*cEta + cPhi*cPhi + cDep*cDep)};
  const int numberOfSteps{static_cast<int>(LargeStep / m_smallStepLength) + 1};
  const float steps{static_cast<float>(m_numberOfCharges * numberOfSteps)};
  const float e1{static_cast<float>(phit.energyLoss() / steps)};
  const float q1{static_cast<float>(e1 * m_siPropertiesSvc->getSiProperties(hashId).electronHolePairsPerEnergy())};

  // in the following, to test the code, we will use the original coordinate
  // system of the SCTtest3SurfaceChargesGenerator x is eta y is phi z is depth
  float xhit{xEta};
  float yhit{xPhi};
  float zhit{xDep};

  if (m_doDistortions) {
    if (element->isBarrel()) {// Only apply disortions to barrel modules
      Amg::Vector2D BOW;
      BOW[0] = m_distortionsTool->correctSimulation(hashId, xhit, yhit, cEta, cPhi, cDep)[0];
      BOW[1] = m_distortionsTool->correctSimulation(hashId, xhit, yhit, cEta, cPhi, cDep)[1];
      xhit = BOW.x();
      yhit = BOW.y();
    }
  }

  const float StepX{cEta / numberOfSteps};
  const float StepY{cPhi / numberOfSteps};
  const float StepZ{cDep / numberOfSteps};

  // check the status of truth information for this SiHit
  // some Truth information is cut for pile up events
  HepMcParticleLink trklink(phit.particleLink());
  if (m_needsMcEventCollHelper) {
    MsgStream* amsg = &(msg());
    trklink.setEventCollection( McEventCollectionHelper::getMcEventCollectionHMPLEnumFromPileUpType(p_pileupType, amsg) );
  }
  SiCharge::Process hitproc{SiCharge::track};
  if (phit.trackNumber() != 0) {
    if (not trklink.isValid()) {
      hitproc = SiCharge::cut_track;
    }
  }

  float dstep{-0.5};
  for (int istep{0}; istep < numberOfSteps; ++istep) {
    dstep += 1.0;
    float z1{zhit + StepZ * dstep};

    // Distance between charge and readout side.
    // design->readoutSide() is +1 if readout side is in +ve depth axis direction and visa-versa.
    float zReadout{static_cast<float>(0.5 * thickness - design->readoutSide() * z1)};
    const double spess{zReadout};

    if (m_doHistoTrap) {
      m_h_depD->Fill(z1);
      m_h_spess->Fill(spess);
    }

    float t_drift{driftTime(zReadout, element)};  // !< t_drift: perpandicular drift time
    if (t_drift>-2.0000002 and t_drift<-1.9999998) {
      ATH_MSG_DEBUG("Checking for rounding errors in compression");
      if ((fabs(z1) - 0.5 * thickness) < 0.000010) {
        ATH_MSG_DEBUG("Rounding error found attempting to correct it. z1 = " << std::fixed << std::setprecision(8) << z1);
        if (z1 < 0.0) {
          z1 = 0.0000005 - 0.5 * thickness;
          // set new coordinate to be 0.5nm inside wafer volume.
        } else {
          z1 = 0.5 * thickness - 0.0000005;
          // set new coordinate to be 0.5nm inside wafer volume.
        }
        zReadout = 0.5 * thickness - design->readoutSide() * z1;
        t_drift = driftTime(zReadout, element);
        if (t_drift>-2.0000002 and t_drift<-1.9999998) {
          ATH_MSG_WARNING("Attempt failed. Making no correction.");
        } else {
          ATH_MSG_DEBUG("Correction Successful! z1 = " << std::fixed << std::setprecision(8) << z1 << ", zReadout = " << zReadout << ", t_drift = " << t_drift);
        }
      } else {
        ATH_MSG_DEBUG("No rounding error found. Making no correction.");
      }
    }
    if (t_drift > 0.0) {
      const float x1{xhit + StepX * dstep};
      float y1{yhit + StepY * dstep};
      y1 += tanLorentz * zReadout; // !< Taking into account the magnetic field
      const float sigma{diffusionSigma(zReadout, element)};

      for (int i{0}; i < m_numberOfCharges; ++i) {
        const float rx{CLHEP::RandGaussZiggurat::shoot(rndmEngine)};
        const float xd{x1 + sigma * rx};
        const float ry{CLHEP::RandGaussZiggurat::shoot(rndmEngine)};
        const float yd{y1 + sigma * ry};

        // For charge trapping with Ramo potential
        const double stripPitch{0.080}; // mm
        double dstrip{y1 / stripPitch}; // mm
        if (dstrip > 0.) {
          dstrip -= static_cast<double>(static_cast<int>(dstrip));
        } else {
          dstrip -= static_cast<double>(static_cast<int>(dstrip)) + 1;
        }

        // now y will be x and z will be y ....just to make sure to confuse everebody
        double y0{dstrip * stripPitch}; // mm
        double z0{thickness - zReadout}; // mm

        // flag m_doCTrap is not used in cleaned-up version, might not need at all.
        // -- Charge Trapping
        if (m_doTrapping) {
          if (m_doHistoTrap) {
            m_h_zhit->Fill(zhit);
          }
          double trap_pos{-999999.}, drift_time{-999999.}; // FIXME need better default values
          if (chargeIsTrapped(spess, element, trap_pos, drift_time)) {
            if (not m_doRamo) {
              break;
            } else {  // if we want to take into account also Ramo Potential
              double Q_m2{0.}, Q_m1{0.}, Q_00{0.}, Q_p1{0.}, Q_p2{0.}; // Charges

              dstrip = y1 / stripPitch; // mm
              // need the distance from the nearest strips
              // edge not centre, xtaka = 1/2*stripPitch
              // centre of detector, y1=0, is in the middle of
              // an interstrip gap
              if (dstrip > 0.) {
                dstrip -= static_cast<double>(static_cast<int>(dstrip));
              } else {
                dstrip -= static_cast<double>(static_cast<int>(dstrip)) + 1;
              }

              // now y will be x and z will be y ....just to make sure to confuse everebody
              double yfin{dstrip * stripPitch}; // mm
              double zfin{thickness - trap_pos}; // mm

              transport(stripPitch, thickness, y0, z0, yfin, zfin, Q_m2, Q_m1, Q_00, Q_p1, Q_p2);
              for (int strip{-2}; strip<=2; strip++) {
                const double ystrip{yd + strip * stripPitch}; // mm
                const SiLocalPosition position(element->hitLocalToLocal(xd, ystrip));
                if (design->inActiveArea(position)) {
                  double charge{0.};
                  if (strip == -2) charge = Q_m2;
                  if (strip == -1) charge = Q_m1;
                  if (strip ==  0) charge = Q_00;
                  if (strip ==  1) charge = Q_p1;
                  if (strip ==  2) charge = Q_p2;
                  const double time{drift_time};
                  if (charge != 0.) {
                    inserter(SiSurfaceCharge(position, SiCharge(q1*charge, time, hitproc, trklink)));
                    continue;
                  }
                }
              }
              ATH_MSG_DEBUG("strip zero charge = " << Q_00); // debug
            } // m_doRamo==true
          } // chargeIsTrapped()
        } // m_doTrapping==true
        if (not m_doRamo) {
          const SiLocalPosition position{element->hitLocalToLocal(xd, yd)};
          if (design->inActiveArea(position)) {
            const float sdist{static_cast<float>(design->scaledDistanceToNearestDiode(position))}; // !< dist on the surface from the hit point to the nearest strip (diode)
            ATH_MSG_VERBOSE("sdist: "<<sdist);
            const float t_surf{surfaceDriftTime(2.0 * sdist)}; // !< Surface drift time
            const float totaltime{(m_tfix > -998.) ? m_tfix.value() : t_drift + timeOfFlight + t_surf}; // !< Total drift time
            inserter(SiSurfaceCharge(position, SiCharge(q1, totaltime, hitproc, trklink)));
          } else {
            ATH_MSG_VERBOSE(std::fixed << std::setprecision(8) << "Local position (phi, eta, depth): ("
                            << position.xPhi() << ", " << position.xEta() << ", " << position.xDepth()
                            << ") of the element is out of active area, charge = " << q1);
          }
        } // end of loop on charges
      }
    }
  }
  return;
}

bool StripSurfaceChargesGenerator::chargeIsTrapped(double spess,
                                                   const SiDetectorElement* element,
                                                   double& trap_pos,
                                                   double& drift_time) const {
  if (element==nullptr) {
    ATH_MSG_ERROR("SCT_SurfaceChargesGenerator::chargeIsTrapped element is nullptr");
    return false;
  }
  bool isTrapped{false};
  const IdentifierHash hashId{element->identifyHash()};
  const double electric_field{m_radDamageSvc->ElectricField(hashId, spess)};

  if (m_doHistoTrap) {
    double mobChar = m_siPropertiesSvc->getSiProperties(hashId).driftMobility(InDetDD::holes);
    m_h_efieldz->Fill(spess, electric_field);
    m_h_efield->Fill(electric_field);
    m_h_mob_Char->Fill(electric_field, mobChar);
    m_h_vel->Fill(electric_field, electric_field * mobChar);
  }
  const double t_electrode{m_radDamageSvc->TimeToElectrode(hashId, spess)};
  drift_time = m_radDamageSvc->TrappingTime(hashId, spess);
  const double z_trap{m_radDamageSvc->TrappingPositionZ(hashId, spess)};
  trap_pos = spess - z_trap;
  if (m_doHistoTrap) {
    m_h_drift_time->Fill(drift_time);
    m_h_t_electrode->Fill(t_electrode);
    m_h_drift_electrode->Fill(drift_time, t_electrode);
    m_h_ztrap_tot->Fill(z_trap);
  }
  // -- Calculate if the charge is trapped, and at which distance
  // -- Charge gets trapped before arriving to the electrode
  if (drift_time < t_electrode) {
    isTrapped = true;
    ATH_MSG_INFO("drift_time: " << drift_time << " t_electrode:  " << t_electrode << " spess " << spess);
    ATH_MSG_INFO("z_trap: " << z_trap);
    if (m_doHistoTrap) {
      m_h_ztrap->Fill(z_trap);
      m_h_trap_drift_t->Fill(drift_time);
      m_h_drift1->Fill(spess, drift_time / t_electrode);
      m_h_gen->Fill(spess, drift_time);
      m_h_gen1->Fill(spess, z_trap);
      m_h_gen2->Fill(spess, z_trap / drift_time * t_electrode);
      m_h_velocity_trap->Fill(electric_field, z_trap / drift_time);
      m_h_mobility_trap->Fill(electric_field, z_trap / drift_time / electric_field);
      m_h_trap_pos->Fill(trap_pos);
    }
  } else {
    isTrapped = false;
    if (m_doHistoTrap) {
      const double z_trap{m_radDamageSvc->TrappingPositionZ(hashId, spess)};
      m_h_no_ztrap->Fill(z_trap);
      m_h_notrap_drift_t->Fill(drift_time);
    }
  }
  return isTrapped;
}


//---------------------------------------------------------------------
//  electronTransport
//---------------------------------------------------------------------
void StripSurfaceChargesGenerator::transport(double pitch, double thickness, double & x0, double & y0, double & xfin, double & yfin, double & Q_m2, double & Q_m1, double& Q_00, double & Q_p1, double & Q_p2 )const{

  double qstrip[5];

  double x = x0/10.;  // original hole position [cm]
  double y = y0/10.;  // original hole position [cm]
  for (int istrip = -2 ; istrip < 3 ; istrip++)
    qstrip[istrip+2] = induced(pitch, thickness, istrip, x, y);

  x = xfin/10.;
  y = yfin/10.;
  // Get induced current by subtracting induced charges
  for (int istrip = -2 ; istrip < 3 ; istrip++) {
    double qnew = induced(pitch, thickness, istrip, x, y);
    double dq = qnew - qstrip[istrip + 2];
    qstrip[istrip + 2] = qnew ;

    switch(istrip) {
    case -2: Q_m2 += dq ; break;
    case -1: Q_m1 += dq ; break;
    case  0: Q_00 += dq ; break;
    case +1: Q_p1 += dq ; break;
    case +2: Q_p2 += dq ; break;
    }
  }
  return;
}


//-------------------------------------------------------------------
//    calculation of induced charge using Weighting (Ramo) function
//-------------------------------------------------------------------
double StripSurfaceChargesGenerator::induced (double pitch, double thickness, int istrip, double x, double y)const{
  // x and y are the coorlocation of charge (e or hole)
  // induced chardege on the strip "istrip" situated at the height y = d
  // the center of the strip (istrip=0) is x = 0.04 [mm]
  // x is width, y is depth

  if ( y < 0. || y > thickness) return 0;
  double xc = pitch * (istrip + 0.5);
  double dx = fabs( x-xc );
  int ix = int( dx / m_pot_deltax );
  if ( ix > 79 ) return 0.;
  int iy = int( y  / m_pot_deltay );
  double fx = (dx - ix*m_pot_deltax) / m_pot_deltax;
  double fy = ( y - iy*m_pot_deltay) / m_pot_deltay;
  int ix1 = ix + 1;
  int iy1 = iy + 1;

  double P
    =  m_potentialVec[ix* m_pot_nstepy + iy] *(1.-fx)*(1.-fy)
    +  m_potentialVec[ix1*m_pot_nstepy+ iy]  *fx*(1.-fy)
    +  m_potentialVec[ix *m_pot_nstepy+ iy1] *(1.-fx)*fy
    +  m_potentialVec[ix1*m_pot_nstepy+ iy1] *fx*fy ;

  return P;
}

unsigned StripSurfaceChargesGenerator::loadPotenticalFile(const std::string& fileName,
                                                          double& deltax, double& deltay,
                                                          unsigned& nstepx, unsigned& nstepy,
                                                          std::vector<double>& potentialVec){
  std::ifstream potentialIFS(fileName.c_str());
  if(potentialIFS.fail()){
    ATH_MSG_ERROR("unable to open ITK strip sensor potential file");
    return 0;
  }

  std::string potentialStr;
  potentialStr.assign((std::istreambuf_iterator<char>(potentialIFS) ),
                      (std::istreambuf_iterator<char>()));
  potentialIFS.close();

  if(potentialStr.empty()){
    ATH_MSG_ERROR("unable to read ITK strip sensor potential file");
    return 0;
  }

  std::istringstream potentialISS(potentialStr);
  potentialStr.clear();
  nstepx = 0;
  nstepy = 0;
  std::string aline;
  while(std::getline(potentialISS, aline)){
    std::smatch sm;
    if(std::regex_match(aline, sm, std::regex("//delta_x:([0-9\\.]+);delta_y:([0-9\\.]+);step_x:([0-9]+);step_y:([0-9]+)\\s*"))){
      deltax = std::stod(sm.str(1));
      deltay = std::stod(sm.str(2));
      nstepx = std::stoi(sm.str(3));
      nstepy = std::stoi(sm.str(4));
      break;
    }
  }
  if (!nstepx || !nstepy){
    ATH_MSG_ERROR("error to read the header line of ITK strip sensor potential file");
    return 0;
  }

  potentialVec.resize(nstepx*nstepy);
  std::vector<double>::iterator pit = potentialVec.begin();
  std::vector<double>::iterator pend = potentialVec.end();
  while(std::getline(potentialISS, aline)){
      std::regex e("([\\+-]?[0-9\\.]+)");
      std::regex_iterator<std::string::iterator> rit ( aline.begin(), aline.end(), e );
      std::regex_iterator<std::string::iterator> rend;
      while (rit!=rend && pit!=pend) {
        *pit = std::stod(rit->str());
        ++rit;
        ++pit;
      }
  }
  if(pit!=pend){
    ATH_MSG_ERROR("error to read the ITK strip sensor potential file (unmatched data size)");
    return 0;
  }
  return nstepx*nstepy;
}
