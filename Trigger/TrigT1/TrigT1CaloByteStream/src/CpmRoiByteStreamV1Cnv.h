/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMV1CNV_H
#define TRIGT1CALOBYTESTREAM_CPMROIBYTESTREAMV1CNV_H

#include <string>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class DataObject;
class IByteStreamEventAccess;
class IOpaqueAddress;
class IROBDataProviderSvc;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;


namespace LVL1BS {

class CpmRoiByteStreamV1Tool;

/** ByteStream converter for Cluster Processor Module RoIs.
 *
 *  @author Peter Faulkner
 */

class CpmRoiByteStreamV1Cnv: public Converter {
public:
  CpmRoiByteStreamV1Cnv(ISvcLocator* svcloc);

  ~CpmRoiByteStreamV1Cnv();

  virtual StatusCode initialize();
  /// Create CPM RoIs from ByteStream
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);
  /// Create ByteStream from CPM RoIs
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  //  Storage type and class ID
  virtual long repSvcType() const { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:

  /// Converter name
  std::string m_name;

  /// Tool that does the actual work
  ToolHandle<LVL1BS::CpmRoiByteStreamV1Tool> m_tool;

  /// Service for reading bytestream
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;

  /// Message log
  bool m_debug;
};

} // end namespace

#endif
