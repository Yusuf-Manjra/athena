#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from TriggerMenuMT.HLTMenuConfig.Menu import EventBuildingInfo
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, MenuSequence
from TrigPartialEventBuilding.TrigPartialEventBuildingConf import PEBInfoWriterAlg
from TrigPartialEventBuilding.TrigPartialEventBuildingConfig import StaticPEBInfoWriterToolCfg, RoIPEBInfoWriterToolCfg
from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
from AthenaCommon.CFElements import seqAND, findAlgorithm
from AthenaCommon.Logging import logging
log = logging.getLogger('EventBuildingSequenceSetup')


def addEventBuildingSequence(chain, eventBuildType):
    '''
    Add an extra ChainStep to a Chain object with a PEBInfoWriter sequence configured for the eventBuildType
    '''
    if not eventBuildType:
        log.error('No eventBuildType specified')
        return
    if eventBuildType not in EventBuildingInfo.getAllEventBuildingIdentifiers():
        log.error('eventBuildType \'%s\' not found in the allowed Event Building identifiers', eventBuildType)
        return

    def pebInfoWriterToolGenerator(chainDict):
        return pebInfoWriterTool(chainDict['chainName'], eventBuildType)

    inputMaker = pebInputMaker(eventBuildType)
    seq = MenuSequence(
        Sequence    = pebSequence(eventBuildType, inputMaker),
        Maker       = inputMaker,
        Hypo        = PEBInfoWriterAlg('PEBInfoWriterAlg_' + eventBuildType),
        HypoToolGen = pebInfoWriterToolGenerator)

    step_name = 'Step{:d}_PEBInfoWriter_{:s}'.format(len(chain.steps)+1, eventBuildType)
    step = ChainStep(name=step_name, Sequences=[seq])
    chain.steps.append(step)


def pebInfoWriterTool(name, eventBuildType):
    '''
    Create PEBInfoWriterTool configuration for the eventBuildType
    '''
    tool = None
    if 'LArPEB' in eventBuildType:
        tool = RoIPEBInfoWriterToolCfg(name)
        tool.DetNames = ['PIXEL', 'SCT', 'TRT', 'TTEM', 'TTHEC', 'FCALEM', 'FCALHAD']
        # TODO: tool.MaxRoIsPerEvent = 5
        tool.addHLTResultToROBList()  # add the main (full) HLT result to the list
        tool.addCTPResultToROBList()  # add the CTP result to the list
    elif 'RPCPEBSecondaryReadout' in eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.ROBList = [0x610080, 0x620080]

    # Name not matched
    if not tool:
        log.error('PEBInfoWriterTool configuration is missing for event building identifier \'%s\'', eventBuildType)
        return None

    # Add Data Scouting HLT result ROB
    if eventBuildType in EventBuildingInfo.getAllDataScoutingResultIDs():
        moduleId = EventBuildingInfo.getAllDataScoutingResultIDs()[eventBuildType]
        tool.addHLTResultToROBList(moduleId)

    return tool


def pebInputMaker(eventBuildType):
    maker = InputMakerForRoI("IMpeb_"+eventBuildType, mergeOutputs=False)
    maker.RoIs = "pebInputRoI_" + eventBuildType
    return maker


def pebSequence(eventBuildType, inputMaker):
    # If a Configurable with the same name already exists, the below call
    # returns the existing one. We add the inputMaker to the sequence only if
    # it's not already there (i.e. if the sequence didn't exist before)
    seq = seqAND("pebSequence_"+eventBuildType)
    if findAlgorithm(seq, inputMaker.name()) != inputMaker:
        seq += inputMaker
    return seq
