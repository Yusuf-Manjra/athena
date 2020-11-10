/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JSUPERCELLTOWERMAPPER_H
#define JSUPERCELLTOWERMAPPER_H

// STL
#include <string>

// Athena/Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IjSuperCellTowerMapper.h"
#include "CaloEvent/CaloCellContainer.h"
#include "L1CaloFEXSim/jTower.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class CaloIdManager;

namespace LVL1 {


class jSuperCellTowerMapper: public AthAlgTool, virtual public IjSuperCellTowerMapper
{
 public:
  jSuperCellTowerMapper(const std::string& type,const std::string& name,const IInterface* parent);
  virtual ~jSuperCellTowerMapper();

  /** standard Athena-Algorithm method */
  virtual StatusCode initialize() override;
  
  virtual StatusCode AssignSuperCellsToTowers(/*jTowerContainer**/std::unique_ptr<jTowerContainer> & my_jTowerContainerRaw) override;
  virtual StatusCode AssignTriggerTowerMapper(/*jTowerContainer**/std::unique_ptr<jTowerContainer> & my_jTowerContainerRaw) override;
  
  virtual void reset() override;

 private:

  SG::ReadHandleKey<CaloCellContainer> m_scellsCollectionSGKey {this, "SCell", "SCell", "SCell"};
  SG::ReadHandleKey<xAOD::TriggerTowerContainer> m_triggerTowerCollectionSGKey {this, "xODTriggerTowers", "xAODTriggerTowers", "xAODTriggerTowers"};

  virtual int FindAndConnectTower(/*jTowerContainer**/std::unique_ptr<jTowerContainer> & my_jTowerContainerRaw,CaloSampling::CaloSample sample,const int region, int layer, const int pos_neg, const int eta_index, const int phi_index, Identifier ID, float et, int prov, bool doPrint) override;
  virtual void ConnectSuperCellToTower(/*jTowerContainer**/std::unique_ptr<jTowerContainer> & my_jTowerContainerRaw, int iETower, Identifier ID, int iCell, float et, int layer, bool doenergysplit) override;
  virtual int FindTowerIDForSuperCell(int towereta, int towerphi) override;
  virtual void PrintCellSpec(const CaloSampling::CaloSample sample, int layer, const int region, const int eta_index, const int phi_index, const int pos_neg, int iETower, int iCell, int prov, Identifier ID, bool doenergysplit) override;

};

} // end of LVL1 namespace
#endif
