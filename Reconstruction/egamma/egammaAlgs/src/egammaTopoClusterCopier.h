/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EGAMMATOPOCLUSTERCOPIER_H
#define EGAMMAALGS_EGAMMATOPOCLUSTERCOPIER_H

#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthContainers/ConstDataVector.h"

#include <Gaudi/Accumulators.h>

class egammaTopoClusterCopier : public AthReentrantAlgorithm {

 public:

  /** @bried constructor */
  egammaTopoClusterCopier (const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief initialize method */
  virtual StatusCode initialize() override final;
  /** @brief execute on container */
  virtual StatusCode execute(const EventContext& ctx) const override final;
  /** @brief finalize method */
  virtual StatusCode finalize() override final;

  private:

  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_inputTopoCollection {this,
      "InputTopoCollection", "CaloTopoCluster", "input topocluster collection"};

  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outputTopoCollectionShallow {this,
      "OutputTopoCollectionShallow", "tmp_egammaTopoCluster",
      "Shallow copy of input collection that allows properties to be modified"};

  SG::WriteHandleKey<ConstDataVector <xAOD::CaloClusterContainer> > m_outputTopoCollection {this,
      "OutputTopoCollection", "egammaTopoCluster"
      "View container of selected topoclusters"};

  Gaudi::Property<float> m_etaCut {this, "EtaCut", 2.6, "maximum |eta| of selected clusters"};
  Gaudi::Property<float> m_ECut {this, "ECut", 700, "minimum energy of selected clusters"};
  Gaudi::Property<float> m_EMFracCut {this, "EMFracCut", 0.5, "mimimum EM fraction"};

  mutable Gaudi::Accumulators::Counter<>                     m_AllClusters;
  mutable Gaudi::Accumulators::Counter<>                     m_PassPreSelection;
  mutable Gaudi::Accumulators::Counter<>                     m_PassSelection;
};

#endif // EGAMMATOOLS_EMCLUSTERTOOL_H
