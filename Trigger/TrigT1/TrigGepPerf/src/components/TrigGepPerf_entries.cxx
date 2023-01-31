/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include "../GepClusteringAlg.h"
DECLARE_COMPONENT( GepClusteringAlg )

#include "../GepJetAlg.h"
DECLARE_COMPONENT( GepJetAlg )

#include "../GepMETAlg.h"
DECLARE_COMPONENT( GepMETAlg )

#include "../GepMETPufitAlg.h"
DECLARE_COMPONENT( GepMETPufitAlg )

#include "../GepClusterTimingAlg.h"
DECLARE_COMPONENT( GepClusterTimingAlg )

#include "../CaloCellsHandlerTool.h"
DECLARE_COMPONENT( CaloCellsHandlerTool )

#include "../GepPi0Alg.h"
DECLARE_COMPONENT(GepPi0Alg)

#include "../EMB1CellsFromCaloCells.h"
DECLARE_COMPONENT(EMB1CellsFromCaloCells)

#include "../EMB1CellsFromCaloClusters.h"
DECLARE_COMPONENT(EMB1CellsFromCaloClusters)
