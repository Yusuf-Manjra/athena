#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq, findAlgorithm, findOwningSequence
from AthenaCommon.AlgSequence import dumpMasterSequence
from AthenaCommon.AppMgr import theApp



from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1

flags.Detector.GeometryPixel = True     
flags.Detector.GeometrySCT   = True 
flags.Detector.GeometryTRT   = True 
flags.Detector.GeometryLAr   = True     
flags.Detector.GeometryTile  = True     
flags.Detector.GeometryMDT   = True 
flags.Detector.GeometryTGC   = True
flags.Detector.GeometryCSC   = True     
flags.Detector.GeometryRPC   = True     
flags.Trigger.writeBS=True # switches on HLTResultMT creation

flags.Trigger.CostMonitoring.doCostMonitoring = True


import importlib
setupMenuPath = "TriggerMenuMT.HLTMenuConfig.Menu."+flags.Trigger.triggerMenuSetup
setupMenuModule = importlib.import_module( setupMenuPath )
assert setupMenuModule != None, "Could not import module {}".format(setupMenuPath)
assert setupMenuModule.setupMenu != None, "Could not import setupMenu from {}".format(setupMenuPath)
flags.needFlagsCategory('Trigger')
setupMenuModule.setupMenu(flags)


flags.Input.isMC = False
flags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"] 

flags.Trigger.LVL1ConfigFile = "LVL1config_Physics_pp_v7.xml" 
flags.Trigger.L1Decoder.forceEnableAllChains = True


flags.lock()

from AthenaCommon.Constants import INFO,DEBUG,WARNING
acc = ComponentAccumulator()

from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
acc.merge(TrigBSReadCfg(flags ))


from TrigUpgradeTest.TriggerHistSvcConfig import TriggerHistSvcConfig
acc.merge(TriggerHistSvcConfig(flags ))


from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT_newJO import generateMenu
from TriggerJobOpts.TriggerConfig import triggerRunCfg
acc.merge( triggerRunCfg( flags, generateMenu ) )

from RegionSelector.RegSelConfig import regSelCfg
acc.merge( regSelCfg( flags ) )


from TrigUpgradeTest.InDetConfig import TrigInDetCondConfig
acc.merge( TrigInDetCondConfig( flags ) )

acc.getEventAlgo( "TrigSignatureMoniMT" ).OutputLevel=DEBUG
print acc.getEventAlgo( "TrigSignatureMoniMT" )


from AthenaCommon.Logging import logging
logging.getLogger('forcomps').setLevel(DEBUG)
acc.foreach_component("*/L1Decoder").OutputLevel = DEBUG
acc.foreach_component("*/L1Decoder/*Tool").OutputLevel = DEBUG # tools
acc.foreach_component("*HLTTop/*Hypo*").OutputLevel = DEBUG # hypo algs
acc.foreach_component("*HLTTop/*Hypo*/*Tool*").OutputLevel = DEBUG # hypo tools
acc.foreach_component("*HLTTop/RoRSeqFilter/*").OutputLevel = DEBUG # filters
acc.foreach_component("*HLTTop/*Input*").OutputLevel = DEBUG # input makers
acc.foreach_component("*HLTTop/*HLTEDMCreator*").OutputLevel = DEBUG # messaging from the EDM creators
acc.foreach_component("*HLTTop/*GenericMonitoringTool*").OutputLevel = WARNING # silcence mon tools (addressing by type)

acc.printConfig()

fname = "newJOtest.pkl"
print "Storing config in the config", fname
with file(fname, "w") as p:
    acc.store( p, nEvents=20, useBootStrapFile=False, threaded=True )
    p.close()
