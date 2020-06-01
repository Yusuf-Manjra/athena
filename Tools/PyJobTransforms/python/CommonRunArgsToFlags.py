# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Translate the commonly used runArgs into configFlags 
def commonRunArgsToFlags(runArgs,configFlags):
    ## Max/skip events
    if hasattr(runArgs,"skipEvents"):
        configFlags.Exec.SkipEvents=runArgs.skipEvents

    if hasattr(runArgs,"maxEvents"):
        configFlags.Exec.MaxEvents=runArgs.maxEvents

    if hasattr(runArgs,"conditionsTag"):
        configFlags.IOVDb.GlobalTag=runArgs.conditionsTag

    if hasattr(runArgs,"geometryVersion"): 
        configFlags.GeoModel.AtlasVersion=runArgs.geometryVersion

    if hasattr(runArgs,"beamType"):
        configFlags.Beam.Type=runArgs.beamType

    
