# ---------------------------------------------------------------------- 
# JO file for RTT
# ---------------------------------------------------------------------- 

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = INFO

from TruthExamples.TruthExamplesConf import *
from PyJobTransformsCore.runargs import *
runArgs = RunArguments()

outputFileName="RTT_Pythia8_Ztautau.root"
print "Output file name: ", outputFileName

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg") 

from EvgenJobTransforms.EvgenConfig import *

# ----------------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()

# Add HepMCAnalysis algorithms
from HepMCAnalysis_i.HepMCAnalysis_iConfig import HepMCAnalysis_i

myHepMCAnalysis = HepMCAnalysis_i("HepMCAnalysis_i", file = outputFileName)
myHepMCAnalysis.EtmissAnalysis=True
myHepMCAnalysis.ZtautauAnalysis=True
myHepMCAnalysis.ZAnalysis=True
myHepMCAnalysis.JetAnalysis=True

topAlg += myHepMCAnalysis
