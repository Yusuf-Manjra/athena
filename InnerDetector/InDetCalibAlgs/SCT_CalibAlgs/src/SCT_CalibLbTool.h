/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibLbTool.h
 * Header file for the SCT_CalibLbTool class
 * @author Shaun Roe
 **/

#ifndef SCT_CalibLbTool_h
#define SCT_CalibLbTool_h

//Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "SCT_CalibAlgs/ISCT_CalibEvtInfo.h"
#include "InDetRawData/SCT_RDO_Container.h"

//Inner detector includes
#include "InDetIdentifier/SCT_ID.h"

//local includes
#include "SCT_CalibAlgs/ISCT_CalibHistoTool.h"

//Gaudi includes
#include "GaudiKernel/ToolHandle.h"

//STL includes
#include <string>

//fwd declarations

namespace InDetDD {
  class  SCT_DetectorManager;
}
class StatusCode;
class ISvcLocator;
class IdentifierHash;

class SCT_CalibLbTool : public extends<AthAlgTool, ISCT_CalibHistoTool>
{
   
 public:
  //@name Tool methods, reimplemented
  //@{
  SCT_CalibLbTool(const std::string&, const std::string&, const IInterface*);
  virtual ~SCT_CalibLbTool() = default;
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  //@}
  //@name ISCT_CalibHistoTool methods, reimplemented
  //@{
  virtual bool book();
  virtual bool fill(const bool fromData=false);
  virtual bool read(const std::string& fileName);
  virtual bool fillFromData();
  //@}

 private:
  ToolHandle<ISCT_CalibEvtInfo> m_evtInfo{this, "SCT_CalibEvtInfo", "SCT_CalibEvtInfo"};

  const SCT_ID* m_pSCTHelper{nullptr};
  const InDetDD::SCT_DetectorManager* m_pManager{nullptr};
  SCT_ID::const_id_iterator m_waferItrBegin;
  SCT_ID::const_id_iterator m_waferItrEnd;
  typedef std::vector<int> VecInt;
  VecInt* m_sct_waferHash{nullptr};
  VecInt* m_sct_firstStrip{nullptr};
  VecInt* m_sct_rdoGroupSize{nullptr};

  int m_lumiBlock{0};

  //private use in this class
  int m_LbRange{0};
  int m_LbsToMerge{0};

  SG::ReadHandleKey<SCT_RDO_Container> m_rdoContainerKey{this, "RDOContainer", "SCT_RDOs"};

  void fillLbForWafer(const IdentifierHash& waferHash, const int theFirstStrip, const int groupSize);

};
#endif
