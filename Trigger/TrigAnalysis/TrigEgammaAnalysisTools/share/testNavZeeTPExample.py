# Default job options for TrigEgammaAnalysisTools
# Authors: 
# Ryan Mackenzie White <ryan.white@cern.ch>
# Denis Damazio <denis.damazio@cern.ch
# 
# Tool and algorithm configuration done using egamma Factories
# Default configurations found in TrigEgammaAnalysisToolsConfig

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)
from AthenaCommon.AppMgr import ToolSvc

import os

if not 'OUTPUT' in dir():
     outputName = 'Validation_Zee'
else:
     outputName = OUTPUT

# To run
# athena -l DEBUG -c "DIR='/afs/cern.ch/user/r/rwhite/workspace/egamma/mc/DC14Val/mc14_13TeV.147406.PowhegPythia8_AZNLO_Zee.recon.AOD.e3059_s1982_s2008_r5787_tid01572494_00'" test_NavZeeTPAll.py

dirtouse = str()
finallist = []
if 'FILE' in dir() :
     finallist.append(FILE)
else :
     if 'DIR' in dir():
          dirtouse=DIR       
     else :
          dirtouse='/afs/cern.ch/user/r/rwhite/workspace/public/validation/mc/DC14/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_tid01572823_00'
     while( dirtouse.endswith('/') ) :
          dirtouse= dirtouse.rstrip('/')
     listfiles=os.listdir(dirtouse)
     for ll in listfiles:
          finallist.append(dirtouse+'/'+ll)


athenaCommonFlags.FilesInput=finallist
#athenaCommonFlags.EvtMax=1000
athenaCommonFlags.EvtMax=-1
rec.readAOD=True
# switch off detectors
rec.doForwardDet=False
rec.doInDet=False
rec.doCalo=False
rec.doMuon=False
rec.doEgamma=False
rec.doTrigger = True; recAlgs.doTrigger=False # disable trigger (maybe necessary if detectors switched off)
rec.doMuon=False
rec.doMuonCombined=False
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doDPD=False
rec.doTruth=False


# autoconfiguration might trigger undesired feature
rec.doESD.set_Value_and_Lock(False) # uncomment if do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if do not write ESD
rec.doAOD.set_Value_and_Lock(False) # uncomment if do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if do not write TAG

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'
#ToolSvc.TrigDecisionTool.OutputLevel = VERBOSE
# Here we configure the output histogram
# And the athena algorithm, simply a loop over tools
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["%s DATAFILE='%s.root' OPT='RECREATE'" % (outputName,outputName)]
#ServiceMgr.THistSvc.Output += ["zee DATAFILE='zee.root' OPT='RECREATE'"]
#from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaTDToolTest
#topSequence+=TrigEgammaTDToolTest("TrigEgammaTDToolTest")
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaAnalysisAlg
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavZeeTPCounts,TrigEgammaNavZeeTPPerf,TrigEgammaNavZeeTPEff,TrigEgammaNavZeeTPRes,TrigEgammaNavZeeTPNtuple,TrigEgammaNavNtuple,TrigEgammaNavZeeTPIneff

from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaEmulationTool, TrigEgammaValidationTool
from TrigEgammaAnalysisTools.TrigEgammaProbelist import * # to import probelist

probelist = default 

Res = TrigEgammaNavZeeTPRes(name="NavZeeTPRes",
        ElectronKey="Electrons",
        ProbeTriggerList=default,
        TagTrigger="e26_lhtight_iloose",
        OutputLevel=2,
        )
Eff = TrigEgammaNavZeeTPEff(name="NavZeeTPEff",
        ElectronKey="Electrons",
        ProbeTriggerList=default,
        TagTrigger="e26_tight_iloose",
        )
Counts = TrigEgammaNavZeeTPCounts(name="NavZeeTPCounts",
        ElectronKey="Electrons",
        ProbeTriggerList=default,
        TagTrigger="e26_tight_iloose",
        )

Ineff = TrigEgammaNavZeeTPIneff(name="NavZeeTPIneff",
        ElectronKey="Electrons",
        ProbeTriggerList=default,
        TagTrigger="e26_lhtight_iloose",
        )

Perf = TrigEgammaNavZeeTPPerf(name="NavZeeTPPerf",
#        File = "ttbar",
        OutputLevel = 2,
        ElectronKey="Electrons",
        ProbeTriggerList=default,
        TagTrigger="e26_lhtight_iloose",
        )

NtupleZee = TrigEgammaNavZeeTPNtuple(name="NavZeeTPNtuple",
        ElectronKey="Electrons",
        ProbeTriggerList=default,
        TagTrigger="e26_lhtight_iloose",
        doRinger=False, # if its true, we will save only tes with ringer.
        )

Ntuple = TrigEgammaNavNtuple(name="NavNtuple",
        ElectronKey="Electrons",
        TriggerList=default,
        doRinger=False, # if its true, we will save only tes with ringer.
        )



#Alg = TrigEgammaAnalysisAlg(name="MyAlg",Tools=[Counts,Eff,TrigEgammaEmulationTool(OutputLevel=2)])
#Alg = TrigEgammaAnalysisAlg(name="MyAlg",Tools=[Res])
#Alg = TrigEgammaAnalysisAlg(name="MyAlg",Tools=[Counts,Eff,TrigEgammaEmulationTool(),TrigEgammaValidationTool()])
valid = TrigEgammaValidationTool(File=outputName)
# Set to True in order to get plots only with unconverted photons
# maybe set some command line parameter?
valid.doUnconverted=False 
Alg = TrigEgammaAnalysisAlg(name="MyAlg",Tools=[valid])
