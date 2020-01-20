#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

"""

"""
__doc__ = "Tool Factories to instantiate Egamma Tools for trigger"

__author__ = "Ryan Mackenzie White"
import logging
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.SystemOfUnits import GeV,MeV,deg
# New configuration for use in rel 19.X with xAOD
# Adapted from egammaRec/egammaGetter.py
# Factory tools, handles configuration of tools and dependencies
from egammaRec.Factories import Factory, ToolFactory, PublicToolFactory, FcnWrapper, getPropertyValue 

# Import tools required for trigger reconstruction
# Following offline tools not used at HLT: 
# egammaCheckEnergyDepositTool 
# EMBremCollectionBuilder
# egammaOQFlagsBuilder
# EMVertexBuilder

mlog = logging.getLogger ('TrigEgammaToolFactories')
# PID -- selectors added via TrigEgammaPIDTools in TrigEgammaHypo
from TrigEgammaHypo.TrigEgammaPidTools import ElectronPidTools
from TrigEgammaHypo.TrigEgammaPidTools import PhotonPidTools
ElectronPidTools()
PhotonPidTools()

# Following tools have TrigEgamma factories
from egammaTools.egammaToolsFactories import EMTrackMatchBuilder, EMFourMomBuilder, EMShowerBuilder
from egammaTools import egammaToolsConf

from egammaTools.egammaToolsConf import EMPIDBuilder
from CaloClusterCorrection import CaloClusterCorrectionConf as Cccc
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools
TrigCaloFillRectangularCluster = PublicToolFactory( Cccc.CaloFillRectangularCluster,
        name = "trigegamma_CaloFillRectangularCluster",
        eta_size = 5,
        phi_size = 7,
        cells_name = "")

# tool to extrapolate to the calo
from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool, Rec__ParticleCaloCellAssociationTool
#this is just regular extrapolator, but in ToolFactory form
from egammaTools.egammaExtrapolators import egammaExtrapolator
CaloExtensionTool =  ToolFactory (Trk__ParticleCaloExtensionTool,
                                  Extrapolator = egammaExtrapolator)

CaloCellAssocTool =  ToolFactory (Rec__ParticleCaloCellAssociationTool,
                                  ParticleCaloExtensionTool = CaloExtensionTool)

from AthenaCommon.GlobalFlags import globalflags
isMC = not globalflags.DataSource()=='data'
from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool as ICT
IsoCorrectionToolTrig = PublicToolFactory(ICT,
                                    name = "NewLeakageCorrToolTrig",
                                    IsMC = isMC)
from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool,xAOD__TrackIsolationTool
from CaloIdentifier import SUBCALO
TrigCaloIsolationTool = ToolFactory(xAOD__CaloIsolationTool,name = "TrigEgammaCaloIsolationTool",
        doEnergyDensityCorrection = False,
        saveOnlyRequestedCorrections = True,
        IsoLeakCorrectionTool          = IsoCorrectionToolTrig,
        CaloFillRectangularClusterTool = TrigCaloFillRectangularCluster,
        ParticleCaloExtensionTool      = CaloExtensionTool,
        ParticleCaloCellAssociationTool = CaloCellAssocTool,
        EMCaloNums = [SUBCALO.LAREM],
        HadCaloNums = [SUBCALO.LARHEC,SUBCALO.TILE])

from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
TrigCaloClustersInConeTool = PublicToolFactory(xAOD__CaloClustersInConeTool,CaloClusterLocation = "CaloCalTopoCluster")

TrigCaloTopoIsolationTool = ToolFactory(xAOD__CaloIsolationTool,name = "TrigEgammaCaloTopoIsolationTool",
        doEnergyDensityCorrection = True,
        saveOnlyRequestedCorrections = True,
        IsoLeakCorrectionTool          = IsoCorrectionToolTrig,
        ClustersInConeTool              = TrigCaloClustersInConeTool,
        CaloFillRectangularClusterTool = TrigCaloFillRectangularCluster,
        UseEMScale = True,
        ParticleCaloExtensionTool      = CaloExtensionTool,
        ParticleCaloCellAssociationTool = CaloCellAssocTool,
        TopoClusterEDCentralContainer = "HLTTopoClusterIsoCentralEventShape",
        TopoClusterEDForwardContainer = "HLTTopoClusterIsoForwardEventShape")

TrigTrackIsolationTool = ToolFactory(xAOD__TrackIsolationTool, name = 'TrigEgammaTrackIsolationTool')

TrkIsoCfg = CfgMgr.xAOD__TrackIsolationTool('TrigEgammaTrackIsolationTool')
TrkIsoCfg.TrackSelectionTool.maxZ0SinTheta = 3.
TrkIsoCfg.TrackSelectionTool.minPt = 1000.
TrkIsoCfg.TrackSelectionTool.CutLevel = "Loose"

TrigElectronPIDBuilder = PublicToolFactory( EMPIDBuilder, name = "TrigElectronPIDBuilder",
    electronIsEMselectors = [ToolSvc.AsgElectronIsEMVLooseSelector,
        ToolSvc.AsgElectronIsEMLooseSelector,
        ToolSvc.AsgElectronIsEMMediumSelector,
        ToolSvc.AsgElectronIsEMTightSelector],
            
    electronIsEMselectorResultNames = ["VLoose","Loose","Medium","Tight"],
    electronLHselectors = [ToolSvc.AsgElectronLHVLooseSelector,
        ToolSvc.AsgElectronLHLooseSelector,
        ToolSvc.AsgElectronLHMediumSelector,
        ToolSvc.AsgElectronLHTightSelector],
    electronLHselectorResultNames= ["LHVLoose","LHLoose","LHMedium","LHTight"],
    UseLuminosityTool = True,
)

TrigElectronCaloPIDBuilder = PublicToolFactory( EMPIDBuilder, name = "TrigElectronCaloPIDBuilder",
    electronLHselectors = [ToolSvc.AsgElectronLHVLooseCaloSelector,
        ToolSvc.AsgElectronLHLooseCaloSelector,
        ToolSvc.AsgElectronLHMediumCaloSelector,
        ToolSvc.AsgElectronLHTightCaloSelector],
    electronLHselectorResultNames= ["LHCaloVLoose","LHCaloLoose","LHCaloMedium","LHCaloTight"],
    LHValueName = "LHCaloValue",
    UseLuminosityTool = True,
)

TrigPhotonPIDBuilder = PublicToolFactory( EMPIDBuilder, name = "TrigPhotonPIDBuilder",
    photonIsEMselectors= [ToolSvc.AsgPhotonIsEMLooseSelector,
        ToolSvc.AsgPhotonIsEMMediumSelector,
        ToolSvc.AsgPhotonIsEMTightSelector],
    photonIsEMselectorResultNames = ["Loose","Medium","Tight"],
)

from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools
TrigEMExtrapolationTools=EMExtrapolationTools.copyPublic(name="TrigEMExtrapolationTools",
                                                        useCaching=False)


def appendtoTrigEMTrackMatchBuilder(tool):
    "add track to calo tool "
    if not hasattr(tool,"EMExtrapolationTools"):
        tool += EMExtrapolationTools()

TrigEMTrackMatchBuilder = EMTrackMatchBuilder.copyPublic(
    name = "TrigEMTrackMatchBuilder",
    postInit=[appendtoTrigEMTrackMatchBuilder],
    broadDeltaEta      = 0.2, #For offline 0.1
    broadDeltaPhi      = 0.2, #For offline 0.15
    useScoring         = False, 
    SecondPassRescale  = False,
    ExtrapolationTool  = TrigEMExtrapolationTools
)

TrigEMShowerBuilder = EMShowerBuilder.copyPublic(
  name = "TrigEgammaShowerBuilder", 
  CellsName = "",
  Print = True,
)

from TriggerMenu.egamma.EgammaSliceFlags import EgammaSliceFlags
from egammaMVACalib.TrigEgammaMVACalibFactories import TrigElectronMVATool, TrigPhotonMVATool

mlog.info("MVA version version %s"%EgammaSliceFlags.calibMVAVersion() )
mlog.info("Cluster Correction version %s"%EgammaSliceFlags.clusterCorrectionVersion() )
EgammaSliceFlags.calibMVAVersion.set_On()


from TrigCaloRec.TrigCaloRecConf import TrigCaloClusterMaker

def configureTrigCaloClusterMonitoring(tool):
    from TrigCaloRec.TrigCaloClusterMakerMonitoring import TrigCaloClusterMakerValidationMonitoring, TrigCaloClusterMakerOnlineMonitoring, TrigCaloClusterMakerCosmicMonitoring
    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    clvalidation = TrigCaloClusterMakerValidationMonitoring()
    clonline = TrigCaloClusterMakerOnlineMonitoring()
    clcosmic = TrigCaloClusterMakerCosmicMonitoring()
    cltime = TrigTimeHistToolConfig("TrigCaloClusterMaker_Time")
    tool.AthenaMonTools = [ clvalidation, clonline, cltime, clcosmic]

def configureClusterBuilder(slwAlg):
    slwName="trigslw"
    if hasattr(slwAlg,slwName): 
        return
    from CaloRec.CaloRecMakers import make_CaloClusterBuilderSW
    trigslw= make_CaloClusterBuilderSW (slwName,
            tower_container = "LArTowerEM",
            eta_size = 3,
            phi_size = 5,
            e_threshold = 2.5 * GeV,
            FillClusterCells = False,
            eta_Duplicate = 5,
            phi_Duplicate = 5
            )
    slwAlg += trigslw
    slwAlg.ClusterMakerTools=[ trigslw.getFullName() ]

def configureClusterCorrections(slwAlg):
    '''Add attributes ClusterCorrectionToolsXX to egammaSwTool object'''
    if not hasattr(slwAlg,"ClusterCorrectionTools"):
        return
    from CaloClusterCorrection.CaloSwCorrections import  make_CaloSwCorrections
    clusterTypes = ("ele37","ele55")
    for cl in clusterTypes:
        clName = "CaloRunClusterCorrections"+cl
        if hasattr(slwAlg,clName):
            continue
        for tool in make_CaloSwCorrections (cl,version=EgammaSliceFlags.clusterCorrectionVersion()):
            slwAlg += tool
            slwAlg.ClusterCorrectionTools += [tool.getFullName()]


from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not hasattr (condSeq,"LArHVCondAlg"):
    from LArRecUtils.LArRecUtilsConf import LArHVCondAlg
    hvcond = LArHVCondAlg(doHV=False,doAffectedHV=False)
    condSeq+=hvcond

TrigCaloClusterMaker_slw = Factory(TrigCaloClusterMaker,name='TrigCaloClusterMaker_slw',ClustersOutputName="TriggerClustersegSW",
        postInit=[configureClusterBuilder,configureClusterCorrections,configureTrigCaloClusterMonitoring])

del mlog
