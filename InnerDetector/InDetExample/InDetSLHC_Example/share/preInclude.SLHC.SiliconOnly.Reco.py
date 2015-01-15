include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.SiliconOnly.py")

rec.doTrigger=False
rec.doCalo=False
rec.doJetMissingETTag=False
rec.doMuonCombined=False
rec.doEgamma=False

## Magnet is on
from AthenaCommon.DetFlags import DetFlags
DetFlags.BField_setOn()

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.SLHC_Geometry=True

## MaterialStoreGateKey
TrkDetFlags.MaterialStoreGateKey="/GLOBAL/TrackingGeo/LayerMaterialITK"

## MaterialMagicTag
from AthenaCommon.GlobalFlags import jobproperties
DetDescrVersion = jobproperties.Global.DetDescrVersion()
# splitGeo = DetDescrVersion.split('-')
# TrkDetFlags.MaterialMagicTag = splitGeo[0] + '-' + splitGeo[1] + '-' + splitGeo[2]
TrkDetFlags.MaterialMagicTag = DetDescrVersion

## MaterialVersion and MaterialSubVersion
TrkDetFlags.MaterialVersion=17
TrkDetFlags.MaterialSubVersion=""

## To use the local material map
TrkDetFlags.MaterialDatabaseLocal=False
if TrkDetFlags.MaterialDatabaseLocal():
    TrkDetFlags.MaterialDatabaseLocalPath="./" 
    TrkDetFlags.MaterialDatabaseLocalName="AtlasLayerMaterial-"+DetDescrVersion+".db"

InDetFlags.doSLHC=True
InDetFlags.doForwardTracks=True
InDetFlags.doSlimming=False
InDetFlags.doBeamHalo=False
InDetFlags.doStatistics=True
InDetFlags.useDCS=False
InDetFlags.doBremRecovery=True
InDetFlags.doCaloSeededBrem=False

InDetFlags.doSGDeletion=False # This should be False. 
InDetFlags.doPixelClusterSplitting=False # This should be False.
InDetFlags.doLowBetaFinder=False # This should be False. 

if rec.OutputFileNameForRecoStep() == 'RAWtoESD':
    InDetFlags.writeRDOs=True

# InDetFlags.doStandardPlots=True 

# if hasattr(runArgs,"outputDESDM_TRACKFile"):
#     from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
#     TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails     = 2
#     TrackD3PDFlags.storeTrackPredictionAtBLayer                     = False
#     TrackD3PDFlags.storeTrackSummary                                = True
#     TrackD3PDFlags.storeHitTruthMatching                            = True
#     TrackD3PDFlags.storeDetailedTruth                               = True
#     InDetFlags.doTrkD3PD.set_Value_and_Lock(True) 
#     from InDetRecExample.InDetKeys import InDetKeys
#     InDetKeys.trkD3PDFileName.set_Value_and_Lock(runArgs.outputDESDM_TRACKFile) 

# if rec.OutputFileNameForRecoStep() == 'ESDtoAOD': 
#     InDetFlags.doNewTracking=False
#     InDetFlags.doVertexFinding=False
#     InDetFlags.doPRDFormation=False
#     InDetFlags.doParticleCreation=False
#     InDetFlags.disableInDetReco.set_Value_and_Lock(False)
#     InDetFlags.doStatistics=False
#     InDetFlags.disableTracking=False
#     InDetFlags.doForwardTracks=False
#     InDetFlags.doSGDeletion=True

# InDetFlags.doPrintConfigurables.set_Value_and_Lock(True)
