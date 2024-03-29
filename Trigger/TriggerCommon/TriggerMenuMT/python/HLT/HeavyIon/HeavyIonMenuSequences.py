# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLT.Config.MenuComponents import MenuSequence
from AthenaCommon.CFElements import seqAND

def HIFwdGapMenuSequence(flags):

  from TriggerMenuMT.HLT.Egamma.TrigEgammaFactories import egammaFSCaloRecoSequence
  egammaFSRecoSequence = egammaFSCaloRecoSequence(flags)

  from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
  HIFwdGapInputMakerAlg = InputMakerForRoI('IM_HIFwdGap',
                                           RoIsLink = 'initialRoI',
                                           RoITool = ViewCreatorInitialROITool(),
                                           RoIs='HIFwdGapRoI', # not used in fact
                                           )

  theSequence = seqAND('HIFwdGapSequence', [egammaFSRecoSequence, HIFwdGapInputMakerAlg])

  from TrigHIHypo.TrigHIHypoConf import TrigHIFwdGapHypoAlg
  theHIFwdGapHypo = TrigHIFwdGapHypoAlg()

  from TrigHIHypo.TrigHIFwdGapHypoConfig import TrigHIFwdGapHypoToolFromDict

  return MenuSequence(flags,
                      Sequence    = theSequence,
                      Maker       = HIFwdGapInputMakerAlg,
                      Hypo        = theHIFwdGapHypo,
                      HypoToolGen = TrigHIFwdGapHypoToolFromDict)


def egammaFSHIEventShapeMakerCfg(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    from TrigCaloRec.TrigCaloRecConfig import hltCaloCellMakerCfg
    from HLTSeeding.HLTSeedingConfig import mapThresholdToL1RoICollection
    from TriggerMenuMT.HLT.Egamma.TrigEgammaKeys import getTrigEgammaKeys

    acc = ComponentAccumulator()    
    cellMakerAcc = hltCaloCellMakerCfg(flags, 'HLTCaloCellMakerEGFS', 
        roisKey = mapThresholdToL1RoICollection('FSNOSEED'), 
        CellsName = 'CaloCellsEGFS', monitorCells=False)

    acc.merge(cellMakerAcc)
    # configure HI Event Shape by hand,
    # in future need to reuse one from HIGlobal.HIGlobalConfig (will need more args & dependency on flags)
    eventShapeFiller = CompFactory.HIEventShapeFillerTool( 
                EventShapeMapTool = CompFactory.HIEventShapeMapTool())

    eventShapeMakerAlg = CompFactory.HIEventShapeMaker('HLTEventShapeMakerEG',
        HIEventShapeFillerTool = eventShapeFiller,
        InputCellKey=cellMakerAcc.getPrimary().CellsName,
        OutputContainerKey=getTrigEgammaKeys(ion=True).egEventShape) 

    acc.addEventAlgo(eventShapeMakerAlg)
    return acc



def HIFwdGapMenuSequenceCfg(flags):
  from ..Config.MenuComponents import InEventRecoCA, SelectionCA, MenuSequenceCA
  from AthenaConfiguration.ComponentFactory import CompFactory
  from TrigHIHypo.TrigHIFwdGapHypoConfig import TrigHIFwdGapHypoToolFromDict

  recoAcc = InEventRecoCA("HIFwdGapReco")
  recoAcc.mergeReco(egammaFSHIEventShapeMakerCfg(flags))

  selAcc = SelectionCA("HLFwdGapSel")
  selAcc.mergeReco(recoAcc)
  selAcc.addHypoAlgo(CompFactory.TrigHIFwdGapHypoAlg())

  return MenuSequenceCA(flags, selAcc, HypoToolGen = TrigHIFwdGapHypoToolFromDict)
