#!/usr/bin/env python
"""Run tests on SCT_G4_SD configuration

Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
"""


if __name__ == '__main__':
  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  log.setLevel(DEBUG)

  #import config flags
  from AthenaConfiguration.AllConfigFlags import initConfigFlags
  flags = initConfigFlags()
  flags.Sim.ISFRun = True

  #Provide input
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  flags.Input.Files = defaultTestFiles.EVNT

  # Finalize
  flags.lock()

  ## Initialize the main component accumulator
  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  from SCT_G4_SD.SCT_G4_SDToolConfig import ITkStripSensorSDCfg
  from SCT_G4_SD.SCT_G4_SDToolConfig import SctSensorSDCfg
  from SCT_G4_SD.SCT_G4_SDToolConfig import SctSensor_CTBCfg

  tools = []
  cfg = ComponentAccumulator()
  tools += [ cfg.popToolsAndMerge(ITkStripSensorSDCfg(flags)) ]
  tools += [ cfg.popToolsAndMerge(SctSensorSDCfg(flags)) ]
  tools += [ cfg.popToolsAndMerge(SctSensor_CTBCfg(flags)) ]

  cfg.setPrivateTools(tools)
  cfg.printConfig(withDetails=True, summariseProps = True)
  flags.dump()

  with open("test.pkl", "wb") as f:
    cfg.store(f)

  print(cfg._privateTools)
  print("-----------------finished----------------------")
