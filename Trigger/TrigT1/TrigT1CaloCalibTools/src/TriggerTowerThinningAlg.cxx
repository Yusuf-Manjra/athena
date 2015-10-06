//  ***************************************************************************
//  *                                                                         *
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  *                                                                         *
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *
//  ***************************************************************************/

/**
   @class TriggerTowerThinningAlg
   @brief Thinning algorithm for the L1Calo Trigger Towers

   This algorithm can be used to keep only TriggerTowers which have either:
   1.) An ADC value > m_minADC  OR
   2.) A Calo Cell energy > m_minCaloCellEnergy

   For a random (TRandom3) m_minRandom % of events, all Trigger Towers are saved.
   This is very useful for pedestal and noise studies

**/

#include "TrigT1CaloCalibTools/TriggerTowerThinningAlg.h"
// TrigT1 common definitions
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

namespace DerivationFramework {

  TriggerTowerThinningAlg::TriggerTowerThinningAlg(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p),
    m_thinningSvc( "ThinningSvc",  n ),
    m_triggerTowerLocation(LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation),
    m_minCaloCellEnergy(0.05),
    m_minADC(30),
    m_useRandom(false),
    m_minRandom(0.03),
    m_nEventsProcessed(0),
    m_nEventsAllTriggerTowersKeptByRandom(0),
    m_nTriggerTowersProcessed(0),
    m_nTriggerTowersKept(0),
    m_nTriggerTowersRejected(0),
    m_random(0)
  {
    declareInterface<DerivationFramework::IThinningTool>(this);    
    declareProperty("ThinService",m_thinningSvc);
    declareProperty("TriggerTowerLocation", m_triggerTowerLocation);
    declareProperty("MinCaloCellEnergy",m_minCaloCellEnergy);
    declareProperty("MinADC",m_minADC);
    declareProperty("UseRandom",m_useRandom);
    declareProperty("MinRandom",m_minRandom);
  }

  TriggerTowerThinningAlg::~TriggerTowerThinningAlg()
  {
  }

  StatusCode TriggerTowerThinningAlg::initialize(){
    ATH_MSG_INFO("L1Calo TriggerTowerThinningAlg::initialize()");

    // Random number generator
    if(m_useRandom == true){
      m_random = new TRandom3(0);
    }  

    return StatusCode::SUCCESS;
  }

  StatusCode TriggerTowerThinningAlg::doThinning() const{
    StatusCode sc;

    // Create the mask to be used for thinning
    std::vector<bool> mask;

    // Counters
    unsigned long nKeep(0),nReject(0),nTotal(0);
    ++m_nEventsProcessed;
    
    // Shall I proceed?
    if (!evtStore()->contains<xAOD::TriggerTowerContainer>( m_triggerTowerLocation )) {    
      ATH_MSG_ERROR("No TriggerTowers in input file, so can't do any thinning!");
      return StatusCode::FAILURE;
    }
    
    const xAOD::TriggerTowerContainer* tts(nullptr);
    CHECK( evtStore()->retrieve( tts , m_triggerTowerLocation ) );
    
    mask.assign(tts->size(),false); // default: don't keep any clusters
    
    // Should we save the trigger tower or not???
    // If this is calibration data, then yes - save the lot
    // Randomly save all trigger towers m_minRandom % of the time.
    //
    // If this is collisions data then only save if:
    // 1.) Any ADC value is greater than m_minADC  OR
    // 2.) The CaloCell energy is greater than m_minCaloCellEnergy

    bool globalSaveMe(false);
    // Random number save all
    if(m_useRandom == true){
      if(globalSaveMe == false){
        if(m_random->Rndm() < m_minRandom){
          globalSaveMe = true;
          m_nEventsAllTriggerTowersKeptByRandom++;
        }
      }
    }

    // Iterate over all trigger towers
    for(auto tt : *tts){

      bool saveMe(false);
      if(globalSaveMe == true){saveMe = true;}
      
      // Test for Calo Energy
      if(saveMe == false){
        if (tt->isAvailable< std::vector<float> > ("CaloCellEnergyByLayer")) {
          std::vector<float> caloCellEnergyByLayer = tt->auxdataConst< std::vector<float> > ("CaloCellEnergyByLayer");
          float totalCaloCellEnergy(0.);
          for (auto c : caloCellEnergyByLayer) {
            totalCaloCellEnergy += c;
          }
          if (totalCaloCellEnergy > m_minCaloCellEnergy) {
            saveMe = true;
          }
        }
      }
      

      // Test for ADC values
      if (saveMe == false) {
        const std::vector<uint16_t> ttADC = tt->adc();
        for (std::vector<uint16_t>::const_iterator i=ttADC.begin();i!=ttADC.end();++i) {
          if ( (*i) > m_minADC) {
            saveMe = true;
            break;
          }
        }
      }

      // assign result to mask
      mask[nTotal] = saveMe;
      
      ++nTotal;
    } // End loop over trigger towers


    if (m_thinningSvc->filter(*tts,mask,IThinningSvc::Operator::Or).isFailure()) {
      ATH_MSG_ERROR("Application of thinning service failed! ");
      return StatusCode::FAILURE;
    }

    // Counters
    m_nTriggerTowersProcessed += nTotal;
    m_nTriggerTowersKept += nKeep;
    m_nTriggerTowersRejected += nReject;

    ATH_MSG_DEBUG(" L1Calo Trigger Tower Thinning statistics: keeping " << nKeep << " cells"
          << " and rejecting " << nReject << " cells");


    return sc;
  }

  StatusCode TriggerTowerThinningAlg::finalize(){

        ATH_MSG_INFO(
          "==> finalize " << name() << "...\n"
          << "***************************************************************\n"
          << "Results of " << name() << " thinning algorithm:\n"
          << "-------------");
        ATH_MSG_INFO(" Number of processed events:  " << m_nEventsProcessed);

        if(m_nTriggerTowersProcessed > 0) { 
          ATH_MSG_INFO(
            " Average percent of Trigger Towers kept = "
            << 100.0 * m_nTriggerTowersKept / (double)m_nTriggerTowersProcessed << " %");
        }
        if(m_useRandom == true && m_nEventsProcessed > 0){
          ATH_MSG_INFO(
            " Percentage of events where all Trigger Towers were kept (should be approx "<< 100.0*m_minRandom << ") = "
            << 100.0 * m_nEventsAllTriggerTowersKeptByRandom / (double)m_nEventsProcessed);
        }
      
    delete m_random;


    return StatusCode::SUCCESS;
  }

} // namespace
