# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict
from builtins import str
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator,conf2toConfigurable
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.CFElements import seqAND, seqOR, parOR, flatAlgorithmSequences, getSequenceChildren, isSequence, hasProp, getProp
from AthenaCommon.Logging import logging
__log = logging.getLogger('TriggerConfig')
def __isCombo(alg):
    return hasProp( alg, "MultiplicitiesMap" )  # alg.getType() == 'ComboHypo':

def __stepNumber(stepName):
    """extract step number frmo strings like Step2... -> 2"""
    return int(stepName.split('_')[0].replace("Step",""))


def collectHypos( steps ):
    """
    Method iterating over the CF and picking all the Hypothesis algorithms

    Returned is a map with the step name and list of all instances of hypos in that step.
    Input is top HLT sequencer.
    """
    __log.info("Collecting hypos from steps")
    from collections import defaultdict
    hypos = defaultdict( list )

    for stepSeq in getSequenceChildren( steps ):
        if not isSequence( stepSeq ):
            continue

        if "filter" in stepSeq.getName():
            __log.debug("Skipping filtering steps %s", stepSeq.getName() )
            continue

        __log.debug( "collecting hypos from step %s", stepSeq.getName() )
#        start = {}
        for seq,algs in flatAlgorithmSequences(stepSeq).items():
            for alg in sorted(algs, key=lambda t: str(t.name)):
                if isSequence( alg ):
                    continue
                # will replace by function once dependencies are sorted
                if hasProp(alg, 'HypoInputDecisions'):
                    __log.debug("found hypo %s in %s", alg.getName(), stepSeq.getName())
                    if __isCombo( alg ) and len(alg.ComboHypoTools):
                        __log.debug( "    with %d comboHypoTools: %s", len(alg.ComboHypoTools), ' '.join(map(str, [tool.getName() for  tool in alg.ComboHypoTools])))
                    hypos[stepSeq.getName()].append( alg )
                else:
                    __log.verbose("Not a hypo %s", alg.getName())

    return OrderedDict(hypos)

def __decisionsFromHypo( hypo ):
    """ return all chains served by this hypo and the keys of produced decision object """
    from TrigCompositeUtils.TrigCompositeUtils import isLegId
    __log.debug("Hypo type %s is combo %r", hypo.getName(), __isCombo(hypo))    
    if __isCombo(hypo):
        return [key for key in list(hypo.MultiplicitiesMap.keys()) if not isLegId(key)], hypo.HypoOutputDecisions
    else: # regular hypos
        return [ t.getName() for t in hypo.HypoTools if not isLegId(t.getName())], [str(hypo.HypoOutputDecisions)]

def __getSequenceChildrenIfIsSequence( s ):
    if isSequence( s ):
        return getSequenceChildren( s )
    return []

def collectViewMakers( steps ):
    """ collect all view maker algorithms in the configuration """
    makers = [] # map with name, instance and encompasing recoSequence
    for stepSeq in __getSequenceChildrenIfIsSequence( steps ):
        for recoSeq in __getSequenceChildrenIfIsSequence( stepSeq ):
            if not isSequence( recoSeq ):
                continue
            algsInSeq = flatAlgorithmSequences( recoSeq )
            for seq,algs in algsInSeq.items():
                for alg in algs:
                    if "EventViewCreator" in alg.getFullJobOptName(): # TODO base it on checking types of write handles once available
                        if alg not in makers:
                            makers.append(alg)
    __log.debug("Found ViewMakers: %s", ' '.join([ maker.getName() for maker in makers ]))
    return makers



def collectFilters( steps ):
    """
    Similarly to collectHypos but works for filter algorithms

    The logic is simpler as all filters are grouped in step filter sequences
    Returns map: step name -> list of all filters of that step
    """
    __log.info("Collecting filters")
    from collections import defaultdict
    filters = defaultdict( list )

    for stepSeq in getSequenceChildren( steps ):
        if "filter" in stepSeq.getName():
            filters[stepSeq.getName()] = getSequenceChildren( stepSeq )
            __log.debug("Found Filters in Step %s : %s", stepSeq.getName(), getSequenceChildren(stepSeq))

    return filters


def collectL1DecoderDecisionObjects(l1decoder):
    decisionObjects = set()
    decisionObjects.update([ str(d.Decisions) for d in l1decoder.roiUnpackers ])
    from L1Decoder.L1DecoderConfig import mapThresholdToL1DecisionCollection
    decisionObjects.add( mapThresholdToL1DecisionCollection("FSNOSEED") ) # Include also Full Scan
    __log.info("Collecting %i decision objects from L1 decoder instance", len(decisionObjects))
    return decisionObjects

def collectHypoDecisionObjects(hypos, inputs = True, outputs = True):
    decisionObjects = set()
    for step, stepHypos in sorted(hypos.items()):
        for hypoAlg in stepHypos:
            __log.debug( "Hypo %s with input %s and output %s ",
                         hypoAlg.getName(), hypoAlg.HypoInputDecisions, hypoAlg.HypoOutputDecisions )
            if isinstance( hypoAlg.HypoInputDecisions, list):
                if inputs:
                    [ decisionObjects.add( str(d) ) for d in hypoAlg.HypoInputDecisions ]
                if outputs:
                    [ decisionObjects.add( str(d) ) for d in hypoAlg.HypoOutputDecisions ]
            else:
                if inputs:
                    decisionObjects.add( str(hypoAlg.HypoInputDecisions) )
                if outputs:
                    decisionObjects.add( str(hypoAlg.HypoOutputDecisions) )
    __log.info("Collecting %i decision objects from hypos", len(decisionObjects))
    return sorted(decisionObjects)

def collectFilterDecisionObjects(filters, inputs = True, outputs = True):
    decisionObjects = set()
    for step, stepFilters in filters.items():
        for filt in stepFilters:
            if inputs and hasattr( filt, "Input" ):
                decisionObjects.update( str(i) for i in filt.Input )
            if outputs and hasattr( filt, "Output" ):
                decisionObjects.update( str(o) for o in filt.Output )
    __log.info("Collecting %i decision objects from filters", len(decisionObjects))
    return decisionObjects

def collectHLTSummaryDecisionObjects(hltSummary):
    decisionObjects = set()
    decisionObjects.add( str(hltSummary.DecisionsSummaryKey) )
    __log.info("Collecting %i decision objects from hltSummary", len(decisionObjects))
    return decisionObjects

def collectDecisionObjects(  hypos, filters, l1decoder, hltSummary ):
    """
    Returns the set of all decision objects of HLT
    """
    decObjL1 = collectL1DecoderDecisionObjects(l1decoder)
    decObjHypo = collectHypoDecisionObjects(hypos, inputs = True, outputs = True)
    decObjFilter = collectFilterDecisionObjects(filters, inputs = True, outputs = True)
    # InputMaker are not needed explicitly as the Filter Outputs = InputMaker Inputs
    # and InputMaker Outputs = Hypo Inputs
    # Therefore we implicitly collect all navigaiton I/O of all InputMakers
    decObjSummary = collectHLTSummaryDecisionObjects(hltSummary)
    decisionObjects = set()
    decisionObjects.update(decObjL1)
    decisionObjects.update(decObjHypo)
    decisionObjects.update(decObjFilter)
    decisionObjects.update(decObjSummary)
    return list(sorted(decisionObjects))

def triggerSummaryCfg(flags, hypos):
    """
    Configures an algorithm(s) that should be run after the selection process
    Returns: ca, algorithm
    """
    acc = ComponentAccumulator()
    DecisionSummaryMakerAlg=CompFactory.DecisionSummaryMakerAlg
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    decisionSummaryAlg = DecisionSummaryMakerAlg()
    allChains = OrderedDict() # keys are chain names, values are lists of collections


    # sort steps according to the step number i.e. strings Step1 Step2 ... Step10 Step11 rather than
    # alphabetic order Step10 Step11 Step1 Step2
    for stepName, stepHypos in sorted( hypos.items(), key=lambda x : __stepNumber(x[0]) ):
        # While filling the allChains dict we will replace the content intentionally
        # i.e. the chain has typically multiple steps and we want the collections from the last one
        # In a step the chain is handled by hypo or hypo followed by the combo,
        # in second case we want out of the later.
        # For that we process first ComboHypo and then regular Hypos 
        # (TODO, review this whn config is symmetrised by addition of ComboHypos always)
        orderedStepHypos = sorted(stepHypos, key=lambda hypo: not __isCombo(hypo))

        stepChains = OrderedDict()
        for hypo in orderedStepHypos:
            hypoChains, hypoOutputKeys = __decisionsFromHypo( hypo )
            for chain in hypoChains:
                if chain not in stepChains:
                    stepChains[chain] = hypoOutputKeys
        allChains.update( stepChains )

    from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
    from L1Decoder.L1DecoderConfig import mapThresholdToL1DecisionCollection
    if len(TriggerConfigHLT.dicts()) == 0:
        __log.warning("No HLT menu, chains w/o algorithms are not handled")
    else:
        for chainName, chainDict in TriggerConfigHLT.dicts().items():
            if chainName not in allChains:
                __log.debug("The chain %s is not mentioned in any step", chainName)
                # TODO once sequences available in the menu we need to crosscheck it here
                assert len(chainDict['chainParts'])  == 1, "Chains w/o the steps can not have mutiple parts in chainDict, it makes no sense: %s"%chainName
                allChains[chainName] = [ mapThresholdToL1DecisionCollection( chainDict['chainParts'][0]['L1threshold'] ) ]

    for c, cont in allChains.items():
        __log.debug("Final decision of chain  %s will be read from %d %s", c, len(cont), str(cont))
    # Flatten all the collections preserving the order
    collectionsWithFinalDecisions = []
    for chain, collections in allChains.items():
        for c in collections:
            if c not in collectionsWithFinalDecisions:
                collectionsWithFinalDecisions.append(c)
    __log.debug("Final keys %s", collectionsWithFinalDecisions)
    decisionSummaryAlg.FinalDecisionKeys = collectionsWithFinalDecisions
    decisionSummaryAlg.FinalStepDecisions = dict(allChains)
    decisionSummaryAlg.DecisionsSummaryKey = "HLTNav_Summary" # Output
    decisionSummaryAlg.DoCostMonitoring = flags.Trigger.CostMonitoring.doCostMonitoring
    decisionSummaryAlg.CostWriteHandleKey = recordable(flags.Trigger.CostMonitoring.outputCollection)
    return acc, decisionSummaryAlg


def triggerMonitoringCfg(flags, hypos, filters, l1Decoder):
    """
    Configures components needed for monitoring chains
    """
    acc = ComponentAccumulator()
    TrigSignatureMoniMT, DecisionCollectorTool=CompFactory.getComps("TrigSignatureMoniMT","DecisionCollectorTool",)
    mon = TrigSignatureMoniMT()
    mon.L1Decisions = "L1DecoderSummary"
    mon.FinalDecisionKey = "HLTNav_Summary" # Input
    if len(hypos) == 0:
        __log.warning("Menu is not configured")
        return acc, mon
    allChains = set() # collects the last decision obj for each chain

    # lambda sort because we have strings Step1 Step2 ... Step10 Step11 and python sorts that
    # to Step10 Step11 Step1 Step2
    stepCounter = 1
    for stepName, stepHypos in sorted( hypos.items(), key=lambda x : __stepNumber(x[0])):
        assert __stepNumber(stepName) == stepCounter, "There are steps that have no hypos, decisions counting is not going to work"
        stepCounter += 1
        stepDecisionKeys = []
        for hypo in stepHypos:
            hypoChains, hypoOutputKeys  = __decisionsFromHypo( hypo )
            stepDecisionKeys.extend( hypoOutputKeys )
            allChains.update( hypoChains )

        dcTool = DecisionCollectorTool( "DecisionCollector" + stepName, Decisions=list(OrderedDict.fromkeys(stepDecisionKeys)))
        __log.debug( "The step monitoring decisions in %s %s", dcTool.getName(), dcTool.Decisions)
        mon.CollectorTools += [ dcTool ]


    #mon.FinalChainStep = allChains
    mon.L1Decisions  = getProp( l1Decoder, 'L1DecoderSummaryKey' )

    # For now use old svcMgr interface as this service is not available from acc.getService()
    algToChainTool = CompFactory.getComp("TrigCompositeUtils::AlgToChainTool")()
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if hasattr(svcMgr,'HltEventLoopMgr'):
        svcMgr.HltEventLoopMgr.TrigErrorMonTool.AlgToChainTool = conf2toConfigurable(algToChainTool)

        svcMgr.HltEventLoopMgr.TrigErrorMonTool.MonTool.defineHistogram(
            'ErrorChainName,ErrorCode', path='EXPERT', type='TH2I',
            title='Error StatusCodes per chain;Chain name;StatusCode',
            xbins=1, xmin=0, xmax=1, ybins=1, ymin=0, ymax=1)


    from DecisionHandling.DecisionHandlingConfig import setupFilterMonitoring
    [ [ setupFilterMonitoring( alg ) for alg in algs ]  for algs in list(filters.values()) ]


    return acc, mon



def triggerOutputCfg(flags, summaryAlg):
    # Following cases are considered:
    # 1) Running in partition or athenaHLT - configure BS output written by the HLT framework
    # 2) Running offline athena and writing BS - configure BS output written by OutputStream alg
    # 3) Running offline athena with POOL output - configure POOL output written by OutputStream alg
    onlineWriteBS = False
    offlineWriteBS = False
    writePOOL = False

    isPartition = len(flags.Trigger.Online.partitionName) > 0
    if flags.Trigger.writeBS:
        if isPartition:
            onlineWriteBS = True
        else:
            offlineWriteBS = True
    if flags.Output.doWriteRDO or flags.Output.doWriteESD or flags.Output.doWriteAOD:
        writePOOL = True

    # Consistency checks
    if offlineWriteBS and not flags.Output.doWriteBS:
        __log.error('flags.Trigger.writeBS is True but flags.Output.doWriteBS is False')
        return None, ''
    if writePOOL and onlineWriteBS:
        __log.error("POOL HLT output writing is configured online")
        return None, ''
    if writePOOL and offlineWriteBS:
        __log.error("Writing HLT output to both BS and POOL in one job is not supported at the moment")
        return None, ''

    # Determine EDM set name
    edmSet = ''
    if writePOOL:
        edmSet = flags.Trigger.AODEDMSet if flags.Output.doWriteAOD else flags.Trigger.ESDEDMSet
    elif onlineWriteBS or offlineWriteBS:
        edmSet = 'BS'

    # Create the configuration
    if onlineWriteBS:
        __log.info("Configuring online ByteStream HLT output")
        acc = triggerBSOutputCfg(flags, summaryAlg)
        # Configure the online HLT result maker to use the above tools
        # For now use old svcMgr interface as this service is not available from acc.getService()
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        hltEventLoopMgr = svcMgr.HltEventLoopMgr
        hltEventLoopMgr.ResultMaker.MakerTools = []
        for tool in acc.popPrivateTools():
            if 'StreamTagMaker' in tool.getName():
                hltEventLoopMgr.ResultMaker.StreamTagMaker = conf2toConfigurable(tool)
            else:
                hltEventLoopMgr.ResultMaker.MakerTools += [ conf2toConfigurable(tool) ]
    elif offlineWriteBS:
        __log.info("Configuring offline ByteStream HLT output")
        acc = triggerBSOutputCfg(flags, summaryAlg, offline=True)
    elif writePOOL:
        __log.info("Configuring POOL HLT output")
        acc = triggerPOOLOutputCfg(flags, edmSet)
    else:
        __log.info("No HLT output writing is configured")
        acc = ComponentAccumulator()

    return acc, edmSet


def triggerBSOutputCfg(flags, summaryAlg, offline=False):
    """
    Returns CA with algorithms and/or tools required to do the serialisation

    decObj - list of all navigation objects
    decObjHypoOut - list of decisions produced by hypos
    summaryAlg - the instance of algorithm producing final decision
    offline - if true CA contains algorithms that need to be merged to output stream sequence,
              if false the CA contains a tool that needs to be added to HltEventLoopMgr
    """
    from TrigEDMConfig import DataScoutingInfo
    from TrigEDMConfig.TriggerEDM import getRun3BSList

    # Get list of all output collections for ByteStream (including DataScouting)
    collectionsToBS = getRun3BSList(["BS"] + DataScoutingInfo.getAllDataScoutingIdentifiers())

    # Build an output dictionary with key = collection type#name, value = list of ROBFragment module IDs
    ItemModuleDict = OrderedDict()
    for typekey, bsfragments in collectionsToBS:
        # Translate readable fragment names like BS, CostMonDS to ROB fragment IDs 0 (full result), 1, ... (DS results)
        moduleIDs = [ DataScoutingInfo.getFullHLTResultID() if f == 'BS' else DataScoutingInfo.getDataScoutingResultID(f)
                      for f in bsfragments ]
        ItemModuleDict[typekey] = moduleIDs

    from TrigOutputHandling.TrigOutputHandlingConfig import TriggerEDMSerialiserToolCfg, StreamTagMakerToolCfg, TriggerBitsMakerToolCfg

    # Tool serialising EDM objects to fill the HLT result
    serialiser = TriggerEDMSerialiserToolCfg('Serialiser')
    for item, modules in ItemModuleDict.items():
        sModules = sorted(modules)
        __log.debug('adding to serialiser list: %s, modules: %s', item, sModules)
        serialiser.addCollection(item, sModules)

    # Tools adding stream tags and trigger bits to HLT result
    stmaker = StreamTagMakerToolCfg()
    bitsmaker = TriggerBitsMakerToolCfg()

    # Map decisions producing PEBInfo from DecisionSummaryMakerAlg.FinalStepDecisions to StreamTagMakerTool.PEBDecisionKeys
    PEBKeys = []
    for keys in summaryAlg.FinalStepDecisions.values():
        for key in keys:
            if 'PEBInfoWriter' in key:
                PEBKeys.append(key)                
    stmaker.PEBDecisionKeys = list(set(PEBKeys))

    acc = ComponentAccumulator(sequenceName="HLTTop")
    if offline:
        # Create HLT result maker and alg
        from TrigOutputHandling.TrigOutputHandlingConfig import HLTResultMTMakerCfg
        HLTResultMTMakerAlg=CompFactory.HLTResultMTMakerAlg
        hltResultMakerTool = HLTResultMTMakerCfg()
        hltResultMakerTool.StreamTagMaker = stmaker
        hltResultMakerTool.MakerTools = [bitsmaker, serialiser]
        hltResultMakerAlg = HLTResultMTMakerAlg()
        hltResultMakerAlg.ResultMaker = hltResultMakerTool
        acc.addEventAlgo(hltResultMakerAlg)

        # Transfer trigger bits to xTrigDecision which is read by offline BS writing ByteStreamCnvSvc
        decmaker = CompFactory.getComp("TrigDec::TrigDecisionMakerMT")("TrigDecMakerMT")
        acc.addEventAlgo(decmaker)

        # Create OutputStream alg
        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamWriteCfg
        writingAcc = ByteStreamWriteCfg(flags, [ "HLT::HLTResultMT#HLTResultMT" ])
        writingAcc.getPrimary().ExtraInputs = [
            ("HLT::HLTResultMT", "HLTResultMT"),
            ("xAOD::TrigDecision", "xTrigDecision")]
        writingAcc.getService('ByteStreamEventStorageOutputSvc').StreamType = 'unknown'
        writingAcc.getService('ByteStreamEventStorageOutputSvc').StreamName = 'SingleStream'
        acc.merge(writingAcc)
    else:
        acc.setPrivateTools( [bitsmaker, stmaker, serialiser] )
    return acc


def triggerPOOLOutputCfg(flags, edmSet):
    # Get the list of output collections from TriggerEDM
    from TrigEDMConfig.TriggerEDM import getTriggerEDMList
    edmList = getTriggerEDMList(edmSet, flags.Trigger.EDMVersion)

    # Build the output ItemList
    itemsToRecord = []
    for edmType, edmKeys in edmList.items():
        itemsToRecord.extend([edmType+'#'+collKey for collKey in edmKeys])

    # Add EventInfo
    itemsToRecord.append('xAOD::EventInfo#EventInfo')
    itemsToRecord.append('xAOD::EventAuxInfo#EventInfoAux.')

    # Create OutputStream
    outputType = ''
    if flags.Output.doWriteRDO:
        outputType = 'RDO'
    if flags.Output.doWriteESD:
        outputType = 'ESD'
    if flags.Output.doWriteAOD:
        outputType = 'AOD'
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc = OutputStreamCfg(flags, outputType, ItemList=itemsToRecord, disableEventTag=True)
    streamAlg = acc.getEventAlgo("OutputStream"+outputType)

    # Keep input RDO objects in the output RDO_TRIG file
    if flags.Output.doWriteRDO:
        streamAlg.TakeItemsFromInput = True

    # Produce trigger bits
    bitsmaker = CompFactory.TriggerBitsMakerTool()
    decmaker = CompFactory.getComp("TrigDec::TrigDecisionMakerMT")("TrigDecMakerMT", BitsMakerTool = bitsmaker)
    acc.addEventAlgo( decmaker )

    # Produce trigger metadata
    menuwriter = CompFactory.getComp("TrigConf::xAODMenuWriterMT")()
    menuwriter.IsHLTJSONConfig = True
    menuwriter.IsL1JSONConfig = True
    menuwriter.WritexAODTriggerMenu = True # This should be removed in the future
    menuwriter.WritexAODTriggerMenuJson = True
    menuwriter.KeyWriterTool = CompFactory.getComp('TrigConf::KeyWriterTool')('KeyWriterToolOffline')
    acc.addEventAlgo( menuwriter )

    # Schedule the insertion of L1 prescales into the conditions store
    # Required for metadata production
    from TrigConfigSvc.TrigConfigSvcCfg import  L1PrescaleCondAlgCfg
    acc.merge( L1PrescaleCondAlgCfg( flags ) )

    # Add metadata to the output stream
    if menuwriter.WritexAODTriggerMenu:
      streamAlg.MetadataItemList += [ "xAOD::TriggerMenuContainer#*", "xAOD::TriggerMenuAuxContainer#*" ]

    if menuwriter.WritexAODTriggerMenuJson:
      streamAlg.MetadataItemList += [ "xAOD::TriggerMenuJsonContainer#*", "xAOD::TriggerMenuJsonAuxContainer#*" ]

    # Ensure OutputStream runs after TrigDecisionMakerMT and xAODMenuWriterMT
    streamAlg.ExtraInputs += [
        ("xAOD::TrigDecision", str(decmaker.TrigDecisionKey)),
        ("xAOD::TrigConfKeys", str(menuwriter.KeyWriterTool.ConfKeys))]

    # Produce xAOD L1 RoIs from RoIBResult
    from AnalysisTriggerAlgs.AnalysisTriggerAlgsCAConfig import RoIBResultToxAODCfg
    xRoIBResultAcc, xRoIBResultOutputs = RoIBResultToxAODCfg(flags, acc.getSequence().name)
    acc.merge(xRoIBResultAcc)
    # Ensure outputs are produced before streamAlg runs
    streamAlg.ExtraInputs += xRoIBResultOutputs

    return acc


def triggerMergeViewsAndAddMissingEDMCfg( flags, edmSet, hypos, viewMakers, decObj, decObjHypoOut ):

    HLTEDMCreatorAlg, HLTEDMCreator=CompFactory.getComps("HLTEDMCreatorAlg","HLTEDMCreator",)
    from TrigEDMConfig.TriggerEDMRun3 import TriggerHLTListRun3, addExtraCollectionsToEDMList

    __log.info( "Number of EDM items in triggerMergeViewsAndAddMissingEDMCfg: %d", len(TriggerHLTListRun3))
    if flags.Trigger.ExtraEDMList:
        __log.info( "Adding extra collections to EDM: %s", str(flags.Trigger.ExtraEDMList))
        addExtraCollectionsToEDMList(TriggerHLTListRun3, flags.Trigger.ExtraEDMList)
        __log.info( "Number of EDM items after adding extra collections: %d", len(TriggerHLTListRun3))


    alg = HLTEDMCreatorAlg("EDMCreatorAlg")

    # configure views merging
    needMerging = [x for x in TriggerHLTListRun3 if len(x) >= 4 and x[3].startswith("inViews:")]
    __log.info("These collections need merging: %s", " ".join([ c[0] for c in needMerging ]))
    mergingTool = HLTEDMCreator( "ViewsMergingTool")
    for coll in needMerging:
        collType, collName = coll[0].split("#")
        collType = collType.split(":")[-1]
        possibleViews = coll[3].split(":")[-1].split(",")
        for viewsColl in possibleViews:
            attrView = getattr(mergingTool, collType+"Views", [])
            attrInView = getattr(mergingTool, collType+"InViews", [])
            attrName = getattr(mergingTool, collType, [])
            #
            attrView.append( viewsColl )
            attrInView.append( collName )
            attrName.append( collName )
            #

            setattr(mergingTool, collType+"Views", attrView )
            setattr(mergingTool, collType+"InViews", attrInView )
            setattr(mergingTool, collType, attrName )
            producer = [ maker for maker in viewMakers if maker.Views == viewsColl ]
            if len(producer) == 0:
                __log.warning("The producer of the %s not in the menu, it's outputs won't ever make it out of the HLT", coll)
                continue
            if len(producer) > 1:
                for pr in producer[1:]:
                    if pr != producer[0]:
                        __log.error("Several View making algorithms produce the same output collection %s: %s", viewsColl, ' '.join([p.getName() for p in producer ]))
                        continue
    alg.OutputTools += [mergingTool]

    tool = HLTEDMCreator( "GapFiller" )
    if len(edmSet) != 0:
        from collections import defaultdict
        groupedByType = defaultdict( list )

        # scan the EDM
        for el in TriggerHLTListRun3:
            if not any([ outputType in el[1].split() for outputType in edmSet ]):
                continue
            collType, collName = el[0].split("#")
            if "Aux" in collType: # the GapFiller crates appropriate Aux obejcts
                continue
            if len(el) >= 4: # see if there is an alias
                aliases = [ x for x in el[3:]  if "alias:" in x ] # assume that the description can be: (.... , [alias:Blah | inViews:XYZ | inViews:XYZ, alias:Blah])
                if len(aliases) == 1:
                    alias = aliases[0].split(":")[1]
                    __log.info("GapFiller configuration found an aliased type '%s' for '%s'", alias, collType)
                    collType = alias
                elif len(aliases) > 1:
                    __log.error("GapFiller configuration found inconsistent '%s' (too many aliases?)", el[3:])

            groupedByType[collType].append( collName )

        for collType, collNameList in groupedByType.items():
            propName = collType.split(":")[-1]
            if hasattr( tool, propName ):
                setattr( tool, propName, collNameList )
                __log.info("GapFiller will create EDM collection type '%s' for '%s'", collType, collNameList)
            else:
                __log.info("EDM collections of type %s are not going to be added to StoreGate, if not created by the HLT", collType )

    __log.debug("The GapFiller is ensuring the creation of all the decision object collections: '%s'", decObj)
    # Append and hence confirm all TrigComposite collections
    # Gap filler is also used to perform re-mapping of the HypoAlg outputs which is a sub-set of decObj
    tool.FixLinks = list(decObjHypoOut)
    tool.TrigCompositeContainer += list(decObj)
    alg.OutputTools += [tool]

    return alg




def triggerRunCfg( flags, seqName = None, menu=None ):
    """
    top of the trigger config (for real triggering online or on MC)
    Returns: ca only
    """
    acc = ComponentAccumulator()

    # L1ConfigSvc needed for L1Decoder
    from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg

    acc.merge( L1ConfigSvcCfg(flags) )

    acc.addSequence( seqOR( "HLTTop") )

    acc.addSequence( parOR("HLTBeginSeq"), parentName="HLTTop" )
    # bit of a hack as for "legacy" type JO a seq name for cache creators has to be given,
    # in newJO realm the seqName will be removed as a comp fragment shoudl be unaware of where it will be attached
    acc.merge( triggerIDCCacheCreatorsCfg( flags, seqName="AthAlgSeq" ), sequenceName="HLTBeginSeq" )

    from L1Decoder.L1DecoderConfig import L1DecoderCfg
    l1DecoderAcc = L1DecoderCfg( flags, seqName =  "HLTBeginSeq")
    # TODO, once moved to newJO the algorithm can be added to l1DecoderAcc and merging will be sufficient here
    acc.merge( l1DecoderAcc )

    # detour to the menu here, (missing now, instead a temporary hack)
    if menu:
        menuAcc = menu( flags )
        HLTSteps = menuAcc.getSequence( "HLTAllSteps" )
        __log.info( "Configured menu with %d steps", len(HLTSteps.Members))
        acc.merge( menuAcc, sequenceName="HLTTop")

    # collect hypothesis algorithms from all sequence
    hypos = collectHypos( HLTSteps )
    filters = collectFilters( HLTSteps )
    acc.addSequence( parOR("HLTEndSeq"), parentName="HLTTop" )
    acc.addSequence( seqAND("HLTFinalizeSeq"), parentName="HLTEndSeq" )

    summaryAcc, summaryAlg = triggerSummaryCfg( flags, hypos )
    acc.merge( summaryAcc, sequenceName="HLTFinalizeSeq" )
    acc.addEventAlgo( summaryAlg, sequenceName="HLTFinalizeSeq" )

    #once menu is included we should configure monitoring here as below
    l1DecoderAlg = l1DecoderAcc.getEventAlgo("L1Decoder")

    monitoringAcc, monitoringAlg = triggerMonitoringCfg( flags, hypos, filters, l1DecoderAlg )
    acc.merge( monitoringAcc, sequenceName="HLTEndSeq" )
    acc.addEventAlgo( monitoringAlg, sequenceName="HLTEndSeq" )

    from TrigCostMonitorMT.TrigCostMonitorMTConfig import TrigCostMonitorMTCfg
    acc.merge( TrigCostMonitorMTCfg( flags ) )

    decObj = collectDecisionObjects( hypos, filters, l1DecoderAlg, summaryAlg )
    decObjHypoOut = collectHypoDecisionObjects(hypos, inputs=False, outputs=True)
    __log.info( "Number of decision objects found in HLT CF %d", len( decObj ) )
    __log.info( "Of which, %d are the outputs of hypos", len( decObjHypoOut ) )
    __log.info( decObj )

    # configure components need to normalise output before writing out
    viewMakers = collectViewMakers( HLTSteps )

    # Add HLT Navigation to EDM list
    from TrigEDMConfig import TriggerEDMRun3
    __log.info( "Number of EDM items before adding navigation: %d", len(TriggerEDMRun3.TriggerHLTListRun3))
    TriggerEDMRun3.addHLTNavigationToEDMList(TriggerEDMRun3.TriggerHLTListRun3, decObj, decObjHypoOut)
    __log.info( "Number of EDM items after adding navigation: %d", len(TriggerEDMRun3.TriggerHLTListRun3))

    # Configure output writing
    outputAcc, edmSet = triggerOutputCfg( flags, summaryAlg )
    acc.merge( outputAcc )

    if edmSet:
        mergingAlg = triggerMergeViewsAndAddMissingEDMCfg( flags, [edmSet] , hypos, viewMakers, decObj, decObjHypoOut )
        acc.addEventAlgo( mergingAlg, sequenceName="HLTFinalizeSeq" )

    return acc

def triggerIDCCacheCreatorsCfg(flags, seqName = None):
    """
    Configures IDC cache loading, for now unconditionally, may make it menu dependent in future
    """
    acc = ComponentAccumulator(sequenceName = seqName)
    from MuonConfig.MuonBytestreamDecodeConfig import MuonCacheCfg
    acc.merge( MuonCacheCfg(), sequenceName = seqName )

    from MuonConfig.MuonRdoDecodeConfig import MuonPrdCacheCfg
    acc.merge( MuonPrdCacheCfg(), sequenceName = seqName )

    from TrigInDetConfig.TrigInDetConfig import InDetIDCCacheCreatorCfg
    acc.merge( InDetIDCCacheCreatorCfg(), sequenceName = seqName )

    return acc

def triggerPostRunCfg(flags):
    """
    Configures components needed for processing trigger informatin in Raw/ESD step
    Returns: ca only
    """
    acc = ComponentAccumulator()
    # configure in order BS decodnig, EDM gap filling, insertion of trigger metadata to ESD

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Trigger.L1Decoder.forceEnableAllChains = True
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1",]
    ConfigFlags.lock()

    def testMenu(flags):
        menuCA = ComponentAccumulator()
        menuCA.addSequence( seqAND("HLTAllSteps") )
        return menuCA


    acc = triggerRunCfg( ConfigFlags, seqName = None, menu = testMenu )
    Configurable.configurableRun3Behavior=0
    from AthenaConfiguration.ComponentAccumulator import appendCAtoAthena
    appendCAtoAthena( acc )


    f=open("TriggerRunConf.pkl","wb")
    acc.store(f)
    f.close()
