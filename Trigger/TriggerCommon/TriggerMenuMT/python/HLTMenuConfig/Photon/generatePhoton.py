# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Electron.ElectronRecoSequences import l2CaloRecoCfg, l2CaloHypoCfg
from TriggerMenuMT.HLTMenuConfig.Photon.PhotonRecoSequences import l2PhotonRecoCfg, l2PhotonHypoCfg
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CAMenuSequence, \
    ChainStep, Chain, getChainStepName, createStepView
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromDict
from TrigEgammaHypo.TrigL2PhotonHypoTool import TrigL2PhotonHypoToolFromDict
from TrigEDMConfig.TriggerEDMRun3 import recordable

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Photon.generatePhoton' )

def generateChains(flags, chainDict):

    firstStepName = getChainStepName('Photon', 1)
    stepReco, stepView = createStepView(firstStepName)

    accCalo = ComponentAccumulator()
    accCalo.addSequence(stepView)

    l2CaloReco = l2CaloRecoCfg(flags)
    accCalo.merge(l2CaloReco, sequenceName=stepReco.getName())

    l2CaloHypo = l2CaloHypoCfg( flags,
                                name = 'L2PhotonCaloHypo',
                                CaloClusters = recordable('HLT_L2CaloEMClusters') )

    accCalo.addEventAlgo(l2CaloHypo, sequenceName=stepView.getName())

    fastCaloSequence = CAMenuSequence( Sequence = l2CaloReco.sequence(),
                                     Maker = l2CaloReco.inputMaker(),
                                     Hypo = l2CaloHypo,
                                     HypoToolGen = TrigL2CaloHypoToolFromDict,
                                     CA = accCalo )

    fastCaloSequence.createHypoTools(chainDict)

    fastCaloStep = ChainStep(firstStepName, [fastCaloSequence])


    secondStepName = getChainStepName('Photon', 2)
    stepReco, stepView = createStepView(secondStepName)

    accPhoton = ComponentAccumulator()
    accPhoton.addSequence(stepView)

    l2PhotonReco = l2PhotonRecoCfg(flags)
    accPhoton.merge(l2PhotonReco, sequenceName=stepReco.getName())

    l2PhotonHypo = l2PhotonHypoCfg( flags,
                                    Photons = 'HLT_L2Photons',
                                    RunInView = True )

    accPhoton.addEventAlgo(l2PhotonHypo, sequenceName=stepView.getName())

    l2PhotonSequence = CAMenuSequence( Sequence = l2PhotonReco.sequence(),
                                     Maker = l2PhotonReco.inputMaker(),
                                     Hypo = l2PhotonHypo,
                                     HypoToolGen = TrigL2PhotonHypoToolFromDict,
                                     CA = accPhoton )

    l2PhotonSequence.createHypoTools(chainDict)

    l2PhotonStep = ChainStep(secondStepName, [l2PhotonSequence])


    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])

    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))


    chain = Chain(chainDict['chainName'], L1Thresholds=l1Thresholds, ChainSteps=[fastCaloStep, l2PhotonStep])
    return chain
