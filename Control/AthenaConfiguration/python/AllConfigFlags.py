# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import TeV
from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.AutoConfigFlags import GetFileMD, getInitialTimeStampsFromRunNumbers, getRunToTimestampDict, getSpecialConfigurationMetadata
from AthenaConfiguration.Enums import ProductionStep
from PyUtils.moduleExists import moduleExists


def _addFlagsCategory (acf, name, generator, modName = None):
    if moduleExists (modName):
        return acf.addFlagsCategory (name, generator)
    return None

        
def _createCfgFlags():

    acf=AthConfigFlags()

    #Flags steering the job execution:
    from AthenaCommon.Constants import INFO
    acf.addFlag('Exec.OutputLevel',INFO) #Global Output Level
    acf.addFlag('Exec.MaxEvents',-1) 
    acf.addFlag('Exec.SkipEvents',0)
    acf.addFlag('Exec.DebugStage','')

    acf.addFlag('ExecutorSplitting.TotalSteps', 0)
    acf.addFlag('ExecutorSplitting.Step', -1)
    acf.addFlag('ExecutorSplitting.TotalEvents', -1)

    #Flags describing the input data 
    acf.addFlag('Input.Files', ["_ATHENA_GENERIC_INPUTFILE_NAME_",] ) # former global.InputFiles
    acf.addFlag('Input.SecondaryFiles', []) # secondary input files for DoubleEventSelector
    acf.addFlag('Input.isMC', lambda prevFlags : "IS_SIMULATION" in GetFileMD(prevFlags.Input.Files).get("eventTypes",[]) ) # former global.isMC
    acf.addFlag('Input.OverrideRunNumber', False )
    acf.addFlag('Input.RunNumber', lambda prevFlags : list(GetFileMD(prevFlags.Input.Files).get("runNumbers",[]))) # former global.RunNumber
    acf.addFlag('Input.LumiBlockNumber', lambda prevFlags : list(GetFileMD(prevFlags.Input.Files).get("lumiBlockNumbers",[]))) # former global.RunNumber
    acf.addFlag('Input.TimeStamp', lambda prevFlags : [] if not prevFlags.Input.OverrideRunNumber else getInitialTimeStampsFromRunNumbers(prevFlags.Input.RunNumber))
    # Configure EvtIdModifierSvc with a list of dictionaries of the form:
    # {'run': 152166, 'lb': 202, 'starttstamp': 1269948352889940910, 'dt': 104.496, 'evts': 1, 'mu': 0.005, 'force_new': False}
    acf.addFlag("Input.RunAndLumiOverrideList", [])

    acf.addFlag('Input.ProjectName', lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("project_name","data17_13TeV") ) # former global.ProjectName
    acf.addFlag('Input.Format', lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("file_type","") ) # former global.InputFormat
    acf.addFlag('Input.ProcessingTags', lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("processingTags","") ) # list of names of streams written to this file
    acf.addFlag('Input.SpecialConfiguration', lambda prevFlags : getSpecialConfigurationMetadata(prevFlags.Input.Files, prevFlags.Input.SecondaryFiles))  # special Configuration options read from input file metadata

    def _inputCollections(inputFile):
        if not inputFile:
            return []

        rawCollections = [type_key[1] for type_key in GetFileMD(inputFile).get("itemList",[])]
        collections = [col for col in rawCollections if not col.endswith('Aux.') ]
        return collections

    def _typedInputCollections(inputFile):
        if not inputFile:
            return []

        collections = ['%s#%s' % type_key for type_key in GetFileMD(inputFile).get("itemList",[])]
        return collections

    acf.addFlag('Input.Collections', lambda prevFlags : _inputCollections(prevFlags.Input.Files) )
    acf.addFlag('Input.SecondaryCollections', lambda prevFlags : _inputCollections(prevFlags.Input.SecondaryFiles) )
    acf.addFlag('Input.TypedCollections', lambda prevFlags : _typedInputCollections(prevFlags.Input.Files) )

    acf.addFlag('Concurrency.NumProcs', 0)
    acf.addFlag('Concurrency.NumThreads', 0 )
    acf.addFlag('Concurrency.NumConcurrentEvents', lambda prevFlags : prevFlags.Concurrency.NumThreads)

    acf.addFlag('Scheduler.CheckDependencies', True)
    acf.addFlag('Scheduler.ShowDataDeps', True)
    acf.addFlag('Scheduler.ShowDataFlow', True)
    acf.addFlag('Scheduler.ShowControlFlow', True)
    acf.addFlag('Scheduler.EnableVerboseViews', True)

    acf.addFlag('Common.MsgSourceLength',50) #Length of the source-field in the format str of MessageSvc
    acf.addFlag('Common.isOnline', False ) #  Job runs in an online environment (access only to resources available at P1) # former global.isOnline
    acf.addFlag('Common.useOnlineLumi', lambda prevFlags : prevFlags.Common.isOnline ) #  Use online version of luminosity. ??? Should just use isOnline?
    acf.addFlag('Common.doExpressProcessing', False)
    acf.addFlag('Common.bunchCrossingSource', lambda prevFlags : "MC" if prevFlags.Input.isMC else "TrigConf") # what BunchCrossingTool should we use?
    acf.addFlag('Common.ProductionStep', ProductionStep.Default)

    def _checkProject():
        import os
        if "AthSimulation_DIR" in os.environ:
            return "AthSimulation"
        if "AthGeneration_DIR" in os.environ:
            return "AthGeneration"
        if "AthAnalysis_DIR" in os.environ:
            return "AthAnalysis"
        #TODO expand this method.
        return "Athena"
    acf.addFlag('Common.Project', _checkProject())

    # replace global.Beam*
    acf.addFlag('Beam.BunchSpacing', 25) # former global.BunchSpacing
    acf.addFlag('Beam.Type', lambda prevFlags : GetFileMD(prevFlags.Input.Files).get('beam_type','collisions') )# former global.BeamType
    acf.addFlag("Beam.NumberOfCollisions", lambda prevFlags : 2. if prevFlags.Beam.Type=='collisions' else 0.) # former global.NumberOfCollisions
    acf.addFlag('Beam.Energy', lambda prevFlags : GetFileMD(prevFlags.Input.Files).get('beam_energy',7*TeV)) # former global.BeamEnergy
    acf.addFlag('Beam.estimatedLuminosity', lambda prevFlags : ( 1E33*(prevFlags.Beam.NumberOfCollisions)/2.3 ) *\
        (25./prevFlags.Beam.BunchSpacing)) # former flobal.estimatedLuminosity



    acf.addFlag('Output.EVNTFileName', '')
    acf.addFlag('Output.EVNT_TRFileName', '')
    acf.addFlag('Output.HITSFileName', '')
    acf.addFlag('Output.RDOFileName',  '')
    acf.addFlag('Output.RDO_SGNLFileName', '')
    acf.addFlag('Output.ESDFileName',  '')
    acf.addFlag('Output.AODFileName',  '')
    acf.addFlag('Output.HISTFileName', '')
    

    acf.addFlag('Output.doWriteRDO', lambda prevFlags: bool(prevFlags.Output.RDOFileName)) # write out RDO file
    acf.addFlag('Output.doWriteRDO_SGNL', lambda prevFlags: bool(prevFlags.Output.RDO_SGNLFileName)) # write out RDO_SGNL file
    acf.addFlag('Output.doWriteESD', lambda prevFlags: bool(prevFlags.Output.ESDFileName)) # write out ESD file
    acf.addFlag('Output.doWriteAOD', lambda prevFlags: bool(prevFlags.Output.AODFileName)) # write out AOD file
    acf.addFlag('Output.doWriteBS',  False) # write out RDO ByteStream file

    # Might move this elsewhere in the future.
    # Some flags from https://gitlab.cern.ch/atlas/athena/blob/master/Tracking/TrkDetDescr/TrkDetDescrSvc/python/TrkDetDescrJobProperties.py
    # (many, e.g. those that set properties of one tool are not needed)
    acf.addFlag('TrackingGeometry.MagneticFileMode', 6)
    acf.addFlag('TrackingGeometry.MaterialSource', 'COOL') # Can be COOL, Input or None

#Detector Flags:
    def __detector():
        from AthenaConfiguration.DetectorConfigFlags import createDetectorConfigFlags
        return createDetectorConfigFlags()
    acf.addFlagsCategory( "Detector", __detector )

#Simulation Flags:
    def __simulation():
        from G4AtlasApps.SimConfigFlags import createSimConfigFlags
        return createSimConfigFlags()
    _addFlagsCategory (acf, "Sim", __simulation, 'G4AtlasApps' )

#Digitization Flags:
    def __digitization():
        from Digitization.DigitizationConfigFlags import createDigitizationCfgFlags
        return createDigitizationCfgFlags()
    _addFlagsCategory(acf, "Digitization", __digitization, 'Digitization' )

#Overlay Flags:
    def __overlay():
        from OverlayConfiguration.OverlayConfigFlags import createOverlayConfigFlags
        return createOverlayConfigFlags()
    _addFlagsCategory(acf, "Overlay", __overlay, 'OverlayConfiguration' )

#Geo Model Flags:
    def __geomodel():
        from AthenaConfiguration.GeoModelConfigFlags import createGeoModelConfigFlags
        return createGeoModelConfigFlags()
    acf.addFlagsCategory( "GeoModel", __geomodel )

#Reco Flags:
    def __reco():
        from RecJobTransforms.RecoConfigFlags import createRecoConfigFlags
        return createRecoConfigFlags()
    _addFlagsCategory(acf, "Reco", __reco, 'RecJobTransforms')

#IOVDbSvc Flags:
    acf.addFlag("IOVDb.GlobalTag",lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("IOVDbGlobalTag",None) or "CONDBR2-BLKPA-2017-05")
    from IOVDbSvc.IOVDbAutoCfgFlags import getDatabaseInstanceDefault
    acf.addFlag("IOVDb.DatabaseInstance",getDatabaseInstanceDefault)
    # Run dependent simulation
    # map from runNumber to timestamp; migrated from RunDMCFlags.py
    acf.addFlag("IOVDb.RunToTimestampDict", lambda prevFlags: getRunToTimestampDict())
    acf.addFlag("IOVDb.DBConnection", lambda prevFlags : "sqlite://;schema=mycool.db;dbname=" + prevFlags.IOVDb.DatabaseInstance)


    def __bfield():
        from MagFieldConfig.BFieldConfigFlags import createBFieldConfigFlags
        return createBFieldConfigFlags()
    _addFlagsCategory(acf, "BField", __bfield, 'MagFieldConfig')

    def __lar():
        from LArConfiguration.LArConfigFlags import createLArConfigFlags
        return createLArConfigFlags()
    _addFlagsCategory(acf, "LAr", __lar, 'LArConfiguration' ) 

    def __tile():
        from TileConfiguration.TileConfigFlags import createTileConfigFlags
        return createTileConfigFlags()
    _addFlagsCategory(acf, 'Tile', __tile, 'TileConfiguration' )


    def __calo():
        from CaloRec.CaloConfigFlags import createCaloConfigFlags
        return createCaloConfigFlags()
    _addFlagsCategory(acf, 'Calo', __calo, 'CaloRec' )

#Random engine Flags:
    acf.addFlag("Random.Engine", "dSFMT") # Random service used in {"dSFMT", "Ranlux64", "Ranecu"}

    def __trigger():
        from TriggerJobOpts.TriggerConfigFlags import createTriggerFlags
        return createTriggerFlags()
    _addFlagsCategory(acf, "Trigger", __trigger, 'TriggerJobOpts' )

    def __indet():
        from InDetConfig.InDetConfigFlags import createInDetConfigFlags
        return createInDetConfigFlags()
    _addFlagsCategory(acf, "InDet", __indet, 'InDetConfig' )

    def __itk():
        from InDetConfig.ITkConfigFlags import createITkConfigFlags
        return createITkConfigFlags()
    _addFlagsCategory(acf, "ITk", __itk, 'InDetConfig' )

    def __hgtd():
        from HGTD_Config.HGTD_ConfigFlags import createHGTD_ConfigFlags
        return createHGTD_ConfigFlags()
    _addFlagsCategory(acf, "HGTD", __hgtd, 'HGTD_Config' )

    def __muon():
        from MuonConfig.MuonConfigFlags import createMuonConfigFlags
        return createMuonConfigFlags()
    _addFlagsCategory(acf, "Muon", __muon, 'MuonConfig' )

    def __muoncombined():
        from MuonCombinedConfig.MuonCombinedConfigFlags import createMuonCombinedConfigFlags
        return createMuonCombinedConfigFlags()
    _addFlagsCategory(acf, "MuonCombined", __muoncombined, 'MuonCombinedConfig' )

    def __egamma():
        from egammaConfig.egammaConfigFlags import createEgammaConfigFlags
        return createEgammaConfigFlags()
    _addFlagsCategory(acf, "Egamma", __egamma, 'egammaConfig' )

    def __met():
        from METReconstruction.METConfigFlags import createMETConfigFlags
        return createMETConfigFlags()
    _addFlagsCategory(acf,"MET",__met, 'METReconstruction')

    def __tau():
        from tauRec.TauConfigFlags import createTauConfigFlags
        return createTauConfigFlags()
    _addFlagsCategory(acf, "Tau",__tau, 'tauRec')

    def __pflow():
        from eflowRec.PFConfigFlags import createPFConfigFlags
        return createPFConfigFlags()
    _addFlagsCategory(acf,"PF",__pflow, 'eflowRec')

    def __btagging():
        from BTagging.BTaggingConfigFlags import createBTaggingConfigFlags
        return createBTaggingConfigFlags()
    _addFlagsCategory(acf,"BTagging",__btagging, 'BTagging')

    def __hi():
        from HIRecConfig.HIRecConfigFlags import createHIRecConfigFlags
        return createHIRecConfigFlags()
    _addFlagsCategory(acf, "HeavyIon", __hi, "HIRecConfig")

    def __dq():
        from AthenaMonitoring.DQConfigFlags import createDQConfigFlags
        dqf = createDQConfigFlags()
        return dqf
    _addFlagsCategory(acf, "DQ", __dq, 'AthenaMonitoring' )

    def __perfmon():
        from PerfMonComps.PerfMonConfigFlags import createPerfMonConfigFlags
        return createPerfMonConfigFlags()
    _addFlagsCategory(acf, "PerfMon", __perfmon, 'PerfMonComps')

    return acf


ConfigFlags=_createCfgFlags()

if __name__=="__main__":
    import sys
    if len(sys.argv)>1:
        ConfigFlags.Input.Files = sys.argv[1:]
    else:
        ConfigFlags.Input.Files = [ "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1",]
    
    ConfigFlags.loadAllDynamicFlags()
    ConfigFlags.initAll()
    ConfigFlags.dump()
    
