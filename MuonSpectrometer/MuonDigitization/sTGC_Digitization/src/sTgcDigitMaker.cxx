/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGC_Digitization/sTgcDigitMaker.h"

#include "MuonDigitContainer/sTgcDigitCollection.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"
#include "MuonSimEvent/sTgcSimIdToOfflineId.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"
#include "PathResolver/PathResolver.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandGamma.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "TF1.h" 
#include <cmath>
#include <iostream>
#include <fstream>

//---------------------------------------------------
//  Constructor and Destructor
//---------------------------------------------------

//----- Constructor
sTgcDigitMaker::sTgcDigitMaker(const sTgcHitIdHelper* hitIdHelper, const MuonGM::MuonDetectorManager* mdManager, bool doEfficiencyCorrection)
{
  m_hitIdHelper             = hitIdHelper;
  m_mdManager               = mdManager;
  m_efficiencyOfWireGangs   = 1.000; // 100% efficiency for sTGCSimHit_p1
  m_efficiencyOfStrips      = 1.000; // 100% efficiency for sTGCSimHit_p1
  m_doEfficiencyCorrection  = doEfficiencyCorrection;
  m_doTimeCorrection        = true;
  m_timeWindowOffsetPad     = 0.;
  m_timeWindowOffsetStrip   = 25.;
  //m_timeWindowPad          = 30.; // TGC  29.32; // 29.32 ns = 26 ns +  4 * 0.83 ns
  //m_timeWindowStrip         = 30.; // TGC  40.94; // 40.94 ns = 26 ns + 18 * 0.83 ns
  //m_bunchCrossingTime       = 24.95; // 24.95 ns =(40.08 MHz)^(-1)
  //m_timeJitterElectronicsStrip   = 2.; // 2ns jitter of electronics from the threshold to peak
  //m_timeJitterElectronicsPad     = 2.; // 2ns jitter of electronics for passing the threshold
  m_GausMean                = 2.27;  //mm; VMM response from Oct/Nov 2013 test beam
  m_GausSigma               = 0.1885;//mm; VMM response from Oct/Nov 2013 test beam
  m_IntegralTimeOfElectr    = 20.00; // ns
  m_CrossTalk               = 0.00; // Turn off cross-talk. Old guesstimate was 0.03: Alexandre Laurier 2020-10-11 
  m_StripResolution         = 0.07; // Angular strip resolution parameter
  m_ChargeSpreadFactor      = 0.;
  m_channelTypes            = 3; // 1 -> strips, 2 -> strips+pad, 3 -> strips/wires/pads
  m_theta = 0.8; // theta=0.8 value best matches the PDF
  m_mean = 2E5;  // mean gain estimated from ATLAS note "ATL-MUON-PUB-2014-001" 
}

//----- Destructor
sTgcDigitMaker::~sTgcDigitMaker()
{}
//------------------------------------------------------
// Initialize
//------------------------------------------------------
StatusCode sTgcDigitMaker::initialize(CLHEP::HepRandomEngine *rndmEngine, const int channelTypes)
{
  // Initialize TgcIdHelper
  if (m_hitIdHelper == nullptr) {
    m_hitIdHelper = sTgcHitIdHelper::GetHelper();
  }

  ////set the flag of timeCorrection
  //m_doTimeCorrection = doTimeCorrection;

  //set the flag of channelTypes which will be digitized
  m_channelTypes = channelTypes; 

  // initialize the TGC identifier helper
  m_idHelper = m_mdManager->stgcIdHelper();

  readFileOfTimeJitter();

  // Read share/sTGC_Digitization_timeArrivale.dat, containing the digit time of arrival
  readFileOfTimeArrival();

  // getting our random numbers stream
  m_engine = rndmEngine;

  // Read share/sTGC_Digitization_energyThreshold.dat file and store values in m_energyThreshold.
  // Currently no point in wasting memory to read an empty file for energy threshold.
  // We have no gap-by-gap energy threshold currently for the sTGC
  // Alexandre Laurier - April 13 2021
  //readFileOfEnergyThreshold();

  //// Read share/sTGC_Digitization_crossTalk.dat file and store values in m_crossTalk.
  //readFileOfCrossTalk();

  // Read share/sTGC_Digitization_deadChamber.dat file and store values in m_isDeadChamber.
  readFileOfDeadChamber();

  // Read share/sTGC_Digitization_effChamber.dat file and store values in m_ChamberEfficiency.
  readFileOfEffChamber();

  // Read share/sTGC_Digitization_timeWindowOffset.dat file and store values in m_timeWindowOffset.
  readFileOfTimeWindowOffset();

  //// Read share/sTGC_Digitization_alignment.dat file and store values in m_alignmentZ, m_alignmentT, m_alignmentS, m_alignmentTHS
  //readFileOfAlignment();
  return StatusCode::SUCCESS;
}

//---------------------------------------------------
// Execute Digitization
//---------------------------------------------------
std::unique_ptr<sTgcDigitCollection> sTgcDigitMaker::executeDigi(const sTGCSimHit* hit, const float /*globalHitTime*/)
{ 

  // check the digitization channel type
  if(m_channelTypes!=1 && m_channelTypes!=2 && m_channelTypes!=3){
    ATH_MSG_ERROR("Invalid ChannelTypes : " << m_channelTypes << " (valid values are : 1 --> strips ; 2 --> strips / wires ; 3 --> strips / wires / pads)"); 
  }

  // SimHits without energy loss are not recorded.
  double energyDeposit = hit->depositEnergy(); // Energy deposit in MeV 
  if(energyDeposit==0.) return nullptr;

  //////////  convert ID for this digitizer system 
  sTgcSimIdToOfflineId simToOffline(m_idHelper);  
  int simId = hit->sTGCId();
  Identifier offlineId = simToOffline.convert(simId);
  std::string stationName= m_idHelper->stationNameString(m_idHelper->stationName(offlineId));
  int stationEta = m_idHelper->stationEta(offlineId);
  int stationPhi  = m_idHelper->stationPhi(offlineId);
  int multiPlet = m_idHelper->multilayer(offlineId);
  int gasGap = m_idHelper->gasGap(offlineId);
  Identifier layid = m_idHelper->channelID(m_idHelper->stationName(offlineId), stationEta, stationPhi, 
                                           multiPlet, gasGap, sTgcIdHelper::sTgcChannelTypes::Wire, 1);

  ATH_MSG_VERBOSE("sTgc hit:  time " << hit->globalTime() << " position " << hit->globalPosition().x() << "  " << hit->globalPosition().y() << "  " << hit->globalPosition().z() << " mclink " << hit->particleLink() << " PDG ID " << hit->particleEncoding() );

  int isSmall = stationName[2] == 'S';

  ATH_MSG_DEBUG("Retrieving detector element for: isSmall " << isSmall << " eta " << stationEta << " phi " << stationPhi << " ml " << multiPlet << " energyDeposit "<<energyDeposit );

  const MuonGM::sTgcReadoutElement* detEl = m_mdManager->getsTgcReadoutElement(layid);
  if( !detEl ){
    ATH_MSG_WARNING("Failed to retrieve detector element for: isSmall " << isSmall << " eta " << stationEta << " phi " << stationPhi << " ml " << multiPlet );
    return nullptr;
  }
 
  // DO THE DIGITIZATTION HERE ////////

  // Retrieve the wire surface
  int surfHash_wire = detEl->surfaceHash(gasGap, sTgcIdHelper::sTgcChannelTypes::Wire);
  const Trk::PlaneSurface& SURF_WIRE = detEl->surface(surfHash_wire); // get the wire surface

  // Hit global position
  Amg::Vector3D GPOS(hit->globalPosition().x(),hit->globalPosition().y(),hit->globalPosition().z());
  // Hit global direction
  const Amg::Vector3D GLODIRE(hit->globalDirection().x(), hit->globalDirection().y(), hit->globalDirection().z());

  // Hit position in the wire surface's coordinate frame 
  Amg::Vector3D hitOnSurface_wire = SURF_WIRE.transform().inverse() * GPOS;
  Amg::Vector2D posOnSurf_wire(hitOnSurface_wire.x(), hitOnSurface_wire.y());

  /* Determine the closest wire and the distance of closest approach
   * Since most particles pass through the the wire plane between two wires,
   * the nearest wire should be one of these two wire. Otherwise, the particle's
   * trajectory is uncommon, and such rare case is not supported yet.
   *
   * Finding that nearest wire follows the following steps:
   * - Compute the distance to the wire at the center of the current wire pitch
   * - Compute the distance to the other adjacent wire and, if it is smaller, 
   *   verify the distance to the next to the adjacent wire
   */

  // hit direction in the wire surface's coordinate frame
  Amg::Vector3D loc_dire_wire = SURF_WIRE.transform().inverse().linear()*GLODIRE;

  // Wire number of the current wire pitch
  int wire_number = detEl->getDesign(layid)->wireNumber(posOnSurf_wire);

  // Compute the distance from the hit to the wire, return value of -9.99 if unsuccessful
  double dist_wire = distanceToWire(hitOnSurface_wire, loc_dire_wire, layid, wire_number);
  if (dist_wire < -9.) {
    ATH_MSG_WARNING("Failed to get the distance between the hit at (" 
                    << hitOnSurface_wire.x() << ", " << hitOnSurface_wire.y() << ")"
                    << " and wire number = " << wire_number 
                    << ", chamber stationName: " << stationName 
                    << ", stationEta: " << stationEta
                    << ", stationPhi: " << stationPhi
                    << ", multiplet:" << multiPlet
                    << ", gas gap: " << gasGap);
  } else {
    // Determine the other adjacent wire, which is +1 if particle passes through the 
    // wire plane between wires wire_number and wire_number+1 and -1 if particle
    // passes through between wires wire_number and wire_number-1
    int adjacent = 1;
    if (dist_wire < 0.) {adjacent = -1;}

    // Compute distance to the other adjacent wire
    double dist_wire_adj = distanceToWire(hitOnSurface_wire, loc_dire_wire, layid, wire_number + adjacent);
    if (std::abs(dist_wire_adj) < std::abs(dist_wire)) {
      dist_wire = dist_wire_adj;
      wire_number = wire_number + adjacent;

      // Check the next to the adjacent wire to catch uncommon track
      if ((wire_number + adjacent) > 1) {
        double tmp_dist = distanceToWire(hitOnSurface_wire, loc_dire_wire, layid, wire_number + adjacent * 2);
        if (std::abs(tmp_dist) < std::abs(dist_wire)) {
          ATH_MSG_WARNING("Wire number is more than one wire pitch away for hit position = (" 
                          << hitOnSurface_wire.x() << ", " << hitOnSurface_wire.y() << ")"
                          << ", wire number = " << wire_number + adjacent * 2
                          << ", with d(-2) = " << tmp_dist
                          << ", while d(0) = " << dist_wire
                          << ", chamber stationName = " << stationName
                          << ", stationEta = " << stationEta
                          << ", stationPhi = " << stationPhi
                          << ", multiplet = " << multiPlet
                          << ", gas gap = " << gasGap);
        }
      }
    }
  }

  // Distance should be in the range [0, 0.9] mm, unless particle passes through 
  // the wire plane near the edges
  double wire_pitch = detEl->wirePitch();
  if ((dist_wire > -9.) && (std::abs(dist_wire) > (wire_pitch / 2))) {
    ATH_MSG_DEBUG("Distance to the nearest wire (" << std::abs(dist_wire) << ") is greater than expected.");
  }

  // Get the gamma pdf parameters associated with the distance of closest approach.
  GammaParameter gamma_par = getGammaParameter(std::abs(dist_wire));
  // Compute the most probable value of the gamma pdf
  double gamma_mpv = (gamma_par.kParameter - 1) * gamma_par.thetaParameter;
  // If the most probable value is near zero, then ensure it is zero
  if ((gamma_par.mostProbableTime) < 0.1) {gamma_mpv = 0.;}
  double t0_par = gamma_par.mostProbableTime - gamma_mpv;

  // Digit time follows a gamma distribution, so a value val is 
  // chosen using a gamma random generator then shifted by t0
  // to account for drift time.
  // Note: CLHEP::RandGamma takes the parameters k and lambda, 
  // where lambda = 1 / theta.
  double digit_time = t0_par + CLHEP::RandGamma::shoot(m_engine, gamma_par.kParameter, 1/gamma_par.thetaParameter);
  if (digit_time < 0.0) {
    // Ensure the digit time is positive
    digit_time = -1.0 * digit_time;
  }
  ATH_MSG_DEBUG("sTgcDigitMaker distance = " << dist_wire 
                << ", time = " << digit_time
                << ", k parameter = " << gamma_par.kParameter
                << ", theta parameter = " << gamma_par.thetaParameter
                << ", most probable time = " << gamma_par.mostProbableTime);

  //// HV efficiency correction
  if (m_doEfficiencyCorrection){
    Identifier tempId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, sTgcIdHelper::sTgcChannelTypes::Wire, 1, true);
    // Transform STL and STS to 0 and 1 respectively
    int stNameInt = (stationName=="STL") ? 0 : 1;
    // If inside eta0 bin of QL1/QS1, remove 1 from eta index
    int etaZero = detEl->isEtaZero(tempId, hitOnSurface_wire.y()) ? 1 : 0;
    float efficiency = getChamberEfficiency(stNameInt, std::abs(stationEta)-etaZero, stationPhi-1, multiPlet-1, gasGap-1);
    // Lose Hits to match HV efficiency
    if (CLHEP::RandFlat::shoot(m_engine,0.0,1.0) > efficiency) return nullptr;
  }

  //// Check the chamber is dead or not.
  if(isDeadChamber(stationName, stationEta, stationPhi, multiPlet, gasGap)) return nullptr;

  //***************************** check effeciency ******************************** 
  // use energyDeposit to implement detector effeciency 
  // Currently, we do not have a gap-by-gap minimum energy deposit threshold
  // If and when this is implemented, the value must be read from a database
  //if(!efficiencyCheck(stationName, stationEta, stationPhi, multiPlet, gasGap, 1, energyDeposit)) return 0; 
  
  IdentifierHash coll_hash;
  // contain (name, eta, phi, multiPlet)
  m_idHelper->get_detectorElement_hash(layid, coll_hash);
  //ATH_MSG_DEBUG(" looking up collection using hash " << (int)coll_hash << " " << m_idHelper->print_to_string(layid) );

  auto digits = std::make_unique<sTgcDigitCollection>(layid, coll_hash);

  bool isValid = 0;

  // #################################################################
  //***************************** BCTAGGER ******************************** 
  // #################################################################
  //m_timeWindowOffsetPad  = getTimeWindowOffset(stationName, stationEta, 2);
  //m_timeWindowOffsetStrip = getTimeWindowOffset(stationName, stationEta, 1);

  // use energyDeposit to implement detector effeciency 
  // Time of flight correction for each chamber
  // the offset is set to 0 for ASD located at the postion where tof is minimum in each chamber,
  // i.e. the ASD at the smallest R in each chamber
  //float tofCorrection = hit->globalPosition().mag()/CLHEP::c_light; //FIXME use CLHEP::c_light
  //float tofCorrection = globalHitTime; 

  //// bunch time
  //float bunchTime = globalHitTime - tofCorrection;


  //const float stripPropagationTime = 3.3*CLHEP::ns/CLHEP::m * detEl->distanceToReadout(posOnSurf_strip, elemId); // 8.5*ns/m was used until MC10. 
  const float stripPropagationTime = 0.; // 8.5*ns/m was used until MC10. 

  float sDigitTimeWire = digit_time;
  float sDigitTimePad = sDigitTimeWire + m_timeWindowOffsetPad;
  float sDigitTimeStrip = sDigitTimeWire + m_timeWindowOffsetStrip + stripPropagationTime;


  //if(m_doTimeCorrection) sDigitTime = bunchTime + timeJitterDetector + timeJitterElectronics + stripPropagationTime;
  //else {
  //  sDigitTime = timeJitterDetector + timeJitterElectronics + stripPropagationTime;
  //  ATH_WARNING("Did not do Time Correction for this hit!");
  //}

  uint16_t bctag = 0;

  //if(sDigitTime < -m_bunchCrossingTime+m_timeWindowOffsetStrip || +m_bunchCrossingTime+m_timeWindowOffsetStrip+ m_timeWindowStrip < sDigitTime) {
  //    ATH_MSG_DEBUG("Strip: Digitized time is outside of time window. " << sDigitTime
  //      	      << " bunchTime: " << bunchTime 
  //                    << " time jitter from detector response : " << timeJitterDetector  
  //                    << " time jitter from electronics response : " << timeJitterElectronics  
  //      	      << " propagation time: " << stripPropagationTime )
  //  return digits;
  //}

  //currently do not give any bcId information, just give the acurate time
  //bctag = bcTagging(sDigitTime, channelType);


  //##################################################################################
  //######################################### strip readout ##########################
  //##################################################################################
  ATH_MSG_DEBUG("sTgcDigitMaker::strip response ");
  int channelType = 1;

  Identifier newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, 1, true);

  // get strip surface 
  int surfHash_strip =  detEl->surfaceHash(gasGap, 1);
  const Trk::PlaneSurface& SURF_STRIP = detEl->surface(surfHash_strip); // get the strip surface

  Amg::Vector3D hitOnSurface_strip = SURF_STRIP.transform().inverse()*GPOS;

  Amg::Vector2D posOnSurf_strip(hitOnSurface_strip.x(),hitOnSurface_strip.y());
  bool insideBounds = SURF_STRIP.insideBounds(posOnSurf_strip);
  if(!insideBounds) { 
    ATH_MSG_DEBUG("Outside of the strip surface boundary : " <<  m_idHelper->print_to_string(newId) << "; local position " <<posOnSurf_strip ); 
    return nullptr;
  }

  //************************************ find the nearest readout element ************************************** 
       
  int stripNumber = detEl->stripNumber(posOnSurf_strip, newId);
  if( stripNumber == -1 ){
    ATH_MSG_ERROR("Failed to obtain strip number " << m_idHelper->print_to_string(newId) );
    ATH_MSG_ERROR(" pos " << posOnSurf_strip );
    //stripNumber = 1;
  }  
  newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripNumber, true, &isValid);
  if(!isValid && stripNumber != -1) {
    ATH_MSG_ERROR("Failed to obtain identifier " << m_idHelper->print_to_string(newId) ); 
    return nullptr;
  }

  int NumberOfStrips = detEl->numberOfStrips(newId); 
  double stripHalfWidth = detEl->channelPitch(newId)*0.5; // 3.2/2 = 1.6 mm

  //************************************ conversion of energy to charge **************************************

  // Constant determined from ionization study within Garfield
  // Note titled Charge Energy Relation which outlines conversion can be found here https://cernbox.cern.ch/index.php/apps/files/?dir=/__myshares/Documents (id:274113) 
  double ionized_charge = (5.65E-6)*energyDeposit/CLHEP::keV; // initial ionized charge in pC per keV deposited

 // To get avalanche gain, polya function is taken from Blum paper https://inspirehep.net/literature/807304
 // m_polyaFunction = new TF1("m_polyaFunction","(1.0/[1])*(TMath::Power([0]+1,[0]+1)/TMath::Gamma([0]+1))*TMath::Power(x/[1],[0])*TMath::Exp(-([0]+1)*x/[1])",0,3000000);
  float gain =  CLHEP::RandGamma::shoot(m_engine, 1. + m_theta, (1. + m_theta)/m_mean); // mean value for total gain due to E field; To calculate this gain from polya distibution we replace "alpha = 1+theta and beta = 1+theta/mean" in gamma PDF. With this gamma PDF gives us same sampling values as we get from polya PDF. 
  double total_charge = gain*ionized_charge; // total charge after avalanche

  //************************************ spread charge among readout element ************************************** 
  //spread charge to a gaussian distribution
  float charge_width = CLHEP::RandGaussZiggurat::shoot(m_engine, m_GausMean, m_GausSigma);
  float norm = 0.5*total_charge/(charge_width*std::sqrt(2.*M_PI)); // each readout plane reads about half the total charge produced on the wire
  TF1 *charge_spread = new TF1("fgaus", "gaus(0)", -1000., 1000.); 
  charge_spread->SetParameters(norm, posOnSurf_strip.x(), charge_width);
  
  // Charge Spread including tan(theta) resolution term.
  double tan_theta = GLODIRE.perp()/GLODIRE.z();
  // The angle dependance on strip resolution goes as tan^2(angle)
  // We add the resolution in quadrature following sTGC test beam papers
  m_ChargeSpreadFactor = m_StripResolution*std::sqrt(1+12.*tan_theta*tan_theta);

  int stripnum = -1;
  for(int neighbor=0; neighbor<=3; neighbor++){  // spread the charge to 7 strips for the current algorithm
    stripnum = stripNumber+neighbor;
    if(stripnum>=1&&stripnum<=NumberOfStrips){
      newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripnum, true, &isValid);
      if(isValid) {
        Amg::Vector2D locpos(0,0);
        if( !detEl->stripPosition(newId,locpos)){
          ATH_MSG_WARNING("Failed to obtain local position for identifier " << m_idHelper->print_to_string(newId) );
        }

        float xmax = locpos.x() + stripHalfWidth;
        float xmin = locpos.x() - stripHalfWidth;
        float charge = charge_spread->Integral(xmin, xmax);
        charge = CLHEP::RandGaussZiggurat::shoot(m_engine, charge, m_ChargeSpreadFactor*charge);

        addDigit(digits.get(),newId, bctag, sDigitTimeStrip, charge, channelType);
        // For now, we remove the cross talk since we have no measurements to use
        // Currently, adding a few % cross talk only muddies our current set of studies
        // Keeping this code for posteriority in case we only get crosstalk values after I no longer work on NSW digi
        // i.e to make sure my replacement knows where to find cross talk
        // Alexandre Laurier - April 13 2021
        //************************************** introduce cross talk ************************************************
        //for(int crosstalk=1; crosstalk<=3; crosstalk++){ // up to the third nearest neighbors
        //  if((stripnum-crosstalk)>=1&&(stripnum-crosstalk)<=NumberOfStrips){
        //    newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripnum-crosstalk, true, &isValid);
        //    if(isValid) addDigit(digits.get(), newId, bctag, sDigitTimeStrip, charge*std::pow(m_CrossTalk, crosstalk), channelType);
        //  }
        //  if((stripnum+crosstalk)>=1&&(stripnum+crosstalk)<=NumberOfStrips){
        //    newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripnum+crosstalk, true, &isValid);
        //    if(isValid) addDigit(digits.get(),newId, bctag, sDigitTimeStrip, charge*std::pow(m_CrossTalk, crosstalk), channelType);
        //  }
        //}// end of introduce cross talk
      } // end isValid
    }// end of when stripnum = stripNumber+neighbor
 
    if(neighbor==0) continue;
    stripnum = stripNumber-neighbor;
    if(stripnum>=1&&stripnum<=NumberOfStrips){
      newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripnum, true, &isValid);
      if(isValid) {
        Amg::Vector2D locpos(0,0);
        if( !detEl->stripPosition(newId,locpos)){
          ATH_MSG_WARNING("Failed to obtain local position for identifier " << m_idHelper->print_to_string(newId) );
        }
        float xmax = locpos.x() + stripHalfWidth;
        float xmin = locpos.x() - stripHalfWidth;
        float charge = charge_spread->Integral(xmin, xmax);
        charge = CLHEP::RandGaussZiggurat::shoot(m_engine, charge, m_ChargeSpreadFactor*charge);

        addDigit(digits.get(),newId, bctag, sDigitTimeStrip, charge, channelType);
        // For now, we remove the cross talk since we have no measurements to use
        // Currently, adding a few % cross talk only muddies our current set of studies
        // Keeping this code for posteriority in case we only get crosstalk values after I no longer work on NSW digi
        // i.e to make sure my replacement knows where to find cross talk
        // Alexandre Laurier - April 13 2021
        //************************************** introduce cross talk ************************************************
        //for(int crosstalk=1; crosstalk<=3; crosstalk++){ // up to the third nearest neighbors
        //  if((stripnum-crosstalk)>=1&&(stripnum-crosstalk)<=NumberOfStrips){
        //   newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripnum-crosstalk, true, &isValid);
        //   if(isValid) addDigit(digits.get(),newId, bctag, sDigitTimeStrip, charge*std::pow(m_CrossTalk, crosstalk), channelType);
        //  }
        //  if((stripnum+crosstalk)>=1&&(stripnum+crosstalk)<=NumberOfStrips){
        //    newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, stripnum+crosstalk, true, &isValid);
        //    if(isValid) addDigit(digits.get(),newId, bctag, sDigitTimeStrip, charge*std::pow(m_CrossTalk, crosstalk), channelType);
        //  }
        //}// end of introduce cross talk
      } // end isValid
    }// end of when stripnum = stripNumber-neighbor
  }//end for spread the charge to 5 strips 

  delete charge_spread;
  // end of strip digitization

  if(m_channelTypes==1) {
    ATH_MSG_WARNING("Only digitize strip response !");
    return digits;
  }

  //##################################################################################
  //######################################### pad readout ##########################
  //##################################################################################
  ATH_MSG_DEBUG("sTgcDigitMaker::pad response ");
  channelType = 0;
  
  //************************************ find the nearest readout element ************************************** 
  int  surfHash_pad =  detEl->surfaceHash(gasGap, 0);
  const Trk::PlaneSurface& SURF_PAD = detEl->surface(surfHash_pad); // get the pad surface

  Amg::Vector3D hitOnSurface_pad = SURF_PAD.transform().inverse()*GPOS;
  Amg::Vector2D posOnSurf_pad(hitOnSurface_pad.x(), hitOnSurface_pad.y());

  Identifier PAD_ID = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, 0, 1, true);// find the a pad id

  insideBounds = SURF_PAD.insideBounds(posOnSurf_pad);

  if(insideBounds) { 
    int padNumber = detEl->stripNumber(posOnSurf_pad, PAD_ID);
    if( padNumber == -1 ){
      ATH_MSG_ERROR("Failed to obtain pad number " << m_idHelper->print_to_string(PAD_ID) );
      ATH_MSG_ERROR(" pos " << posOnSurf_pad );
      //padNumber = 1;
    }  
    newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, padNumber, true, &isValid);
    if(isValid) {  
      addDigit(digits.get(), newId, bctag, sDigitTimePad, channelType);
    }
    else if(padNumber != -1) {  
      ATH_MSG_ERROR("Failed to obtain identifier " << m_idHelper->print_to_string(newId) ); 
    }
  }
  else {
    ATH_MSG_DEBUG("Outside of the pad surface boundary :" << m_idHelper->print_to_string(PAD_ID)<< " local position " <<posOnSurf_pad ); 
  }
  
  if(m_channelTypes==2) {
    ATH_MSG_WARNING("Only digitize strip/pad response !");
    return digits;
  }
   
 
  //##################################################################################
  //######################################### wire readout ##########################
  //##################################################################################
  ATH_MSG_DEBUG("sTgcDigitMaker::wire response ");
  channelType = 2;

    // Find the ID of the first wiregroup
    Identifier WIREGP_ID = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, 1, true);
        
    //************************************ find the nearest readout element ************************************** 
    insideBounds = SURF_WIRE.insideBounds(posOnSurf_wire);

    if(insideBounds) {
        // Determine the wire number

        int wiregroupNumber = detEl->stripNumber(posOnSurf_wire, WIREGP_ID);
        if( wiregroupNumber == -1 ){
          ATH_MSG_ERROR("Failed to obtain wire number " << m_idHelper->print_to_string(WIREGP_ID) );
          ATH_MSG_ERROR(" pos " << posOnSurf_wire );
        }
  
        // Find ID of the actual wiregroup
        newId = m_idHelper->channelID(m_idHelper->parentID(layid), multiPlet, gasGap, channelType, wiregroupNumber, true, &isValid);
  
        if(isValid) {
          int NumberOfWiregroups = detEl->numberOfStrips(newId); // 0 --> pad, 1 --> strip, 2 --> wire
          if(wiregroupNumber>=1&&wiregroupNumber<=NumberOfWiregroups) addDigit(digits.get(), newId, bctag, sDigitTimeWire, channelType);
        } // end of if(isValid)
        else if (wiregroupNumber != -1){
          ATH_MSG_ERROR("Failed to obtain wiregroup identifier " << m_idHelper->print_to_string(newId) );
        }
    }
    else {
      ATH_MSG_DEBUG("Outside of the wire surface boundary :" << m_idHelper->print_to_string(WIREGP_ID)<< " local position " <<posOnSurf_wire );
    }
    // end of wire digitization

  return digits;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
double sTgcDigitMaker::distanceToWire(Amg::Vector3D& position, Amg::Vector3D& direction, Identifier id, int wire_number) const
{
  // Wire number should be one or greater
  if (wire_number < 1) {
    return -9.99;
  }

  // Get the current sTGC element (a four-layer chamber)
  const MuonGM::sTgcReadoutElement* detEl = m_mdManager->getsTgcReadoutElement(id);

  // Wire number too large
  if (wire_number > detEl->numberOfWires(id)) {
    return -9.99;
  }

  // Wire pitch
  double wire_pitch = detEl->wirePitch();
  // Wire local position on the wire plane, the y-coordinate is arbitrary and z-coordinate is zero
  double wire_posX = detEl->positionFirstWire(id) + (wire_number - 1) * wire_pitch;
  Amg::Vector3D wire_position(wire_posX, position.y(), 0.);
  // The wires are parallel to Y in the wire plane's coordinate frame
  Amg::Vector3D wire_direction(0., 1., 0.);

  // Determine the sign of the distance, which is: 
  //  - negative if particle crosses the wire surface on the wire_number-1 side and 
  //  + positive if particle crosses the wire surface on the wire_number+1 side
  double sign = 1.0;
  if ((position.x() - wire_posX) < 0.) {
    sign = -1.0;
  }

  // Distance of closest approach is the distance between the two lines: 
  //      - particle's segment
  //      - wire line

  // Find a line perpendicular to both hit direction and wire direction
  Amg::Vector3D perp_line = direction.cross(wire_direction);
  double norm_line = std::sqrt(perp_line.dot(perp_line));
  if (norm_line < 1.0e-5) {
    ATH_MSG_WARNING("Unable to compute the distance of closest approach," 
                    << " a negative value is assumed to indicate the error.");
    return -9.99;
  }
  // Compute the distance of closest approach, which is given by the projection of 
  // the vector going from hit position to wire position onto the perpendicular line
  double distance = std::abs((position - wire_position).dot(perp_line) / norm_line);
     
  return (sign * distance);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void sTgcDigitMaker::readFileOfTimeJitter()
{

  const char* const fileName = "sTGC_Digitization_timejitter.dat";
  std::string fileWithPath = PathResolver::find_file (fileName, "DATAPATH");

  std::ifstream ifs;
  if (!fileWithPath.empty()) {
    ifs.open(fileWithPath.c_str(), std::ios::in);
  }
  else {
    ATH_MSG_FATAL("readFileOfTimeJitter(): Could not find file " << fileName );
    exit(-1);
  }

  if(ifs.bad()){
    ATH_MSG_FATAL("readFileOfTimeJitter(): Could not open file "<< fileName );
    exit(-1);
  }

  int angle = 0;
  int bins = 0;
  int i = 0;
  float prob = 0.;

  while(ifs.good()){
    ifs >> angle >> bins;
    if (ifs.eof()) break;
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "readFileOfTimeJitter(): Timejitter, angle, Number of bins, prob. dist.: " << angle << " " << bins << " ";
    m_vecAngle_Time.resize(i + 1);
    for (int j = 0; j < 41/*bins*/; j++) {
      ifs >> prob;
      m_vecAngle_Time[i].push_back(prob);
      if (j == 0)
        if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "readFileOfTimeJitter(): ";
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << prob << " ";
    }
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << endmsg;
    i++;
  }
  ifs.close();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
float sTgcDigitMaker::timeJitter(float inAngle_time) const
{

  int   ithAngle = static_cast<int>(inAngle_time/5.);
  float wAngle = inAngle_time/5. - static_cast<float>(ithAngle);
  int   jthAngle;
  if (ithAngle > 11) {
    ithAngle = 12;
    jthAngle = 12;
  }
  else {
    jthAngle = ithAngle+1;
  }

  float jitter;
  float prob = 1.;
  float probRef = 0.;

  while (prob > probRef) {
    prob   = CLHEP::RandFlat::shoot(m_engine, 0.0, 1.0);
    jitter = CLHEP::RandFlat::shoot(m_engine, 0.0, 1.0)*40.; // trial time jitter in nsec
    int ithJitter = static_cast<int>(jitter);
    // probability distribution calculated from weighted sum between neighboring bins of angles
    probRef = (1.-wAngle)*m_vecAngle_Time[ithAngle][ithJitter]
      +    wAngle *m_vecAngle_Time[jthAngle][ithJitter];
  }

  ATH_MSG_VERBOSE("sTgcDigitMaker::timeJitter : angle = " << inAngle_time 
                  << ";  timeJitterDetector = " << jitter );

  return jitter;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
bool sTgcDigitMaker::efficiencyCheck(const int channelType) const {
  if(channelType == 0) { // wire group
    if(CLHEP::RandFlat::shoot(m_engine,0.0,1.0) < m_efficiencyOfWireGangs) return true;
  }
  else if(channelType == 1) { // strip
    if(CLHEP::RandFlat::shoot(m_engine,0.0,1.0) < m_efficiencyOfStrips) return true;
  }
  ATH_MSG_DEBUG("efficiencyCheck(): Hit removed. channelType: " << channelType );
  return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++
bool sTgcDigitMaker::efficiencyCheck(const std::string& stationName, const int stationEta, const int stationPhi,const int multiPlet, const int gasGap, const int channelType, const double energyDeposit) const {
  // If the energy deposit is equal to or greater than the threshold value of the chamber,
  // return true.
  return (energyDeposit >= getEnergyThreshold(stationName, stationEta, stationPhi, multiPlet, gasGap, channelType));
}
//+++++++++++++++++++++++++++++++++++++++++++++++
//uint16_t sTgcDigitMaker::bcTagging(const float digitTime, const int channelType) const {
//
//  uint16_t bctag = 0;
//
//  double offset, window;
//  if(channelType == 1) { //strips 
//    offset = m_timeWindowOffsetStrip;
//    window = m_timeWindowStrip;
//  }
//  else { // wire gangs or pads
//    offset = m_timeWindowOffsetPad;
//    window = m_timeWindowPad;
//  }
//
//  if(-m_bunchCrossingTime+offset < digitTime && digitTime < -m_bunchCrossingTime+offset+window) {
//    bctag = (bctag | 0x1);
//  }
//  if(                     offset < digitTime && digitTime <                      offset+window) {
//    bctag = (bctag | 0x2);
//  }
//  if(+m_bunchCrossingTime+offset < digitTime && digitTime < +m_bunchCrossingTime+offset+window) {
//    bctag = (bctag | 0x4);
//  }
//
//  return bctag;
//}
//+++++++++++++++++++++++++++++++++++++++++++++++
void sTgcDigitMaker::addDigit(sTgcDigitCollection* digits, const Identifier id, const uint16_t bctag, const float digittime, int channelType) const {

  if(channelType!=0&&channelType!=2) {
    ATH_MSG_WARNING("Wrong sTgcDigit object with channelType" << channelType );
  }

  bool duplicate = false;
  for(sTgcDigitCollection::const_iterator it=digits->begin(); it!=digits->end(); ++it) {
    if(id==(*it)->identify() && digittime==(*it)->time()) {
      duplicate = true;
      break;
    }
  }
  if(!duplicate) {
    digits->push_back(new sTgcDigit(id, bctag, digittime, -1, 0, 0));
  }

  return;
}

void sTgcDigitMaker::addDigit(sTgcDigitCollection* digits, const Identifier id, const uint16_t bctag, const float digittime, float charge, int channelType) const {

  if(channelType!=1) {
    ATH_MSG_WARNING("Wrong sTgcDigit object with channelType" << channelType );
  }

  bool duplicate = false;
  for(sTgcDigitCollection::iterator it=digits->begin(); it!=digits->end(); ++it) {
    if(id==(*it)->identify() && digittime==(*it)->time()) {
      (*it)->set_charge(charge+(*it)->charge());  
      duplicate = true;
      break;
    }
  }
  if(!duplicate) {
    digits->push_back(new sTgcDigit(id, bctag, digittime, charge, 0, 0));
  }

  return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
void sTgcDigitMaker::readFileOfEnergyThreshold() {
  // Indices to be used
  int iStationName, stationEta, stationPhi, multiPlet, gasGap, channelType;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(stationPhi=0; stationPhi<N_STATIONPHI; stationPhi++) {
        for(multiPlet=0; multiPlet<N_MULTIPLET; multiPlet++) {
          for(gasGap=0; gasGap<N_GASGAP; gasGap++) {
            for(channelType=0; channelType<N_CHANNELTYPE; channelType++) {
              m_energyThreshold[iStationName][stationEta][stationPhi][multiPlet][gasGap][channelType] = 0.00052; //MeV
            }
          }
        }
      }
    }
  }

  // Find path to the sTGC_Digitization_energyThreshold.dat file
  const std::string fileName = "sTGC_Digitization_energyThreshold.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath.empty()) {
    ATH_MSG_FATAL("readFileOfEnergyThreshold(): Could not find file " << fileName.c_str() );
    return;
  }

  // Open the sTGC_Digitization_energyThreshold.dat file
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfEnergyThreshold(): Could not open file " << fileName.c_str() );
    return;
  }

  double energyThreshold;
  // Read the sTGC_Digitization_energyThreshold.dat file
  while(ifs.good()&& !ifs.eof() ) {
    ifs >> iStationName >> stationEta
        >> stationPhi   >> multiPlet
        >> gasGap >> channelType >> energyThreshold;
    ATH_MSG_DEBUG(  "sTgcDigitMaker::readFileOfEnergyThreshold"
                    << " stationName= " << iStationName
                    << " stationEta= " << stationEta
                    << " stationPhi= " << stationPhi
                    << " multiPlet= " << multiPlet
                    << " gasGap= " << gasGap
                    << " channelType= " << channelType
                    << " energyThreshold(MeV)= " << energyThreshold );

    // Subtract offsets to use indices of energyThreshold array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    stationPhi   -= OFFSET_STATIONPHI;
    multiPlet    -= OFFSET_MULTIPLET;
    gasGap       -= OFFSET_GASGAP;
    channelType  -= OFFSET_CHANNELTYPE;

    // Check the indices are valid
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) continue;
    if(multiPlet   <0 || multiPlet   >=N_MULTIPLET  ) continue;
    if(gasGap      <0 || gasGap      >=N_GASGAP     ) continue;
    if(channelType <0 || channelType >=N_CHANNELTYPE    ) continue;

    m_energyThreshold[iStationName][stationEta][stationPhi][multiPlet][gasGap][channelType] = energyThreshold;

    // If it is the end of the file, get out from while loop.
    if(ifs.eof()) break;
  }

  // Close the sTGC_Digitization_energyThreshold.dat file
  ifs.close();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
void sTgcDigitMaker::readFileOfDeadChamber() {
  // Indices to be used
  int iStationName, stationEta, stationPhi, multiPlet, gasGap;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(stationPhi=0; stationPhi<N_STATIONPHI; stationPhi++) {
        for(multiPlet=0; multiPlet<N_MULTIPLET; multiPlet++) {
          for(gasGap=0; gasGap<N_GASGAP; gasGap++) {
            m_isDeadChamber[iStationName][stationEta][stationPhi][multiPlet][gasGap] = false;
          }
        } 
      }
    }
  }

  // Find path to the sTGC_Digitization_deadChamber.dat file
  const std::string fileName = "sTGC_Digitization_deadChamber.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath.empty()) {
    ATH_MSG_FATAL("readFileOfDeadChamber(): Could not find file " << fileName.c_str() );
    return;
  }

  // Open the sTGC_Digitization_deadChamber.dat file
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfDeadChamber(): Could not open file " << fileName.c_str() );
    return;
  }

  // Read the sTGC_Digitization_deadChamber.dat file
  unsigned int nDeadChambers = 0;
  std::string comment;
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> stationPhi >> multiPlet >> gasGap;
    bool valid = getline(ifs, comment).good();
    if(!valid) break;

    ATH_MSG_DEBUG( "sTgcDigitMaker::readFileOfDeadChamber"
                    << " stationName= " << iStationName
                    << " stationEta= " << stationEta
                    << " stationPhi= " << stationPhi
                    << " multiPlet= "  << multiPlet 
                    << " gasGap= " << gasGap
                    << " comment= " << comment );

    // Subtract offsets to use indices of isDeadChamber array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    stationPhi   -= OFFSET_STATIONPHI;
    multiPlet    -= OFFSET_MULTIPLET;
    gasGap       -= OFFSET_GASGAP;

    // Check the indices are valid
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) continue;
    if(multiPlet   <0 || multiPlet   >=N_MULTIPLET  ) continue;
    if(gasGap      <0 || gasGap      >=N_GASGAP     ) continue;

    m_isDeadChamber[iStationName][stationEta][stationPhi][multiPlet][gasGap] = true;
    nDeadChambers++;

    // If it is the end of the file, get out from while loop.
    if(ifs.eof()) break;
  }

  // Close the sTGC_Digitization_deadChamber.dat file
  ifs.close();

  ATH_MSG_INFO("sTgcDigitMaker::readFileOfDeadChamber: the number of dead chambers = " << nDeadChambers );
}

void sTgcDigitMaker::readFileOfEffChamber() {
  // Indices to be used
  int iStationName, stationEta, stationPhi, multiPlet, gasGap;
  float eff;

  for(iStationName=0; iStationName<2; iStationName++) { // Small - Large
    for(stationEta=0; stationEta<4; stationEta++) { // 4 eta
      for(stationPhi=0; stationPhi<8; stationPhi++) { // 8 phi sectors
        for(multiPlet=0; multiPlet<2; multiPlet++) { // pivot- confirm
          for(gasGap=0; gasGap<4; gasGap++) { // 4 layers
            m_ChamberEfficiency[iStationName][stationEta][stationPhi][multiPlet][gasGap] = 1.;
          }
        } 
      }
    }
  }

  // Find path to the sTGC_Digitization_EffChamber.dat file
  const std::string fileName = "sTGC_Digitization_EffChamber.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath.empty()) {
    ATH_MSG_FATAL("readFileOfEffChamber(): Could not find file " << fileName.c_str() );
    return;
  }

  // Open the sTGC_Digitization_EffChamber.dat file
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfEffChamber(): Could not open file " << fileName.c_str() );
    return;
  }

  // Read the sTGC_Digitization_EffChamber.dat file
  /* Each line has 6 values:
     value #1 : Large (0) or Small (1)
     value #2 : Eta 0,1,2,3 : Mirroring A side efficiency to C side for now.
     value #3 : Phi 1 to 8
     value #4 : Multiplet 0 for Large pivot or small confirm.
                Multiplet 1 for Large Confirm or Small Pivot
     value #5 : gasGap (1-4)
     value #6 : Efficiency
  */
  unsigned int nDeadChambers = 0;
  std::string comment;
  // This is just to skip the first line which describes the format
  getline(ifs, comment);
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> stationPhi >> multiPlet >> gasGap >> eff;
    bool valid = getline(ifs, comment).good();
    if(!valid) break;

    ATH_MSG_DEBUG( "sTgcDigitMaker::readFileOfEffChamber"
                    << " stationName= " << iStationName
                    << " stationEta= " << stationEta
                    << " stationPhi= " << stationPhi
                    << " multiPlet= "  << multiPlet
                    << " gasGap= " << gasGap
                    << " efficiency= " << eff
                    << " comment= " << comment );

    // Subtract offsets to use indices of efficiency array
    stationPhi = stationPhi - 1;
    gasGap = gasGap - 1;

    // Check the indices are valid
    if(iStationName<0 || iStationName>=2) continue;
    if(stationEta  <0 || stationEta  >=4 ) continue;
    if(stationPhi  <0 || stationPhi  >=8 ) continue;
    if(multiPlet   <0 || multiPlet   >=2  ) continue;
    if(gasGap      <0 || gasGap      >=4     ) continue;

    m_ChamberEfficiency[iStationName][stationEta][stationPhi][multiPlet][gasGap] = eff;
    if (eff==0) nDeadChambers++;

    // If it is the end of the file, get out from while loop.
    if(ifs.eof()) break;
  }

  // Close the sTGC_Digitization_deadChamber.dat file
  ifs.close();

  ATH_MSG_INFO("sTgcDigitMaker::readFileOfEffChamber: the number of dead chambers = " << nDeadChambers );
}
//+++++++++++++++++++++++++++++++++++++++++++++++
void sTgcDigitMaker::readFileOfTimeWindowOffset() {
  // Indices to be used
  int iStationName, stationEta, channelType;

  for(iStationName=0; iStationName<N_STATIONNAME; iStationName++) {
    for(stationEta=0; stationEta<N_STATIONETA; stationEta++) {
      for(channelType=0; channelType<N_CHANNELTYPE; channelType++) {
        m_timeWindowOffset[iStationName][stationEta][channelType] = 0.;
      }
    }
  }

  // Find path to the sTGC_Digitization_timeWindowOffset.dat file
  const std::string fileName = "sTGC_Digitization_timeWindowOffset.dat";
  std::string fileWithPath = PathResolver::find_file(fileName.c_str(), "DATAPATH");
  if(fileWithPath.empty()) {
    ATH_MSG_FATAL("readFileOfTimeWindowOffset(): Could not find file " << fileName.c_str() );
    return;
  }

  // Open the sTGC_Digitization_timeWindowOffset.dat file
  std::ifstream ifs;
  ifs.open(fileWithPath.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("readFileOfTimeWindowOffset(): Could not open file " << fileName.c_str() );
    return;
  }

  // Read the sTGC_Digitization_timeWindowOffset.dat file
  double timeWindowOffset;
  while(ifs.good()) {
    ifs >> iStationName >> stationEta >> channelType >> timeWindowOffset;
    ATH_MSG_DEBUG(  "sTgcDigitMaker::readFileOfTimeWindowOffset"
                    << " stationName= " << iStationName
                    << " stationEta= " << stationEta
                    << " channelType= " << channelType
                    << " timeWindowOffset= " << timeWindowOffset );

    // Subtract offsets to use indices of isDeadChamber array
    iStationName -= OFFSET_STATIONNAME;
    stationEta   -= OFFSET_STATIONETA;
    channelType  -= OFFSET_CHANNELTYPE;

    // Check the indices are valid
    if(iStationName<0 || iStationName>=N_STATIONNAME) continue;
    if(stationEta  <0 || stationEta  >=N_STATIONETA ) continue;
    if(channelType     <0 || channelType     >=N_CHANNELTYPE    ) continue;

    m_timeWindowOffset[iStationName][stationEta][channelType] = timeWindowOffset;

    // If it is the end of the file, get out from while loop.
    if(ifs.eof()) break;
  }

  // Close the sTGC_Digitization_timeWindowOffset.dat file
  ifs.close();
}



//+++++++++++++++++++++++++++++++++++++++++++++++
double sTgcDigitMaker::getEnergyThreshold(const std::string& stationName, int stationEta, int stationPhi, int multiPlet, int gasGap, int channelType) const {
  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  stationPhi   -= OFFSET_STATIONPHI;
  multiPlet    -= OFFSET_MULTIPLET;
  gasGap       -= OFFSET_GASGAP;
  channelType  -= OFFSET_CHANNELTYPE;

  double energyThreshold = +999999.;

  // If the indices are valid, the energyThreshold array is fetched.
  if((iStationName>=0 && iStationName<N_STATIONNAME) &&
     (stationEta  >=0 && stationEta  <N_STATIONETA ) &&
     (stationPhi  >=0 && stationPhi  <N_STATIONPHI ) &&
     (multiPlet   >=0 && multiPlet   <N_MULTIPLET ) &&
     (gasGap      >=0 && gasGap      <N_GASGAP     ) &&
     (channelType >=0 && channelType <N_CHANNELTYPE    )) {
    energyThreshold = m_energyThreshold[iStationName][stationEta][stationPhi][multiPlet][gasGap][channelType];
  }

  // Show the energy threshold value
  ATH_MSG_VERBOSE( "sTgcDigitMaker::getEnergyThreshold"
                    << " stationName= " << iStationName+OFFSET_STATIONNAME
                    << " stationEta= " << stationEta+OFFSET_STATIONETA
                    << " stationPhi= " << stationPhi+OFFSET_STATIONPHI
                    << " multiPlet= " << multiPlet+OFFSET_MULTIPLET
                    << " gasGap= "      << gasGap+OFFSET_GASGAP
                    << " channelType= " << channelType+OFFSET_CHANNELTYPE
                    << " energyThreshold(MeV)= " << energyThreshold );

  return energyThreshold;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
bool sTgcDigitMaker::isDeadChamber(const std::string& stationName, int stationEta, int stationPhi, int multiPlet, int gasGap) {
  bool v_isDeadChamber = true;

  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  stationPhi   -= OFFSET_STATIONPHI;
  multiPlet    -= OFFSET_MULTIPLET;
  gasGap       -= OFFSET_GASGAP;

  // If the indices are valid, the energyThreshold array is fetched.
  if((iStationName>=0 && iStationName<N_STATIONNAME) &&
     (stationEta  >=0 && stationEta  <N_STATIONETA ) &&
     (stationPhi  >=0 && stationPhi  <N_STATIONPHI ) &&
     (multiPlet   >=0 && multiPlet   <N_MULTIPLET  ) && 
     (gasGap      >=0 && gasGap      <N_GASGAP     )) {
    v_isDeadChamber = m_isDeadChamber[iStationName][stationEta][stationPhi][multiPlet][gasGap];
  }

  // Show the energy threshold value
  ATH_MSG_VERBOSE(  "sTgcDigitMaker::isDeadChamber"
                    << " stationName= " << iStationName+OFFSET_STATIONNAME
                    << " stationEta= " << stationEta+OFFSET_STATIONETA
                    << " stationPhi= " << stationPhi+OFFSET_STATIONPHI
                    << " multiPlet = " << multiPlet +OFFSET_MULTIPLET
                    << " gasGap= " << gasGap+OFFSET_GASGAP
                    << " isDeadChamber= " << v_isDeadChamber );

  return v_isDeadChamber;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
float sTgcDigitMaker::getChamberEfficiency(int stationName, int stationEta, int stationPhi, int multiPlet, int gasGap) {

  // If the indices are valid, the energyThreshold array is fetched.
  if((stationName>=0 && stationName<2 ) &&
     (stationEta  >=0 && stationEta<4 ) &&
     (stationPhi  >=0 && stationPhi<8 ) &&
     (multiPlet   >=0 && multiPlet<2  ) &&
     (gasGap      >=0 && gasGap<4     )) {
    return m_ChamberEfficiency[stationName][stationEta][stationPhi][multiPlet][gasGap];
  }
  else ATH_MSG_INFO("sTGC getChamberEfficiency bug! Indexes not ok!");

  return 1.;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
double sTgcDigitMaker::getTimeWindowOffset(const std::string& stationName, int stationEta, int channelType) const {
  // Convert std::string stationName to int iStationName from 41 to 48
  int iStationName = getIStationName(stationName);

  // Subtract offsets to use these as the indices of the energyThreshold array
  iStationName -= OFFSET_STATIONNAME;
  stationEta   -= OFFSET_STATIONETA;
  channelType  -= OFFSET_CHANNELTYPE;

  // Check the indices are valid
  if(iStationName<0 || iStationName >=N_STATIONNAME) return 0.;
  if(stationEta  <0 || stationEta   >=N_STATIONETA ) return 0.;
  if(channelType <0 || channelType  >=N_CHANNELTYPE    ) return 0.;

  // Values were determined to reproduce the fraction of Previous BC hit fraction in Z->mumu data during Periods B,D,E in 2011.
  return m_timeWindowOffset[iStationName][stationEta][channelType];
}

int sTgcDigitMaker::getIStationName(const std::string& stationName) const {
  int iStationName = 0;
  if(     stationName=="STS") iStationName = 0;
  else if(stationName=="STL") iStationName = 1;

  return iStationName;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
void sTgcDigitMaker::readFileOfTimeArrival() {
  // Verify the file sTGC_Digitization_timeArrival.dat exists
  const std::string file_name = "sTGC_Digitization_timeArrival.dat";
  std::string file_path = PathResolver::find_file(file_name.c_str(), "DATAPATH");
  if(file_path.empty()) {
    ATH_MSG_FATAL("readFileOfTimeWindowOffset(): Could not find file " << file_name.c_str() );
    return;
  }

  // Open the sTGC_Digitization_timeArrival.dat file
  std::ifstream ifs;
  ifs.open(file_path.c_str(), std::ios::in);
  if(ifs.bad()) {
    ATH_MSG_FATAL("sTgcDigitMaker: Failed to open time of arrival file " << file_name.c_str() );
    return;
  }

  // Read the sTGC_Digitization_timeWindowOffset.dat file
  std::string line;
  GammaParameter param;

  while (std::getline(ifs, line)) {
    std::string key;
    std::istringstream iss(line);
    iss >> key;
    if (key.compare("bin") == 0) {
      iss >> param.lowEdge >> param.kParameter >> param.thetaParameter >> param.mostProbableTime;
      m_gammaParameter.push_back(param);
    } 
  }

  // Close the file
  ifs.close();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
sTgcDigitMaker::GammaParameter sTgcDigitMaker::getGammaParameter(double distance) const {

  double d = distance;
  if (d < 0.) {
    ATH_MSG_WARNING("getGammaParameter: expecting a positive distance, but got negative value: " << d
                     << ". Proceed to the calculation with the absolute value.");
    d = -1.0 * d;
  }

  // Find the parameters assuming the container is sorted
  int index{-1};
  for (auto& par: m_gammaParameter) {
    if (distance < par.lowEdge) {
      break;
    }
    ++index;
  }
  return m_gammaParameter.at(index);
}

////+++++++++++++++++++++++++++++++++++++++++++++++
//void sTgcDigitMaker::adHocPositionShift(const std::string stationName, int stationEta, int stationPhi,
//                                       const Amg::Vector3D direCos, Amg::Vector3D &localPos) const {
//  int iStationName = getIStationName(stationName);
//  iStationName -= OFFSET_STATIONNAME;
//  stationEta   -= OFFSET_STATIONETA;
//  stationPhi   -= OFFSET_STATIONPHI;
//  // Check the indices are valid
//  if(iStationName<0 || iStationName>=N_STATIONNAME) return;
//  if(stationEta  <0 || stationEta  >=N_STATIONETA ) return;
//  if(stationPhi  <0 || stationPhi  >=N_STATIONPHI ) return;
//
//  // Local +x (-x) direction is global +z direction on A-side (C-side).
//  double localDisplacementX = m_alignmentT[iStationName][stationEta][stationPhi];
//
//  // Local +z direction is global +r direction.
//  double localDisplacementZ = m_alignmentZ[iStationName][stationEta][stationPhi];
//
//  // Local +s direction is global +phi direction.
//  double localDisplacementY = m_alignmentS[iStationName][stationEta][stationPhi];
//
//  // Rotation around the s-axis is not implemented yet (2011/11/29).
//  // m_alignmentTHS[tmpStationName][tmpStationEta][tmpStationPhi];
//
//  // Convert local x translation to local y and z translations
//  double localDisplacementYByX = 0.;
//  double localDisplacementZByX = 0.;
//  if(fabs(localDisplacementX)>1.0E-12) { // local y and z translations are non-zero only if local x translation is non-zero.
//    if(fabs(direCos[0])<1.0E-12) return; // To avoid zero-division
//    localDisplacementYByX = direCos[1]/direCos[0]*localDisplacementX;
//    localDisplacementZByX = direCos[2]/direCos[0]*localDisplacementX;
//  }
//  localPos.y() = localPos.y()+localDisplacementYByX+localDisplacementY;
//  localPos.z() = localPos.z()+localDisplacementZByX-localDisplacementZ;
//}


