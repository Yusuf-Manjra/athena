/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULTBYTESTREAM_L1TRIGGERRESULTMAKER_H
#define TRIGT1RESULTBYTESTREAM_L1TRIGGERRESULTMAKER_H

// Trigger includes
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/TrigCompositeContainer.h"

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

/** @class L1TriggerResultMaker
 *  @brief Algorithm creating L1TriggerResult and linking the relevant L1 xAOD collections to it
 **/
class L1TriggerResultMaker : public AthReentrantAlgorithm {
public:
  /// Standard constructor
  L1TriggerResultMaker(const std::string& name, ISvcLocator* svcLoc);

  // ------------------------- AthReentrantAlgorithm methods -------------------
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& eventContext) const override;

private:
  // ------------------------- Properties --------------------------------------
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_l1TriggerResultWHKey {
    this, "L1TriggerResultWHKey", "L1TriggerResult", "Key of the output L1 Trigger Result"};

  // Muon RoIs
  SG::ReadHandleKey<xAOD::MuonRoIContainer> m_muRoIKey {
    this, "MuRoIKey", "LVL1MuonRoIs", "Key of the muon RoI container to be linked to L1 Trigger Result"};
  Gaudi::Property<std::string> m_muRoILinkName {
    this, "MuRoILinkName", "mu_roi", "Name of the link to be created from L1 Trigger Result to muon RoI container"};

  // Placeholder for other L1 xAOD outputs:
  // - CTP result
  // - L1Topo result
  // - L1Calo (Run3) RoIs
};

#endif // TRIGT1RESULTBYTESTREAM_L1TRIGGERRESULTMAKER_H
