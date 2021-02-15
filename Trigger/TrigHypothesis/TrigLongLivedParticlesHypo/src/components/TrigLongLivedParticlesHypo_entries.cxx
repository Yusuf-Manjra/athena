#include "TrigLongLivedParticlesHypo/TrigL2HVJetHypoAllCuts.h"
#include "TrigLongLivedParticlesHypo/TrigL2HVJetHypo.h"
#include "TrigLongLivedParticlesHypo/TrigL2HVJetHypoTrk.h"
#include "TrigLongLivedParticlesHypo/MuonClusterHypo.h"
#include "TrigLongLivedParticlesHypo/MuonClusterIsolationHypo.h"
#include "TrigLongLivedParticlesHypo/TrigLoFRemovalHypo.h"
#include "TrigLongLivedParticlesHypo/TrigNewLoFHypo.h"
#include "TrigLongLivedParticlesHypo/TrigCaloRatioHypo.h"
#include "TrigLongLivedParticlesHypo/TrigLLPInnerDetectorHypo.h"
#include "../TrigIsoHPtTrackTriggerHypoAlgMT.h"
#include "../TrigIsoHPtTrackTriggerHypoTool.h"


DECLARE_COMPONENT (TrigIsoHPtTrackTriggerHypoAlgMT )
DECLARE_COMPONENT (TrigIsoHPtTrackTriggerHypoTool )

#include "TrigLongLivedParticlesHypo/FastTrackFinderLRTHypoTool.h"
#include "TrigLongLivedParticlesHypo/FastTrackFinderLRTHypoAlg.h"

DECLARE_COMPONENT( TrigL2HVJetHypoAllCuts )
DECLARE_COMPONENT( TrigL2HVJetHypo )
DECLARE_COMPONENT( TrigL2HVJetHypoTrk )
DECLARE_COMPONENT( MuonClusterHypo )
DECLARE_COMPONENT( MuonClusterIsolationHypo )
DECLARE_COMPONENT( TrigLoFRemovalHypo )
DECLARE_COMPONENT( TrigNewLoFHypo )
DECLARE_COMPONENT( TrigCaloRatioHypo )

DECLARE_COMPONENT( FastTrackFinderLRTHypoTool )
DECLARE_COMPONENT( FastTrackFinderLRTHypoAlg )
