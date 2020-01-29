# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
    ------ Documentation on HLT Tree creation -----

++ Filter creation strategy

++ Connections between InputMaker/HypoAlg/Filter

++ Seeds

++ Combined chain strategy

- The combined chains use duplicates of the single-object-HypoAlg, called HypoAlgName_for_stepName.
  These duplicates are connected to a dedicated ComboHypoAlg (added by the framework), able to count object multiplicity
     -- This is needed for two reasons:
           -- the HypoAlg is designed to have only one input TC (that is already for the single object)
           -- otherwise the HypoAlg would be equipped with differnt HypoTools with the same name (see for example e3_e8)
     -- If the combined chain is symmetric (with multiplicity >1), the Hypo is duplicated only once,
        equipped with a HypoTool configured as single object and followed by one ComboHypoAlg





"""

# Classes to configure the CF graph, via Nodes
from AthenaCommon.CFElements import parOR, seqAND, seqOR
from AthenaCommon.AlgSequence import dumpSequence
from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFDot import  stepCF_DataFlow_to_dot, stepCF_ControlFlow_to_dot, all_DataFlow_to_dot, create_dot
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponentsNaming import CFNaming
#from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep


from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )

#### Here functions to create the CF tree from CF configuration objects
def makeSummary(name, flatDecisions):
    """ Returns a TriggerSummaryAlg connected to given decisions"""
    from DecisionHandling.DecisionHandlingConfig import TriggerSummaryAlg    
    summary = TriggerSummaryAlg( CFNaming.stepSummaryName(name) )
    summary.InputDecision = "L1DecoderSummary"
    summary.FinalDecisions = list(set(flatDecisions))
    return summary


def createStepRecoNode(name, seq_list, dump=False):
    """ elementary HLT reco step, contianing all sequences of the step """

    log.debug("Create reco step %s with %d sequences", name, len(seq_list))
    stepCF = parOR(name + CFNaming.RECO_POSTFIX)
    for seq in seq_list:
        stepCF += createCFTree(seq)

    if dump:
        dumpSequence (stepCF, indent=0)
    return stepCF


def createStepFilterNode(name, seq_list, dump=False):
    """ elementary HLT filter step: OR node containing all Filters of the sequences. The node gates execution of next reco step """

    log.debug("Create filter step %s with %d filters", name, len(seq_list))
    filter_list=[]
    for seq in seq_list:
        filterAlg = seq.filter.Alg
        log.info("createStepFilterNode: Add  %s to filter node %s", filterAlg.name(), name)
        filter_list.append(filterAlg)


    stepCF = parOR(name + CFNaming.FILTER_POSTFIX, subs=set(filter_list))

    if dump:
        dumpSequence (stepCF, indent=0)
    return stepCF


def createCFTree(CFseq):
    """ Creates AthSequencer nodes with sequences attached """

    log.debug(" *** Create CF Tree for CFSequence %s", CFseq.step.name)
    filterAlg = CFseq.filter.Alg

    #empty step:
    if len(CFseq.step.sequences)==0:
        seqAndWithFilter = seqAND(CFseq.step.name, [filterAlg])
        return seqAndWithFilter

    stepReco = parOR(CFseq.step.name + CFNaming.RECO_POSTFIX)  # all reco algoritms from al lthe sequences in a parallel sequence
    seqAndView = seqAND(CFseq.step.name + CFNaming.VIEW_POSTFIX, [stepReco])  # include in seq:And to run in views: add here the Hypo
    seqAndWithFilter = seqAND(CFseq.step.name, [filterAlg, seqAndView])  # add to the main step+filter

    already_connected = []
    for menuseq in CFseq.step.sequences:
        ath_sequence = menuseq.sequence.Alg
        name = ath_sequence.name()
        if name in already_connected:
            log.debug("AthSequencer %s already in the Tree, not added again",name)
            continue
        else:
            already_connected.append(name)
            stepReco += ath_sequence
        if type(menuseq.hypo) is list:
           for hp in menuseq.hypo:
              seqAndView += hp.Alg
        else:
           seqAndView += menuseq.hypo.Alg

    if CFseq.step.isCombo:
        seqAndView += CFseq.step.combo.Alg

    return seqAndWithFilter



#######################################
## CORE of Decision Handling
#######################################

def makeHLTTree(newJO=False, triggerConfigHLT = None):
    """ creates the full HLT tree"""

    # Check if triggerConfigHLT exits, if yes, derive information from this
    # this will be in use once TrigUpgrade test has migrated to TriggerMenuMT completely

    # get topSequnece
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    #add the L1Upcacking
    #    TopHLTRootSeq += L1UnpackingSeq

    # connect to L1Decoder
    l1decoder = [ d for d in topSequence.getChildren() if d.getType() == "L1Decoder" ]
    if len(l1decoder)  != 1 :
        raise RuntimeError(" Can't find 1 instance of L1Decoder in topSequence, instead found this in topSequence "+str(topSequence.getChildren()) )

    # take L1Decoder out of topSeq
    topSequence.remove( l1decoder )

    # main HLT top sequence
    hltTop = seqOR("HLTTop")

    # put L1Decoder here
    hltTop += l1decoder

    # add the HLT steps Node
    steps = seqAND("HLTAllSteps")
    hltTop +=  steps

    # make DF and CF tree from chains
    finalDecisions = decisionTreeFromChains(steps, triggerConfigHLT.configsList(), triggerConfigHLT.dictsList(), newJO)

    flatDecisions=[]
    for step in finalDecisions:
        flatDecisions.extend (step)

    summary= makeSummary("TriggerSummaryFinal", flatDecisions)
    hltTop += summary

    # TODO - check we are not running things twice. Once here and once in TriggerConfig.py

    from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectViewMakers, collectDecisionObjects,\
        triggerMonitoringCfg, triggerSummaryCfg, triggerMergeViewsAndAddMissingEDMCfg, collectHypoDecisionObjects
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior+=1

    hypos = collectHypos(steps)
    filters = collectFilters(steps)
    viewMakers = collectViewMakers(steps)
    summaryAcc, summaryAlg = triggerSummaryCfg( ConfigFlags, hypos )
    hltTop += summaryAlg
    summaryAcc.appendToGlobals()
    decObj = collectDecisionObjects( hypos, filters, l1decoder[0], summaryAlg )
    decObjHypoOut = collectHypoDecisionObjects(hypos, inputs=False, outputs=True)

    monAcc, monAlg = triggerMonitoringCfg( ConfigFlags, hypos, filters, l1decoder[0] )
    monAcc.appendToGlobals()
    hltTop += monAlg

    # this is a shotcut for now, we always assume we may be writing ESD & AOD outputs, so all gaps will be filled
    edmAlg = triggerMergeViewsAndAddMissingEDMCfg(['AOD', 'ESD'], hypos, viewMakers, decObj, decObjHypoOut)
    hltTop += edmAlg

    Configurable.configurableRun3Behavior-=1

    topSequence += hltTop

    # Test the configuration
    from TriggerMenuMT.HLTMenuConfig.Menu.CFValidation import testHLTTree
    testHLTTree( topSequence )

def matrixDisplayOld( allCFSeq ):
    from collections import defaultdict
    longestName = 5
    mx = defaultdict(lambda: dict())
    for stepNumber,step in enumerate(allCFSeq, 1):
        for seq in step:
            mx[stepNumber][seq.step.name] = seq # what if ther eare more sequences in one step?

            longestName = max(longestName, len(seq.step.name) )

    longestName = longestName + 1

    def __getHyposOfStep( s ):
        if len(s.step.sequences):
            if len(s.step.sequences)==1:
                if type(s.step.sequences[0].hypo) is list:
                    return s.step.sequences[0].hypo[0].tools
                else:
                    return s.step.sequences[0].hypo.tools
            else:
                return s.step.combo.getChains().keys()
        return []
   



    def __nextSteps( index, stepName ):
        nextStepName = "Step%s_"%index + "_".join(stepName.split("_")[1:])
        for sname, seq in mx[index].iteritems():
            if sname == nextStepName:
                return sname.ljust( longestName ) + __nextSteps( index + 1, nextStepName )
        return ""

    log.debug("" )
    log.debug("chains^ vs steps ->")
    log.debug( "="*90 )
    for sname, seq in mx[1].iteritems():
        guessChainName = '_'.join( sname.split( "_" )[1:] )
        log.debug( " Reco chain: %s: %s", guessChainName.rjust(longestName),  __nextSteps( 1, sname ) )
        log.debug( " %s", " ".join( __getHyposOfStep( seq ) ) )
        log.debug( "" )

    log.debug( "%s", "="*90 )
    log.debug( "" )


    
def matrixDisplay( allCFSeq ):
 
    def __getHyposOfStep( step ):
        if len(step.sequences):
            if len(step.sequences)==1:
                if type(step.sequences[0].hypo) is list:
                    return step.sequences[0].hypo[0].tools
                else:
                    return step.sequences[0].hypo.tools
            else:
                return step.combo.getChains().keys()
        return []
 
   
    # fill dictionary to cumulate chains on same sequences, in steps (dict with composite keys)
    from collections import defaultdict
    mx = defaultdict(list)

    for stepNumber,cfseq_list in enumerate(allCFSeq, 1):
        for cfseq in cfseq_list:
            chains = __getHyposOfStep(cfseq.step)
            for seq in cfseq.step.sequences:
                mx[stepNumber, seq.sequence.Alg.name()].extend(chains)


    # sort dictionary by fist key=step
    from collections import  OrderedDict
    sorted_mx = OrderedDict(sorted( mx.items(), key= lambda k: k[0]))

    log.info( "" )
    log.info( "="*90 )
    log.info( "Cumulative Summary of steps")
    log.info( "="*90 )
    for (step, seq), chains in sorted_mx.items():
        log.info( "(step, sequence)  ==> (%d, %s) is in chains: ",  step, seq)
        for chain in chains:
            log.info( "              %s",chain)

    log.info( "="*90 )



   

def decisionTreeFromChains(HLTNode, chains, allDicts, newJO):
    """ creates the decision tree, given the starting node and the chains containing the sequences  """

    log.debug("Run decisionTreeFromChains on %s", HLTNode.name())
    HLTNodeName= HLTNode.name()
    if len(chains) == 0:
        log.info("Configuring empty decisionTree")
        return []

    # add chains to multiplicity map (new step here, as this was originally in the __init__ of Chain class

    (finalDecisions, CFseq_list) = createDataFlow(chains, allDicts)
    if not newJO:
        createControlFlow(HLTNode, CFseq_list)
    else:
        from TriggerMenuMT.HLTMenuConfig.Menu.HLTCFConfig_newJO import createControlFlowNewJO
        createControlFlowNewJO(HLTNode, CFseq_list)

    # decode and attach HypoTools:
    for chain in chains:
        chain.decodeHypoToolConfs()

    log.debug("finalDecisions: %s", finalDecisions)
    if create_dot():
        all_DataFlow_to_dot(HLTNodeName, CFseq_list)

    # matrix display
    matrixDisplay( CFseq_list )

    return finalDecisions


def createDataFlow(chains, allDicts):
    """ Creates the filters and connect them to the menu sequences"""

    # find tot nsteps
    chainWithMaxSteps = max(chains, key=lambda chain: len(chain.steps))
    NSTEPS = len(chainWithMaxSteps.steps)

    log.debug("createDataFlow for %d chains and total %d steps", len(chains), NSTEPS)

    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CFSequence
    # initialize arrays for monitor
    finalDecisions = [ [] for n in range(NSTEPS) ]
    CFseqList = [ [] for n in range(NSTEPS) ]

    # loop over chains
    for chain in chains:
        log.info("\n Configuring chain %s with %d steps: \n   - %s ", chain.name,len(chain.steps),'\n   - '.join(map(str, [{step.name:step.multiplicity} for step in chain.steps])))

        lastCFseq = None
        for nstep, chainStep in enumerate( chain.steps ):
            log.debug("\n************* Start connecting step %d %s for chain %s", nstep+1, chainStep.name, chain.name)

            filterInput = chain.L1decisions if nstep == 0 else lastCFseq.decisions
            log.debug("Seeds added; having in the filter now: %s", filterInput)


            if len(filterInput) == 0 :
                log.error("ERROR: Filter for step %s has %d inputs! At least one is expected", chainStep.name, len(filterInput))


            # make one filter per step:
            sequenceFilter= None
            filterName = CFNaming.filterName(chainStep.name)
            filterOutput =[ CFNaming.filterOutName(filterName, inputName) for inputName in filterInput ]

            (foundFilter, foundCFSeq) = findCFSequences(filterName, CFseqList[nstep])
            log.debug("Found %d CF sequences with filter name %s", foundFilter, filterName)
             # add error if more than one
            if not foundFilter:
                sequenceFilter = buildFilter(filterName, filterInput)
                CFseq = CFSequence( ChainStep=chainStep, FilterAlg=sequenceFilter)
                CFseq.connect(filterOutput)
                CFseqList[nstep].append(CFseq)
                lastCFseq=CFseq
            else:
                if len(foundCFSeq) > 1:
                    log.error("Found more than one seuqences containig this filter %s", filterName)
                lastCFseq=foundCFSeq[0]
                sequenceFilter=lastCFseq.filter
                lastCFseq.connect(filterOutput)
                [ sequenceFilter.addInput(inputName) for inputName in filterInput ]
                [ sequenceFilter.addOutput(outputName) for outputName in  filterOutput ]


            # add chains to the filter:
            chainLegs = chain.getChainLegs()
            for leg in chainLegs:
                sequenceFilter.setChains(leg)
                log.debug("Adding chain %s to %s", leg, sequenceFilter.Alg.name())
            log.debug("Now Filter has chains: %s", sequenceFilter.getChains())

            if chainStep.isCombo:
                if chainStep.combo is not None:
                    chainStep.combo.addChain( [d for d in allDicts if d['chainName'] == chain.name ][0])
                    log.debug("Added chains to ComboHypo: %s",chainStep.combo.getChains())

            if len(chain.steps) == nstep+1:
                log.debug("Adding finalDecisions for chain %s at step %d:", chain.name, nstep+1)
                for dec in lastCFseq.decisions:
                    finalDecisions[nstep].append(dec)
                    log.debug(dec)
                    
        #end of loop over steps
        log.info("\n Built CF for chain %s with %d steps: \n   - %s ", chain.name,len(chain.steps),'\n   - '.join(map(str, [{step.name:step.multiplicity} for step in chain.steps])))
    #end of loop over chains


    log.debug("End of createDataFlow for %d chains and total %d steps", len(chains), NSTEPS)
    return (finalDecisions, CFseqList)


def createControlFlow(HLTNode, CFseqList):
    """ Creates Control Flow Tree starting from the CFSequences"""

    HLTNodeName= HLTNode.name()
    log.debug("createControlFlow on node %s",HLTNodeName)

    for nstep in range(len(CFseqList)):
        stepSequenceName =  CFNaming.stepName(nstep)
        log.debug("\n******** Create CF Tree %s with AthSequencers", stepSequenceName)

        #first make the filter step
        stepFilterNode = createStepFilterNode(stepSequenceName, CFseqList[nstep], dump=False)
        HLTNode += stepFilterNode

        # then the reco step
        stepRecoNode = createStepRecoNode(stepSequenceName, CFseqList[nstep], dump=False)
        HLTNode += stepRecoNode


        # then the monitor summary
        stepDecisions = []
        for CFseq in CFseqList[nstep]:
            stepDecisions.extend(CFseq.decisions)

        summary=makeSummary( stepSequenceName, stepSequenceName )

        HLTNode += summary

        if create_dot():
            log.debug("Now Draw...")
            stepCF_DataFlow_to_dot(stepRecoNode.name(), CFseqList[nstep])
            stepCF_ControlFlow_to_dot(stepRecoNode)

        log.info("************* End of step %d, %s", nstep+1, stepSequenceName)

    return




"""
Not used, kept for reference and testing purposes
To be removed in future
"""
def generateDecisionTreeOld(HLTNode, chains, allChainDicts):
    log.debug("Run generateDecisionTreeOld on %s", HLTNode.name())
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from collections import defaultdict
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import CFSequence

    chainStepsMatrix = defaultdict(lambda: defaultdict(lambda: list()))

    ## Fill chain steps matrix
    for chain in chains:
        chain.decodeHypoToolConfs()#allChainDicts)
        for stepNumber, chainStep in enumerate(chain.steps):
            chainName = chainStep.name.split('_')[0]
            chainStepsMatrix[stepNumber][chainName].append(chain)

    allSequences = []

    ## Matrix with steps lists generated. Creating filters for each cell
    for nstep in chainStepsMatrix:
        CFsequences = []
        stepDecisions = []
        stepAccs = []
        stepHypos = []

        for chainName in chainStepsMatrix[nstep]:
            chainsInCell = chainStepsMatrix[nstep][chainName]

            if not chainsInCell:
                continue


            stepCategoryAcc = ComponentAccumulator()

            stepHypo = None

            for chain in chainsInCell:
                for seq in chain.steps[nstep].sequences:
                    if seq.ca:
                        stepCategoryAcc.merge( seq.ca )

                    alg = seq.hypo.Alg
                    if stepHypo is None:
                        stepHypo = alg
                        stepHypos.append( alg )
                    stepCategoryAcc.addEventAlgo( alg )

            stepAccs.append( stepCategoryAcc )

            stepCategoryAcc.printConfig( True, True )
            firstChain = chainsInCell[0]

            if nstep == 0:
                filter_input = firstChain.L1decisions
            else:
                filter_input = []
                for sequence in firstChain.steps[nstep - 1].sequences:
                    filter_input += sequence.outputs

            # One aggregated filter per chain (one per column in matrix)
            filterName = 'Filter_{}'.format( firstChain.steps[nstep].name )
            filter_output =[]
            for i in filter_input:
                filter_output.append( CFNaming.filterOutName(filterName, i))
            sfilter = buildFilter(filterName,  filter_input)

            chainStep = firstChain.steps[nstep]

            CFseq = CFSequence( ChainStep=chainStep, FilterAlg=sfilter, connections=filter_output )
            CFsequences.append( CFseq )


            for sequence in chainStep.sequences:
                stepDecisions += sequence.outputs

            for chain in chainsInCell:
                sfilter.setChains(chain.name)

        allSequences.append(CFsequences)

        stepName = 'Step{}'.format(nstep)
        stepFilter = createStepFilterNode(stepName, CFsequences, dump=False)
        stepCF = createStepRecoNode('{}_{}'.format(HLTNode.name(), stepName), CFsequences, dump=False)

        from AthenaCommon.CFElements import findOwningSequence
        for oneAcc, cfseq, hypo in zip( stepAccs, CFsequences, stepHypos):
            owning = findOwningSequence( stepCF, hypo.getName() )
            acc.addSequence( owning )
            acc.merge( oneAcc, sequenceName = owning.getName() )
        summary = makeSummary('TriggerSummary{}'.format(stepName), stepDecisions)

        HLTNode += stepFilter
        HLTNode += stepCF
        HLTNode += summary
        if create_dot():
            stepCF_DataFlow_to_dot('{}_{}'.format(HLTNode.name(), stepName), CFsequences)
            stepCF_ControlFlow_to_dot(stepCF)
            all_DataFlow_to_dot(HLTNode.name(), allSequences)

        matrixDisplay( allSequences )
    return acc



def findCFSequences(filter_name, cfseqList):
      """
      searches for a filter, with given name, in the CF sequence list of this step
      """
      log.debug( "findCFSequences: filter base name %s", filter_name )
      foundFilters = [cfseq for cfseq in cfseqList if filter_name == cfseq.filter.Alg.name()]
      log.debug("found %d filters with base name %s", len( foundFilters ), filter_name)

      found=len(foundFilters)
      if found:
          return (found, foundFilters)
      return (found, None)


def buildFilter(filter_name,  filter_input):
    """
     Build the FILTER
     one filter per previous sequence at the start of the sequence: always create a new one
     if the previous hypo has more than one output, try to get all of them
     one filter per previous sequence: 1 input/previous seq, 1 output/next seq
    """
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import  RoRSequenceFilterNode
    sfilter = RoRSequenceFilterNode(name=filter_name)
    for i in filter_input:
        sfilter.addInput(i)
    for i in filter_input:
        sfilter.addOutput(CFNaming.filterOutName(filter_name, i))

    log.debug("Added inputs to filter: %s", sfilter.getInputList())
    log.debug("Added outputs to filter: %s", sfilter.getOutputList())
    log.debug("Filter Done: %s", sfilter.Alg.name())

    return (sfilter)
