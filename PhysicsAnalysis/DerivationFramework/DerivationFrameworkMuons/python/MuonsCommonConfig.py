# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# MuonsCommonConfig.py 
# Configures all tools needed for muon object selection and kernels
# used to write results into SG. 
# ComponentAccumulator version
#********************************************************************

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def MuonsCommonCfg(flags, suff=""):
    """Main method configuring common muon augmentations"""
    
    Container = "Muons"+suff

    acc = ComponentAccumulator()
    from DerivationFrameworkMuons import DFCommonMuonsConfig   
    DFCommonMuonsTrtCutOff = DFCommonMuonsConfig.TrtCutOff
   
    #====================================================================
    # MCP GROUP TOOLS 
    #====================================================================
   
    from DerivationFrameworkTools.DerivationFrameworkToolsConfig import AsgSelectionToolWrapperCfg
    DFCommonMuonToolWrapperTools = []
    
    ### IDHits
    # turn of the momentum correction which is not needed for IDHits cut and Preselection
    from MuonSelectorTools.MuonSelectorToolsConfig import MuonSelectionToolCfg
    from AthenaConfiguration.Enums import LHCPeriod
    isRun3 = False
    if flags.GeoModel.Run == LHCPeriod.Run3: isRun3 = True
    if not hasattr(acc, "DFCommonMuonsSelector"):
        DFCommonMuonsSelector = acc.popToolsAndMerge(MuonSelectionToolCfg(
            flags,
            name            = "DFCommonMuonsSelector",
            MaxEta          = 3.,
            MuQuality       = 3,
            IsRun3Geo       = isRun3,
            TurnOffMomCorr  = True)) 
        acc.addPublicTool(DFCommonMuonsSelector)
    if DFCommonMuonsTrtCutOff is not None: DFCommonMuonsSelector.TrtCutOff = DFCommonMuonsTrtCutOff
    DFCommonMuonToolWrapperIDCuts = acc.getPrimaryAndMerge(AsgSelectionToolWrapperCfg(
        flags,
        name               = "DFCommonMuonToolWrapperIDCuts"+suff,
        AsgSelectionTool   = DFCommonMuonsSelector,
        CutType            = "IDHits",
        StoreGateEntryName = "DFCommonMuonPassIDCuts",
        ContainerName      = Container))
    DFCommonMuonToolWrapperTools.append(DFCommonMuonToolWrapperIDCuts)
   
    DFCommonMuonToolWrapperPreselection = acc.getPrimaryAndMerge(AsgSelectionToolWrapperCfg(
        flags,
        name               = "DFCommonMuonToolWrapperPreselection"+suff,
        AsgSelectionTool   = DFCommonMuonsSelector,
        CutType            = "Preselection",
        StoreGateEntryName = "DFCommonMuonPassPreselection",
        ContainerName      = Container))
    DFCommonMuonToolWrapperTools.append(DFCommonMuonToolWrapperPreselection)
   
    #############
    #  Add tools
    #############
    CommonAugmentation = CompFactory.DerivationFramework.CommonAugmentation
    acc.addEventAlgo(CommonAugmentation("DFCommonMuonsKernel"+suff,
                                        AugmentationTools = DFCommonMuonToolWrapperTools))

    from IsolationAlgs.DerivationTrackIsoConfig import DerivationTrackIsoCfg
    # A selection of WP is probably needed, as only a few variables are in CP content !
    #   maybe MUON derivations can add some other ones for studies
    #listofTTVAWP = [ 'Loose', 'Nominal', 'Tight',
    #                 'Prompt_D0Sig', 'Prompt_MaxWeight',
    #                 'Nonprompt_Hard_D0Sig',
    #                 'Nonprompt_Medium_D0Sig',
    #                 'Nonprompt_All_D0Sig',
    #                 'Nonprompt_Hard_MaxWeight',
    #                 'Nonprompt_Medium_MaxWeight',
    #                 'Nonprompt_All_MaxWeight' ]
    for WP in [ 'Nonprompt_All_MaxWeight' ]:
        acc.merge(DerivationTrackIsoCfg(flags, WP = WP, object_types = ('Electrons', 'Muons'), postfix=suff))

    if "LRT" in Container and not hasattr(acc, 'LRTMuonCaloIsolationBuilder'):
        from IsolationAlgs.IsolationSteeringDerivConfig import LRTMuonIsolationSteeringDerivCfg
        acc.merge(LRTMuonIsolationSteeringDerivCfg(flags))

        from IsolationAlgs.IsolationBuilderConfig import muIsolationCfg
        acc.merge(muIsolationCfg(flags,
                                 name="muonIsolationLRT",
                                 MuonCollectionContainerName = Container
        ))

    return acc
