#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
'''@file SCTTracksMonAlg.py
@author Ken Kreul
@date 2019-07-15
@brief Based on AthenaMonitoring/ExampleMonitorAlgorithm.py
'''

def SCTTracksMonAlgConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''
    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags, 'SCTTracksMonCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    from SCT_Monitoring.SCT_MonitoringConf import SCTTracksMonAlg
    myMonAlg = helper.addAlgorithm(SCTTracksMonAlg, 'SCTTracksMonAlg')
    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    myMonAlg.TriggerChain = ''
    # myMonAlg.RandomHist = True

    ############################## WORKAROUND (START) ##########################
    ############################## TO RUN TRACKSUMMARYTOOL #####################

    # Taken from InnerDetector/InDetDigitization/PixelDigitization/python/PixelDigitizationConfigNew.py
    from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
    InDetPixelConditionsSummaryTool = result.popToolsAndMerge(PixelConditionsSummaryCfg(inputFlags))

    # Taken from Tracking/TrkExtrapolation/TrkExTools/python/AtlasExtrapolatorConfig.py
    # AtlasExtrapolatorConfig can give only private extrapolator. We need public extrapolator.
    from TrkDetDescrSvc.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    trackGeomCfg = TrackingGeometrySvcCfg(inputFlags)
    geom_svc = trackGeomCfg.getPrimary() 
    geom_svc.GeometryBuilder.Compactify = False ######## To avoid crash ########
    result.merge(trackGeomCfg)
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg(inputFlags))
    from TrkExTools.TrkExToolsConf import Trk__Navigator
    AtlasNavigator = Trk__Navigator(name = 'AtlasNavigator')
    AtlasNavigator.TrackingGeometrySvc = geom_svc
    result.addPublicTool(AtlasNavigator) 

    # Taken from InnerDetector/InDetExample/InDetRecExample/share/InDetRecLoadTools.py
    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
    InDetPropagator = Propagator(name = 'InDetPropagator')
    InDetPropagator.AccuracyParameter = 0.0001
    InDetPropagator.MaxStraightLineStep = .004
    result.addPublicTool(InDetPropagator)
    from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
    InDetMaterialUpdator = Trk__MaterialEffectsUpdator(name = "InDetMaterialEffectsUpdator")
    result.addPublicTool(InDetMaterialUpdator)
    InDetSubPropagators = []
    InDetSubUpdators    = []
    # -------------------- set it depending on the geometry ----------------------------------------------------
    # default for ID is (Rk,Mat)
    InDetSubPropagators += [ InDetPropagator.name() ]
    InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
    # default for Calo is (Rk,MatLandau)
    InDetSubPropagators += [ InDetPropagator.name() ]
    InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
    # default for MS is (STEP,Mat)
    #InDetSubPropagators += [ InDetStepPropagator.name() ]
    InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
    from TrkExTools.TrkExToolsConf import Trk__Extrapolator
    InDetExtrapolator = Trk__Extrapolator(name                    = 'InDetExtrapolator',
                                          Propagators             = [ InDetPropagator ],
                                          MaterialEffectsUpdators = [ InDetMaterialUpdator ],
                                          Navigator               = AtlasNavigator,
                                          SubPropagators          = InDetSubPropagators,
                                          SubMEUpdators           = InDetSubUpdators)
    result.addPublicTool(InDetExtrapolator)
    from InDetTestPixelLayer.InDetTestPixelLayerConf import InDet__InDetTestPixelLayerTool
    InDetTestPixelLayerTool = InDet__InDetTestPixelLayerTool(name = "InDetTestPixelLayerTool",
                                                             PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                             CheckActiveAreas=True,
                                                             CheckDeadRegions=True,
                                                             CheckDisabledFEs=True)
    result.addPublicTool(InDetTestPixelLayerTool)
    from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
    InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool",
                                                          Extrapolator = InDetExtrapolator,
                                                          PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                          usePixel      = inputFlags.Detector.GeometryPixel,
                                                          useSCT        = inputFlags.Detector.GeometrySCT,
                                                          CountDeadModulesAfterLastHit = True,
                                                          PixelLayerTool = InDetTestPixelLayerTool)
    result.addPublicTool(InDetHoleSearchTool)
    from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
    InDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool",
                                                                          PixelClusterAmbiguitiesMapName = "PixelClusterAmbiguitiesMap",
                                                                          SetupCorrect                   = True,
                                                                          addTRToutliers                 = True)
    result.addPublicTool(InDetPrdAssociationTool)
    from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
    InDetTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name            = "InDetSummaryHelper",
                                                                     AssoTool        = InDetPrdAssociationTool,
                                                                     PixelToTPIDTool = None,
                                                                     TestBLayerTool  = None,
                                                                     RunningTIDE_Ambi = True,
                                                                     DoSharedHits    = False,
                                                                     HoleSearch      = InDetHoleSearchTool,
                                                                     usePixel        = inputFlags.Detector.GeometryPixel,
                                                                     useSCT          = inputFlags.Detector.GeometrySCT,
                                                                     useTRT          = inputFlags.Detector.GeometryTRT)
    from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
    InDetTrackSummaryTool = Trk__TrackSummaryTool(name = "InDetTrackSummaryTool",
                                                  InDetSummaryHelperTool = InDetTrackSummaryHelperTool,
                                                  doSharedHits           = False,
                                                  doHolesInDet           = True,
                                                  TRT_ElectronPidTool    = None,
                                                  TRT_ToT_dEdxTool       = None,
                                                  PixelToTPIDTool        = None)
    ############################## WORKAROUND (END) ############################

    # Set InDetTrackSummaryTool to TrackSummaryTool of SCTLorentzMonAlg
    myMonAlg.TrackSummaryTool = InDetTrackSummaryTool

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # First, add a tool that's set up by a different configuration function. 
    # # In this case, CaloNoiseToolCfg returns its own component accumulator, 
    # # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(inputFlags)
    # result.merge(caloNoiseAcc)
    # myMonAlg.CaloNoiseTool = caloNoiseTool

    # set up geometry / conditions
    from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
    result.merge(InDetGeometryCfg(inputFlags))

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # myMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    myMonGroup = helper.addGroup(
        myMonAlg,
        "SCTTracksMonitor",
        "SCT/GENERAL/"
    )

    ### STEP 5 ###
    # Configure histograms
    regionNames = ["EndCapC", "Barrel", "EndCapA"]
    N_REGIONS = len(regionNames)
    s_triggerNames = ["RNDM", "BPTX", "L1CAL", "TGC", "RPC", "MBTS", "COSM", "Calib"]
    N_TRIGGER_TYPES = len(s_triggerNames)
    N_HIT_BINS =  50
    FIRST_HIT_BIN =  0
    LAST_HIT_BIN  = N_HIT_BINS-FIRST_HIT_BIN-1 # This is already defined in SCT_MonitoringNumbers.h
    myMonGroup.defineHistogram(varname="trk_N", # ; means alias
                                           type="TH1F",
                                           title="Number of tracks"+";Number of Tracks",
                                           path="tracks", # path cannot be "".
                                           xbins=400, xmin=0., xmax=4000.)
    myMonGroup.defineHistogram(varname="trk_chi2", # ; means alias
                                           type="TH1F",
                                           title="Track #chi^{2} div ndf"+";Number of track #chi^{2}/NDF",
                                           path="tracks", # path cannot be "".
                                           xbins=150, xmin=0., xmax=150.)
    myMonGroup.defineHistogram(varname="trk_d0", # ; means alias
                                           type="TH1F",
                                           title="Track d0"+";d0 [mm]",
                                           path="tracks", # path cannot be "".
                                           xbins=160, xmin=-40., xmax=40.)
    myMonGroup.defineHistogram(varname="trk_z0", # ; means alias
                                           type="TH1F",
                                           title="Track z0"+";z0 [mm]",
                                           path="tracks", # path cannot be "".
                                           xbins=200, xmin=-200., xmax=200.)
    myMonGroup.defineHistogram(varname="trk_phi", # ; means alias
                                           type="TH1F",
                                           title="Track Phi"+";#phi [rad]",
                                           path="tracks", # path cannot be "".
                                           xbins=160, xmin=-4., xmax=4.)
    myMonGroup.defineHistogram(varname="trk_pt", # ; means alias
                                           type="TH1F",
                                           title="Track P_{T}"+";P_{T} [GeV]",
                                           path="tracks", # path cannot be "".
                                           xbins=150, xmin=0., xmax=150.)

    myMonGroup.defineHistogram(varname="trk_sct_hits", # ; means alias
                                           type="TH1F",
                                           title="SCT HITS per single Track"+";Num of Hits",
                                           path="tracks", # path cannot be "".
                                           xbins=N_HIT_BINS, xmin=FIRST_HIT_BIN, xmax=LAST_HIT_BIN)
    myMonGroup.defineHistogram(varname="trk_eta", # ; means alias
                                           type="TH1F",
                                           title="Track Eta"+";#eta",
                                           path="tracks", # path cannot be "".
                                           xbins=160, xmin=-4., xmax=4.)
                                           
    myMonGroup.defineHistogram(varname="trackTriggers", # ; means alias
                                           type="TH1I",
                                           title="Tracks for different trigger types",
                                           path="tracks", # path cannot be "".
                                           xbins=N_TRIGGER_TYPES, xmin=-0.5, xmax=7.5, labels=s_triggerNames)
    myMonGroup.defineHistogram(varname="region"+","+"hitsRegion"+";"+"SCTTrackRate", # ; means alias
                                           type="TProfile",
                                           title="Track per event for SCT regions",
                                           path="tracks", # path cannot be "".
                                           xbins=3, xmin=0.0, xmax=3.0, labels=regionNames) #LABELS have to be added
    myMonGroup.defineHistogram(varname="tracksPerRegion", # ; means alias
                                           type="TH1F",
                                           title="Number of tracks in eta regions",
                                           path="tracks", # path cannot be "".
                                           xbins=N_REGIONS, xmin=0, xmax=N_REGIONS, labels=regionNames)


    for region in regionNames:
        myMonGroup.defineHistogram(varname="total"+region+"Residual",
                                    title="Overall Residual Distribution for the "+region+";Residual [mm]",
                                    type="TH1F", path="tracks",
                                    xbins=100, xmin=-0.5, xmax=0.5)
        myMonGroup.defineHistogram(varname="total"+region+"Pull",
                                    title="Overall Pull Distribution for the "+region+";Pull",
                                    type="TH1F", path="tracks",
                                    xbins=100, xmin=-5, xmax=5)
    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    #return helper.result()
    
    # # Otherwise, merge with result object and return
    result.merge(helper.result())
    return result

if __name__ == "__main__": 
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    #ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]
    ConfigFlags.Input.Files = [" /afs/cern.ch/work/k/kkreul/Qual/data18_13TeV.00364485.express_express.merge.RAW/data18_13TeV.00364485.express_express.merge.RAW._lb0706._SFO-3._0001.1 "]
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'SCTTracksMonOutput.root'
    ConfigFlags.GeoModel.Align.Dynamic = False
    ConfigFlags.Detector.GeometryID = True
    ConfigFlags.Detector.GeometryPixel = True
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.Detector.GeometryTRT = True
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    geoCfg=AtlasGeometryCfg(ConfigFlags)
    cfg.merge(geoCfg)

    sctTracksMonAcc = SCTTracksMonAlgConfig(ConfigFlags)
    cfg.merge(sctTracksMonAcc)

    cfg.run()
