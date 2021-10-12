# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.AutoConfigFlags import GetFileMD, DetDescrInfo
from AthenaConfiguration.Enums import ProductionStep

def createGeoModelConfigFlags():
    gcf=AthConfigFlags()

    gcf.addFlag('GeoModel.Layout', 'atlas') # replaces global.GeoLayout

    gcf.addFlag("GeoModel.AtlasVersion",
                lambda prevFlags : ((prevFlags.Input.Files and
                                    prevFlags.Input.Files != ["_ATHENA_GENERIC_INPUTFILE_NAME_"] and
                                    GetFileMD(prevFlags.Input.Files).get("GeoAtlas",None))
                                    or "ATLAS-R2-2016-01-00-01"))

    gcf.addFlag("GeoModel.Align.Dynamic",
                lambda prevFlags : prevFlags.Common.ProductionStep != ProductionStep.Simulation and not prevFlags.Input.isMC)

    gcf.addFlag("GeoModel.Align.LegacyConditionsAccess",
                lambda prevFlags : prevFlags.Common.Project == "AthSimulation" or prevFlags.Common.ProductionStep == ProductionStep.Simulation)
            # Mainly for G4 which still loads alignment on initialize

    gcf.addFlag("GeoModel.StripGeoType",
                lambda prevFlags : DetDescrInfo(prevFlags.GeoModel.AtlasVersion)['Common']['StripGeoType'])

    gcf.addFlag("GeoModel.Run",
                lambda prevFlags : DetDescrInfo(prevFlags.GeoModel.AtlasVersion)['Common']['Run'])
                # Based on CommonGeometryFlags.Run (InDetGeometryFlags.isSLHC replaced by GeoModel.Run=="RUN4")

    gcf.addFlag("GeoModel.Type",
                lambda prevFlags : DetDescrInfo(prevFlags.GeoModel.AtlasVersion)['Common']['GeoType'])
                # Geometry type in {ITKLoI, ITkLoI-VF, etc...}

    gcf.addFlag("GeoModel.IBLLayout",
                lambda prevFlags : DetDescrInfo(prevFlags.GeoModel.AtlasVersion)['Pixel']['IBLlayout'])
                # IBL layer layout  in {"planar", "3D", "noIBL"}

    gcf.addFlag("GeoModel.useLocalGeometry", False)  # use local XML files for development

    return gcf
