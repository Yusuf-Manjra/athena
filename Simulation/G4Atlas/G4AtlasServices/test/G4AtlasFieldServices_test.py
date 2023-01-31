#!/usr/bin/env python
"""Run tests on G4AtlasFieldServices

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

if __name__ == '__main__':
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg

  # Set up logging
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  log.setLevel(DEBUG)

  #import config flags
  from AthenaConfiguration.AllConfigFlags import initConfigFlags
  from AthenaConfiguration.Enums import Project
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  flags = initConfigFlags()
  flags.Input.Files = defaultTestFiles.EVNT

  # Finalize
  flags.lock()

  ## Initialize a new component accumulator
  cfg = MainServicesCfg(flags)

  from G4AtlasServices.G4AtlasFieldServices import StandardFieldSvcCfg
  from G4AtlasServices.G4AtlasFieldServices import ForwardFieldSvcCfg
  from G4AtlasServices.G4AtlasFieldServices import Q1FwdG4FieldSvcCfg

  #add the algorithm
  acc1 = StandardFieldSvcCfg(flags)
  acc2 = ForwardFieldSvcCfg(flags)

  # don't run for simulation only tests (todo - make new general test)
  if flags.Common.Project is not Project.AthSimulation:
    acc3 = Q1FwdG4FieldSvcCfg(flags)
    cfg.merge(acc3)

  cfg.merge(acc1)
  cfg.merge(acc2)

  # Dump config
  #cfg.getService("StoreGateSvc").Dump = True
  #cfg.getService("ConditionStore").Dump = True
  cfg.printConfig(withDetails=True, summariseProps = True)
  flags.dump()

  with open("test.pkl", "wb") as f:
    cfg.store(f)

  print (cfg._publicTools)
  print ("-----------------finished----------------------")
