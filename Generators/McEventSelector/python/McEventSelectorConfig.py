# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/AthenaJobConfigRun3

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def McEventSelectorCfg(configFlags):
    cfg=ComponentAccumulator()

    McCnvSvc=CompFactory.McCnvSvc
    mcCnvSvc = McCnvSvc()
    cfg.addService(mcCnvSvc)
    EvtPersistencySvc=CompFactory.EvtPersistencySvc
    cfg.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[mcCnvSvc.getFullJobOptName(),]))

    McEventSelector=CompFactory.McEventSelector
    evSel=McEventSelector("EventSelector")
    rn = configFlags.Input.RunNumber
    if isinstance(rn, type([])):
        rn = rn[0]
    evSel.RunNumber = rn
    evSel.InitialTimeStamp = configFlags.Input.InitialTimeStamp
    cfg.addService(evSel)
    cfg.setAppProperty("EvtSel",evSel.getFullJobOptName())

    return cfg
