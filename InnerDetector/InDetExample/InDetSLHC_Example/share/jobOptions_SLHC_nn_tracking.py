#--------------------------------------------------------------
# Run tracking on digit.RDO file and produce NN training ntuple.
# this is based on the standard SLHC reconstruction.
# To ge the propper output in the TrkValidation ntuple need to
# edit InDetRecNtupleCreation in InnerDetector/InDetExample/InDetRecExample
# - Tracking
#
# author: Tiago Perez <tperez@cern.ch>
# date  : 9-Jan-2012
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = WARNING
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = False
doWriteAOD      = False
# --- read BS - please look at the default input file(s) to know the det descr and cond tag
doReadBS        = False
# --- do auditors ?
doAuditors      = True

if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True 
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'geant4'
# --- input is pool for SLHC 
globalflags.InputFormat = 'pool'

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-SLHC-01-00-00'

# --- printout
globalflags.print_JobProperties()
    
from AthenaCommon.BeamFlags import jobproperties
# --- default is high luminosity for SLHC
jobproperties.Beam.numberOfCollisions = 40.0  

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-SDR-BS14T-ATLAS-00"

# --- no conditions for SLHC
if len(globalflags.ConditionsTag())!=0:
   from IOVDbSvc.CondDB import conddb
   conddb.setGlobalTag(globalflags.ConditionsTag())

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec
rec.Commissioning=False

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
DetFlags.BCM_setOff()
# --- no TRT for SLHC
DetFlags.TRT_setOff()
DetFlags.detdescr.TRT_setOff()
DetFlags.makeRIO.TRT_setOff()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.InputFormat() == 'pool')

InDetFlags.doLowBetaFinder = False

# --- uncomment to change the default of one of the following options:
#OutputLevel          = DEBUG

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'
if (hasattr(InDetFlags,"doPixelClusterSplitting")) :
  InDetFlags.doPixelClusterSplitting=False
#---slhc
InDetFlags.doTrackSegmentsPixel = False
InDetFlags.doTrackSegmentsSCT   = False
InDetFlags.doTrackSegmentsTRT   = False

InDetFlags.doNewTracking  =         True
InDetFlags.doLowPt        =         False
InDetFlags.doxKalman      =         False
InDetFlags.doiPatRec      =         False
InDetFlags.doBackTracking =         False
InDetFlags.doTRTStandalone =        False
#InDetFlags.doSingleSpBackTracking = False
InDetFlags.postProcessing =         True
InDetFlags.doBeamGas      =         False
InDetFlags.doBeamHalo     =         False
InDetFlags.doCosmics      =         False
InDetFlags.doSharedHits   =         False
# --- Turn off track slimming
InDetFlags.doSlimming = False
#---endslhc

# --- de-activate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# --- activate creation of ntuples, standard plots
InDetFlags.doTrkNtuple      = True
InDetFlags.doStandardPlots  = False
InDetFlags.doSGDeletion     = False
InDetFlags.doTrkD3PD        = False # was True
# --- activate cluster ntuple
InDetFlags.doPixelClusterNtuple = True

from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails     = 2
TrackD3PDFlags.storeTrackPredictionAtBLayer                     = False
TrackD3PDFlags.storeTrackSummary                                = True
TrackD3PDFlags.storeHitTruthMatching                            = True
TrackD3PDFlags.storeDetailedTruth                               = True

from InDetRecExample.InDetKeys import InDetKeys
InDetKeys.trkValidationNtupleName = 'TrkValidation_onTrak_ttbar_100.root'
InDetKeys.trkD3PDFileName = 'myInDetTrackD3PD.root'

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# turn on SLHC tracking
#--------------------------------------------------------------

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSLHC = True

# --- do not use DCS
InDetFlags.useDCS  = False

# --- turn off conditions
# ... missing

from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags.SLHC_Version = ''

# ---- setup of pixel tool
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
pixelTool = svcMgr.GeoModelSvc.DetectorTools['PixelDetectorTool']
# --- turn off alignment
pixelTool.Alignable = False

# --- setup of tracking geometry
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.SLHC_Geometry                   = True
TrkDetFlags.PixelBuildingOutputLevel        = WARNING
TrkDetFlags.SCT_BuildingOutputLevel         = WARNING
TrkDetFlags.TRT_BuildingOutputLevel         = WARNING
TrkDetFlags.MagneticFieldCallbackEnforced   = False
TrkDetFlags.TRT_BuildStrawLayers            = False
TrkDetFlags.MaterialFromCool                = True
TrkDetFlags.MaterialDatabaseLocal           = False and TrkDetFlags.MaterialFromCool()
TrkDetFlags.MaterialStoreGateKey            = '/GLOBAL/TrackingGeo/SLHC_LayerMaterial'
TrkDetFlags.MaterialTagBase                 = 'SLHC_LayerMat_v'
TrkDetFlags.MaterialVersion                 = 6
if SLHC_Flags.SLHC_Version() is '' :
 TrkDetFlags.MaterialMagicTag                = jobproperties.Global.DetDescrVersion()
else :
 TrkDetFlags.MaterialMagicTag                = SLHC_Flags.SLHC_Version() 
if TrkDetFlags.MaterialDatabaseLocal() is True :
   TrkDetFlags.MaterialDatabaseLocalPath    = ''
   TrkDetFlags.MaterialDatabaseLocalName    = 'SLHC_LayerMaterial-'+SLHC_Flags.SLHC_Version()+'.db'
TrkDetFlags.MagneticFieldCallbackEnforced         = False
TrkDetFlags.LArUseMaterialEffectsOnTrackProvider  = False
TrkDetFlags.TileUseMaterialEffectsOnTrackProvider = False

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")

#--------------------------------------------------------------
# try to access ntuple writer
#--------------------------------------------------------------
from InDetTrackValidation.InDetTrackValidationConf import InDet__PixelClusterValidationNtupleWriter
PixelNtupleWriter = InDet__PixelClusterValidationNtupleWriter(name                          = 'InDetPixelClusterValidationNtupleWriter',
                                                              NtupleFileName                = 'TRKVAL',
                                                              NtupleDirectoryName           = 'Validation',
                                                              #NtupleTreeName                = 'PixelRIOs', ## not needed PixelRIOs is default. 
                                                              PixelClusterContainer         = InDetKeys.PixelClusters(),
                                                              WriteDetailedPixelInformation = True,
                                                              DoHits 		            = True,
                                                              OnlyClusterOnTrack            = True, ## For OnTrack ntuples
                                                              DoMC			    = True,
                                                              WriteNNTraining               = True,
                                                              # Extra flags ONLY ON PRIVATE InDetTrackValidation/PixelClusterValidationNtupleWriter
                                                              UseToT = True, 
                                                              DetGeo = 'SLHC')

topSequence += PixelNtupleWriter
if (InDetFlags.doPrintConfigurables()):
  print PixelNtupleWriter 

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = -1
#ServiceMgr.EventSelector.SkipEvents = 2
#ServiceMgr.StoreGateSvc.Dump = True

ServiceMgr.PoolSvc.AttemptCatalogPatch=True

# --- default SLHC test file from digit output
# ServiceMgr.EventSelector.InputCollections = [ 'tperez.minbias.digit.RDO.pool.root' ]
ServiceMgr.EventSelector.InputCollections = [ 'root://eosatlas.cern.ch//eos/atlas/user/t/tperez/NNtrainSamples/ttbar_100evt.digit.RDO.pool.root' ]

# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()
