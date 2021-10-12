# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def ITkStripGeoModelCfg(flags):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(flags)
    geoModelSvc = acc.getPrimary()

    from AthenaConfiguration.ComponentFactory import CompFactory
    ITkStripDetectorTool = CompFactory.StripDetectorTool()
    # ITkStripDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic #Will we need to do dynamic alignment for ITk?
    ITkStripDetectorTool.Alignable = False # make this a flag? Set true as soon as decided on folder structure
    ITkStripDetectorTool.DetectorName = "ITkStrip"
    if flags.GeoModel.useLocalGeometry:
        # Setting this filename triggers reading from local file rather than DB  
        ITkStripDetectorTool.GmxFilename = flags.ITk.stripGeometryFilename 
    geoModelSvc.DetectorTools += [ ITkStripDetectorTool ]
    return acc


def ITkStripAlignmentCfg(flags):
    if flags.GeoModel.Align.LegacyConditionsAccess:
        from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
        return addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Align", "/Indet/Align")
    else:
        from SCT_ConditionsAlgorithms.SCT_AlignCondAlgConfig import ITkStripAlignCondAlgCfg
        return ITkStripAlignCondAlgCfg(flags)


def ITkStripSimulationGeometryCfg(flags):
    # main GeoModel config
    acc = ITkStripGeoModelCfg(flags)
    acc.merge(ITkStripAlignmentCfg(flags))
    return acc


def ITkStripReadoutGeometryCfg(flags):
    # main GeoModel config
    acc = ITkStripGeoModelCfg(flags)
    acc.merge(ITkStripAlignmentCfg(flags))
    from SCT_ConditionsAlgorithms.SCT_DetectorElementCondAlgConfig import ITkStripDetectorElementCondAlgCfg
    acc.merge(ITkStripDetectorElementCondAlgCfg(flags))
    return acc
