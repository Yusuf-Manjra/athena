# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigBjetHypo.TrigBjetHypoConf import TrigBtagFex
#from TrigBjetHypo.TrigBtagFexTuning import *

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import mm, GeV



def getBtagFexInstance( instance, version, algo ):
    if instance=="EF" :
        return BtagFex( instance=instance, version=version, algo=algo, name="EFBtagFex_"+algo )


class BtagFex (TrigBtagFex):
    __slots__ = []
    
    def __init__(self, instance, version, algo, name):
        super( BtagFex, self ).__init__( name )
        
        mlog = logging.getLogger('BtagHypoConfig.py')
        
        AllowedInstances = ["EF"]
        AllowedVersions  = ["2012"]
        AllowedAlgos     = ["EFID"]
        
        if instance not in AllowedInstances :
            mlog.error("Instance "+instance+" is not supported!")
            return None
        
        if version not in AllowedVersions :
            mlog.error("Version "+version+" is not supported!")
            return None

#        if instance=="EF" :
#            calibInstance = "EF"
#        
#        self.par_0_MC = getTuning_par_0_MC(calibInstance)
#        self.par_1_MC = getTuning_par_1_MC(calibInstance)
#        self.par_0_DT = getTuning_par_0_DT(calibInstance)
#        self.par_1_DT = getTuning_par_1_DT(calibInstance)
#            
#        self.SizeIP1D = getTuning_SizeIP1D(calibInstance)
#        self.bIP1D    = getTuning_bIP1D   (calibInstance)
#        self.uIP1D    = getTuning_uIP1D   (calibInstance)
#        self.SizeIP2D = getTuning_SizeIP2D(calibInstance)
#        self.bIP2D    = getTuning_bIP2D   (calibInstance)
#        self.uIP2D    = getTuning_uIP2D   (calibInstance)
#        self.SizeIP3D = getTuning_SizeIP3D(calibInstance)
#        self.bIP3D    = getTuning_bIP3D   (calibInstance)
#        self.uIP3D    = getTuning_uIP3D   (calibInstance)
#        
#        self.SizeIP1D_lowSiHits = getTuning_SizeIP1D_lowSiHits(calibInstance)
#        self.bIP1D_lowSiHits    = getTuning_bIP1D_lowSiHits   (calibInstance)
#        self.uIP1D_lowSiHits    = getTuning_uIP1D_lowSiHits   (calibInstance)
#        self.SizeIP2D_lowSiHits = getTuning_SizeIP2D_lowSiHits(calibInstance)
#        self.bIP2D_lowSiHits    = getTuning_bIP2D_lowSiHits   (calibInstance)
#        self.uIP2D_lowSiHits    = getTuning_uIP2D_lowSiHits   (calibInstance)
#        self.SizeIP3D_lowSiHits = getTuning_SizeIP3D_lowSiHits(calibInstance)
#        self.bIP3D_lowSiHits    = getTuning_bIP3D_lowSiHits   (calibInstance)
#        self.uIP3D_lowSiHits    = getTuning_uIP3D_lowSiHits   (calibInstance)
#        
#        self.SizeSV   = getTuning_SizeSV  (calibInstance)
#        self.bSV      = getTuning_bSV     (calibInstance)
#        self.uSV      = getTuning_uSV     (calibInstance)
#        self.SizeMVtx = getTuning_SizeMVtx(calibInstance)
#        self.bMVtx    = getTuning_bMVtx   (calibInstance)
#        self.uMVtx    = getTuning_uMVtx   (calibInstance)
#        self.SizeEVtx = getTuning_SizeEVtx(calibInstance)
#        self.bEVtx    = getTuning_bEVtx   (calibInstance)
#        self.uEVtx    = getTuning_uEVtx   (calibInstance)
#        self.SizeNVtx = getTuning_SizeNVtx(calibInstance)
#        self.bNVtx    = getTuning_bNVtx   (calibInstance)
#        self.uNVtx    = getTuning_uNVtx   (calibInstance)
        
        self.AlgoId = -1
        
        if instance=="EF" :
            if algo=="EFID" :
                self.AlgoId = 1
        
        if self.AlgoId==-1 :
            mlog.error("AlgoId is wrongly set!")
            return None
        
        ## Unset = -1; HLT jet direction = 1; HLT track-jet direction = 2; LVL1 jet RoI direction =3
        self.UseJetDirection = -1
        
        if version=="2012" :
            self.UseJetDirection = 1
            self.RetrieveHLTJets = True
        
        if self.UseJetDirection==-1 :
            mlog.error("UseJetDirection is wrongly set!")
            return None

        ## Unset = -1; EF jets = 3
        self.TagHLTJets = -1

        if version=="2012" :
            if instance=="EF" :
                self.TagHLTJets = 3
        
        if self.TagHLTJets==-1 :
            mlog.error("TagHLTJets is wrongly set!")
            return None
        
        if instance=="EF" :
            self.Instance = "EF"
        
        self.UseBeamSpotFlag    = False
        self.SetBeamSpotWidth   = 1*mm
        self.UseParamFromData   = False
        self.useLowSiHits       = True
        
        self.UseErrIPParam      = False
        self.UseEtaPhiTrackSel  = False
        
        self.Taggers            = ["IP1D", "IP2D", "IP3D", "CHI2", "MVTX", "EVTX", "NVTX", "SVTX", "COMB"]

        if algo=="EFID" :
            self.TrkSel_Chi2    = 0.0
            self.TrkSel_BLayer  = 1
            self.TrkSel_PixHits = 2
            self.TrkSel_SiHits  = 7
            self.TrkSel_D0      = 1*mm
            self.TrkSel_Z0      = 2*mm
            self.TrkSel_Pt      = 1*GeV

        if instance=="EF" :
            from TrigBjetHypo.TrigBtagFexMonitoring import TrigEFBtagFexValidationMonitoring, TrigEFBtagFexOnlineMonitoring
            validation = TrigEFBtagFexValidationMonitoring()
            online     = TrigEFBtagFexOnlineMonitoring()    

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("TimeHistogramForTrigBtagHypo")
        time.TimerHistLimits = [0,2]
        
        self.AthenaMonTools = [ time, validation, online ]


        # IMPORT OFFLINE TOOLS
        self.setupOfflineTools = False
        if self.setupOfflineTools :
            from AthenaCommon.AppMgr import ToolSvc
            from BTagging.BTaggingFlags import BTaggingFlags
            BTaggingFlags.CalibrationTag = 'BTagCalibALL-07-09'
            BTaggingFlags.DoNotSetupBTagging = True
            from BTagging.BTaggingConfiguration import setupJetBTaggerTool
            setupJetBTaggerTool(ToolSvc, "AntiKt4LCTopo")
            from BTagging.BTaggingConfiguration import getJetCollectionTool, getJetCollectionSecVertexingTool, getJetCollectionMainAssociatorTool
            self.BTagTrackAssocTool = getJetCollectionMainAssociatorTool("AntiKt4LCTopo")
            self.BTagTool           = getJetCollectionTool("AntiKt4LCTopo")
            self.BTagSecVertexing   = getJetCollectionSecVertexingTool("AntiKt4LCTopo")
