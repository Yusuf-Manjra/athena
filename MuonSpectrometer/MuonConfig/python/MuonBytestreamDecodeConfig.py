#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Constants import DEBUG

## Small class to hold the names for cache containers, should help to avoid copy / paste errors
class MuonCacheNames(object):
    MdtCsmCache = "MdtCsmRdoCache"
    CscCache    = "CscRdoCache"
    RpcCache    = "RpcRdoCache"
    TgcCache    = "TgcRdoCache"

## This configuration function creates the IdentifiableCaches for RDO
#
# The function returns a ComponentAccumulator which should be loaded first
# If a configuration wants to use the cache, they need to use the same names as defined here
def MuonCacheCfg():
    acc = ComponentAccumulator()

    from MuonByteStream.MuonByteStreamConf import MuonCacheCreator
    cacheCreator = MuonCacheCreator(MdtCsmCacheKey = MuonCacheNames.MdtCsmCache,
                                    CscCacheKey    = MuonCacheNames.CscCache,
                                    RpcCacheKey    = MuonCacheNames.RpcCache,
                                    TgcCacheKey    = MuonCacheNames.TgcCache)
    acc.addEventAlgo( cacheCreator, primary=True )
    return acc


## This configuration function sets up everything for decoding RPC bytestream data into RDOs
#
# The forTrigger paramater is used to put the algorithm in RoI mode
# The function returns a ComponentAccumulator and the data-decoding algorithm, which should be added to the right sequence by the user
def RpcBytestreamDecodeCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()
    
    # We need the RPC cabling to be setup
    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
    acc.merge( RPCCablingConfigCfg(flags) )

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags)) 

    # Setup the RPC ROD decoder
    from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcROD_Decoder
    RPCRodDecoder = Muon__RpcROD_Decoder(name	     = "RpcROD_Decoder" )

    # RAW data provider tool needs ROB data provider service (should be another Config function?)
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
    robDPSvc = ROBDataProviderSvc()
    acc.addService( robDPSvc )

    # Setup the RAW data provider tool
    from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RPC_RawDataProviderToolMT
    MuonRpcRawDataProviderTool = Muon__RPC_RawDataProviderToolMT(name    = "RPC_RawDataProviderToolMT",
                                                               Decoder = RPCRodDecoder )
    if forTrigger:
        MuonRpcRawDataProviderTool.RpcContainerCacheKey   = MuonCacheNames.RpcCache
        MuonRpcRawDataProviderTool.WriteOutRpcSectorLogic = False

    acc.addPublicTool( MuonRpcRawDataProviderTool ) # This should be removed, but now defined as PublicTool at MuFastSteering 
    
    # Setup the RAW data provider algorithm
    from MuonByteStream.MuonByteStreamConf import Muon__RpcRawDataProvider
    RpcRawDataProvider = Muon__RpcRawDataProvider(name         = "RpcRawDataProvider",
                                                  ProviderTool = MuonRpcRawDataProviderTool )

    if forTrigger:
        # Configure the RAW data provider for ROI access
        RpcRawDataProvider.RoIs = "MURoIs" # Maybe we don't want to hard code this?

    acc.addEventAlgo(RpcRawDataProvider, primary=True)
    return acc

def TgcBytestreamDecodeCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()

    # We need the TGC cabling to be setup
    from MuonConfig.MuonCablingConfig import TGCCablingConfigCfg
    acc.merge( TGCCablingConfigCfg(flags) )

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags)) 

    # Setup the TGC ROD decoder
    from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RodDecoderReadout
    TGCRodDecoder = Muon__TGC_RodDecoderReadout(name = "TgcROD_Decoder")

    # RAW data provider tool needs ROB data provider service (should be another Config function?)
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
    robDPSvc = ROBDataProviderSvc()
    acc.addService( robDPSvc )

    # Setup the RAW data provider tool
    from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TGC_RawDataProviderToolMT
    MuonTgcRawDataProviderTool = Muon__TGC_RawDataProviderToolMT(name    = "TGC_RawDataProviderToolMT",
                                                               Decoder = TGCRodDecoder )

    if forTrigger:
        MuonTgcRawDataProviderTool.TgcContainerCacheKey   = MuonCacheNames.TgcCache

    acc.addPublicTool( MuonTgcRawDataProviderTool ) # This should be removed, but now defined as PublicTool at MuFastSteering 
    
    # Setup the RAW data provider algorithm
    from MuonByteStream.MuonByteStreamConf import Muon__TgcRawDataProvider
    TgcRawDataProvider = Muon__TgcRawDataProvider(name         = "TgcRawDataProvider",
                                                  ProviderTool = MuonTgcRawDataProviderTool )
    
    acc.addEventAlgo(TgcRawDataProvider,primary=True)

    return acc

def MdtBytestreamDecodeCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()

    # We need the MDT cabling to be setup
    from MuonConfig.MuonCablingConfig import MDTCablingConfigCfg
    acc.merge( MDTCablingConfigCfg(flags) )

    from MuonConfig.MuonCalibConfig import MdtCalibrationSvcCfg
    acc.merge( MdtCalibrationSvcCfg(flags)  )

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags)) 

    # Setup the MDT ROD decoder
    from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import MdtROD_Decoder
    MDTRodDecoder = MdtROD_Decoder(name	     = "MdtROD_Decoder")

    # RAW data provider tool needs ROB data provider service (should be another Config function?)
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
    robDPSvc = ROBDataProviderSvc()
    acc.addService( robDPSvc )

    # Setup the RAW data provider tool
    from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MDT_RawDataProviderToolMT
    MuonMdtRawDataProviderTool = Muon__MDT_RawDataProviderToolMT(name    = "MDT_RawDataProviderToolMT",
                                                               Decoder = MDTRodDecoder)

    if forTrigger:
        MuonMdtRawDataProviderTool.CsmContainerCacheKey = MuonCacheNames.MdtCsmCache

    acc.addPublicTool( MuonMdtRawDataProviderTool ) # This should be removed, but now defined as PublicTool at MuFastSteering 

    # Setup the RAW data provider algorithm
    from MuonByteStream.MuonByteStreamConf import Muon__MdtRawDataProvider
    MdtRawDataProvider = Muon__MdtRawDataProvider(name         = "MdtRawDataProvider",
                                                  ProviderTool = MuonMdtRawDataProviderTool )

    acc.addEventAlgo(MdtRawDataProvider,primary=True)

    return acc

def CscBytestreamDecodeCfg(flags, forTrigger=False):
    acc = ComponentAccumulator()

    # We need the CSC cabling to be setup
    from MuonConfig.MuonCablingConfig import CSCCablingConfigCfg # Not yet been prepared
    acc.merge( CSCCablingConfigCfg(flags) )

    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    acc.merge(MuonGeoModelCfg(flags)) 

    # Setup the CSC ROD decoder
    from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscROD_Decoder
    CSCRodDecoder = Muon__CscROD_Decoder(name	     = "CscROD_Decoder" )

    # RAW data provider tool needs ROB data provider service (should be another Config function?)
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
    robDPSvc = ROBDataProviderSvc()
    acc.addService( robDPSvc )

    # Setup the RAW data provider tool
    from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CSC_RawDataProviderToolMT
    MuonCscRawDataProviderTool = Muon__CSC_RawDataProviderToolMT(name    = "CSC_RawDataProviderToolMT",
                                                               Decoder = CSCRodDecoder)
    if forTrigger:
        MuonCscRawDataProviderTool.CscContainerCacheKey = MuonCacheNames.CscCache

    acc.addPublicTool( MuonCscRawDataProviderTool ) # This should be removed, but now defined as PublicTool at MuFastSteering 
    
    # Setup the RAW data provider algorithm
    from MuonByteStream.MuonByteStreamConf import Muon__CscRawDataProvider
    CscRawDataProvider = Muon__CscRawDataProvider(name         = "CscRawDataProvider",
                                                  ProviderTool = MuonCscRawDataProviderTool )

    acc.addEventAlgo(CscRawDataProvider,primary=True)

    return acc

if __name__=="__main__":
    # To run this, do e.g. 
    # python ../athena/MuonSpectrometer/MuonConfig/python/MuonBytestreamDecode.py

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    #from AthenaConfiguration.TestDefaults import defaultTestFiles
    #ConfigFlags.Input.Files = defaultTestFiles.RAW
    # Set global tag by hand for now
    ConfigFlags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2018-13"#"CONDBR2-BLKPA-2015-17"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"#"ATLAS-R2-2015-03-01-00"

    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaCommon.Logging import log 

    log.setLevel(DEBUG)
    log.info('About to setup Rpc Raw data decoding')

    cfg=ComponentAccumulator()
    
    # Seem to need this to read BS properly
    from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
    cfg.merge(TrigBSReadCfg(ConfigFlags ))

    # Schedule Rpc data decoding
    rpcdecodingAcc = RpcBytestreamDecodeCfg( ConfigFlags ) 
    cfg.merge( rpcdecodingAcc )

    # Schedule Tgc data decoding
    tgcdecodingAcc = TgcBytestreamDecodeCfg( ConfigFlags ) 
    cfg.merge( tgcdecodingAcc )

    # Schedule Mdt data decoding

    mdtdecodingAcc  = MdtBytestreamDecodeCfg( ConfigFlags )
    cfg.merge( mdtdecodingAcc )

    # Schedule Csc data decoding
    cscdecodingAcc = CscBytestreamDecodeCfg( ConfigFlags ) 
    cfg.merge( cscdecodingAcc )

    # Need to add POOL converter  - may be a better way of doing this?
    from AthenaCommon import CfgMgr
    cfg.addService( CfgMgr.AthenaPoolCnvSvc() )
    cfg.getService("EventPersistencySvc").CnvServices += [ "AthenaPoolCnvSvc" ]

    log.info('Print Config')
    cfg.printConfig(withDetails=True)

    # Store config as pickle
    log.info('Save Config')
    with open('MuonBytestreamDecode.pkl','w') as f:
        cfg.store(f)
        f.close()

