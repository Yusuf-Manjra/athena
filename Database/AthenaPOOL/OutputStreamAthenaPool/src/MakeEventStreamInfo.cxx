/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @file MakeEventStreamInfo.cxx
 *  @brief This file contains the implementation for the MakeEventStreamInfo class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: MakeEventStreamInfo.cxx,v 1.7 2009-02-09 22:48:31 gemmeren Exp $
 **/

#include "MakeEventStreamInfo.h"

#include "GaudiKernel/IAlgorithm.h"

#include "PersistentDataModel/DataHeader.h"
#include "EventInfo/EventStreamInfo.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "EventInfoUtils/EventInfoFromxAOD.h"

//___________________________________________________________________________
MakeEventStreamInfo::MakeEventStreamInfo(const std::string& type,
	const std::string& name,
	const IInterface* parent) : ::AthAlgTool(type, name, parent),
		m_metaDataStore("StoreGateSvc/MetaDataStore", name),
                m_eventStore("StoreGateSvc", name)
{
   // Declare IAthenaOutputStreamTool interface
   declareInterface<IAthenaOutputTool>(this);

   // Declare the properties
   declareProperty("Key", m_key = "");
   declareProperty("EventInfoKey", m_eventInfoKey = "EventInfo");
   declareProperty("OldEventInfoKey", m_oEventInfoKey = "McEventInfo");
}
//___________________________________________________________________________
MakeEventStreamInfo::~MakeEventStreamInfo() {
}
//___________________________________________________________________________
StatusCode MakeEventStreamInfo::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   // Locate the MetaDataStore
   if (!m_metaDataStore.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not find MetaDataStore");
      return(StatusCode::FAILURE);
   }
   if (!m_eventStore.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not find EventStore");
      return(StatusCode::FAILURE);
   }

   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode MakeEventStreamInfo::postInitialize() {
   // Remove EventStreamInfo with same key if it exists
   if (m_metaDataStore->contains<EventStreamInfo>(m_key.value())) {
      const EventStreamInfo* pEventStream = nullptr;
      if (!m_metaDataStore->retrieve(pEventStream, m_key.value()).isSuccess()) {
         ATH_MSG_ERROR("Unable to retrieve EventStreamInfo object");
         return(StatusCode::FAILURE);
      }
      if (!m_metaDataStore->removeDataAndProxy(pEventStream).isSuccess()) {
         ATH_MSG_ERROR("Unable to remove proxy for EventStreamInfo object");
         return(StatusCode::FAILURE);
      }
   }
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode MakeEventStreamInfo::preExecute() {
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode MakeEventStreamInfo::postExecute() {
   const IAlgorithm* parentAlg = dynamic_cast<const IAlgorithm*>(this->parent());
   if (parentAlg == nullptr) {
      ATH_MSG_ERROR("Unable to get parent Algorithm");
      return(StatusCode::FAILURE);
   }

   SG::ReadHandle<DataHeader> dataHeader(parentAlg->name());
   if (!dataHeader.isValid()) {
      return(StatusCode::SUCCESS);
   }
   // Retrieve the EventInfo object
   EventType evtype;
   unsigned long long runN = 0;
   unsigned lumiN = 0;
   SG::ReadHandle<xAOD::EventInfo> xEventInfo(m_eventInfoKey);
   if (xEventInfo.isValid()) {
      runN = xEventInfo->runNumber();
      lumiN = xEventInfo->lumiBlock();
      evtype = eventTypeFromxAOD(xEventInfo.get());
   } else {
      SG::ReadHandle<EventInfo> oEventInfo(m_oEventInfoKey);
      if (oEventInfo.isValid()) {
         runN = oEventInfo->event_ID()->run_number();
         lumiN = oEventInfo->event_ID()->lumi_block();
         evtype = *oEventInfo->event_type();
      } else {
         ATH_MSG_ERROR("Unable to retrieve EventInfo object");
         return(StatusCode::FAILURE);
      }
   }
   if (!m_metaDataStore->contains<EventStreamInfo>(m_key.value())) {
      EventStreamInfo* pEventStream = new EventStreamInfo();
      if (m_metaDataStore->record(pEventStream, m_key.value()).isFailure()) {
         ATH_MSG_ERROR("Could not register EventStreamInfo object");
         return(StatusCode::FAILURE);
      }
   }
   EventStreamInfo* pEventStream = nullptr;
   if (!m_metaDataStore->retrieve(pEventStream, m_key.value()).isSuccess()) {
      ATH_MSG_ERROR("Unable to retrieve EventStreamInfo object");
      return(StatusCode::FAILURE);
   }
   pEventStream->addEvent();
   pEventStream->insertProcessingTag(dataHeader->getProcessTag());
   pEventStream->insertLumiBlockNumber( lumiN );
   pEventStream->insertRunNumber( runN );
   for (std::vector<DataHeaderElement>::const_iterator iter = dataHeader->begin(), iterEnd = dataHeader->end();
		   iter != iterEnd; iter++) {
      pEventStream->insertItemList(iter->getPrimaryClassID(), iter->getKey());
   }
   pEventStream->insertEventType( evtype );
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode MakeEventStreamInfo::preFinalize() {
   if (!m_metaDataStore->contains<EventStreamInfo>(m_key.value())) {
      EventStreamInfo* pEventStream = new EventStreamInfo();
      if (m_metaDataStore->record(pEventStream, m_key.value()).isFailure()) {
         ATH_MSG_ERROR("Could not register EventStreamInfo object");
         return(StatusCode::FAILURE);
      }
   }
   return(StatusCode::SUCCESS);
}
//___________________________________________________________________________
StatusCode MakeEventStreamInfo::finalize() {
   ATH_MSG_DEBUG("in finalize()");
   // release the MetaDataStore
   if (!m_metaDataStore.release().isSuccess()) {
      ATH_MSG_WARNING("Could not release MetaDataStore");
   }
   if (!m_eventStore.release().isSuccess()) {
      ATH_MSG_WARNING("Could not release EventStore");
   }
   return(StatusCode::SUCCESS);
}
