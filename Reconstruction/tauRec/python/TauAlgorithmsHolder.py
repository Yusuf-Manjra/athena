# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

################################################################################
#
#@file TauAlgorithmsHolder.py
#
#@brief All tau algorithms needed for tau reconstruction are configured here.
#
################################################################################

from AthenaCommon.SystemOfUnits import GeV, mm
from tauRec.tauRecFlags import tauFlags

cached_instances = {}

sPrefix = 'tauRec_'
bAODmode = False

# standard container names
_DefaultVertexContainer = "PrimaryVertices"
_DefaultTrackContainer ="InDetTrackParticles"
_DefaultLargeD0TrackContainer ="InDetLargeD0TrackParticles"

######################################################################## 
def setPrefix(prefix): 
    global sPrefix 
    sPrefix = prefix 
    
######################################################################## 
def setAODmode(mode): 
    global bAODmode 
    bAODmode = mode


########################################################################
# Atlas Extrapolator
def getAtlasExtrapolator():
    _name = sPrefix + 'theAtlasExtrapolator'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    #Configure the extrapolator
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    theAtlasExtrapolator=AtlasExtrapolator(name = _name)
    theAtlasExtrapolator.DoCaloDynamic = False # this turns off dynamic
    
    ToolSvc += theAtlasExtrapolator
    cached_instances[_name] = theAtlasExtrapolator
    return theAtlasExtrapolator

########################################################################
# JetSeedBuilder
def getJetSeedBuilder():
    _name = sPrefix + 'JetSeedBuilder'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import JetSeedBuilder
    JetSeedBuilder = JetSeedBuilder(name = _name)
            
    cached_instances[_name] = JetSeedBuilder
    return JetSeedBuilder

########################################################################
# Tau energy calibration and tau axis direction
def getTauAxis():
    _name = sPrefix + 'TauAxis'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauAxisSetter
    TauAxisSetter = TauAxisSetter(  name = _name, 
                                    ClusterCone = 0.2,
                                    VertexCorrection = True )
                                    
    cached_instances[_name] = TauAxisSetter                
    return TauAxisSetter

########################################################################
# Tau energy calibration
def getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix=''):
 
    _name = sPrefix +'EnergyCalibrationLC' + postfix
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauCalibrateLC
    TauCalibrateLC = TauCalibrateLC(name = _name,
                                    calibrationFile = tauFlags.tauRecCalibrateLCConfig(),
                                    Key_vertexInputContainer = _DefaultVertexContainer)
            
    cached_instances[_name] = TauCalibrateLC                
    return TauCalibrateLC

########################################################################
# Tau cell variables calculation
def getCellVariables(cellConeSize=0.2, prefix=''):
    #if prefix is not given, take global one 
    if not prefix: 
        prefix=sPrefix 
 
    _name = prefix + 'CellVariables'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauCellVariables
    TauCellVariables = TauCellVariables(name = _name,
            StripEthreshold = 0.2*GeV,
            CellCone = cellConeSize,
            VertexCorrection = True)
            
    cached_instances[_name] = TauCellVariables   
    return TauCellVariables

########################################################################
# ParticleCaloExtensionTool
def getParticleCaloExtensionTool():
    _name = sPrefix + 'ParticleCaloExtensionTool'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
    
    tauParticleCaloExtensionTool = Trk__ParticleCaloExtensionTool(name = _name, Extrapolator = getAtlasExtrapolator())
    
    ToolSvc += tauParticleCaloExtensionTool  
    cached_instances[_name] = tauParticleCaloExtensionTool
    return tauParticleCaloExtensionTool   

                
########################################################################
########################################################################
# Tracking Tools
# TODO: rearrange
########################################################################


########################################################################
# TauFullLinearizedTrackFactory
def getTauFullLinearizedTrackFactory():
    _name = sPrefix + 'TauFullLinearizedTrackFactory'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
    TauFullLinearizedTrackFactory=Trk__FullLinearizedTrackFactory(name = _name, Extrapolator = getAtlasExtrapolator())
    
    ToolSvc += TauFullLinearizedTrackFactory
    cached_instances[_name] = TauFullLinearizedTrackFactory
    return TauFullLinearizedTrackFactory

########################################################################
# TauCrossDistancesSeedFinder
def getTauCrossDistancesSeedFinder():
    _name = 'TauCrossDistancesSeedFinder'

    from AthenaCommon.AppMgr import ToolSvc

    if _name in cached_instances:
        return cached_instances[_name]

    #first the seed finder utils
    from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__SeedNewtonTrkDistanceFinder
    TauNewtonTrkDistanceFinder = Trk__SeedNewtonTrkDistanceFinder( name = sPrefix+'TauSeedNewtonTrkDistanceFinder')
    ToolSvc += TauNewtonTrkDistanceFinder
    
    #then the seed finder tools
    from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
    TauCrossDistancesSeedFinder = Trk__CrossDistancesSeedFinder( name = _name, TrkDistanceFinder=TauNewtonTrkDistanceFinder)

    cached_instances[_name] = TauCrossDistancesSeedFinder
    ToolSvc +=TauCrossDistancesSeedFinder
    return TauCrossDistancesSeedFinder

########################################################################
# TauAdaptiveVertexFitter
def getTauAdaptiveVertexFitter():
    _name = sPrefix + 'TauAdaptiveVertexFitter'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    #then the fitter utils
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator
    TauInDetImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name = sPrefix+'TauTrkImpactPoint3dEstimator', Extrapolator = getAtlasExtrapolator())
    ToolSvc += TauInDetImpactPoint3dEstimator
    
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__DetAnnealingMaker
    TauDetAnnealingMaker = Trk__DetAnnealingMaker(name = sPrefix+'TauDetAnnealingMaker', SetOfTemperatures = [ 64, 32, 16, 8, 4, 2, 1 ] )
    ToolSvc += TauDetAnnealingMaker
    
    #then the fitters (smoother + adaptive with smoothing + fast billoir)
    from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
    TauSequentialVertexSmoother = Trk__SequentialVertexSmoother(name = sPrefix+'TauSequentialVertexSmoother')
    ToolSvc += TauSequentialVertexSmoother
     
    from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveVertexFitter
    TauAdaptiveVertexFitter = Trk__AdaptiveVertexFitter(name = _name, 
                                                        SeedFinder=getTauCrossDistancesSeedFinder(), 
                                                        ImpactPoint3dEstimator=TauInDetImpactPoint3dEstimator, 
                                                        VertexSmoother=TauSequentialVertexSmoother, 
                                                        AnnealingMaker=TauDetAnnealingMaker,
                                                        LinearizedTrackFactory=getTauFullLinearizedTrackFactory())
    
    cached_instances[_name] = TauAdaptiveVertexFitter
    ToolSvc +=TauAdaptiveVertexFitter
    return TauAdaptiveVertexFitter

########################################################################
# TauTrackToVertexIPEstimator
def getTauTrackToVertexIPEstimator():
    _name = sPrefix + 'TauTrackToVertexIPEstimator'
    
    from AthenaCommon.AppMgr import ToolSvc
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
    TauTrackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(name = _name,
                                                                Extrapolator=getAtlasExtrapolator(),
                                                                LinearizedTrackFactory=getTauFullLinearizedTrackFactory())
    cached_instances[_name] = TauTrackToVertexIPEstimator
    ToolSvc += TauTrackToVertexIPEstimator
    return TauTrackToVertexIPEstimator                                     

#########################################################################
# Tau Variables
# TODO: rename + rearrange
def getTauCommonCalcVars():
    _name = sPrefix + 'TauCommonCalcVars'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauCommonCalcVars
    TauCommonCalcVars = TauCommonCalcVars(name = _name)
    
    cached_instances[_name] = TauCommonCalcVars    
    return TauCommonCalcVars

#########################################################################
# Tau Vertex Variables
def getTauVertexVariables():
    _name = sPrefix + 'TauVertexVariables'
    
    if _name in cached_instances:
        return cached_instances[_name]

    from tauRecTools.tauRecToolsConf import TauVertexVariables
    TauVertexVariables = TauVertexVariables(  name = _name,
                                              VertexFitter = getTauAdaptiveVertexFitter(),
                                              SeedFinder = getTauCrossDistancesSeedFinder(),
                                              )
    
    cached_instances[_name] = TauVertexVariables    
    return TauVertexVariables

#########################################################################
# Tau Variables
# TODO: rename + rerrange
def getTauSubstructure():
    _name = sPrefix + 'TauSubstructure'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauSubstructureVariables
    TauSubstructureVariables = TauSubstructureVariables(name = _name)
    
    cached_instances[_name] = TauSubstructureVariables
    return TauSubstructureVariables

#########################################################################
# ele veto variables
def getElectronVetoVars():
    _name = sPrefix + 'TauElectronVetoVars'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauElectronVetoVariables
    TauElectronVetoVariables = TauElectronVetoVariables(name = _name,
                                                        VertexCorrection = True,
                                                        ParticleCaloExtensionTool = getParticleCaloExtensionTool(),
                                                        tauEVParticleCache = getParticleCache())
    
    cached_instances[_name] = TauElectronVetoVariables
    return TauElectronVetoVariables


#########################################################################
# cell weight tool
def getCellWeightTool():
    _name = sPrefix + 'CellWeightTool'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    #from CaloClusterCorrection.CaloClusterCorrectionConf import H1WeightToolCSC12Generic    
    from CaloRec.CaloTopoClusterFlags import jobproperties
    # -- auto configure weight tool
    finder = jobproperties.CaloTopoClusterFlags.cellWeightRefFinder.get_Value()
    size   = jobproperties.CaloTopoClusterFlags.cellWeightRefSize.get_Value()
    signal = jobproperties.CaloTopoClusterFlags.cellWeightRefSignal.get_Value()
    
    from CaloClusterCorrection.StandardCellWeightCalib import getCellWeightTool
    CaloWeightTool = getCellWeightTool(finder,size,signal)
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += CaloWeightTool
    
    cached_instances[_name] = CaloWeightTool
    return CaloWeightTool

#########################################################################
# Pi0 algo
# Cluster finder for Pi0 algo
def getPi0ClusterFinder():    
    _name = sPrefix + 'Pi0ClusterFinder'
    
    if _name in cached_instances:
        return cached_instances[_name]

    from tauRecTools.tauRecToolsConf import TauPi0CreateROI
    TauPi0CreateROI = TauPi0CreateROI(name = _name,
                                      #        CaloWeightTool = getCellWeightTool(),
                                      #        ExtrapolateToCaloTool = getExtrapolateToCaloTool(),
                                      Key_caloCellInputContainer="AllCalo"
                                      )
    
    cached_instances[_name] = TauPi0CreateROI
    return TauPi0CreateROI

#####################
# create Pi0 clusters
def getPi0ClusterCreator():
    _name = sPrefix + 'Pi0ClusterCreator'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from tauRecTools.tauRecToolsConf import TauPi0ClusterCreator
    TauPi0ClusterCreator = TauPi0ClusterCreator(name = _name)
    
    cached_instances[_name] = TauPi0ClusterCreator
    return TauPi0ClusterCreator

#####################
# Set energy of cluster to take care of charged pion energy deposited in the ECAL
def getPi0ClusterScaler(): 
    _name = sPrefix + 'Pi0ClusterScaler'

    if _name in cached_instances:
        return cached_instances[_name]

    from tauRecTools.tauRecToolsConf import TauPi0ClusterScaler
    TauPi0ClusterScaler = TauPi0ClusterScaler(name = _name)

    cached_instances[_name] = TauPi0ClusterScaler
    return TauPi0ClusterScaler

#####################
# calculate MVA scores of pi0 clusters
def getPi0ScoreCalculator():
    _name = sPrefix + 'Pi0ScoreCalculator'

    if _name in cached_instances:
        return cached_instances[_name]

    from tauRecTools.tauRecToolsConf import TauPi0ScoreCalculator
    TauPi0ScoreCalculator = TauPi0ScoreCalculator(name = _name,
        BDTWeightFile = 'TauPi0BDTWeights.root',
        )

    cached_instances[_name] = TauPi0ScoreCalculator
    return TauPi0ScoreCalculator

#####################
# select pi0 clusters
def getPi0Selector():
    _name = sPrefix + 'Pi0Selector'

    if _name in cached_instances:
        return cached_instances[_name]

    from tauRec.tauRecFlags import jobproperties
    pi0EtCuts = jobproperties.tauRecFlags.pi0EtCuts()
    pi0MVACuts_1prong = jobproperties.tauRecFlags.pi0MVACuts_1prong()
    pi0MVACuts_mprong = jobproperties.tauRecFlags.pi0MVACuts_mprong()
     

    from tauRecTools.tauRecToolsConf import TauPi0Selector
    TauPi0Selector = TauPi0Selector(name = _name,
        ClusterEtCut         = pi0EtCuts,
        ClusterBDTCut_1prong = pi0MVACuts_1prong,
        ClusterBDTCut_mprong = pi0MVACuts_mprong,
        )

    cached_instances[_name] = TauPi0Selector
    return TauPi0Selector

#########################################################################
# Photon Shot Finder algo
def getTauShotFinder():    
    _name = sPrefix + 'TauShotFinder'
    
    if _name in cached_instances:
        return cached_instances[_name]

    from tauRec.tauRecFlags import jobproperties
    shotPtCut_1Photon = jobproperties.tauRecFlags.shotPtCut_1Photon()
    shotPtCut_2Photons = jobproperties.tauRecFlags.shotPtCut_2Photons()

    from tauRecTools.tauRecToolsConf import TauShotFinder
    TauShotFinder = TauShotFinder(name = _name,
                                  CaloWeightTool = getCellWeightTool(),
                                  NCellsInEta           = 5,
                                  MinPtCut              = shotPtCut_1Photon,
                                  AutoDoubleShotCut     = shotPtCut_2Photons,
                                  Key_caloCellInputContainer="AllCalo"
                                  )
    cached_instances[_name] = TauShotFinder
    return TauShotFinder


#########################################################################
def getInDetTrackSelectorTool():
    _name = sPrefix + 'InDetTrackSelectorTool'  
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    #Configures tau track selector tool (should eventually check whether an existing one is available)
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
    InDetTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = _name,
                                                                pTMin                = 1000.,
                                                                IPd0Max              = 1.,
                                                                IPz0Max              = 1.5, 
                                                                useTrackSummaryInfo  = True,
                                                                nHitBLayer           = 0, 
                                                                nHitPix              = 2,  # PixelHits + PixelDeadSensors
                                                                nHitSct              = 0,  # SCTHits + SCTDeadSensors
                                                                nHitSi               = 7,  # PixelHits + SCTHits + PixelDeadSensors + SCTDeadSensors
                                                                nHitTrt              = 0,  # nTRTHits
                                                                useSharedHitInfo     = False,
                                                                nSharedBLayer        = 99999,
                                                                nSharedPix           = 99999,
                                                                nSharedSct           = 99999,
                                                                nSharedSi            = 99999,
                                                                useTrackQualityInfo  = True,
                                                                fitChi2OnNdfMax      = 99999,
                                                                TrackSummaryTool     = None,
                                                                Extrapolator         = getAtlasExtrapolator())
                                                                
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += InDetTrackSelectorTool
    
    cached_instances[_name] = InDetTrackSelectorTool
    return InDetTrackSelectorTool


#########################################################################
def getInDetTrackSelectionToolForTJVA():
    _name = sPrefix + 'InDetTrackSelectionToolForTJVA'  
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    #Configures tau track selector tool (should eventually check whether an existing one is available)
    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    InDetTrackSelectionToolForTJVA = InDet__InDetTrackSelectionTool(name = _name,
                                                                    minPt                = 1000.,
                                                                    maxD0                = 9999.*mm,
                                                                    maxZ0                = 9999.*mm,                                                                 
                                                                    minNPixelHits        = 2,  # PixelHits + PixelDeadSensors
                                                                    minNSctHits          = 0,  # SCTHits + SCTDeadSensors
                                                                    minNSiHits           = 7,  # PixelHits + SCTHits + PixelDeadSensors + SCTDeadSensors
                                                                    minNTrtHits          = 0)
                                                                    #fitChi2OnNdfMax      = 99999,
                                                                    #TrackSummaryTool     = None,
                                                                    #Extrapolator         = getAtlasExtrapolator())
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += InDetTrackSelectionToolForTJVA
    
    cached_instances[_name] = InDetTrackSelectionToolForTJVA
    return InDetTrackSelectionToolForTJVA


############################################################################
def getInDetTrackSelectorToolxAOD():
    _name = sPrefix + 'IInDetTrackSelectionTool'  
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    #Configures tau track selection tool (should eventually check whether an existing one is available)
    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    myInDetTrackSelectionTool = InDet__InDetTrackSelectionTool(name = _name,
                                                               CutLevel="TightPrimary",
                                                               )
                                                                
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += myInDetTrackSelectionTool
    
    cached_instances[_name] = myInDetTrackSelectionTool
    return myInDetTrackSelectionTool
    
#########################################################################
def getTauVertexFinder(doUseTJVA=False):
    _name = sPrefix + 'TauVertexFinder'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    #if doUseTJVA:
    #    setupTauJVFTool()
    
    # Algorithm that overwrites numTrack() and charge() of all tauJets in the container
    from tauRecTools.tauRecToolsConf import TauVertexFinder
    TauVertexFinder = TauVertexFinder(name = _name,
                                      UseTJVA                 = doUseTJVA,
                                      UseTJVA_Tiebreak        = tauFlags.doTJVA_Tiebreak(),
                                      AssociatedTracks="GhostTrack", # OK??
                                      InDetTrackSelectionToolForTJVA = getInDetTrackSelectionToolForTJVA(),
                                      Key_trackPartInputContainer=_DefaultTrackContainer,
                                      Key_vertexInputContainer = _DefaultVertexContainer,
                                      TVATool = getTVATool()
                                      )
    
    cached_instances[_name] = TauVertexFinder         
    return TauVertexFinder 

#########################################################################
def getTrackToVertexTool():
    _name = sPrefix + 'TrackToVertexTool'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
    TrackToVertexTool = Reco__TrackToVertex( name = _name,
                                             Extrapolator = getAtlasExtrapolator())
                                             
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += TrackToVertexTool
    
    cached_instances[_name] = TrackToVertexTool
    return TrackToVertexTool

########################################################################
# Tau-Track Association
def getTauTrackFinder(removeDuplicateTracks=True):
    _name = sPrefix + 'TauTrackFinder'
    
    if _name in cached_instances:
        return cached_instances[_name] 
    
    from tauRecTools.tauRecToolsConf import TauTrackFinder
    TauTrackFinder = TauTrackFinder(name = _name,
                                    MaxJetDrTau = 0.2,
                                    MaxJetDrWide          = 0.4,
                                    TrackSelectorToolTau  = getInDetTrackSelectorTool(),
                                    TrackToVertexTool         = getTrackToVertexTool(),
                                    ParticleCaloExtensionTool = getParticleCaloExtensionTool(),
                                    tauParticleCache = getParticleCache(),
                                    removeDuplicateCoreTracks = removeDuplicateTracks,
                                    Key_trackPartInputContainer = _DefaultTrackContainer,
                                    Key_LargeD0TrackInputContainer = _DefaultLargeD0TrackContainer if tauFlags.associateLRT() else "",
                                    TrackToVertexIPEstimator = getTauTrackToVertexIPEstimator(),
                                    #maxDeltaZ0wrtLeadTrk = 2, #in mm
                                    #removeTracksOutsideZ0wrtLeadTrk = True
                                    )
    
    cached_instances[_name] = TauTrackFinder      
    return TauTrackFinder


# Associate the cluster in jet constituents to the tau candidate
def getTauClusterFinder():
    _name = sPrefix + 'TauClusterFinder'

    if _name in cached_instances:
        return cached_instances[_name]
  
    from JetRec.JetRecFlags import jetFlags

    doJetVertexCorrection = False
    if tauFlags.isStandalone():
        doJetVertexCorrection = True
    if jetFlags.useVertices() and jetFlags.useTracks():
        doJetVertexCorrection = True

    from tauRecTools.tauRecToolsConf import TauClusterFinder
    TauClusterFinder = TauClusterFinder(name = _name,
                                        JetVertexCorrection = doJetVertexCorrection)

    cached_instances[_name] = TauClusterFinder
    return TauClusterFinder


########################################################################
# MvaTESVariableDecorator
def getMvaTESVariableDecorator():
    _name = sPrefix + 'MvaTESVariableDecorator'
    from tauRecTools.tauRecToolsConf import MvaTESVariableDecorator
    MvaTESVariableDecorator = MvaTESVariableDecorator(name = _name,
                                                      Key_vertexInputContainer=_DefaultVertexContainer,
                                                      VertexCorrection = True)
    cached_instances[_name] = MvaTESVariableDecorator
    return MvaTESVariableDecorator

########################################################################
# MvaTESEvaluator
def getMvaTESEvaluator():
    _name = sPrefix + 'MvaTESEvaluator'
    from tauRecTools.tauRecToolsConf import MvaTESEvaluator
    MvaTESEvaluator = MvaTESEvaluator(name = _name,
                                      WeightFileName = tauFlags.tauRecMvaTESConfig())
    cached_instances[_name] = MvaTESEvaluator
    return MvaTESEvaluator

########################################################################
# TauCombinedTES
def getTauCombinedTES():
    _name = sPrefix + 'TauCombinedTES'
    from tauRecTools.tauRecToolsConf import TauCombinedTES
    TauCombinedTES = TauCombinedTES(name = _name,
                                    WeightFileName = tauFlags.tauRecCombinedTESConfig())
    cached_instances[_name] = TauCombinedTES
    return TauCombinedTES
    

########################################################################
def getTauTrackRNNClassifier():
    _name = sPrefix + 'TauTrackRNNClassifier'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from AthenaCommon.AppMgr import ToolSvc
    from tauRecTools.tauRecToolsConf import tauRecTools__TauTrackRNNClassifier as TauTrackRNNClassifier
    from tauRecTools.tauRecToolsConf import tauRecTools__TrackRNN as TrackRNN

    import cppyy
    cppyy.load_library('libxAODTau_cDict')

    _RNN = TrackRNN(name = _name + "_0",
                    InputWeightsPath = tauFlags.tauRecRNNTrackClassificationConfig()[0],
                    calibFolder = tauFlags.tauRecToolsCVMFSPath(), 
                   )
    ToolSvc += _RNN
    cached_instances[_RNN.name] = _RNN

    _classifyLRT = True
    if tauFlags.associateLRT() and not tauFlags.classifyLRT():
        _classifyLRT = False

    # create tool alg
    myTauTrackClassifier = TauTrackRNNClassifier( name = _name,
                                                  Classifiers = [_RNN],
                                                  classifyLRT = _classifyLRT )

    cached_instances[_name] = myTauTrackClassifier 
    return myTauTrackClassifier


def getTauJetRNNEvaluator():
    _name = sPrefix + 'TauJetRNN'
    from tauRecTools.tauRecToolsConf import TauJetRNNEvaluator
    myTauJetRNNEvaluator = TauJetRNNEvaluator(name = _name,
                                              NetworkFile0P = "",
                                              NetworkFile1P = tauFlags.tauRecTauJetRNNConfig()[0],
                                              NetworkFile3P = tauFlags.tauRecTauJetRNNConfig()[1],
                                              OutputVarname = "RNNJetScore",
                                              MaxTracks = 10,
                                              MaxClusters = 6,
                                              MaxClusterDR = 1.0,
                                              VertexCorrection = True,
                                              InputLayerScalar = "scalar",
                                              InputLayerTracks = "tracks",
                                              InputLayerClusters = "clusters",
                                              OutputLayer = "rnnid_output",
                                              OutputNode = "sig_prob")

    cached_instances[_name] = myTauJetRNNEvaluator
    return myTauJetRNNEvaluator


def getTauWPDecoratorJetRNN():
    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.load_library('libxAODTau_cDict')

    _name = sPrefix + 'TauWPDecoratorJetRNN'
    from tauRecTools.tauRecToolsConf import TauWPDecorator
    myTauWPDecorator = TauWPDecorator( name=_name,
                                       flatteningFile1Prong = "rnnid_mc16d_flat_1p.root",
                                       flatteningFile3Prong = "rnnid_mc16d_flat_3p.root",
                                       CutEnumVals =
                                       [ ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigVeryLoose, ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigLoose,
                                         ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigMedium, ROOT.xAOD.TauJetParameters.IsTauFlag.JetRNNSigTight ],
                                       SigEff1P = [0.95, 0.85, 0.75, 0.60],
                                       SigEff3P = [0.95, 0.75, 0.60, 0.45],
                                       ScoreName = "RNNJetScore",
                                       NewScoreName = "RNNJetScoreSigTrans",
                                       DefineWPs = True,
                                       )
    cached_instances[_name] = myTauWPDecorator
    return myTauWPDecorator


def getTauIDVarCalculator():
    _name = sPrefix + 'TauIDVarCalculator'
    from tauRecTools.tauRecToolsConf import TauIDVarCalculator   

    myTauIDVarCalculator = TauIDVarCalculator(name=_name,
                                              VertexCorrection = True)
    cached_instances[_name] = myTauIDVarCalculator
    return myTauIDVarCalculator

def getTauEleRNNEvaluator():
    _name = sPrefix + 'TauEleRNN' 
    from tauRecTools.tauRecToolsConf import TauJetRNNEvaluator
    tool = TauJetRNNEvaluator(name = _name,
                              NetworkFile1P = tauFlags.tauRecTauEleRNNConfig()[0],
                              NetworkFile3P = tauFlags.tauRecTauEleRNNConfig()[1],
                              OutputVarname = "RNNEleScore",
                              MaxTracks = 10,
                              MaxClusters = 6,
                              MaxClusterDR = 1.0,
                              VertexCorrection = True,
                              InputLayerScalar = "scalar",
                              InputLayerTracks = "tracks",
                              InputLayerClusters = "clusters",
                              OutputLayer = "rnneveto_output",
                              OutputNode = "sig_prob")

    cached_instances[_name] = tool
    return tool

def getTauWPDecoratorEleRNN():
    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.load_library('libxAODTau_cDict')

    _name = sPrefix + 'TauWPDecoratorEleRNN'
    from tauRecTools.tauRecToolsConf import TauWPDecorator
    myTauWPDecorator = TauWPDecorator( name=_name,
                                       flatteningFile1Prong="rnneveto_mc16d_flat_1p.root",
                                       flatteningFile3Prong="rnneveto_mc16d_flat_3p.root",
                                       CutEnumVals =
                                       [ ROOT.xAOD.TauJetParameters.IsTauFlag.EleRNNLoose,
                                         ROOT.xAOD.TauJetParameters.IsTauFlag.EleRNNMedium,
                                         ROOT.xAOD.TauJetParameters.IsTauFlag.EleRNNTight ],
                                       SigEff1P = [0.95, 0.90, 0.85],
                                       SigEff3P = [0.98, 0.95, 0.90],
                                       UseEleBDT = True ,
                                       ScoreName = "RNNEleScore",
                                       NewScoreName = "RNNEleScoreSigTrans",
                                       DefineWPs = True,
                                       )
    cached_instances[_name] = myTauWPDecorator
    return myTauWPDecorator
              
def getTauDecayModeNNClassifier():
    _name = sPrefix + 'TauDecayModeNNClassifier'

    if _name in cached_instances:
        return cached_instances[_name]

    from tauRecTools.tauRecToolsConf import TauDecayModeNNClassifier
    TauDecayModeNNClassifier = TauDecayModeNNClassifier(name=_name, WeightFile=tauFlags.tauRecDecayModeNNClassifierConfig())
    cached_instances[_name] = TauDecayModeNNClassifier
    return TauDecayModeNNClassifier

def getTauVertexedClusterDecorator():
    from tauRec.tauRecFlags import tauFlags
    from tauRecTools.tauRecToolsConf import TauVertexedClusterDecorator

    _name = sPrefix + 'TauVertexedClusterDecorator'
    
    if _name in cached_instances:
        return cached_instances[_name]
  
    myTauVertexedClusterDecorator = TauVertexedClusterDecorator(name = _name,
                                                                SeedJet = tauFlags.tauRecSeedJetCollection(), 
                                                                VertexCorrection = True)
    
    cached_instances[_name] = myTauVertexedClusterDecorator
    return myTauVertexedClusterDecorator

def getParticleCache():
    #If reading from ESD we not create a cache of extrapolations to the calorimeter, so we should signify this by setting the cache key to a null string
    from RecExConfig.RecFlags import rec
    if rec.doESD():
        ParticleCache = "ParticleCaloExtension"
    else: 
        ParticleCache = ""
    
    return ParticleCache

def getTVATool():
    _name = sPrefix + "TVATool"
    if _name in cached_instances:
        return cached_instances[_name]

    from TrackVertexAssociationTool.getTTVAToolForReco import getTTVAToolForReco
    TVATool = getTTVAToolForReco(name = _name,
                                 WorkingPoint = "Nonprompt_Hard_MaxWeight",
                                 TrackContName = _DefaultTrackContainer,
                                 VertexContName= _DefaultVertexContainer)

    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += TVATool

    cached_instances[_name] = TVATool
    return TVATool



# deprecated in R22

def getTauTrackClassifier():
    _name = sPrefix + 'TauTrackClassifier'
    
    if _name in cached_instances:
        return cached_instances[_name]
    
    from AthenaCommon.AppMgr import ToolSvc
    from tauRecTools.tauRecToolsConf import tauRecTools__TauTrackClassifier as TauTrackClassifier
    from tauRecTools.tauRecToolsConf import tauRecTools__TrackMVABDT as TrackMVABDT

    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.load_library('libxAODTau_cDict.so')

    # =========================================================================
    _BDT_TTCT_ITFT_0 = TrackMVABDT(name = _name + "_0",
                                   #InputWeightsPath = "TMVAClassification_BDT.weights.root",
                                   #Threshold      = -0.005,
                                   InputWeightsPath = tauFlags.tauRecMVATrackClassificationConfig()[0][0],
                                   Threshold = tauFlags.tauRecMVATrackClassificationConfig()[0][1],
                                   ExpectedFlag   = ROOT.xAOD.TauJetParameters.TauTrackFlag.unclassified, 
                                   SignalType     = ROOT.xAOD.TauJetParameters.TauTrackFlag.classifiedCharged, 
                                   BackgroundType = ROOT.xAOD.TauJetParameters.TauTrackFlag.classifiedIsolation,
                                   calibFolder = tauFlags.tauRecToolsCVMFSPath(), 
                                   )
    ToolSvc += _BDT_TTCT_ITFT_0
    cached_instances[_BDT_TTCT_ITFT_0.name] = _BDT_TTCT_ITFT_0
    
    _BDT_TTCT_ITFT_0_0 = TrackMVABDT(name = _name + "_0_0",
                                     #InputWeightsPath = "TMVAClassification_BDT_0.weights.root",
                                     #Threshold      = -0.0074,
                                     InputWeightsPath = tauFlags.tauRecMVATrackClassificationConfig()[1][0],
                                     Threshold = tauFlags.tauRecMVATrackClassificationConfig()[1][1],
                                     ExpectedFlag   = ROOT.xAOD.TauJetParameters.TauTrackFlag.classifiedCharged,
                                     SignalType     = ROOT.xAOD.TauJetParameters.TauTrackFlag.classifiedCharged,
                                     BackgroundType = ROOT.xAOD.TauJetParameters.TauTrackFlag.classifiedConversion,
                                     calibFolder = tauFlags.tauRecToolsCVMFSPath(),
                                     )
    ToolSvc += _BDT_TTCT_ITFT_0_0
    cached_instances[_BDT_TTCT_ITFT_0_0.name] = _BDT_TTCT_ITFT_0_0
    
    _BDT_TTCT_ITFT_0_1 = TrackMVABDT(name = _name + "_0_1",
                                     #InputWeightsPath = "TMVAClassification_BDT_1.weights.root",
                                     #Threshold      = 0.0005,
                                     InputWeightsPath = tauFlags.tauRecMVATrackClassificationConfig()[2][0],
                                     Threshold = tauFlags.tauRecMVATrackClassificationConfig()[2][1],
                                     ExpectedFlag   = ROOT.xAOD.TauJetParameters.TauTrackFlag.classifiedIsolation, 
                                     SignalType     = ROOT.xAOD.TauJetParameters.TauTrackFlag.classifiedIsolation, 
                                     BackgroundType = ROOT.xAOD.TauJetParameters.TauTrackFlag.classifiedFake,
                                     calibFolder = tauFlags.tauRecToolsCVMFSPath(),
                                     )
    ToolSvc += _BDT_TTCT_ITFT_0_1
    cached_instances[_BDT_TTCT_ITFT_0_1.name] = _BDT_TTCT_ITFT_0_1

    # create tool alg
    myTauTrackClassifier = TauTrackClassifier( name = _name,
                                               Classifiers = [_BDT_TTCT_ITFT_0, _BDT_TTCT_ITFT_0_0, _BDT_TTCT_ITFT_0_1] )
    #ToolSvc += TauTrackClassifier #only add to tool service sub tools to your tool, the main tool will be added via TauRecConfigured
    cached_instances[_name] = myTauTrackClassifier 

    return myTauTrackClassifier


def getTauJetBDTEvaluator(_n, weightsFile="", minNTracks=0, maxNTracks=10000, outputVarName="BDTJetScore", minAbsTrackEta=-1, maxAbsTrackEta=-1):
    _name = sPrefix + _n
    from tauRecTools.tauRecToolsConf import TauJetBDTEvaluator
    myTauJetBDTEvaluator = TauJetBDTEvaluator(name=_name,
                                              weightsFile=weightsFile,
                                              minNTracks=minNTracks,
                                              maxNTracks=maxNTracks,
                                              minAbsTrackEta=minAbsTrackEta,
                                              maxAbsTrackEta=maxAbsTrackEta,
                                              outputVarName=outputVarName)
    cached_instances[_name] = myTauJetBDTEvaluator
    return myTauJetBDTEvaluator


def getTauWPDecoratorJetBDT():
    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.load_library('libxAODTau_cDict')

    _name = sPrefix + 'TauWPDecoratorJetBDT'
    from tauRecTools.tauRecToolsConf import TauWPDecorator
    myTauWPDecorator = TauWPDecorator( name=_name,
                                       flatteningFile1Prong = "FlatJetBDT1Pv2.root",
                                       flatteningFile3Prong = "FlatJetBDT3Pv2.root",
                                       CutEnumVals = 
                                       [ ROOT.xAOD.TauJetParameters.IsTauFlag.JetBDTSigVeryLoose, ROOT.xAOD.TauJetParameters.IsTauFlag.JetBDTSigLoose,
                                         ROOT.xAOD.TauJetParameters.IsTauFlag.JetBDTSigMedium, ROOT.xAOD.TauJetParameters.IsTauFlag.JetBDTSigTight ],
                                       SigEff1P = [0.95, 0.85, 0.75, 0.60],
                                       SigEff3P = [0.95, 0.75, 0.60, 0.45],
                                       ScoreName = "BDTJetScore",
                                       NewScoreName = "BDTJetScoreSigTrans",
                                       DefineWPs = True,
                                       )
    cached_instances[_name] = myTauWPDecorator
    return myTauWPDecorator


def getTauWPDecoratorEleBDT():
    import PyUtils.RootUtils as ru
    ROOT = ru.import_root()
    import cppyy
    cppyy.load_library('libxAODTau_cDict')

    _name = sPrefix + 'TauWPDecoratorEleBDT'
    from tauRecTools.tauRecToolsConf import TauWPDecorator
    TauScoreFlatteningTool = TauWPDecorator( name=_name,
                                             flatteningFile1Prong = "EleBDTFlat1P.root",
                                             flatteningFile3Prong = "EleBDTFlat3P.root",
                                             UseEleBDT = True ,
                                             ScoreName = "BDTEleScore",
                                             NewScoreName = "BDTEleScoreSigTrans",
                                             DefineWPs = True,
                                             CutEnumVals = 
                                             [ROOT.xAOD.TauJetParameters.IsTauFlag.EleBDTLoose, 
                                              ROOT.xAOD.TauJetParameters.IsTauFlag.EleBDTMedium, 
                                              ROOT.xAOD.TauJetParameters.IsTauFlag.EleBDTTight],
                                             SigEff1P = [0.95, 0.85, 0.75],
                                             SigEff3P = [0.95, 0.85, 0.75],
                                             ) 
    cached_instances[_name] = TauScoreFlatteningTool
    return TauScoreFlatteningTool
