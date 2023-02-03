#!/usr/bin/env python
"""Run tests on BLM_G4_SD configuration

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

  # Setup the tool
  from BLM_G4_SD.BLM_G4_SDToolConfig import BLMSensorSDCfg
  cfg = BLMSensorSDCfg(flags)
  cfg.printConfig(withDetails=True, summariseProps = True)
  flags.dump()

  with open("test.pkl", "wb") as f:
    cfg.store(f)

  print(cfg._privateTools)
  print("-----------------finished----------------------")
