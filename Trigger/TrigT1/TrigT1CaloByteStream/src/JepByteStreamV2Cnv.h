/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPBYTESTREAMV2CNV_H
#define TRIGT1CALOBYTESTREAM_JEPBYTESTREAMV2CNV_H

#include <string>

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class DataObject;
class IByteStreamEventAccess;
class IOpaqueAddress;
class ISvcLocator;
class StatusCode;

template <typename> class CnvFactory;


namespace LVL1BS {

class JepByteStreamV2Tool;

/** ByteStream converter for JEP container post LS1
 *
 *  @author Peter Faulkner
 */

class JepByteStreamV2Cnv: public Converter {
public:
  JepByteStreamV2Cnv(ISvcLocator* svcloc);

  ~JepByteStreamV2Cnv();

  virtual StatusCode initialize();
  /// Create ByteStream from JEP Container
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

  //  Storage type and class ID
  virtual long repSvcType() const { return i_repSvcType(); }
  static  long storageType();
  static const CLID& classID();

private:

  /// Converter name
  std::string m_name;

  /// Tool that does the actual work
  ToolHandle<LVL1BS::JepByteStreamV2Tool> m_tool;
};

} // end namespace

#endif
