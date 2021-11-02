#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.CFElements import parOR

#logging
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

def precisionElectronRecoSequence(RoIs, ion=False, variant=''):
    """ With this function we will setup the sequence of offline EgammaAlgorithms so to make a electron for TrigEgamma 

    Sequence of algorithms is the following:
      - egammaRecBuilder/TrigEgammaRecElectron creates egammaObjects out of clusters and tracks. 
      - electronSuperClusterBuilder algorithm will create superclusters out of the topoclusters and tracks in egammaRec under the electron hypothesis
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/egammaSuperClusterBuilder.py#L26 
      - TopoEgammBuilder will create photons and electrons out of trakcs and SuperClusters. Here at HLT electrons the aim is to ignore photons.
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/src/topoEgammaBuilder.cxx
    """

    log.debug('precisionElectronRecoSequence(RoIs = %s, variant = %s)',(RoIs,variant))

    tag = '_ion' if ion is True else ''
    
    import AthenaCommon.CfgMgr as CfgMgr
    # First the data verifiers:
    # Here we define the data dependencies. What input needs to be available for the Fexs (i.e. TopoClusters from precisionCalo) in order to run
    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloMenuSequences import precisionCaloMenuDefs
       
    # precision Tracking related data dependencies
    from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaKeys import  getTrigEgammaKeys

    TrigEgammaKeys = getTrigEgammaKeys(variant)

    caloClusters = precisionCaloMenuDefs.caloClusters(ion)
    trackParticles = TrigEgammaKeys.TrigElectronTracksCollectionName

    ViewVerifyTrk   = CfgMgr.AthViews__ViewDataVerifier("PrecisionTrackViewDataVerifier" + tag)

    ViewVerifyTrk.DataObjects = [( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' ),
                                 ( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+%s' % caloClusters ),
                                 ( 'xAOD::TrackParticleContainer','StoreGateSvc+%s' % trackParticles)]


    """ Retrieve the factories now """
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigElectronFactories import TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectronCfg
    from TriggerMenuMT.HLTMenuConfig.Egamma.TrigEgammaFactories import  TrigEMTrackMatchBuilder, TrigElectronIsoBuilderCfg

    # Create the sequence of three steps:
    #  - TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectron

    # Create the sequence of three steps:
    #  - TrigEgammaRecElectron, TrigElectronSuperClusterBuilder, TrigTopoEgammaElectron
    #The sequence of these algorithms
    thesequence = parOR( "precisionElectron" + RoIs + tag)
    thesequence += ViewVerifyTrk
    
    ## TrigEMTrackMatchBuilder_noGSF ##
    TrigEMTrackMatchBuilder = TrigEMTrackMatchBuilder("TrigEMTrackMatchBuilder_noGSF" + tag)
    TrigEMTrackMatchBuilder.TrackParticlesName = trackParticles

    ## TrigEgammaRecElectron_noGSF ##
    TrigEgammaRecAlgo = TrigEgammaRecElectron("TrigEgammaRecElectron_noGSF" + tag)
    thesequence += TrigEgammaRecAlgo
    TrigEgammaRecAlgo.TrackMatchBuilderTool = TrigEMTrackMatchBuilder
    TrigEgammaRecAlgo.InputClusterContainerName = caloClusters

    ## TrigElectronSuperClusterBuilder_noGSF ##
    TrigSuperElectronAlgo = TrigElectronSuperClusterBuilder("TrigElectronSuperClusterBuilder_noGSF" + tag)
    thesequence += TrigSuperElectronAlgo
    TrigSuperElectronAlgo.InputEgammaRecContainerName =  TrigEgammaRecAlgo.egammaRecContainer
    TrigSuperElectronAlgo.TrackMatchBuilderTool = TrigEMTrackMatchBuilder

    ## TrigTopoEgammaElectronCfg_noGSF ##
    TrigTopoEgammaAlgo = TrigTopoEgammaElectronCfg("TrigTopoEgammaElectronCfg_noGSF" + tag)
    thesequence += TrigTopoEgammaAlgo
    TrigTopoEgammaAlgo.InputElectronRecCollectionName = TrigSuperElectronAlgo.SuperElectronRecCollectionName
    collectionOut = TrigTopoEgammaAlgo.ElectronOutputName
    TrigTopoEgammaAlgo.DummyElectronOutputName = "HLT_PrecisionDummyElectron"
    collectionOut_dummy = TrigTopoEgammaAlgo.DummyElectronOutputName
    ## TrigElectronIsoBuilderCfg_noGSF ##
    isoBuilder = TrigElectronIsoBuilderCfg("TrigElectronIsoBuilderCfg_noGSF" + tag)
    isoBuilder.useBremAssoc = False
    thesequence += isoBuilder

    #online monitoring for topoEgammaBuilder
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigElectronFactories import PrecisionElectronTopoMonitorCfg
    PrecisionElectronRecoMonAlgo = PrecisionElectronTopoMonitorCfg()
    PrecisionElectronRecoMonAlgo.ElectronKey = TrigTopoEgammaAlgo.ElectronOutputName
    PrecisionElectronRecoMonAlgo.IsoVarKeys = [ '%s.ptcone20' % TrigTopoEgammaAlgo.ElectronOutputName, '%s.ptvarcone20' % TrigTopoEgammaAlgo.ElectronOutputName]
    thesequence += PrecisionElectronRecoMonAlgo

    #online monitoring for TrigElectronSuperClusterBuilder
    from TriggerMenuMT.HLTMenuConfig.Electron.TrigElectronFactories import PrecisionElectronSuperClusterMonitorCfg
    PrecisionElectronSuperClusterMonAlgo = PrecisionElectronSuperClusterMonitorCfg()
    PrecisionElectronSuperClusterMonAlgo.InputEgammaRecContainerName = TrigSuperElectronAlgo.SuperElectronRecCollectionName
    thesequence += PrecisionElectronSuperClusterMonAlgo
    return (thesequence, collectionOut, collectionOut_dummy)

