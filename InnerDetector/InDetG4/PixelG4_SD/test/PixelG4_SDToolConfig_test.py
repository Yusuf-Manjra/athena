#!/usr/bin/env python
"""Run tests on PixelG4_SD configuration

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
  from PixelG4_SD.PixelG4_SDToolConfig import PixelSensorSDCfg
  from PixelG4_SD.PixelG4_SDToolConfig import PixelSensor_CTBCfg
  from PixelG4_SD.PixelG4_SDToolConfig import DBMSensorSDCfg

  tools = []
  cfg = ComponentAccumulator()
  tools += [ cfg.popToolsAndMerge(PixelSensorSDCfg(flags)) ]
  tools += [ cfg.popToolsAndMerge(PixelSensor_CTBCfg(flags)) ]
  tools += [ cfg.popToolsAndMerge(DBMSensorSDCfg(flags)) ]

  cfg.setPrivateTools(tools)
  cfg.printConfig(withDetails=True, summariseProps = True)
  flags.dump()

  with open("test.pkl", "wb") as f:
    cfg.store(f)

  print(cfg._privateTools)
  print("-----------------finished----------------------")
