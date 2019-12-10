#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def regSelCfg( flags ):
    """ Configures Region Selector Svc according to the detector flags """
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from RegionSelector.RegionSelectorConf import RegSelSvc
    from AthenaCommon.SystemOfUnits import mm
    acc = ComponentAccumulator()

    regSel = RegSelSvc()
    regSel.DeltaZ = 225 * mm

    if flags.Detector.GeometryLAr:
        regSel.enableCalo = True

        from LArGeoAlgsNV.LArGMConfig import LArGMCfg
        acc.merge( LArGMCfg(  flags ) )

        from LArRegionSelector.LArRegionSelectorConf import LArRegionSelectorTable
        regSel.LArRegionSelectorTable = LArRegionSelectorTable(name="LArRegionSelectorTable")

        from IOVDbSvc.IOVDbSvcConfig import addFolders
        acc.merge( addFolders(flags, ['/LAR/Identifier/FebRodMap'], 'LAR' ))

    if flags.Detector.GeometryTile:
        regSel.enableCalo = True

        from TileGeoModel.TileGMConfig import TileGMCfg
        acc.merge( TileGMCfg( flags ) )
        acc.getService('GeoModelSvc').DetectorTools['TileDetectorTool'].GeometryConfig = 'RECO'

        from TileRawUtils.TileRawUtilsConf import TileRegionSelectorTable
        regSel.TileRegionSelectorTable = TileRegionSelectorTable(name="TileRegionSelectorTable")

    if flags.Detector.GeometryPixel:
        regSel.enableID = True
        regSel.enablePixel = True        
        from InDetRegionSelector.InDetRegionSelectorConf import SiRegionSelectorTable
        regSel.PixelRegionLUT_CreatorTool = SiRegionSelectorTable(name        = "PixelRegionSelectorTable",
                                                                  ManagerName = "Pixel",
                                                                  OutputFile  = "RoITablePixel.txt",
                                                                  PrintHashId = True,
                                                                  PrintTable  = False)

    if flags.Detector.GeometrySCT:
        regSel.enableID = True
        regSel.enableSCT = True
        from InDetRegionSelector.InDetRegionSelectorConf import SiRegionSelectorTable
        regSel.SCT_RegionLUT_CreatorTool = SiRegionSelectorTable(name        = "SCT_RegionSelectorTable",
                                                                 ManagerName = "SCT",
                                                                 OutputFile  = "RoITableSCT.txt",
                                                                 PrintHashId = True,
                                                                 PrintTable  = False)

    if flags.Detector.GeometryTRT:
        regSel.enableID = True
        regSel.enableTRT = True
        from InDetRegionSelector.InDetRegionSelectorConf import TRT_RegionSelectorTable
        regSel.TRT_RegionLUT_CreatorTool = TRT_RegionSelectorTable(name = "TRT_RegionSelectorTable",
                                                                   ManagerName = "TRT",
                                                                   OutputFile  = "RoITableTRT.txt",
                                                                   PrintHashId = True,
                                                                   PrintTable  = False)

    # by default, all 'enableX' flags of RegSelSvc are set to True, so turn them off if not needed
    if not flags.Detector.GeometryRPC:
        regSel.enableRPC  = False
    else:
        regSel.enableMuon = True
        from MuonRegionSelector.MuonRegionSelectorConf import RPC_RegionSelectorTable
        regSel.RPCRegionSelectorTable = RPC_RegionSelectorTable(name = "RPC_RegionSelectorTable")

    if not flags.Detector.GeometryMDT:
        regSel.enableMDT  = False
    else:
        regSel.enableMuon = True
        from MuonRegionSelector.MuonRegionSelectorConf import MDT_RegionSelectorTable
        regSel.MDTRegionSelectorTable = MDT_RegionSelectorTable(name = "MDT_RegionSelectorTable")

    if not flags.Detector.GeometryTGC:
        regSel.enableTGC  = False
    else:
        regSel.enableMuon = True
        from MuonRegionSelector.MuonRegionSelectorConf import TGC_RegionSelectorTable
        regSel.TGCRegionSelectorTable = TGC_RegionSelectorTable(name = "TGC_RegionSelectorTable")

    if not flags.Detector.GeometryCSC:
        regSel.enableCSC  = False
    else:
        regSel.enableMuon = True
        from MuonRegionSelector.MuonRegionSelectorConf import CSC_RegionSelectorTable
        regSel.CSCRegionSelectorTable = CSC_RegionSelectorTable(name = "CSC_RegionSelectorTable")

    if not flags.Detector.GeometryMM:
        regSel.enableMM = False

    if not flags.Detector.GeometrysTGC:
        regSel.enablesTGC = False

    acc.addService( regSel )

    return acc

if __name__ == "__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Constants import DEBUG

    ConfigFlags.Detector.GeometryPixel = True
    ConfigFlags.Detector.GeometrySCT   = True
    ConfigFlags.Detector.GeometryTRT   = True
    ConfigFlags.Detector.GeometryLAr   = True
    ConfigFlags.Detector.GeometryTile  = True
    ConfigFlags.Detector.GeometryMDT   = True
    ConfigFlags.Detector.GeometryTGC   = True
    ConfigFlags.Detector.GeometryRPC   = True

    ConfigFlags.Input.Files = defaultTestFiles.RAW    
    ConfigFlags.Input.isMC = False
    ConfigFlags.dump()
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    cfg=MainServicesSerialCfg() 

    ## move up

    # when trying AOD
    #    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    #    cfg.merge( PoolReadCfg( ConfigFlags ) )

    
    from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
    cfg.merge(TrigBSReadCfg( ConfigFlags ))
    
    
    acc = regSelCfg( ConfigFlags )
    cfg.merge( acc )


    from RegSelSvcTest.RegSelSvcTestConf import RegSelTestAlg
    testerAlg = RegSelTestAlg()
    testerAlg.Mt=True    
    testerAlg.OutputLevel=DEBUG
    cfg.addEventAlgo( testerAlg )
    from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
    apcs=AthenaPoolCnvSvc()
    cfg.addService(apcs)
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc
    cfg.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[apcs.getFullJobOptName(),]))

    cfg.getService("IOVDbSvc").OutputLevel=DEBUG
    
    cfg.store( open( "test.pkl", "wb" ) )
    print("used flags")
    ConfigFlags.dump()
    cfg.run(0)
    print("All OK")
