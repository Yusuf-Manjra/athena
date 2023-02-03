#!/usr/bin/env python
"""Run tests on SCT_GeoModel configuration

Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import initConfigFlags
    from AthenaConfiguration.Enums import Project
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    flags = initConfigFlags()
    flags.Input.Files = defaultTestFiles.HITS_RUN2
    flags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
    flags.GeoModel.Align.Dynamic = False
    flags.lock()

    if flags.Common.Project is Project.AthSimulation:
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_SimulationGeometryCfg
        acc = SCT_SimulationGeometryCfg(flags)
        f=open('SCT_SimulationGeometryCfg.pkl','wb')
    else:
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
        acc = SCT_ReadoutGeometryCfg(flags)
        f=open('SCT_ReadoutGeometryCfg.pkl','wb')
    acc.store(f)
    f.close()
