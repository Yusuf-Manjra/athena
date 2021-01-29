#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from TrigEDMConfig import DataScoutingInfo
from TriggerMenuMT.HLTMenuConfig.Menu import EventBuildingInfo
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, MenuSequence
from TrigPartialEventBuilding.TrigPartialEventBuildingConf import PEBInfoWriterAlg
from TrigPartialEventBuilding.TrigPartialEventBuildingConfig import StaticPEBInfoWriterToolCfg, RoIPEBInfoWriterToolCfg
from DecisionHandling import DecisionHandlingConf
from libpyeformat_helper import SourceIdentifier, SubDetector
from AthenaCommon.CFElements import seqAND, findAlgorithm
from AthenaCommon.Logging import logging
log = logging.getLogger('EventBuildingSequenceSetup')


def addEventBuildingSequence(chain, eventBuildType, chainDict):
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

    inputMaker = pebInputMaker(chain, eventBuildType)
    seq = MenuSequence(
        Sequence    = pebSequence(eventBuildType, inputMaker),
        Maker       = inputMaker,
        Hypo        = PEBInfoWriterAlg('PEBInfoWriterAlg_' + eventBuildType),
        HypoToolGen = pebInfoWriterToolGenerator)

    step_name = 'Step{:d}_PEBInfoWriter_{:s}'.format(len(chain.steps)+1, eventBuildType)
    step = ChainStep(name=step_name, Sequences=[seq], chainDicts=[chainDict], comboHypoCfg=None)
    chain.steps.append(step)


def pebInfoWriterTool(name, eventBuildType):
    '''
    Create PEBInfoWriterTool configuration for the eventBuildType
    '''
    tool = None
    if 'BeamSpotPEB' in eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addSubDets([
            SubDetector.PIXEL_BARREL,
            SubDetector.PIXEL_DISK_SIDE, # note different name in C++, ADHI-4753
            SubDetector.PIXEL_B_LAYER,
            SubDetector.PIXEL_IBL,
            SubDetector.SCT_BARREL_A_SIDE,
            SubDetector.SCT_BARREL_C_SIDE,
            SubDetector.SCT_ENDCAP_A_SIDE,
            SubDetector.SCT_ENDCAP_C_SIDE,
            SubDetector.TDAQ_CTP
        ])
    elif 'LArPEB' in eventBuildType:
        tool = RoIPEBInfoWriterToolCfg(name)
        tool.addRegSelDets(['Pixel', 'SCT', 'TRT', 'TTEM', 'TTHEC', 'FCALEM', 'FCALHAD'])
        tool.MaxRoIs = 5
        tool.addHLTResultToROBList()  # add the main (full) HLT result to the list
        tool.addCTPResultToROBList()  # add the CTP result to the list
    elif 'RPCPEBSecondaryReadout' in eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addROBs([0x610080, 0x620080])
    elif 'SCTPEB' in eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addSubDets([SubDetector.SCT_BARREL_A_SIDE,
                         SubDetector.SCT_BARREL_C_SIDE,
                         SubDetector.SCT_ENDCAP_A_SIDE,
                         SubDetector.SCT_ENDCAP_C_SIDE
        ])
    elif 'TilePEB' in eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addSubDets([SubDetector.TILECAL_LASER_CRATE,
                         SubDetector.TILECAL_BARREL_A_SIDE,
                         SubDetector.TILECAL_BARREL_C_SIDE,
                         SubDetector.TILECAL_EXT_A_SIDE,
                         SubDetector.TILECAL_EXT_C_SIDE,
                         SubDetector.TDAQ_CTP,
                         SubDetector.TDAQ_CALO_PREPROC, # = 0x71
                         SubDetector.TDAQ_CALO_CLUSTER_PROC_DAQ, # = 0x72
                         SubDetector.TDAQ_CALO_CLUSTER_PROC_ROI, # = 0x73
                         SubDetector.TDAQ_CALO_JET_PROC_DAQ, # = 0x74
                         SubDetector.TDAQ_CALO_JET_PROC_ROI # = 0x75
        ])
    elif 'AlfaPEB' in eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addSubDets([SubDetector.FORWARD_ALPHA,
                         SubDetector.TDAQ_CTP
        ])
    elif 'CSCPEB' in eventBuildType:
        tool = StaticPEBInfoWriterToolCfg(name)
        tool.addSubDets([
            SubDetector.MUON_CSC_ENDCAP_A_SIDE,
            SubDetector.MUON_CSC_ENDCAP_C_SIDE
         ])

    elif eventBuildType in DataScoutingInfo.getAllDataScoutingIdentifiers():
        # Pure DataScouting configuration
        tool = StaticPEBInfoWriterToolCfg(name)
        moduleId = DataScoutingInfo.getDataScoutingResultID(eventBuildType)
        tool.addHLTResultToROBList(moduleId)

    # Name not matched
    if not tool:
        log.error('PEBInfoWriterTool configuration is missing for event building identifier \'%s\'', eventBuildType)
        return None

    return tool


def pebInputMaker(chain, eventBuildType):
    maker = DecisionHandlingConf.InputMakerForRoI("IMpeb_"+eventBuildType)
    maker.RoIs = "pebInputRoI_" + eventBuildType
    if len(chain.steps) == 0:
        # Streamers: use initial RoI
        maker.RoITool = DecisionHandlingConf.ViewCreatorInitialROITool()
    else:
        # Other chains: use previous RoI
        maker.RoITool = DecisionHandlingConf.ViewCreatorPreviousROITool()
    return maker


def pebSequence(eventBuildType, inputMaker):
    # If a Configurable with the same name already exists, the below call
    # returns the existing one. We add the inputMaker to the sequence only if
    # it's not already there (i.e. if the sequence didn't exist before)
    seq = seqAND("pebSequence_"+eventBuildType)
    if findAlgorithm(seq, inputMaker.name()) != inputMaker:
        seq += inputMaker
    return seq


def findEventBuildingStep(chainConfig):
    pebSteps = [s for s in chainConfig.steps if 'PEBInfoWriter' in s.name and 'EmptyPEBAlign' not in s.name]
    if len(pebSteps) == 0:
        return None
    elif len(pebSteps) > 1:
        raise RuntimeError('Multiple Event Building steps in one chain are not supported but found in chain ' + chainConfig.name)
    return pebSteps[0]


def alignEventBuildingSteps(chain_configs, chain_dicts):
    def is_peb_dict(chainNameAndDict):
        return len(chainNameAndDict[1]['eventBuildType']) > 0

    all_peb_chain_dicts = dict(filter(is_peb_dict, chain_dicts.items()))
    all_peb_chain_names = list(all_peb_chain_dicts.keys())

    def is_peb_config(chainNameAndConfig):
        return chainNameAndConfig[0] in all_peb_chain_names

    all_peb_chain_configs = dict(filter(is_peb_config, chain_configs.items()))

    maxPebStepPosition = {} # {eventBuildType: N}

    def getPebStepPosition(chainConfig):
        pebStep = findEventBuildingStep(chainConfig)
        return chainConfig.steps.index(pebStep) + 1

    # First loop to find the maximal PEB step positions to which we need to align
    for chainName, chainConfig in all_peb_chain_configs.items():
        pebStepPosition = getPebStepPosition(chainConfig)
        ebt = all_peb_chain_dicts[chainName]['eventBuildType']
        if ebt not in maxPebStepPosition or pebStepPosition > maxPebStepPosition[ebt]:
            maxPebStepPosition[ebt] = pebStepPosition

    # Second loop to insert empty steps before the PEB steps where needed
    for chainName, chainConfig in all_peb_chain_configs.items():
        pebStepPosition = getPebStepPosition(chainConfig)
        ebt = all_peb_chain_dicts[chainName]['eventBuildType']
        if pebStepPosition < maxPebStepPosition[ebt]:
            numStepsNeeded = maxPebStepPosition[ebt] - pebStepPosition
            log.debug('Aligning PEB step for chain %s by adding %d empty steps', chainName, numStepsNeeded)
            chainConfig.insertEmptySteps('EmptyPEBAlign', numStepsNeeded, pebStepPosition-1)


# Unit test
if __name__ == "__main__":
    failures = 0
    for eb_identifier in EventBuildingInfo.getAllEventBuildingIdentifiers():
        tool = None
        try:
            tool = pebInfoWriterTool('TestTool_'+eb_identifier, eb_identifier)
        except Exception as ex:
            failures += 1
            log.error('Caught exception while configuring PEBInfoWriterTool for %s: %s', eb_identifier, ex)
            continue

        if not tool:
            failures += 1
            log.error('No tool created for %s', eb_identifier)
            continue

        if tool.__cpp_type__ not in ['StaticPEBInfoWriterTool', 'RoIPEBInfoWriterTool']:
            failures += 1
            log.error('Unexpected tool type for %s: %s', eb_identifier, tool.__cpp_type__)
            continue

        robs = tool.ROBList if tool.__cpp_type__ == 'StaticPEBInfoWriterTool' else tool.ExtraROBs
        dets = tool.SubDetList if tool.__cpp_type__ == 'StaticPEBInfoWriterTool' else tool.ExtraSubDets
        robs_check_passed = True
        for rob_id in robs:
            rob_sid = SourceIdentifier(rob_id)
            rob_det_id = rob_sid.subdetector_id()
            if int(rob_det_id) in dets:
                robs_check_passed = False
                log.error('Redundant configuration for %s: ROB %s added to the ROB list while full SubDetector '
                          '%s is already in the SubDets list', eb_identifier, rob_sid.human(), str(rob_det_id))

        if not robs_check_passed:
            failures += 1
            continue

        log.info('%s correctly configured', tool.name)

    import sys
    sys.exit(failures)
