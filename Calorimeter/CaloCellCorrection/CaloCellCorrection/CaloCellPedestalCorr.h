/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOCELLPEDESTALCORR_H
#define CALOREC_CALOCELLPEDESTALCORR_H

#include "CaloUtils/CaloCellCorrection.h"
#include "CaloIdentifier/CaloGain.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/DataHandle.h"
#include "CaloCondBlobObjs/ICaloCoolIdTool.h"
#include "CaloInterface/ICaloLumiBCIDTool.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/ToolHandle.h"
#include <unordered_map>

class CaloCondBlobFlt;
class CaloCell;
class CaloCell_ID;

class CaloCellPedestalCorr : public CaloCellCorrection

{

public:

  CaloCellPedestalCorr(const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~CaloCellPedestalCorr() {};

  virtual StatusCode initialize() override;

  void MakeCorrection (CaloCell* theCell,
                       const EventContext& ctx) const override;

private:

  //=== callback function for luminosity storate
  virtual StatusCode updateLumi(IOVSVC_CALLBACK_ARGS);

  virtual StatusCode updateMap(IOVSVC_CALLBACK_ARGS);
  //=== blob storage
  const DataHandle<CondAttrListCollection> m_noiseAttrListColl;
  typedef std::unordered_map<unsigned int, const CaloCondBlobFlt*> NoiseBlobMap_t;
  NoiseBlobMap_t m_noiseBlobMap;

  ToolHandle<ICaloCoolIdTool> m_caloCoolIdTool;
  float m_lumi0;
  std::string m_folderName;
  std::string m_lumiFolderName;

  const CaloCell_ID* m_cellId;

  ToolHandle<ICaloLumiBCIDTool> m_caloLumiBCIDTool;
 
  bool m_isMC;
};

#endif
