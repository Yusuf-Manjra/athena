// Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

#include "./TrigDBHelper.h"
#include "TrigConfIO/TrigDBMonitoringLoader.h"

TrigConf::TrigDBMonitoringLoader::TrigDBMonitoringLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBMonitoringLoader", connection)
{
   /*
     DB queries
   */
   { // for schema version 7
      auto & q = m_queries[7];        
      // tables
      q.addToTableList ( "SUPER_MASTER_TABLE", "SMT" );
      q.addToTableList ( "HLT_MONITORING_GROUPS", "HMG" );
      // bind vars
      q.extendBinding<int>("smk");
      // conditions
      q.extendCondition("SMT.SMT_ID = :smk");
      q.extendCondition(" AND SMT.SMT_HLT_MENU_ID = HMG.HMG_HLT_MENU_ID");
      q.extendCondition(" AND HMG.HMG_IN_USE=1");
      q.extendCondition(" ORDER BY HMG.HMG_ID DESC");
      // attributes
      q.extendOutput<int>        ( "HMG.HMG_ID" );
      q.extendOutput<coral::Blob>( "HMG.HMG_DATA" );
      // the field with the data
      q.setDataName("HMG.HMG_DATA");
   }
}


// Destructor defined here because QueryDefinition is an incomplete type in the header
TrigConf::TrigDBMonitoringLoader::~TrigDBMonitoringLoader() = default;


bool
TrigConf::TrigDBMonitoringLoader::loadHLTMonitoring ( unsigned int smk,
                                          boost::property_tree::ptree & hltmonitoring,
                                          const std::string & outFileName ) const
{
   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);
   const size_t sv = schemaVersion(session.get());
   QueryDefinition qdef = getQueryDefinition(sv, m_queries);
   try {
      qdef.setBoundValue<int>("smk", smk);
      auto q = qdef.createQuery( session.get() );
      auto & cursor = q->execute();
      if ( ! cursor.next() ) {
         TRG_MSG_ERROR("Tried reading HLT menu, but SuperMasterKey " << smk << " is not available" );
         throw TrigConf::NoSMKException("TrigDBMonitoringLoader: SMK " + std::to_string(smk) + " not available");
      }
      const coral::AttributeList& row = cursor.currentRow();
      const coral::Blob& dataBlob = row[qdef.dataName()].data<coral::Blob>();
      writeRawFile( dataBlob, outFileName );
      blobToPtree( dataBlob, hltmonitoring );
   }
   catch(coral::QueryException & ex) {
      TRG_MSG_ERROR("When reading HLT menu for SMK " << smk << " a coral::QueryException was caught ( " << ex.what() <<" )" );
      throw TrigConf::QueryException("TrigDBMonitoringLoader: " + std::string(ex.what()));
   }
   return true;
}

bool
TrigConf::TrigDBMonitoringLoader::loadHLTMonitoring( unsigned int smk, HLTMonitoring & hltmonitoring,
                                         const std::string & outFileName ) const
{
   boost::property_tree::ptree pthlt;
   loadHLTMonitoring( smk, pthlt, outFileName );
   try {
      hltmonitoring.setData(std::move(pthlt));
      hltmonitoring.setSMK(smk);
   }
   catch(std::exception & ex) {
      hltmonitoring.clear();
      TRG_MSG_ERROR("When reading HLT menu for SMK " << smk << " a parsing error occured ( " << ex.what() <<" )" );
      throw TrigConf::ParsingException("TrigDBMonitoringLoader: parsing error " + std::string(ex.what()));
   }
   return true;
}
