// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventFormatSvc.h 641395 2015-01-23 20:07:21Z ssnyder $
#ifndef XAODEVENTFORMATCNV_EVENTFORMATSVC_H
#define XAODEVENTFORMATCNV_EVENTFORMATSVC_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "StoreGate/StoreGateSvc.h"

// xAOD include(s):
#include "xAODEventFormat/EventFormat.h"

// Local include(s):
#include "xAODEventFormatCnv/IEventFormatSvc.h"

namespace xAODMaker {

   /**
    *  @short Implementation of the IEventFormatSvc service interface
    *
    *         This class takes care of managing the xAOD::EventFormat
    *         objects of the xAOD files.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 641395 $
    * $Date: 2015-01-23 21:07:21 +0100 (Fri, 23 Jan 2015) $
    */
   class EventFormatSvc : public AthService,
                          public virtual IEventFormatSvc,
                          public virtual IIncidentListener {

   public:
      /// Regular Service constructor
      EventFormatSvc( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the service
      virtual StatusCode initialize();

      /// @name Implementation of the IEventFormatSvc interface
      /// @{

      /// Get the object describing the current input/output file's format
      const xAOD::EventFormat* eventFormat();

      /// @}

      /// Function handling the incoming incidents
      virtual void handle( const Incident& inc );

      /// Function communicating the implemented interfaces to Gaudi
      virtual StatusCode queryInterface( const InterfaceID& riid,
                                         void** ppvIf );

   private:
      /// Fill the internal object with metadata from the event
      int collectMetadata( xAOD::EventFormat& ef );

      /// StoreGate key of the EventFormat object
      std::string m_objectName;

      /// Connection to the CLID service
      ServiceHandle< IClassIDSvc > m_clidSvc;
      /// Connection to the event store
      ServiceHandle< StoreGateSvc > m_eventStore;
      /// Connection to the metadata store
      ServiceHandle< StoreGateSvc > m_metaStore;
      /// Connection to the incident service
      ServiceHandle< IIncidentSvc > m_incidentSvc;
      /// We automatically generate event format information for type names
      /// that contain the string `xAOD'.  This property gives a list of
      /// additional type names that should have format information.
      std::vector<std::string> m_formatNames;

      /// The object that we write out in the end
      xAOD::EventFormat* m_ef;

      /// Helper variable detecting the first event in the processing
      bool m_firstEvent;

   }; // class EventFormatSvc

} // namespace xAODMaker

#endif // XAODEVENTFORMATCNV_EVENTFORMATSVC_H
