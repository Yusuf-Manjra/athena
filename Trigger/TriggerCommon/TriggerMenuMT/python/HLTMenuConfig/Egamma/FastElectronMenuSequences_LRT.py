#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.AllConfigFlags import ConfigFlags 

# menu components   
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, RecoFragmentsPool
from AthenaCommon.CFElements import parOR, seqAND
from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorCentredOnClusterROITool
from TrigEDMConfig.TriggerEDMRun3 import recordable

def fastElectronSequence_LRT(ConfigFlags):
    """ second step:  tracking....."""
    
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    IDTrigConfig = getInDetTrigConfig( "electronLRT" )   

    from TrigInDetConfig.InDetSetup import makeInDetAlgs
    RoIs = "EMIDRoIs_LRT" # contract with the fastCalo
    viewAlgs, viewVerify = makeInDetAlgs( config = IDTrigConfig, rois = RoIs )

    # A simple algorithm to confirm that data has been inherited from parent view
    # Required to satisfy data dependencies
    from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequences import CaloMenuDefs  
    viewVerify.DataObjects += [( 'xAOD::TrigEMClusterContainer' , 'StoreGateSvc+%s' % CaloMenuDefs.L2CaloClusters ),
                               ( 'TrigRoiDescriptorCollection' , 'StoreGateSvc+%s' % RoIs )]

    TrackParticlesName = ""
    for viewAlg in viewAlgs:
        if "InDetTrigTrackParticleCreatorAlg" in viewAlg.name():
            TrackParticlesName = viewAlg.TrackParticlesName
      

    from TrigEgammaFastRec.TrigEgammaFastElectronConfig import TrigEgammaFastElectron_ReFastAlgo_Clean
    theElectronFex = TrigEgammaFastElectron_ReFastAlgo_Clean("EgammaFastElectronFex_Clean_LRT")
    theElectronFex.TrigEMClusterName = CaloMenuDefs.L2CaloClusters
    theElectronFex.TrackParticlesName = TrackParticlesName
    theElectronFex.ElectronsName=recordable("HLT_FastElectrons_LRT")
    theElectronFex.DummyElectronsName= "HLT_FastDummyElectrons"
 
    # EVCreator:
    l2ElectronViewsMaker = EventViewCreatorAlgorithm("IMl2Electron_LRT")
    l2ElectronViewsMaker.RoIsLink = "initialRoI" # Merge inputs based on their initial L1 ROI
    # Spawn View on SuperRoI encompassing all clusters found within the L1 RoI
    roiTool = ViewCreatorCentredOnClusterROITool()
    roiTool.AllowMultipleClusters = False # If True: SuperROI mode. If False: highest eT cluster in the L1 ROI
    roiTool.RoisWriteHandleKey = recordable("HLT_Roi_FastElectron_LRT")
    roiTool.RoIEtaWidth = 0.1
    roiTool.RoIPhiWidth = 0.40
    l2ElectronViewsMaker.RoITool = roiTool
    l2ElectronViewsMaker.InViewRoIs = RoIs
    l2ElectronViewsMaker.Views = "EMElectronViews_LRT"
    l2ElectronViewsMaker.ViewFallThrough = True
    l2ElectronViewsMaker.RequireParentView = True

    theElectronFex.RoIs = l2ElectronViewsMaker.InViewRoIs
    electronInViewAlgs = parOR("electronInViewAlgs_LRT", viewAlgs + [ theElectronFex ])
    l2ElectronViewsMaker.ViewNodeName = "electronInViewAlgs_LRT"

    electronAthSequence = seqAND("electronAthSequence_LRT", [l2ElectronViewsMaker, electronInViewAlgs ] )
    return (electronAthSequence, l2ElectronViewsMaker)
   
def fastElectronMenuSequence_LRT(do_idperf):
    """ Creates 2nd step Electron  MENU sequence"""
    # retrieve the reco sequence+IM
    (electronAthSequence, l2ElectronViewsMaker) = RecoFragmentsPool.retrieve(fastElectronSequence_LRT, ConfigFlags)

    # make the Hypo
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaFastElectronHypoAlgMT
    if do_idperf is True:
        theElectronHypo = TrigEgammaFastElectronHypoAlgMT("TrigEgammaFastElectronHypoAlgMT_idperf_LRT")
        theElectronHypo.Electrons = "HLT_FastDummyElectrons"
    else:
        theElectronHypo = TrigEgammaFastElectronHypoAlgMT("TrigEgammaFastElectronHypoAlgMT_LRT")
        theElectronHypo.Electrons = "HLT_FastElectrons_LRT"

    theElectronHypo.RunInView=True

    from TrigEgammaHypo.TrigEgammaFastElectronHypoTool import TrigEgammaFastElectronHypoToolFromDict

    return  MenuSequence( Maker       = l2ElectronViewsMaker,                                        
                          Sequence    = electronAthSequence,
                          Hypo        = theElectronHypo,
                          HypoToolGen = TrigEgammaFastElectronHypoToolFromDict )

