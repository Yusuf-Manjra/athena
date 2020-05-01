#!/usr/bin/env python
"""Run tests on G4Geometry Tool configuration

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


if __name__ == '__main__':
  from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
  import os

  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = 1


  #import config flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT

  ConfigFlags.Detector.GeometryPixel = True
  ConfigFlags.Detector.GeometrySCT = True
  ConfigFlags.Detector.GeometryTRT = True
  ConfigFlags.Detector.SimulateMuon = True
  ConfigFlags.Detector.SimulateID = True
  ConfigFlags.Detector.SimulateCalo = True
  ConfigFlags.Detector.SimulateBpipe = True
  ConfigFlags.Detector.SimulateFwdRegion = True
  ConfigFlags.Detector.GeometryLAr = True
  ConfigFlags.Detector.GeometryTile = True
  #turn the forward region off
  ConfigFlags.Detector.GeometryLucid = False
  ConfigFlags.Detector.GeometryZDC = False
  ConfigFlags.Detector.GeometryALFA = False
  ConfigFlags.Detector.GeometryAFP = False
  ConfigFlags.Sim.WorldRRange = 15000
  ConfigFlags.Sim.WorldZRange = 27000

  ConfigFlags.Sim.TwissFileNomReal = "nominal" #so it doesn't crash

  # Finalize 
  ConfigFlags.lock()


  from G4AtlasTools.G4GeometryToolConfig import BeamPipeGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import PixelGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import SCTGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import TRTGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import IDETEnvelopeCfg
  from G4AtlasTools.G4GeometryToolConfig import ATLASEnvelopeCfg
  from G4AtlasTools.G4GeometryToolConfig import CALOEnvelopeCfg

  ## Initialize a new component accumulator
  cfg = ComponentAccumulator()

  acc, tool  = BeamPipeGeoDetectorToolCfg(ConfigFlags)
  cfg.merge(acc)
  cfg.addPublicTool(tool)

  acc1, tool1  = PixelGeoDetectorToolCfg(ConfigFlags)
  cfg.merge(acc1)
  cfg.addPublicTool(tool1)

  acc2, tool2  = SCTGeoDetectorToolCfg(ConfigFlags)
  cfg.merge(acc2)
  cfg.addPublicTool(tool2)

  acc3, tool3  = TRTGeoDetectorToolCfg(ConfigFlags)
  cfg.merge(acc3)
  cfg.addPublicTool(tool3)

  acc4, tool4  = IDETEnvelopeCfg(ConfigFlags)
  cfg.merge(acc4)
  cfg.addPublicTool(tool4)

  acc6,tool6 = CALOEnvelopeCfg(ConfigFlags)
  cfg.merge(acc6)
  cfg.addPublicTool(tool6)
  

  accATLAS,toolATLAS = ATLASEnvelopeCfg(ConfigFlags)
  cfg.merge(accATLAS)
  cfg.addPublicTool(toolATLAS)

  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()

  f=open("test.pkl","wb")
  cfg.store(f) 
  f.close()

  print ("-----------------finished----------------------")
