#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

def getxAODConfigSvc(ConfigFlags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory

    acc = ComponentAccumulator()

    cfgsvc = CompFactory.TrigConf.xAODConfigSvc('xAODConfigSvc')
    # We serve in-file metadata where possible. If it does not exist (e.g. RAWtoALL), then it is obtained from the Conditions and Detector stores
    cfgsvc.UseInFileMetadata = ConfigFlags.Trigger.triggerConfig == 'INFILE'
    acc.addService(cfgsvc, primary=True)

    if ConfigFlags.Trigger.triggerConfig == 'INFILE':
        from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg
        acc.merge(MetaDataSvcCfg(ConfigFlags))

    return acc
