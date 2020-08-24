/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBYTESTREAMCNVSVC_H
#define TRIGBYTESTREAMCNVSVC_H

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "AthenaMonitoringKernel/Monitored.h"

// Forward declarations
class StoreGateSvc;
class IROBDataProviderSvc;

/** @class TrigByteStreamCnvSvc
 *  @brief A ByteStreamCnvSvc implementation for online use
 *
 *  It overrides the connectOutput and commitOutput methods of the base class. In this implementation, they create
 *  the specific online HLT output and send it out directly to the TDAQ infrastructure without using an output service.
 *
 *  It needs to inherit from ByteStreamCnvSvc rather than ByteStreamCnvSvcBase, because some elements of the athena
 *  framework (particularly converters) rely on the properties or functionality of the offline ByteStreamCnvSvc.
 **/
class TrigByteStreamCnvSvc : public ByteStreamCnvSvc {
public:
  /// Standard constructor
  TrigByteStreamCnvSvc(const std::string& name, ISvcLocator* svcLoc);
  /// Standard destructor
  virtual ~TrigByteStreamCnvSvc();

  // ------------------------- IService methods --------------------------------
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // ------------------------- IConversionSvc methods --------------------------
  /// In the case of online BS data, this method creates the output FullEventFragment and fills its header
  using ByteStreamCnvSvcBase::connectOutput;
  virtual StatusCode connectOutput(const std::string& outputFile) override;
  /// This overload is kept only for interface compatibility
  virtual StatusCode connectOutput(const std::string& outputFile, const std::string& openMode) override;
  /// In the case of online BS data, this method binds and sends out the output FullEventFragment
  virtual StatusCode commitOutput(const std::string& outputFile, bool do_commit) override;

private:
  // ------------------------- Helper methods ----------------------------------
  /// Print contents of m_rawEventWrite
  void printRawEvent();
  /// Fill histograms from contents of a FullEventFragment
  void monitorRawEvent(const std::unique_ptr<uint32_t[]>& rawEventPtr) const;

  // ------------------------- Service / Tool handles --------------------------
  ServiceHandle<StoreGateSvc> m_evtStore {this, "EventStore", "StoreGateSvc"};
  ServiceHandle<IROBDataProviderSvc> m_robDataProviderSvc {this, "ROBDataProvider", "ROBDataProviderSvc"};
  ToolHandle<GenericMonitoringTool> m_monTool {this, "MonTool", "" , "Monitoring tool"};
};

#endif // TRIGBYTESTREAMCNVSVC_H
