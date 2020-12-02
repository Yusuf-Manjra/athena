#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR

#logging
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionElectronRecoSequences_GSF')

def precisionElectronRecoSequence_GSF(RoIs):
    """
    Similar reconstruction sequence as ../PrecisionElectronRecoSequences.py, tailored to run GSF
    """

    log.debug('precisionElectronRecoSequence_GSF(RoIs = %s)',RoIs)
    
    import AthenaCommon.CfgMgr as CfgMgr
    # First the data verifiers:
    # Here we define the data dependencies. What input needs to be available for the Fexs (i.e. TopoClusters from precisionCalo) in order to run
    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuDefs
       
    # precision Tracking related data dependencies
    from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import TrigEgammaKeys, TrigEgammaKeys_GSF

    ViewVerifyTrk_GSF   = CfgMgr.AthViews__ViewDataVerifier("PrecisionTrackViewDataVerifier_GSF")

    ViewVerifyTrk_GSF.DataObjects = [( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' ),
                                 ( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+%s' % precisionCaloMenuDefs.precisionCaloClusters ),
                                 ( 'xAOD::TrackParticleContainer','StoreGateSvc+%s' % TrigEgammaKeys.TrigElectronTracksCollectionName),
                                 # verifier object needed by GSF
                                 ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.averageInteractionsPerCrossing' ), 
                                 ( 'TRT_RDO_Container' , 'StoreGateSvc+TRT_RDOs' ),
                                 ( 'IDCInDetBSErrContainer' , 'StoreGateSvc+PixelByteStreamErrs' ),
                                 ( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+PixelClusterAmbiguitiesMap' ),
                                 ( 'InDet::PixelGangedClusterAmbiguities' , 'StoreGateSvc+SplitClusterAmbiguityMap' ),
                                 ( 'InDet::TRT_DriftCircleContainer' , 'StoreGateSvc+TRT_DriftCircles' ),
                                 ( 'SG::AuxElement' , 'StoreGateSvc+EventInfo.AveIntPerXDecor' )]


    """ Retrieve the factories now """
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigElectronFactories import TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectronCfg
    from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import  TrigEMTrackMatchBuilder, TrigElectronIsoBuilderCfg
   
    # Create the sequence of three steps:
    #  - TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectron

        
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigEMBremCollectionBuilder import TrigEMBremCollectionBuilder
    # Create the sequence of steps:
    #  - TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectron
    #The sequence of these algorithms
    thesequence_GSF = parOR( "precisionElectron_GSF%s" % RoIs)
    thesequence_GSF += ViewVerifyTrk_GSF
    
    ## TrigEMBremCollectionBuilder ##
    track_GSF = TrigEMBremCollectionBuilder()
    thesequence_GSF += track_GSF

    ## TrigEMTrackMatchBuilder_GSF ##
    TrigEMTrackMatchBuilder_GSF = TrigEMTrackMatchBuilder("TrigEMTrackMatchBuilder_GSF")
    TrigEMTrackMatchBuilder_GSF.TrackParticlesName = track_GSF.OutputTrkPartContainerName
    
    ## TrigEgammaRecElectron_GSF ##
    TrigEgammaRecAlgo_GSF = TrigEgammaRecElectron("TrigEgammaRecElectron_GSF")
    thesequence_GSF += TrigEgammaRecAlgo_GSF
    TrigEgammaRecAlgo_GSF.TrackMatchBuilderTool = TrigEMTrackMatchBuilder_GSF
    TrigEgammaRecAlgo_GSF.InputTopoClusterContainerName = precisionCaloMenuDefs.precisionCaloClusters

    ## TrigElectronSuperClusterBuilder_GSF ##
    TrigSuperElectronAlgo_GSF = TrigElectronSuperClusterBuilder("TrigElectronSuperClusterBuilder_GSF")
    thesequence_GSF += TrigSuperElectronAlgo_GSF
    TrigSuperElectronAlgo_GSF.TrackMatchBuilderTool = TrigEMTrackMatchBuilder_GSF

    ## TrigTopoEgammaElectronCfg_GSF ##
    TrigTopoEgammaAlgo_GSF = TrigTopoEgammaElectronCfg("TrigTopoEgammaElectronCfg_GSF") 
    thesequence_GSF += TrigTopoEgammaAlgo_GSF
    TrigTopoEgammaAlgo_GSF.ElectronOutputName = TrigEgammaKeys_GSF.outputElectronKey_GSF
    collectionOut_GSF = TrigTopoEgammaAlgo_GSF.ElectronOutputName

    ## TrigElectronIsoBuilderCfg_GSF ##
    isoBuilder_GSF = TrigElectronIsoBuilderCfg("TrigElectronIsoBuilderCfg_GSF")
    thesequence_GSF += isoBuilder_GSF
    isoBuilder_GSF.ElectronCollectionContainerName = TrigEgammaKeys_GSF.outputElectronKey_GSF
    return (thesequence_GSF, collectionOut_GSF)
