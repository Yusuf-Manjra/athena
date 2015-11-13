#==============================================================
#
#
#		This job option runs the G4 simulation
#		of the ATLAS detector and the GeantinoMapping.
#		It can be run using athena.py
#
__version__="$Revision: 674388 $"
#==============================================================


#--- Algorithm sequence ---------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

#--- Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL) ----
#from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel  = INFO
ServiceMgr.MessageSvc.defaultLimit = 20000

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

import random
import time

### pass arguments with athena -c "..." ...jobOptions.py:
if 'myMomentum' not in dir() :
    myMomentum = 10000

if 'myRandomOffset' not in dir() :
    myRandomOffset = int(random.uniform(0,time.time())*0.001 )

if 'myRandomSeed1' not in dir() :
    myRandomSeed1 = int(random.uniform(0,time.time()))

if 'myRandomSeed2' not in dir() :
    myRandomSeed2 = int(random.uniform(0,time.time()))

if 'myMaxEvent' not in dir() :
    myMaxEvent = 10

if 'myPt' not in dir() :
    myPt = 'pt'  # values are 'p' or 'pt'

if 'myGeo' not in dir() :
    myGeo = 'ATLAS-R1-2012-02-01-00_VALIDATION'

print 'Random seeds and offset as calcluated by jobOptions ', myRandomSeed1, ' ', myRandomSeed2, ' offset - ', myRandomOffset


# Set everything to ATLAS
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# the global flags
globalflags.ConditionsTag = 'OFLCOND-SIM-00-00-00'
print globalflags.ConditionsTag


#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()   ### is this necessary?
athenaCommonFlags.PoolHitsOutput = 'Hits.pool.root'

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags() # Going to use an ATLAS layout
SimFlags.SimLayout = myGeo
SimFlags.EventFilter.set_Off()

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = 50000

import ParticleGun as PG
pg = PG.ParticleGun()
pg.sampler.pid = (-13, 13)
pg.sampler.mom = PG.PtEtaMPhiSampler(pt=5000, eta=[-3.5,3.5])
topSeq += pg

SimFlags.RandomSeedOffset = myRandomOffset
SimFlags.RandomSeedList.addSeed( "ParticleGun", myRandomSeed1, myRandomSeed2 )

from RngComps.RngCompsConf import AtRndmGenSvc 
myAtRndmGenSvc = AtRndmGenSvc()
myAtRndmGenSvc.Seeds = ["ParticleGun "+str(myRandomSeed1)+" "+str(myRandomSeed2) ] 
myAtRndmGenSvc.OutputLevel 	    = VERBOSE
myAtRndmGenSvc.EventReseeding   = False
ServiceMgr += myAtRndmGenSvc

# ToolSvc setup
from AthenaCommon.AppMgr import ToolSvc

from TrkValTools.TrkValToolsConf import Trk__PositionMomentumWriter as PmWriter
PmWriter = PmWriter('PosMomWriter')
ToolSvc += PmWriter

## Add an action
def energyloss_action():
    from G4AtlasApps import AtlasG4Eng,PyG4Atlas
    EnergyLossRecorder = PyG4Atlas.UserAction('TrkG4UserActions','EnergyLossRecorder', ['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(EnergyLossRecorder)

SimFlags.InitFunctions.add_function('preInitG4', energyloss_action)

############### The Material hit collection ##################

if not hasattr(ServiceMgr, 'THistSvc'):
       from GaudiSvc.GaudiSvcConf import THistSvc
       ServiceMgr += THistSvc()
# add the G4 validation output stream
ServiceMgr.THistSvc.Output += [ "val DATAFILE='/tmp/salzburg/EnergyLossRecorder.root' TYPE='ROOT' OPT='RECREATE'" ]


#from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
## --- check dictionary
#ServiceMgr.AthenaSealSvc.CheckDictionary   = True
## --- commit interval (test)
#ServiceMgr.AthenaPoolCnvSvc.OutputLevel = DEBUG
#ServiceMgr.AthenaPoolCnvSvc.CommitInterval = 10
#MaterialStream              = AthenaPoolOutputStream ( 'MaterialStream' )
#MaterialStream.OutputFile   =   "MaterialStepFile.root" 
#MaterialStream.ItemList    += [ 'MaterialStepVector#*']

##############################################################

## Populate alg sequence
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

#--- End jobOptions.GeantinoMapping.py file  ------------------------------

