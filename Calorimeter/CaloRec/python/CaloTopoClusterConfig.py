# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.SystemOfUnits import MeV

def caloTopoCoolFolderCfg(configFlags):
    result=ComponentAccumulator()
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    # rely on global tag for both MC and data; do not specify folder tags
    # use CALO_OFL only for GEO>=18
    hadCalibFolders = [
        "HadCalibration2/CaloEMFrac",
        "HadCalibration2/H1ClusterCellWeights",
        "HadCalibration2/CaloOutOfCluster",
        "HadCalibration2/CaloOutOfClusterPi0",
        "HadCalibration2/CaloDMCorr2"
    ]
    hadCalibPrefix = "/CALO/"
    hadCalibDB = "CALO_ONL"
    if configFlags.Input.isMC:
        hadCalibPrefix = "/CALO/Ofl/"
        hadCalibDB = "CALO_OFL"
    hadCalibFolders = [ hadCalibPrefix + foldername for foldername in hadCalibFolders ]
    result.merge(addFolders(configFlags, hadCalibFolders, hadCalibDB, className="CaloLocalHadCoeff"))

    return result

def getTopoClusterLocalCalibTools(configFlags):
    from CaloUtils.CaloUtilsConf import CaloLCClassificationTool, CaloLCWeightTool, CaloLCOutOfClusterTool, CaloLCDeadMaterialTool
    from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
    # Local cell weights
    LCClassify   = CaloLCClassificationTool("LCClassify")
    LCClassify.ClassificationKey   = "EMFracClassify"
    LCClassify.UseSpread = False
    LCClassify.MaxProbability = 0.5
    # add the moments EM_PROBABILITY, HAD_WEIGHT, OOC_WEIGHT, DM_WEIGHT to the AOD:
    LCClassify.StoreClassificationProbabilityInAOD = True
    LCClassify.WeightingOfNegClusters = configFlags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute

    LCWeight = CaloLCWeightTool("LCWeight")
    LCWeight.CorrectionKey       = "H1ClusterCellWeights"
    LCWeight.SignalOverNoiseCut  = 2.0
    # *****
    LCWeight.UseHadProbability   = True

    LocalCalib = CaloClusterLocalCalib ("LocalCalib")
    LocalCalib.ClusterClassificationTool     = [LCClassify]
    LocalCalib.ClusterRecoStatus             = [1,2]
    LocalCalib.LocalCalibTools               = [LCWeight]
    LocalCalib.WeightingOfNegClusters = configFlags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute

    # Out-of-cluster corrections
    LCOut     = CaloLCOutOfClusterTool("LCOut")
    LCOut.CorrectionKey       = "OOCCorrection"
    LCOut.UseEmProbability    = False
    LCOut.UseHadProbability   = True

    OOCCalib   = CaloClusterLocalCalib ("OOCCalib")
    OOCCalib.ClusterRecoStatus   = [1,2]
    OOCCalib.LocalCalibTools     = [LCOut]
    OOCCalib.WeightingOfNegClusters = configFlags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute

    LCOutPi0  = CaloLCOutOfClusterTool("LCOutPi0")
    LCOutPi0.CorrectionKey    = "OOCPi0Correction"
    LCOutPi0.UseEmProbability  = True
    LCOutPi0.UseHadProbability = False

    OOCPi0Calib   = CaloClusterLocalCalib ("OOCPi0Calib")
    OOCPi0Calib.ClusterRecoStatus   = [1,2]
    OOCPi0Calib.LocalCalibTools     = [LCOutPi0]

    OOCPi0Calib.WeightingOfNegClusters = configFlags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute

    # Dead material corrections
    LCDeadMaterial   = CaloLCDeadMaterialTool("LCDeadMaterial")
    LCDeadMaterial.HadDMCoeffKey       = "HadDMCoeff2"
    LCDeadMaterial.ClusterRecoStatus   = 0
    LCDeadMaterial.WeightModeDM        = 2 
    LCDeadMaterial.UseHadProbability   = True
    LCDeadMaterial.WeightingOfNegClusters = configFlags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute

    DMCalib    = CaloClusterLocalCalib ("DMCalib")
    DMCalib.ClusterRecoStatus   = [1,2]
    DMCalib.LocalCalibTools      = [LCDeadMaterial]

    DMCalib.WeightingOfNegClusters = configFlags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute

    lccalibtools = [
        LocalCalib,
        OOCCalib,
        OOCPi0Calib,
        DMCalib]
    return lccalibtools

def getTopoMoments(configFlags):
    from CaloRec.CaloRecConf import CaloClusterMomentsMaker
    TopoMoments = CaloClusterMomentsMaker ("TopoMoments")
    TopoMoments.WeightingOfNegClusters = configFlags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute
    from AthenaCommon.SystemOfUnits import deg
    TopoMoments.MaxAxisAngle = 20*deg
    TopoMoments.TwoGaussianNoise = configFlags.Calo.TopoCluster.doTwoGaussianNoise
    TopoMoments.MinBadLArQuality = 4000
    TopoMoments.MomentsNames = ["FIRST_PHI" 
                                ,"FIRST_ETA"
                                ,"SECOND_R" 
                                ,"SECOND_LAMBDA"
                                ,"DELTA_PHI"
                                ,"DELTA_THETA"
                                ,"DELTA_ALPHA" 
                                ,"CENTER_X"
                                ,"CENTER_Y"
                                ,"CENTER_Z"
                                ,"CENTER_MAG"
                                ,"CENTER_LAMBDA"
                                ,"LATERAL"
                                ,"LONGITUDINAL"
                                ,"FIRST_ENG_DENS" 
                                ,"ENG_FRAC_EM" 
                                ,"ENG_FRAC_MAX" 
                                ,"ENG_FRAC_CORE" 
                                ,"FIRST_ENG_DENS" 
                                ,"SECOND_ENG_DENS" 
                                ,"ISOLATION"
                                ,"ENG_BAD_CELLS"
                                ,"N_BAD_CELLS"
                                ,"N_BAD_CELLS_CORR"
                                ,"BAD_CELLS_CORR_E"
                                ,"BADLARQ_FRAC"
                                ,"ENG_POS"
                                ,"SIGNIFICANCE"
                                ,"CELL_SIGNIFICANCE"
                                ,"CELL_SIG_SAMPLING"
                                ,"AVG_LAR_Q"
                                ,"AVG_TILE_Q"
                                ,"PTD"
                                ,"MASS"
                                ]

    # Disable for now, as broken on MC
    if False:
        # *****
        # Is this still right?
        # only add HV related moments if it is offline.
        # from IOVDbSvc.CondDB import conddb
        # if not conddb.isOnline:
        from LArCellRec.LArCellRecConf import LArHVFraction
        if configFlags.Input.isMC:
            TopoMoments.LArHVFraction=LArHVFraction(HVScaleCorrKey="LArHVScaleCorr")
        else:
            TopoMoments.LArHVFraction=LArHVFraction(HVScaleCorrKey="LArHVScaleCorrRecomputed")
        TopoMoments.MomentsNames += ["ENG_BAD_HV_CELLS"
                                     ,"N_BAD_HV_CELLS"
                                     ]

    return TopoMoments

# a.k.a. DigiTruth
def getTopoTruthMoments(configFlags):
    from CaloRec.CaloRecConf import CaloClusterMomentsMaker_DigiHSTruth
    TopoMoments_Truth = CaloClusterMomentsMaker_DigiHSTruth ("TopoMoments_Truth")
    from LArCellRec.LArCellRecConf import LArHVFraction
    TopoMoments_Truth.LArHVFraction=LArHVFraction(HVScaleCorrKey="LArHVScaleCorr")
    TopoMoments_Truth.WeightingOfNegClusters = configFlags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute
    from AthenaCommon.SystemOfUnits import deg
    TopoMoments_Truth.MaxAxisAngle = 20*deg
    TopoMoments_Truth.TwoGaussianNoise = configFlags.Calo.TopoCluster.doTwoGaussianNoise
    TopoMoments_Truth.MinBadLArQuality = 4000
    TopoMoments_Truth.MomentsNames = ["FIRST_PHI_DigiHSTruth"
                                      ,"FIRST_ETA_DigiHSTruth"
                                      ,"SECOND_R_DigiHSTruth"
                                      ,"SECOND_LAMBDA_DigiHSTruth"
                                      ,"DELTA_PHI_DigiHSTruth"
                                      ,"DELTA_THETA_DigiHSTruth"
                                      ,"DELTA_ALPHA_DigiHSTruth"
                                      ,"CENTER_X_DigiHSTruth"
                                      ,"CENTER_Y_DigiHSTruth"
                                      ,"CENTER_Z_DigiHSTruth"
                                      ,"CENTER_MAG_DigiHSTruth"
                                      ,"CENTER_LAMBDA_DigiHSTruth"
                                      ,"LATERAL_DigiHSTruth"
                                      ,"LONGITUDINAL_DigiHSTruth"
                                      ,"ENG_FRAC_CORE_DigiHSTruth"
                                      ,"FIRST_ENG_DENS_DigiHSTruth"
                                      ,"SECOND_ENG_DENS_DigiHSTruth"
                                      ,"ISOLATION_DigiHSTruth"
                                      ,"BAD_CELLS_CORR_E_DigiHSTruth"
                                      ,"ENG_POS_DigiHSTruth"
                                      ,"SIGNIFICANCE_DigiHSTruth"
                                      ,"CELL_SIGNIFICANCE_DigiHSTruth"
                                      ,"CELL_SIG_SAMPLING_DigiHSTruth"
                                      ,"AVG_LAR_Q_DigiHSTruth"
                                      ,"AVG_TILE_Q_DigiHSTruth"
                                      ,"ENERGY_DigiHSTruth"
                                      ,"PHI_DigiHSTruth"
                                      ,"ETA_DigiHSTruth"
                                      ]
    return TopoMoments_Truth

def getTopoCalibMoments(configFlags):
    from CaloCalibHitRec.CaloCalibHitRecConf import CaloCalibClusterMomentsMaker2
    TopoCalibMoments = CaloCalibClusterMomentsMaker2 ("TopoCalibMoments")
    TopoCalibMoments.MomentsNames = ["ENG_CALIB_TOT"
                                     ,"ENG_CALIB_OUT_L"
                                     #,"ENG_CALIB_OUT_M"
                                     # ,"ENG_CALIB_OUT_T"
                                     # ,"ENG_CALIB_DEAD_L"
                                     # ,"ENG_CALIB_DEAD_M"
                                     # ,"ENG_CALIB_DEAD_T"
                                     ,"ENG_CALIB_EMB0"
                                     ,"ENG_CALIB_EME0"
                                     ,"ENG_CALIB_TILEG3"
                                     ,"ENG_CALIB_DEAD_TOT"
                                     ,"ENG_CALIB_DEAD_EMB0"
                                     ,"ENG_CALIB_DEAD_TILE0"
                                     ,"ENG_CALIB_DEAD_TILEG3"
                                     ,"ENG_CALIB_DEAD_EME0"
                                     ,"ENG_CALIB_DEAD_HEC0"
                                     ,"ENG_CALIB_DEAD_FCAL"
                                     ,"ENG_CALIB_DEAD_LEAKAGE"
                                     ,"ENG_CALIB_DEAD_UNCLASS"
                                     ,"ENG_CALIB_FRAC_EM"
                                     ,"ENG_CALIB_FRAC_HAD"
                                     ,"ENG_CALIB_FRAC_REST"]
    
    TopoCalibMoments.CalibrationHitContainerNames = ["LArCalibrationHitInactive"
                                                     ,"LArCalibrationHitActive"
                                                     ,"TileCalibHitActiveCell"
                                                     ,"TileCalibHitInactiveCell"]
    TopoCalibMoments.DMCalibrationHitContainerNames = ["LArCalibrationHitDeadMaterial"
                                                       ,"TileCalibHitDeadMaterial"]
    return TopoCalibMoments

# Steering options for trigger
# Maybe offline reco options should be extracted from flags elsewhere
def CaloTopoClusterCfg(configFlags,cellsname="AllCalo",clustersname="",doLCCalib=None,sequenceName='AthAlgSeq'):
    result=ComponentAccumulator()
    if (sequenceName != 'AthAlgSeq'):
        from AthenaCommon.AlgSequence import AthSequencer
        result.addSequence(AthSequencer(sequenceName))

    if not clustersname:
        clustersname = "CaloTopoClusters"

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg
    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    # Schedule total noise cond alg
    result.merge(CaloNoiseCondAlgCfg(configFlags,"totalNoise"))
    # Schedule electronic noise cond alg (needed for LC weights)
    result.merge(CaloNoiseCondAlgCfg(configFlags,"electronicNoise"))
    
    from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMaker, CaloClusterSnapshot

    result.merge(LArGMCfg(configFlags))

    from LArCalibUtils.LArHVScaleConfig import LArHVScaleCfg
    result.merge(LArHVScaleCfg(configFlags))

    result.merge(TileGMCfg(configFlags))

    if not doLCCalib:
        theCaloClusterSnapshot=CaloClusterSnapshot(OutputName=clustersname+"snapshot",SetCrossLinks=True)
    else:
        theCaloClusterSnapshot=CaloClusterSnapshot(OutputName=clustersname,SetCrossLinks=True)
         
    # maker tools
    TopoMaker = CaloTopoClusterMaker("TopoMaker")
        
    TopoMaker.CellsName = cellsname
    TopoMaker.CalorimeterNames=["LAREM",
                                "LARHEC",
                                "LARFCAL",
                                "TILE"]
    # cells from the following samplings will be able to form
    # seeds. By default no sampling is excluded
    TopoMaker.SeedSamplingNames = ["PreSamplerB", "EMB1", "EMB2", "EMB3",
                                   "PreSamplerE", "EME1", "EME2", "EME3",
                                   "HEC0", "HEC1","HEC2", "HEC3",
                                   "TileBar0", "TileBar1", "TileBar2",
                                   "TileExt0", "TileExt1", "TileExt2",
                                   "TileGap1", "TileGap2", "TileGap3",
                                   "FCAL0", "FCAL1", "FCAL2"] 
    TopoMaker.NeighborOption = "super3D"
    TopoMaker.RestrictHECIWandFCalNeighbors  = False
    TopoMaker.RestrictPSNeighbors  = True
    TopoMaker.CellThresholdOnEorAbsEinSigma     =    0.0
    TopoMaker.NeighborThresholdOnEorAbsEinSigma =    2.0
    TopoMaker.SeedThresholdOnEorAbsEinSigma     =    4.0
    
    # note E or AbsE 
    #
    # the following property must be set to TRUE in order to make double
    # sided cuts on the seed and the cluster level 
    #
    TopoMaker.SeedCutsInAbsE                 = True
    TopoMaker.ClusterEtorAbsEtCut            = 0.0*MeV
    # use 2-gaussian or single gaussian noise for TileCal
    TopoMaker.TwoGaussianNoise = configFlags.Calo.TopoCluster.doTwoGaussianNoise
        
    TopoSplitter = CaloTopoClusterSplitter("TopoSplitter")
    # cells from the following samplings will be able to form local
    # maxima. The excluded samplings are PreSamplerB, EMB1,
    # PreSamplerE, EME1, all Tile samplings, all HEC samplings and the
    # two rear FCal samplings.
    #
    TopoSplitter.SamplingNames = ["EMB2", "EMB3",
                                  "EME2", "EME3",
                                  "FCAL0"]
    # cells from the following samplings will also be able to form
    # local maxima but only if they are not overlapping in eta and phi
    # with local maxima in previous samplings from the primary list.
    #
    TopoSplitter.SecondarySamplingNames = ["EMB1","EME1",
                                           "TileBar0","TileBar1","TileBar2",
                                           "TileExt0","TileExt1","TileExt2",
                                           "HEC0","HEC1","HEC2","HEC3",
                                           "FCAL1","FCAL2"]
    TopoSplitter.ShareBorderCells = True
    TopoSplitter.RestrictHECIWandFCalNeighbors  = False
    TopoSplitter.WeightingOfNegClusters = configFlags.Calo.TopoCluster.doTreatEnergyCutAsAbsolute
    #
    # the following options are not set, since these are the default
    # values
    #
    # NeighborOption                = "super3D",
    # NumberOfCellsCut              = 4,
    # EnergyCut                     = 500*MeV,
        

    CaloTopoCluster=CaloClusterMaker(clustersname)
    CaloTopoCluster.ClustersOutputName=clustersname

    CaloTopoCluster.ClusterMakerTools = [TopoMaker, TopoSplitter]
    
    from CaloBadChannelTool.CaloBadChanToolConfig import CaloBadChanToolCfg
    caloBadChanTool = result.popToolsAndMerge( CaloBadChanToolCfg(configFlags) )
    from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterBadChannelList
    BadChannelListCorr = CaloClusterBadChannelList(badChannelTool = caloBadChanTool)
    CaloTopoCluster.ClusterCorrectionTools += [BadChannelListCorr]

    CaloTopoCluster.ClusterCorrectionTools += [getTopoMoments(configFlags)]

    if doLCCalib is None:
        doLCCalib = configFlags.Calo.TopoCluster.doTopoClusterLocalCalib
    if doLCCalib:
        CaloTopoCluster.ClusterCorrectionTools += [theCaloClusterSnapshot]
        #if not clustersname:
        CaloTopoCluster.ClustersOutputName="CaloCalTopoClusters"
        CaloTopoCluster.ClusterCorrectionTools += getTopoClusterLocalCalibTools(configFlags)

        from CaloRec.CaloTopoClusterConfig import caloTopoCoolFolderCfg
        result.merge(caloTopoCoolFolderCfg(configFlags))

    result.addEventAlgo(CaloTopoCluster,primary=True,sequenceName=sequenceName)
    return result



if __name__=="__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]
    ConfigFlags.Output.ESDFileName="esdOut.pool.root"

    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

    cfg=MainServicesThreadedCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    # from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg
    # cfg.mergeAll(IOVDbSvcCfg(ConfigFlags))
    
    theKey="CaloCalTopoClustersNew"

    topoAcc=CaloTopoClusterCfg(ConfigFlags)
    topoAlg = topoAcc.getPrimary()
    topoAlg.ClustersOutputName=theKey
    
    cfg.merge(topoAcc)

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(ConfigFlags,"xAOD", ItemList=["xAOD::CaloClusterContainer#CaloCalTopoClusters*",#+theKey,
                                                            "xAOD::CaloClusterAuxContainer#*CaloCalTopoClusters*Aux.",#+theKey+"Aux.",
                                                            # "CaloClusterCellLinkContainer#"+theKey+"_links"
                                                           ]))

    from AthenaServices.AthenaServicesConf import ThinningSvc, ThinningOutputTool
    cfg.addService(ThinningSvc())
    tot = ThinningOutputTool("Thin_xAOD",ThinningSvc = cfg.getService("ThinningSvc"))
    cfg.getEventAlgo("OutputStreamxAOD").HelperTools += [tot]

    from ThinningUtils.ThinningUtilsConf import ThinNegativeEnergyCaloClustersAlg
    theNegativeEnergyCaloClustersThinner = ThinNegativeEnergyCaloClustersAlg(
        "ThinNegativeEnergyCaloClustersAlg",
        CaloClustersKey=theKey,
        ThinNegativeEnergyCaloClusters = True,
    )
    cfg.addEventAlgo(theNegativeEnergyCaloClustersThinner,"AthAlgSeq")
  
#    cfg.getService("StoreGateSvc").Dump=True

    cfg.run(10)
    #f=open("CaloTopoCluster.pkl","wb")
    #cfg.store(f)
    #f.close()
    
