# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getBarrelFastSimDedicatedSD(name="BarrelFastSimDedicatedSD", **kwargs):
    return CfgMgr.BarrelFastSimDedicatedSD(name, **kwargs)

def getEndcapFastSimDedicatedSD(name="EndcapFastSimDedicatedSD", **kwargs):
    return CfgMgr.EndcapFastSimDedicatedSD(name, **kwargs)

def getFCALFastSimDedicatedSD(name="FCALFastSimDedicatedSD", **kwargs):
    return CfgMgr.FCALFastSimDedicatedSD(name, **kwargs)

def getFCAL2FastSimDedicatedSD(name="FCAL2FastSimDedicatedSD", **kwargs):
    return CfgMgr.FCALFastSimDedicatedSD(name, **kwargs)

#def getDeadMaterialShower(name="DeadMaterialShower", **kwargs):
#    return CfgMgr.DeadMaterialShower(name, **kwargs)

def getEMBFastShower(name="EMBFastShower", **kwargs):
    kwargs.setdefault("RegionNames",        ["EMB"])
    kwargs.setdefault("EFlagToShowerLib",   False)
    kwargs.setdefault("GFlagToShowerLib",   False)
    kwargs.setdefault("NeutFlagToShowerLib",False)
    kwargs.setdefault("PionFlagToShowerLib",False)
    kwargs.setdefault("ContainLow",         True)
    kwargs.setdefault("AbsLowEta",          0.3)
    kwargs.setdefault("ContainHigh",        True)
    kwargs.setdefault("AbsHighEta",         1.1)
    kwargs.setdefault("ContainCrack",       True)
    kwargs.setdefault("AbsCrackEta1",       0.5)
    kwargs.setdefault("AbsCrackEta2",       1.1)
    kwargs.setdefault("DetectorTag",        100000)
    kwargs.setdefault("SensitiveDetector",  "BarrelFastSimDedicatedSD")
    return CfgMgr.LArFastShowerTool(name, **kwargs)

def getEMECFastShower(name="EMECFastShower", **kwargs):
    kwargs.setdefault("RegionNames",        ["EMECPara"])
    kwargs.setdefault("EFlagToShowerLib",   False)
    kwargs.setdefault("GFlagToShowerLib",   False)
    kwargs.setdefault("NeutFlagToShowerLib",False)
    kwargs.setdefault("PionFlagToShowerLib",False)
    kwargs.setdefault("ContainLow",         True)
    kwargs.setdefault("AbsLowEta",          1.8)
    kwargs.setdefault("ContainHigh",        True)
    kwargs.setdefault("AbsHighEta",         2.9)
    kwargs.setdefault("ContainCrack",       True)
    kwargs.setdefault("AbsCrackEta1",       2.2)
    kwargs.setdefault("AbsCrackEta2",       2.8)
    kwargs.setdefault("DetectorTag",        200000)
    kwargs.setdefault("SensitiveDetector", "EndcapFastSimDedicatedSD")
    return CfgMgr.LArFastShowerTool(name, **kwargs)

def getFCALFastShower(name="FCALFastShower", **kwargs):
    kwargs.setdefault("RegionNames",        ["FCALPara"])
    kwargs.setdefault("EFlagToShowerLib",   True)
    kwargs.setdefault("GFlagToShowerLib",   True)
    kwargs.setdefault("NeutFlagToShowerLib",True)
    kwargs.setdefault("PionFlagToShowerLib",True)
    kwargs.setdefault("ContainLow",         True)
    kwargs.setdefault("AbsLowEta",          3.8)
    kwargs.setdefault("ContainHigh",        True)
    kwargs.setdefault("AbsHighEta",         4.4)
    kwargs.setdefault("DetectorTag",        300000)
    kwargs.setdefault("SensitiveDetector", "FCALFastSimDedicatedSD")
    return CfgMgr.LArFastShowerTool(name, **kwargs)

def getFCAL2FastShower(name="FCAL2FastShower", **kwargs):
    kwargs.setdefault("RegionNames",        ["FCAL2Para"])
    kwargs.setdefault("EFlagToShowerLib",   True)
    kwargs.setdefault("GFlagToShowerLib",   True)
    kwargs.setdefault("NeutFlagToShowerLib",True)
    kwargs.setdefault("PionFlagToShowerLib",True)
    kwargs.setdefault("ContainLow",         True)
    kwargs.setdefault("AbsLowEta",          3.8)
    kwargs.setdefault("ContainHigh",        True)
    kwargs.setdefault("AbsHighEta",         4.4)
    kwargs.setdefault("DetectorTag",        400000)
    kwargs.setdefault("SensitiveDetector", "FCALFastSimDedicatedSD")
    return CfgMgr.LArFastShowerTool(name, **kwargs)
