from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from LArROD.LArRODConf import LArRawChannelBuilderAlg
from LArRecUtils.LArADC2MeVCondAlgConfig import LArADC2MeVCondAlgCfg
from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg

def LArRawChannelBuilderAlgCfg(configFlags):
    acc=LArADC2MeVCondAlgCfg(configFlags)
    acc.merge(LArElecCalibDbCfg(configFlags,("OFC","Shape","Pedestal")))


    if configFlags.Input.isMC:
        #defaults are fine .. 
        acc.addEventAlgo(LArRawChannelBuilderAlg())
    else:
        acc.addEventAlgo(LArRawChannelBuilderAlg(LArRawChannelKey="LArRawChannels_fromDigits"))
        from  ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        acc.merge(ByteStreamReadCfg(configFlags,typeNames=["LArDigitContainer/FREE",]))
    return acc


if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    log.setLevel(DEBUG)

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc=LArRawChannelBuilderAlgCfg(ConfigFlags)
    
    from LArEventTest.LArEventTestConf import DumpLArRawChannels
    acc.addEventAlgo(DumpLArRawChannels(LArRawChannelContainerName="LArRawChannels_fromDigits",))

    f=open("LArRawChannelBuilderAlg.pkl","w")
    acc.store(f)
    f.close()
