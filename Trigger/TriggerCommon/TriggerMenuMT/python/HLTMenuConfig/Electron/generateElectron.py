# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Electron.ElectronRecoSequences import l2CaloRecoCfg
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequenceCA, \
    ChainStep, Chain, EmptyMenuSequence, InViewRecoCA, SelectionCA

from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import TrigEgammaFastCaloHypoToolFromDict
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import getChainMultFromDict

from AthenaConfiguration.AccumulatorCache import AccumulatorCache

@AccumulatorCache
def _fastCaloSeq(flags):
    selAcc=SelectionCA('FastCaloElectron')
    selAcc.mergeReco(l2CaloRecoCfg(flags))

     # this alg needs EventInfo decorated with the  pileup info
    from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
    selAcc.merge(LumiBlockMuWriterCfg(flags))

    from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import TrigEgammaFastCaloHypoAlgCfg
    l2CaloHypo = TrigEgammaFastCaloHypoAlgCfg(flags,
                                              name='ElectronEgammaFastCaloHypo',
                                              CaloClusters=recordable('HLT_FastCaloEMClusters'))
    
    selAcc.mergeHypo(l2CaloHypo)

    fastCaloSequence = MenuSequenceCA(selAcc,
                                      HypoToolGen=TrigEgammaFastCaloHypoToolFromDict)
    return (selAcc , fastCaloSequence)

def _fastCalo(flags, chainDict):

    selAcc , fastCaloSequence = _fastCaloSeq(flags)

     # this cannot work for asymmetric combined chains....FP
    return ChainStep(name=selAcc.name, Sequences=[fastCaloSequence], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))

@AccumulatorCache
def _ftfSeq(flags):
    selAcc=SelectionCA('ElectronFTF')
    # # # fast ID (need to be customised because require secialised configuration of the views maker - i.e. parent has to be linked)
    name = 'IMFastElectron'
    evtViewMaker = CompFactory.EventViewCreatorAlgorithm(name,
                                                            ViewFallThrough = True,
                                                            RoIsLink        = 'initialRoI',
                                                            RoITool         = CompFactory.ViewCreatorInitialROITool(),
                                                            InViewRoIs      = name+'RoIs',
                                                            Views           = name+'Views',
                                                            ViewNodeName    = 'FastElectronInView',
                                                            RequireParentView = True)
    del name
    from TrigInDetConfig.TrigInDetConfig import trigInDetFastTrackingCfg
    idTracking = trigInDetFastTrackingCfg(flags, roisKey=evtViewMaker.InViewRoIs, signatureName='Electron')
    fastInDetReco = InViewRecoCA('FastElectron', viewMaker=evtViewMaker)
    fastInDetReco.mergeReco(idTracking)
    fastInDetReco.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDVElectronFastCalo',
                                DataObjects=[('xAOD::TrigEMClusterContainer', 'StoreGateSvc+HLT_FastCaloEMClusters')]) )

    from TrigEgammaRec.TrigEgammaFastElectronConfig import fastElectronFexAlgCfg
    fastInDetReco.mergeReco(fastElectronFexAlgCfg(flags, rois=evtViewMaker.InViewRoIs))
    selAcc.mergeReco(fastInDetReco)
    fastElectronHypoAlg = CompFactory.TrigEgammaFastElectronHypoAlg()
    fastElectronHypoAlg.Electrons = 'HLT_FastElectrons'
    fastElectronHypoAlg.RunInView = True
    selAcc.addHypoAlgo(fastElectronHypoAlg)
    from TrigEgammaHypo.TrigEgammaFastElectronHypoTool import TrigEgammaFastElectronHypoToolFromDict
    fastInDetSequence = MenuSequenceCA(selAcc,
                                        HypoToolGen=TrigEgammaFastElectronHypoToolFromDict)
    return (selAcc , fastInDetSequence)

def _ftf(flags, chainDict):
    selAcc , fastInDetSequence = _ftfSeq(flags)
    return ChainStep( name=selAcc.name, Sequences=[fastInDetSequence], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))

@AccumulatorCache
def _precisonCaloSeq(flags):
    recoAcc = InViewRecoCA('ElectronRoITopoClusterReco', RequireParentView = True)
    recoAcc.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDV'+recoAcc.name,
                                                              DataObjects=[('TrigRoiDescriptorCollection', recoAcc.inputMaker().InViewRoIs),
                                                                           ('CaloBCIDAverage', 'StoreGateSvc+CaloBCIDAverage')]))

    from TrigCaloRec.TrigCaloRecConfig import hltCaloTopoClusteringCfg
    recoAcc.mergeReco(hltCaloTopoClusteringCfg(flags,
                                               namePrefix='Electron',
                                               roisKey=recoAcc.inputMaker().InViewRoIs) ) # RoI

    copier = CompFactory.egammaTopoClusterCopier('TrigEgammaTopoClusterCopierPrecisionCaloRoIs',
                                                 InputTopoCollection='HLT_TopoCaloClustersRoI',
                                                 OutputTopoCollection='HLT_CaloEMClusters',
                                                 OutputTopoCollectionShallow='tmp_HLT_CaloEMClusters')
    recoAcc.addRecoAlgo(copier)

    selAcc = SelectionCA('PrecisionCalo')
    selAcc.mergeReco(recoAcc)
    hypoAlg = CompFactory.TrigEgammaPrecisionCaloHypoAlg(name='ElectronPrecisionCaloHypo',
                                                           CaloClusters=recordable('HLT_CaloEMClusters'))
    selAcc.addHypoAlgo(hypoAlg)
    from TrigEgammaHypo.TrigEgammaPrecisionCaloHypoTool import TrigEgammaPrecisionCaloHypoToolFromDict
    menuSequence = MenuSequenceCA(selAcc,
                                  HypoToolGen=TrigEgammaPrecisionCaloHypoToolFromDict)
    return (selAcc , menuSequence)

def _precisonCalo(flags, chainDict):
    selAcc , menuSequence = _precisonCaloSeq(flags)
    return ChainStep(name=selAcc.name, Sequences=[menuSequence], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))

@AccumulatorCache
def _precisionTrackingSeq(flags):
    name='ElectronPrecisionTracking'
    selAcc=SelectionCA('ElectronPrecisionTracking')

    evtViewMaker = CompFactory.EventViewCreatorAlgorithm(f'IM{name}',
                                                            ViewFallThrough = True,
                                                            RoIsLink        = 'initialRoI',
                                                            RoITool         = CompFactory.ViewCreatorPreviousROITool(),
                                                            InViewRoIs      = flags.Trigger.InDetTracking.Electron.roi, #this will then be renamed to: flags.InDet.Tracking.roi
                                                            Views           = f'IM{name}Views',
                                                            ViewNodeName    = f'{name}InView',
                                                            RequireParentView = True)
    precisionInDetReco = InViewRecoCA(name, viewMaker=evtViewMaker)

    from TrigInDetConfig.TrigInDetConfig import trigInDetPrecisionTrackingCfg
    idTracking = trigInDetPrecisionTrackingCfg(flags, signatureName='Electron')
    precisionInDetReco.mergeReco(idTracking)

    selAcc.mergeReco(precisionInDetReco)
    from TrigEgammaHypo.TrigEgammaPrecisionTrackingHypoTool import TrigEgammaPrecisionTrackingHypoToolFromDict
    hypoAlg = CompFactory.TrigEgammaPrecisionTrackingHypoAlg('ElectronprecisionTrackingHypo', CaloClusters='HLT_CaloEMClusters')
    selAcc.addHypoAlgo(hypoAlg)
    menuSequence = MenuSequenceCA(selAcc,
                                  HypoToolGen=TrigEgammaPrecisionTrackingHypoToolFromDict)
    return (selAcc , menuSequence)

def _precisionTracking(flags, chainDict):
    selAcc , menuSequence = _precisionTrackingSeq(flags)
    return ChainStep(name=selAcc.name, Sequences=[menuSequence], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))

@AccumulatorCache
def _precisionElectronSeq(flags, doIDperf=False):
    name='PrecionElectron'
    selAcc=SelectionCA(name)
    recoAcc = InViewRecoCA(name, RequireParentView=True)
    #TODO this should likely go elsewhere (egamma experts to decide)
    from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import TrigEgammaKeys
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloMenuSequences import precisionCaloMenuDefs
    recoAcc.addRecoAlgo(CompFactory.AthViews.ViewDataVerifier(name='VDV'+recoAcc.name,
                                                              DataObjects=[( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' ),
                                                                           ( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+%s' % precisionCaloMenuDefs.caloClusters(ion=False) ),
                                                                           ( 'xAOD::TrackParticleContainer','StoreGateSvc+%s' % TrigEgammaKeys.TrigElectronTracksCollectionName)] ) )


    def TrigEMTrackMatchBuilderToolCfg(flags, name='TrigEMTrackMatchBuilder_noGSF'):
        acc = ComponentAccumulator()

        from egammaTrackTools.egammaTrackToolsConfig import EMExtrapolationToolsCfg
        emExtrapolatorTools = recoAcc.popToolsAndMerge(EMExtrapolationToolsCfg(flags))
        builderTool = CompFactory.EMTrackMatchBuilder(  name, #TODO, this is provate tool, it does not need to be specifically named
                                                        TrackParticlesName = TrigEgammaKeys.TrigElectronTracksCollectionName,
                                                        ExtrapolationTool  = emExtrapolatorTools,
                                                        broadDeltaEta      = 0.1, #candidate match is done in 2 times this  so +- 0.2
                                                        broadDeltaPhi      = 0.15,  #candidate match is done in 2 times this  so +- 0.3
                                                        useCandidateMatch  = True,
                                                        useScoring         = True,
                                                        SecondPassRescale  = True,
                                                        UseRescaleMetric   = True,
                                                        isCosmics          = flags.Beam.Type=='cosmics')
        acc.setPrivateTools(builderTool)
        return acc

    def TrigEgammaRecElectronCfg(flags, name='TrigEgammaRecElectron_noGSF'):
        acc = ComponentAccumulator()
        electronRec = CompFactory.egammaRecBuilder( name,
                                                    InputClusterContainerName= 'HLT_CaloEMClusters',
                                                    egammaRecContainer= TrigEgammaKeys.EgammaRecKey,
                                                    doConversions = False,
                                                    TrackMatchBuilderTool = recoAcc.popToolsAndMerge(TrigEMTrackMatchBuilderToolCfg(flags)) )
        acc.addEventAlgo(electronRec)
        return acc

    recoAcc.mergeReco( TrigEgammaRecElectronCfg(flags) )
    

    def TrigElectronSuperClusterBuilderCfg(flags):
        acc = ComponentAccumulator()
        from egammaTools.egammaSwToolConfig import egammaSwToolCfg
        from egammaMVACalib.egammaMVACalibConfig import egammaMVASvcCfg
        superClusterBuilder = CompFactory.electronSuperClusterBuilder( 'TrigElectronSuperClusterBuilder_noGSF',
                                                                        InputEgammaRecContainerName = TrigEgammaKeys.EgammaRecKey,
                                                                        SuperElectronRecCollectionName = TrigEgammaKeys.SuperElectronRecCollectionName,
                                                                        ClusterCorrectionTool = recoAcc.popToolsAndMerge(egammaSwToolCfg(flags)),
                                                                        MVACalibSvc = recoAcc.getPrimaryAndMerge(egammaMVASvcCfg(flags)),
                                                                        EtThresholdCut = 1000,
                                                                        TrackMatchBuilderTool = recoAcc.popToolsAndMerge(TrigEMTrackMatchBuilderToolCfg(flags)) )
        acc.addEventAlgo(superClusterBuilder)
        return acc

    recoAcc.mergeReco(TrigElectronSuperClusterBuilderCfg(flags))

    def TrigEMClusterToolCfg(flags):
        acc = ComponentAccumulator()
        from egammaMVACalib.egammaMVACalibConfig import egammaMVASvcCfg
        tool = CompFactory.EMClusterTool('TrigEMClusterTool',
                                            OutputClusterContainerName = TrigEgammaKeys.TrigEMClusterToolOutputContainer,
                                            MVACalibSvc = acc.getPrimaryAndMerge(egammaMVASvcCfg(flags))
        )        
        acc.setPrivateTools(tool)
        return acc

    def TrigTopoEgammaElectronCfg(flags, name='topoEgammaBuilder_TrigElectrons'):
        acc = ComponentAccumulator()
        from egammaTools.EMShowerBuilderConfig import EMShowerBuilderCfg
        builder = CompFactory.topoEgammaBuilder(name,
                                                SuperElectronRecCollectionName = TrigEgammaKeys.SuperElectronRecCollectionName,
                                                SuperPhotonRecCollectionName = TrigEgammaKeys.SuperPhotonRecCollectionName,
                                                ElectronOutputName = TrigEgammaKeys.outputElectronKey,
                                                PhotonOutputName = TrigEgammaKeys.outputPhotonKey,  
                                                AmbiguityTool = CompFactory.EGammaAmbiguityTool(),
                                                EMClusterTool = acc.popToolsAndMerge(TrigEMClusterToolCfg(flags)),
                                                EMShowerTool = acc.popToolsAndMerge(EMShowerBuilderCfg(flags, CellsName="CaloCells")),
                                                egammaTools = [CompFactory.EMFourMomBuilder()], # TODO use list defined elsewhere
                                                doPhotons = False,
                                                doElectrons = True)
        acc.addEventAlgo(builder)
        return acc

    recoAcc.mergeReco(TrigTopoEgammaElectronCfg(flags))

    def TrigTrackIsolationToolCfg(flags, name='TrigTrackIsolationTool'):
        acc = ComponentAccumulator()

        tpicTool = CompFactory.xAOD.TrackParticlesInConeTool(TrackParticleLocation = TrigEgammaKeys.TrigElectronTracksCollectionName)

        tiTool = CompFactory.xAOD.TrackIsolationTool(name,
                                                    TrackParticleLocation = TrigEgammaKeys.TrigElectronTracksCollectionName,
                                                    VertexLocation = '',
                                                    TracksInConeTool  = tpicTool)
        # configure default TrackSelectionTool
        tiTool.TrackSelectionTool.maxZ0SinTheta = 3
        tiTool.TrackSelectionTool.minPt         = 1000
        tiTool.TrackSelectionTool.CutLevel      = "Loose"

        acc.setPrivateTools(tiTool)
        return acc

    def TrigElectronIsoBuilderCfg(flags, name='TrigElectronIsolationBuilder'):
        acc = ComponentAccumulator()
        from xAODPrimitives.xAODIso import xAODIso as isoPar
        builder = CompFactory.IsolationBuilder(
                                        name,
                                        ElectronCollectionContainerName = 'HLT_egamma_Electrons',
                                        CaloCellIsolationTool = None,
                                        CaloTopoIsolationTool = None,
                                        PFlowIsolationTool    = None,
                                        TrackIsolationTool    = acc.popToolsAndMerge(TrigTrackIsolationToolCfg(flags)),
                                        ElIsoTypes            = [[isoPar.ptcone20]],
                                        ElCorTypes            = [[isoPar.coreTrackPtr]],
                                        ElCorTypesExtra       = [[]])
        acc.addEventAlgo(builder)
        return acc

    recoAcc.mergeReco(TrigElectronIsoBuilderCfg(flags))

    selAcc.mergeReco(recoAcc)
    from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import TrigEgammaPrecisionElectronHypoToolFromDict, TrigEgammaPrecisionElectronHypoAlgCfg
    selAcc.mergeHypo( TrigEgammaPrecisionElectronHypoAlgCfg(flags, 'TrigEgammaHypoAlg_noGSF', TrigEgammaKeys.outputElectronKey, doIDperf=doIDperf ) )
    menuSequence = MenuSequenceCA(selAcc,
                                   HypoToolGen=TrigEgammaPrecisionElectronHypoToolFromDict)
    return (selAcc, menuSequence)

def _precisionElectron(flags, chainDict):
    selAcc , menuSequence = _precisionElectronSeq(flags)
    return ChainStep(name=selAcc.name, Sequences=[menuSequence], chainDicts=[chainDict], multiplicity=getChainMultFromDict(chainDict))


def generateChains(flags, chainDict):

    l1Thresholds=[]
    for part in chainDict['chainParts']:
        l1Thresholds.append(part['L1threshold'])

    emptyStep = ChainStep(name="EmptyElStep", Sequences=[EmptyMenuSequence("EmptyElStep")], chainDicts=[chainDict])
    # # # offline egamma
    steps = []
    if 'etcut' in chainDict['chainName']:
        steps = [ _fastCalo(flags, chainDict), 
                  _ftf(flags, chainDict),
                  _precisonCalo(flags, chainDict),
                  _precisionTracking(flags, chainDict), 
                  emptyStep]
    else:
        steps = [ _fastCalo(flags, chainDict),
                  _ftf(flags, chainDict),
                  _precisonCalo(flags, chainDict),
                  _precisionTracking(flags, chainDict), 
                  _precisionElectron(flags, chainDict)]

    chain = Chain(chainDict['chainName'], L1Thresholds=l1Thresholds,
                            ChainSteps=steps)

    return chain

if __name__ == '__main__':
    # run with: python -m TriggerMenuMT.HLTMenuConfig.Electron.generateElectron
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()
    from ..Menu.DictFromChainName import dictFromChainName
    chain = generateChains(ConfigFlags, dictFromChainName('HLT_e26_L1EM15'))
    for step in chain.steps:
        for s in step.sequences:
            if not isinstance(s, EmptyMenuSequence):
                s.ca.printConfig(withDetails=True, summariseProps=False) # flip the last arg to see all settings
                s.ca.wasMerged() # to silence check for orphanted CAs
