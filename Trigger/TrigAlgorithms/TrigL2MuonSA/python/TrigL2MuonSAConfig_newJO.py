#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
#  This file configs the L2MuonSA reco alg in the newJO way, 
#      but now is located here temporarily until newJO migrations are done in all trigger signatures.
#  This should be moved at somewhere in offline.

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

### Output Name ###
muFastInfo = "MuonL2SAInfo"

# Get Rpc data decoder for MuFast data preparator 
def RpcDataPreparatorCfg( flags, roisKey ):

    acc = ComponentAccumulator()

    # Get BS decoder 
    from MuonConfig.MuonBytestreamDecodeConfig import RpcBytestreamDecodeCfg
    rpcAcc = RpcBytestreamDecodeCfg( flags, forTrigger=True )
    #rpcAcc.getEventAlgo("RpcRawDataProvider").RoIs = roisKey
    acc.merge( rpcAcc )

    # Get BS->RDO convertor
    from MuonConfig.MuonRdoDecodeConfig import RpcRDODecodeCfg    
    rpcAcc = RpcRDODecodeCfg( flags, forTrigger=True )
    #rpcAcc.getEventAlgo("RpcRdoToRpcPrepData").RoIs = roisKey
    acc.merge( rpcAcc )

    # Set Rpc data preparator for MuFast data preparator
    TrigL2MuonSA__RpcDataPreparator=CompFactory.TrigL2MuonSA__RpcDataPreparator
    RpcDataPreparator = TrigL2MuonSA__RpcDataPreparator( RpcPrepDataProvider  = "",
                                                         RpcRawDataProvider   = "",
                                                         DecodeBS = False,
                                                         DoDecoding = False )
    acc.addPublicTool( RpcDataPreparator, primary=True ) # Now this is needed, but should be removed
 
    return acc, RpcDataPreparator

# Get Tgc data decoder for MuFast data preparator 
def TgcDataPreparatorCfg( flags, roisKey ):

    acc = ComponentAccumulator()

    # Get BS decoder 
    from MuonConfig.MuonBytestreamDecodeConfig import TgcBytestreamDecodeCfg
    tgcAcc = TgcBytestreamDecodeCfg( flags, forTrigger=True )
    #TgcRawDataProvider.RoIs = roisKey
    acc.merge( tgcAcc )

    # Get BS->RDO convertor
    from MuonConfig.MuonRdoDecodeConfig import TgcRDODecodeCfg    
    tgcAcc = TgcRDODecodeCfg( flags, forTrigger=True )
    #tgcAcc.getEventAlgo("TgcRdoToTgcPrepData").RoIs = roisKey
    acc.merge( tgcAcc )

    # Set Tgc data preparator for MuFast data preparator
    TrigL2MuonSA__TgcDataPreparator=CompFactory.TrigL2MuonSA__TgcDataPreparator
    TgcDataPreparator = TrigL2MuonSA__TgcDataPreparator( TgcPrepDataProvider  = "",
                                                         TgcRawDataProvider   = "",
                                                         DecodeBS = False,
                                                         DoDecoding = False )

 
    return acc, TgcDataPreparator

# Get Mdt data decoder for MuFast data preparator 
def MdtDataPreparatorCfg( flags, roisKey ):

    acc = ComponentAccumulator()

    # Get BS decoder 
    from MuonConfig.MuonBytestreamDecodeConfig import MdtBytestreamDecodeCfg
    mdtAcc = MdtBytestreamDecodeCfg( flags, forTrigger=True )
    #MdtRawDataProvider.RoIs = roisKey
    acc.merge( mdtAcc )

    # Get BS->RDO convertor
    from MuonConfig.MuonRdoDecodeConfig import MdtRDODecodeCfg    
    mdtAcc = MdtRDODecodeCfg( flags, forTrigger=True )
    #mdtAcc.getEventAlgo("MdtRdoToMdtPrepData").RoIs = roisKey
    acc.merge( mdtAcc )

    # Set Mdt data preparator for MuFast data preparator
    TrigL2MuonSA__MdtDataPreparator=CompFactory.TrigL2MuonSA__MdtDataPreparator
    MdtDataPreparator = TrigL2MuonSA__MdtDataPreparator( MdtPrepDataProvider  = "",
                                                         MDT_RawDataProvider   = "",
                                                         DecodeBS = False,
                                                         DoDecoding = False )
 
    return acc, MdtDataPreparator

# Get Csc data decoder for MuFast data preparator 
def CscDataPreparatorCfg( flags, roisKey ):

    acc = ComponentAccumulator()

    # Get BS decoder 
    from MuonConfig.MuonBytestreamDecodeConfig import CscBytestreamDecodeCfg
    cscAcc = CscBytestreamDecodeCfg( flags, forTrigger=True )
    #CscRawDataProvider.RoIs = roisKey
    acc.merge( cscAcc )

    # Get BS->RDO convertor
    from MuonConfig.MuonRdoDecodeConfig import CscRDODecodeCfg    
    cscAcc = CscRDODecodeCfg( flags, forTrigger=True )
    #cscAcc.getEventAlgo("CscRdoToCscPrepData").RoIs = roisKey
    acc.merge( cscAcc )

    # Get cluster builder
    from MuonConfig.MuonRdoDecodeConfig import CscClusterBuildCfg
    cscAcc = CscClusterBuildCfg( flags, forTrigger=True )
    acc.merge( cscAcc )

    # Set Csc data preparator for MuFast data preparator
    TrigL2MuonSA__CscDataPreparator=CompFactory.TrigL2MuonSA__CscDataPreparator
    CscDataPreparator = TrigL2MuonSA__CscDataPreparator( CscPrepDataProvider  = "",
                                                         CscClusterProvider   = "",
                                                         CscRawDataProvider   = "",
                                                         DecodeBS = False,
                                                         DoDecoding = False )

    acc.addPublicTool( CscDataPreparator, primary=True ) # This should be removed
 
    return acc, CscDataPreparator

# Based on TrigL2MuonSAMTConfig at TrigL2MuonSA/TrigL2MuonSAConfig.py
def muFastSteeringCfg( flags, roisKey, setup="" ):

    acc = ComponentAccumulator()

    # Get RPC decoder
    rpcAcc, RpcDataPreparator = RpcDataPreparatorCfg( flags, roisKey )
    acc.merge( rpcAcc )

    # Get TGC decoder
    tgcAcc, TgcDataPreparator = TgcDataPreparatorCfg( flags, roisKey )
    acc.merge( tgcAcc )

    # Get MDT decoder
    mdtAcc, MdtDataPreparator = MdtDataPreparatorCfg( flags, roisKey )
    acc.merge( mdtAcc )

    # Get CSC decoder
    cscAcc, CscDataPreparator = CscDataPreparatorCfg( flags, roisKey )
    acc.merge( cscAcc )

    # Set MuFast data preparator
    TrigL2MuonSA__MuFastDataPreparator=CompFactory.TrigL2MuonSA__MuFastDataPreparator
    MuFastDataPreparator = TrigL2MuonSA__MuFastDataPreparator( CSCDataPreparator = CscDataPreparator,
                                                               MDTDataPreparator = MdtDataPreparator,
                                                               RPCDataPreparator = RpcDataPreparator,
                                                               TGCDataPreparator = TgcDataPreparator )

    # Setup the station fitter
    TrigL2MuonSA__MuFastStationFitter,TrigL2MuonSA__PtFromAlphaBeta=CompFactory.getComps("TrigL2MuonSA__MuFastStationFitter","TrigL2MuonSA__PtFromAlphaBeta")
    PtFromAlphaBeta = TrigL2MuonSA__PtFromAlphaBeta()
    if flags.Trigger.run2Config == '2016':
        PtFromAlphaBeta.useCscPt = False
        PtFromAlphaBeta.AvoidMisalignedCSCs = True
    else:
        PtFromAlphaBeta.useCscPt = True
        PtFromAlphaBeta.AvoidMisalignedCSCs = True

    MuFastStationFitter = TrigL2MuonSA__MuFastStationFitter( PtFromAlphaBeta = PtFromAlphaBeta )

    TrigL2MuonSA__MuFastPatternFinder,TrigL2MuonSA__MuFastTrackFitter,TrigL2MuonSA__MuFastTrackExtrapolator,TrigL2MuonSA__MuCalStreamerTool,TrigL2MuonSA__CscSegmentMaker=CompFactory.getComps("TrigL2MuonSA__MuFastPatternFinder","TrigL2MuonSA__MuFastTrackFitter","TrigL2MuonSA__MuFastTrackExtrapolator","TrigL2MuonSA__MuCalStreamerTool","TrigL2MuonSA__CscSegmentMaker")
    MuFastPatternFinder     = TrigL2MuonSA__MuFastPatternFinder()
    MuFastTrackFitter       = TrigL2MuonSA__MuFastTrackFitter()
    MuFastTrackExtrapolator = TrigL2MuonSA__MuFastTrackExtrapolator()
    MuCalStreamerTool       = TrigL2MuonSA__MuCalStreamerTool()
    CscSegmentMaker         = TrigL2MuonSA__CscSegmentMaker()

    # Set Reco alg of muFast step
    from TrigL2MuonSA.TrigL2MuonSAMonitoring import TrigL2MuonSAMonitoring
    MuFastSteering=CompFactory.MuFastSteering
    muFastAlg = MuFastSteering( name                   = "MuFastSteering_Muon"+setup,
                                DataPreparator         = MuFastDataPreparator,
                                StationFitter          = MuFastStationFitter,
                                PatternFinder          = MuFastPatternFinder,
                                TrackFitter            = MuFastTrackFitter,
                                TrackExtrapolator      = MuFastTrackExtrapolator,
                                CalibrationStreamer    = MuCalStreamerTool, 
                                CscSegmentMaker        = CscSegmentMaker,
                                R_WIDTH_TGC_FAILED     = 200,
                                R_WIDTH_RPC_FAILED     = 400,
                                DoCalibrationStream    = False,
                                USE_ROIBASEDACCESS_CSC = True,
                                RpcErrToDebugStream    = True,
                                Timing                 = False,
                                MonTool                = TrigL2MuonSAMonitoring() )

    # Default backextrapolator is for MC Misaligned Detector
    # Based on MuonBackExtrapolatorForMisalignedDet at TrigMuonBackExtrapolator/TrigMuonBackExtrapolatorConfig.py
    TrigMuonBackExtrapolator=CompFactory.TrigMuonBackExtrapolator
    muFastAlg.BackExtrapolator = TrigMuonBackExtrapolator( name        = "MisalignedBackExtrapolator",
                                                           Aligned     = False,
                                                           DataSet     = False )

    if flags.Trigger.run2Config == '2016':
        muFastAlg.UseEndcapInnerFromBarrel = False
    else: 
        muFastAlg.UseEndcapInnerFromBarrel = True

    if setup == '900GeV':
        muFastAlg.WinPt = 4.0
        muFastAlg.Scale_Road_BarrelInner  = 3
        muFastAlg.Scale_Road_BarrelMiddle = 3
        muFastAlg.Scale_Road_BarrelOuter  = 3
    else:
        muFastAlg.WinPt = 6.0
        muFastAlg.Scale_Road_BarrelInner  = 1
        muFastAlg.Scale_Road_BarrelMiddle = 1
        muFastAlg.Scale_Road_BarrelOuter  = 1

    if setup == 'MuonCalib':
        muFastAlg.DoCalibrationStream = True
        muFastAlg.MuonCalDataScouting = False
        muFastAlg.MuonCalBufferSize   = 1024*1024

    if setup == 'MuonCalibDataScouting':
        muFastAlg.DoCalibrationStream = True
        muFastAlg.MuonCalDataScouting = True
        muFastAlg.MuonCalBufferSize   = 1024*1024

    return acc, muFastAlg

def PtBarrelLUTSvcCfg( flags ):

    acc = ComponentAccumulator()
    from TrigL2MuonSA.TrigL2MuonSAConfig import PtBarrelLUTSvc
    ptBarrelLUTSvc = PtBarrelLUTSvc()
    acc.addService( ptBarrelLUTSvc )

    return acc, ptBarrelLUTSvc

def PtBarrelLUTSvcCfg_MC( flags ):

    acc = ComponentAccumulator()
    from TrigL2MuonSA.TrigL2MuonSAConfig import PtBarrelLUTSvc_MC
    ptBarrelLUTSvc_MC = PtBarrelLUTSvc_MC() 
    acc.addService( ptBarrelLUTSvc_MC )

    return acc, ptBarrelLUTSvc_MC

def PtEndcapLUTSvcCfg( flags ):

    acc = ComponentAccumulator()
    from TrigL2MuonSA.TrigL2MuonSAConfig import PtEndcapLUTSvc
    ptEndcapLUTSvc = PtEndcapLUTSvc()
    acc.addService( ptEndcapLUTSvc )

    return acc, ptEndcapLUTSvc

def PtEndcapLUTSvcCfg_MC( flags ):

    acc = ComponentAccumulator()
    from TrigL2MuonSA.TrigL2MuonSAConfig import PtEndcapLUTSvc_MC
    ptEndcapLUTSvc_MC = PtEndcapLUTSvc_MC() 
    acc.addService( ptEndcapLUTSvc_MC )

    return acc, ptEndcapLUTSvc_MC


def AlignmentBarrelLUTSvcCfg( flags ):

    acc = ComponentAccumulator()
    from TrigL2MuonSA.TrigL2MuonSAConfig import AlignmentBarrelLUTSvc
    alignmentBarrelLUTSvc = AlignmentBarrelLUTSvc()
    acc.addService( alignmentBarrelLUTSvc )

    return acc, alignmentBarrelLUTSvc

# In the future, above functions should be moved to TrigL2MuonSA package(?)

def l2MuFastAlgCfg( flags, roisKey="" ):

    acc = ComponentAccumulator()

    if not roisKey:
        from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection
        roisKey = mapThresholdToL1RoICollection("MU")

    # Get Reco alg of muFast step
    muFastAcc, muFastFex = muFastSteeringCfg( flags, roisKey )  
    muFastFex.MuRoIs = roisKey
    muFastFex.RecMuonRoI = "HLT_RecMURoIs"
    muFastFex.MuonL2SAInfo = muFastInfo
    muFastFex.forID = "forID"
    muFastFex.forMS = "forMS"
    acc.merge( muFastAcc )

    # Get services of the Reco alg
    acc.merge( PtBarrelLUTSvcCfg(flags)[0] )   
    acc.merge( PtBarrelLUTSvcCfg_MC(flags)[0] )   
    acc.merge( PtEndcapLUTSvcCfg(flags)[0] )   
    acc.merge( PtEndcapLUTSvcCfg_MC(flags)[0] )   
    acc.merge( AlignmentBarrelLUTSvcCfg(flags)[0] )

    return acc, muFastFex


def l2MuFastRecoCfg( flags ):

    # Set EventViews for muFast step
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco
    reco = InViewReco("L2MuFastReco")

    # Get Reco alg of muFast Step in order to set into the view
    algAcc, alg = l2MuFastAlgCfg( flags, roisKey=reco.name+"RoIs")

    reco.addRecoAlg( alg )
    reco.merge( algAcc )

    return reco


def l2MuFastHypoCfg( flags, name="UNSPECIFIED", muFastInfo="UNSPECIFIED" ):

    TrigMufastHypoAlg=CompFactory.TrigMufastHypoAlg
    muFastHypo = TrigMufastHypoAlg( name )
    muFastHypo.MuonL2SAInfoFromMuFastAlg = muFastInfo 

    return muFastHypo
 
