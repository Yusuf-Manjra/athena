# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
LArCellBuilderFromLArRawChannelTool, LArCellMerger, LArCellNoiseMaskingTool=CompFactory.getComps("LArCellBuilderFromLArRawChannelTool","LArCellMerger","LArCellNoiseMaskingTool",)
from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg 
from LArCalibUtils.LArHVScaleConfig import LArHVScaleCfg

def LArCellBuilderCfg(configFlags):
    result=ComponentAccumulator()
    result.merge(LArOnOffIdMappingCfg(configFlags))
    theLArCellBuilder = LArCellBuilderFromLArRawChannelTool()

    theLArCellBuilder.addDeadOTX = False #Create flag? Requires bad-feb DB access
    result.setPrivateTools(theLArCellBuilder)
    return result



def LArCellCorrectorCfg(configFlags):
    result=ComponentAccumulator()
    
    correctionTools=[]

    if configFlags.LAr.RawChannelSource=="both":
        theMerger=LArCellMerger(RawChannelsName="LArRawChannels_FromDigits")
        correctionTools.append(theMerger)
    
    if configFlags.LAr.doCellNoiseMasking or configFlags.LAr.doCellSporadicNoiseMasking:
        from LArBadChannelTool.LArBadChannelConfig import LArBadChannelMaskerCfg
        theNoiseMasker=LArCellNoiseMaskingTool()
        if configFlags.LAr.doCellNoiseMasking:
            acc= LArBadChannelMaskerCfg(configFlags,problemsToMask=["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys"],ToolName="CellNoiseMask")
            theNoiseMasker.MaskingTool=acc.popPrivateTools()
            result.merge(acc)

            pass
        if configFlags.LAr.doCellSporadicNoiseMasking:
            acc=LArBadChannelMaskerCfg(configFlags,problemsToMask=["sporadicBurstNoise",],ToolName="SporadicNoiseMask")
            theNoiseMasker.MaskingSporadicTool=acc.popPrivateTools()
            result.merge(acc)
            pass
        correctionTools.append(theNoiseMasker)

    result.setPrivateTools(correctionTools)
    return result    


def LArHVCellContCorrCfg(configFlags):
    acc=ComponentAccumulator()
    acc.merge(LArHVScaleCfg(configFlags)) #CondAlgo & co for HVScale Corr
    LArCellContHVCorrTool=CompFactory.LArCellContHVCorrTool
    theLArCellHVCorrTool = LArCellContHVCorrTool()
    acc.setPrivateTools(theLArCellHVCorrTool)
    return acc


