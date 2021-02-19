#include "TrigCaloRec/TrigL1FCALTTSumFex.h"

#include "TrigCaloRec/TrigCaloClusterMaker.h"
#include "TrigCaloRec/TrigCaloTowerMaker.h"
#include "TrigCaloRec/TrigFullCaloCellMaker.h"
#include "TrigCaloRec/TrigFullCaloClusterMaker.h"
#include "TrigCaloRec/TrigLArNoisyROAlg.h"
#include "TrigCaloRec/TrigL1BSTowerMaker.h"
#include "TrigCaloRec/TrigL1BSTowerHypo.h"

#include "../TrigCaloClusterMakerMT.h"
#include "../TrigCaloTowerMakerMT.h"
#include "../TrigCaloClusterCalibratorMT.h"

#include "TrigCaloRec/TrigCaloTopoTowerAlgorithm.h"
#include "TrigCaloRec/TrigCaloCell2ClusterMapper.h"
#include "../HLTCaloCellMaker.h"
#include "../HLTCaloCellSumMaker.h"

DECLARE_COMPONENT( TrigCaloTowerMakerMT )
DECLARE_COMPONENT( TrigCaloClusterCalibratorMT )
DECLARE_COMPONENT( TrigCaloClusterMakerMT )
DECLARE_COMPONENT( TrigFullCaloCellMaker )
DECLARE_COMPONENT( TrigCaloTowerMaker )
DECLARE_COMPONENT( TrigCaloClusterMaker )
DECLARE_COMPONENT( TrigCaloTopoTowerAlgorithm )
DECLARE_COMPONENT( TrigCaloCell2ClusterMapper )
DECLARE_COMPONENT( TrigFullCaloClusterMaker )
DECLARE_COMPONENT( TrigLArNoisyROAlg )
DECLARE_COMPONENT( TrigL1BSTowerMaker )
DECLARE_COMPONENT( TrigL1BSTowerHypo )
DECLARE_COMPONENT( TrigL1FCALTTSumFex )
DECLARE_COMPONENT( HLTCaloCellMaker )
DECLARE_COMPONENT( HLTCaloCellSumMaker )

