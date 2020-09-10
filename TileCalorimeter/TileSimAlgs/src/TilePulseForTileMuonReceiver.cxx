/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************************
//
//  Filename : TilePulseForTileMuonReceiver.cxx
//  Author   : Joao Gentil Mendes Saraiva (jmendes@cern.ch)
//  Created  : October 2013
//
//  DESCRIPTION:
//
//     The algorithm will do:
//     Create a digitized pulse with 7 samples based in a measured pulse.
//     The pulse is reconstructed using the matched filter alogtrithm.
//     Digits and reconstructed pulse parameters (e,t,qf) are saved in contaienrs in TES.
//
//     This is part of the tile-d project (2015).
//
//  HISTORY:
//
//     10.dec.2013 ready for submission in svn
//     16.jan.2014 fixed coverity errors
//     06.oct.2015 including the barrel cells 
//
//  BUGS:
// 
//
//
//****************************************************************************************

// Tile includes
#include "TilePulseForTileMuonReceiver.h"
#include "TileEvent/TileMutableDigitsContainer.h"
#include "TileEvent/TileMutableRawChannelContainer.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TilePulseShapes.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileRecUtils/TileRawChannelBuilder.h"


// Calo includes
#include "CaloIdentifier/TileID.h"

// Atlas includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "AthenaKernel/RNGWrapper.h"


// external
#include <CLHEP/Random/Randomize.h>
#include <CLHEP/Units/SystemOfUnits.h>

using CLHEP::RandGaussQ;
using CLHEP::RandFlat;
using CLHEP::MeV;

#include "cmath"

// constructor
//
TilePulseForTileMuonReceiver::TilePulseForTileMuonReceiver(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_tileID(nullptr)
  , m_tileHWID(nullptr)
  , m_tileInfo(nullptr)
  , m_cablingService(nullptr)
  , m_nSamples(0)
  , m_iTrig(0)
  , m_adcMax(0)
  , m_tileThresh(0.0)
  , m_nShape(0)
  , m_nBinsPerX(0)
  , m_binTime0(0)
  , m_timeStep(0.0)
  , m_run2(true)
{

}

// destructor
//
TilePulseForTileMuonReceiver::~TilePulseForTileMuonReceiver() {
}

// initialize
//
StatusCode TilePulseForTileMuonReceiver::initialize() {

  //  Check cabling RUN>=RUN2 OK
  //
  ATH_CHECK( m_cablingSvc.retrieve() );
  m_cablingService = m_cablingSvc->cablingService();

  if (! m_cablingService->isRun2Cabling() ) {
    ATH_MSG_INFO("TilePulseForTileMuonReceiver should not be used for RUN1 simulations");
    m_run2 = false;
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_INFO("Initializing TilePulseForTileMuonReceiver");
  }

  //=== retrieve TileID helper and TileInfo from det store
  CHECK(detStore()->retrieve(m_tileID));
  CHECK(detStore()->retrieve(m_tileHWID));
  CHECK(detStore()->retrieve(m_tileInfo, m_infoName));
  //=== get TileCondToolEmscale
  CHECK(m_tileToolEmscale.retrieve());
  //=== get TileCondToolNoiseSample
  CHECK(m_tileToolNoiseSample.retrieve());

  CHECK(m_MuRcvBuildTool.retrieve());

  m_nSamples = m_tileInfo->NdigitSamples();    // number of time slices for each chan
  m_iTrig = m_tileInfo->ItrigSample();      // index of the triggering time slice
  m_adcMax = m_tileInfo->ADCmax();          // adc saturation value
  m_tileThresh = m_tileInfo->ThresholdDigits(TileID::LOWGAIN);

  ATH_MSG_VERBOSE("Cabling Services: "                << m_cablingService
                   << " Number of Samples: "          << m_nSamples
                   << " Triggering tile slice: "      << m_iTrig
                   << " ADC saturation value: "       << m_adcMax
                   << " TileCal Threshold LOW GAIN: " << m_tileThresh);

  ATH_CHECK(m_rndmSvc.retrieve());

  m_nShape    = m_tileInfo->MuRcvNBins();
  m_nBinsPerX = m_tileInfo->MuRcvBinsPerX();
  m_binTime0  = m_tileInfo->MuRcvTime0Bin();
  m_timeStep  = 25.0 / m_nBinsPerX;

  ATH_MSG_VERBOSE( "Pulse info : "
		<< "shape "                  << m_nShape
                <<" nbins "                  << m_nBinsPerX
                <<" time "                   << m_binTime0
                <<" time step "              << m_timeStep
                <<" Triggering tile sample " << m_iTrig);

  // decrease by 1, now it is the position of lastest element in a vector
  //
  --m_nShape;

  if (m_useCoolPulseShapes) {
    ATH_MSG_INFO( "Using pulse from database.");

    CHECK(m_tileToolPulseShape.retrieve());
  } else {
    ATH_MSG_INFO( "Using pulse from TileInfo.");

    m_shapeMuonReceiver = m_tileInfo->MuRcvFullShape();
    m_shapeMuonReceiver.push_back(0.0);
    m_tileToolPulseShape.disable();
  }

  if (m_maskBadChannels) {
    CHECK(m_tileBadChanTool.retrieve());
  } else {
    m_tileBadChanTool.disable();
  }

  ATH_CHECK( m_hitContainerKey.initialize(m_run2) );
  ATH_CHECK( m_muRcvDigitsContainerKey.initialize(m_run2) );
  ATH_CHECK( m_muRcvRawChannelContainerKey.initialize(m_run2) );

  ATH_MSG_VERBOSE("TilePulseForTileMuonReceiver initialization completed");
  return StatusCode::SUCCESS;
}

// execute
//

StatusCode TilePulseForTileMuonReceiver::execute() {

  if (m_run2) {
    ATH_MSG_VERBOSE( "ATT: RUN2 settings TilePulseForTileMuonReceiver will run now" );
    ATH_MSG_DEBUG( "Executing TilePulseForTileMuonReceiver" );
  } else {
    ATH_MSG_VERBOSE( "ATT: RUN1 settings TilePulseForTileMuonReceiver will end now" );
    return StatusCode::SUCCESS;
  }

  // Conversion from TMDB channel number to channel number in a drawer: EB (0-3) LB(0-13)
  // Including the cells used in the "The potential of using the ATLAS Tile calorimeter in Phase-II for the
  // Level-0 muon trigger" (ATL-COM-TILECAL-2015-007.pdf): ALL D-layer + BC-8.
  //
#define nEBchan 6
#define nLBchan 9
  // EB: D5(L,R),D6(L,R),D4(L,R)
  int EBchan[nEBchan]={17,16,37,38,3,2};
  // LB: D0,D1(L,R),D2(L,R),D3(L,R),BC8(L,R)
  int LBchan[nLBchan]={0,13,14,25,24,41,44,39,40};
  // Set to maximum possible index 
#if (nEBchan > nLBchan)
  double pDigitSamplesArray[nEBchan][7];
#else
  double pDigitSamplesArray[nLBchan][7];
#endif

  // PULSE

  // Random generators output arrays
  //
  double Rndm[16];
  double Rndm_dG[1];

  // Noise and pedestal from db
  //
  double pedSim = 0.;
  double sigma_Hfn1 = 0.;
  double sigma_Hfn2 = 0.;
  double sigma_Norm = 0.;
  double sigmaSim(0.0);

  // Measured parameters: noise, pedestal and calibration
  //
  double muRcv_NoiseSigma;
  double muRcv_Ped;
  double muRcv_Calib;
  double muRcv_Max;
  // double muRcv_Thresh;

  // Get hit container from TES
  //
  SG::ReadHandle<TileHitContainer> hitContainer(m_hitContainerKey);
  ATH_CHECK( hitContainer.isValid() );

  // Set up buffers for handling information in a single collection.
  //
  IdentifierHash idhash;
  IdContext drawer_context = m_tileHWID->drawer_context();

  // Get a container for the digits
  //
  auto muRcvDigitsContainer = std::make_unique<TileMutableDigitsContainer>(true,
                                                                           TileFragHash::Digitizer,
                                                                           TileRawChannelUnit::ADCcounts);
  ATH_CHECK( muRcvDigitsContainer->status() );
    
  // Get a container for the raw channels
  //
  auto muRcvRawChannelContainer = std::make_unique<TileMutableRawChannelContainer>(true,
                                                                                   TileFragHash::MF,
                                                                                   TileRawChannelUnit::MegaElectronVolts,
                                                                                   SG::VIEW_ELEMENTS);
  ATH_CHECK( muRcvRawChannelContainer->status() );


  // Vector of digits to set into the container
  //
  std::vector<float> digitsBuffer(m_nSamples);

  // Prepare RNG service
  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this, m_randomStreamName);
  rngWrapper->setSeed( m_randomStreamName, Gaudi::Hive::currentContext() );

  /////////////////////////////////////////////////////////////////////////////////
  // (a.0) iterate over collections in the HIT container: access 'ros' and 'drawer'
  //

  for (const TileHitCollection* hitCollection : *hitContainer) {

    // Get array of HWID's for this drawer (stored locally).
    //
    HWIdentifier drawer_id = m_tileHWID->drawer_id(hitCollection->identify());
    int ros = m_tileHWID->ros(drawer_id);
    int drawer = m_tileHWID->drawer(drawer_id);
    int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    bool eb_ros   = ((ros == TileHWID::EXTBAR_POS) || (ros == TileHWID::EXTBAR_NEG));
    int upperLim  = (eb_ros) ? nEBchan : nLBchan;

    ATH_MSG_VERBOSE("(A.00) Looping over all collections for TMDB in the HIT container");
    memset(pDigitSamplesArray, 0, sizeof(pDigitSamplesArray));

    ATH_MSG_VERBOSE("(A.01) Going through collection ROS/DRAWER : "<< ros <<"/"<< drawer);
    ATH_MSG_DEBUG(" Going through collection ROS/DRAWER : "<< ros <<"/"<< drawer);

    if (m_cablingService->connected(ros, drawer)) {
      ATH_MSG_VERBOSE("(A.02)   ROS: "<< ros << " drawer: " << drawer << " is connected");
    } else {
      ATH_MSG_VERBOSE("(A.02)   ROS: "<< ros << " drawer: " << drawer << " is NOT connected");
      continue;
    }

    // Get drawer idhash for later access to the database to get ped and noise
    //
    m_tileHWID->get_hash(drawer_id, idhash, &drawer_context);

    //////////////////////////////////////////////////////////////////////////////
    // (a.1) Iterate over all hits in a collection : access 'channel'
    //
    if ( hitCollection->empty() ) ATH_MSG_DEBUG("-- No hits in this drawer! Filling channels with noise and pedestal. --");

    for (const TileHit* tile_hit : *hitCollection) {

      // Get the pmt ID
      //
      Identifier pmt_id = tile_hit->pmt_ID();

      // keep only D-cells and in addition cell BC8
      // 
      int tower = m_tileID->tower(pmt_id);
      int sample = m_tileID->sample(pmt_id);
      if ( ! ((sample == TileID::SAMP_D) || (sample == TileID::SAMP_BC && tower == 7)) )
        continue;

      ATH_MSG_VERBOSE("(B.00) ++ Iterate over all the D layer channels with hits for TMDB");

      // In COOL database data will be organized acoording to TMDB channel (TMDBchan): 0..n with n=5 in EB and n=8 in LB 
      int TMDBchan;
      // In here we need to access the real TILE HW channel (TILEchan) that it has a correspondance with TMDB chan given by EBchan and LBNchan
      int TILEchan;

      if (eb_ros) {
        // cells D5, D6 and then D4 at the end
        if (m_tileID->side(pmt_id) > 0)
          TMDBchan = 1 - m_tileID->pmt(pmt_id) + ((tower>9) ? (tower - 10) : 4);
        else
          TMDBchan =     m_tileID->pmt(pmt_id) + ((tower>9) ? (tower - 10) : 4);
	TILEchan=EBchan[TMDBchan];
      } else {
        // Barrel (extension for HL-LHC)
        if (tower == 0) {
          TMDBchan = 0; // cell D0, channel 0 always
        } else {
          // cells D1. D2, D3 and BC8
          if (m_tileID->side(pmt_id) > 0)
            TMDBchan = 1 - m_tileID->pmt(pmt_id) + ((tower<7) ? (tower-1) : 7);
          else
            TMDBchan =     m_tileID->pmt(pmt_id) + ((tower<7) ? (tower-1) : 7);
        }
	TILEchan=LBchan[TMDBchan];
      }

      double* pDigitSamples = pDigitSamplesArray[TMDBchan];

      if (msgLvl(MSG::VERBOSE)){
        HWIdentifier adc_id = m_tileHWID->adc_id(drawer_id, TMDBchan, TileID::LOWGAIN);

        ATH_MSG_VERBOSE( "(B.01) Correct pmt being transported in XXchan[]: "<<TMDBchan<<" "<<TILEchan<< "=?"
                         << m_tileHWID->channel(m_cablingService->s2h_channel_id(pmt_id))
                         << " For reference get TMDB adc_id: " << m_tileHWID->to_string(adc_id)  );
        ATH_MSG_VERBOSE( "(B.02) New hit in ROS/DRAWER/PMT "<<ros<<"/"<<drawer<<"/"<<TMDBchan<<" ("<<TILEchan<<")"
                         << " pmt_id "<< m_tileID->to_string(pmt_id,-1)
                         << " adc_id "<< m_tileHWID->to_string(adc_id)  );
      }
      
      // Scintillator Energy -> Cell Energy (uses sampling fraction)
      //
      double hit_calib = m_tileInfo->HitCalib(pmt_id);

      ATH_MSG_VERBOSE("------ Sampling fraction: " << hit_calib);

      /////////////////////////////////////////////////////////////////////////////////////
      // (a.2) Loop over the hits of this channel
      //       Calibrations are applied per subhit and energy added per subhit of a channel
      //

      int n_hits = tile_hit->size();

      ATH_MSG_VERBOSE("------ Number of hits in channel: " << n_hits);

      for (int ihit = 0; ihit < n_hits; ++ihit) {

        ATH_MSG_VERBOSE("(C.00) ++ Iterating over the hits of channel " << TILEchan  <<": hit #" << ihit);

        double e_hit = tile_hit->energy(ihit); // [MeV] energy deposited in scintillator
        double e_pmt = e_hit * hit_calib;       // [MeV] true cell energy

        ATH_MSG_VERBOSE("(C.01) Energy in scintillator [MeV]: " << e_hit << " true cell energy [MeV]: " << e_pmt);

        // Need to pass the negative of t_hit, this is because the trigger returns the amplitude
        // at a given phase whereas the t_hit returns it from t=0 when the hit took place
        //
        double t_hit = tile_hit->time(ihit);

        ATH_MSG_VERBOSE("(C.02.01) Phase " << t_hit);

        // Load pulse
        //
        int   k      = 0;
        float phase  = 0.0;
        float y      = 0.0;
        float dy     = 0.0;
        double shape = 0.0;

        int ishift = (int) (t_hit / m_timeStep + 0.5);

        ATH_MSG_VERBOSE( "(C.02.02)   ishift :" << t_hit << "/" << m_timeStep << "+0.5 = " << ishift);

        if (m_useCoolPulseShapes) {
          for (int js = 0; js < m_nSamples; ++js) {
            k = m_binTime0 + (js - m_iTrig) * m_nBinsPerX - ishift;
            if (k < 0) k = 0;
            else if (k > m_nShape) k = m_nShape;

            ATH_MSG_VERBOSE( "(C.02.03)   k : " << m_binTime0 << "+(" << js << "-" << m_iTrig << ")*" << m_nBinsPerX <<  "-" << ishift << " = " << k);

            phase = (k - m_binTime0) * m_timeStep;

            ATH_MSG_VERBOSE( "(C.02.04)   phase : " << k << "-" << m_binTime0 << "*" << m_timeStep << " = " << phase);

            m_tileToolPulseShape->getPulseShapeYDY(drawerIdx, TMDBchan, TileID::LOWGAIN, phase, y, dy);
            shape = (double) y;
            pDigitSamples[js] += e_pmt * shape; // MeV

            ATH_MSG_VERBOSE( "(C.03.0) Sample no.= " << js
                            << " idx= " << k
                            << " Shape wt. = " << shape
                            << " Amp = " << pDigitSamples[js] << " [MeV]");
          } //END loop over samples
        } else {
          for (int js = 0; js < m_nSamples; ++js) {
            k = m_binTime0 + (js - m_iTrig) * m_nBinsPerX - ishift;
            if (k < 0) k = 0;
            else if (k > m_nShape) k = m_nShape;

            ATH_MSG_VERBOSE( "(C.02.03)   k : " << m_binTime0 << "+(" << js << "-" << m_iTrig << ")*" << m_nBinsPerX <<  "-" << ishift << " = " << k);

            pDigitSamples[js] += e_pmt * m_shapeMuonReceiver[k]; // MeV

            ATH_MSG_VERBOSE( "(C.03.0) Sample no.= " << js
                            << " idx= " << k
                            << " Shape wt. = " << m_shapeMuonReceiver[k]
                            << " Amp = " << pDigitSamples[js]
                            << "[MeV]  Energy: " << e_pmt << " LOGAIN from TileInfo");
          } //END loop over samples
        } // END if (m_useCoolPulseShapes)
      } // END loop over sub-HITS

      ATH_MSG_VERBOSE("(C.04)   ENDED Loop over hits");
      ATH_MSG_DEBUG("   Number of hits " << n_hits
                    << " channel " << m_tileHWID->to_string(drawer_id,-2) << "/" << TMDBchan << "<-->" << TILEchan
                    << " digitized pulse [MeV] "<< pDigitSamples[0]
                    << "/" << pDigitSamples[1]
                    << "/" << pDigitSamples[2]
                    << "/" << pDigitSamples[3]
                    << "/" << pDigitSamples[4]
                    << "/" << pDigitSamples[5]
                    << "/" << pDigitSamples[6]);

    } // END loop over a HIT collection

    /////////////////////////////////////////////////////////////////////////
    // (a.3) The pulse has a shape and a amplitude in MeV now it is converted 
    //       into ADC counts and add the NOISE and the PEDESTAL
    //       PEDESTAL [ADC counts] and NOISE [ADC counts] as stored in Tile 
    //       Conditions (for NOW are fixed values LATER from COOL)
    //       Keep containers for each module (each partition) the same size 
    //       between events
    //
    for (int TMDBchan = 0; TMDBchan < upperLim; ++TMDBchan) {

      double* pDigitSamples=pDigitSamplesArray[TMDBchan];
      int TILEchan = (eb_ros) ? EBchan[TMDBchan] : LBchan[TMDBchan];

      HWIdentifier adc_id = m_tileHWID->adc_id(drawer_id, TMDBchan, TileID::LOWGAIN); 
      
      ATH_MSG_DEBUG( "(D.00) ++ Add noise and pedestal in "
                       << " TMDBchan: "   << TMDBchan << " TILEchan: " << TILEchan
                       << " ROS: "        << ros
                       << " drawer: "     << drawer
                       << " drawer idx: " << drawerIdx
                       << " drawer_id: "  << m_tileHWID->to_string(drawer_id,-2)
		       << " channel: "    << m_tileHWID->to_string(adc_id,-1));

      // Different for each channel_id might be the case in the future (now a const. in TileInfoLoader.cxx)
      //
      muRcv_NoiseSigma = m_tileInfo->MuRcvNoiseSigma(adc_id); // [adc]
      // muRcv_Thresh = m_tileInfo->MuRcvThresh(adc_id);      // [adc] ... not used
      muRcv_Ped   = m_tileInfo->MuRcvPed(adc_id);             // [adc]
      muRcv_Calib = m_tileInfo->MuRcvCalib(adc_id);           // pCb->[adc]
      muRcv_Max   = m_tileInfo->MuRcvMax(adc_id);             // [adc]

      ATH_MSG_VERBOSE( "(D.01)   Tile Muon Receiver parameters:"
                      << " sig " << muRcv_NoiseSigma
                      << " noi " << muRcv_NoiseSigma
                      << " ped " << muRcv_Ped
                      << " cal " << muRcv_Calib
                      << " max " << muRcv_Max);

      // adc/pCb / MeV/pCb = adc/MeV
      //
      double mev2ADC_factor = muRcv_Calib / m_tileToolEmscale->channelCalib(drawerIdx,TILEchan,TileID::LOWGAIN, 1.
                                                                            , TileRawChannelUnit::PicoCoulombs
                                                                            , TileRawChannelUnit::MegaElectronVolts);

      // Generate an array to randomize the noise for each digit
      //
      RandGaussQ::shootArray(*rngWrapper, m_nSamples, Rndm, 0.0, 1.0);

      ATH_MSG_VERBOSE( "(D.02)   Pulse digits [MeV]:"
                       << " " << pDigitSamples[0]
                       << " " << pDigitSamples[1]
                       << " " << pDigitSamples[2]
                       << " " << pDigitSamples[3]
                       << " " << pDigitSamples[4]
                       << " " << pDigitSamples[5]
                       << " " << pDigitSamples[6]
                       << " [All ZERO if there is no hit in channel.] ");

      ATH_MSG_VERBOSE( "(D.02.00)   Channel: "<<ros<<'/'<<drawer<<'/'<< TMDBchan
                      << " adc/pCb: "<< muRcv_Calib
                      << " Mev/pCb: "<< m_tileToolEmscale->channelCalib( drawerIdx, 
                                                                         TILEchan, 
                                                                         TileID::LOWGAIN, 
                                                                         1., 
                                                                         TileRawChannelUnit::PicoCoulombs, 
                                                                         TileRawChannelUnit::MegaElectronVolts)
                      << " final calibration factor adc/MeV: "<< mev2ADC_factor);

      // Collecting pedestal from the database
      //
      if (m_tilePedestal) {
        pedSim = m_tileToolNoiseSample->getPed(idhash, TMDBchan, TileID::LOWGAIN);
        // As in TileDigitsMaker bug fix for wrong ped value in DB
	//
        if (pedSim == 0.0) pedSim = 30.;
      } else {
        pedSim = muRcv_Ped;
      }
      // Collecting noise from the database
      //
      if (m_tileNoise) {
        RandFlat::shootArray(*rngWrapper, 1, Rndm_dG, 0.0, 1.0);
        sigma_Hfn1 = m_tileToolNoiseSample->getHfn1(idhash, TMDBchan, TileID::LOWGAIN);
        sigma_Hfn2 = m_tileToolNoiseSample->getHfn2(idhash, TMDBchan, TileID::LOWGAIN);
        if (sigma_Hfn1 > 0 || sigma_Hfn2) {
          sigma_Norm = sigma_Hfn1 / (sigma_Hfn1 + sigma_Hfn2 * m_tileToolNoiseSample->getHfnNorm(idhash, TMDBchan, TileID::LOWGAIN));
        } else {
          sigma_Hfn1 = m_tileToolNoiseSample->getHfn(idhash, TMDBchan, TileID::LOWGAIN);
          sigma_Norm = 1.;
        }
        if (Rndm_dG[0] < sigma_Norm) sigmaSim = sigma_Hfn1;
        else sigmaSim = sigma_Hfn2;
      } else {
        sigmaSim = muRcv_NoiseSigma;
      }
      // Loop over samples and either use noise and ped from db or user location (TileInfoLoader.cxx)
      //
      for (int js = 0; js < m_nSamples; ++js) {
        ATH_MSG_VERBOSE( "(D.02.0"<< js <<")   sample "<< js <<" E [MeV]: "<< pDigitSamples[js]);
        digitsBuffer[js] = pDigitSamples[js] * mev2ADC_factor;
        ATH_MSG_VERBOSE( "(D.02.0"<< js <<")   sample "<< js <<" calibration Mev->adc "<< mev2ADC_factor <<"-> E [adc]: "<< digitsBuffer[js]);
        // Pedestal (amp)
	//
        digitsBuffer[js] += pedSim;
        ATH_MSG_VERBOSE( "(D.02.0"<< js <<")   sample "<< js <<" adding pedestal "<< pedSim <<"-> E [adc]: "<< digitsBuffer[js]);
        // Noise (rms)
	//
        digitsBuffer[js] += sigmaSim * Rndm[js];
        ATH_MSG_VERBOSE( "(D.02.0"<< js <<")   sample "<< js <<" adding noise "<< sigmaSim * Rndm[js] <<"-> E [adc]: "<< digitsBuffer[js]);
        // Simulated pulse above allowed maximum
        //
        if (digitsBuffer[js] > muRcv_Max) digitsBuffer[js] = muRcv_Max;
        // Rounding the ADC counts
        //
        if (m_integerDigits) digitsBuffer[js] = round(digitsBuffer[js]);
      }
      // If channel is good, create TileDigits object and store in container.
      //
      bool chanIsBad = false;
      if (m_maskBadChannels) {
        TileBchStatus status = m_tileBadChanTool->getAdcStatus(drawerIdx, TILEchan, TileID::LOWGAIN);
        chanIsBad = status.isBad();
      }
      if (chanIsBad) {
        for (int js = 0; js < m_nSamples; ++js) {
          digitsBuffer[js] = 255;// in TMDB we have 8-bit ADCs
        }
        ATH_MSG_VERBOSE( "(D.03)   Masking Channel: "<< ros << '/' << drawer << '/' << TILEchan <<" ("<< TMDBchan <<") LowGain" );
      } else {
        ATH_MSG_VERBOSE( "(D.03)   Good Channel: "<< ros << '/' << drawer << '/' << TILEchan <<" ("<< TMDBchan <<") LowGain" );
      }
      ATH_MSG_VERBOSE( "(D.04)   Changed to TMDB adc_id: " << m_tileHWID->to_string(adc_id) 
                       << " and create a TileDigits object and set it into a container." );
      std::unique_ptr<TileDigits> muonReceiverDigits = std::make_unique<TileDigits>(adc_id, digitsBuffer);
      ATH_MSG_VERBOSE( "(D.05)   Create a TileRawChannelObject object and set it into a container " );
      TileRawChannel* muRcvRawChannel = m_MuRcvBuildTool->rawChannel(muonReceiverDigits.get());
      ATH_CHECK( muRcvDigitsContainer->push_back(std::move(muonReceiverDigits)) );
      ATH_CHECK( muRcvRawChannelContainer->push_back(muRcvRawChannel) );
      if (msgLvl(MSG::DEBUG)){
        ATH_MSG_DEBUG( " Channel " << m_tileHWID->to_string(adc_id,-1) 
                       << " Digitized pulse [ADC] "<< digitsBuffer[0]
                       << "/" << digitsBuffer[1]
                       << "/" << digitsBuffer[2]
                       << "/" << digitsBuffer[3]
                       << "/" << digitsBuffer[4]
                       << "/" << digitsBuffer[5]
                       << "/" << digitsBuffer[6]  );
        ATH_MSG_DEBUG( " Raw channel reconstruction Ch: "<< m_tileHWID->to_string(adc_id,-1)
                       <<" E [ADC]: "<< muRcvRawChannel->amplitude()
                       <<" Time [ns]: "<< muRcvRawChannel->time()
                       <<" Qf: "<< muRcvRawChannel->quality()  );
      }
    }
  } // END loop over all HIT collections in container
  if (msgLvl(MSG::VERBOSE)) muRcvDigitsContainer->print();
  // (b) Register the digits container in the TES
  //
  ATH_MSG_VERBOSE ( "(A.05)   Send to event store all collected objects " );


  SG::WriteHandle<TileDigitsContainer> muRcvDigitsCnt(m_muRcvDigitsContainerKey);
  ATH_CHECK( muRcvDigitsCnt.record(std::move(muRcvDigitsContainer)) );

  SG::WriteHandle<TileRawChannelContainer> muRcvRawChannelCnt(m_muRcvRawChannelContainerKey);
  ATH_CHECK( muRcvRawChannelCnt.record(std::move(muRcvRawChannelContainer)) );

  ATH_MSG_VERBOSE( "TilePulseForTileMuonReceiver execution completed" );

  return StatusCode::SUCCESS;
}

// finalize
//
StatusCode TilePulseForTileMuonReceiver::finalize() {
  ATH_MSG_VERBOSE("Finalizing TilePulseForTileMuonReceiver");

  ATH_MSG_INFO("TilePulseForTileMuonReceiver finalized successfully");
  return StatusCode::SUCCESS;
}
