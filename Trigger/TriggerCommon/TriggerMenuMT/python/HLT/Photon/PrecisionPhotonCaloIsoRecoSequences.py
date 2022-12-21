#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import parOR
from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import getTrigEgammaKeys

#logging
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)



def precisionPhotonCaloIsoRecoSequence(RoIs, ion=False):
    """ With this function we will setup the sequence of Calo isolation to be executed after PrecisionPhoton in  TrigEgamma 

    """

    TrigEgammaKeys = getTrigEgammaKeys(ion=ion)

    log.debug('precisionPhotonCaloIsoRecoSequence(RoIs = %s)',RoIs)

    tag = '_ion' if ion is True else ''

    # First the data verifiers:
    # Here we define the data dependencies. What input needs to be available for the Fexs (i.e. TopoClusters from precisionCalo) in order to run
    import AthenaCommon.CfgMgr as CfgMgr

    caloClusters = TrigEgammaKeys.precisionPhotonCaloClusterContainer
    collectionIn = TrigEgammaKeys.precisionPhotonContainer # Input container with non isolated photons. These photons will be decorated with isolation variables (by running TrigPhotonIsoBuilderCfg[HI]). Monitoring of isolation variables is also executed over input photon container
    collectionOut = TrigEgammaKeys.precisionPhotonIsoContainer # Output container with isolated photons. The resulting decorated photons will be copied over to this new container by the HypoAlg. Hypo Tool and decision will be done over photons in this isolated output photon container 

    ViewVerify = CfgMgr.AthViews__ViewDataVerifier("PrecisionPhotonCaloIsoPhotonViewDataVerifier" + tag)
    ViewVerify.DataObjects = [( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+%s' % caloClusters ),
                              ( 'xAOD::CaloClusterContainer' , 'StoreGateSvc+%s' % TrigEgammaKeys.precisionTopoClusterContainer), # this is for the calo isolation tool 
                              ( 'xAOD::PhotonContainer' , 'StoreGateSvc+%s' % TrigEgammaKeys.precisionPhotonContainer), # This is the Photon input container with non-isolated photons
                              ( 'CaloCellContainer' , 'StoreGateSvc+CaloCells' ),
                              ( 'CaloCellContainer' , 'StoreGateSvc+CaloCellsFS' ),
                              ( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ),
                              ( 'xAOD::EventShape' , 'StoreGateSvc+TrigIsoEventShape' ),
                              ( 'xAOD::IParticleContainer' , 'StoreGateSvc+HLT_TopoCaloClustersFS'),
                              ( 'PseudoJetContainer' , 'StoreGateSvc+PseudoJetTrigEMTopo' )]
                              
    if ion is True: ViewVerify.DataObjects.append(( 'CaloCellContainer' , 'StoreGateSvc+CorrectedRoICaloCells' ))

    log.debug('retrieve(precisionPhotonCaloIsoRecoSequence,None,RoIs = %s)',RoIs)


    # The sequence of these algorithms
    thesequence = parOR( "precisionPhotonCaloIsoAlgs" + tag) # This thing creates the sequence with name precisionPhotonCaloIsoAlgs
    thesequence += ViewVerify

    # Add CaloIsolationTool
    from TriggerMenuMT.HLT.Egamma.TrigEgammaFactories import TrigPhotonIsoBuilderCfg, TrigPhotonIsoBuilderHICfg #, egammaFSEventDensitySequence

    if ion:
        thesequence += TrigPhotonIsoBuilderHICfg('TrigPhotonIsolationBuilderHI' + tag)
    else:
        #thesequence += egammaFSEventDensitySequence() 
        thesequence += TrigPhotonIsoBuilderCfg('TrigPhotonIsolationBuilder')

    #online monitoring for topoEgammaBuilder
    from TriggerMenuMT.HLT.Photon.TrigPhotonFactories import PrecisionPhotonCaloIsoMonitorCfg
    PrecisionPhotonCaloIsoRecoMonAlgo = PrecisionPhotonCaloIsoMonitorCfg('PrecisionPhotonCaloIsoEgammaBuilderMon' + tag)
    PrecisionPhotonCaloIsoRecoMonAlgo.PhotonKey = TrigEgammaKeys.precisionPhotonContainer 
    PrecisionPhotonCaloIsoRecoMonAlgo.IsoVarKeys = [ '%s.topoetcone20' % collectionIn, '%s.topoetcone40' % collectionIn]
    thesequence += PrecisionPhotonCaloIsoRecoMonAlgo


    return (thesequence, collectionOut)





