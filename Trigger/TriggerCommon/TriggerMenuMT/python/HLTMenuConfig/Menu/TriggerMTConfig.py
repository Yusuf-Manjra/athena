# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#########################################################################################
# User interface for the configuration, nothing beneath this class need to be studied
#
import re
import string
from collections import defaultdict
import xml.dom.minidom as minidom
import xml.etree.cElementTree as etree

from AthenaCommon.CFElements import parOR, seqAND, stepSeq


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMTConfig' )



class TriggerMTConfig:
    sCurrentTriggerConfig = None
    def currentTriggerConfig():
        return TriggerMTConfig.sCurrentTriggerConfig
    currentTriggerConfig = staticmethod(currentTriggerConfig)

    def __init__(self, hltfile=None, signaturesOverwritten=False):
        self.menuName = 'TestMenu'
        self.__HLTFile = hltfile
        self.allThresholds     = {}
        self.allItems          = {}
        self.allChains         = {}

        self.theHLTChains      = []
        self.theSeqLists       = []
        self.theSeqDict        = {} 
        self.theTopoStartFrom    = []

        # Sanity check since the second argument in __init__ used to be the l1 xml file name
        if type(signaturesOverwritten)!=bool:
            log.error('Wrong type for signaturesOverwritten. Received %s but expected bool', signaturesOverwritten)

        self.signaturesOverwritten = signaturesOverwritten
        TriggerMTConfig.sCurrentTriggerConfig = self

    def printIt(self):
        log.debug("self.theSeqLists: %s",  self.theSeqLists)
        log.debug("self.theSeqDict: %s", self.theSeqDict) 
        log.debug("self.theTopoStartFrom: %s", self.theTopoStartFrom) 
        


#########################################################################################
#########################################################################################
##### From HLTCFConfig.py
#########################################################################################
#########################################################################################


# Classes to configure the CF graph, via Nodes
from AthenaCommon.CFElements import parOR, seqAND, stepSeq


allAlgs={}
def useExisting(name):
    global allAlgs
    return allAlgs[name]

def AlgExisting(name):
    global allAlgs
    return name in allAlgs


def remember(name, instance):
    global allAlgs
    allAlgs[name] = instance
    return instance


class AlgNode():
    def __init__(self, Alg, inputProp, outputProp):
        self.name = ("%s_%s_%s")%( Alg.name(), inputProp, outputProp)
        self.algname = Alg.name()
        self.outputProp=outputProp
        self.inputProp=inputProp
        self.configureAlg(Alg)
                
    def configureAlg(self, Alg):
        alg_name = Alg.getName()
        if AlgExisting(alg_name):
            print "AlgNode::%s: found already Alg %s"%(self.name,alg_name)
            self.Alg = useExisting(alg_name)
        else:
            print "AlgNode::%s: new Alg %s"%(self.name,alg_name)
            self.Alg=Alg
            remember(alg_name, self.Alg)
            
        if self.Alg is  None:
            return

        self.addDefaultOutput()

    def addDefaultOutput(self):
        print "This is main addDefaultOutput"
        self.setOutput(("%s_%s_out"%(self.algname,self.outputProp)))
        #self.setOutput(("%s_out"%(self.name)))
        

    def setPar(self, prop, name):
        cval = self.Alg.getProperties()[prop]
        try:
            if type(cval) == type(list()):                
                cval.append(name)
                setattr(self.Alg, prop, cval)
            else:
                setattr(self.Alg, prop, name)
        except:
            pass

    def setParArray(self, prop, name):
        cval = self.Alg.getProperties()[prop]
        try:
            cval.append(name)
            setattr(self.Alg, prop, cval)
        except:
            pass
 
    def getPar(self, prop):
        try:
            return getattr(self.Alg, prop)
        except:
            return self.Alg.getDefaultProperty(prop)
        raise "Error in reading property " + prop + " from " + self.Alg


    def setOutput(self, name):
        return self.setPar(self.outputProp,name)

    def getOutput(self):
        return self.getPar(self.outputProp)

    def getOutputList(self):
        outputs = []
        cval = self.getOutput()
        if type(cval) == type(list()):  
            outputs.extend(cval)
        else:
            outputs.append(cval)
        return outputs
    
    def setInput(self, name):
        return self.setPar(self.inputProp,name)

    def addInput(self, name):
        return self.setParArray(self.inputProp,name)
    
    def getInput(self):
        return self.getPar(self.inputProp)

    def getInputList(self):
        inputs = []
        cval = self.getInput()
        if type(cval) == type(list()):  
            inputs.extend(cval)
        else:
            inputs.append(cval)
        return inputs

    def __str__(self):
        return "Alg::%s  [%s] -> [%s]"%(self.name,' '.join(map(str, self.getInputList())), ' '.join(map(str, self.getOutputList())))


 
class HypoAlgNode(AlgNode):
    def __init__(self, Alg, inputProp, outputProp):
        AlgNode.__init__(self, Alg, inputProp, outputProp)
        self.tools = []
        self.previous=[]
       

    def addHypoTool(self, hypotool):
        if "Comb" in self.name: ###TMP combo, only one threshold
            import re
            thresholds = map(int, re.findall(r'\d+', hypotool.getName()))
            self.setPar('Threshold1', thresholds[0])
            self.setPar('Threshold2', thresholds[1])
            status=self.setPar('DecisionLabel', hypotool.getName())
        else:
            self.tools.append(hypotool.getName())
            status= self.setParArray('HypoTools',hypotool)        
        return status

    def addPreviousDecision(self,prev):
        self.previous.append(prev)
        status= self.setParArray('previousDecisions',prev)
        return status

    def setPreviousDecision(self,prev):
        if "Comb" in self.name: ###TMP combo: how handle previous decisions in combo?            
            self.previous.append(prev)
            if  "from_L1MU" in prev:
                if "pt" in self.getPar("Property1"):
                    status= self.setPar('previousDecisions1',prev)
                if "pt" in self.getPar("Property2"):
                    status= self.setPar('previousDecisions2',prev)

            if  "from_L1EM" in prev:
                if "et" in self.getPar("Property1"):
                    status= self.setPar('previousDecisions1',prev)
                if "et" in self.getPar("Property2"):
                    status= self.setPar('previousDecisions2',prev)

            if  "Output1" in prev:
                status= self.setPar('previousDecisions1',prev)
            if  "Output2" in prev:
                status= self.setPar('previousDecisions2',prev)

        else:
            self.previous.append(prev)
            status= self.setPar('previousDecisions',prev)
        return status
    
    def __str__(self):
        return "HypoAlg::%s  [%s] -> [%s], previous = [%s], HypoTools=[%s] --> Alg %s"%(self.name,' '.join(map(str, self.getInputList())),
                                                                                 ' '.join(map(str, self.getOutputList())),
                                                                                 ' '.join(map(str, self.previous)),
                                                                                 ' '.join(map(str, self.tools)), self.Alg)



class SequenceFilterNode(AlgNode):
    def __init__(self, Alg, inputProp, outputProp):
        AlgNode.__init__(self,  Alg, inputProp, outputProp)
        
    def addDefaultOutput(self):
        print "This is SequenceFilter addDefaultOutput"
        return #do nothing    
       
    def setChains(self, name):
        return self.setPar("Chains", name)
    
    def getChains(self):
        return self.getPar("Chains")


from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRoRSeqFilter
class TestRoRSequenceFilterNode(SequenceFilterNode):       
    def __init__(self, name):
        Alg= HLTTest__TestRoRSeqFilter(name, OutputLevel = 2)
        inputProp='Inputs'
        outputProp='Outputs'
        SequenceFilterNode.__init__(self,  Alg, inputProp, outputProp)
        if "Step1" in self.name: # so that we see events running through, will be gone once L1 emulation is included
            self.Alg.AlwaysPass = True   
  
   

from DecisionHandling.DecisionHandlingConf import RoRSeqFilter, DumpDecisions
class RoRSequenceFilterNode(SequenceFilterNode):
    def __init__(self, name):
        Alg= RoRSeqFilter(name, OutputLevel = 2)
        inputProp='Input'
        outputProp='Output'
        SequenceFilterNode.__init__(self,  Alg, inputProp, outputProp)
        



#### Here functions to create the CF tree from CF configuration objects


from AthenaCommon.AlgSequence import dumpSequence
def create_step_reco_node(name, seq_list, dump=0):
    print "Create reco step %s with %d sequences"%(name, len(seq_list))
    stepCF = parOR(name+"_reco")
    for seq in seq_list:        
        step_seq = create_CFSequence(seq)             
        stepCF += step_seq
    
    if dump: dumpSequence (stepCF, indent=0)        
    return stepCF


def create_step_filter_node(name, seq_list, dump=0):
    print "Create filter step %s with %d filters"%(name, len(seq_list))
    stepCF = parOR(name+"_filter")
    for seq in seq_list:
        filterAlg=seq.filter.Alg                    
        print "Add  %s to filter node %s"%(filterAlg.name(),name)
        stepCF += filterAlg
    
    if dump: dumpSequence (stepCF, indent=0)        
    return stepCF


def create_CFSequence(CFseq):   
    print "\n * Create CFSequence %s"%(CFseq.name)
    filterAlg=CFseq.filter.Alg
   
    nodes = CFseq.getAllNodes()
    algos = []
    for node in nodes:
        algos.append(node.Alg)

    algos=list(set(algos))

    step_seq = create_step_sequence( CFseq.name, filterAlg=filterAlg, rest=algos)       
    return step_seq

def create_step_sequence(name, filterAlg, rest):
    """ elementary HLT step sequencer, filterAlg is gating, rest is anything that needs to happe within the step """
    stepReco = parOR(name+"_reco", rest)
    stepAnd = seqAND(name, [ filterAlg, stepReco ])
    return stepAnd
  

def find_stepCF_algs(step_list):
    algos = []
    for step in step_list:
        #print "Finding algos of step %s"%step
        step_algs = []
        step_algs.append(step.filter)
        for seq in step.menuSeq.nodeSeqList:
            step_algs.extend(seq.algs )
            step_algs.append(seq.hypo)
        algos.extend(step_algs)
    return algos        
            


def addChainToHypoTool(hypotool, chain):
    prop="Chains"
    cval = hypotool.getProperties()[prop]
    try:
        cval.append(chain)
        setattr(hypotool, prop, cval)
    except:
        pass


def addChainToHypoAlg(hypoAlg, chain):
    if "Comb" in hypoAlg.algname:
        prop="Chains"
        cval = hypoAlg.Alg.getProperties()[prop]
        try:
            cval.append(chain)
            setattr(hypoAlg.Alg, prop, cval)
        except:
            pass


#######################################
def decisionTree_From_Chains(HLTAllStepsSeq, chains, NSTEPS=1):

    testRoR=False
    from TrigUpgradeTest.MenuComponents import CFSequence
    #loop over chains to configure
    count_steps=0
    for nstep in range(0, NSTEPS):
        stepCF_name =  "Step%i"%(nstep+1)
        CFseq_list = []

        for chain in chains:
            chain_step_name= "%s:%s"%(stepCF_name, chain.name)
            print "\n*******Filling step %s for chain  %s"%(stepCF_name, chain.name)
      
            chain_step=chain.steps[count_steps]
            sequenceHypoTools=chain_step.sequenceHypoTools
            countseq=0
            for st in sequenceHypoTools:
                sequence=st.sequence
                hypotool=st.hypotool
                addChainToHypoTool(hypotool, chain.name)
                cfseq_name= sequence.name

                print "Going through sequence %s with threshold %s"%(sequence.name, hypotool.getName())
                #define sequence input
                if count_steps == 0: # seeding
                    sequence_input= [nodeSeq.seed for nodeSeq in sequence.nodeSeqList]
                    print "Seeds from this chain: %s"%(sequence_input)
                    previous_sequence="".join(chain.group_seed)
                else:
                    # from previous step, map the seuqence in the same order?
                    previous_sequence=chain.steps[count_steps-1].sequences[countseq].name #menu sequence
                    sequence_input=chain.steps[count_steps-1].sequences[countseq].outputs
                    print "Connect to previous sequence:"
                    print sequence_input
                # add hypotools
                for nodeSeq in sequence.nodeSeqList:
                    hypoAlg= nodeSeq.hypo                
                    print "Adding %s to %s"%(hypotool.getName(),hypoAlg.algname)
                    hypoAlg.addHypoTool(hypotool)
                    addChainToHypoAlg(hypoAlg, chain.name) # only for TMP Combo
                    


                #### FILTER
                # one filter per previous sequence at the start of the sequence: check if it exists or create a new one        
                # if the previous hypo has more than one output, try to get all of them
                # one filter per previous sequence: 1 input/previous seq, 1 output/next seq 
                filter_name="Filter%s_on_%s"%(stepCF_name,previous_sequence)
                filter_out=["%s_from_%s_out"%(filter_name,i) for i in sequence_input]
                filter_already_exists=False
                findFilter= [cfseq.filter for cfseq in CFseq_list if filter_name in cfseq.filter.algname]        
                if len(findFilter):
                    print "Filter %s already exists"%(filter_name)
                    filter_already_exists=True
                    sfilter=findFilter[0]
                    print "Adding chain %s to %s"%(chain.name,sfilter.algname)
                    sfilter.setChains(chain.name)
                    continue
                else:
                    if (testRoR):
                        sfilter = TestRoRSequenceFilterNode(name=filter_name)
                    else:
                        sfilter = RoRSequenceFilterNode(name=filter_name)
                    for o in filter_out: sfilter.setOutput(o)            
                    for i in sequence_input: sfilter.addInput(i)
                    sfilter.setChains(chain.name)
                    print "Filter Done: %s"%(sfilter)
                    
                #loop over NodeSequences of this sequence to add inputs to InputMaker and send decisions to HypoAlg
                for nodeSeq in sequence.nodeSeqList:
                    seed=nodeSeq.seed                    
                    input_maker_input= [inp for inp in sfilter.getOutputList() if ("from_"+seed) in inp]
                    print "Adding %d inputs to sequence::%s from Filter::%s (from seed %s)"%(len(input_maker_input), nodeSeq.name, sfilter.algname, seed)
                    for i in input_maker_input: print i
                    if len(input_maker_input) == 0:
#                        print "ERROR, no inputs to sequence are set!"
                        sys.exit("ERROR, no inputs to sequence are set!") 
#                        return                    
                    for i in input_maker_input: nodeSeq.addInput(i)

                    hypoAlg= nodeSeq.hypo
                    #node = AlgNode(Alg=hypoAlg.Alg,inputProp='', outputProp='Output1')
                    for i in input_maker_input: hypoAlg.setPreviousDecision(i) # works for one
                    #for i in input_maker_input: hypoAlg.addPreviousDecision(i)

                    print hypoAlg

                                    
                CF_seq = CFSequence( cfseq_name, FilterAlg=sfilter, MenuSequence=sequence)    
                #for node in otherNodes: CF_seq.addNode(node)
                print CF_seq
                CFseq_list.append(CF_seq)
                countseq+=1
           
            #end of sequence/threshold
                
        #end of loop over chains for this step, now implement CF:
        print "\n******** Create CF Tree %s with AthSequencers",stepCF_name
        #first make the filter step
        stepFilter = create_step_filter_node(stepCF_name, CFseq_list, dump=0)
        HLTAllStepsSeq += stepFilter
        
        stepCF = create_step_reco_node(stepCF_name, CFseq_list, dump=0)
        HLTAllStepsSeq += stepCF
        
        print "Now Draw..."
        stepCF_DataFlow_to_dot(stepCF_name, CFseq_list)
        stepCF_ControlFlow_to_dot(stepCF)

       
        print "Added stepCF %s to the root"%stepCF_name     
        count_steps+=1
        print "************* End of step %s"%stepCF_name
        # end of steps

    #stepCF_ControlFlow_to_dot(HLTAllStepsSeq)
    dumpSequence (HLTAllStepsSeq, indent=0)
    return
#####




###### Here some graphical methods


from ViewAlgsTest.connectAlgorithmsIO import connectAlgorithmsIO, graph_generator
from AthenaCommon.AlgSequence import AthSequencer


def algColor(alg):
    if "Hypo" in alg: return "darkorchid1"
    if "Filter" in alg: return "chartreuse3"
    if "InputMaker" in alg: return "cyan3"
    return "cadetblue1"
    
def stepCF_ControlFlow_to_dot(stepCF):
    def _dump (seq, indent):
        o = list()
        for c in seq.getChildren():
            if "AthSequencer" in c.getFullName():
                o.append( ("%s[color=%s, shape=circle, width=.5, fixedsize=true ,style=filled]\n"%(c.name(),_seqColor(c)), indent) )
            else:
                o.append( ("%s[fillcolor=%s,style=filled]\n"%(c.name(),algColor(c.name())), indent) )
            o.append( ("%s -> %s\n"%(seq.name(), c.name()), indent))
            o.extend( _dump (c, indent+1) )
        return o

    def _parOR (seq):
        if seq.ModeOR is True:
            if seq.Sequential is False:
                if seq.StopOverride is True:
                    return True
        return False

    def _seqAND(seq):
        if seq.ModeOR is False:
            if seq.Sequential is True:
                if seq.StopOverride is False:
                    return True
        return False

    def _seqColor(seq):
        if _parOR(seq): return "red"
        if _seqAND(seq): return "blue"
        return "black"

   

    # to visualize: dot -T pdf Step1.dot > Step1.pdf
    file = open( '%s.CF.dot'%stepCF.name(), mode="wt" )
    #strict
    file.write( 'digraph step  {\n'\
                +'\n'\
                +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                +'  edge [ fontname=Helvetica ]\n'
                +'  %s   [shape=Mdiamond]\n'%stepCF.name())

    indent=0
    out = [("%s[color=%s shape=circle]\n"%(stepCF.name(),_seqColor(stepCF)), indent)]
  
    out.extend( _dump( stepCF, indent=indent+1 ) )
    for n,i in out:
        line= "  "*i+ n
        file.write(line)

    file.write( '}\n')



    

def stepCF_DataFlow_to_dot(name, cfseq_list):
    # to visualize: dot -T pdf Step1.dot > Step1.pdf
    file = open( '%s.DF.dot'%name, mode="wt" )
    #strict
    file.write( 'digraph step  {\n'\
                +'\n'\
                +'  node [ shape=polygon, fontname=Helvetica ]\n'\
                +'  edge [ fontname=Helvetica ]\n'
                +'  %s   [shape=Mdiamond]\n'%name)


    for cfseq in cfseq_list:
        print cfseq.name
        file.write("  %s[fillcolor=%s style=filled]\n"%(cfseq.filter.algname,algColor(cfseq.filter.algname)))
        for inp in cfseq.filter.getInputList():
            file.write(addConnection(name, cfseq.filter.algname, inp))
           
        file.write(  '\n  subgraph cluster_%s {\n'%(cfseq.name)\
                    +'     node [color=white style=filled]\n'\
                    +'     style=filled\n'\
                    +'     color=lightgrey\n'\
                    +'     fontname=Helvetica\n'\
                    +'     label = %s\n'%(cfseq.name))

        cfseq_algs = []
        cfseq_algs.append(cfseq.filter)       

        for seq in cfseq.menuSeq.nodeSeqList:
            #            cfseq_algs.append(seq.inputMaker)
            cfseq_algs.extend(seq.algs )
            if seq.reuse==False:
                #                file.write("    %s[fillcolor=%s]\n"%(seq.inputMaker.algname, algColor(seq.inputMaker.algname)))
                for alg in seq.algs: file.write("    %s[fillcolor=%s]\n"%(alg.algname, algColor(alg.algname)))
                seq.reuse=True
            cfseq_algs.append(seq.hypo)
 
            file.write("    %s[color=%s]\n"%(seq.hypo.algname, algColor(seq.hypo.algname)))
        file.write('  }\n')              
        file.write(findConnections(cfseq_algs))
        file.write('\n')    
  
    file.write( '}')
    file.close()


def findConnections(alg_list):
    lineconnect=''
    for nodeA in alg_list:
        for nodeB in alg_list:
            if nodeA is nodeB:
                continue
            dataIntersection = list(set(nodeA.getOutputList()) & set(nodeB.getInputList()))
            if len(dataIntersection) > 0:
                for line in dataIntersection:
                    lineconnect+=addConnection(nodeA.algname,nodeB.algname, line)
                    print 'Data connections between %s and %s: %s'%(nodeA.algname, nodeB.algname, line)
    return lineconnect

def addConnection(nodeA, nodeB, label):
    line = "    %s -> %s [label=%s]\n"%(nodeA,nodeB,label)
    # print line
    return line






















