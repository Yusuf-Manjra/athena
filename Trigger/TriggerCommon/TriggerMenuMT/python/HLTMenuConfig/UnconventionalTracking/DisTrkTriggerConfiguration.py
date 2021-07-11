# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.CFElements import seqAND
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


def DisTrkTriggerHypoSequence():
        from TrigLongLivedParticlesHypo.TrigDisappearingTrackTriggerHypoTool import  TrigDisappearingTrackTriggerHypoToolFromDict
        from TrigLongLivedParticlesHypo.TrigLongLivedParticlesHypoConf       import (TrigDisappearingTrackTriggerHypoAlg)

        # Setup the hypothesis algorithm
        theDisTrkTrackTriggerHypo = TrigDisappearingTrackTriggerHypoAlg("DisTrkTrack")

        from TrigEDMConfig.TriggerEDMRun3 import recordable
        theDisTrkTrackTriggerHypo.DisTrkBDTSel = recordable("HLT_DisTrkBDTSel")

        from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
        from AthenaConfiguration.ComponentFactory import CompFactory
        DummyInputMakerAlg = conf2toConfigurable(CompFactory.InputMakerForRoI( "IM_DisTrkTrack_HypoOnlyStep" ))
        DummyInputMakerAlg.RoITool = conf2toConfigurable(CompFactory.ViewCreatorInitialROITool())

        log.debug("Building the Step dictinary for DisTrk")
        return MenuSequence( Sequence    = seqAND("DisTrkEmptyStep",[DummyInputMakerAlg]),
                             Maker       = DummyInputMakerAlg,
                             Hypo        = theDisTrkTrackTriggerHypo,
                             HypoToolGen = TrigDisappearingTrackTriggerHypoToolFromDict,
                         )
