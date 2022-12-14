###############################################################
#
# Job options file to put Calibration Hits into ntuple
#
# author: Sanya Solodkov	email:  sanya.solodkov@cern.ch
#
# April, 24  Added optional Identifier checking
#            &  MC Truth
#==============================================================
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

include( "PartPropSvc/PartPropSvc.py" )

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.LAr_setOn()
DetFlags.em_setOn()
DetFlags.Tile_setOn()
DetFlags.Muon_setOff()
#
DetFlags.simulate.Truth_setOn()

DetDescrVersion = "Rome-Final"
include ("AtlasGeoModel/SetGeometryVersion.py")
include ("AtlasGeoModel/GeoModelInit.py")
GeoModelSvc.TileVersionOverride="TileCal-01"

# Initialize TileCal conditions
include( "TileConditions/TileConditions_jobOptions.py" )

# Pool Converters
#
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
include( "TileEventAthenaPool/TileEventAthenaPool_joboptions.py" )
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
# extra dictionary for calibration hits
include( "CaloSimEvent/CaloSimEventDict_joboptions.py" )

#Store Calibration Hits in CBNT
include( "CBNT_Athena/CBNT_Athena_jobOptions.py" )
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )
include( "CaloCalibHitRec/CalibHit_CBNT_jobOptions.py" )


doIDcheck = False
# Call for the checker algorithm to discover are there CalibHits Identifiers,
# which are invalid or repeated or CalibHits container type not respective
#
if doIDcheck:
    theApp.TopAlg += ["CalibHitIDCheck"]
    CalibHitIDCheck = Algorithm ( "CalibHitIDCheck")
    CalibHitIDCheck.Check    = True  #default
    CalibHitIDCheck.ShowAll  = False #default
    CalibHitIDCheck.CheckAll = True  #default is False


# Pool input
EventSelector = Service( "EventSelector" )
EventSelector.InputCollections = [ "atlas_MyOutputFile.root" ] 


writePool = False

# example how to write POOL file with results of reconstruction
# and also to copy Calibration hits from input POOL file to the output
#
if writePool:
    #--------------------------------------------------------------
    #---   Write output POOL file
    #--------------------------------------------------------------
    theApp.OutStream     =["Stream1"]
    theApp.OutStreamType ="AthenaOutputStream"
    Stream1 = Algorithm( "Stream1" )
    Stream1.EvtConversionSvc     ="AthenaPoolCnvSvc"
    #--------------------------------------------------------------
    Stream1.OutputFile = "SimplePoolFile2.root"
    Stream1.ItemList=["EventInfo#*",                       # EventInfo
                      "LArRawChannelContainer#*" ,         # LAr Raw Channels
                      "TileRawChannelContainer#*" ,        # Tile Raw Channels
                      "CaloCalibrationHitContainer#*",     # calib hits
                      "TileHitVector#*",                   # Tile hits
                      "TileHitContainer#*",                # Tile hits
                      "LArHitContainer#*",                 # LAr hits
                      "McEventCollection#*"                # mc truth (hepmc)
		]


# LAr hits
#commented because it's not in a Release yet
#
#theApp.Dlls += [ "LArG4Analysis" ]
#theApp.TopAlg += [ "LArHitAnalysis" ]
#LArHitAnalysis = Algorithm( "LArHitAnalysis" )
#LArHitAnalysis.HistLoc = "LArHitAnalysis"
#LArHitAnalysis.HistID = "4404"
#LArHitAnalysis.NTupleBase = "/NTUPLES/FILE1"
#LArHitAnalysis.NTupleLoc = "LArHitAnalysis"
#LArHitAnalysis.NTupleID = "4405"
#LArHitAnalysis.CalibrationNTupleID = "4406"

# Tile hits 
theApp.Dlls += ["TileSimAlgs"]
theApp.Dlls += ["TileRec"]
theApp.TopAlg += ["TileHitVecToCnt"]
theApp.TopAlg += ["TileHitToNtuple"]
TileHitVecToCnt = Algorithm( "TileHitVecToCnt" )
TileHitVecToCnt.HitTimeFlag=1; # reset hit time to zero

# truth information
theApp.Dlls += [ "CBNT_Truth" ]
CBNT_Athena.Members += [ "CBNT_Truth" ]
CBNT_Truth=Algorithm("CBNT_Truth")
CBNT_Truth.Members += [ "CBNT_TruthSelector/All" ]

# auditors
theApp.Dlls += [ "GaudiAud" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor"]

# Root Ntuple output

RootHistSvc = Service( "RootHistSvc" )
RootHistSvc.OutputFile = "LArG4AnalysisHist.root"

theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='CalibHits.root' OPT='NEW'" ]


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3
MessageSvc.defaultLimit = 9999999

AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.OutputLevel = 5

NovaCnvSvc = Service( "NovaCnvSvc" )
NovaCnvSvc.OutputLevel = 5


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax =  10000
#EventSelector.SkipEvents = 12
theApp.EvtSel = "EventSelector"

