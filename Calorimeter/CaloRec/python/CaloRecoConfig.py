# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def CaloRecoCfg(configFlags, clustersname=None,doLCCalib=None):
    
    result=ComponentAccumulator()
    if not configFlags.Input.isMC:
        #Data-case: Schedule ByteStream reading for LAr & Tile
        from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg
        result.merge(LArRawDataReadingCfg(configFlags))


        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        result.merge( ByteStreamReadCfg(configFlags,type_names=['TileDigitsContainer/TileDigitsCnt','TileRawChannelContainer/TileRawChannelCnt']))


        from LArROD.LArRawChannelBuilderAlgConfig import LArRawChannelBuilderAlgCfg
        result.merge(LArRawChannelBuilderAlgCfg(configFlags))

        
        from TileRecUtils.TileRawChannelMakerConfig import TileRawChannelMakerCfg
        result.merge( TileRawChannelMakerCfg(configFlags) )

        from LArCellRec.LArTimeVetoAlgConfig import LArTimeVetoAlgCfg
        result.merge(LArTimeVetoAlgCfg(configFlags))

              
    #Configure cell-building
    from CaloRec.CaloCellMakerConfig import CaloCellMakerCfg
    result.merge(CaloCellMakerCfg(configFlags))
    
    #Configure topo-cluster builder
    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    result.merge(CaloTopoClusterCfg(configFlags, clustersname=clustersname, doLCCalib=doLCCalib))

    #Configure forward towers:
    from CaloRec.CaloFwdTopoTowerConfig import CaloFwdTopoTowerCfg
    result.merge(CaloFwdTopoTowerCfg(configFlags,CaloTopoClusterContainerKey="CaloTopoClusters"))

    #Configure NoisyROSummary
    from LArCellRec.LArNoisyROSummaryConfig import LArNoisyROSummaryCfg
    result.merge(LArNoisyROSummaryCfg(configFlags))

    from LArROD.LArFebErrorSummaryMakerConfig import LArFebErrorSummaryMakerCfg
    result.merge(LArFebErrorSummaryMakerCfg(configFlags))


    return result


if __name__=="__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG,WARNING
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_13TeV.00330470.physics_Main.daq.RAW._lb0310._SFO-1._0001.data",]

    ConfigFlags.fillFromArgs()

    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    acc = MainServicesCfg(ConfigFlags)

    acc.merge(CaloRecoCfg(ConfigFlags))


    CaloCellDumper=CompFactory.CaloCellDumper
    acc.addEventAlgo(CaloCellDumper())

    ClusterDumper=CompFactory.ClusterDumper
    acc.addEventAlgo(ClusterDumper("TopoDumper",ContainerName="CaloCalTopoClusters",FileName="TopoCluster.txt"))

    f=open("CaloRec.pkl","wb")
    acc.store(f)
    f.close()

    acc.run(10,OutputLevel=WARNING)
