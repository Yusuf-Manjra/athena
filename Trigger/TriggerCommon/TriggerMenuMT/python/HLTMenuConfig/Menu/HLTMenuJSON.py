# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import json
from collections import OrderedDict as odict
from TrigConfigSvc.TrigConfigSvcCfg import getHLTMenuFileName

from AthenaCommon.Logging import logging
__log = logging.getLogger( __name__ )

def __getStepsDataFromAlgSequence(HLTAllSteps):
    """ Generates a list where the index corresponds to a Step number and the stored object is a list of Sequencers making up the Step 
    """
    stepsData = []
    if HLTAllSteps is not None:
        for HLTStep in HLTAllSteps.getChildren():
            if "_reco" not in HLTStep.name(): # Avoid the pre-step Filter execution
                # Look for newJO reco
                for Step in HLTStep.getChildren():
                    for View in Step.getChildren():
                        for Reco in View.getChildren():
                            if "_reco" in Reco.name() and HLTStep.name() not in stepsData:
                                stepsData.append( HLTStep.getChildren() )
                                break
                continue

            stepsData.append( HLTStep.getChildren() )
    else:
        __log.warn( "No HLTAllSteps sequencer, will not export per-Step data for chains.")
    return stepsData

def __getChainSequencers(stepsData, chainName):
    """ Finds the Filter which is responsible for this Chain in each Step.
        Return a list of the per-Step name() of the Sequencer which is unlocked by the Chain's Filter in the Step.
    """
    sequencers = []
    counter = 0
    from DecisionHandling.TrigCompositeUtils import chainNameFromLegName
    for step in stepsData:
        counter += 1
        mySequencer = None
        endOfChain = False
        for sequencer in step:
            sequencerFilter = sequencer.getChildren()[0] # Always the first child in the step
            if any(chainName in chainNameFromLegName(fChain) for fChain in sequencerFilter.Chains):
                if mySequencer is not None:
                    __log.error( "Multiple Filters found (corresponding Sequencers %s, %s) for %s in Step %i!",
                        mySequencer.name(), sequencer.name(), chainName, counter)
                mySequencer = sequencer
        if mySequencer is None:
            endOfChain = True
            if counter == 1 and  'noalg' not in chainName:
                __log.warn("No Filter found for %s in Step 1", chainName)
        else:
            if endOfChain is True:
                __log.error( "Found another Step, (Step %i) for chain %s "
                    "which looked like it had already finished after %i Steps!", 
                    counter, chainName, sequencers.len())
            sequencers.append(mySequencer.name())
    return sequencers

def __getSequencerAlgs(stepsData):
    """ For each Sequencer in each Step, return a flat list of the full name of all Algorithms under the Sequencer
    """
    from AthenaCommon.CFElements import findAllAlgorithms
    sequencerAlgs = odict()
    for step in stepsData:
        for sequencer in step:
            sequencerAlgs[ sequencer.name() ] = list(map(lambda x: x.getFullName(), findAllAlgorithms(sequencer)))
    return sorted(sequencerAlgs.items(), key=lambda t: t[0])

def __generateJSON( chainDicts, chainConfigs, HLTAllSteps, menuName, fileName ):
    """ Generates JSON given the ChainProps and sequences
    """
    menuDict = odict([ ("filetype", "hltmenu"), ("name", menuName), ("chains", []), ("sequencers", odict()) ])

    # List of steps data
    stepsData = __getStepsDataFromAlgSequence(HLTAllSteps)

    from TriggerMenuMT.HLTMenuConfig.Menu import StreamInfo
    for chain in chainDicts:
        streamDicts = []
        streamTags = StreamInfo.getStreamTags(chain["stream"])
        for streamTag in streamTags:
            streamDicts.append({"name": streamTag.name(),
                                "type": streamTag.type(),
                                "obeyLB": streamTag.obeysLumiBlock(),
                                "forceFullEventBuilding": streamTag.forceFullEventBuilding()})

        l1Thresholds  = []
        [ l1Thresholds.append(p['L1threshold']) for p in chain['chainParts'] ]

        chainDict = odict([ 
            ("counter", chain["chainCounter"]),
            ("name", chain["chainName"]),
            ("nameHash", chain["chainNameHash"]),
            ("l1item", chain["L1item"]),
            ("l1thresholds", l1Thresholds),
            ("groups", chain["groups"]),
            ("streams", streamDicts),
            ("sequencers", __getChainSequencers(stepsData, chain["chainName"]) )
        ])

        menuDict["chains"].append( chainDict )

    # All algorithms executed by a given Sequencer
    menuDict["sequencers"].update( __getSequencerAlgs(stepsData) )

    __log.info( "Writing trigger menu to %s", fileName )
    with open( fileName, 'w' ) as fp:
        json.dump( menuDict, fp, indent=4, sort_keys=False )


def generateJSON():
    __log.info("Generating HLT JSON config in the rec-ex-common job")
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.CFElements import findSubSequence

    return __generateJSON( TriggerConfigHLT.dictsList(), 
                           TriggerConfigHLT.configsList(), 
                           findSubSequence(AlgSequence(), "HLTAllSteps"),
                           TriggerFlags.triggerMenuSetup(),
                           getHLTMenuFileName() )
    
def generateJSON_newJO( chainDicts, chainConfigs, HLTAllSteps ):
    __log.info("Generating HLT JSON config in the new JO")
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    return __generateJSON( chainDicts, 
                           chainConfigs, 
                           HLTAllSteps,
                           ConfigFlags.Trigger.triggerMenuSetup, 
                           getHLTMenuFileName( ConfigFlags) )
