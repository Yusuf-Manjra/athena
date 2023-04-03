# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

def TIDV_cond_fix(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from IOVDbSvc.IOVDbSvcConfig import addOverride
    from AthenaCommon.Logging import logging
    log = logging.getLogger('TIDV_cond_fix')
    log.info('Overriding conditions for old MC inputs')

    cfg = ComponentAccumulator()
    cfg.merge(addOverride(flags, '/PIXEL/PixelModuleFeMask', 'PixelModuleFeMask-SIM-MC16-000-03'))
    cfg.merge(addOverride(flags, '/TRT/Calib/PID_NN', 'TRTCalibPID_NN_v1'))
    cfg.merge(addOverride(flags, '/PIXEL/PixelClustering/PixelNNCalibJSON', 'PixelNNCalibJSON-SIM-RUN2-000-02'))
    return cfg
