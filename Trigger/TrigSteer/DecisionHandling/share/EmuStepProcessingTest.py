###############################################################
#
# Job options file
#
# Based on AthExStoreGateExamples
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------
from __future__ import print_function

# input parameters:
class opt:
    doMenu          = False # use either menu or manual chain building



from AthenaCommon.Logging import logging
from TriggerJobOpts.TriggerFlags import TriggerFlags
from DecisionHandling.EmuStepProcessingConfig import generateHLTSeedingAndChainsManually, generateHLTSeedingAndChainsByMenu
from AthenaCommon.AlgSequence import AlgSequence
from TriggerMenuMT.HLTMenuConfig.Menu.HLTMenuJSON import generateJSON

# signatures
# steps: sequential AND of 1=Filter 2=Processing
# chainstep=single chain step
# global step=joint for all chains
# filters: one SeqFilter per step, per chain
# inputMakers: one per each first RecoAlg in a step (so one per step), one input per chain that needs that step

log = logging.getLogger('EmuStepProcessingTest.py')
log.info('Setup options:')
defaultOptions = [a for a in dir(opt) if not a.startswith('__')]
for option in defaultOptions:
    if option in globals():
        setattr(opt, option, globals()[option])
        log.info(' %20s = %s' % (option, getattr(opt, option)))
    else:        
        log.info(' %20s = (Default) %s' % (option, getattr(opt, option)))

from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg,HLTConfigSvcCfg
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from AthenaConfiguration.AllConfigFlags import ConfigFlags

ConfigFlags.Trigger.triggerMenuSetup = TriggerFlags.triggerMenuSetup= 'LS2_v1'
ConfigFlags.Trigger.generateMenuDiagnostics = True

CAtoGlobalWrapper(L1ConfigSvcCfg,ConfigFlags)
CAtoGlobalWrapper(HLTConfigSvcCfg,ConfigFlags)


topSequence = AlgSequence()

if opt.doMenu is True:
    generateHLTSeedingAndChainsByMenu(topSequence)
else:
    generateHLTSeedingAndChainsManually(topSequence)

generateJSON()




from AthenaCommon.AppMgr import theApp, ServiceMgr as svcMgr
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
if hasattr(svcMgr.THistSvc, "Output"):
    from TriggerJobOpts.TriggerHistSvcConfig import setTHistSvcOutput
    setTHistSvcOutput(svcMgr.THistSvc.Output)

print ("EmuStepProcessing: dump top Sequence after CF/DF Tree build")
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence( topSequence )

from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectDecisionObjects, collectHypoDecisionObjects, triggerOutputCfg
from AthenaCommon.CFElements import findAlgorithm,findSubSequence
hypos = collectHypos(findSubSequence(topSequence, "HLTAllSteps"))
filters = collectFilters(findSubSequence(topSequence, "HLTAllSteps"))

nfilters = sum(len(v) for v in filters.values())
nhypos = sum(len(v) for v in hypos.values())
log.info( "Algorithms counting: Number of Filter algorithms: %d  -  Number of Hypo algorithms: %d", nfilters , nhypos) 

#dumpMasterSequence()

theApp.EvtMax = 4

