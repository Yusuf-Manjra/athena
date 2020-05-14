#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaCommon.CFElements import parOR

#logging
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Photon.PhotonRecoSequences' )  

def precisionPhotonRecoSequence(RoIs):
    """ With this function we will setup the sequence of offline EgammaAlgorithms so to make a photon for TrigEgamma 

    Sequence of algorithms is the following:
      - egammaRecBuilder/TrigEgammaRecPhoton creates egammaObjects out of clusters and tracks. Here, at HLT photons we will only use clusters. 
      - photonSuperClusterBuilder algorithm will create superclusters out of the toposlusters and tracks in egammaRec under the photon hypothesis
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/egammaSuperClusterBuilder.py#L26 
      - TopoEgammBuilder will create photons and electrons out of trakcs and SuperClusters. Here at HLT photons the aim is to ignore electrons and not use tracks at all.
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/src/topoEgammaBuilder.cxx
    """


    log.debug('precisionPhotonRecoSequence(RoIs = %s)',RoIs)

    # First the data verifiers:
    # Here we define the data dependencies. What input needs to be available for the Fexs (i.e. TopoClusters from precisionCalo) in order to run
    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuDefs
    import AthenaCommon.CfgMgr as CfgMgr
    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("PrecisionPhotonPhotonViewDataVerifier")
    ViewVerify.DataObjects = [( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+' + precisionCaloMenuDefs.precisionCaloClusters ),
                              ( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' ),
                              ( 'xAOD::VertexContainer' , 'StoreGateSvc+GSFConversionVertices' ),
                              ( 'CaloAffectedRegionInfoVec' , 'ConditionStore+LArAffectedRegionInfo' )]

    # Make sure the required objects are still available at whole-event level
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence.SGInputLoader.Load += [( 'xAOD::VertexContainer' , 'StoreGateSvc+GSFConversionVertices' ),
                                       ( 'CaloAffectedRegionInfoVec' , 'ConditionStore+LArAffectedRegionInfo' )]


    # Retrieve the factories now
    from TriggerMenuMT.HLTMenuConfig.Photon.TrigPhotonFactories import TrigEgammaRecPhoton, TrigPhotonSuperClusterBuilder, TrigTopoEgammaPhotons








    log.debug('retrieve(precisionPhotonRecoSequence,None,RoIs = %s)',RoIs)

    # The sequence of these algorithms
    thesequence = parOR( "precisionPhotonAlgs") # This thing creates the sequence with name precisionPhotonAlgs
    thesequence += ViewVerify

    # Add to the sequence the three steps:
    #  - TrigEgammaBuilder, TrigPhotonSuperClusters, TrigTopoEgammaPhotons
    TrigEgammaAlgo = TrigEgammaRecPhoton()
    TrigEgammaAlgo.InputTopoClusterContainerName = precisionCaloMenuDefs.precisionCaloClusters
    thesequence += TrigEgammaAlgo

    trigPhotonAlgo = TrigPhotonSuperClusterBuilder()
    trigPhotonAlgo.InputEgammaRecContainerName = TrigEgammaAlgo.egammaRecContainer
    thesequence += trigPhotonAlgo

    trigTopoEgammaAlgo = TrigTopoEgammaPhotons()
    trigTopoEgammaAlgo.SuperPhotonRecCollectionName = trigPhotonAlgo.SuperPhotonRecCollectionName
    collectionOut = trigTopoEgammaAlgo.PhotonOutputName
    thesequence += trigTopoEgammaAlgo

    return (thesequence, collectionOut)








def l2PhotonAlgCfg( flags ):
    acc = ComponentAccumulator()
    #from TrigEgammaHypo.TrigL2PhotonFexMTConfig import L2PhotonFex_1
    from AthenaConfiguration.ComponentFactory import CompFactory

    photonFex= CompFactory.TrigL2PhotonFexMT("L2PhotonFex_1")
    #photonFex= L2PhotonFex_1()
    photonFex.TrigEMClusterName = recordable("HLT_L2CaloEMClusters")
    photonFex.PhotonsName = recordable("HLT_L2Photons")
    photonFex.RoIs = "L2PhotonRecoRoIs"

    return acc, photonFex

def l2PhotonRecoCfg( flags ):
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco

    reco = InViewReco("L2PhotonReco")
    reco.inputMaker().RequireParentView = True
    reco.inputMaker().RoIsLink="initialRoI"

    from AthenaConfiguration.ComponentFactory import CompFactory
    moveClusters = CompFactory.getComp("AthViews::ViewDataVerifier")("photonViewDataVerifier")
    moveClusters.DataObjects = [ ('xAOD::TrigEMClusterContainer','StoreGateSvc+HLT_L2CaloEMClusters') ]

    reco.addRecoAlg( moveClusters )

    algAcc, alg = l2PhotonAlgCfg( flags )
    reco.addRecoAlg( alg )
    reco.merge( algAcc )

    return reco

def l2PhotonHypoCfg( flags, Photons='Unspecified', RunInView=True):
    from AthenaConfiguration.ComponentFactory import CompFactory

    l2PhotonHypo = CompFactory.TrigL2PhotonHypoAlgMT()
    l2PhotonHypo.Photons = Photons
    l2PhotonHypo.RunInView = RunInView

    return l2PhotonHypo


def generatePhotonsCfg( flags ):

    acc = ComponentAccumulator()
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, ChainStep, Chain, RecoFragmentsPool

    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromDict
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    l2CaloHypo              = TrigL2CaloHypoAlgMT("L2PhotonCaloHypo")
    l2CaloHypo.CaloClusters = 'L2CaloEMClusters'


    from TriggerMenuMT.HLTMenuConfig.Electron.ElectronRecoSequences import l2CaloRecoCfg
    
    
    l2CaloReco = RecoFragmentsPool.retrieve( l2CaloRecoCfg, flags )
    acc.merge( l2CaloReco )
    
    fastCaloSequence = MenuSequence( Sequence    = l2CaloReco.sequence(),
                                     Maker       = l2CaloReco.inputMaker(),
                                     Hypo        = l2CaloHypo,
                                     HypoToolGen = TrigL2CaloHypoToolFromDict )

    fastCaloStep = ChainStep( "Photon_step1", [fastCaloSequence] )



    l2PhotonReco = RecoFragmentsPool.retrieve( l2PhotonRecoCfg, flags )
    acc.merge( l2PhotonReco )
    
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypoAlgMT
    l2PhotonHypo = TrigL2PhotonHypoAlgMT()
    l2PhotonHypo.Photons = "L2Photons"
    l2PhotonHypo.RunInView=True

    from TrigEgammaHypo.TrigL2PhotonHypoTool import TrigL2PhotonHypoToolFromDict

    l2PhotonSequence = MenuSequence( Sequence    = l2PhotonReco.sequence(),
                                     Maker       = l2PhotonReco.inputMaker(),
                                     Hypo        = l2PhotonHypo,
                                     HypoToolGen = TrigL2PhotonHypoToolFromDict )

    l2PhotonStep = ChainStep( "Photon_step2", [ l2PhotonSequence] )
    


    chains = [ Chain(c.split()[0], c.split()[1], [fastCaloStep, l2PhotonStep] )  for c in flags.Trigger.menu.photons ]    


    

    return acc, chains


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    ConfigFlags.Trigger.menu.photons = ["HLT_g5_etcut L1_EM3", "HLT_g10_etcut L1_EM3"]
    ConfigFlags.lock()

    acc, sequences = generatePhotonsCfg( ConfigFlags )
    acc.printConfig()
