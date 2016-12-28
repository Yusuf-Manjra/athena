/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// an action to query the SD responsible for the storing of the
// TrackRecords *at the entrance of the ID* if no track it aborts
// the event - jamie boyd 15 nov 06


#ifndef G4CosmicAndFilter_H
#define G4CosmicAndFilter_H


#include "G4AtlasInterfaces/IEndEventAction.h"
#include "AthenaBaseComps/AthMessaging.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
namespace G4UA{


class G4CosmicAndFilter:
 public AthMessaging, public IEndEventAction
 {

 public:
   
   struct Config
   {
     std::string collectionName="TRTBarrelEntryLayer";
     std::string collectionName2="CaloEntryLayer";
   };
   
   G4CosmicAndFilter(const Config& config);
   
   struct Report
   {
     int ntot=0;
     int npass=0;

     void merge(const Report& rep){
       ntot+=rep.ntot;
       npass+=rep.npass;
     }
   };
   
   const Report& getReport() const
   { return m_report; }
   
   virtual void endOfEvent(const G4Event*) override;
 private:
   Config m_config;
   Report m_report;
   
   typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
   /// Pointer to StoreGate (event store by default)
   mutable StoreGateSvc_t m_evtStore;
   /// Pointer to StoreGate (detector store by default)
   mutable StoreGateSvc_t m_detStore;
   
}; // class G4CosmicFilter


} // namespace G4UA 


#endif
