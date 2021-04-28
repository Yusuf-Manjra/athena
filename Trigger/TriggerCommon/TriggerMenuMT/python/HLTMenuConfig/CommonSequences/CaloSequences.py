#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool, MenuSequence
from AthenaCommon.CFElements import seqAND, parOR
from TrigEDMConfig.TriggerEDMRun3 import recordable
from .FullScanDefs import caloFSRoI

class CaloMenuDefs(object):
      """Static Class to collect all string manipulations in Calo sequences """
      from TrigEDMConfig.TriggerEDMRun3 import recordable
      L2CaloClusters= recordable("HLT_FastCaloEMClusters")


#
# central or forward fast calo sequence 
#
def fastCaloSequence(flags, doForward=False, name="fastCaloSequence"):
    """ Creates Fast Calo reco sequence"""
    if doForward:
        from TrigT2CaloCommon.CaloDef import fastCaloEVFWDCreator as theEVCreator
        from TrigT2CaloCommon.CaloDef import fastCaloRecoFWDSequence as theRecoSequence
    else:
        from TrigT2CaloCommon.CaloDef import fastCaloEVCreator as theEVCreator
        from TrigT2CaloCommon.CaloDef import fastCaloRecoSequence as theRecoSequence

    (fastCaloViewsMaker, InViewRoIs) = theEVCreator()
    # reco sequence always build the rings
    (fastCaloInViewSequence, sequenceOut) = theRecoSequence(InViewRoIs, doRinger=True)
     # connect EVC and reco
    fastCaloSequence = seqAND(name, [fastCaloViewsMaker, fastCaloInViewSequence ])
    return (fastCaloSequence, fastCaloViewsMaker, sequenceOut)


#
# Forward fast calo sequence only
#
def fastCaloFWDSequence(flags):
    return fastCaloSequence(flags, doForward=True, name="fastCaloFWDSequence")




def fastCaloMenuSequence(name, doRinger=True):
    """ Creates Egamma Fast Calo  MENU sequence
    The Hypo name changes depending on name, so for different implementations (Electron, Gamma,....)
    """
    (sequence, fastCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(fastCaloSequence, flags=None)
    # check if use Ringer and are electron because there aren't ringer for photons yet:

    # hypo
    if doRinger:
      from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import createTrigEgammaFastCaloHypoAlgMT
    else:
      from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import createTrigEgammaFastCaloHypoAlgMT_noringer as createTrigEgammaFastCaloHypoAlgMT


    theFastCaloHypo = createTrigEgammaFastCaloHypoAlgMT(name+"EgammaFastCaloHypo", sequenceOut)
    CaloMenuDefs.L2CaloClusters = sequenceOut

    from TrigEgammaHypo.TrigEgammaFastCaloHypoTool import TrigEgammaFastCaloHypoToolFromDict
    return MenuSequence( Sequence    = sequence,
                         Maker       = fastCaloViewsMaker,
                         Hypo        = theFastCaloHypo,
                         HypoToolGen = TrigEgammaFastCaloHypoToolFromDict )


#
# Create e/g fast calo menu sequence for central or forward region.
#
def fastCaloFWDMenuSequence(name, doRinger=True):
    """ Creates Egamma Fast Calo FWD MENU sequence (Reco and Hypo)
    The Hypo name changes depending on name, so for different implementations (Electron, Gamma,....)
    """

    (sequence, fastCaloViewsMaker, sequenceOut) = RecoFragmentsPool.retrieve(fastCaloFWDSequence, flags=None)
    
    # hypo
    from TrigEgammaForwardHypo.TrigEgammaForwardHypoConf import TrigEgammaForwardFastCaloHypoAlg
    from TrigEgammaForwardHypo.TrigEgammaForwardFastCaloHypoTool import TrigEgammaForwardFastCaloHypoToolFromDict
    theFastCaloHypo = TrigEgammaForwardFastCaloHypoAlg(name+"EgammaFastCaloFWDHypo")
    theFastCaloHypo.CaloClusters = sequenceOut

    from TrigEgammaForwardHypo.TrigEgammaForwardFastCaloHypoTool import createTrigEgammaForwardFastCaloHypoAlg
   
    theFastCaloHypo = createTrigEgammaForwardFastCaloHypoAlg(name+"EgammaFastCaloFWDHypo", sequenceOut)
    CaloMenuDefs.L2CaloClusters = sequenceOut

    return MenuSequence( Sequence    = sequence,
                         Maker       = fastCaloViewsMaker,
                         Hypo        = theFastCaloHypo,
                         HypoToolGen = TrigEgammaForwardFastCaloHypoToolFromDict )




def cellRecoSequence(flags, name="HLTCaloCellMakerFS", RoIs=caloFSRoI, outputName="CaloCellsFS"):
    """ Produce the full scan cell collection """
    if not RoIs:
        from L1Decoder.L1DecoderConfig import mapThresholdToL1RoICollection
        RoIs = mapThresholdToL1RoICollection("FSNOSEED")
    from TrigT2CaloCommon.CaloDef import setMinimalCaloSetup
    setMinimalCaloSetup()
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker
    alg = HLTCaloCellMaker(name)
    alg.RoIs=RoIs
    alg.TrigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
    alg.CellsName=outputName
    return parOR(name+"RecoSequence", [alg]), str(alg.CellsName)

def caloClusterRecoSequence(
        flags, name="HLTCaloClusterMakerFS", RoIs=caloFSRoI,
        outputName="HLT_TopoCaloClustersFS"):
    """ Create the EM-level fullscan clusters """
    cell_sequence, cells_name = RecoFragmentsPool.retrieve(cellRecoSequence, flags=None, RoIs=RoIs)
    from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMakerMT_topo
    alg = TrigCaloClusterMakerMT_topo(
            name,
            doMoments=True,
            doLC=False,
            cells=cells_name)
    alg.CaloClusters = recordable(outputName)
    return parOR(name+"RecoSequence", [cell_sequence, alg]), str(alg.CaloClusters)

def LCCaloClusterRecoSequence(
        flags, name="HLTCaloClusterCalibratorLCFS", RoIs=caloFSRoI,
        outputName="HLT_TopoCaloClustersLCFS"):
    """ Create the LC calibrated fullscan clusters

    The clusters will be created as a shallow copy of the EM level clusters
    """
    em_sequence, em_clusters = RecoFragmentsPool.retrieve(caloClusterRecoSequence, flags=None, RoIs=RoIs)
    from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterCalibratorMT_LC
    alg = TrigCaloClusterCalibratorMT_LC(
            name,
            InputClusters = em_clusters,
            OutputClusters = outputName,
            OutputCellLinks = outputName+"_cellLinks")
    return parOR(name+"RecoSequence", [em_sequence, alg]), str(alg.OutputClusters)
