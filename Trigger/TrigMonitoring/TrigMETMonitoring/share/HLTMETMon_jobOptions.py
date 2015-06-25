#############################################################
# File   : Job Options to run monitoring in stand-alone mode 
# Author : Venkat.Kaushik@cern.ch
# Date   : Jan 2010
#############################################################
doLocal = True

import sys

##############
# Input files
##############
InputFiles = ['AOD.SampleA.ttbar.root',] ##AOD_new.root',]

#mc15

#Wplusenu
#InputFiles = ['/afs/cern.ch/user/y/ychiu/work/MonitoringMay13/test/mc15data/mc15_13TeV.361100.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wplusenu.merge.AOD.e3601_s2576_s2132_r6630_r6264_tid05367769_00/AOD.05367769._000216.pool.root.1'];

#Wplusmunu
#InputFiles = ['/afs/cern.ch/user/y/ychiu/work/MonitoringMay13/test/mc15data/mc15_13TeV.361101.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wplusmunu.merge.AOD.e3601_s2576_s2132_r6630_r6264_tid05371399_00/AOD.05371399._000167.pool.root.1'];

#minbias
#InputFiles = ['/afs/cern.ch/user/y/ychiu/work/MonitoringMay13/test/mc15data/mc15_13TeV.361203.Pythia8_A2_MSTW2008LO_ND_minbias.merge.AOD.e3639_s2601_s2132_r6630_r6264_tid05405009_00/AOD.05405009._000059.pool.root.1'];

#ttbar
#InputFiles = ['/afs/cern.ch/user/y/ychiu/work/MonitoringMay13/test/mc15data/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r6630_r6264_tid05382619_00/AOD.05382619._000139.pool.root.1'];



############################
# Auto configure everything
############################
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections=InputFiles
ServiceMgr.PoolSvc.AttemptCatalogPatch = True;
ServiceMgr.OutputLevel=ERROR

#
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

if len(athenaCommonFlags.FilesInput()) == 0:
  athenaCommonFlags.FilesInput = ServiceMgr.EventSelector.InputCollections
    
#
import AthenaPoolCnvSvc.ReadAthenaPool
    
import AthenaPoolCnvSvc.WriteAthenaPool
    
#
from RecExConfig.RecFlags import rec

rec.AutoConfiguration          .set_Value_and_Lock(["everything"])

rec.abortOnUncheckedStatusCode .set_Value_and_Lock(False)
    
rec.doCBNT                     .set_Value_and_Lock(False)
rec.doESD                      .set_Value_and_Lock(False)
rec.doHist                     .set_Value_and_Lock(False)
rec.doPerfMon                  .set_Value_and_Lock(False)

rec.doWriteAOD                 .set_Value_and_Lock(False)
rec.doWriteESD                 .set_Value_and_Lock(False)
rec.doWriteTAG                 .set_Value_and_Lock(False)
rec.doWriteTAGCOM              .set_Value_and_Lock(False)

#
from AthenaCommon.Include import include
    
include ("RecExCommon/RecExCommon_topOptions.py")

#
for i in ServiceMgr:  i.OutputLevel=INFO

##############################
# Output file
##############################
OutputFile = 'Output_HLT_MetMon.root'

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += ["MonitorHLTmet DATAFILE='" + OutputFile + "' OPT='RECREATE'"]

##############################
# Add the Toplevel Alg
##############################

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

###############################
# Configure the job
###############################

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'

from RecExConfig.RecFlags import rec
rec.readRDO = False
rec.readAOD = True
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False

###############################
# Setup Athena Monitoring
###############################
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

if not 'DQMonFlags' in dir():
  from AthenaMonitoring.DQMonFlags import DQMonFlags

HLTmetOutputLevel = INFO
myTrigDecisionTool = ""
if (DQMonFlags.useTrigger() and hasattr(ToolSvc, DQMonFlags.nameTrigDecTool())):
  myTrigDecisionTool = getattr(ToolSvc, DQMonFlags.nameTrigDecTool())
else:
  from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
  ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
  ToolSvc.TrigDecisionTool.OutputLevel=ERROR
  ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR

HLTmetMonMan = AthenaMonManager(name="HLTMonManager",
                                FileKey             = DQMonFlags.monManFileKey(),
                                Environment         = DQMonFlags.monManEnvironment(),
                                OutputLevel         = HLTmetOutputLevel)

topSequence += HLTmetMonMan

# in order to check available detectors
from RecExConfig.RecFlags  import rec

try:
  from TrigMETMonitoring.TrigMETMonitoringConfig import HLTMETMonitoringTool
  HLTmetMonMan.AthenaMonTools += HLTMETMonitoringTool()
except:
  print "Problems with HLTMETTool, tool not enabled"

HLTmetMonMan.FileKey = "MonitorHLTmet"

print HLTmetMonMan

##########################
# Events to run over
##########################
theApp.EvtMax = -1

