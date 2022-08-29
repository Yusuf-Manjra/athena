# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetVKalVxInJetTool package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import LHCPeriod

def InDetTrkInJetTypeCfg(flags, name="TrkInJetType", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("trkSctHits", 4 if flags.GeoModel.Run <= LHCPeriod.Run3 else -1)
    acc.setPrivateTools(CompFactory.InDet.InDetTrkInJetType(name, **kwargs))
    return acc

def InDetVKalVxInJetToolCfg(flags, name="InDetVKalVxInJetTool", **kwargs):
    acc = ComponentAccumulator()

    if "TrackClassTool" not in kwargs:
        kwargs.setdefault("TrackClassTool", acc.popToolsAndMerge(
            InDetTrkInJetTypeCfg(flags)))

    kwargs.setdefault("ExistIBL", flags.GeoModel.Run in [LHCPeriod.Run2, LHCPeriod.Run3])
    kwargs.setdefault("getNegativeTag", "Flip" in name)

    if flags.GeoModel.Run >= LHCPeriod.Run4:
        from InDetConfig.InDetEtaDependentCutsConfig import IDEtaDependentCuts_SV1_SvcCfg
        acc.merge(IDEtaDependentCuts_SV1_SvcCfg(flags, name="IDEtaDepCutsSvc_" + name))
        kwargs.setdefault("InDetEtaDependentCutsSvc", acc.getService("IDEtaDepCutsSvc_" + name))
        kwargs.setdefault("useVertexCleaningPix", False) # Would use hardcoded InDet Pixel geometry
        kwargs.setdefault("useITkMaterialRejection", True)

    acc.setPrivateTools(CompFactory.InDet.InDetVKalVxInJetTool(name,**kwargs))
    return acc

def MSV_InDetVKalVxInJetToolCfg(flags, name="IDVKalMultiVxInJet", **kwargs):
    kwargs.setdefault("getNegativeTail", False)
    kwargs.setdefault("ConeForTag", 1.0)
    kwargs.setdefault("MultiVertex", True)
    return InDetVKalVxInJetToolCfg(flags, name, **kwargs)

def InDetVKalVxInHiPtJetToolCfg(flags, name="InDetVKalVxInHiPtJetTool", **kwargs):
    kwargs.setdefault("CutSharedHits", 0)
    kwargs.setdefault("Sel2VrtChi2Cut", 4.)
    kwargs.setdefault("CutBVrtScore", 0.002)
    return InDetVKalVxInJetToolCfg(flags, name, **kwargs)
