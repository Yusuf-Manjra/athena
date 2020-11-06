/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_JEPROIBYTESTREAMCNV_H
#define TRIGT1CALOBYTESTREAM_JEPROIBYTESTREAMCNV_H

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

class JepRoiByteStreamTool;

/** ByteStream converter for JEP RoI container
 *
 *  @author Peter Faulkner
 */

class JepRoiByteStreamCnv: public Converter {

  friend class CnvFactory<JepRoiByteStreamCnv>;

protected:

  JepRoiByteStreamCnv(ISvcLocator* svcloc);

public:

  ~JepRoiByteStreamCnv();

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
  ToolHandle<LVL1BS::JepRoiByteStreamTool> m_tool;
};

} // end namespace

#endif
