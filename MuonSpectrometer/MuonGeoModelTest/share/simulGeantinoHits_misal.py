if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-R2-2015-02-01-00_VALIDATION"
    printfunc ("MuonGeoModelTest/simulGeantinoHits Setting now DetDescrVersion to ", DetDescrVersion)
else:
    printfunc ("MuonGeoModelTest/simulGeantinoHits DetDescrVersion already set to ", DetDescrVersion)

# now it's necessary to specify the global condition tag
from AthenaCommon.GlobalFlags import globalflags
if not 'ConditionsTag' in dir():
   ConditionsTag="OFLCOND-RUN12-SDR-22"
   printfunc ("MuonGeoModelTest/simulGeantinoHits Setting now ConditionsTag to ", ConditionsTag)
else:
    printfunc ("MuonGeoModelTest/simulGeantinoHits DetDescrVersion already set to ", ConditionsTag)
                    
#globalflags.ConditionsTag = 'OFLCOND-SIM-01-00-00'
globalflags.ConditionsTag = ConditionsTag


#==============================================================
#
# Job options file for Geant4 Simulations
# modified version of jobOptions.G4Atlas_Sim.py (in Simulation/G4Atlas/G4AtlasApps/share)
#
# Atlas simulation 
#
__version__="$Revision: 1.5 $"
#==============================================================

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
#
DetFlags.simulate.Truth_setOn()
#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.PoolHitsOutput='MuonHits.root'
athenaCommonFlags.EvtMax=1000

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
simFlags.SimLayout.set_On()
simFlags.SimLayout.set_Value(DetDescrVersion) # specific value
#simFlags.SimLayout.set_On()                  # use the default value
#  sets the EtaPhi, VertexSpread and VertexRange checks on
simFlags.EventFilter.set_Off()
simFlags.RunNumber=222500
#
printfunc ("Reading alignment constants from DB")
from IOVDbSvc.CondDB import conddb
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/BARREL','/MUONALIGN/MDT/BARREL',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEA','/MUONALIGN/MDT/ENDCAP/SIDEA',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEC','/MUONALIGN/MDT/ENDCAP/SIDEC',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEA','/MUONALIGN/TGC/SIDEA',className='CondAttrListCollection')
conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEC','/MUONALIGN/TGC/SIDEC',className='CondAttrListCollection')

from AthenaCommon.AlgSequence import AthSequencer
condSequence = AthSequencer("AthCondSeq")
from MuonCondAlg.MuonCondAlgConf import MuonAlignmentCondAlg
condSequence+=MuonAlignmentCondAlg('MuonAlignmentCondAlg')
MuonAlignmentCondAlg.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                                       "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                       "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                       "/MUONALIGN/TGC/SIDEA",
                                       "/MUONALIGN/TGC/SIDEC"]

from AtlasGeoModel.MuonGM import GeoModelSvc
#GeoModelSvc.MuonVersionOverride = "MuonSpectrometer-R.07.01"
MuonDetectorTool = GeoModelSvc.DetectorTools[ "MuonDetectorTool" ]
MuonDetectorTool.UseConditionDb = 1
MuonDetectorTool.OutputLevel=VERBOSE

## Run ParticleGun
import ParticleGun as PG
pg = PG.ParticleGun(randomStream="SINGLE", randomSeed = simFlags.RandomSeedOffset.get_Value())
pg.sampler.pid = (999)
pg.sampler.pos = PG.PosSampler(x=0, y=0, z=0, t=0)
pg.sampler.mom = PG.EEtaMPhiSampler(energy=100000, eta=[-3,3], phi=[-PG.PI, PG.PI])
topSeq += pg

include("G4AtlasApps/G4Atlas.flat.configuration.py")

## Add G4 sim framework alg sequence
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
topSeq.G4AtlasAlg.InputTruthCollection = "GEN_EVENT"

#---  Output printout level ----------------------------------- 
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#you can override this for individual modules if necessary
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3
