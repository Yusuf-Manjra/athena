# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
#from AthenaConfiguration.MainServicesConfig import MainServicesSerial


def BunchCrossingCondAlgCfg(configFlags):
    from LumiBlockComps.LumiBlockCompsConf import BunchCrossingCondAlg
    from IOVDbSvc.IOVDbSvcConfig import addFolders

    result=ComponentAccumulator()

    run1=(configFlags.IOVDb.DatabaseInstance=='COMP200')

    folder = '/TDAQ/OLC/LHC/FILLPARAMS'
    result.merge(addFolders(configFlags,folder,'TDAQ',className = 'AthenaAttributeList'))


    alg = BunchCrossingCondAlg(Run1=run1,
                               FillParamsFolderKey =folder )

    result.addCondAlgo(alg)

    return result
    



if __name__=="__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags


    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg

    ConfigFlags.IOVDb.DatabaseInstance="CONDBR2"
    ConfigFlags.Input.isMC=False
    ConfigFlags.IOVDb.GlobalTag="CONDBR2-BLKPA-2017-05"
    ConfigFlags.lock()


    

    result=MainServicesSerialCfg()


    from McEventSelector.McEventSelectorConf import McEventSelector,McCnvSvc
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc

    #event & time-stamp from the q431 test input 
    mcevtsel=McEventSelector(RunNumber=330470,
                             EventsPerRun=1,
                             FirstEvent=1183722158,
                             FirstLB=310,
                             EventsPerLB=1,
                             InitialTimeStamp=1500867637,
                             TimeStampInterval=1
                         )

    result.addService(mcevtsel)
    result.setAppProperty("EvtSel",mcevtsel.getFullJobOptName())

    mccnvsvc=McCnvSvc()
    result.addService(mccnvsvc)


    result.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[mccnvsvc.getFullJobOptName(),]))
    
    result.merge(BunchCrossingCondAlgCfg(ConfigFlags))
    
    
    from LumiBlockComps.LumiBlockCompsConf import BunchCrossingCondTest
    result.addEventAlgo(BunchCrossingCondTest(FileName="BCData1.txt"))

    result.run(1)
                       
    
    #f=open("test.pkl","w")
    #result.store(f)
    #f.close()
