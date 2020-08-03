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
    FullGeant4SimCfg, PassBackGeant4SimCfg
)


def DefaultSimSelectorCfg(flags, name="ISF_DefaultSimSelector", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.ISF.DefaultSimSelector(name, **kwargs))
    return acc


def DefaultParticleKillerSelectorCfg(flags, name="ISF_DefaultParticleKillerSelector", **kwargs):
    acc = ParticleKillerSvcCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISF_ParticleKillerSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.ParticleKiller)
    acc.merge(DefaultSimSelectorCfg(flags, name, **kwargs))
    return acc


def PileupParticleKillerSelectorCfg(flags, name="ISF_PileupParticleKillerSelector", **kwargs):
    acc = ParticleKillerSvcCfg(flags)
    kwargs.setdefault("PileupBCID", [1])
    kwargs.setdefault("Simulator", acc.getService("ISF_ParticleKillerSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.ParticleKiller)
    acc.setPrivateTools(CompFactory.ISF.KinematicPileupSimSelector(name, **kwargs))
    return acc


def DefaultGeant4SelectorCfg(flags, name="ISF_DefaultGeant4Selector", **kwargs):
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.merge(DefaultSimSelectorCfg(flags, name, **kwargs))
    return acc


def DefaultAFIIGeant4SelectorCfg(flags, name="ISF_DefaultAFIIGeant4Selector", **kwargs):
    acc = AFIIGeant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    acc.merge(DefaultGeant4SelectorCfg(flags, name, **kwargs))
    return acc


def DefaultLongLivedGeant4SelectorCfg(flags, name="ISF_DefaultLongLivedGeant4Selector", **kwargs):
    acc = LongLivedGeant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISF_LongLivedGeant4SimSvc"))
    acc.merge(DefaultGeant4SelectorCfg(flags, name, **kwargs))
    return acc


def DefaultAFII_QS_Geant4SelectorCfg(flags, name="ISF_DefaultAFII_QS_Geant4Selector", **kwargs):
    acc = AFII_QS_Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    acc.merge(DefaultGeant4SelectorCfg(flags, name, **kwargs))
    return acc


def FullGeant4SelectorCfg(flags, name="ISF_FullGeant4Selector", **kwargs):
    acc = FullGeant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISF_FullGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.merge(DefaultSimSelectorCfg(flags, name, **kwargs))
    return acc


def PassBackGeant4SelectorCfg(flags, name="ISF_PassBackGeant4Selector", **kwargs):
    acc = PassBackGeant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISF_PassBackGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.merge(DefaultSimSelectorCfg(flags, name, **kwargs))
    return acc


def DefaultFastCaloSimSelectorCfg(flags, name="ISF_DefaultFastCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator", "ISF_FastCaloSimSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSim)
    return DefaultSimSelectorCfg(flags, name, **kwargs)


def DefaultLegacyAFIIFastCaloSimSelectorCfg(flags, name="ISF_DefaultLegacyAFIIFastCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator", "ISF_LegacyAFIIFastCaloSimSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSim)
    return DefaultSimSelectorCfg(flags, name, **kwargs)


def DefaultFastCaloSimV2SelectorCfg(flags, name="ISF_DefaultFastCaloSimV2Selector", **kwargs):
    kwargs.setdefault("Simulator", "ISF_FastCaloSimSvcV2") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSimV2)
    return DefaultSimSelectorCfg(flags, name, **kwargs)


def DefaultDNNCaloSimSelectorCfg(flags, name="ISF_DefaultDNNCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator", "ISF_DNNCaloSimSvc") # TODO
    return DefaultSimSelectorCfg(flags, name, **kwargs)


def FastHitConvAlgFastCaloSimSelectorCfg(flags, name="ISF_FastHitConvAlgFastCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator", "ISF_FastHitConvAlgFastCaloSimSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSim)
    return DefaultSimSelectorCfg(flags, name, **kwargs)

def FastHitConvAlgLegacyAFIIFastCaloSimSelectorCfg(flags, name="ISF_FastHitConvAlgLegacyAFIIFastCaloSimSelector", **kwargs):
    kwargs.setdefault("Simulator", "ISF_FastHitConvAlgLegacyAFIIFastCaloSimSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSim)
    return DefaultSimSelectorCfg(flags, name, **kwargs)


def DefaultFatrasSelectorCfg(flags, name="ISF_DefaultFatrasSelector", **kwargs):
    kwargs.setdefault("Simulator", "ISF_FatrasSimSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    return DefaultSimSelectorCfg(flags, name, **kwargs)


def DefaultFatrasNewExtrapolationSelectorCfg(flags, name="ISF_DefaultFatrasNewExtrapolationSelector", **kwargs):
    kwargs.setdefault("Simulator", "ISF_FatrasNewExtrapolationSimSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    return DefaultSimSelectorCfg(flags, name, **kwargs)


def DefaultParametricSimulationSelectorCfg(flags, name="ISF_DefaultParametricSimulationSelector", **kwargs):
    kwargs.setdefault("Simulator", "ISF_ParametricSimSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Parametric)
    return DefaultSimSelectorCfg(flags, name, **kwargs)


# PileUpSimSelector Configurations
def PileupSimSelectorCfg(flags, name="ISF_PileupSimSelector", **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.ISF.KinematicPileupSimSelector(name, **kwargs))
    return acc


def FatrasPileupSelectorCfg(flags, name="ISF_FatrasPileupSelector", **kwargs):
    kwargs.setdefault("PileupBCID", [1])
    kwargs.setdefault("Simulator", "ISF_FatrasPileupSimSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FatrasPileup)
    return PileupSimSelectorCfg(flags, name, **kwargs)


def FatrasPileupSelector_noHits(flags, name="ISF_FatrasPileupSelector_noHits", **kwargs):
    kwargs.setdefault("PileupBCID", [2])
    kwargs.setdefault("Simulator", "ISF_FatrasPileupSimSvc_noHits") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FatrasPileup)
    return PileupSimSelectorCfg(flags, name, **kwargs)


def FastCaloSimPileupSelectorCfg(flags, name="ISF_FastCaloSimPileupSelector", **kwargs):
    kwargs.setdefault("PileupBCID"  , flags.Sim.FastChain.BCID)
    kwargs.setdefault("Simulator", "ISF_FastCaloSimPileupSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSimPileup)
    return PileupSimSelectorCfg(flags, name, **kwargs)


def FastCaloSimPileupOTSelectorCfg(flags, name="ISF_FastCaloSimPileupOTSelector", **kwargs):
    kwargs.setdefault("PileupBCID", flags.Sim.FastChain.BCID)
    kwargs.setdefault("Simulator", "ISF_FastCaloSimPileupOTSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSimPileup)
    return PileupSimSelectorCfg(flags, name, **kwargs)


# KinematicSimSelector Configurations
def ElectronGeant4SelectorCfg(flags, name="ISF_ElectronGeant4Selector", **kwargs):
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("ParticlePDG", 11)
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def NeutralGeant4SelectorCfg(flags, name="ISF_NeutralGeant4Selector", **kwargs):
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("Charge", 0)
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def ProtonAFIIGeant4SelectorCfg(flags, name="ISF_ProtonAFIIGeant4Selector", **kwargs):
    acc = AFIIGeant4SimCfg(flags)
    kwargs.setdefault("MaxMom", 750)
    kwargs.setdefault("ParticlePDG", 2212)
    kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def ProtonAFII_QS_Geant4SelectorCfg(flags, name="ISF_ProtonAFII_QS_Geant4Selector", **kwargs):
    acc = AFII_QS_Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    acc.merge(ProtonAFIIGeant4SelectorCfg(flags, name, **kwargs))
    return acc


def PionAFIIGeant4SelectorCfg(flags, name="ISF_PionAFIIGeant4Selector", **kwargs):
    acc = AFIIGeant4SimCfg(flags)
    kwargs.setdefault("MaxMom", 200)
    kwargs.setdefault("ParticlePDG", 211)
    kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def PionG4FastCaloGeant4Selector(flags, name="ISF_PionG4FastCaloGeant4Selector", **kwargs):
    acc = AFIIGeant4SimCfg(flags)
    kwargs.setdefault("MaxEkin", 200)
    kwargs.setdefault("ParticlePDG", 211)
    if flags.Concurrency.NumThreads == 0:
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def ProtonG4FastCaloGeant4Selector(flags, name="ISF_ProtonG4FastCaloGeant4Selector", **kwargs):
    acc = AFIIGeant4SimCfg(flags)
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 2212)
    if flags.Concurrency.NumThreads == 0:
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def NeutronG4FastCaloGeant4Selector(flags, name="ISF_NeutronG4FastCaloGeant4Selector", **kwargs):
    acc = AFIIGeant4SimCfg(flags)
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 2112)
    if flags.Concurrency.NumThreads == 0:
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def ChargedKaonG4FastCaloGeant4Selector(flags, name="ISF_ChargedKaonG4FastCaloGeant4Selector", **kwargs):
    acc = AFIIGeant4SimCfg(flags)
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 321)
    if flags.Concurrency.NumThreads == 0:
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def KLongG4FastCaloGeant4Selector(flags, name="ISF_KLongG4FastCaloGeant4Selector", **kwargs):
    acc = AFIIGeant4SimCfg(flags)
    kwargs.setdefault("MaxEkin", 400)
    kwargs.setdefault("ParticlePDG", 130)
    if flags.Concurrency.NumThreads == 0:
        kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def PionAFII_QS_Geant4SelectorCfg(flags, name="ISF_PionAFII_QS_Geant4Selector", **kwargs):
    acc = AFII_QS_Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    acc.merge(PionAFIIGeant4SelectorCfg(flags, name, **kwargs))
    return acc


def ChargedKaonAFIIGeant4SelectorCfg(flags, name="ISF_ChargedKaonAFIIGeant4Selector", **kwargs):
    acc = AFIIGeant4SimCfg(flags)
    kwargs.setdefault("MaxMom", 750)
    kwargs.setdefault("ParticlePDG", 321)
    kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def ChargedKaonAFII_QS_Geant4SelectorCfg(flags, name="ISF_ChargedKaonAFII_QS_Geant4Selector", **kwargs):
    acc = AFII_QS_Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    acc.merge(ChargedKaonAFIIGeant4SelectorCfg(flags, name, **kwargs))
    return acc


def KLongAFIIGeant4SelectorCfg(flags, name="ISF_KLongAFIIGeant4Selector", **kwargs):
    acc = AFIIGeant4SimCfg(flags)
    kwargs.setdefault("MaxMom", 750)
    kwargs.setdefault("ParticlePDG", 130)
    kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def KLongAFII_QS_Geant4SelectorCfg(flags, name="ISF_KLongAFII_QS_Geant4Selector", **kwargs):
    acc = AFII_QS_Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    acc.merge(KLongAFIIGeant4SelectorCfg(flags, name, **kwargs))
    return acc


def MuonSelectorCfg(flags, name="ISF_MuonSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("ParticlePDG", 13)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc

def MuonGeant4SelectorCfg(flags, name="ISF_MuonGeant4Selector", **kwargs):
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.merge(MuonSelectorCfg(flags, name, **kwargs))
    return acc


def MuonAFIIGeant4SelectorCfg(flags, name="ISF_MuonAFIIGeant4Selector", **kwargs):
    acc = AFIIGeant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISF_AFIIGeant4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.merge(MuonGeant4SelectorCfg(flags, name, **kwargs))
    return acc


def MuonAFII_QS_Geant4SelectorCfg(flags, name="ISF_MuonAFII_QS_Geant4Selector", **kwargs):
    acc = AFII_QS_Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISF_AFII_QS_Geant4SimSvc"))
    acc.merge(MuonGeant4SelectorCfg(flags, name, **kwargs))
    return acc


def MuonFatrasSelectorCfg(flags, name="ISF_MuonFatrasSelector", **kwargs):
    kwargs.setdefault("Simulator", "ISF_FatrasSimSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    return MuonSelectorCfg(flags, name, **kwargs)


def MuonFatrasPileupSelectorCfg(flags, name="ISF_MuonFatrasPileupSelector", **kwargs):
    kwargs.setdefault("Simulator", "ISF_FatrasPileupSimSvc") # TODO
    kwargs.setdefault("PileupBCID", [1])
    kwargs.setdefault("ParticlePDG", 13)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    return PileupSimSelectorCfg(flags, name, **kwargs)


def WithinEta5FastCaloSimSelectorCfg(flags, name="ISF_WithinEta5FastCaloSimSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("Simulator", "ISF_FastCaloSimSvc") # TODO
    kwargs.setdefault("MinPosEta", -5.0)
    kwargs.setdefault("MaxPosEta",  5.0)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.FastCaloSim)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def EtaGreater5ParticleKillerSimSelectorCfg(flags, name="ISF_EtaGreater5ParticleKillerSimSelector", **kwargs):
    acc = ParticleKillerSvcCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISF_ParticleKillerSvc"))
    kwargs.setdefault("MinPosEta", -5.0)
    kwargs.setdefault("MaxPosEta",  5.0)
    kwargs.setdefault("InvertCuts", True)
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.ParticleKiller)
    acc.setPrivateTools(CompFactory.ISF.KinematicSimSelector(name, **kwargs))
    return acc


def EtaGreater5PileupParticleKillerSimSelectorCfg(flags, name="ISF_EtaGreater5PileupParticleKillerSimSelector", **kwargs):
    acc = ParticleKillerSvcCfg(flags)
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
    kwargs.setdefault("Simulator", "ISF_FatrasSimSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    return PhotonConeSelectorCfg(flags, name, **kwargs)


def PhotonConeGeant4SelectorCfg(flags, name="ISF_PhotonConeGeant4Selector", **kwargs):
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.merge(PhotonConeSelectorCfg(flags, name, **kwargs))
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
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.merge(HiggsLeptonsConeSimSelectorCfg(flags, name, **kwargs))
    return acc


def ElectronsMuonsConeSimSelectorCfg(flags, name="ISF_ElectronsMuonsConeSimSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("ConeCreatorPDGs", [11, 13]) # e, mu
    acc.setPrivateTools(CompFactory.ISF.ConeSimSelector(name, **kwargs))
    return acc


def HiggsLeptonsConeGeant4CaloSelectorCfg(flags, name="ISF_HiggsLeptonsConeGeant4CaloSelector", **kwargs):
    kwargs.setdefault( "ExtrapolateToCaloEntry", True)
    kwargs.setdefault( "Extrapolator", "ISF_CaloEntryTrkExtrapolator")
    return HiggsLeptonsConeGeant4SelectorCfg(flags, name, **kwargs)


def WLeptonsConeGeant4SelectorCfg(flags, name="ISF_WLeptonsConeGeant4Selector", **kwargs):
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    kwargs.setdefault("ConeCreatorMinPt", 0.)
    kwargs.setdefault("ConeSize", 0.4)
    kwargs.setdefault("CheckConeCreatorAncestors", True)
    kwargs.setdefault("ConeCreatorAncestor", [24]) # W
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault("ConeCreatorAncestorRelation", 0)
    acc.merge(ElectronsMuonsConeSimSelectorCfg(flags, name, **kwargs))
    return acc


def ZLeptonsDirectionConeGeant4SelectorCfg(flags, name="ISF_ZLeptonsDirectionConeGeant4Selector", **kwargs):
    # this selector picks all particles with a mometum direction
    # within DeltaR<ConeSize relative to the Z decay lepton directions
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    kwargs.setdefault("ConeCreatorMinPt", 0.)
    kwargs.setdefault("ConeSize", 0.4)
    kwargs.setdefault("CheckConeCreatorAncestors", True)
    kwargs.setdefault("ConeCreatorAncestor", [23]) # Z
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault("ConeCreatorAncestorRelation", 0)
    acc.merge(ElectronsMuonsConeSimSelectorCfg(flags, name, **kwargs))
    return acc


def ZLeptonsPositionConeGeant4SelectorCfg(flags, name="ISF_ZLeptonsPositionConeGeant4Selector", **kwargs):
    # this selector picks all particles with a position inside a cone
    # around the Z decay leptons directions
    kwargs.setdefault("CheckParticlePosition", True)
    return ZLeptonsDirectionConeGeant4SelectorCfg(flags, name, **kwargs)


def JPsiLeptonsConeGeant4SelectorCfg(flags, name="ISF_JPsiLeptonsConeGeant4Selector", **kwargs):
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    kwargs.setdefault("ConeCreatorMinPt", 0.)
    kwargs.setdefault("ConeSize", 0.4)
    kwargs.setdefault("CheckConeCreatorAncestors", True)
    kwargs.setdefault("ConeCreatorAncestor", [443]) # J/Psi
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault("ConeCreatorAncestorRelation", 0)
    acc.merge(ElectronsMuonsConeSimSelectorCfg(flags, name, **kwargs))
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
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.merge(BHadronProductsSimSelectorCfg(flags, name, **kwargs))
    return acc


def BHadronProductsFatrasSelectorCfg(flags, name="ISF_BHadronProductsFatrasSelector", **kwargs):
    kwargs.setdefault("Simulator", "ISF_FatrasSimSvc") # TODO
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Fatras)
    return BHadronProductsSimSelectorCfg(flags, name, **kwargs)


def TauProductsSimSelectorCfg(flags, name="ISF_TauProductsSimSelector", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("RelativesPDGCode", [15]) # tau
    # see HepMC manual for HepMC::GenVertex::particle iterator
    # 0=parents, 1=family, 2=ancestors, 3=relatives
    kwargs.setdefault("Relation", 0)
    acc.setPrivateTools(CompFactory.ISF.TruthAssocSimSelector(name, **kwargs))
    return acc


def TauProductsGeant4SelectorCfg(flags, name="ISF_TauProductsGeant4Selector", **kwargs):
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.merge(TauProductsSimSelectorCfg(flags, name, **kwargs))
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
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.merge(ZProductsSimSelectorCfg(flags, name, **kwargs))
    return acc


# HistorySimSelector Configurations
def SubDetStickyGeant4SimSelectorCfg(flags, name="ISF_SubDetStickyGeant4SimSelector", **kwargs):
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("PrevSimSvc", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("RequiresUnchangedGeoID", True)
    kwargs.setdefault("Simulator", "ISF_Geant4SimSvc")
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.HistorySimSelector(name, **kwargs))
    return acc


def GlobalStickyGeant4SimSelectorCfg(flags, name="ISF_GlobalStickyGeant4SimSelector", **kwargs):
    acc = Geant4SimCfg(flags)
    kwargs.setdefault("PrevSimSvc", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("RequiresUnchangedGeoID", False)
    kwargs.setdefault("Simulator", acc.getService("ISFG4SimSvc"))
    kwargs.setdefault("SimulationFlavor", SimulationFlavor.Geant4)
    acc.setPrivateTools(CompFactory.ISF.HistorySimSelector(name, **kwargs))
    return acc
