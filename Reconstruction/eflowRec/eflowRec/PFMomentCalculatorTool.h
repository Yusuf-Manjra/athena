/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFMOMENTCALCULATORTOOL_H
#define PFMOMENTCALCULATORTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "eflowRec/IPFBaseTool.h"
#include "eflowRec/IPFClusterCollectionTool.h"

class eflowCaloObjectContainer;

/**
This tool uses a CaloClusterCollectionProcessor to calculate new moments for the modified calorimeter clusters (modified when we do the charged shower subtraction), and makes use of IPFClusterCollectionTool to generate a VIEW container of xAOD::CaloCluster to be used in the CaloClusterCollectionProcessor tool. Inherits from IPFBaseAlgTool.
*/
class PFMomentCalculatorTool : public extends<AthAlgTool, IPFBaseTool> {

  public:
  
  PFMomentCalculatorTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~PFMomentCalculatorTool() {}

  StatusCode initialize();
  void execute(const eflowCaloObjectContainer& theEflowCaloObjectContainer,xAOD::CaloClusterContainer& theCaloClusterContainer);
  StatusCode finalize();

 private:

  /** Tool to put all clusters into a temporary container - then we use this to calculate moments, some of which depend on configuration of nearby clusters */
  ToolHandle<IPFClusterCollectionTool> m_clusterCollectionTool{this,"PFClusterCollectionTool","eflowRecClusterCollectionTool","Tool to put all clusters into a temporary container - then we use this to calculate moments, some of which depend on configuration of nearby clusters"};
  
  /** Tool to calculate cluster moments */
  ToolHandle<CaloClusterCollectionProcessor> m_clusterMomentsMaker{this,"CaloClusterMomentsMaker","CaloClusterMomentsMaker","Tool to calculate cluster moments"};

  /** Toggle whether we are in LC mode - false by default */
  Gaudi::Property<bool> m_LCMode{this,"LCMode",false,"Toggle whether we are in LC mode - false by default"};

};

#endif
