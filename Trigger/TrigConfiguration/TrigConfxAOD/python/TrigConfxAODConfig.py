#
#  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
#

def getxAODConfigSvc(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory

    acc = ComponentAccumulator()

    cfgsvc = CompFactory.TrigConf.xAODConfigSvc('xAODConfigSvc')
    # We serve in-file metadata where possible. If it does not exist (e.g. RAWtoALL), then it is obtained from the Conditions and Detector stores
    cfgsvc.UseInFileMetadata = flags.Trigger.triggerConfig == 'INFILE'
    acc.addService(cfgsvc, primary=True)

    if flags.Trigger.triggerConfig == 'INFILE':
        from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg
        acc.merge(MetaDataSvcCfg(flags))

    return acc
