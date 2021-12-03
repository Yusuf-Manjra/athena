#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
from AthenaCommon.CFElements import parOR
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool
from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import TrigEgammaRec, TrigEgammaSuperClusterBuilder 
from AthenaCommon.Logging import logging

log = logging.getLogger(__name__)

# Dummy flag arg needed so that each reco sequence is held separately
# in the RecoFragmentsPool -- only the RoIs are used to distinguish
# different sequences. New convention is just to pass "None" for flags
# taken from Jet/JetRecoSequences.py
def precisionCaloRecoSequence(DummyFlag, RoIs, ion=False, variant=''):
    log.debug('DummyFlag = %s',str(DummyFlag))
    log.debug('RoIs = %s',RoIs)

    from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaKeys import  getTrigEgammaKeys
    TrigEgammaKeys = getTrigEgammaKeys(variant, ion=ion)

    from TrigT2CaloCommon.CaloDef import HLTRoITopoRecoSequence, HLTHIRoITopoRecoSequence
    topoRecoSequence = HLTHIRoITopoRecoSequence if ion is True else HLTRoITopoRecoSequence
    (caloRecoSequence, caloclusters) = RecoFragmentsPool.retrieve(topoRecoSequence, None, RoIs=RoIs, algSuffix=variant)

    tag = 'HI' if ion is True else '' 
    outputCaloClusters = TrigEgammaKeys.precisionCaloClusterContainer
    log.debug('precisionOutputCaloClusters = %s',outputCaloClusters)

    egammaTopoClusterCopier = AlgFactory( egammaAlgsConf.egammaTopoClusterCopier,
                                          name = 'eTrigEgammaTopoClusterCopier' + tag + RoIs + variant ,
                                          InputTopoCollection = caloclusters,
                                          OutputTopoCollection = TrigEgammaKeys.precisionCaloTopoCollection,
                                          OutputTopoCollectionShallow = "tmp_" + TrigEgammaKeys.precisionCaloTopoCollection,
                                          doAdd = False )

    algo = egammaTopoClusterCopier()
    precisionRecoSequence = parOR( "electronRoITopoRecoSequence"+tag + variant)
    precisionRecoSequence += caloRecoSequence
    precisionRecoSequence += algo
    trigEgammaRec = TrigEgammaRec(name = 'eTrigEgammaRec' + tag + RoIs + variant)
    precisionRecoSequence += trigEgammaRec
    trigEgammaSuperClusterBuilder = TrigEgammaSuperClusterBuilder(name = 'eTrigEgammaSuperClusterBuilder' + tag + RoIs + variant)
    trigEgammaSuperClusterBuilder.SuperClusterCollectionName = outputCaloClusters
    trigEgammaSuperClusterBuilder.CalibrationType = 'electron'
    precisionRecoSequence +=  trigEgammaSuperClusterBuilder
    sequenceOut = outputCaloClusters

    return (precisionRecoSequence, sequenceOut)

