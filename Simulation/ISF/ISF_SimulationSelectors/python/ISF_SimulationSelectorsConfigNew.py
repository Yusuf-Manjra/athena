"""
ISF_SimulationSelectors for ComponentAccumulator configuration

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.SystemOfUnits import GeV
from ISF_SimulationSelectors import SimulationFlavor
from ISF_Services.ISF_ServicesCoreConfigNew import ParticleKillerSvcCfg
from ISF_Geant4Services.ISF_Geant4ServicesConfigNew import (
    Geant4SimCfg, AFIIGeant4SimCfg, LongLivedGeant4SimCfg, AFII_QS_Geant4SimCfg,
    FullGeant4SimCfg, PassBackGeant4SimCfg,
)
from ISF_FastCaloSimServices.ISF_FastCaloSimServicesConfigNew import (
    FastCaloSimSvcCfg, LegacyAFIIFastCaloSimSvcCfg,
    FastCaloSimV2SvcCfg, DNNCaloSimSvcCfg,
    FastHitConvAlgFastCaloSimSvcCfg,
    FastCaloSimPileupSvcCfg, FastCaloSimPileupOTSvcCfg,
)
from ISF_FatrasServices.ISF_FatrasConfig import (
    fatrasSimServiceIDCfg, fatrasPileupSimServiceIDCfg,
)


def DefaultParticleKillerSelectorCfg(flags, name="ISF_DefaultParticleKillerSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(ParticleKillerSvcCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_ParticleKillerSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.ParticleKiller)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def PileupParticleKillerSelectorCfg(flags, name="ISF_PileupParticleKillerSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PileupBCID", [1])
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(ParticleKillerSvcCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_ParticleKillerSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.ParticleKiller)
    acc.setPrivateTools(CompFactory.ISF.KinematicPileupSimSelector(name, **kwargs))
    return acc


def DefaultGeant4SelectorCfg(flags, name="ISF_DefaultGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultAFIIGeant4SelectorCfg(flags, name="ISF_DefaultAFIIGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFIIGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    tool = acc.popToolsAndMerge(DefaultGeant4SelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def DefaultLongLivedGeant4SelectorCfg(flags, name="ISF_DefaultLongLivedGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(LongLivedGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_LongLivedGeant4SimSvc"))
    tool = acc.popToolsAndMerge(DefaultGeant4SelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def DefaultAFII_QS_Geant4SelectorCfg(flags, name="ISF_DefaultAFII_QS_Geant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFII_QS_Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    tool = acc.popToolsAndMerge(DefaultGeant4SelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def FullGeant4SelectorCfg(flags, name="ISF_FullGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(FullGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_FullGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def PassBackGeant4SelectorCfg(flags, name="ISF_PassBackGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(PassBackGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_PassBackGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultFastCaloSimSelectorCfg(flags, name="ISF_DefaultFastCaloSimSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(FastCaloSimSvcCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_FastCaloSimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSim)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultLegacyAFIIFastCaloSimSelectorCfg(flags, name="ISF_DefaultLegacyAFIIFastCaloSimSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(LegacyAFIIFastCaloSimSvcCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_LegacyAFIIFastCaloSimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSim)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultFastCaloSimV2SelectorCfg(flags, name="ISF_DefaultFastCaloSimV2Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(FastCaloSimV2SvcCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_FastCaloSimSvcV2"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSimV2)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultDNNCaloSimSelectorCfg(flags, name="ISF_DefaultDNNCaloSimSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(DNNCaloSimSvcCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_DNNCaloSimSvc"))
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def FastHitConvAlgFastCaloSimSelectorCfg(flags, name="ISF_FastHitConvAlgFastCaloSimSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(FastHitConvAlgFastCaloSimSvcCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_FastHitConvAlgFastCaloSimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSim)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultFatrasSelectorCfg(flags, name="ISF_DefaultFatrasSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(fatrasSimServiceIDCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_FatrasSimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultFatrasNewExtrapolationSelectorCfg(flags, name="ISF_DefaultFatrasNewExtrapolationSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultParametricSimulationSelectorCfg(flags, name="ISF_DefaultParametricSimulationSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        kwargs.setdefault("Simulator", "ISF_ParametricSimSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Parametric)
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


# PileUpSimSelector Configurations
def PileupSimSelectorCfg(flags, name="ISF_PileupSimSelector", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.ISF.KinematicPileupSimSelector(name, **kwargs))
    return acc


def FatrasPileupSelectorCfg(flags, name="ISF_FatrasPileupSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PileupBCID", [1])
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(fatrasPileupSimServiceIDCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_FatrasPileupSimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FatrasPileup)
    tool = acc.popToolsAndMerge(PileupSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


# FatrasPileupSelector_noHits not migrated
# due to missing getter for Simulator "ISF_FatrasPileupSimSvc_noHits"


def FastCaloSimPileupSelectorCfg(flags, name="ISF_FastCaloSimPileupSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PileupBCID"  , flags.Sim.FastChain.BCID)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(FastCaloSimPileupSvcCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_FastCaloSimPileupSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSimPileup)
    tool = acc.popToolsAndMerge(PileupSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def FastCaloSimPileupOTSelectorCfg(flags, name="ISF_FastCaloSimPileupOTSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PileupBCID", flags.Sim.FastChain.BCID)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(FastCaloSimPileupOTSvcCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_FastCaloSimPileupOTSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSimPileup)
    tool = acc.popToolsAndMerge(PileupSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


# KinematicSimSelector Configurations
def ElectronGeant4SelectorCfg(flags, name="ISF_ElectronGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("ParticlePDG", 11)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def NeutralGeant4SelectorCfg(flags, name="ISF_NeutralGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("Charge", 0)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def ProtonAFIIGeant4SelectorCfg(flags, name="ISF_ProtonAFIIGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("MaxMom", 750)
    kwargs.setdefault("ParticlePDG", 2212)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFIIGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def ProtonAFII_QS_Geant4SelectorCfg(flags, name="ISF_ProtonAFII_QS_Geant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFII_QS_Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    tool = acc.popToolsAndMerge(ProtonAFIIGeant4SelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def PionAFIIGeant4SelectorCfg(flags, name="ISF_PionAFIIGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("MaxMom", 200)
    kwargs.setdefault("ParticlePDG", 211)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFIIGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def PionG4FastCaloGeant4Selector(flags, name="ISF_PionG4FastCaloGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("MaxEkin", 200)
    kwargs.setdefault("ParticlePDG", 211)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFIIGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def ProtonG4FastCaloGeant4Selector(flags, name="ISF_ProtonG4FastCaloGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 2212)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFIIGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def NeutronG4FastCaloGeant4Selector(flags, name="ISF_NeutronG4FastCaloGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 2112)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFIIGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def ChargedKaonG4FastCaloGeant4Selector(flags, name="ISF_ChargedKaonG4FastCaloGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 321)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFIIGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def KLongG4FastCaloGeant4Selector(flags, name="ISF_KLongG4FastCaloGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 130)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFIIGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def PionAFII_QS_Geant4SelectorCfg(flags, name="ISF_PionAFII_QS_Geant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFII_QS_Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    tool = acc.popToolsAndMerge(PionAFIIGeant4SelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def ChargedKaonAFIIGeant4SelectorCfg(flags, name="ISF_ChargedKaonAFIIGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("MaxMom", 750)
    kwargs.setdefault("ParticlePDG", 321)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFIIGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def ChargedKaonAFII_QS_Geant4SelectorCfg(flags, name="ISF_ChargedKaonAFII_QS_Geant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFII_QS_Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    tool = acc.popToolsAndMerge(ChargedKaonAFIIGeant4SelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def KLongAFIIGeant4SelectorCfg(flags, name="ISF_KLongAFIIGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("MaxMom", 750)
    kwargs.setdefault("ParticlePDG", 130)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFIIGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def KLongAFII_QS_Geant4SelectorCfg(flags, name="ISF_KLongAFII_QS_Geant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFII_QS_Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    tool = acc.popToolsAndMerge(KLongAFIIGeant4SelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def MuonSelectorCfg(flags, name="ISF_MuonSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("ParticlePDG", 13)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def MuonGeant4SelectorCfg(flags, name="ISF_MuonGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    tool = acc.popToolsAndMerge(MuonSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def MuonAFIIGeant4SelectorCfg(flags, name="ISF_MuonAFIIGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFIIGeant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    tool = acc.popToolsAndMerge(MuonGeant4SelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def MuonAFII_QS_Geant4SelectorCfg(flags, name="ISF_MuonAFII_QS_Geant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(AFII_QS_Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    tool = acc.popToolsAndMerge(MuonGeant4SelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def MuonFatrasSelectorCfg(flags, name="ISF_MuonFatrasSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(fatrasSimServiceIDCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_FatrasSimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    tool = acc.popToolsAndMerge(MuonSelectorCfg(flags, name, **kwargs)) 
    acc.setPrivateTools(tool)
    return acc


def MuonFatrasPileupSelectorCfg(flags, name="ISF_MuonFatrasPileupSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(fatrasPileupSimServiceIDCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_FatrasPileupSimSvc"))
    kwargs.setdefault("PileupBCID", [1])
    kwargs.setdefault("ParticlePDG", 13)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    tool = acc.popToolsAndMerge(PileupSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def WithinEta5FastCaloSimSelectorCfg(flags, name="ISF_WithinEta5FastCaloSimSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(FastCaloSimSvcCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_FastCaloSimSvc"))
    kwargs.setdefault("MinPosEta", -5.0)
    kwargs.setdefault("MaxPosEta",  5.0)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSim)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def EtaGreater5ParticleKillerSimSelectorCfg(flags, name="ISF_EtaGreater5ParticleKillerSimSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(ParticleKillerSvcCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_ParticleKillerSvc"))
    kwargs.setdefault("MinPosEta", -5.0)
    kwargs.setdefault("MaxPosEta",  5.0)
    kwargs.setdefault("InvertCuts", True)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.ParticleKiller)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def EtaGreater5PileupParticleKillerSimSelectorCfg(flags, name="ISF_EtaGreater5PileupParticleKillerSimSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(ParticleKillerSvcCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_ParticleKillerSvc"))
    kwargs.setdefault("MinPosEta", -5.0)
    kwargs.setdefault("MaxPosEta",  5.0)
    kwargs.setdefault("InvertCuts", True)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.ParticleKiller)
    acc.setPrivateTools(CompFactory.ISF.KinematicPileupSimSelector(name, **kwargs))
    return acc


# ConeSimSelector Configurations
def PhotonConeSelectorCfg(flags, name="ISF_PhotonConeSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("ConeCreatorPDGs", [22]) # photons
    kwargs.setdefault("ConeCreatorMinPt", 20.*GeV)
    kwargs.setdefault("ConeSize", 0.6)
    kwargs.setdefault("CheckConeCreatorAncestors", False)
    acc.setPrivateTools(CompFactory.ISF.ConeSimSelector(name, **kwargs))
    return acc


def PhotonConeFatrasSelectorCfg(flags, name="ISF_PhotonConeFatrasSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(fatrasSimServiceIDCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_FatrasSimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    tool = acc.popToolsAndMerge(PhotonConeSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def PhotonConeGeant4SelectorCfg(flags, name="ISF_PhotonConeGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    tool = acc.popToolsAndMerge(PhotonConeSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def HiggsLeptonsConeSimSelectorCfg(flags, name="ISF_HiggsLeptonsConeSimSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("ConeCreatorPDGs", [11, 13]) # e, mu
    kwargs.setdefault("ConeCreatorMinPt", 0.)
    kwargs.setdefault("ConeSize", 0.4)
    kwargs.setdefault("CheckConeCreatorAncestors", True)
    kwargs.setdefault("ConeCreatorAncestor", [24, 23]) # W , Z
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault("ConeCreatorAncestorRelation", 1)
    acc.setPrivateTools(CompFactory.ISF.ConeSimSelector(name, **kwargs))
    return acc


def HiggsLeptonsConeGeant4SelectorCfg(flags, name="ISF_HiggsLeptonsConeGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    tool = acc.popToolsAndMerge(HiggsLeptonsConeSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def ElectronsMuonsConeSimSelectorCfg(flags, name="ISF_ElectronsMuonsConeSimSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("ConeCreatorPDGs", [11, 13]) # e, mu
    acc.setPrivateTools(CompFactory.ISF.ConeSimSelector(name, **kwargs))
    return acc


def HiggsLeptonsConeGeant4CaloSelectorCfg(flags, name="ISF_HiggsLeptonsConeGeant4CaloSelector", **kwargs):
    kwargs.setdefault("ExtrapolateToCaloEntry", True)
    kwargs.setdefault("Extrapolator", "ISF_CaloEntryTrkExtrapolator")
    return HiggsLeptonsConeGeant4SelectorCfg(flags, name, **kwargs)


def WLeptonsConeGeant4SelectorCfg(flags, name="ISF_WLeptonsConeGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    kwargs.setdefault("ConeCreatorMinPt", 0.)
    kwargs.setdefault("ConeSize", 0.4)
    kwargs.setdefault("CheckConeCreatorAncestors", True)
    kwargs.setdefault("ConeCreatorAncestor", [24]) # W
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault("ConeCreatorAncestorRelation", 0)
    tool = acc.popToolsAndMerge(ElectronsMuonsConeSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def ZLeptonsDirectionConeGeant4SelectorCfg(flags, name="ISF_ZLeptonsDirectionConeGeant4Selector", **kwargs):
    # this selector picks all particles with a mometum direction
    # within DeltaR<ConeSize relative to the Z decay lepton directions
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    kwargs.setdefault("ConeCreatorMinPt", 0.)
    kwargs.setdefault("ConeSize", 0.4)
    kwargs.setdefault("CheckConeCreatorAncestors", True)
    kwargs.setdefault("ConeCreatorAncestor", [23]) # Z
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault("ConeCreatorAncestorRelation", 0)
    tool = acc.popToolsAndMerge(ElectronsMuonsConeSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def ZLeptonsPositionConeGeant4SelectorCfg(flags, name="ISF_ZLeptonsPositionConeGeant4Selector", **kwargs):
    # this selector picks all particles with a position inside a cone
    # around the Z decay leptons directions
    kwargs.setdefault("CheckParticlePosition", True)
    return ZLeptonsDirectionConeGeant4SelectorCfg(flags, name, **kwargs)


def JPsiLeptonsConeGeant4SelectorCfg(flags, name="ISF_JPsiLeptonsConeGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    kwargs.setdefault("ConeCreatorMinPt", 0.)
    kwargs.setdefault("ConeSize", 0.4)
    kwargs.setdefault("CheckConeCreatorAncestors", True)
    kwargs.setdefault("ConeCreatorAncestor", [443]) # J/Psi
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault("ConeCreatorAncestorRelation", 0)
    tool = acc.popToolsAndMerge(ElectronsMuonsConeSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


# TruthAssocSimSelector configurations
def BHadronProductsSimSelectorCfg(flags, name="ISF_BHadronProductsSimSelector", **kwargs):
    acc = ComponentAccumulator()
    B_hadrons = [
        511, 521, 531, 541,
        5122, 5112, 5132,
        5232, 5212, 5222
    ]
    kwargs.setdefault("RelativesPDGCode", B_hadrons)
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault("Relation", 2)
    acc.setPrivateTools(CompFactory.ISF.TruthAssocSimSelector(name, **kwargs))
    return acc


def BHadronProductsGeant4SelectorCfg(flags, name="ISF_BHadronProductsGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    tool = acc.popToolsAndMerge(BHadronProductsSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def BHadronProductsFatrasSelectorCfg(flags, name="ISF_BHadronProductsFatrasSelector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(fatrasSimServiceIDCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISF_FatrasSimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    tool = acc.popToolsAndMerge(BHadronProductsSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def TauProductsSimSelectorCfg(flags, name="ISF_TauProductsSimSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("RelativesPDGCode", [15]) # tau
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault("Relation", 0)
    acc.setPrivateTools(CompFactory.ISF.TruthAssocSimSelector(name, **kwargs))
    return acc


def TauProductsGeant4SelectorCfg(flags, name="ISF_TauProductsGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    tool = acc.popToolsAndMerge(TauProductsSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def ZProductsSimSelectorCfg(flags, name="ISF_ZProductsSimSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("RelativesPDGCode", [23]) # Z
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault("Relation", 0)
    acc.setPrivateTools(CompFactory.ISF.TruthAssocSimSelector(name, **kwargs))
    return acc


def ZProductsGeant4SelectorCfg(flags, name="ISF_ZProductsGeant4Selector", **kwargs):
    acc = ComponentAccumulator()
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    tool = acc.popToolsAndMerge(ZProductsSimSelectorCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


# HistorySimSelector Configurations
def SubDetStickyGeant4SimSelectorCfg(flags, name="ISF_SubDetStickyGeant4SimSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("RequiresUnchangedGeoID", True)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("PrevSimSvc", acc.getService("ISFG4SimSvc"))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.HistorySimSelector(name, **kwargs))
    return acc


def GlobalStickyGeant4SimSelectorCfg(flags, name="ISF_GlobalStickyGeant4SimSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PrevSimSvc", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("RequiresUnchangedGeoID", False)
    if flags.Concurrency.NumThreads == 0 and 'MT' not in flags.Sim.ISF.Simulator:
        acc.merge(Geant4SimCfg(flags))
        kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.HistorySimSelector(name, **kwargs))
    return acc
