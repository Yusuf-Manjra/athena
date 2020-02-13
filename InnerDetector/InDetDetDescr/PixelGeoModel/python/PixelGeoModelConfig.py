# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from AthenaConfiguration.ComponentFactory import CompFactory

def getPixelDetectorTool(name="PixelDetectorTool", **kwargs):
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
    if InDetGeometryFlags.isSLHC():
        kwargs.setdefault("ServiceBuilderTool",    "InDetServMatBuilderToolSLHC")
    else:
        kwargs.setdefault("ServiceBuilderTool",    "")

    return CfgMgr.PixelDetectorTool(name, **kwargs)


############## ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def PixelGeometryCfg( flags ):
  from AtlasGeoModel.GeoModelConfig import GeoModelCfg
  acc = GeoModelCfg( flags )
  geoModelSvc=acc.getPrimary()
  GeometryDBSvc=CompFactory.GeometryDBSvc
  acc.addService(GeometryDBSvc("InDetGeometryDBSvc"))
  PixelDetectorTool=CompFactory.PixelDetectorTool
  pixelDetectorTool = PixelDetectorTool("PixelDetectorTool")
  InDetDD__BCM_Builder=CompFactory.InDetDD__BCM_Builder
  bcmTool = InDetDD__BCM_Builder()
  pixelDetectorTool.BCM_Tool = bcmTool
  InDetDD__BLM_Builder=CompFactory.InDetDD__BLM_Builder
  blmTool = InDetDD__BLM_Builder()
  pixelDetectorTool.BLM_Tool = blmTool
  if flags.GeoModel.Run=="RUN4":
      InDetServMatBuilderToolSLHC=CompFactory.InDetServMatBuilderToolSLHC
      InDetServMatBuilderToolSLHC = InDetServMatBuilderToolSLHC()
      acc.addPublicTool( InDetServMatBuilderToolSLHC )
      pixelDetectorTool.ServiceBuilderTool = InDetServMatBuilderToolSLHC
  pixelDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
  geoModelSvc.DetectorTools += [ pixelDetectorTool ]
  acc.addService(geoModelSvc)

  # IBL module distortions
  acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/IBLDist","/Indet/IBLDist",className="CondAttrListCollection"))

  if flags.GeoModel.Align.Dynamic:
    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection"))
    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL2/PIX","/Indet/AlignL2/PIX",className="CondAttrListCollection"))
    acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer"))
  else:
    if (not flags.Detector.SimulatePixel) or flags.Detector.OverlayPixel:
      acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))
    else:
      acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Align","/Indet/Align"))
  if flags.Common.Project != "AthSimulation": # Protection for AthSimulation builds
      if (not flags.Detector.SimulatePixel) or flags.Detector.OverlayPixel:
          PixelAlignCondAlg=CompFactory.PixelAlignCondAlg
          pixelAlignCondAlg = PixelAlignCondAlg(name = "PixelAlignCondAlg",
                                                UseDynamicAlignFolders = flags.GeoModel.Align.Dynamic)
          acc.addCondAlgo(pixelAlignCondAlg)
          PixelDetectorElementCondAlg=CompFactory.PixelDetectorElementCondAlg
          pixelDetectorElementCondAlg = PixelDetectorElementCondAlg(name = "PixelDetectorElementCondAlg")
          acc.addCondAlgo(pixelDetectorElementCondAlg)
  return acc
