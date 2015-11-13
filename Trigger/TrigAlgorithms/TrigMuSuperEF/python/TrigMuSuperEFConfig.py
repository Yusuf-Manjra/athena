# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from TrigMuSuperEFMonitoring import TrigMuSuperEFMonitoring,TrigMuSuperEFValidationMonitoring
from TrigMuonEF.TrigMuonEFMonitoring import TrigMuonEFStandaloneToolMonitoring,TrigMuonEFCombinerToolMonitoring,TrigMuonEFStandaloneToolValidationMonitoring,TrigMuonEFCombinerToolValidationMonitoring
from TrigMuonEF.TrigMuonEFConf import TrigMuonEFTrackIsolationTool

from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
from TrigMuSuperEFConf import TrigMuSuperEF
from AthenaCommon.BeamFlags import jobproperties

from AthenaCommon.CfgGetter import getPublicTool
from AthenaCommon import CfgMgr
from AthenaCommon.SystemOfUnits import mm

#
# Default config: RoI based, Combined, TrigMuonEF only
# Old default:  RoI based, Combined, TrigMuonEF first, TrigMuGirl is TrigMuonEF fails. Should be put back once TrigMuGirl is back in release
#
class TrigMuSuperEFConfig(TrigMuSuperEF):
    __slots__ = ()

    def __init__(self,name="TrigMuSuperEF",**kwargs):
        kwargs.setdefault("doInsideOut", True)
        kwargs.setdefault("doOutsideIn", True)
        kwargs.setdefault("insideOutFirst", False)
        kwargs.setdefault("fullScan", False)
        kwargs.setdefault("StandaloneOnly", False)
        kwargs.setdefault("CombinerOnly", False)
        kwargs.setdefault("CaloTagOnly", False)
        kwargs.setdefault("TMEF_standaloneTrackTool", "TrigMuonEFStandaloneTrackTool")
        kwargs.setdefault("MuonCombinedTool","TMEF_MuonCombinedTool")
        kwargs.setdefault("TrkToTrackParticleConvTool","TMEF_TrkToTrackParticleConvTool")
        kwargs.setdefault("MuonCreatorTool","TMEF_MuonCreatorTool")
        kwargs.setdefault("deltaEtaRoI",0.2)
        kwargs.setdefault("deltaPhiRoI",0.2)
        kwargs.setdefault("UseL2Info",False)
        kwargs.setdefault("DoCache", True)

        doTrigMuonEF     = kwargs["doOutsideIn"]
        doTrigMuGirl     = kwargs["doInsideOut"]
        doStandaloneOnly = kwargs["StandaloneOnly"]
        doFullScan       = kwargs["fullScan"]
        doCaloTagOnly    = kwargs["CaloTagOnly"]
        combinerOnly     = kwargs["CombinerOnly"]
        doCosmics        = jobproperties.Beam.beamType == 'cosmics'
        
        # turn on seeded data decoding by default
        TriggerFlags.MuonSlice.doEFRoIDrivenAccess = True
        
        # make instance
        super(TrigMuSuperEFConfig,self).__init__(name,**kwargs)

        # setup monitoring depending on configuration
        monTools = []
        # top level histograms use Combined Muons
        if not doStandaloneOnly:
            monTools.append( TrigMuSuperEFMonitoring() )
            monTools.append( TrigMuSuperEFValidationMonitoring() )
        # only add TrigMuonEF monitoring if it is run
        if doTrigMuonEF:
            if not combinerOnly:
                monTools.append( TrigMuonEFStandaloneToolMonitoring() )
                monTools.append( TrigMuonEFStandaloneToolValidationMonitoring() )
            if not doStandaloneOnly:
                monTools.append( TrigMuonEFCombinerToolMonitoring() )
                monTools.append( TrigMuonEFCombinerToolValidationMonitoring() )

        # only add TrigMuGirl monitoring if it is run
        if doTrigMuGirl:
            from AthenaCommon.CfgGetter import getPublicTool,getPublicToolClone
            self.StauCreatorTool = getPublicToolClone("TMEF_StauCreatorTool","TMEF_MuonCreatorTool",BuildStauContainer=True)
            self.MuGirlTool = getPublicTool("TrigMuGirlTagTool")
            #from TrigMuGirl.TrigMuGirlMonitoring import TrigMuGirlToolMonitoring
            #montool = TrigMuGirlToolMonitoring()
            #print montool
            #monTools.append( montool )

        # turn off PrepRawData decoders in MuGirl
        if doTrigMuGirl:
            from MuGirlCandidate.MuGirlCandidateConf import MuGirlNS__CandidateTool
            MuGirlNS__CandidateTool.doDecoding = False

        if self.UseL2Info:
            self.TMEF_standaloneTrackTool.useL2Hits=True
        else:
            self.TMEF_standaloneTrackTool.useL2Hits=False


        # always add timing monitoring
        timetool =  TrigTimeHistToolConfig("Time") 
        timetool.NumberOfHistBins = 100
        timetool.TimerHistLimits = [0,1000]
        monTools.append( timetool )
        
        self.AthenaMonTools = monTools
    #  end of __init__       


#
# Several pre-configured RoI based variations
#
def TrigMuSuperEF(name="TrigMuSuperEF",**kwargs):
    return TrigMuSuperEFConfig(name,**kwargs)
    
def TrigMuSuperEF_MGfirst(name="TrigMuSuperEF_MGfirst",**kwargs):

    kwargs.setdefault("doInsideOut", True)
    kwargs.setdefault("doOutsideIn", True)
    kwargs.setdefault("insideOutFirst", True)
    return TrigMuSuperEFConfig(name,**kwargs)


def TrigMuSuperEF_TMEFfirst(name="TrigMuSuperEF_TMEFfirst",**kwargs):
    kwargs.setdefault("doInsideOut", True)
    kwargs.setdefault("doOutsideIn", True)
    kwargs.setdefault("insideOutFirst", False)
    return TrigMuSuperEFConfig(name,**kwargs)


def TrigMuSuperEF_MGonly(name="TrigMuSuperEF_MGonly",**kwargs):
    kwargs.setdefault("doInsideOut", True)
    kwargs.setdefault("doOutsideIn", False)
    kwargs.setdefault("UseL2Info",False)
    return TrigMuSuperEFConfig(name,**kwargs)


def TrigMuSuperEF_TMEFonly(name="TrigMuSuperEF_TMEFonly",**kwargs):
    kwargs.setdefault("doInsideOut", False)
    kwargs.setdefault("doOutsideIn", True)
    kwargs.setdefault("UseL2Info",False)
    return TrigMuSuperEFConfig(name,**kwargs)


def TrigMuSuperEF_both(name="TrigMuSuperEF_both",**kwargs):
    kwargs.setdefault("doInsideOut", True)
    kwargs.setdefault("doOutsideIn", True)
    kwargs.setdefault("forceBoth", True)
    return TrigMuSuperEFConfig(name,**kwargs)


def TrigMuSuperEF_SAonly(name="TrigMuSuperEF_SAonly",**kwargs):
    kwargs.setdefault("doInsideOut", False)
    kwargs.setdefault("doOutsideIn", True)
    kwargs.setdefault("StandaloneOnly", True)
    return TrigMuSuperEFConfig(name,**kwargs)

def TrigMuSuperEF_TMEFCombinerOnly(name="TrigMuSuperEF_TMEFCombinerOnly",**kwargs):
    kwargs.setdefault("doInsideOut", False)
    kwargs.setdefault("doOutsideIn", True)
    kwargs.setdefault("CombinerOnly", True)
    kwargs.setdefault("UseL2Info",False)
    return TrigMuSuperEFConfig(name,**kwargs)


def TrigMuSuperEF_WideCone(name="TrigMuSuperEF_WideCone",**kwargs):
    kwargs.setdefault("deltaEtaRoI", 0.3)
    kwargs.setdefault("deltaPhiRoI", 0.3)
    kwargs.setdefault("UseL2Info",False)
    kwargs.setdefault("IdTrackParticles", "InDetTrigTrackingxAODCnv_Muon_IDTrig")
#    kwargs.setdefault("ExtrapolatedTrackParticleContName", "MuonEFInfo_ExtrapTrackParticles_FullScan")
#    kwargs.setdefault("MSonlyTrackParticleContName",  "MuonEFInfo_MSonlyTrackParticles_FullScan")
#    kwargs.setdefault("CBTrackParticleContName",  "MuonEFInfo_CombTrackParticles_FullScan")
#    kwargs.setdefault("MuonContName", "MuonEFInfo_FullScan" )
    return TrigMuSuperEFConfig(name,**kwargs)

def TrigMuSuperEF_WideCone05(name="TrigMuSuperEF_WideCone05",**kwargs):
    kwargs.setdefault("deltaEtaRoI", 0.5)
    kwargs.setdefault("deltaPhiRoI", 0.5)
    kwargs.setdefault("IdTrackParticles", "InDetTrigTrackingxAODCnv_Muon_IDTrig")
    kwargs.setdefault("doInsideOut", False)
    kwargs.setdefault("doOutsideIn", True)
    kwargs.setdefault("StandaloneOnly", True)
    kwargs.setdefault("UseL2Info",False)
#    kwargs.setdefault("ExtrapolatedTrackParticleContName", "MuonEFInfo_ExtrapTrackParticles_FullScan")
#    kwargs.setdefault("MSonlyTrackParticleContName",  "MuonEFInfo_MSonlyTrackParticles_FullScan")
#    kwargs.setdefault("CBTrackParticleContName",  "MuonEFInfo_CombTrackParticles_FullScan")
#    kwargs.setdefault("MuonContName", "MuonEFInfo_FullScan" )
    return TrigMuSuperEFConfig(name,**kwargs)
#
# Full scan configs
#
def TrigMuSuperEF_FSCB(name="TrigMuSuperEF_FSCB",**kwargs):
    kwargs.setdefault("doInsideOut",False)
    kwargs.setdefault("doOutsideIn",True)
    kwargs.setdefault("fullScan",True)    
    kwargs.setdefault("UseL2Info",False)
#    kwargs.setdefault("ExtrapolatedTrackParticleContName", "MuonEFInfo_ExtrapTrackParticles_FullScan")
#    kwargs.setdefault("MSonlyTrackParticleContName",  "MuonEFInfo_MSonlyTrackParticles_FullScan")
#    kwargs.setdefault("CBTrackParticleContName",  "MuonEFInfo_CombTrackParticles_FullScan")
#    kwargs.setdefault("MuonContName", "MuonEFInfo_FullScan" )
    return TrigMuSuperEFConfig(name,**kwargs)


def TrigMuSuperEF_FSSA(name="TrigMuSuperEF_FSSA",**kwargs):
    kwargs.setdefault("StandaloneOnly",True)
    kwargs.setdefault("UseL2Info",False)
#    kwargs.setdefault("ExtrapolatedTrackParticleContName", "MuonEFInfo_ExtrapTrackParticles_FullScan")
#    kwargs.setdefault("MSonlyTrackParticleContName",  "MuonEFInfo_MSonlyTrackParticles_FullScan")
#    kwargs.setdefault("CBTrackParticleContName",  "MuonEFInfo_CombTrackParticles_FullScan")
#    kwargs.setdefault("MuonContName", "MuonEFInfo_FullScan" )
    return TrigMuSuperEF_FSCB(name,**kwargs)

def TrigMuSuperEF_CTonly(name="TrigMuSuperEF_CTonly", **kwargs):
    kwargs.setdefault("CaloTagOnly", True)
    kwargs.setdefault("IdTrackParticles", "")
#    kwargs.setdefault("TrackIsolationTool", "TrigMuSuperEF_TrackIsolationTool");
    return TrigMuSuperEF_FSCB(name, **kwargs)


class TrigMuSuperEFHypoConfig(TrigMuonEFCombinerHypoConfig):
    __slots__ = ()

    # overwrite the __new__ of TrigMuonEFCombinerHypeConfig so we can set our own instance name
    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (args[0],'Muon',args[1])] + list(args)
        # Note: skip the __new__ of TrigMuonEFCombinerHypeConfig, so go directly to its superclass !!!
        return super(TrigMuonEFCombinerHypoConfig, cls ).__new__( cls, *newargs, **kwargs )



def TrigMuSuperEF_MuonCaloTagTool( name='OnlineMuonCaloTagTool', **kwargs ):
    kwargs.setdefault("CaloMuonTagLoose",       getPublicTool("CaloMuonTagLoose") )
    kwargs.setdefault("CaloMuonTagTight",       getPublicTool("CaloMuonTag") )
    kwargs.setdefault("CaloMuonLikelihoodTool", getPublicTool("CaloMuonLikelihoodTool") )
    kwargs.setdefault("TrackDepositInCaloTool", getPublicTool("TrigMuSuperEF_TrackDepositInCaloTool") )
    kwargs.setdefault("TrackSelectorTool",      getPublicTool("CaloTrkMuIdAlgTrackSelectorTool") )
    #kwargs.setdefault("TrackIsolationTool",     getPublicTool("TrigMuSuperEF_TrackIsolationTool") )
    kwargs.setdefault("TrackIsolationTool",     None)
    return CfgMgr.MuonCombined__MuonCaloTagTool(name,**kwargs )

def TrigMuSuperEF_TrackIsolationTool( name = "TrigMuSuperEF_TrackIsolationTool", **kwargs):
    return TrigMuonEFTrackIsolationTool(name, deltaZCut = 6.0*mm, removeSelf=True, useAnnulus=False) 

def TrigMuSuperEF_TrackDepositInCaloTool(name = "TrigMuSuperEF_TrackDepositInCaloTool", **kwargs):
    #kwargs.setdefault("CaloCellContainerName", "")
    #kwargs.setdefault("CaloClusterContainerName", "TrigCaloClusterContainer")
    return CfgMgr.TrackDepositInCaloTool(name, **kwargs)

