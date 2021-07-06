/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     LArBadFebMaskingTool
PACKAGE:  offline/Calorimeter/LArCellRec

AUTHORS: G.Unal
CREATION:  23 april 2009

PURPOSE:

********************************************************************/

#include "LArBadFebMaskingTool.h"

#include "CaloEvent/CaloCellContainer.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArRawEvent/LArFebErrorSummary.h" 
#include "LArIdentifier/LArOnlineID.h" 
#include "LArRecConditions/LArBadFeb.h"
#include "LArRecEvent/LArEventBitInfo.h"
#include "xAODEventInfo/EventInfo.h"
#include "GaudiKernel/ThreadLocalContext.h"

using xAOD::EventInfo;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArBadFebMaskingTool::LArBadFebMaskingTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :base_class(type, name, parent),
   m_maskParity(true),m_maskSampleHeader(true),m_maskEVTID(true),m_maskScacStatus(true),
   m_maskScaOutOfRange(true),m_maskGainMismatch(true),m_maskTypeMismatch(true),m_maskNumOfSamples(true),
   m_maskEmptyDataBlock(true),m_maskDspBlockSize(true),m_maskCheckSum(true),m_maskMissingHeader(true),
   m_maskBadGain(true),m_minFebsInError(1),
   m_larFebErrorSummaryKey("LArFebErrorSummary"),
   m_errorToMask(0),
   m_calo_id(nullptr),
   m_onlineID(nullptr),
   m_evt(0),
   m_mask(0)
{ 
  declareProperty("maskParity",m_maskParity);
  declareProperty("maskSampleHeader",m_maskSampleHeader);
  declareProperty("maskEVTID",m_maskEVTID);
  declareProperty("maskScacStatus",m_maskScacStatus);
  declareProperty("maskScaOutOfRange",m_maskScaOutOfRange);
  declareProperty("maskGainMismatch",m_maskGainMismatch);
  declareProperty("maskTypeMismatch",m_maskTypeMismatch);
  declareProperty("maskNumOfSamples",m_maskNumOfSamples);
  declareProperty("maskEmptyDataBlock",m_maskEmptyDataBlock);
  declareProperty("maskDspBlockSize",m_maskDspBlockSize);
  declareProperty("maskCheckSum",m_maskCheckSum);
  declareProperty("maskMissingHeader",m_maskMissingHeader);
  declareProperty("maskBadGain",m_maskBadGain);
  declareProperty("minFebInError",m_minFebsInError); // Minimum number of FEBs in error to trigger EventInfo::LArError (1 by default/bulk, 4 in online/express).
  declareProperty("larFebErrorSummaryKey",m_larFebErrorSummaryKey);
  declareProperty("eventInfoKey", m_eventInfoKey = "EventInfo");
}


/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode LArBadFebMaskingTool::initialize()
{
  m_evt=0;
  m_mask=0;

  m_errorToMask = 0;
  if (m_maskParity)         m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::Parity);
  if (m_maskSampleHeader)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::SampleHeader);
  if (m_maskEVTID)          m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::EVTID);
  if (m_maskScacStatus)     m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::ScacStatus);
  if (m_maskScaOutOfRange)  m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::ScaOutOfRange);
  if (m_maskGainMismatch)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::GainMismatch);
  if (m_maskTypeMismatch)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::TypeMismatch);
  if (m_maskNumOfSamples)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::NumOfSamples);
  if (m_maskEmptyDataBlock) m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::EmptyDataBlock);
  if (m_maskDspBlockSize)   m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::DspBlockSize);
  if (m_maskCheckSum)       m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::CheckSum);
  if (m_maskMissingHeader)  m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::MissingHeader);
  if (m_maskBadGain)        m_errorToMask = m_errorToMask | (1 <<LArFebErrorSummary::BadGain);

  ATH_MSG_INFO (" bit mask for errors to mask " << m_errorToMask);

  // initialize read handle key
  ATH_CHECK(m_larFebErrorSummaryKey.initialize());
  ATH_CHECK(m_eventInfoKey.initialize());

  const  CaloIdManager* caloIdMgr = 0;
  ATH_CHECK( detStore()->retrieve( caloIdMgr ) );
  m_calo_id = caloIdMgr->getCaloCell_ID();

  ATH_CHECK( m_badFebKey.initialize());
  ATH_CHECK( m_cablingKey.initialize());
  ATH_CHECK( detStore()->retrieve(m_onlineID, "LArOnlineID") );

  return StatusCode::SUCCESS;

}

StatusCode LArBadFebMaskingTool::finalize()
{
   ATH_MSG_INFO ("  ---- Summary from LArBadFebMaskingTool ");
   ATH_MSG_INFO ("   Number of events processed    " << m_evt);
   ATH_MSG_INFO ("   Number of masked Feb total    " << m_mask);
   float ratio=0.;
   if (m_evt>0) ratio=((float)(m_mask))/((float)(m_evt));
   ATH_MSG_INFO ("   Number of masked Feb per event " << ratio);

   return StatusCode::SUCCESS;
}

StatusCode LArBadFebMaskingTool::process (CaloCellContainer* theCont,
                                          const EventContext& ctx) const
{
  m_evt++;


  ATH_MSG_DEBUG (" in  LArBadFebMaskingTool::process ");
  //const LArFebErrorSummary* larFebErrorSummary;
  //StatusCode sc = evtStore()->retrieve(larFebErrorSummary,m_larFebErrorSummaryKey);
  SG::ReadHandle<LArFebErrorSummary>larFebErrorSummary(m_larFebErrorSummaryKey, ctx);
  if (!larFebErrorSummary.isValid()) {
    ATH_MSG_WARNING (" cannot retrieve Feb error summary.  Skip  LArBadFebMaskingTool::process ");
    return StatusCode::SUCCESS;
  }

  // retrieve map of Feb-errors
  const std::map<unsigned int,uint16_t>& febMap = larFebErrorSummary->get_all_febs();

  ATH_MSG_DEBUG (" Number of Febs " << febMap.size());

  // retrieve EventInfo
  SG::ReadHandle<xAOD::EventInfo> eventInfo (m_eventInfoKey, ctx);
  
  bool flagBadEvent = false;   // flag bad event = Feb error not known in database
  int nbOfFebsInError = 0;

  // catch cases of empty LAR container  => severe problem in decoding => flag event as in ERROR
  unsigned int nLar = theCont->nCellsCalo(CaloCell_ID::LAREM)+theCont->nCellsCalo(CaloCell_ID::LARHEC)+theCont->nCellsCalo(CaloCell_ID::LARFCAL);
  if (nLar==0) {
     ATH_MSG_DEBUG (" empty lar cell container ");
     flagBadEvent = true;
  }


  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey,ctx};
  const LArOnOffIdMapping* cabling=*cablingHdl;

  SG::ReadCondHandle<LArBadFebCont> badFebHdl{m_badFebKey,ctx};
  const LArBadFebCont* badFebs=*badFebHdl;


// loop over all Febs

  for (HWIdentifier febId : m_onlineID->feb_range()) {
      bool toMask1 = false;   // mask because of bad error
      bool inError = false;   // mask because Feb listed in database as being to mask
      bool isDead  = false;   // set to true if Feb is deadAll or deadReadout 

// for debug
      unsigned int ifeb = febId.get_identifier32().get_compact();
      ATH_MSG_DEBUG (" process Feb: " << ifeb);

      std::map<unsigned int,uint16_t>::const_iterator it1 = febMap.find(ifeb);
      if (it1 != febMap.end()) {
        uint16_t ierror = (*it1).second;
        if (ierror & m_errorToMask) toMask1=true;
        ATH_MSG_DEBUG (" ierror,toMask " << ierror << " " << toMask1 << " ");
      }


      LArBadFeb febstatus = badFebs->status(febId);
      inError = febstatus.inError();
      isDead = ( febstatus.deadReadout() | febstatus.deadAll() );
      ATH_MSG_DEBUG (" inError, isDead "  << inError << " " << isDead);
      

      if (toMask1 && !inError && !isDead) nbOfFebsInError = nbOfFebsInError + 1;

      if (toMask1 || inError) {
         m_mask++;
         for (int ch=0; ch<128; ++ch) {
           HWIdentifier hwid = m_onlineID->channel_Id(febId, ch);
           if (cabling->isOnlineConnected(hwid)) {
              Identifier id = cabling->cnvToIdentifier( hwid);
              IdentifierHash theCellHashID = m_calo_id->calo_cell_hash(id);
              int index = theCont->findIndex(theCellHashID);
              if (index<0) {
                 ATH_MSG_DEBUG (" cell " << hwid.get_compact() << " " << id.get_compact() << " is not in the container ");
                 continue;
              }
              CaloCell* aCell = theCont->at(index);

              if (aCell) {
                ATH_MSG_DEBUG ("  mask cell hwid= " << hwid.get_compact() << " offlineid = " << id.get_compact()
                               << " " << aCell->ID().get_compact());
                aCell->setEnergy(0.);
                aCell->setTime(0.);
                uint16_t qua=0;
                aCell->setQuality(qua);
                uint16_t provenance = (aCell->provenance() | 0x0800);
                aCell->setProvenance(provenance);
              }

           }  // isConnected
         }    // loop over channels in Feb
      }       // toMask

  }       // loop over Febs in error

  if (nbOfFebsInError >= m_minFebsInError) flagBadEvent=true;

  if (flagBadEvent) {
    ATH_MSG_DEBUG (" set error bit for LAr for this event ");
    if (!eventInfo->updateErrorState(EventInfo::LAr,EventInfo::Error)) {
      ATH_MSG_WARNING (" cannot set error state for LAr ");
    }
    if (!eventInfo->updateEventFlagBit(EventInfo::LAr,LArEventBitInfo::DATACORRUPTED)) {
      ATH_MSG_WARNING (" cannot set event bit info for LAr ");
    }
  }
 

  return StatusCode::SUCCESS;
}
