/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// +==========================================================================+
// +                                                                          +
// + Authors .......: G.Unal                                                  +
// +    migrate LAr digitization to PileUpTool framework                      +
// +==========================================================================+
//

#include "LArDigitization/LArPileUpTool.h"

#include "LArDigitization/LArHitEMap.h"

#include "AthenaKernel/ITriggerTime.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CaloIdentifier/LArID.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "CaloIdentifier/LArNeighbours.h"
#include "Identifier/IdentifierHash.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArSimEvent/LArHitFloat.h"
#include "LArSimEvent/LArHit.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "EventInfoUtils/EventIDFromStore.h"

#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"

#include <CLHEP/Random/Randomize.h>

using CLHEP::RandFlat;
using CLHEP::RandGaussZiggurat;

LArPileUpTool::LArPileUpTool(const std::string& type, const std::string& name, const IInterface* parent) :
  PileUpToolBase(type, name, parent) {

  declareInterface<ILArPileUpTool>(this);

  // default properties
  m_LowGainThresh[EM]    = 3900;//ADC counts in MediumGain
  m_HighGainThresh[EM]   = 1300;//ADC counts in MediumGain
  m_LowGainThresh[HEC]   = 2500;//ADC counts in MediumGain
  m_HighGainThresh[HEC]  = 0;//-> high-gain never used for HEC
  m_LowGainThresh[FCAL]  = 2000.;//ADC counts in Medium Gain
  m_HighGainThresh[FCAL] = 1100.;//ADCcounts in MediumGain
  m_LowGainThresh[EMIW]    = 3900;//ADC counts in MediumGain
  m_HighGainThresh[EMIW]   = 1300;//ADC counts in MediumGain
  //
  // ........ declare the private data as properties
  //
  declareProperty("LowGainThreshEM",m_LowGainThresh[EM],"Medium/Low gain transition in EM");
  declareProperty("HighGainThreshEM",m_HighGainThresh[EM],"Medium/High gain transition in EM");
  declareProperty("LowGainThreshHEC",m_LowGainThresh[HEC],"Medium/Low gain transition in HEC");
  declareProperty("HighGainThreshHEC",m_HighGainThresh[HEC],"Medium/High gain transition in HEC");
  declareProperty("LowGainThreshFCAL",m_LowGainThresh[FCAL],"Medium/Low gain transition in FCAL");
  declareProperty("HighGainThreshFCAL",m_HighGainThresh[FCAL],"Medium/High gain transition in FCAL");
  declareProperty("LowGainThreshEMECIW",m_LowGainThresh[EMIW],"Medium/Low gain transition in EMEC IW");
  declareProperty("HighGainThreshEMECIW",m_HighGainThresh[EMIW],"Medium/High gain transition in EMEC IW");

  return;
}


LArPileUpTool::~LArPileUpTool()
{
  return;
}


StatusCode LArPileUpTool::initialize()
{
	
   ATH_MSG_INFO(" initialize LArPileUpTool : digit container name " << m_DigitContainerName);
  //
  // ........ print random event overlay flag
  //
  if (m_RndmEvtOverlay)
  {
    m_NoiseOnOff = false ;
    m_PileUp = true ;
    ATH_MSG_INFO(" pileup and/or noise added by overlaying digits of random events");
    if (m_isMcOverlay) ATH_MSG_INFO("   random events are from MC ");
    else               ATH_MSG_INFO("   random events are from data ");
  }
  else
  {
     ATH_MSG_INFO(" No overlay of random events");
  }

  ATH_CHECK(m_mergeSvc.retrieve());
  ATH_MSG_INFO( "PileUpMergeSvc successfully initialized");

  //
  // ......... print the noise flag
  //
  if ( m_NoiseOnOff )
  {
    ATH_MSG_INFO(" Electronic noise will be added in each cell " );
  }
  else
  {
    ATH_MSG_INFO(" No electronic noise added.");

    //not useful (see MakeDigit), but in case of...
    m_NoiseInEMB= false;
    m_NoiseInEMEC=false;
    m_NoiseInHEC= false;
    m_NoiseInFCAL=false;
  }

  //
  // ............ print cross-talk configuration
  //
  if (m_CrossTalk )
  {
    ATH_MSG_INFO(" Cross-talk in EM barrel will be taken into account : ");
    ATH_MSG_INFO("     Cross talk strip strip included ");
    if (m_CrossTalk2Strip) ATH_MSG_INFO("     Cross talk strip-2nd strip included ");
    ATH_MSG_INFO("     Cross talk strip middle included ");
    if (m_CrossTalkStripMiddle) ATH_MSG_INFO("     Cross talk strip middle included ");
    if (m_CrossTalkMiddle)      ATH_MSG_INFO("     Cross talk middle middle included");
  }
  else
  {
    ATH_MSG_INFO(" no Cross-Talk simulated");
  }

  if (m_onlyUseContainerName) {
    if (m_useLArHitFloat) {
      for (const auto& RHkey : m_hitFloatContainerKeys.keys()) {
        m_hitContainerNames.push_back(RHkey->key());
      }
    }
    else {
      for (const auto& RHkey : m_hitContainerKeys.keys()) {
        m_hitContainerNames.push_back(RHkey->key());
      }
    }
  }
  ATH_MSG_DEBUG("Input objects in these containers : '" << m_hitContainerNames << "'");

  // Initialize ReadHandleKey
  ATH_CHECK(m_hitContainerKeys.initialize(!m_onlyUseContainerName && !m_hitContainerKeys.empty() ));
  ATH_CHECK(m_hitFloatContainerKeys.initialize(!m_onlyUseContainerName && !m_hitFloatContainerKeys.empty() ));

  if (m_Windows) {
    ATH_CHECK(  detStore()->retrieve(m_caloDDMgr, "CaloMgr") );
  }

  //retrieve ID helpers
  ATH_CHECK(detStore()->retrieve(m_calocell_id,"CaloCell_ID"));


  const CaloIdManager* caloIdMgr = nullptr;
  StatusCode sc = detStore()->retrieve(caloIdMgr);
  if (sc.isFailure()) {
    ATH_MSG_ERROR(" Unable to retrieve CaloIdManager from DetectoreStore");
    return StatusCode::FAILURE;
  }
  m_larem_id   = caloIdMgr->getEM_ID();
  m_larhec_id  = caloIdMgr->getHEC_ID();
  m_larfcal_id = caloIdMgr->getFCAL_ID();

  sc = detStore()->retrieve(m_laronline_id);
  if (sc.isFailure()) {
    ATH_MSG_ERROR(" Unable to retrieve LArOnlineId from DetectoreStore");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_adc2mevKey.initialize());

  // retrieve tool to compute sqrt of time correlation matrix
  if ( !m_RndmEvtOverlay  &&  m_NoiseOnOff) {
    ATH_CHECK(m_autoCorrNoiseKey.initialize());
  }
  if (m_maskingTool.retrieve().isFailure()) {
       ATH_MSG_INFO(" No tool for bad channel masking");
      m_useBad=false;
  }

  ATH_CHECK(m_badFebKey.initialize());

  if (m_useTriggerTime) {
     if (m_triggerTimeTool.retrieve().isFailure()) {
        ATH_MSG_ERROR(" Unable to find Trigger Time Tool");
        return StatusCode::FAILURE;
     }
  }
  else {
    m_triggerTimeTool.disable();
  }

  ATH_CHECK(m_rndmGenSvc.retrieve());

  if (m_NSamples>32) {
     ATH_MSG_WARNING(" Cannot do more than 32 samples:  NSamples reset to 32 ");
     m_NSamples=32;
  }
// working arrays to store rndm numbers,samples
  m_Samples.resize(m_NSamples);
  if(m_doDigiTruth) {
    m_Samples_DigiHSTruth.resize(m_NSamples);
  }

  m_Noise.resize(m_NSamples);

// register data handle for conditions data

  if ( !m_RndmEvtOverlay && !m_pedestalNoise && m_NoiseOnOff ) {
    ATH_CHECK(m_noiseKey.initialize());
  }
  ATH_CHECK(m_shapeKey.initialize());
  ATH_CHECK(m_fSamplKey.initialize());
  ATH_CHECK(m_pedestalKey.initialize());

  ATH_CHECK(m_cablingKey.initialize());

  ATH_CHECK(m_hitMapKey.initialize());
  ATH_CHECK(m_hitMapKey_DigiHSTruth.initialize());

  ATH_CHECK(m_DigitContainerName.initialize());
  ATH_CHECK(m_DigitContainerName_DigiHSTruth.initialize());
  


  // decide sample to use for gain selection
  //   It is sample 2 (starting from 0) by default when we start from beginning of pulse shape
  //   Then shift by firstSample
  //    Note: this logic could be improved by looking at the true maximum of the full pulse shape instead, right now we
  //     implicitely assume that all MC 32-samples pulse shape have always the max at sample 2......
  m_sampleGainChoice = 2 - m_firstSample;
  if (m_sampleGainChoice<0) m_sampleGainChoice=0;

  ATH_MSG_DEBUG("Initialization completed successfully");

  return StatusCode::SUCCESS;

}

// ----------------------------------------------------------------------------------------------------------------------------------

StatusCode LArPileUpTool::prepareEvent(unsigned int /*nInputEvents */)
{

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  m_cabling=*cablingHdl;
  if(!m_cabling) {
     ATH_MSG_ERROR("Failed to retrieve LAr Cabling map with key " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }

  m_hitmap=SG::makeHandle(m_hitMapKey);
  auto hitMapPtr=std::make_unique<LArHitEMap>(m_cabling,m_calocell_id,m_caloDDMgr,m_RndmEvtOverlay);
  ATH_CHECK(m_hitmap.record(std::move(hitMapPtr)));
  ATH_MSG_DEBUG(" Number of created  cells in Map " << m_hitmap->GetNbCells());

  if (!m_useMBTime) m_energySum.assign(m_hitmap->GetNbCells(),0.);

  
  if (m_doDigiTruth) {
    m_hitmap_DigiHSTruth=SG::makeHandle(m_hitMapKey_DigiHSTruth);
    auto hitMapPtr=std::make_unique<LArHitEMap>(m_cabling,m_calocell_id,m_caloDDMgr,m_RndmEvtOverlay);
    ATH_CHECK(m_hitmap_DigiHSTruth.record(std::move(hitMapPtr)));
    if (!m_useMBTime) m_energySum_DigiHSTruth.assign(m_hitmap_DigiHSTruth->GetNbCells(),0.);
  }

  // get the trigger time if requested

  m_trigtime=0;
  if (m_useTriggerTime && m_triggerTimeTool) {
     m_trigtime = m_triggerTimeTool->time();
     ATH_MSG_DEBUG(" Trigger time used : " << m_trigtime);
  }

  ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this);
  rngWrapper->setSeed( name(), Gaudi::Hive::currentContext() );

  // add random phase (i.e subtract it from trigtime)
  if (m_addPhase) {
    m_trigtime = m_trigtime - (m_phaseMin + (m_phaseMax-m_phaseMin)*RandFlat::shoot(*rngWrapper)  );
  }

  if (m_Windows) {
    ATH_MSG_DEBUG(" redefine windows list ");
    m_hitmap->BuildWindows(m_WindowsEtaSize,m_WindowsPhiSize,
                          m_WindowsPtCut);
    if(m_doDigiTruth) {
      m_hitmap_DigiHSTruth->BuildWindows(m_WindowsEtaSize,m_WindowsPhiSize, m_WindowsPtCut);
    }

  }

  //
  // ....... create the LAr Digit Container
  //
  
  m_DigitContainer = SG::makeHandle(m_DigitContainerName);//new LArDigitContainer();
  ATH_CHECK(m_DigitContainer.record(std::make_unique<LArDigitContainer>()));

  if (m_doDigiTruth) {
    m_DigitContainer_DigiHSTruth = SG::makeHandle(m_DigitContainerName_DigiHSTruth);//new LArDigitContainer();
    ATH_CHECK(m_DigitContainer_DigiHSTruth.record(std::make_unique<LArDigitContainer>()));
  }

  //
  // ..... get OFC pointer for overlay case

  m_larOFC=NULL;
  if(m_RndmEvtOverlay  && !m_isMcOverlay) {
    StatusCode sc=detStore()->retrieve(m_larOFC);
    if (sc.isFailure())
    {
      ATH_MSG_ERROR("Can't retrieve LArOFC from Conditions Store");
      return StatusCode::FAILURE;
    }
  }


  m_nhit_tot = 0;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------------------------------------------------------

StatusCode LArPileUpTool::processBunchXing(int bunchXing,
                                           SubEventIterator bSubEvents,
                                           SubEventIterator eSubEvents)
{
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  float tbunch = (float)(bunchXing);

//
// ............ loop over the wanted hit containers
//
  SubEventIterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {

    // do we deal with the MC signal event ?
    bool isSignal = ( (iEvt->type()==xAOD::EventInfo_v1::PileUpType::Signal) || m_RndmEvtOverlay);

    // fill LArHits in map
    if (this->fillMapFromHit(iEvt, tbunch,isSignal).isFailure()) {

      ATH_MSG_ERROR(" cannot fill map from hits ");
      return StatusCode::FAILURE;
    }

    // store digits from randoms for overlay
    if (m_RndmEvtOverlay) {
      const LArDigitContainer* rndm_digit_container;
      if (m_mergeSvc->retrieveSingleSubEvtData(m_RandomDigitContainer, rndm_digit_container, bunchXing, iEvt).isSuccess()) {
	int ndigit=0;
	for (const LArDigit* digit : *rndm_digit_container) {
	  if (m_hitmap->AddDigit(digit)) ndigit++;
	}
	ATH_MSG_INFO(" Number of digits stored for RndmEvt Overlay " << ndigit);
      }
    }

    ++iEvt;

  }

  if (!m_useMBTime) {
      if (!this->fillMapfromSum(tbunch)) {
         ATH_MSG_ERROR(" error in FillMapFromSum ");
         return StatusCode::FAILURE;
      }
  }


  return StatusCode::SUCCESS;

}

// ---------------------------------------------------------------------------------------------------------------------------------

StatusCode LArPileUpTool::processAllSubEvents()
{

  if (this->prepareEvent(0).isFailure()) {
     ATH_MSG_ERROR("error in prepareEvent");
     return StatusCode::FAILURE;
  }

  if(!m_onlyUseContainerName && m_RndmEvtOverlay) {
    auto hitVectorHandles = m_hitContainerKeys.makeHandles();
    for (auto & inputHits : hitVectorHandles) {
      if (!inputHits.isValid()) {
        ATH_MSG_ERROR("BAD HANDLE"); //FIXME improve error here
        return StatusCode::FAILURE;
      }
      bool isSignal(true);
      double SubEvtTimOffset(0.0);
      double timeCurrBunch=-9999999.;
      for (const LArHit* hit : *inputHits) {
        m_nhit_tot++;
        float energy = (float) (hit->energy());
        float time;
        if (m_ignoreTime && isSignal) time=0.;
        else time = (float) (SubEvtTimOffset+ hit->time() -m_trigtime);
        Identifier cellId = hit->cellID();
        if (!m_useMBTime) {
          if (std::fabs(SubEvtTimOffset-timeCurrBunch)>1.) {
            if (timeCurrBunch>-9999.) {
              if (!this->fillMapfromSum(timeCurrBunch)) {
                ATH_MSG_ERROR(" error in FillMapFromSum ");
                return(StatusCode::FAILURE);
              }
            }
            timeCurrBunch = SubEvtTimOffset;
          }
        }
        if (this->AddHit(cellId,energy,time,isSignal).isFailure()) return StatusCode::FAILURE;
      } // End of loop over LArHitContainer
    } // End of loop over SG::ReadHandles
  }

  if (!m_PileUp) {
    float time=0.;
    StoreGateSvc* myEvtStore = &(*evtStore());
    if (this->fillMapFromHit(myEvtStore,time,true).isFailure()) {
      ATH_MSG_ERROR("error in fillMapFromHit");
      return StatusCode::FAILURE;
    }
  }

  else {

    for (const std::string& containerName : m_hitContainerNames) {

      ATH_MSG_DEBUG(" pileUpOld asking for: " << containerName);

      double timeCurrBunch=-9999999.;

      if (!m_useLArHitFloat) {
        typedef PileUpMergeSvc::TimedList<LArHitContainer>::type TimedHitContList;
        TimedHitContList hitContList;
        //
        // retrieve list of pairs (time,container) from PileUp service

        if (!(m_mergeSvc->retrieveSubEvtsData(containerName
            ,hitContList).isSuccess()) && hitContList.size()==0) {
         ATH_MSG_ERROR("Could not fill TimedHitContList");
         return StatusCode::FAILURE;
        }

        // loop over this list
        TimedHitContList::iterator iFirstCont(hitContList.begin());
        TimedHitContList::iterator iEndCont(hitContList.end());
        if(m_RndmEvtOverlay) {
          iEndCont = iFirstCont ;
          ATH_MSG_DEBUG(" random event overlay mode : only time 0 read ");
          ++iEndCont ;
        }
        double SubEvtTimOffset;
        while (iFirstCont != iEndCont) {
        // get time for this subevent
        // new coding of time information (January 05)
          bool isSignal = ( iFirstCont == hitContList.begin());
          const PileUpTimeEventIndex* time_evt = &(iFirstCont->first);
          SubEvtTimOffset = time_evt->time();
          // get LArHitContainer for this subevent
          const LArHitContainer& firstCont = *(iFirstCont->second);
          // Loop over cells in this LArHitContainer
          for (const LArHit* hit : firstCont) {
            m_nhit_tot++;
            float energy = (float) (hit->energy());
            float time;
            if (m_ignoreTime && isSignal) time=0.;
            else time = (float) (SubEvtTimOffset+ hit->time() -m_trigtime);
            Identifier cellId = hit->cellID();

            if (!m_useMBTime) {
              if (std::fabs(SubEvtTimOffset-timeCurrBunch)>1.) {
                 if (timeCurrBunch>-9999.) {
                    if (!this->fillMapfromSum(timeCurrBunch)) {
                        ATH_MSG_ERROR(" error in FillMapFromSum ");
                        return(StatusCode::FAILURE);
                    }
                  }
                  timeCurrBunch = SubEvtTimOffset;
              }
            }
            if (this->AddHit(cellId,energy,time,isSignal).isFailure()) return StatusCode::FAILURE;
          }              //  loop over  hits
          ++iFirstCont;
        }                 // loop over subevent list
      }

      else {

        typedef PileUpMergeSvc::TimedList<LArHitFloatContainer>::type TimedHitContList;
        TimedHitContList hitContList;
        //
        // retrieve list of pairs (time,container) from PileUp service

        if (!(m_mergeSvc->retrieveSubEvtsData(containerName
              ,hitContList).isSuccess()) && hitContList.size()==0) {
         ATH_MSG_ERROR("Could not fill TimedHitContList");
         return StatusCode::FAILURE;
        }

        // loop over this list
        TimedHitContList::iterator iFirstCont(hitContList.begin());
        TimedHitContList::iterator iEndCont(hitContList.end());
        if(m_RndmEvtOverlay) {
          iEndCont = iFirstCont ;
          ATH_MSG_DEBUG("random event overlay mode : only time 0 read ");
          ++iEndCont ;
        }
        double SubEvtTimOffset;
        while (iFirstCont != iEndCont) {
          bool isSignal = ( iFirstCont == hitContList.begin());
          // get time for this subevent
          // new coding of time information (January 05)
          const PileUpTimeEventIndex* time_evt = &(iFirstCont->first);
          SubEvtTimOffset = time_evt->time();
          // get LArHitContainer for this subevent
          const LArHitFloatContainer& firstCont = *(iFirstCont->second);
          // Loop over cells in this LArHitContainer
          for (const LArHitFloat& hit : firstCont) {
            m_nhit_tot++;
            float energy = (float)( hit.energy());
            float time;
            if (m_ignoreTime && isSignal) time=0.;
            else time = (float) (SubEvtTimOffset+ hit.time() - m_trigtime);
            Identifier cellId = hit.cellID();

            if (!m_useMBTime) {
              if (std::fabs(SubEvtTimOffset-timeCurrBunch)>1.) {
                 if (timeCurrBunch>-9999.) {
                    if (!this->fillMapfromSum(timeCurrBunch)) {
                        ATH_MSG_ERROR(" error in FillMapFromSum ");
                        return(StatusCode::FAILURE);
                    }
                  }
                  timeCurrBunch = SubEvtTimOffset;
              }
            }
            if (this->AddHit(cellId,energy,time,isSignal).isFailure()) return StatusCode::FAILURE;
          }              //  loop over  hits
          ++iFirstCont;
        }                 // loop over subevent list

      }  // LArHitFloat vs LArHit useage

      if (!m_useMBTime) {
        if (!this->fillMapfromSum(timeCurrBunch)) {
             ATH_MSG_ERROR(" error in FillMapFromSum ");
             return(StatusCode::FAILURE);
        }
      }

    }    // loop over containers

    // get digits for random overlay
    if(m_RndmEvtOverlay)
    {

      typedef PileUpMergeSvc::TimedList<LArDigitContainer>::type TimedDigitContList ;
      LArDigitContainer::const_iterator rndm_digititer_begin ;
      LArDigitContainer::const_iterator rndm_digititer_end ;
      LArDigitContainer::const_iterator rndm_digititer ;


      TimedDigitContList digitContList;
      if (!(m_mergeSvc->retrieveSubEvtsData(m_RandomDigitContainer,
           digitContList).isSuccess()) || digitContList.size()==0)
      {
         ATH_MSG_ERROR("Cannot retrieve LArDigitContainer for random event overlay or empty Container");
	 ATH_MSG_ERROR("Random Digit Key= " << m_RandomDigitContainer << ",size=" << digitContList.size());
         return StatusCode::FAILURE ;
      }
      TimedDigitContList::iterator iTzeroDigitCont(digitContList.begin()) ;
      double SubEvtTimOffset;
      // get time for this subevent
      const PileUpTimeEventIndex* time_evt = &(iTzeroDigitCont->first);
      SubEvtTimOffset = time_evt->time();
      ATH_MSG_DEBUG(" Subevt time : " << SubEvtTimOffset);
      const LArDigitContainer& rndm_digit_container =  *(iTzeroDigitCont->second);
      int ndigit=0;
      for (const LArDigit* digit : rndm_digit_container) {
       if (m_hitmap->AddDigit(digit)) ndigit++;
      }
      ATH_MSG_INFO(" Number of digits stored for RndmEvt Overlay " << ndigit);
    }

  }  // if pileup

  if (this->mergeEvent().isFailure()) {
    ATH_MSG_ERROR("error in mergeEvent");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

// ---------------------------------------------------------------------------------------------------------------------------------


StatusCode LArPileUpTool::mergeEvent()
{
   SG::ReadCondHandle<LArBadFebCont> badFebHdl(m_badFebKey);
   const LArBadFebCont* badFebs=*badFebHdl;

   int it,it_end;
   it =  0;
   it_end = m_hitmap->GetNbCells();

   Identifier cellID;
   const std::vector<std::pair<float,float> >* TimeE;
   const std::vector<std::pair<float,float> >* TimeE_DigiHSTruth = nullptr;

   ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this);
   CLHEP::HepRandomEngine * engine = *rngWrapper;

   for( ; it!=it_end;++it) // now loop on cells
   {
      const LArHitList& hitlist = m_hitmap->GetCell(it);
      
      if (!m_Windows || hitlist.inWindows()) {
	TimeE = &(hitlist.getData());
	if(m_doDigiTruth) {
	  auto& hitlist_DigiHSTruth=m_hitmap_DigiHSTruth->GetCell(it);
	  TimeE_DigiHSTruth = &(hitlist_DigiHSTruth.getData());
	}

	if (TimeE->size() > 0 || m_NoiseOnOff || m_RndmEvtOverlay) {
	  const Identifier cellID=m_calocell_id->cell_id(IdentifierHash(it));
	  HWIdentifier ch_id = m_cabling->createSignalChannelIDFromHash(IdentifierHash(it));
	  HWIdentifier febId = m_laronline_id->feb_Id(ch_id);
	  bool missing=!(badFebs->status(febId).good());
	  if (!missing) {
	    const LArDigit * digit = 0 ;
	    if(m_RndmEvtOverlay) digit = m_hitmap->GetDigit(it);
	    // MakeDigit called if in no overlay mode or
	    // if in overlay mode and random digit exists
	    if( (!m_RndmEvtOverlay) || (m_RndmEvtOverlay && digit) ) {
	      if ( this->MakeDigit(cellID, ch_id,TimeE, digit, engine, TimeE_DigiHSTruth)
		   == StatusCode::FAILURE ) return StatusCode::FAILURE;
	    }
	  }
	}
      }     // check window
   }        // end of loop over the cells


  ATH_MSG_DEBUG(" total number of hits found= " << m_nhit_tot);
  ATH_MSG_DEBUG(" number of created digits  = " << m_DigitContainer->size());

  return StatusCode::SUCCESS;

}

// ============================================================================================

StatusCode LArPileUpTool::fillMapFromHit(StoreGateSvc* myStore, float bunchTime, bool isSignal)
{
  for (const std::string& containerName : m_hitContainerNames) {

  //
  // ..... Get the pointer to the Hit Container from StoreGate
  //
    ATH_MSG_DEBUG(" fillMapFromHit: asking for: " << containerName);

    if (m_useLArHitFloat) {
     const LArHitFloatContainer* hit_container = nullptr;
     if (myStore->contains<LArHitFloatContainer>(containerName)) {
       StatusCode sc = myStore->retrieve( hit_container,containerName ) ;
       if (sc.isFailure() || !hit_container) {
          return StatusCode::FAILURE;
       }
       LArHitFloatContainer::const_iterator hititer;
       for(hititer=hit_container->begin();
           hititer != hit_container->end();hititer++)
       {
         m_nhit_tot++;
         Identifier cellId = (*hititer).cellID();
         float energy = (float) (*hititer).energy();
         float time;
         if (m_ignoreTime) time=0.;
         else time   = (float) ((*hititer).time() - m_trigtime);
         time = time + bunchTime;

         if (this->AddHit(cellId,energy,time,isSignal).isFailure()) return StatusCode::FAILURE;
       }
     }
     else {
      if (isSignal)  {
         ATH_MSG_WARNING(" LAr HitFloat container not found for signal event key " << containerName);
      }
     }

    }
    else {
     const LArHitContainer* hit_container = nullptr;
     if (myStore->contains<LArHitContainer>(containerName)) {
       StatusCode sc = myStore->retrieve( hit_container,containerName ) ;
       if (sc.isFailure() || !hit_container) {
          return StatusCode::FAILURE;
       }
       LArHitContainer::const_iterator hititer;
       for(hititer=hit_container->begin();
           hititer != hit_container->end();hititer++)
       {
         m_nhit_tot++;
         Identifier cellId = (*hititer)->cellID();
         float energy = (float) (*hititer)->energy();
         float time;
         if (m_ignoreTime) time=0.;
         else time   = (float) ((*hititer)->time() - m_trigtime);
         time = time + bunchTime;

         if (this->AddHit(cellId,energy,time,isSignal).isFailure()) return StatusCode::FAILURE;
       }
     }
     else {
      if (isSignal)  {
         ATH_MSG_WARNING(" LAr Hit container not found for signal event key " << containerName);
      }
     }
    }
  }   // end loop over containers

  return StatusCode::SUCCESS;
}

// ============================================================================================
StatusCode LArPileUpTool::fillMapFromHit(SubEventIterator iEvt, float bunchTime, bool isSignal)
{
  for (const std::string& containerName : m_hitContainerNames) {

  //
  // ..... Get the pointer to the Hit Container from StoreGate through the merge service
  //

    ATH_MSG_DEBUG(" fillMapFromHit: asking for: " << containerName);

    if (m_useLArHitFloat) {

      const LArHitFloatContainer * hit_container;

      if (!(m_mergeSvc->retrieveSingleSubEvtData(containerName, hit_container, bunchTime,
						 iEvt).isSuccess())){
	ATH_MSG_ERROR(" LAr Hit container not found for event key " << containerName);
	return StatusCode::FAILURE;
      }

      LArHitFloatContainer::const_iterator hititer;
      for(hititer=hit_container->begin();
	  hititer != hit_container->end();hititer++)
	{
	  m_nhit_tot++;
	  Identifier cellId = (*hititer).cellID();
	  float energy = (float) (*hititer).energy();
	  float time;
	  if (m_ignoreTime) time=0.;
	  else time   = (float) ((*hititer).time() - m_trigtime);
	  time = time + bunchTime;

         if (this->AddHit(cellId,energy,time,isSignal).isFailure()) return StatusCode::FAILURE;
	}
    }
    else {

      const LArHitContainer * hit_container;

      if (!(m_mergeSvc->retrieveSingleSubEvtData(containerName, hit_container, bunchTime,
                                             iEvt).isSuccess())){
	ATH_MSG_ERROR(" LAr Hit container not found for event key " << containerName);
	return StatusCode::FAILURE;
      }

      LArHitContainer::const_iterator hititer;
      for(hititer=hit_container->begin();
	  hititer != hit_container->end();hititer++)
	{
	  m_nhit_tot++;
	  Identifier cellId = (*hititer)->cellID();
	  float energy = (float) (*hititer)->energy();
	  float time;
	  if (m_ignoreTime) time=0.;
	  else time   = (float) ((*hititer)->time() - m_trigtime);
	  time = time + bunchTime;

         if (this->AddHit(cellId,energy,time,isSignal).isFailure()) return StatusCode::FAILURE;
	}
    }
  }   // end loop over containers

  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------------------------------------------------------

StatusCode LArPileUpTool::AddHit(const Identifier cellId, const float energy, const float time, const bool isSignal)
{

  // remove pathological energies...
  if (std::fabs(energy)>1e+9) {
      ATH_MSG_WARNING(" Pathological energy ignored Id= "<< m_larem_id->show_to_string(cellId) << "  energy= " << energy );
      return StatusCode::SUCCESS;
  }

#ifndef NDEBUG
  ATH_MSG_DEBUG(" Found hit  Id= "<< m_larem_id->show_to_string(cellId)<< "  energy= " << energy << "(MeV)  time= " << time << "(ns)");
#endif

  IdentifierHash idHash=m_calocell_id->calo_cell_hash(cellId);

// simulation of cross talk if requested (EM barrel + EndCap)
  if (m_CrossTalk && isSignal && m_calocell_id->is_em(cellId)) 
  {
       std::vector<IdentifierHash> neighbourList;
       std::vector<float> energyList;
       //bool dump=false;
       //if (energy>200. || m_larem_id->sampling(cellId)==3) dump=true;
       //if(dump) std::cout << " Input cell energy " << m_larem_id->show_to_string(cellId) << " " << energy  << std::endl;
       this->cross_talk(idHash,cellId,energy,  //FIXME -> Needs to work with full hash! 
                        neighbourList,energyList);
       //if(dump) std::cout <<" After cross-talk " ;
       for (unsigned int icell=0;icell<neighbourList.size();icell++)
       {
         //unsigned int index=neighbourList[icell];
	 //Turn sub-calo hash in neighbour list into gloabl calo-cell hash:
	 const IdentifierHash index=m_calocell_id->calo_cell_hash(m_calocell_id->sub_calo(cellId),neighbourList[icell]);
         float e = energyList[icell];
         //Identifier id2=m_larem_id->channel_id(neighbourList[icell]);
         //if(dump) std::cout << "Cell/E " <<  m_larem_id->show_to_string(id2) << " " << e << " ";
         if ( !m_hitmap->AddEnergy(index,e,time) )
         {
             ATH_MSG_ERROR("  Cell " << m_larem_id->show_to_string(cellId) << " could not add the energy= " << energy  << " (GeV)");
             return(StatusCode::FAILURE);
         }
         if ( m_doDigiTruth){ 
           if(!m_hitmap_DigiHSTruth->AddEnergy(index,e,time) ) {
             ATH_MSG_ERROR("  Cell " << m_larem_id->show_to_string(cellId) << " could not add the energy= " << energy  << " (GeV)");
             return(StatusCode::FAILURE);
           }
        }
       }
       //if (dump) std::cout << std::endl;
  }
  else    // no cross-talk simulated
  {
      if (isSignal || m_useMBTime)
      {
        if ( !m_hitmap->AddEnergy(idHash,energy,time) )
        {
          ATH_MSG_ERROR("  Cell " << m_larem_id->show_to_string(cellId) << " could not add the energy= " << energy  << " (GeV)");
          return(StatusCode::FAILURE);
         }
         if ( m_doDigiTruth){ 
          if(!m_hitmap_DigiHSTruth->AddEnergy(idHash,energy,time) ) {
             ATH_MSG_ERROR("  Cell " << m_larem_id->show_to_string(cellId) << " could not add the energy= " << energy  << " (GeV)");
             return(StatusCode::FAILURE);
          }
        }
      }
      else
      {
        if (idHash<m_energySum.size()) m_energySum[idHash] += energy;
      }
  }     // end if cross-talk
  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------------------------------------------------------------------

float  LArPileUpTool::get_strip_xtalk(int eta)
// eta = in strip cell units for Barrel EM
// definition:signal observed in strip ieta when strip ieta-1 is pulsed
{
 static const float fstrip[448] = {
0.000000,0.000000,4.941840,4.995340,5.068500,4.899490,5.202390,4.878160,
4.420290,4.725940,4.654810,4.844920,4.839140,4.675500,4.964490,4.815590,
3.468430,4.831320,4.820070,4.988120,4.978390,4.893490,5.071190,4.750850,
4.283550,4.661670,4.564020,4.756390,4.870670,4.606740,4.995110,4.801720,
3.346320,4.644580,4.656630,4.744210,4.620510,4.717910,5.037100,4.732250,
4.369770,4.726470,4.621690,4.808740,5.002980,4.857060,5.265280,5.066990,
3.387600,4.625930,4.631260,4.778710,4.703340,4.692220,5.028430,4.770270,
4.326720,4.759730,4.726690,4.990210,4.945640,4.936730,5.345750,5.043110,
3.080140,4.712660,4.737110,4.947980,4.955620,4.786610,5.059140,4.702760,
4.338590,4.602430,4.579650,4.686150,4.774360,4.577370,4.910050,4.768110,
3.442700,4.755480,4.735680,4.901400,4.955710,4.707240,5.054100,4.716160,
4.204750,4.643470,4.535960,4.694300,4.834650,4.566540,5.003450,4.846720,
3.329760,4.711250,4.681380,4.864680,4.618070,4.700280,4.912730,4.536010,
4.205330,4.547820,4.447080,4.719420,4.637420,4.664910,5.075560,4.932030,
3.313540,4.546530,4.555570,4.640850,4.562400,4.522530,4.815550,4.532470,
4.239320,4.603560,4.493110,4.843560,4.642890,4.767900,5.088340,4.878920,
2.940620,4.806970,4.799430,5.471210,5.493130,4.705590,5.003270,4.726930,
4.264920,4.549220,4.444930,4.536950,4.584510,4.397790,4.724790,4.497190,
3.257680,4.647240,4.639690,4.732290,4.679650,4.594370,4.768920,4.453610,
4.029560,4.350470,4.296220,4.484810,4.531020,4.329120,4.748570,4.599390,
3.165220,4.500810,4.499350,4.585880,4.568960,4.417530,4.745520,4.323080,
3.941150,4.368430,4.300980,4.424110,4.427210,4.340210,4.875340,4.639700,
3.126750,4.495110,4.456790,4.578960,4.347800,4.327210,4.611190,4.318600,
3.950290,4.283310,4.264750,4.411260,4.438900,4.518220,4.946140,4.706910,
2.669280,4.414860,4.480410,4.612140,4.694750,4.371030,4.678530,4.332440,
3.934310,4.290490,4.292760,4.418330,4.556070,4.535030,4.971880,4.776510,
3.141220,4.546930,4.564900,4.665540,4.716050,4.590120,4.793560,4.448830,
3.954200,4.331560,4.313700,4.461670,4.629640,4.509890,4.939050,4.840610,
3.194720,4.624200,4.655340,4.702320,4.679990,4.650580,4.915920,4.499970,
4.096780,4.465760,4.378710,4.582450,4.562690,4.635780,5.073300,4.890270,
3.305540,4.737480,4.774630,4.836870,4.790870,4.584430,4.907440,4.544700,
4.148710,4.506930,4.552770,4.692520,4.691690,4.867530,5.234160,4.519710,
0.231351,4.365310,3.911650,4.153410,4.970110,3.744390,4.071460,3.827700,
2.569980,4.027880,3.934860,3.903720,3.917390,3.796970,4.143480,3.852800,
2.578970,3.959390,3.909980,3.979270,3.889130,3.769090,4.148010,3.863630,
2.562030,3.989410,3.926630,4.131010,3.881370,3.788440,4.077710,3.909060,
2.527100,3.958940,3.866060,3.897480,3.800990,3.858710,4.230590,3.991950,
2.676870,3.824300,3.822470,3.912880,3.855500,3.833780,4.302050,4.069750,
2.636010,3.857680,3.848860,3.985250,3.890090,3.933520,4.381560,4.174220,
2.624630,3.912370,3.859100,4.071840,4.030760,4.089820,4.578140,4.282170,
2.251570,4.205960,4.200120,4.216630,4.202320,4.089250,4.410050,4.175700,
2.932060,4.282820,4.295150,4.328940,4.188230,4.119590,4.416830,4.218860,
2.895670,4.336220,4.244110,4.328980,4.227790,4.147420,4.500270,4.338000,
2.923310,4.353290,4.224150,4.320260,4.196040,4.187190,4.517070,4.416470,
2.914500,4.272400,4.204720,4.227430,4.227190,4.251880,4.635880,4.443870,
3.025330,4.236750,4.197720,4.266430,4.259500,4.270780,4.694650,4.520890,
2.913380,4.135570,4.140760,4.306100,4.311090,4.448770,5.410390,4.620640,
2.798590,4.267570,4.110760,4.303560,4.017360,4.182780,4.562770,4.429320,
2.414850,4.533350,4.387020,4.488800,4.420460,4.293000,4.623510,4.471620,
3.032110,4.449420,4.456320,4.477240,4.333340,4.220500,4.560130,4.366560,
2.873710,4.488090,4.355840,4.535650,4.196280,4.215340,4.478340,4.293760,
2.857740,4.445170,4.388450,4.525350,4.210190,4.143450,4.322170,4.301670,
2.790860,4.398790,4.340690,4.368470,4.103100,4.088190,4.385410,4.164060,
2.834180,4.331880,4.241480,4.314500,4.021430,3.995670,4.304520,4.156070,
2.658500,4.238980,4.058690,4.300010,3.874970,4.063210,4.309450,4.289980,
2.681620,4.237940,4.016890,4.202180,3.911600,4.077670,4.432300,4.500680
 };


 float result;
 if (eta >= 0 && eta < 448) {
  result = 0.01*fstrip[eta];
 }
  else
 {
  result=0;
 }
 return result;
}

// ----------------------------------------------------------------------------------------------------------------------------------

float LArPileUpTool::get_strip_xtalk_ec(int region,int eta)
// region,eta=region,eta for strips in End Cap outer wheel:
// region=0 to 5 with (1,3,96,48,64,4) strips => total = 216 strips
{
 static const float fstripec[216]={
0,0.647012,0.169466,-0.362632,0.731123,4.89171,5.28338,5.07526,
5.21291,5.025,5.34636,5.2136,3.8526,4.7453,4.86375,4.83032,
4.9479,4.8181,5.09672,4.84965,3.64455,4.64494,4.72441,4.70608,
4.81206,4.75978,4.9402,4.82231,3.60479,4.56231,4.67807,4.66655,
4.75519,4.68897,5.12308,5.00899,4.0,4.193,4.28249,4.3706,
4.32711,4.34628,4.51432,4.21356,3.03489,4.12521,4.21644,4.41794,
4.24279,4.29728,4.53824,4.22955,3.07725,4.04687,4.13052,4.23513,
4.21481,4.08073,4.50568,4.15085,2.95349,4.05713,4.05885,4.12274,
4.05337,4.07104,4.39499,4.08766,2.87246,3.99885,3.91702,4.03367,
4.15477,3.96968,4.36601,4.11122,2.82138,3.93271,3.96892,4.01186,
4.04095,3.9507,4.21476,3.99467,2.71107,3.87781,3.97037,3.91075,
3.9159,3.8519,4.29611,3.9003,2.67276,3.81504,3.93936,3.92909,
3.81958,3.88976,4.2133,3.80019,3.17429,3.73371,3.80235,3.9614,
4.02772,3.95582,4.25171,3.94301,3.54611,3.77185,3.76848,3.86328,
2.99154,3.72234,4.09029,3.82296,3.4633,3.80757,3.88435,3.96779,
3.75777,3.81033,4.20651,3.85612,2.65187,3.59496,3.63019,3.73253,
3.83799,3.79509,4.1756,3.83532,3.55439,3.83061,3.84964,4.01281,
2.78266,3.77474,4.03185,3.70673,3.39524,3.81235,3.8061,3.94906,
3.9598,3.82479,4.16271,3.88879,3.04199,3.46445,3.53472,3.63066,
3.64371,3.43247,3.84775,3.5936,2.22315,3.39068,3.53677,3.73777,
3.52771,3.44231,3.86722,3.4239,2.25642,3.43095,3.44673,3.60134,
3.56666,3.40409,3.9638,3.71488,2.14987,3.44628,3.54865,3.81282,
3.62762,3.50164,3.87881,3.64674,2.16211,3.34645,2.59739,3.26494,
2.35902,3.44475,3.59458,3.641,3.1766,3.21545,3.45884,3.49696,
2.26263,3.32139,3.57328,3.54561,2.5631,2.74267,3.10053,2.80255,
1.60569,2.79466,3.13383,2.95736,2.52495,2.61651,2.97129,2.65964,
1.70944,2.61858,3.02026,3.2007,3.0,2.4479,2.99424,2.72339
 };
 int eta2=-1;
 if (region==0) eta2=eta;
 if (region==1) eta2=eta+1;
 if (region==2) eta2=eta+4;
 if (region==3) eta2=eta+100;
 if (region==4) eta2=eta+148;
 if (region==5) eta2=eta+212;
 float result;
 if (eta2>=0 && eta2 < 216) {
  result=0.01*fstripec[eta2];
 }
 else
 {
  result =0.;
 }
 return result;

}

// ----------------------------------------------------------------------------------------------------------------------------------

float  LArPileUpTool::get_middleback_xtalk(int eta)
// eta=in middle cell units
// signal observed in back cell a ieta when one corresponding middle cell is pulsed
{
 static const float fback[56] = {
0.00581541, 0.00373308, 0.00473475, 0.00375685, 0.00468253, 0.00314712,
0.00346465, 0.00471375, 0.00491536, 0.00322414, 0.00341004, 0.00463626,
0.00460005, 0.00304867, 0.0032422, 0.00438614, 0.00526148, 0.00332974,
0.00391853, 0.00523458, 0.00549952, 0.00368928, 0.00417336, 0.00554728,
0.00555951, 0.00353167, 0.0042602, 0.0056228, 0.00600353, 0.00381754,
0.00489258, 0.00553622, 0.00491844, 0.00298871, 0.00336651, 0.00471029,
0.00546964, 0.00369092, 0.00421188, 0.00450773, 0.00694431, 0.00471822,
0.0054511, 0.00739328, 0.00757383, 0.00570223, 0.00582577, 0.00795886,
0.0070187, 0.0081468, 0.00854689, 0.00756213, 0.00660363, 0.00429604,
0, 0};

 float result;
 if (eta >=0 && eta <56) {
  result = fback[eta];
 }
 else
 {
 result=0.;
 }
 return result;
}

// ----------------------------------------------------------------------------------------------------------------------------------

float LArPileUpTool::get_middleback_xtalk_ecow(int eta)
// eta=in middle cell units (from 0 to 42 for eta between 1.425 and 2.5), for the first 3 cells (1.425-1.5) there is no back layer
{
 static const float fbackecow[43] = {
0., 0., 0., 0.00652081, 0.0046481, 0.0052409,
0.00399028, 0.00424165, 0.00312753, 0.00342063, 0.00391649, 0.00397975,
0.00332866, 0.00350436, 0.00408691, 0.00410245, 0.00328291, 0.00360715,
0.00367647, 0.00380246, 0.00350512, 0.00355757, 0.0036714, 0.00275938,
0.00277525, 0.00319203, 0.00309221, 0.00324716, 0.00315221, 0.00297258,
0.00309299, 0.00334898, 0.00296156, 0.00347265, 0.00302631, 0.00323425,
0.00304208, 0.00349578, 0.00325548, 0.00195648, 0.000967903, 0.00228932,
-0.000110377
 };
 float result;
 if (eta >=0 && eta < 43) {
  result = fbackecow[eta];
 }
 else
 {
  result=0.;
 }
 return result;
}

// ----------------------------------------------------------------------------------------------------------------------------------

float LArPileUpTool::get_middleback_xtalk_eciw(int eta)
// eta=in back cell units (from 0 to 6 for eta between 2.5 and 3.2)
//  (same size for the two samplings in depth)
{
 static const float fbackeciw[7] = {
 0.400584,0.332231,0.348472,0.222996,0.402843,0.362233,0.448652
 };
 float result;
 if (eta>=0 && eta < 7) {
  result = 0.01*fbackeciw[eta];
 }
 else
 {
  result=0.;
 }
 return result;
}

// ----------------------------------------------------------------------------------------------------------------------------------

float LArPileUpTool::get_stripmiddle_xtalk(int eta)
// eta = ieta of strip in barrel
// signal observed in strip ieta when corresponding middle cell is puilsed
{
 static const float fstripmiddle[448]= {
0.000000,0.057593,0.066206,0.065568,0.074090,0.071852,0.067529,0.056533,
0.063181,0.070852,0.069660,0.068299,0.073207,0.070664,0.068899,0.058289,
0.062070,0.069117,0.067532,0.065357,0.072678,0.069367,0.066721,0.056106,
0.063151,0.070274,0.069376,0.067289,0.072044,0.069219,0.068826,0.058357,
0.061702,0.068027,0.066000,0.064036,0.072736,0.069575,0.066915,0.056115,
0.063321,0.069319,0.069737,0.067376,0.073263,0.069775,0.068742,0.058619,
0.061698,0.068377,0.066455,0.064298,0.072952,0.069158,0.065425,0.056288,
0.064203,0.070191,0.069912,0.068650,0.073171,0.070858,0.068896,0.059596,
0.062067,0.072063,0.069730,0.068473,0.073368,0.072757,0.068430,0.059692,
0.061603,0.071009,0.071016,0.069952,0.072357,0.070800,0.069715,0.060913,
0.063531,0.071730,0.070056,0.068970,0.074330,0.071989,0.069472,0.059886,
0.063687,0.071704,0.072171,0.071236,0.073437,0.071373,0.071246,0.062733,
0.064949,0.072393,0.071508,0.069492,0.074456,0.073138,0.069920,0.061572,
0.063575,0.071779,0.070597,0.069672,0.073149,0.070695,0.071248,0.062548,
0.064342,0.071599,0.070892,0.068660,0.074601,0.071896,0.069740,0.060747,
0.064433,0.071032,0.070988,0.070382,0.074561,0.071609,0.070358,0.063174,
0.060402,0.069860,0.067935,0.067482,0.072190,0.070435,0.065446,0.059008,
0.060452,0.069471,0.068584,0.068664,0.070999,0.069964,0.068538,0.060262,
0.061247,0.069745,0.067499,0.066873,0.072509,0.070726,0.067629,0.059162,
0.062625,0.071129,0.071199,0.070301,0.072662,0.070973,0.070674,0.062273,
0.064056,0.071638,0.069509,0.068664,0.074220,0.073025,0.069910,0.061364,
0.063989,0.072063,0.072199,0.070516,0.074376,0.072716,0.071088,0.063537,
0.063696,0.072608,0.070414,0.068859,0.074111,0.074098,0.069647,0.061648,
0.065144,0.074563,0.073813,0.073579,0.075916,0.075318,0.074495,0.065329,
0.065056,0.075442,0.075504,0.074006,0.077679,0.077138,0.074631,0.067735,
0.066079,0.075890,0.074706,0.074512,0.077435,0.075215,0.075741,0.068456,
0.068687,0.078836,0.077000,0.077254,0.081442,0.080019,0.078276,0.070143,
0.069991,0.079240,0.080317,0.080183,0.082367,0.080627,0.079765,0.073177,
0.072121,0.082907,0.081502,0.081124,0.085603,0.084507,0.081715,0.075112,
0.072514,0.082970,0.083017,0.082940,0.085169,0.084747,0.083225,0.075169,
0.073025,0.084558,0.082168,0.083268,0.086358,0.086288,0.083046,0.074887,
0.077361,0.089017,0.089347,0.088715,0.091416,0.087911,0.058186,0.054394,
0.041336,0.052718,0.073998,0.075921,0.077883,0.077766,0.077420,0.071660,
0.064533,0.074894,0.075092,0.075635,0.076608,0.075771,0.076752,0.070370,
0.066999,0.078883,0.078791,0.079434,0.080629,0.079828,0.079841,0.073674,
0.068958,0.079916,0.079954,0.080276,0.080855,0.080957,0.080605,0.074600,
0.070171,0.081625,0.081963,0.082812,0.083441,0.083164,0.083321,0.077478,
0.071194,0.082135,0.083004,0.083437,0.083920,0.083872,0.083731,0.077397,
0.071816,0.083838,0.084357,0.084403,0.085843,0.085616,0.085412,0.078550,
0.073485,0.085697,0.085906,0.086108,0.086535,0.085842,0.085598,0.079094,
0.075442,0.087865,0.088416,0.088902,0.089865,0.089244,0.090911,0.083901,
0.074853,0.087526,0.088328,0.089100,0.089351,0.088918,0.089116,0.082991,
0.075936,0.088739,0.089550,0.090286,0.091007,0.090810,0.091770,0.085127,
0.079755,0.094275,0.094572,0.095602,0.096079,0.095100,0.096046,0.089397,
0.079121,0.092840,0.093718,0.094796,0.095346,0.094070,0.095548,0.088966,
0.080869,0.093817,0.093813,0.094287,0.094524,0.094668,0.095494,0.087706,
0.082012,0.095262,0.095519,0.096840,0.095924,0.096433,0.095911,0.089911,
0.082909,0.096672,0.095899,0.097223,0.097262,0.096446,0.097197,0.091773,
0.087488,0.101989,0.102731,0.104303,0.105120,0.103843,0.103617,0.096640,
0.089244,0.103986,0.104969,0.104566,0.105460,0.104711,0.105673,0.099122,
0.091722,0.106743,0.108065,0.110441,0.109489,0.108598,0.109482,0.101707,
0.094108,0.107301,0.109594,0.110375,0.109384,0.108924,0.109918,0.103579,
0.095175,0.109468,0.110099,0.111190,0.110922,0.110931,0.110588,0.103792,
0.096311,0.111721,0.111981,0.113994,0.114025,0.113152,0.114576,0.108146,
0.106523,0.122756,0.123525,0.124886,0.125457,0.125061,0.123872,0.117687,
0.112486,0.132309,0.132745,0.134622,0.135338,0.135407,0.135259,0.130872
 };
 float result;
 if (eta>=0 && eta<448) {
   result = 0.01*fstripmiddle[eta];
 }
 else
 {
   result=0.;
 }
 return result;
}

// ----------------------------------------------------------------------------------------------------------------------------------

float LArPileUpTool::get_stripmiddle_xtalk_ec(int region,int eta)
// eta = ieta of strip in endcap (outer wheel)
{
  static const float fstripmiddleow[216]= {
-2.50602,-0.476067,-0.139486,-0.128851,0.0390557,0.0493432,0.0527529,0.0531567,
0.0578755,0.0578588,0.0590179,0.0476273,0.0504438,0.0556113,0.0567476,0.0569607,
0.0581592,0.055223,0.0550925,0.045735,0.0505001,0.0567269,0.0572249,0.0562184,
0.0567264,0.055431,0.0555519,0.0455135,0.0476087,0.0538574,0.0541163,0.0536874,
0.0530759,0.0512971,0.0505691,0.0404547,0.0577378,0.0710235,0.0669423,0.0697705,
0.0698465,0.0700433,0.0663293,0.0583974,0.0606457,0.0703779,0.0659176,0.0673009,
0.0674643,0.0665941,0.0633545,0.0551358,0.0604582,0.0692449,0.0641828,0.0667649,
0.0658649,0.0677935,0.0618821,0.0546772,0.0588805,0.0664603,0.0637221,0.0648594,
0.0645644,0.0663554,0.0623134,0.0538591,0.0558902,0.0656736,0.0617502,0.0645883,
0.0639567,0.065985,0.0612916,0.0531633,0.0538625,0.0616824,0.0579105,0.0600404,
0.0595016,0.0606717,0.0570288,0.0492493,0.0511538,0.062043,0.0548816,0.0581663,
0.0576214,0.0605366,0.0510894,0.0466091,0.0501537,0.0587429,0.0546108,0.0574415,
0.0566192,0.0573286,0.0514807,0.045724,0.0690077,0.0837654,0.0739633,0.0784337,
0.0744955,0.0704726,0.0626987,0.076321,0.0724027,0.0767479,0.0683885,0.0648873,
0.0634866,0.078299,0.0660834,0.072008,0.068058,0.0643917,0.0588662,0.0727035,
0.0687075,0.0734932,0.0649575,0.0606128,0.0565887,0.0718651,0.0605041,0.066728,
0.063079,0.0607112,0.0527683,0.0668151,0.06269,0.0680627,0.0586427,0.0557382,
0.053216,0.0681206,0.0555227,0.0616511,0.0574567,0.0555733,0.0493507,0.0639906,
0.0580138,0.0638682,0.0535373,0.0501332,0.080116,0.0979333,0.08077,0.0800679,
0.07985,0.0956901,0.0832461,0.0819788,0.0808887,0.0975798,0.0835345,0.078607,
0.0784665,0.0926911,0.0792357,0.0781658,0.0717282,0.0888529,0.0718227,0.0717151,
0.0700012,0.0860595,0.0730921,0.0712477,0.0656817,0.0831567,0.0647184,0.066461,
0.0638998,0.0803663,0.0651302,0.0651395,0.0507913,0.072851,0.0510739,0.0567894,
0.055003,0.0749684,0.059737,0.0609512,0.0518167,0.0713727,0.0511887,0.0539079,
0.0523586,0.0695313,0.0530894,0.0581158,0.0485283,0.0649812,0.0433144,0.0489274,
0.0464547,0.0632957,0.046902,0.0512296,0.0432063,0.0607857,0.0385948,0.0443324,
0.0400325,0.0582119,0.0429047,0.0804729,0.0295451,0.0437979,0.0240229,0.0398702
 };
 int eta2=-1;
 if (region==0) eta2=eta;
 if (region==1) eta2=eta+1;
 if (region==2) eta2=eta+4;
 if (region==3) eta2=eta+100;
 if (region==4) eta2=eta+148;
 if (region==5) eta2=eta+212;
 float result;
 if (eta2>=0 && eta2<216) {
   result = 0.01* fstripmiddleow[eta2];
 }
 else {
   result = 0.;
 }
 return result;
}

// ----------------------------------------------------------------------------------------------------------------------------------

// 2nd neighbor cross-talk in barrel
float LArPileUpTool::get_2strip_xtalk(int eta) {
   if (eta<380) return 0.004;
   else         return 0.005;
}

// ----------------------------------------------------------------------------------------------------------------------------------

// 2nd neighbor cross-talk in end-cap
float LArPileUpTool::get_2strip_xtalk_ec(int region, int eta) {
 int eta2=-1;
 if (region==0) eta2=eta;
 if (region==1) eta2=eta+1;
 if (region==2) eta2=eta+4;
 if (region==3) eta2=eta+100;
 if (region==4) eta2=eta+148;
 if (region==5) eta2=eta+212;
 if (eta2<40) return 0.004;
 else if (eta2<150) return 0.0025;
 else return 0.004;

}

// ----------------------------------------------------------------------------------------------------------------------------------

// middle to middle cross-talk in barrel

float LArPileUpTool::get_middle_xtalk1(int ieta) {
 static const float middle_barrel_xtalk1[56]={
 0.000000,0.005352,0.006647,0.004541,0.005774,0.004467,
 0.005626,0.004118,0.005527,0.004558,0.005754,0.004386,
 0.005496,0.004072,0.005483,0.003973,0.006305,0.005371,
 0.005479,0.003798,0.005788,0.003905,0.005495,0.003158,
 0.005689,0.003876,0.006026,0.002955,0.005911,0.002950,
 0.005837,0.002410,-0.000550,0.007546,0.007911,0.006502,
 0.008427,0.006633,0.008937,0.006282,0.008913,0.006508,
 0.009016,0.005964,0.009230,0.006104,0.009746,0.006198,
 0.010103,0.006973,0.011460,0.006248,0.009688,0.007183,
 0.010024,0.010212};

  if (ieta<0 ||ieta>55) return 0;
  return middle_barrel_xtalk1[ieta];
}

float LArPileUpTool::get_middle_xtalk2(int ieta) {
 static const float middle_barrel_xtalk2[56]={
 0.004981,0.006938,0.004264,0.005237,0.004481,0.005738,
 0.004026,0.005395,0.004610,0.005865,0.004134,0.004966,
 0.004182,0.005500,0.004925,0.006109,0.004272,0.006086,
 0.003637,0.005229,0.003620,0.005745,0.003284,0.005644,
 0.003574,0.006029,0.003210,0.005395,0.002710,0.005967,
 0.003019,-0.001277,0.007491,0.008206,0.006319,0.007886,
 0.006738,0.009192,0.006128,0.008657,0.006431,0.009153,
 0.005842,0.009034,0.006101,0.009814,0.006949,0.009089,
 0.006710,0.012355,0.006297,0.009299,0.006999,0.010666,
 0.009492,0.000000};

  if (ieta<0 ||ieta>55) return 0;
  return middle_barrel_xtalk2[ieta];
}

// ----------------------------------------------------------------------------------------------------------------------------------

// middle to middle cross-talk in end-cap

float LArPileUpTool::get_middle_xtalk1_ec(int ieta) {
  static const float middle_endcap_xtalk1[43]={
  0.000000,-0.002289,0.000726,0.007255,0.002213,0.004363,
  0.002208,0.005293,0.002690,0.002920,0.002130,0.003346,
  0.000657,0.002562,0.000152,0.002327,0.002135,0.002988,
  0.001780,0.003334,0.001334,0.003300,0.000626,0.003578,
  0.001449,0.002353,0.000943,0.002904,0.000625,0.002306,
  -0.000139,0.002406,0.000535,0.001682,-0.000085,0.001295,
  -0.000604,0.002379,-0.001201,0.001004,-0.001378,-0.000014,
  -0.003236};

  if (ieta<0 ||ieta>42) return 0;
  return middle_endcap_xtalk1[ieta];
}

float LArPileUpTool::get_middle_xtalk2_ec(int ieta) {
  static const float middle_endcap_xtalk2[43]={
  -0.001290,0.001778,0.001121,0.002102,0.004127,0.002443,
  0.003744,0.001327,0.003492,0.001388,0.002851,0.001007,
  0.002180,0.001223,0.004599,0.001143,0.003484,0.001263,
  0.003391,0.001650,0.002738,0.002008,0.003503,0.000397,
  0.003168,0.000686,0.002884,0.000583,0.002037,0.000566,
  0.002362,-0.000312,0.002437,-0.000944,0.001629,0.000135,
  0.001934,-0.000824,0.001673,-0.002031,-0.001384,-0.002844,
  0.000000};

  if (ieta<0 ||ieta>42) return 0;
  return middle_endcap_xtalk2[ieta];
}



// -------------------------------------------------------------------------------------------------------------------------------------

void LArPileUpTool::cross_talk(const IdentifierHash& hashId,
                                const Identifier& cellId,
                                const float& energy,
                                std::vector<IdentifierHash>& neighbourList,
                                std::vector<float>& energyList)
{
  neighbourList.clear();
  energyList.clear();
  int result=0;
  neighbourList.reserve(8);
  energyList.reserve(8);

  int ibec     = abs(m_larem_id->barrel_ec(cellId));   // 1 barrel 2 EC OW  3 EC IW
  int sampling = m_larem_id->sampling(cellId);
  int eta      = m_larem_id->eta(cellId);
  int region   = m_larem_id->region(cellId);
  float fcr=0,e,er;
  float fcr2=0.;
  std::vector<IdentifierHash> tmpList;

  er=energy;    // total energy of hit to be spread among channels

// cross-talk in strips
  if ((ibec==1 && sampling == 1 && region == 0)
      || (ibec==2 && sampling ==1) )
  {

     if (ibec==1) fcr2 = this->get_2strip_xtalk(eta);
     if (ibec==2) fcr2 = this->get_2strip_xtalk_ec(region,eta);

// next in eta
     if ( (ibec==1 && eta !=447) || (ibec==2 && (eta!=3 || region !=5)) )
     {
       result=m_larem_id->get_neighbours(hashId,
                  LArNeighbours::nextInEta,tmpList);
       if(ibec==1) fcr =  this->get_strip_xtalk(eta+1)*m_scaleStripXtalk;
       if(ibec==2) fcr =  this->get_strip_xtalk_ec(region,eta+1)*m_scaleStripXtalk;

       if (result==0) {
         if (tmpList.size() == 1) {
            e=energy*fcr;
            er=er-e;
            neighbourList.push_back(tmpList[0]);
            energyList.push_back(e);

// second neighbor cross-talk
            if ((   (ibec==1 && eta !=446)
                  ||(ibec==2 && (eta!=2 || region !=5)) )  && m_CrossTalk2Strip) {
                std::vector<IdentifierHash> tmpList2;
                result = m_larem_id->get_neighbours(tmpList[0],LArNeighbours::nextInEta,tmpList2);
                if (result==0) {
                   if (tmpList2.size()==1) {
                     e=energy*fcr2;
                     er=er-e;
                     neighbourList.push_back(tmpList2[0]);
                     energyList.push_back(e);
                   }
                }
            }

         }
       }
     }
// prev in eta (if possible)
     if ( (ibec==1 && eta >1) || (ibec==2 && (eta !=0 || region !=0)) )
     {
       result=m_larem_id->get_neighbours(hashId,
                  LArNeighbours::prevInEta,tmpList);
       if(ibec==1) fcr =  this->get_strip_xtalk(eta)*m_scaleStripXtalk;
       if(ibec==2) fcr =  this->get_strip_xtalk_ec(region,eta)*m_scaleStripXtalk;
       if (result==0 ) {
         if (tmpList.size() == 1) {
            e=energy*fcr;
            er=er-e;
            neighbourList.push_back(tmpList[0]);
            energyList.push_back(e);

// second neighbor cross-talk
            if ((   (ibec==1 && eta !=2)
                  ||(ibec==2 && (eta!=1 || region !=0)) ) && m_CrossTalk2Strip) {
                std::vector<IdentifierHash> tmpList2;
                result = m_larem_id->get_neighbours(tmpList[0],LArNeighbours::prevInEta,tmpList2);
                if (result==0) {
                   if (tmpList2.size()==1) {
                     e=energy*fcr2;
                     er=er-e;
                     neighbourList.push_back(tmpList2[0]);
                     energyList.push_back(e);
                   }
                }
            }

         }
       }
     }
  }

// cross-talk strip to middle
  if (m_CrossTalkStripMiddle) {
    if ((ibec==1 && sampling==1 && region==0)
       || (ibec==2 && sampling==1) )
    {
      if (ibec==1) fcr = this->get_stripmiddle_xtalk(eta)*m_scaleStripMiddle;
      if (ibec==2) fcr = this->get_stripmiddle_xtalk_ec(region,eta)*m_scaleStripMiddle;

      if (ibec==1) fcr = fcr*2.;  // 8 strips for 4 middle cells
      if (ibec==2) {
         if (region==0) fcr=fcr/4.;   // 1 strip for 4 middle cells
         if (region==1) fcr=fcr/4.;   // 1 strip for 4 middle cells
         if (region==2) fcr=fcr*2.;   // 8 strips for 4 middle cells
         if (region==3) fcr=fcr*1.5;  // 6 strips for 4 middle cells
                                      // (region 4:  4strips for 4 middle cells)
         if (region==5) fcr=fcr/4.;   // 1 strip for 4 middle cells
      }

      // next sampling  (should have only one cell)
      result = m_larem_id->get_neighbours(hashId, LArNeighbours::nextInSamp,tmpList);
      if (result==0) {
        e=energy*fcr;
        for (unsigned int ii=0;ii<tmpList.size();ii++) {
          er = er-e;
          neighbourList.push_back(tmpList[ii]);
          energyList.push_back(e);
        }
      }
    }

// cross-talk middle to strip
    if ((ibec==1 && sampling==2 && region==0)
        || (ibec==2 && sampling==2) )
    {
      // previous sampling, expect 8 channels in middle for barrel, varing number in end-cap
      result = m_larem_id->get_neighbours(hashId,
       LArNeighbours::prevInSamp,tmpList);
      if (result==0) {
        for (unsigned int ii=0;ii<tmpList.size();ii++) {
          Identifier stripId = m_larem_id->channel_id(tmpList[ii]);
          if (m_larem_id->sampling(stripId)==1) {
            neighbourList.push_back(tmpList[ii]);
            int eta2 = m_larem_id->eta(stripId);
            int region2 = m_larem_id->region(stripId);
            if (ibec==1) fcr=this->get_stripmiddle_xtalk(eta2)*m_scaleStripMiddle;
            if (ibec==2) fcr=this->get_stripmiddle_xtalk_ec(region2,eta2)*m_scaleStripMiddle;
            e=energy*fcr;
            er=er-e;
            energyList.push_back(e);
          }
        }
      }
    }
  }  // strip middle crosstalk

// cross-talk middle to middle
  if (m_CrossTalkMiddle) {
    if ((ibec==1 && sampling==2 && region==0 ) ||
        (ibec==2 && sampling==2 && region==1)) {

     // fmiddle1 crosstalk to eta-1
     // fmiddle2 crosstalk to eta+1
     float fmiddle1=0.;
     float fmiddle2=0.;
     if (ibec==1) {
        fmiddle1 = this->get_middle_xtalk1(eta)*m_scaleMiddleXtalk;
        fmiddle2 = this->get_middle_xtalk2(eta)*m_scaleMiddleXtalk;
     }
     if (ibec==2) {
         fmiddle1 = this->get_middle_xtalk1_ec(eta)*m_scaleMiddleXtalk;
         fmiddle2 = this->get_middle_xtalk2_ec(eta)*m_scaleMiddleXtalk;
     }

     // next in eta
     if ( (ibec==1 && eta<55) || (ibec==2 && eta <42) ) {
       result=m_larem_id->get_neighbours(hashId,
                  LArNeighbours::nextInEta,tmpList);
       if (result==0) {
         if (tmpList.size() == 1) {
            e=energy*fmiddle2;
            er=er-e;
            neighbourList.push_back(tmpList[0]);
            energyList.push_back(e);
          }
       }
     }
     // previous in eta
     if ( (ibec==1 && eta>0) || (ibec==2 && eta >0) ) {
       result=m_larem_id->get_neighbours(hashId,
                  LArNeighbours::prevInEta,tmpList);
       if (result==0) {
         if (tmpList.size() == 1) {
            e=energy*fmiddle1;
            er=er-e;
            neighbourList.push_back(tmpList[0]);
            energyList.push_back(e);
          }
       }
     }
    }
  }

// cross-talk in middle to back
  if ( (ibec==1 && sampling ==2 && region == 0 )
      || (ibec==2 && sampling ==2 && region ==1 && eta > 2)  // no sampling 3 before 1.5
      || (ibec==3 && sampling ==1) )                         // inner wheel
  {
     if (ibec==1) {
      fcr = this->get_middleback_xtalk(eta);
     } else if(ibec==2) {
      fcr = this->get_middleback_xtalk_ecow(eta);
     } else if(ibec==3) {
      fcr = this->get_middleback_xtalk_eciw(eta);  // same size of middle and back in IW
     }
// next sampling
     result=m_larem_id->get_neighbours(hashId,
      LArNeighbours::nextInSamp,tmpList);
     if (result==0) {
        if (tmpList.size() == 1) {
            e=energy*fcr;
            er=er-e;
            neighbourList.push_back(tmpList[0]);
            energyList.push_back(e);
        }
     }
  }

// cross-talk back to middle
  if ( (ibec==1 && sampling == 3 && region == 0 )
     ||(ibec==2 && sampling ==3)
     ||(ibec==3 && sampling ==2) )
  {
    if (ibec==1) {
     // eta2 = eta for middle layer cells
     int eta2=2*eta;
     fcr=0.5*(this->get_middleback_xtalk(eta2)+this->get_middleback_xtalk(eta2+1));
    } else if(ibec==2) {
     int eta2=3+2*eta;
     fcr=0.5*(this->get_middleback_xtalk_ecow(eta)+this->get_middleback_xtalk(eta2+1));
    } else if(ibec==3) {
     fcr=this->get_middleback_xtalk_eciw(eta);
    }
// previous sampling, expect two channels in middle for barrel + OW, one for IW
    result = m_larem_id->get_neighbours(hashId,
       LArNeighbours::prevInSamp,tmpList);
    if (result==0) {
       if ((ibec==1 || ibec==2) && tmpList.size() == 2) {
          fcr=fcr/2.;
          e=energy*fcr;
          er=er-2.*e;
          neighbourList.push_back(tmpList[0]);
          neighbourList.push_back(tmpList[1]);
          energyList.push_back(e);
          energyList.push_back(e);
       }
       if (ibec==1 && tmpList.size()==1) {
          e=energy*fcr;
          er=er-e;
          neighbourList.push_back(tmpList[0]);
          energyList.push_back(e);
       }
       if (ibec==3 && tmpList.size() ==1) {
          e=energy*fcr;
          er=er-e;
          neighbourList.push_back(tmpList[0]);
          energyList.push_back(e);
       }
    }
  }

// remaining energy in original cell
  neighbourList.push_back(hashId);
  energyList.push_back(er);

}

// ----------------------------------------------------------------------------------------------------------------------------------


StatusCode LArPileUpTool::MakeDigit(const Identifier & cellId,
                                    HWIdentifier & ch_id,
                                    const std::vector<std::pair<float,float> >* TimeE,
                                    const LArDigit * rndmEvtDigit, CLHEP::HepRandomEngine * engine,
				    const std::vector<std::pair<float,float> >* TimeE_DigiHSTruth)

{
  bool createDigit_DigiHSTruth = true;

  int i;
  short Adc;
  short Adc_DigiHSTruth;

  CaloGain::CaloGain igain;
  std::vector<short> AdcSample(m_NSamples);
  std::vector<short> AdcSample_DigiHSTruth(m_NSamples);

  float MeV2GeV=0.001;   // to convert hit from MeV to GeV before apply GeV->ADC

  float SF=1.;
  float SigmaNoise;
  std::vector<float> rndm_energy_samples(m_NSamples) ;


  SG::ReadCondHandle<LArADC2MeV> adc2mevHdl(m_adc2mevKey);
  const LArADC2MeV* adc2MeVs=*adc2mevHdl;

  SG::ReadCondHandle<ILArfSampl> fSamplHdl(m_fSamplKey);
  const ILArfSampl* fSampl=*fSamplHdl;

  SG::ReadCondHandle<ILArPedestal> pedHdl(m_pedestalKey);
  const ILArPedestal* pedestal=*pedHdl;

  const ILArNoise* noise=nullptr;  
  if ( !m_RndmEvtOverlay && !m_pedestalNoise && m_NoiseOnOff ){
    SG::ReadCondHandle<ILArNoise> noiseHdl(m_noiseKey);
    noise=*noiseHdl;
  }

  const LArAutoCorrNoise* autoCorrNoise=nullptr;
  if ( !m_RndmEvtOverlay  &&  m_NoiseOnOff) {
    SG::ReadCondHandle<LArAutoCorrNoise>  autoCorrNoiseHdl(m_autoCorrNoiseKey);
    autoCorrNoise=*autoCorrNoiseHdl;
  }


  LArDigit *Digit;
  LArDigit *Digit_DigiHSTruth;



  int iCalo=0;
  if(m_larem_id->is_lar_em(cellId)) {
     if (m_larem_id->is_em_endcap_inner(cellId)) iCalo=EMIW;
     else iCalo=EM;
  }
  if(m_larem_id->is_lar_hec(cellId))  iCalo=HEC;
  if(m_larem_id->is_lar_fcal(cellId)) iCalo=FCAL;

  CaloGain::CaloGain  initialGain = CaloGain::LARHIGHGAIN;
  if (iCalo==HEC) initialGain = CaloGain::LARMEDIUMGAIN;

  CaloGain::CaloGain rndmGain = CaloGain::LARHIGHGAIN;

// ........ retrieve data (1/2) ................................
//
  SF=fSampl->FSAMPL(ch_id);

//
// ....... dump info ................................
//


#ifndef NDEBUG
  ATH_MSG_DEBUG("    Cellid " << m_larem_id->show_to_string(cellId));
  ATH_MSG_DEBUG("    SF: " << SF);
#endif

//
// ....... make the five samples
//
  for (i=0;i<m_NSamples;i++) {
   m_Samples[i]=0.;
   if(m_doDigiTruth) {
     m_Samples_DigiHSTruth[i]=0.;
   }

  }

#ifndef NDEBUG
  ATH_MSG_DEBUG(" number of hit for this cell " << TimeE->size());
#endif

//
// convert Hits into energy samples and add result to m_Samples assuming LARHIGHGAIN for pulse shape
//
  bool isDead = false;
  if (m_useBad) isDead = m_maskingTool->cellShouldBeMasked(ch_id);

  if (!isDead) {
    if( this->ConvertHits2Samples(cellId,ch_id,initialGain,TimeE, m_Samples).isFailure() ) return StatusCode::SUCCESS;
    if(m_doDigiTruth){
      if( this->ConvertHits2Samples(cellId,ch_id,initialGain,TimeE_DigiHSTruth, m_Samples_DigiHSTruth).isFailure() ) return StatusCode::SUCCESS;
    }
  }

//
// .... add random event digit if needed
//
 float energy2adc ;
 float rAdc ;
 if(m_RndmEvtOverlay && rndmEvtDigit ) // no overlay if missing random digit
 {
  rndmGain= rndmEvtDigit->gain();
  auto polynom_adc2mev =adc2MeVs->ADC2MEV(cellId,rndmEvtDigit->gain());
  if (polynom_adc2mev.size() > 1) {
     float adc2energy = SF * polynom_adc2mev[1];
     const std::vector<short> & rndm_digit_samples = rndmEvtDigit->samples() ;
     float Pedestal = pedestal->pedestal(ch_id,rndmEvtDigit->gain());
     if (Pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
       ATH_MSG_WARNING("  Pedestal not found in database for this channel offID " << cellId << " Use sample 0 for random");
       Pedestal = rndm_digit_samples[0];
     }
     ATH_MSG_DEBUG(" Params for inverting LAr Digitization: pedestal " << Pedestal << "  adc2energy " << adc2energy);

// in case Medium or low gain, take into account ramp intercept in ADC->"energy" computation
//   this requires to take into account the sum of the optimal filter coefficients, as they don't compute with ADC shift
     float adc0=0.;
     if (!m_isMcOverlay  && m_larOFC && rndmEvtDigit->gain()>0) {
        ILArOFC::OFCRef_t ofc_a = m_larOFC->OFC_a(ch_id,rndmEvtDigit->gain(),0);
        float sumOfc=0.;
        if (ofc_a.size()>0) {
          for (unsigned int j=0;j<ofc_a.size();j++) sumOfc += ofc_a.at(j);
        }
        if (sumOfc>0) adc0 =  polynom_adc2mev[0] * SF /sumOfc;
     }

     int nmax=m_NSamples;
     if ((int)(rndm_digit_samples.size()) < m_NSamples) {
         ATH_MSG_WARNING("Less digit Samples than requested in digitization for cell " << ch_id.get_compact() << " Digit has " << rndm_digit_samples.size() << " samples.  Digitization request " << m_NSamples); 
         nmax=rndm_digit_samples.size();
     }
     for(i=0 ; i<nmax ; i++)
     {
       rAdc = (rndm_digit_samples[i] - Pedestal ) * adc2energy + adc0;
       rndm_energy_samples[i] = rAdc ;
       m_Samples[i] += rAdc ;
     }
  }
  else {
    ATH_MSG_WARNING(" No ramp found for this random cell " << m_larem_id->show_to_string(cellId) << " for gain " << rndmEvtDigit->gain());
  }
 }
//...................................................................

//
//........ choice of the gain
//
//
// fix the shift +1 if HEC  and nSamples 4 and firstSample 0
  int ihecshift=0;
  if(iCalo == HEC && m_NSamples.value() == 4 && m_firstSample.value() == 0) ihecshift=1;
  float samp2=m_Samples[m_sampleGainChoice-ihecshift]*MeV2GeV;
  if ( samp2 <= m_EnergyThresh ) return(StatusCode::SUCCESS);

  if(m_doDigiTruth){
    float Samp2_DigiHSTruth=m_Samples_DigiHSTruth[m_sampleGainChoice-ihecshift]*MeV2GeV;
    if ( Samp2_DigiHSTruth <= m_EnergyThresh ) createDigit_DigiHSTruth = false;
  }
    //We choose the gain in applying thresholds on the 3rd Sample (index "2")
    //converted in ADC counts in MediumGain (index "1" of (ADC2MEV)).
    //Indeed, thresholds in ADC counts are defined with respect to the MediumGain.
    //
    //              1300              3900
    // ---------------|----------------|--------------> ADC counts in MediumGain
    //    HighGain  <---  MediumGain  --->  LowGain

  float pseudoADC3;
  float Pedestal = pedestal->pedestal(ch_id,CaloGain::LARMEDIUMGAIN);
  if (Pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
   ATH_MSG_DEBUG(" Pedestal not found for medium gain ,cellID " << cellId <<  " assume 1000 ");
   Pedestal=1000.;
  }
  auto polynom_adc2mev = adc2MeVs->ADC2MEV(cellId,CaloGain::LARMEDIUMGAIN);
  if ( polynom_adc2mev.size() < 2) {
    ATH_MSG_WARNING(" No medium gain ramp found for cell " << m_larem_id->show_to_string(cellId) << " no digit produced...");
    return StatusCode::SUCCESS;
  }
  pseudoADC3 = m_Samples[m_sampleGainChoice-ihecshift]/(polynom_adc2mev[1])/SF + Pedestal ;

  //
  // ......... try a gain
  //
  if ( pseudoADC3 <= m_HighGainThresh[iCalo])
    igain = CaloGain::LARHIGHGAIN;
   else if ( pseudoADC3 <= m_LowGainThresh[iCalo])
    igain = CaloGain::LARMEDIUMGAIN;
  else
    igain = CaloGain::LARLOWGAIN;

 // check that select gain is never lower than random gain in case of overlay
  if (rndmGain==CaloGain::LARMEDIUMGAIN && igain==CaloGain::LARHIGHGAIN) igain=CaloGain::LARMEDIUMGAIN;
  if (rndmGain==CaloGain::LARLOWGAIN ) igain=CaloGain::LARLOWGAIN;

//
// recompute m_Samples if igain != HIGHGAIN
//
   if (igain != initialGain ){

     for (i=0;i<m_NSamples;i++) {
       if(m_doDigiTruth) m_Samples_DigiHSTruth[i] = 0.;
       if (m_RndmEvtOverlay) m_Samples[i]= rndm_energy_samples[i] ;
       else m_Samples[i] = 0.;
     }

     if (!isDead) {
       if( this->ConvertHits2Samples(cellId,ch_id,igain,TimeE, m_Samples) == StatusCode::FAILURE ) return StatusCode::SUCCESS;
       if(m_doDigiTruth){
         if( this->ConvertHits2Samples(cellId,ch_id,igain,TimeE_DigiHSTruth, m_Samples_DigiHSTruth) == StatusCode::FAILURE ) return StatusCode::SUCCESS;
       }
     }

   }

//
// ........ add the noise ................................
//

  int BvsEC=0;
  if(iCalo==EM || iCalo==EMIW) BvsEC=abs(m_larem_id->barrel_ec(cellId));

  if(    m_NoiseOnOff
      && (    (BvsEC==1 && m_NoiseInEMB)
           || (BvsEC>1  && m_NoiseInEMEC)
           || (iCalo==HEC && m_NoiseInHEC)
           || (iCalo==FCAL && m_NoiseInFCAL)
         )
    )
    //add the noise only in the wanted sub-detectors
  {
     if ( !m_RndmEvtOverlay ) {
        if (!m_pedestalNoise) {
          SigmaNoise =noise->noise(ch_id,igain );
        } else {
          float noise = pedestal->pedestalRMS(ch_id,igain);
          if (noise >= (1.0+LArElecCalib::ERRORCODE) ) SigmaNoise = noise;
          else SigmaNoise=0.;
        }
        // Sqrt of noise covariance matrix
	const std::vector<float>& CorGen=autoCorrNoise->autoCorrSqrt(cellId,igain);

        RandGaussZiggurat::shootArray(engine,m_NSamples,m_Rndm,0.,1.);

        int index;
        for (int i=0;i<m_NSamples;i++) {
           m_Noise[i]=0.;
           for(int j=0;j<=i;j++) {
             index = i*m_NSamples + j;
             m_Noise[i] += m_Rndm[j] * CorGen[index];
           }
           m_Noise[i]=m_Noise[i]*SigmaNoise;
        }
     } else for (int i=0;i<m_NSamples;i++) m_Noise[i]=0.;
  }
//
// ......... convert into adc counts  ................................
//
  Pedestal = pedestal->pedestal(ch_id,igain);
  if (Pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
     ATH_MSG_WARNING(" pedestal not found for cellId " << cellId << " assume 1000" );
     Pedestal=1000.;
  }
  polynom_adc2mev = adc2MeVs->ADC2MEV(cellId,igain);
  if (polynom_adc2mev.size() < 2) {
    ATH_MSG_WARNING(" No ramp found for requested gain " << igain << " for cell " << m_larem_id->show_to_string(cellId) << " no digit made...");
    return StatusCode::SUCCESS;
  }

  energy2adc=1./(polynom_adc2mev[1])/SF;

// in case Medium or low gain, take into account ramp intercept in energy->ADC computation
//   this requires to take into account the sum of the optimal filter coefficients, as they don't compute with ADC shift
  if(!m_isMcOverlay && m_RndmEvtOverlay  && igain>0 && m_larOFC)
  {
    float sumOfc=0.;
    ILArOFC::OFCRef_t ofc_a = m_larOFC->OFC_a(ch_id,igain,0);
    if (ofc_a.size()>0) {
      for (unsigned int j=0;j<ofc_a.size();j++) sumOfc+= ofc_a.at(j);
    }
    if ((polynom_adc2mev[1])>0 && sumOfc>0) Pedestal = Pedestal - (polynom_adc2mev[0])/(polynom_adc2mev[1])/sumOfc;
    ATH_MSG_DEBUG("  Params for final LAr Digitization  gain: " << igain << "    pedestal: " << Pedestal <<  "   energy2adc: " << energy2adc);
  }
  for(i=0;i<m_NSamples;i++)
  {
    double xAdc;
    double xAdc_DigiHSTruth = 0;

    if ( m_NoiseOnOff ){
      xAdc =  m_Samples[i]*energy2adc + m_Noise[i] + Pedestal + 0.5;
      if(m_doDigiTruth) {
        xAdc_DigiHSTruth =  m_Samples_DigiHSTruth[i]*energy2adc + m_Noise[i] + Pedestal + 0.5;
      }
    }

    else {
      if (m_roundingNoNoise) {
        float flatRndm = RandFlat::shoot(engine);
        xAdc =  m_Samples[i]*energy2adc + Pedestal + flatRndm;
        if(m_doDigiTruth) {
          xAdc_DigiHSTruth =  m_Samples_DigiHSTruth[i]*energy2adc + Pedestal + flatRndm;
        }

      }
      else{
         xAdc =  m_Samples[i]*energy2adc + Pedestal + 0.5;
         if(m_doDigiTruth) {
           xAdc_DigiHSTruth =  m_Samples_DigiHSTruth[i]*energy2adc + Pedestal + 0.5;
         }
      }

    }

//
// ........ truncate at maximum value + 1
//          add possibility to saturate at 0 for negative signals
//
    if (xAdc <0)  Adc=0;
    else if (xAdc >= MAXADC) Adc=MAXADC;
    else Adc = (short) xAdc;

    AdcSample[i]=Adc;

    if(m_doDigiTruth){
      if (xAdc_DigiHSTruth <0)  Adc_DigiHSTruth=0;
      else if (xAdc_DigiHSTruth >= MAXADC) Adc_DigiHSTruth=MAXADC;
      else Adc_DigiHSTruth = (short) xAdc_DigiHSTruth;
      AdcSample_DigiHSTruth[i] = Adc_DigiHSTruth;
    }

#ifndef NDEBUG
    ATH_MSG_DEBUG(" Sample " << i << "  Energy= " << m_Samples[i] << "  Adc=" << Adc);
#endif

  }

//
// ...... create the LArDigit and push it into the Digit container ..................
//
  Digit = new LArDigit(ch_id,igain,AdcSample);
  m_DigitContainer->push_back(Digit);


  if(m_doDigiTruth && createDigit_DigiHSTruth){
    createDigit_DigiHSTruth = false;

    for(int i=0; i<m_NSamples; i++) {
      if(m_Samples_DigiHSTruth[i] != 0) createDigit_DigiHSTruth = true;
    }

    Digit_DigiHSTruth = new LArDigit(ch_id,igain,AdcSample_DigiHSTruth);
    m_DigitContainer_DigiHSTruth->push_back(Digit_DigiHSTruth);
  }


  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------------------------------------------------------------------

StatusCode LArPileUpTool::ConvertHits2Samples(const Identifier & cellId, const HWIdentifier ch_id, CaloGain::CaloGain igain,
					      //const std::vector<std::pair<float,float> >  *TimeE)
					      const std::vector<std::pair<float,float> >  *TimeE, std::vector<double> &sampleList)

{
// Converts  hits of a particular LAr cell into energy samples
// declarations
   int nsamples ;
   int nsamples_der ;
   int i ;
   int j ;
   float energy ;
   float time ;

   SG::ReadCondHandle<ILArShape> shapeHdl(m_shapeKey);
   const ILArShape* shape=*shapeHdl;


// ........ retrieve data (1/2) ................................
//
   ILArShape::ShapeRef_t Shape = shape->Shape(ch_id,igain);
   ILArShape::ShapeRef_t ShapeDer = shape->ShapeDer(ch_id,igain);

  nsamples = Shape.size();
  nsamples_der = ShapeDer.size();

  if (nsamples==0) {
    ATH_MSG_INFO(" No samples for cell = " << cellId );
    return StatusCode::FAILURE;
  }

#ifndef NDEBUG
  ATH_MSG_DEBUG(" Cellid " << m_larem_id->show_to_string(cellId));
  for (i=0;i<nsamples;i++)
  {
       ATH_MSG_DEBUG(Shape[i] << " ");
  }
#endif

  std::vector<std::pair<float,float> >::const_iterator first = TimeE->begin();
  std::vector<std::pair<float,float> >::const_iterator last  = TimeE->end();

  while (first != last)
  {
   energy = (*first).first;
   time   = (*first).second;

#ifndef NDEBUG
  ATH_MSG_DEBUG("m_NSamples, m_usePhase " << m_NSamples << " " << m_usePhase);
#endif

   // fix the shift +1 if HEC  and nSamples 4 and firstSample 0
   // in case of data overlay this should NOT  be done as the pulse shape read from the database is already shifted
   //   but this should still be done in case of MC overlay
   int ihecshift=0;
   if((!m_RndmEvtOverlay || m_isMcOverlay) && m_larem_id->is_lar_hec(cellId) && m_NSamples.value() == 4 && m_firstSample.value() == 0) ihecshift=1;


   if (!m_usePhase) {

 // Atlas like mode where we use 25ns binned pulse shape and derivative to deal with time offsets

// shift between reference shape and this time
      int ishift=(int)(rint(time*(1./25.)));
      double dtime=time-25.*((double)(ishift));

      for (i=0;i<m_NSamples.value();i++)
      {
       j = i - ishift + m_firstSample + ihecshift;
#ifndef NDEBUG
       ATH_MSG_DEBUG(" time/i/j " << time << " "<< i << " " << j);
#endif
       if (j >=0 && j < nsamples ) {
         if (j<nsamples_der && std::fabs(ShapeDer[j])<10. )
              sampleList[i] += (Shape[j]- ShapeDer[j]*dtime)*energy ;
         else sampleList[i] += Shape[j]*energy ;
       }
      }
   }

// Mode to use phase (tbin) to get pulse shape ( pulse shape with fine time binning should be available)

   else {

     // FIXME hardcode 8phases3ns configuration (cannot access parameters from ILArShape interface now)
     int nTimeBins = 8;
     float timeBinWidth = 25./24.*3.;

//    -50<t<-25 phase=-t-25, shift by one peak time (for s2 uses shape(3) with tbin)
// for -25<t<0  phase = -t, no shift of peak time
// for 0<t<25   phase=25-t, shift by one peak time (for s2 uses shape(1) with tbin)
//    25<t<50   phase=50-t, shift by two
//  etc...

      int ishift = (int)(time*(1./25.));
      int tbin;
      if (time>0) {
         tbin = (int)(fmod(time,25)/timeBinWidth);
          if (tbin>0) {
             tbin=nTimeBins-tbin;
             ishift +=1;
          }
      } else {
         tbin = (int)(fmod(-time,25)/timeBinWidth);
      }

      double dtime = time - ( 25.*((float)(ishift)) - timeBinWidth*tbin);

      Shape = shape->Shape(ch_id,igain,tbin);
      ShapeDer = shape->ShapeDer(ch_id,igain,tbin);

      nsamples = Shape.size();
      nsamples_der = ShapeDer.size();


      for (i=0;i<m_NSamples.value();i++)
      {
       j = i - ishift+m_firstSample + ihecshift;
#ifndef NDEBUG
       ATH_MSG_DEBUG(" time/i/j " << time << " "<< i << " " << j);
#endif
       if (j >=0 && j < nsamples ) {
         if (j<nsamples_der && std::fabs(ShapeDer[j])<10. )
              sampleList[i] += (Shape[j]- ShapeDer[j]*dtime)*energy ;
         else sampleList[i] += Shape[j]*energy ;
       }
      }

   }     // else if of m_usePhase

   ++first;
  }         // loop over hits

   return StatusCode::SUCCESS;

}

// ----------------------------------------------------------------------------------------------------------------------------------

//
// take accumulated energy in cell vector for a given bunch time and push that in the hit map

bool LArPileUpTool::fillMapfromSum(float bunchTime)  {

  for (unsigned int i=0;i<m_energySum.size();i++) {
     float e = m_energySum[i];
     if (e>1e-6) {
        if (!m_hitmap->AddEnergy(i,e,bunchTime)) return false;
     }
     m_energySum[i]=0.;
  }
  if(m_doDigiTruth){
    for (unsigned int i=0;i<m_energySum_DigiHSTruth.size();i++) {
       float e = m_energySum_DigiHSTruth[i];
       if (e>1e-6) {
         if (!m_hitmap_DigiHSTruth->AddEnergy(i,e,bunchTime)) return false;
       }
       m_energySum_DigiHSTruth[i]=0.;
    }
  }

  return true;
}
