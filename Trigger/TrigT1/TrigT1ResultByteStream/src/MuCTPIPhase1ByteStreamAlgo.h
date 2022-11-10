// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_MUCTPIPHASE1BYTESTREAMALGO_H
#define TRIGT1RESULTBYTESTREAM_MUCTPIPHASE1BYTESTREAMALGO_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

#include "StoreGate/ReadHandleKey.h"
#include "TrigConfData/L1CTPFiles.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfIO/TrigDBCTPFilesLoader.h"
#include "TrigT1Result/MuCTPI_Phase1_RDO.h"

// Local include(s):
#include "MuCTPISrcIdMap.h"

///this is done in the original (run2) tool; trying to replace this with an include above.
// Forward declaration(s):
//class MuCTPI_Phase1_RDO;

/**
 *   @short Algo doing the ROB -> MuCTPI_Phase1_RDO ByteStream conversion
 */
class MuCTPIPhase1ByteStreamAlgo : public AthReentrantAlgorithm {

public:
  /// Default constructor
  MuCTPIPhase1ByteStreamAlgo( const std::string& name, ISvcLocator* svcLoc );

  virtual StatusCode initialize() override;  
  virtual StatusCode execute(const EventContext& eventContext) const override;

  /// Convert ROBFragment to MuCTPI_RDO
  StatusCode convert( const IROBDataProviderSvc::ROBF* rob, SG::WriteHandle<MuCTPI_Phase1_RDO>& outputHandle ) const;



private:
  /// Object storing the various IDs of the MuCTPI fragment
  Gaudi::Property<uint32_t> m_robId{this, "ROBID", 0x760000, "MuCTPI DAQ readout ROB ID"};


  /// ROBDataProvider service handle
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc { this, "ROBDataProviderSvc", "ROBDataProviderSvc", "ROB data provider"};

  SG::WriteHandleKey<MuCTPI_Phase1_RDO> m_MuCTPI_Phase1_RDOKey{ this, "MuCTPI_Phase1_RDOKey", "MUCTPI_Phase1_RDO" };

  std::vector<uint32_t> m_muctpi_Nbits = {3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1}; //default value, used for DQ mon
  Gaudi::Property<std::string> m_alias_db{this,"TriggerDBAlias","TRIGGERDB_RUN3","Alias to the TriggerDB to download CTP configuration from"};

}; // class MuCTPIPhase1ByteStreamAlgo

#endif // TRIGT1RESULTBYTESTREAM_MUCTPIPHASE1BYTESTREAMALGO_H
