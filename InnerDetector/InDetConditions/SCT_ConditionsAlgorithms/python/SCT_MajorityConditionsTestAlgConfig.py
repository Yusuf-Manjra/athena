"""Define method to configure and test SCT_MajorityConditionsTestAlg

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def SCT_MajorityConditionsTestAlgCfg(flags, name="SCT_MajorityConditionsTestAlg", **kwargs):
    """Return a configured SCT_MajorityConditionsTestAlg"""
    acc = ComponentAccumulator()
    from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_MajorityConditionsCfg
    kwargs.setdefault("MajorityTool", acc.popToolsAndMerge(SCT_MajorityConditionsCfg(flags)))
    acc.addEventAlgo(CompFactory.SCT_MajorityConditionsTestAlg(name, **kwargs))
    return acc

if __name__=="__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    from AthenaConfiguration.AllConfigFlags import initConfigFlags
    flags = initConfigFlags()
    flags.Input.Files = []
    flags.Input.isMC = False
    flags.Input.ProjectName = "data16_13TeV"
    flags.Input.RunNumber = 310809
    flags.Input.TimeStamp = 1476741326 # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)
    flags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2017-06"
    flags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"
    flags.Detector.GeometrySCT = True
    flags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(flags)

    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge(McEventSelectorCfg(flags))

    cfg.merge(SCT_MajorityConditionsTestAlgCfg(flags))

    cfg.run(maxEvents=20)
