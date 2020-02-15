#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def MuonDQAMonitoringConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    result = ComponentAccumulator()

    if flags.DQ.Steering.Muon.doRawMon:
        # do not run in RAW->ESD, or AOD-only
        if flags.DQ.Environment not in ('tier0Raw', 'AOD'):
            from MdtRawDataMonitoring.MDTMonitorAlgorithm import MdtMonitoringConfig
            from RpcRawDataMonitoring.RpcMonitorAlgorithm import RpcMonitoringConfig
            result.merge(MdtMonitoringConfig(flags))
            result.merge(RpcMonitoringConfig(flags))

        from TgcRawDataMonitoring.TgcRawDataMonitorAlgorithm import TgcRawDataMonitoringConfig
    
        result.merge(TgcRawDataMonitoringConfig(flags))
        
    return result
