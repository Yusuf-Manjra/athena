# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


def dEdxTriggerHypoSequence(flags):
        from TrigLongLivedParticlesHypo.TrigdEdxTrackHypoConfig import TrigdEdxTrackHypoToolFromDict
        from TrigLongLivedParticlesHypo.TrigdEdxTrackHypoConfig import createTrigdEdxTrackHypoAlg
        
        thedEdxTrackHypo = createTrigdEdxTrackHypoAlg(flags, "dEdxTrack")

        from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
        from AthenaConfiguration.ComponentFactory import CompFactory
        DummyInputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_dEdxTrack_HypoOnlyStep" ))
        DummyInputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())

        return MenuSequence( flags,
                             Sequence    = seqAND("dEdxEmptyStep",[DummyInputMakerAlg]),
                             Maker       = DummyInputMakerAlg,
                             Hypo        = thedEdxTrackHypo,
                             HypoToolGen = TrigdEdxTrackHypoToolFromDict,
                     )
