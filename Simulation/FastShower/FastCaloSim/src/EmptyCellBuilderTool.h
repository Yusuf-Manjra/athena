/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTCALOSIM_EMPTYCELLBUILDERTOOL_H
#define FASTCALOSIM_EMPTYCELLBUILDERTOOL_H

/**
 * @file   CellBuilderTool.cxx
 * @class  CellBuilderTool
 * @brief  Building Cells objects from Atlfast
 * @author Michael Duehrssen 
 */

#include "FastCaloSim/BasicCellBuilderTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

class EmptyCellBuilderTool: public BasicCellBuilderTool
{
public:    
  EmptyCellBuilderTool(const std::string& type, 
		       const std::string& name, 
		       const IInterface* parent);
  
  virtual StatusCode initialize() override;

  // update theCellContainer
  virtual StatusCode process (CaloCellContainer * theCellContainer,
                              const EventContext& ctx) const override;
private:
  void create_empty_calo(const EventContext& ctx,
                         CaloCellContainer * theCellContainer) const;


  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };
};

#endif
