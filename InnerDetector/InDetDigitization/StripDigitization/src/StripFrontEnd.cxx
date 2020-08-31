/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "StripFrontEnd.h"

// Random number
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"  // for RandGaussZiggurat
#include "CLHEP/Random/RandPoisson.h"

#include "SiDigitization/SiHelper.h"
#include "SCT_Digitization/ISCT_Amp.h"
#include "SCT_ConditionsServices/ISCT_ReadCalibChipDataSvc.h" // Calibration
                                                              // Cond DB data
#include "InDetIdentifier/SCT_ID.h"

#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "SCT_ReadoutGeometry/SCT_ModuleSideDesign.h"

// C++ Standard Library
#include <algorithm>
#include <iostream>

// #define SCT_DIG_DEBUG

using namespace std;
// ---------------

using namespace InDetDD;

// constructor
StripFrontEnd::StripFrontEnd(const std::string &type, const std::string &name,
                           const IInterface *parent)
    : AthAlgTool(type, name, parent),
    m_sc(0),
    m_SCTdetMgr(0),
    m_sct_id(0),
    m_sct_amplifier("StripAmp", this),
    m_ReadCalibChipDataSvc("SCT_ReadCalibChipDataSvc", name),
    m_rndmEngine(nullptr) {
    declareInterface< ISCT_FrontEnd >(this);

    declareProperty("NoiseBarrel", m_NoiseBarrel = 1500.0, "NoiseBarrel");
    declareProperty("NoiseBarrel3", m_NoiseBarrel3 = 1541.0, "NoiseBarrel3");
    declareProperty("NoiseInners", m_NoiseInners = 1090.0, "NoiseInners");
    declareProperty("NoiseMiddles", m_NoiseMiddles = 1557.0, "NoiseMiddles");
    declareProperty("NoiseShortMiddles", m_NoiseShortMiddles = 940.0,
                    "NoiseShortMiddles");
    declareProperty("NoiseOuters", m_NoiseOuters = 1618.0, "NoiseOuters");
    declareProperty("NOBarrel", m_NOBarrel = 1.5e-5, "NoiseOccupancyBarrel");
    declareProperty("NOBarrel3", m_NOBarrel3 = 2.1e-5, "NoiseOccupancyBarrel3");
    declareProperty("NOInners", m_NOInners = 5.0e-9, "NoiseOccupancyInners");
    declareProperty("NOMiddles", m_NOMiddles = 2.7e-5, "NoiseOccupancyMiddles");
    declareProperty("NOShortMiddles", m_NOShortMiddles = 2.0e-9,
                    "NoiseOccupancyShortMiddles");
    declareProperty("NOOuters", m_NOOuters = 3.5e-5, "NoiseOccupancyOuters");
    declareProperty("NoiseOn", m_NoiseOn = true,
                    "To know if noise is on or off when using calibration data");
    declareProperty("AddNoiseDiodes", m_addNoiseDiode=true, "To switch off the creation of noise diodes");
    declareProperty("AnalogueNoiseOn", m_analogueNoiseOn = true,
                    "To know if analogue noise is on or off");
    declareProperty("GainRMS", m_GainRMS = 0.031,
                    "Gain spread parameter within the strips for a given Chip gain");
    declareProperty("Ospread", m_Ospread = 0.0001,
                    "offset spread within the strips for a given Chip offset");
    declareProperty("OffsetGainCorrelation", m_OGcorr = 0.00001,
                    "Gain/offset correlation for the strips");
    declareProperty("Threshold", m_Threshold = 1.0, "Threshold");
    declareProperty("TimeOfThreshold", m_timeOfThreshold = 30.0,
                    "Threshold time");
    declareProperty("PulseAveragingFlag", m_PulseAveragingFlag = false,
                    "Flag to set the Pulse Averaging");
    declareProperty("DataCompressionMode", m_data_compression_mode = Edge_01X,
                    "Front End Data Compression Mode");
    declareProperty("DataReadOutMode", m_data_readout_mode = Condensed,
                    "Front End Data Read out mode Mode");
    declareProperty("UseCalibData", m_useCalibData = true,
                    "Flag to use Calib Data");
    declareProperty("MaxStripsPerSide", m_strip_max = 768, "For SLHC studies");
    declareProperty("SCT_ReadCalibChipDataSvc", m_ReadCalibChipDataSvc);
    declareProperty("SCT_Amp", m_sct_amplifier);
}

// Destructor:
StripFrontEnd::~StripFrontEnd() {
}

// ----------------------------------------------------------------------
// Initialize
// ----------------------------------------------------------------------
StatusCode StripFrontEnd::initialize() {
    if (m_NoiseOn and (not m_analogueNoiseOn)) {
        ATH_MSG_FATAL("AnalogueNoiseOn/m_analogueNoiseOn should be true if NoiseOn/m_NoiseOn is true.");
        return StatusCode::FAILURE;
    }

    // should not neec this?
    m_sc = AthAlgTool::initialize();
    if (m_sc.isFailure()) {
        ATH_MSG_FATAL("SCT_FrontEnd::initialize() failed");
        return m_sc;
    }
    ATH_MSG_DEBUG("SCT_FrontEnd::initialize()");
    // Get SCT helper
    ATH_CHECK(detStore()->retrieve(m_sct_id, "SCT_ID"));
    // Get SCT detector manager
    ATH_CHECK(detStore()->retrieve(m_SCTdetMgr, "SCT"));
    // Get the amplifier tool
    ATH_CHECK(m_sct_amplifier.retrieve());
    ATH_MSG_DEBUG("SCT Amplifier tool located ");


    // Get the SCT_ReadCaliDataSvc
    if (m_useCalibData) {
        ATH_CHECK(m_ReadCalibChipDataSvc.retrieve());
        ATH_MSG_DEBUG("CalibChipData Service located ");
    }

    // Get the maximum number of strips of any module
    m_strip_max = m_SCTdetMgr->numerology().maxNumStrips();

    // Allocate the appropiate memory for vectors
    ATH_CHECK(initVectors(m_strip_max));
    ATH_MSG_DEBUG("Class variable vectors allocated");

    constexpr float fC = 6242.2;
    m_Threshold = m_Threshold * fC;


#ifdef SCT_DIG_DEBUG
    ATH_MSG_INFO("\tNoise factors:");
    ATH_MSG_INFO("\tBarrel = " << m_NoiseBarrel << " Outer Barrel = " <<
        m_NoiseBarrel3 << " EC, inners = " << m_NoiseInners << " EC, middles = "
                               << m_NoiseMiddles << " EC, short middles = " <<
        m_NoiseShortMiddles << " EC, outers = " << m_NoiseOuters);
    ATH_MSG_INFO("\tPerforming averaging " << (m_PulseAveragingFlag           ?
                                               "TRUE" : "FALSE"));
    ATH_MSG_INFO("\tThreshold=" << m_Threshold << " fC, time of Threshold=" <<
        m_timeOfThreshold);
#endif

    ATH_MSG_INFO("m_Threshold             (Threshold)           = " <<
        m_Threshold);
    ATH_MSG_INFO("m_timeOfThreshold       (TimeOfThreshold)     = " <<
        m_timeOfThreshold);
    ATH_MSG_INFO("m_data_compression_mode (DataCompressionMode) = " <<
        m_data_compression_mode);
    ATH_MSG_INFO("m_data_readout_mode     (DataReadOutMode)     = " <<
        m_data_readout_mode);

    // Check configuration. If it is invalid, abort this job.
    if (not (m_data_compression_mode==Level_X1X or
             m_data_compression_mode==Edge_01X or
             m_data_compression_mode==AnyHit_1XX_X1X_XX1)) {
      ATH_MSG_FATAL("m_data_compression_mode = " << m_data_compression_mode
                    << " is invalid. Abort this job!!!");
      return StatusCode::FAILURE;
    }
    if (not (m_data_readout_mode==Condensed or m_data_readout_mode==Expanded)) {
      ATH_MSG_FATAL("m_data_readout_mode = " << m_data_readout_mode
                    << " is invalid. Abort this job!!!");
      return StatusCode::FAILURE;
    }
    if ((m_data_compression_mode==Level_X1X or m_data_compression_mode==AnyHit_1XX_X1X_XX1)
        and m_data_readout_mode==Condensed) {
      ATH_MSG_FATAL("m_data_compression_mode = " << m_data_compression_mode
                    << (m_data_compression_mode==Level_X1X ? " (Level_X1X)" : " (AnyHit_1XX_X1X_XX1)")
                    << " requires timing information."
                    << " However, m_data_readout_mode = " << m_data_readout_mode
                    << " (Condensed) does not keep timing information. Abort this job!!!");
      return StatusCode::FAILURE;
    }

    return m_sc;
}

StatusCode StripFrontEnd::finalize() {
    // should not be needed?
    m_sc = AthAlgTool::finalize();
    if (m_sc.isFailure()) {
        ATH_MSG_FATAL("SCT_FrontEnd::finalize() failed");
        return m_sc;
    }
#ifdef SCT_DIG_DEBUG
    ATH_MSG_INFO("SCT_FrontEnd::finalize()");
#endif
    return m_sc;
}

// ----------------------------------------------------------------------
// Init the class variable  vectors
// ----------------------------------------------------------------------
StatusCode StripFrontEnd::initVectors(const int &strips) const {
    m_Offset.assign(strips, 0.0);
    m_GainFactor.assign(strips, 0.0);
    m_NoiseFactor.assign(strips, 0.0);

    if (m_data_readout_mode == Condensed) {
        m_Analogue[0].reserve(strips);
        m_Analogue[1].reserve(strips);
    }
    else { // Expanded
        m_Analogue[0].reserve(strips);
        m_Analogue[1].reserve(strips);
        m_Analogue[2].reserve(strips);
    }

    return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// prepare gain and offset for the strips for a given module
// ----------------------------------------------------------------------
StatusCode StripFrontEnd::prepareGainAndOffset(
    SiChargedDiodeCollection &collection, const Identifier &moduleId) const {
    // now we need to generate gain and offset channel by channel: some algebra
    // for generation of partially correlated random numbers
    float W = m_OGcorr * m_GainRMS * m_Ospread / (m_GainRMS * m_GainRMS -
                                                  m_Ospread * m_Ospread);
    float A = 4 * W * W + 1.0;
    float x1 = (A - sqrt(A)) / (2.0 * A);
    float sinfi = sqrt(x1);
    float cosfi = sqrt(1.0 - x1);

    sinfi = sinfi * m_OGcorr / fabs(m_OGcorr);
    float S = m_GainRMS * m_GainRMS + m_Ospread * m_Ospread;
    float D = (m_GainRMS * m_GainRMS - m_Ospread * m_Ospread) / (cosfi * cosfi -
                                                                 sinfi * sinfi);
    float S1 = sqrt((S + D) * 0.5);
    float S2 = sqrt((S - D) * 0.5);
    float Noise = 0;
    int mode = 1;

    // To set noise values for different module types, barrel, EC, inners,
    // middles,
    // short middles, and outers
    if (m_analogueNoiseOn) {
        if (m_sct_id->barrel_ec(moduleId) == 0) { // barrel_ec=0 corresponds to
                                                  // barrel
            if (m_sct_id->layer_disk(moduleId) == 3) { // outer barrel layer 10
                                                       // degrees warmer
                Noise = m_NoiseBarrel3;
            }
            else {
                Noise = m_NoiseBarrel;
            }
        }
        else {
            int moduleType = m_sct_id->eta_module(moduleId);
            switch (moduleType) { // eta = 0, 1, or 2 corresponds to outers,
                                  // middles and inners?! (at least in the
                                  // offline world)
            case 0:
            {
                Noise = m_NoiseOuters;
                break;
            }

            case 1:
            {
                if (m_sct_id->layer_disk(moduleId) == 7) {
                    Noise = m_NoiseShortMiddles;
                }
                else {
                    Noise = m_NoiseMiddles;
                }
                break;
            }

            case 2:
            {
                Noise = m_NoiseInners;
                break;
            }

            default:
            {
                Noise = m_NoiseBarrel;
                ATH_MSG_ERROR("moduleType(eta): " << moduleType <<
                    " unknown, using barrel");
            }
            }// end of switch structure
        }
    }

    // Loop over collection and setup gain/offset/noise for the hit and
    // neighbouring strips
    SiChargedDiodeIterator i_chargedDiode = collection.begin();
    SiChargedDiodeIterator i_chargedDiode_end = collection.end();

    for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
        SiChargedDiode diode = (*i_chargedDiode).second;// should be const as we
                                                        // aren't trying to
                                                        // change it here - but
                                                        // getReadoutCell() is
                                                        // not a const method...
        unsigned int flagmask = diode.flag() & 0xFE;       // Get the flag for
                                                           // this diode ( if
                                                           // flagmask = 1 If
                                                           // diode is
                                                           // disconnected/disabled
                                                           // skip it)
        if (!flagmask) { // If the diode is OK (not flagged)
            const SiReadoutCellId roCell = diode.getReadoutCell();

            if (roCell.isValid()) {
                int strip = roCell.strip();
                int i = std::max(strip - 1, 0);
                int i_end = std::min(strip + 2, m_strip_max);

                // loop over strips
                for (; i < i_end; i++) {
                    // Need to check if strip is already setup
                    if (m_Analogue[1][i] <= 0.0) {
                        float g = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,
                                                                  0.0, S1);
                        float o = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,
                                                                  0.0, S2);

                        m_GainFactor[i] = 1.0 + (cosfi * g + sinfi * o);
                        m_Offset[i] = (cosfi * o - sinfi * g);
                        m_NoiseFactor[i] = Noise * mode;

                        // Fill the noise and offset values into the Analogue
                        if (m_data_readout_mode == Condensed) {
                            m_Analogue[0][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                            m_Analogue[1][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                        }
                        else { // Expanded
                            m_Analogue[0][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                            m_Analogue[1][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                            m_Analogue[2][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                        }
                    }
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// prepare gain and offset for the strips for a given module using
// Cond Db data to get the chip calibration data
// ----------------------------------------------------------------------
StatusCode StripFrontEnd::prepareGainAndOffset(
    SiChargedDiodeCollection &collection, const int &side, const
    Identifier &moduleId) const {
    // Get chip data from calib DB
    std::vector<float> gainByChipVect = m_ReadCalibChipDataSvc->getNPtGainData(
        moduleId, side, "GainByChip");
    std::vector<float> gainRMSByChipVect =
        m_ReadCalibChipDataSvc->getNPtGainData(moduleId, side, "GainRMSByChip");
    std::vector<float> offsetByChipVect =
        m_ReadCalibChipDataSvc->getNPtGainData(moduleId, side, "OffsetByChip");
    std::vector<float> offsetRMSByChipVect =
        m_ReadCalibChipDataSvc->getNPtGainData(moduleId, side,
                                               "OffsetRMSByChip");
    std::vector<float> noiseByChipVect(6, 0.0);

    if (m_analogueNoiseOn) { // Check if noise should be on or off
        noiseByChipVect = m_ReadCalibChipDataSvc->getNPtGainData(moduleId, side,
                                                                 "NoiseByChip");
    }

    // Need to check if empty, most should have data, but a few old DEAD modules
    // don't
    if (gainByChipVect.empty() || noiseByChipVect.empty()) {
        ATH_MSG_DEBUG("No calibration data in cond DB for module " <<
            moduleId << " using JO values");
        if (StatusCode::SUCCESS != prepareGainAndOffset(collection, moduleId)) {
            return StatusCode::FAILURE;
        }
        else {
            return StatusCode::SUCCESS;
        }
    }

    // Don't really need to set up values for each chip...
    float gainMeanValue = meanValue(gainByChipVect);
    if (gainMeanValue < 0.0) {
        ATH_MSG_DEBUG("All chip gain values are 0 for module " << moduleId <<
            " using JO values");
        if (StatusCode::SUCCESS != prepareGainAndOffset(collection, moduleId)) {
            return StatusCode::FAILURE;
        }
        else {
            return StatusCode::SUCCESS;
        }
    }

    std::vector<float> gain(6, 0.0);
    std::vector<float> offset(6, 0.0);
    std::vector<float> S1(6, 0.0);
    std::vector<float> S2(6, 0.0);
    std::vector<float> sinfi(6, 0.0);
    std::vector<float> cosfi(6, 0.0);
    float gainRMS = 0.0;
    float offsetRMS = 0.0;

    for (int i = 0; i < 6; ++i) {
        // Some very few chips have 0 values, dead/bypassed/etc, so check and
        // use some fixed values instead
        if (gainByChipVect[i] > 0.1) {
            gain[i] = gainByChipVect[i] / gainMeanValue;
            offset[i] = offsetByChipVect[i] / m_Threshold;
            gainRMS = gainRMSByChipVect[i] / gainMeanValue;
            offsetRMS = offsetRMSByChipVect[i] / m_Threshold;
        }
        else {
            gain[i] = 55.0 / gainMeanValue;
            offset[i] = 42.0 / m_Threshold;
            gainRMS = 1.3 / gainMeanValue;
            offsetRMS = 2.0 / m_Threshold;
        }

        float W = m_OGcorr * gainRMS * offsetRMS / (gainRMS * gainRMS -
                                                    offsetRMS * offsetRMS);
        float A = 4 * W * W + 1.0;
        float x1 = (A - sqrt(A)) / (2.0 * A);
        sinfi[i] = sqrt(x1);
        cosfi[i] = sqrt(1.0 - x1);
        sinfi[i] = sinfi[i] * m_OGcorr / fabs(m_OGcorr);
        float S = gainRMS * gainRMS + offsetRMS * offsetRMS;
        float D = (gainRMS * gainRMS - offsetRMS * offsetRMS) / (cosfi[i] *
                                                                 cosfi[i] -
                                                                 sinfi[i] *
                                                                 sinfi[i]);
        S1[i] = sqrt((S + D) / 2.0);
        S2[i] = sqrt((S - D) / 2.0);
    }

    // Loop over collection and setup gain/offset/noise for the hit and
    // neighbouring strips
    SiChargedDiodeIterator i_chargedDiode = collection.begin();
    SiChargedDiodeIterator i_chargedDiode_end = collection.end();

    for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
        SiChargedDiode diode = (*i_chargedDiode).second;// should be const as we
                                                        // aren't trying to
                                                        // change it here - but
                                                        // getReadoutCell() is
                                                        // not a const method...
        unsigned int flagmask = diode.flag() & 0xFE;       // Get the flag for
                                                           // this diode ( if
                                                           // flagmask = 1 If
                                                           // diode is
                                                           // disconnected/disabled
                                                           // skip it)
        if (!flagmask) { // If the diode is OK (not flagged)
            const SiReadoutCellId roCell = diode.getReadoutCell();

            if (roCell.isValid()) {
                int strip = roCell.strip();
                int i = std::max(strip - 1, 0);
                int i_end = std::min(strip + 2, m_strip_max);

                // loop over strips
                for (; i < i_end; i++) {
                    // Need to check if strip is already setup
                    if (m_Analogue[1][i] <= 0.0) {
                        // Values depends on which chip the strip is on (complex
                        // when strip is on chip edge)
                        int chip = i / 128;
                        float g = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,
                                                                  0.0,
                                                                  S1[chip]);
                        float o = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,
                                                                  0.0,
                                                                  S2[chip]);

                        m_GainFactor[i] = gain[chip] + (cosfi[chip] * g +
                                                        sinfi[chip] * o);
                        m_Offset[i] = offset[chip] + (cosfi[chip] * o -
                                                      sinfi[chip] * g);
                        m_NoiseFactor[i] = noiseByChipVect[chip];

                        // Fill the noise and offset values into the Analogue
                        if (m_data_readout_mode == Condensed) {
                            m_Analogue[0][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                            m_Analogue[1][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                        }
                        else { // Expanded
                            m_Analogue[0][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                            m_Analogue[1][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                            m_Analogue[2][i] = m_Offset[i] + m_NoiseFactor[i] *
                                               CLHEP::RandGaussZiggurat::shoot(
                                m_rndmEngine);
                        }
                    }
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
StatusCode StripFrontEnd::randomNoise(SiChargedDiodeCollection &collection, const
                                     Identifier &moduleId) const {
    // Add random noise

    double occupancy = 0.0;
    double NoiseOccupancy = 0.0;
    float Noise = 0.0;
    int nNoisyStrips = 0;
    double mode = 1.;

    const bool noise_expanded_mode = (m_data_compression_mode == AnyHit_1XX_X1X_XX1 and m_data_readout_mode == Expanded);

    // Will give 3 times as much noise occupancy if running in any hit expanded
    // mode
    if (noise_expanded_mode) {
        mode = 3.;
    }

    // Sets fixed noise occupancy values for different module types, barrel, EC,
    // inners, middles
    // short middles, and outers
    if (m_sct_id->barrel_ec(moduleId) == 0) { // barrel_ec=0 corresponds to
                                              // barrel
        if (m_sct_id->layer_disk(moduleId) == 3) { // outer barrel layer 10
                                                   // degrees warmer
            NoiseOccupancy = m_NOBarrel3;
            Noise = m_NoiseBarrel3;
        }
        else {
            NoiseOccupancy = m_NOBarrel;
            Noise = m_NoiseBarrel;
        }
    }
    else {
        int moduleType = m_sct_id->eta_module(moduleId);
        switch (moduleType) { // eta = 0, 1, or 2 corresponds to outers, middles
                              // and inners?! (at least in the offline world)
        case 0:
        {
            NoiseOccupancy = m_NOOuters;
            Noise = m_NoiseOuters;
            break;
        }

        case 1:
        {
            if (m_sct_id->layer_disk(moduleId) == 7) {
                NoiseOccupancy = m_NOShortMiddles;
                Noise = m_NoiseShortMiddles;
            }
            else {
                NoiseOccupancy = m_NOMiddles;
                Noise = m_NoiseMiddles;
            }
            break;
        }

        case 2:
        {
            NoiseOccupancy = m_NOInners;
            Noise = m_NoiseInners;
            break;
        }

        default:
        {
            NoiseOccupancy = m_NOBarrel;
            Noise = m_NoiseBarrel;
            ATH_MSG_ERROR("moduleType(eta): " << moduleType <<
                " unknown, using barrel");
        }
        }// end of switch structure
    }

    // Calculate the number of "free strips"
    int nEmptyStrips = 0;
    std::vector<int> emptyStrips;
    emptyStrips.reserve(m_strip_max);
    for (int i = 0; i < m_strip_max; i++) {
        if (m_StripHitsOnWafer[i] == 0) {
            emptyStrips.push_back(i);
            ++nEmptyStrips;
        }
    }

    if (nEmptyStrips != 0) {
        // Should randomize the fixed NO values, so we get some differences per
        // wafer
        occupancy = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,
                                                    NoiseOccupancy,
                                                    NoiseOccupancy * 0.1);

        // Modify the occupancy if threshold is not 1.0 fC
        if (m_Threshold > 6242.3 || m_Threshold < 6242.1) {
            const float fC = 6242.2;
            occupancy = occupancy * exp(-(0.5 / (Noise * Noise) * (m_Threshold *
                                                                   m_Threshold -
                                                                   fC * fC)));
        }
        nNoisyStrips = CLHEP::RandPoisson::shoot(m_rndmEngine, m_strip_max *
                                                 occupancy * mode);

        // Check and adapt the number of noisy strips to the number of free
        // strips
        if (nEmptyStrips < nNoisyStrips) {
            nNoisyStrips = nEmptyStrips;
        }

        // Find random strips to get noise hits
        for (int i = 0; i < nNoisyStrips; i++) {
            int index = CLHEP::RandFlat::shootInt(m_rndmEngine, nEmptyStrips -
                                                  i); // strip == 10, 12 free
                                                      // strips
            // have vector [10000100100200211001] 20 strips
            int strip = emptyStrips.at(index);
            emptyStrips.erase(emptyStrips.begin()+index); // Erase it not to use it again
            if (m_StripHitsOnWafer[strip]!=0) {
                ATH_MSG_ERROR(index << "-th empty strip, strip " << strip << " should be empty but is not empty! Something is wrong!");
            }
            m_StripHitsOnWafer[strip] = 3;                                    //
                                                                              // !<
                                                                              // Random
                                                                              // Noise
                                                                              // hit
            // Add tbin info to noise diode
            if (noise_expanded_mode) { // !< if any hit mode, any time bin otherwise fixed tbin=2
                int noise_tbin = CLHEP::RandFlat::shootInt(m_rndmEngine, 3);
                // !< random number 0, 1 or 2
                if (noise_tbin == 0) {
                    noise_tbin = 4; // !< now 1,2 or 4
                }
                if (StatusCode::SUCCESS != addNoiseDiode(collection, strip,
                                                         noise_tbin)) {
                    ATH_MSG_ERROR("Can't add noise hit diode to collection");
                }
            }
            else {
                if (StatusCode::SUCCESS != addNoiseDiode(collection, strip,
                                                         2)) {
                    ATH_MSG_ERROR("Can't add noise hit diode to collection");
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
StatusCode StripFrontEnd::randomNoise(SiChargedDiodeCollection &collection, const
                                     Identifier &moduleId, const
                                     int &side) const {
    int n_chips = 6;
    int chipStripmax = m_strip_max / n_chips;
    std::vector<float> NOByChipVect(n_chips, 0.0);
    std::vector<float> ENCByChipVect(n_chips, 0.0);
    std::vector<int> nNoisyStrips(n_chips, 0);
    double mode = 1.;

    const bool noise_expanded_mode = (m_data_compression_mode == AnyHit_1XX_X1X_XX1 and m_data_readout_mode == Expanded);

    // Will give 3 times as much noise occupancy if running in any hit expanded
    // mode
    if (noise_expanded_mode) {
        mode = 3.;
    }

    // Get chip data from calib DB
    NOByChipVect = m_ReadCalibChipDataSvc->getNoiseOccupancyData(moduleId, side,
                                                                 "OccupancyByChip");
    ENCByChipVect = m_ReadCalibChipDataSvc->getNPtGainData(moduleId, side,
                                                           "NoiseByChip");

    // Need to check if empty, most should have data, but a few old DEAD modules
    // don't, and 9C...
    if (NOByChipVect.empty()) {
        ATH_MSG_DEBUG("No calibration data in cond DB for module " <<
            moduleId << " using JO values");
        if (StatusCode::SUCCESS != randomNoise(collection, moduleId)) {
            return StatusCode::FAILURE;
        }
        else {
            return StatusCode::SUCCESS;
        }
    }
    else {
        for (int i = 0; i < n_chips; i++) {
            // A 0 value can mean two things now, chip out of config for long
            // time and no value was uploaded
            // or its short middles and inners and the value is for all purposes
            // 0! so ok.

            // Modify the occupancy if threshold is not 1.0 fC
            if (m_Threshold > 6242.3 || m_Threshold < 6242.1) {
                constexpr float fC = 6242.2;
                NOByChipVect[i] = NOByChipVect[i] * exp(-(0.5 /
                                                          (ENCByChipVect[i] *
                                                           ENCByChipVect[i]) *
                                                          (m_Threshold *
                                                           m_Threshold
                                                           -
                                                           fC * fC)));
            }

            nNoisyStrips[i] = CLHEP::RandPoisson::shoot(m_rndmEngine,
                                                        chipStripmax *
                                                        NOByChipVect[i] * mode);
        }
    }

    // Loop over the chips on the wafer
    for (int chip_index = 0; chip_index < n_chips; ++chip_index) {
        int chip_strip_offset = chipStripmax * chip_index; // First strip number
                                                           // on chip

        // Calculate the number of "free strips" on this chip
        int nEmptyStripsOnChip = 0;
        std::vector<int> emptyStripsOnChip;
        emptyStripsOnChip.reserve(chipStripmax);
        for (int i = 0; i < chipStripmax; i++) {
            if (m_StripHitsOnWafer[i + chip_strip_offset] == 0) {
                emptyStripsOnChip.push_back(i);
                ++nEmptyStripsOnChip;
            }
        }

        // if no empty strips on chip do nothing
        if (nEmptyStripsOnChip != 0) {
            // Check and adapt the number of noisy strips to the number of free
            // strips
            if (nEmptyStripsOnChip < nNoisyStrips[chip_index]) {
                nNoisyStrips[chip_index] = nEmptyStripsOnChip;
            }

            // Find random strips to get noise hits
            for (int i = 0; i < nNoisyStrips[chip_index]; i++) {
                int index = CLHEP::RandFlat::shootInt(m_rndmEngine,
                                                              nEmptyStripsOnChip
                                                              - i);
                int strip_on_chip = emptyStripsOnChip.at(index);
                emptyStripsOnChip.erase(emptyStripsOnChip.begin()+index); // Erase it not to use it again
                int strip = strip_on_chip + chip_strip_offset;
                if (m_StripHitsOnWafer[strip]!=0) {
                    ATH_MSG_ERROR(index << "-th empty strip, strip " << strip << " should be empty but is not empty! Something is wrong!");
                }
                m_StripHitsOnWafer[strip] = 3;                                    //
                                                                                  // !<
                                                                                  // Random
                                                                                  // Noise
                                                                                  // hit
                // Add tbin info to noise diode
                if (noise_expanded_mode) { // !< if any hit mode, any time bin
                  // !< otherwise fixed tbin=2
                    int noise_tbin = CLHEP::RandFlat::shootInt(m_rndmEngine, 3);
                    // !< random number 0, 1 or 2
                    if (noise_tbin == 0) {
                        noise_tbin = 4; // !< now 1, 2 or 4
                    }
                    if (StatusCode::SUCCESS != addNoiseDiode(collection, strip,
                                                             noise_tbin)) {
                        ATH_MSG_ERROR(
                            "Can't add noise hit diode to collection");
                    }
                }
                else {
                    if (StatusCode::SUCCESS != addNoiseDiode(collection, strip,
                                                             2)) {
                        ATH_MSG_ERROR(
                            "Can't add noise hit diode to collection");
                    }
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// process the collection of pre digits this will need to go through
// all single-strip pre-digits calculate the amplifier response add noise
// (this could be moved elsewhere later) apply threshold do clustering
// ----------------------------------------------------------------------
void StripFrontEnd::process(SiChargedDiodeCollection &collection) const {
    // get SCT module side design and check it
    const SCT_ModuleSideDesign *p_design = dynamic_cast<const
                                                        SCT_ModuleSideDesign *>(
        &(collection.design()));

    if (!p_design) {
        return;
    }

    // Check number of strips in design and from manager(max number of strips on
    // any module)
    // The design value should always be equal or lower than the manager one
    // However, no resising is now done in case of a lower value
    const int strip_max = p_design->cells();
    if (strip_max > m_strip_max) {
        ATH_MSG_ERROR("The number of strips from the design, " << strip_max
                                                               <<
            "are higher than the manager max value, " << m_strip_max <<
            ", will use design and continue...");

        // Resize vectors
        if (StatusCode::SUCCESS != initVectors(strip_max)) {
            ATH_MSG_FATAL("Can't resize class variable vectors");
        }
        else {
            m_strip_max = strip_max;
            ATH_MSG_DEBUG("Class variable vectors resized");
        }
    }

    // Contains strip hit info, reset to 0 for each wafer processed
    m_StripHitsOnWafer.assign(m_strip_max, 0);

    // Containes the charge for each bin on each hit strip
    if (m_data_readout_mode == Condensed) {
        for (int i = 0; i < m_strip_max; ++i) {
            m_Analogue[0][i] = 0.0;
            m_Analogue[1][i] = 0.0;
        }
    }
    else { // Expanded
        for (int i = 0; i < m_strip_max; ++i) {
            m_Analogue[0][i] = 0.0;
            m_Analogue[1][i] = 0.0;
            m_Analogue[2][i] = 0.0;
        }
    }

    // Get wafer, moduleId and side
    Identifier waferId = collection.identify();
    Identifier moduleId = m_sct_id->module_id(waferId);
    const int side = m_sct_id->side(waferId);

    // Check if collection empty
    if (!collection.empty()) {
        // Setup gain/offset/noise to the hit and neighbouring strips
        if (m_useCalibData) { // Use calib cond DB data
            if (StatusCode::SUCCESS != prepareGainAndOffset(collection, side,
                                                            moduleId)) {
                ATH_MSG_ERROR("\tCan't prepare Gain and Offset");
            }
        }
        else { // Use JO values
            if (StatusCode::SUCCESS != prepareGainAndOffset(collection,
                                                            moduleId)) {
                ATH_MSG_ERROR("\tCan't prepare Gain and Offset");
            }
        }

        if (StatusCode::SUCCESS != doSignalChargeForHits(collection)) {
            ATH_MSG_ERROR("\tCan't doSignalChargeForHits");
        }

        if (StatusCode::SUCCESS != doThresholdCheckForRealHits(collection)) {
            ATH_MSG_ERROR("\tCan't doThresholdCheckForRealHits");
        }

        if (m_addNoiseDiode and StatusCode::SUCCESS != doThresholdCheckForCrosstalkHits(
                collection)) {
            ATH_MSG_ERROR("\tCan't doThresholdCheckForCrosstalkHits");
        }
    }

    if (m_NoiseOn) {
        if (m_useCalibData) { // Check if using DB or not
            if (StatusCode::SUCCESS != randomNoise(collection, moduleId,
                                                   side)) {
                ATH_MSG_ERROR("\tCan't do random noise on wafer?!");
            }
        }
        else { // Use JO fixed values
            if (StatusCode::SUCCESS != randomNoise(collection, moduleId)) {
                ATH_MSG_ERROR("\tCan't do random noise on wafer?!");
            }
        }
    }

    // Check for strips above threshold and do clustering
    if (StatusCode::SUCCESS != doClustering(collection)) {
        ATH_MSG_ERROR("\tCan't cluster the hits?!");
    }
}

StatusCode StripFrontEnd::doSignalChargeForHits(
    SiChargedDiodeCollection &collection) const {
    typedef SiTotalCharge::list_t list_t;

    // *****************************************************************************
    // Loop over the diodes (strips ) and for each of them define the total
    // signal
    // *****************************************************************************

    // set up number of needed bins depending on the compression mode
    short bin_max = 0;
    if (m_data_readout_mode == Condensed) {
        bin_max = m_data_compression_mode;
    }
    else {
        bin_max = 3;
    }

    std::vector<float> response(bin_max);

    SiChargedDiodeIterator i_chargedDiode = collection.begin();
    SiChargedDiodeIterator i_chargedDiode_end = collection.end();
    for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
        SiChargedDiode diode = (*i_chargedDiode).second;// should be const as we
                                                        // aren't trying to
                                                        // change it here - but
                                                        // getReadoutCell() is
                                                        // not a const method...
        unsigned int flagmask = diode.flag() & 0xFE;       // Get the flag for
                                                           // this diode ( if
                                                           // flagmask = 1 If
                                                           // diode is
                                                           // disconnected/disabled
                                                           // skip it)
        if (!flagmask) { // If the diode is OK (not flagged)
            const SiReadoutCellId roCell = diode.getReadoutCell();

            if (roCell.isValid()) {
                int strip = roCell.strip();

                const list_t &ChargesOnStrip =
                    diode.totalCharge().chargeComposition();

                // Accumulate averages !!!
                if (m_PulseAveragingFlag) {
                    m_sct_amplifier->AccumulateAverages(ChargesOnStrip);
                }

                if (m_data_readout_mode == Condensed) {
                    // Amplifier response
                    m_sct_amplifier->response(ChargesOnStrip, m_timeOfThreshold,
                                              response);
                    for (short bin = 0; bin < bin_max; ++bin) {
                        m_Analogue[bin][strip] += m_GainFactor[strip] *
                                                  response[bin];
                    }
                    // Add Crosstalk signal for neighboring strip
                    m_sct_amplifier->crosstalk(ChargesOnStrip,
                                               m_timeOfThreshold, response);
                    for (short bin = 0; bin < bin_max; ++bin) {
                        if (strip + 1 < m_strip_max) {
                            m_Analogue[bin][strip + 1] += m_GainFactor[strip +
                                                                       1] *
                                                          response[bin];
                        }
                        if (strip > 0) {
                            m_Analogue[bin][strip - 1] += m_GainFactor[strip -
                                                                       1] *
                                                          response[bin];
                        }
                    }
                }
                else { // Expanded
                    // Amplifier response
                    m_sct_amplifier->response(ChargesOnStrip, m_timeOfThreshold,
                                              response);
                    for (short bin = 0; bin < bin_max; ++bin) {
                        m_Analogue[bin][strip] += m_GainFactor[strip] *
                                                  response[bin];
                    }
                    // Add Crosstalk signal for neighboring strip
                    m_sct_amplifier->crosstalk(ChargesOnStrip,
                                               m_timeOfThreshold, response);
                    for (short bin = 0; bin < bin_max; ++bin) {
                        if (strip + 1 < m_strip_max) {
                            m_Analogue[bin][strip + 1] += m_GainFactor[strip +
                                                                       1] *
                                                          response[bin];
                        }
                        if (strip > 0) {
                            m_Analogue[bin][strip - 1] += m_GainFactor[strip -
                                                                       1] *
                                                          response[bin];
                        }
                    }
                }
            }
            else { // if roCell not valid
                ATH_MSG_WARNING("\t Cannot get the cell ");
            }
        }
        else {// If diode is disconnected/disabled skip it
            ATH_MSG_WARNING("\tDisabled or disconnected diode (strip)");
        }
    }
    return StatusCode::SUCCESS;
}

StatusCode StripFrontEnd::doThresholdCheckForRealHits(
    SiChargedDiodeCollection &collection) const {
    // **********************************************************************************
    // Flag strips below threshold and flag the threshold check into
    // m_StripHitsOnWafer
    // **********************************************************************************

    SiChargedDiodeIterator i_chargedDiode = collection.begin();
    SiChargedDiodeIterator i_chargedDiode_end = collection.end();

    for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
        SiChargedDiode &diode = (*i_chargedDiode).second;
        SiReadoutCellId roCell = diode.getReadoutCell();
        if (roCell.isValid()) {
            int strip = roCell.strip();
            if (strip > -1 && strip < m_strip_max) {
                if (m_data_readout_mode == Condensed) {
                    if ((m_Analogue[0][strip] >= m_Threshold ||
                         m_Analogue[1][strip] < m_Threshold)) {
                        SiHelper::belowThreshold(diode, true);   // Below strip
                                                                 // diode signal
                                                                 // threshold
                        m_StripHitsOnWafer[strip] = -1;
                    }
                    else if (((0x10 & diode.flag()) == 0x10) || ((0x4 &
                                                                  diode.flag())
                                                                 == 0x4)) {
                        // previously a crazy strip number could have screwed
                        // things up here.
                        m_StripHitsOnWafer[strip] = -1;
                    }
                    else {
                        m_StripHitsOnWafer[strip] = 1;
                        SiHelper::SetTimeBin(diode, 2); // set timebin info
                    }
                }
                else { // Expanded
                    // Check hit pattern
                    int have_hit_bin = 0;
                    if (m_Analogue[0][strip] >= m_Threshold) {
                        have_hit_bin = 4;
                    }
                    if (m_Analogue[1][strip] >= m_Threshold) {
                        have_hit_bin += 2;
                    }
                    if (m_Analogue[2][strip] >= m_Threshold) {
                        have_hit_bin += 1;
                    }
                    if (((0x10 & diode.flag()) == 0x10) || ((0x4 &
                                                             diode.flag()) ==
                                                            0x4)) {
                        // previously a crazy strip number could have screwed
                        // things up here.
                        m_StripHitsOnWafer[strip] = -1;
                    }
                    else if (m_data_compression_mode == Level_X1X) {
                        if (have_hit_bin == 2 or have_hit_bin == 3 or
                            have_hit_bin == 6 or have_hit_bin == 7) {
                            m_StripHitsOnWafer[strip] = 1;
                            SiHelper::SetTimeBin(diode, have_hit_bin);
                        }
                        else {
                            SiHelper::belowThreshold(diode, true); // Below
                                                                   // strip
                                                                   // diode
                                                                   // signal
                                                                   // threshold
                            m_StripHitsOnWafer[strip] = -1;
                        }
                    }
                    else if (m_data_compression_mode == Edge_01X) {
                        if (have_hit_bin == 2 or have_hit_bin == 3) {
                            m_StripHitsOnWafer[strip] = 1;
                            SiHelper::SetTimeBin(diode, have_hit_bin);
                        }
                        else {
                            SiHelper::belowThreshold(diode, true); // Below
                                                                   // strip
                                                                   // diode
                                                                   // signal
                                                                   // threshold
                            m_StripHitsOnWafer[strip] = -1;
                        }
                    }
                    else if (m_data_compression_mode == AnyHit_1XX_X1X_XX1) {
                        if (have_hit_bin == 0) {
                            SiHelper::belowThreshold(diode, true); // Below
                                                                   // strip
                                                                   // diode
                                                                   // signal
                                                                   // threshold
                            m_StripHitsOnWafer[strip] = -1;
                        }
                        else {
                            m_StripHitsOnWafer[strip] = 1;
                            if (m_data_readout_mode == Expanded) { // !< check for
                                                               // exp mode or
                                                               // not
                                SiHelper::SetTimeBin(diode, have_hit_bin);
                            }
                            else { // Condensed
                                SiHelper::SetTimeBin(diode, 2);
                            }
                        }
                    }
                }
            }
        }
    }
    return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
StatusCode StripFrontEnd::doThresholdCheckForCrosstalkHits(
    SiChargedDiodeCollection &collection) const {
    // Check for noise+crosstalk strips above threshold
    // m_StripHitsOnWafer: real hits above threshold == 1 or below/disconnected
    // == -1
    // =0 for free strips or strips with charge to be checked (m_Analogue[1]!=0)
    // Set 2 for crosstalk noise hits and -2 for below ones

    for (int strip = 0; strip < m_strip_max; strip++) {
        // Find strips with m_StripHitsOnWafer[strip] == 0
        if (m_StripHitsOnWafer[strip] != 0) { // real hits already checked
            continue;
        }
        if (m_Analogue[1][strip] > 0) { // Better way of doing this?! set
                                        // m_StripHitsOnWafer to x in
                                        // prepareGainAndOffset
            if (m_data_readout_mode == Condensed) {
                if ((m_Analogue[0][strip] >= m_Threshold ||
                     m_Analogue[1][strip] < m_Threshold)) {
                    m_StripHitsOnWafer[strip] = -2;           // Below threshold
                }
                else {
                    m_StripHitsOnWafer[strip] = 2;            // Crosstalk+Noise
                                                              // hit
                    if (StatusCode::SUCCESS != addNoiseDiode(collection, strip,
                                                             2)) {
                        ATH_MSG_ERROR(
                            "Can't add noise hit diode to collection");
                    }
                }
            }
            else { // Expanded
                int have_hit_bin = 0;
                if (m_Analogue[0][strip] >= m_Threshold) {
                    have_hit_bin = 4;
                }
                if (m_Analogue[1][strip] >= m_Threshold) {
                    have_hit_bin += 2;
                }
                if (m_Analogue[2][strip] >= m_Threshold) {
                    have_hit_bin += 1;
                }
                if (m_data_compression_mode == Level_X1X) {
                    if (have_hit_bin == 2 or have_hit_bin == 3 or
                        have_hit_bin == 6 or have_hit_bin == 7) {
                        m_StripHitsOnWafer[strip] = 2;          // Crosstalk+Noise
                                                                // hit
                        if (StatusCode::SUCCESS != addNoiseDiode(collection,
                                                                 strip,
                                                                 have_hit_bin))
                        {
                            ATH_MSG_ERROR(
                                "Can't add noise hit diode to collection");
                        }
                    }
                    else {
                        m_StripHitsOnWafer[strip] = -2;         // Below
                                                                // threshold
                    }
                }
                else if (m_data_compression_mode == Edge_01X) {
                    if (have_hit_bin == 2 or have_hit_bin == 3) {
                        m_StripHitsOnWafer[strip] = 2; // Noise hit
                        if (StatusCode::SUCCESS != addNoiseDiode(collection,
                                                                 strip,
                                                                 have_hit_bin))
                        {
                            ATH_MSG_ERROR(
                                "Can't add noise hit diode to collection");
                        }
                    }
                    else {
                        m_StripHitsOnWafer[strip] = -2;         // Below
                                                                // threshold
                    }
                }
                else if (m_data_compression_mode == 3) { // !< any hit mode
                    if (have_hit_bin == 0) {
                        m_StripHitsOnWafer[strip] = -2;         // Below
                                                                // threshold
                    }
                    else {
                        m_StripHitsOnWafer[strip] = 2;          // !<
                                                                // Crosstalk+Noise
                                                                // hit
                        if (m_data_readout_mode == Expanded) { // !< check for
                                                              // exp mode or not
                            if (StatusCode::SUCCESS != addNoiseDiode(collection,
                                                                     strip,
                                                                     have_hit_bin))
                            {
                                ATH_MSG_ERROR(
                                    "Can't add noise hit diode to collection");
                            }
                        }
                        else { // Condensed
                            if (StatusCode::SUCCESS != addNoiseDiode(collection,
                                                                     strip,
                                                                     2)) {
                                ATH_MSG_ERROR(
                                    "Can't add noise hit diode to collection");
                            }
                        }
                    }
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}

StatusCode StripFrontEnd::doClustering(SiChargedDiodeCollection &collection)
  const {
    // ********************************
    // now do clustering
    // ********************************
    int strip = 0;             
    int clusterSize = 0;      
    int numberOfClusters = 0;

    const SCT_ModuleSideDesign &sctDesign = dynamic_cast<const SCT_ModuleSideDesign &> (collection.design());
    
    SiCellId hitStrip;        

    if (m_data_readout_mode == Condensed) {
        do {
            if (m_StripHitsOnWafer[strip] > 0) {
                // ====== First step: Get the cluster size
                // ===================================================
                int clusterFirstStrip = strip;                                       

                // Find end of cluster. In multi-row sensors, cluster cannot span rows.
                int row = sctDesign.row(strip);
                if (row < 0) {
                    row = 0;
                }
                int lastStrip1DInRow(0);
                for (int i = 0; i < row + 1; ++i) {
                    lastStrip1DInRow += sctDesign.diodesInRow(i);
                }
                
                while (strip < lastStrip1DInRow - 1 && m_StripHitsOnWafer[strip +1] > 0) {
                    ++strip; // !< Find first strip hit and add up the following strips
                }
                int clusterLastStrip = strip;                                        
                                                                                     
                ++numberOfClusters;                                                  
                                                                                     
                clusterSize = (clusterLastStrip - clusterFirstStrip) + 1;           

		hitStrip = SiCellId(clusterFirstStrip);
		  
                SiChargedDiode &HitDiode = *(collection.find(hitStrip));             
                                                                                    
                SiHelper::SetStripNum(HitDiode, clusterSize);                         
                                                                                      
                SiChargedDiode *PreviousHitDiode = &HitDiode;
                for (int i = clusterFirstStrip + 1; i <= clusterLastStrip;
                     ++i) {
		  hitStrip  = SiCellId(i);
		  SiChargedDiode &HitDiode2 = *(collection.find(hitStrip));          
                                                                                       
		  SiHelper::ClusterUsed(HitDiode2, true);                             

                    PreviousHitDiode->setNextInCluster(&HitDiode2);
                    PreviousHitDiode = &HitDiode2;
                }
            }
            ++strip;                // !< This is the starting point of the next
                                    // cluster within this collection
        } while (strip < m_strip_max);
    }
    else {
      // Expanded read out mode, basically one RDO/strip per cluster
      // But if consecutively fired strips have the same time bin, those are converted into one cluster.
      do {
        clusterSize = 1;
        if (m_StripHitsOnWafer[strip] > 0) {
          hitStrip = SiCellId(strip);
          SiChargedDiode& hitDiode = *(collection.find(hitStrip));
          int timeBin = SiHelper::GetTimeBin(hitDiode);
          SiChargedDiode* previousHitDiode = &hitDiode;
          // Check if consecutively fired strips have the same time bin
          for (int newStrip=strip+1; newStrip<m_strip_max; newStrip++) {
            if (not (m_StripHitsOnWafer[newStrip]>0)) break;
            Identifier newHitStrip = m_sct_id->strip_id(collection.identify(), newStrip);
            SiChargedDiode& newHitDiode = *(collection.find(newHitStrip));
            if (timeBin!=SiHelper::GetTimeBin(newHitDiode)) break;
            SiHelper::ClusterUsed(newHitDiode, true);
            previousHitDiode->setNextInCluster(&newHitDiode);
            previousHitDiode = &newHitDiode;
            clusterSize++;
          }
          SiHelper::SetStripNum(hitDiode, clusterSize);
#ifdef SCT_DIG_DEBUG
          ATH_MSG_DEBUG("RDO Strip = " << strip << ", tbin = " <<
                        SiHelper::GetTimeBin(hitDiode) <<
                        ", HitInfo(1=real, 2=crosstalk, 3=noise): " <<
                        m_StripHitsOnWafer[strip]);
#endif
        }
        strip += clusterSize; // If more than one strip fires, those following strips are skipped.
      } while (strip < m_strip_max);
    }

    // clusters below threshold, only from pre-digits that existed before no
    // pure noise clusters below threshold will be made
    // D. Costanzo. I don't see why we should cluster the strips below
    // threshold. I'll pass on the info of strips below threshold
    // to the SDOs. I'll leave the SCT experts the joy to change this if they
    // don't like what I did or if this requires too much memory/disk

    return StatusCode::SUCCESS;
}

StatusCode StripFrontEnd::addNoiseDiode(SiChargedDiodeCollection &collection,
                                       const int &strip, const
                                       int &tbin) const {
    SiCellId ndiode(strip);                                        // !< create
                                                                   // a new
                                                                   // diode
    SiCharge noiseCharge(2 * m_Threshold, 0, SiCharge::noise);         // !< add
                                                                       // some
                                                                       // noise
                                                                       // to it

    collection.add(ndiode, noiseCharge);                            // !< add it
                                                                    // to the
                                                                    // collection

    // Get the strip back to check
    Identifier idstrip = m_sct_id->strip_id(collection.identify(), strip);
    SiChargedDiode *NoiseDiode = (collection.find(idstrip));
    if (NoiseDiode == NULL) {
        return StatusCode::FAILURE;
    }
    SiHelper::SetTimeBin(*NoiseDiode, tbin);  // set timebin info

    return StatusCode::SUCCESS;
}

float StripFrontEnd::meanValue(std::vector<float> &calibDataVect) const {
    float mean_value = 0.0;
    int nData = 0;
    unsigned vec_size = calibDataVect.size();

    for (unsigned int i = 0; i < vec_size; ++i) {
        if (calibDataVect[i] > 0.1) {
            mean_value += calibDataVect[i];
            ++nData;
        }
    }
    if (nData == 0) {
        return -1;
    }
    else {
        return mean_value / nData;
    }
}
