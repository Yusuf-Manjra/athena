# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF
KG Tan, 17/06/2012
"""

from AthenaCommon import CfgMgr
from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from AthenaCommon.SystemOfUnits import *

#--------------------------------------------------------------------------------------------------
## LorentzVectorGenerators

def getVertexPositionFromFile(name="ISF_VertexPositionFromFile", **kwargs):
    # VertexPositionFromFile
    kwargs.setdefault("VertexPositionsFile"         , "vtx-pos.txt")
    kwargs.setdefault("RunAndEventNumbersFile"      , "run-evt-nums.txt")
    return CfgMgr.ISF__VertexPositionFromFile(name, **kwargs)

def getVertexBeamCondPositioner(name="ISF_VertexBeamCondPositioner", **kwargs):
    # VertexBeamCondPositioner
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('RandomSvc'               , simFlags.RandomSvc.get_Value())
    return CfgMgr.ISF__VertexBeamCondPositioner(name, **kwargs)

def getLongBeamspotVertexPositioner(name="ISF_LongBeamspotVertexPositioner", **kwargs):
    # LongBeamspotVertexPositioner
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('LParameter'              , 150.0)
    kwargs.setdefault('RandomSvc'               , simFlags.RandomSvc.get_Value())
    return CfgMgr.ISF__LongBeamspotVertexPositioner(name, **kwargs)

def getCrabKissingVertexPositioner(name="ISF_CrabKissingVertexPositioner", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault('BunchLength'             , 75.0)
    kwargs.setdefault('RandomSvc'               , simFlags.RandomSvc.get_Value())
    kwargs.setdefault('BunchShape'              , "GAUSS")
    return CfgMgr.ISF__CrabKissingVertexPositioner(name, **kwargs)

#--------------------------------------------------------------------------------------------------
## GenEventManipulators

def getGenEventValidityChecker(name="ISF_GenEventValidityChecker", **kwargs):
    # GenEventValidityChecker
    return CfgMgr.ISF__GenEventValidityChecker(name, **kwargs)

def getGenEventVertexPositioner(name="ISF_GenEventVertexPositioner", **kwargs):
    # GenEventVertexPositioner
    from ISF_Config.ISF_jobProperties import ISF_Flags
    if ISF_Flags.VertexPositionFromFile():
        kwargs.setdefault("VertexShifters"          , [ 'ISF_VertexPositionFromFile' ])
    else:
        # TODO At this point there should be the option of using the
        # ISF_LongBeamspotVertexPositioner too.
        kwargs.setdefault("VertexShifters"          , [ 'ISF_VertexBeamCondPositioner' ])
    return CfgMgr.ISF__GenEventVertexPositioner(name, **kwargs)

def getGenEventBeamEffectBooster(name="ISF_GenEventBeamEffectBooster", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if not simFlags.RandomSeedList.checkForExistingSeed("BEAM"):
        simFlags.RandomSeedList.addSeed( "BEAM", 3499598793, 7345291 )
    return CfgMgr.ISF__GenEventBeamEffectBooster(name, **kwargs)

#--------------------------------------------------------------------------------------------------
## GenParticleFilters

def getParticleFinalStateFilter(name="ISF_ParticleFinalStateFilter", **kwargs):
    # ParticleFinalStateFilter
    # use CheckGenInteracting==False to allow GenEvent neutrinos to propagate into the simulation
    #kwargs.setdefault("CheckGenInteracting"     , False )
    return CfgMgr.ISF__GenParticleFinalStateFilter(name, **kwargs)

def getParticleSimWhiteList(name="ISF_ParticleSimWhiteList", **kwargs):
    # GenParticleSimWhiteList
    return CfgMgr.ISF__GenParticleSimWhiteList(name, **kwargs)

def getParticlePositionFilter(name="ISF_ParticlePositionFilter", **kwargs):
    # ParticlePositionFilter
    kwargs.setdefault('GeoIDService' , 'ISF_GeoIDSvc'    )
    return CfgMgr.ISF__GenParticlePositionFilter(name, **kwargs)

def getParticlePositionFilterID(name="ISF_ParticlePositionFilterID", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr

    kwargs.setdefault('CheckRegion'  , [ AtlasRegion.fAtlasID ] )
    return getParticlePositionFilter(name, **kwargs)

def getParticlePositionFilterCalo(name="ISF_ParticlePositionFilterCalo", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr

    kwargs.setdefault('CheckRegion'  , [ AtlasRegion.fAtlasID,
                                            AtlasRegion.fAtlasForward,
                                            AtlasRegion.fAtlasCalo ] )
    return getParticlePositionFilter(name, **kwargs)

def getParticlePositionFilterMS(name="ISF_ParticlePositionFilterMS", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr

    kwargs.setdefault('CheckRegion'  , [ AtlasRegion.fAtlasID,
                                            AtlasRegion.fAtlasForward,
                                            AtlasRegion.fAtlasCalo,
                                            AtlasRegion.fAtlasMS ] )
    return getParticlePositionFilter(name, **kwargs)

def getParticlePositionFilterWorld(name="ISF_ParticlePositionFilterWorld", **kwargs):
    # importing Reflex dictionary to access AtlasDetDescr::AtlasRegion enum
    import ROOT, cppyy
    cppyy.loadDictionary('AtlasDetDescrDict')
    AtlasRegion = ROOT.AtlasDetDescr
    kwargs.setdefault('CheckRegion'  , [ AtlasRegion.fAtlasID,
                                            AtlasRegion.fAtlasForward,
                                            AtlasRegion.fAtlasCalo,
                                            AtlasRegion.fAtlasMS,
                                            AtlasRegion.fAtlasCavern ] )
    return getParticlePositionFilter(name, **kwargs)

def getParticlePositionFilterDynamic(name="ISF_ParticlePositionFilterDynamic", **kwargs):
    # automatically choose the best fitting filter region
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.Muon_on():
      return getParticlePositionFilterWorld(name, **kwargs)
    elif DetFlags.Calo_on():
      return getParticlePositionFilterCalo(name, **kwargs)
    elif DetFlags.ID_on():
      return getParticlePositionFilterID(name, **kwargs)
    else:
      return getParticlePositionFilterWorld(name, **kwargs)

def getGenParticleInteractingFilter(name="ISF_GenParticleInteractingFilter", **kwargs):
    return CfgMgr.ISF__GenParticleInteractingFilter(name, **kwargs)

def getEtaPhiFilter(name="ISF_EtaPhiFilter", **kwargs):
    # EtaPhiFilter
    from AthenaCommon.DetFlags import DetFlags
    EtaRange = 7.0 if DetFlags.geometry.Lucid_on() else 6.0
    kwargs.setdefault('MinEta' , -EtaRange)
    kwargs.setdefault('MaxEta' , EtaRange)
    #kwargs.setdefault('MinPhi' , -M_PI)
    #kwargs.setdefault('MaxPhi' , M_PI)
    #kwargs.setdefault('MinMom' , -1)
    #kwargs.setdefault('MaxMom' , -1)
    #kwargs.setdefault('ParticlePDG' , [ 211, -211 ])
    return CfgMgr.ISF__GenParticleGenericFilter(name, **kwargs)

#--------------------------------------------------------------------------------------------------
## Stack Fillers

def getLongLivedStackFiller(name="ISF_LongLivedStackFiller", **kwargs):
    kwargs.setdefault("GenParticleFilters"      , [ 'ISF_ParticleSimWhiteList',
                                                    'ISF_ParticlePositionFilterDynamic',
                                                    'ISF_EtaPhiFilter',
                                                    'ISF_GenParticleInteractingFilter', ] )
    return getStackFiller(name, **kwargs)


def getStackFiller(name="ISF_StackFiller", **kwargs):
    kwargs.setdefault("InputMcEventCollection"                          , 'GEN_EVENT'  )
    kwargs.setdefault("OutputMcEventCollection"                         , 'TruthEvent' )
    kwargs.setdefault("PurgeOutputCollectionToSimulatedParticlesOnly"   , False        )
    kwargs.setdefault("UseGeneratedParticleMass"                        , False        )
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() == "cosmics":
        kwargs.setdefault("GenEventManipulators"                        , [
                                                                           'ISF_GenEventValidityChecker',
                                                                          ])
        kwargs.setdefault("GenParticleFilters"                          , [
                                                                           'ISF_ParticleFinalStateFilter',
                                                                           'ISF_GenParticleInteractingFilter',
                                                                          ])
    else:
        kwargs.setdefault("GenEventManipulators"                        , [
                                                                           'ISF_GenEventValidityChecker',
                                                                           'ISF_GenEventVertexPositioner',
                                                                          ])
        kwargs.setdefault("GenParticleFilters"                          , [
                                                                           'ISF_ParticleFinalStateFilter',
                                                                           'ISF_ParticlePositionFilterDynamic',
                                                                           'ISF_EtaPhiFilter',
                                                                           'ISF_GenParticleInteractingFilter',
                                                                          ])
    from ISF_Config.ISF_jobProperties import ISF_Flags
    kwargs.setdefault("BarcodeService"                                  , ISF_Flags.BarcodeService() )
    return CfgMgr.ISF__GenEventStackFiller(name, **kwargs)

#--------------------------------------------------------------------------------------------------
## Truth Strategies

# Brems: fBremsstrahlung (3)
# Conversion: fGammaConversion (14), fGammaConversionToMuMu (15), fPairProdByCharged (4)
# Decay: 201-298, fAnnihilation(5), fAnnihilationToMuMu (6), fAnnihilationToHadrons (7)
# Ionization: fIonisation (2), fPhotoElectricEffect (12)
# Hadronic: (111,121,131,141,151,161,210)
# Compton: fComptonScattering (13)
# G4 process types:
#  http://www-geant4.kek.jp/lxr/source//processes/management/include/G4ProcessType.hh
# G4 EM sub types:
#  http://www-geant4.kek.jp/lxr/source//processes/electromagnetic/utils/include/G4EmProcessSubType.hh
# G4 HadInt sub types:
#  http://www-geant4.kek.jp/lxr/source//processes/hadronic/management/include/G4HadronicProcessType.hh#L46
def getTruthStrategyGroupID_MC15(name="ISF_MCTruthStrategyGroupID_MC15", **kwargs):
    kwargs.setdefault('ParentMinPt'         , 100.*MeV)
    kwargs.setdefault('ChildMinPt'          , 300.*MeV)
    kwargs.setdefault('VertexTypes'         , [ 3, 14, 15, 4, 5, 6, 7, 2, 12, 13 ])
    kwargs.setdefault('VertexTypeRangeLow'  , 201)  # All kinds of decay processes
    kwargs.setdefault('VertexTypeRangeHigh' , 298)  # ...
    return CfgMgr.ISF__GenericTruthStrategy(name, **kwargs);


def getTruthStrategyGroupID(name="ISF_MCTruthStrategyGroupID", **kwargs):
    kwargs.setdefault('ParentMinPt'         , 100.*MeV)
    kwargs.setdefault('ChildMinPt'          , 100.*MeV)
    kwargs.setdefault('VertexTypes'         , [ 3, 14, 15, 4, 5, 6, 7, 2, 12, 13 ])
    kwargs.setdefault('VertexTypeRangeLow'  , 201)  # All kinds of decay processes
    kwargs.setdefault('VertexTypeRangeHigh' , 298)  # ...
    return CfgMgr.ISF__GenericTruthStrategy(name, **kwargs);


def getTruthStrategyGroupIDHadInt_MC15(name="ISF_MCTruthStrategyGroupIDHadInt_MC15", **kwargs):
    kwargs.setdefault('ParentMinPt'                       , 100.*MeV)
    kwargs.setdefault('ChildMinPt'                        , 300.*MeV)
    kwargs.setdefault('VertexTypes'                       , [ 111, 121, 131, 141, 151, 161, 210 ])
    kwargs.setdefault('AllowChildrenOrParentPassKineticCuts' , True)
    return CfgMgr.ISF__GenericTruthStrategy(name, **kwargs);


def getTruthStrategyGroupIDHadInt(name="ISF_MCTruthStrategyGroupIDHadInt", **kwargs):
    kwargs.setdefault('ParentMinPt'                       , 100.*MeV)
    kwargs.setdefault('ChildMinPt'                        , 100.*MeV)
    kwargs.setdefault('VertexTypes'                       , [ 111, 121, 131, 141, 151, 161, 210 ])
    kwargs.setdefault('AllowChildrenOrParentPassKineticCuts' , True)
    return CfgMgr.ISF__GenericTruthStrategy(name, **kwargs);


def getTruthStrategyGroupCaloMuBrem_MC15(name="ISF_MCTruthStrategyGroupCaloMuBrem_MC15", **kwargs):
    kwargs.setdefault('ParentMinEkin'       , 500.*MeV)
    kwargs.setdefault('ChildMinEkin'        , 300.*MeV)
    kwargs.setdefault('VertexTypes'         , [ 3 ])
    kwargs.setdefault('ParentPDGCodes'      , [ 13, -13 ])
    return CfgMgr.ISF__GenericTruthStrategy(name, **kwargs);

def getTruthStrategyGroupCaloMuBrem(name="ISF_MCTruthStrategyGroupCaloMuBrem", **kwargs):
    kwargs.setdefault('ParentMinEkin'       , 500.*MeV)
    kwargs.setdefault('ChildMinEkin'        , 100.*MeV)
    kwargs.setdefault('VertexTypes'         , [ 3 ])
    kwargs.setdefault('ParentPDGCodes'      , [ 13, -13 ])
    return CfgMgr.ISF__GenericTruthStrategy(name, **kwargs);

def getTruthStrategyGroupCaloDecay_MC15(name="ISF_MCTruthStrategyGroupCaloDecay_MC15", **kwargs):
    kwargs.setdefault('ParentMinEkin'       , 1000.*MeV)
    kwargs.setdefault('ChildMinEkin'        , 500.*MeV)
    kwargs.setdefault('VertexTypes'         , [ 5, 6, 7 ])
    kwargs.setdefault('VertexTypeRangeLow'  , 201)  # All kinds of decay processes
    kwargs.setdefault('VertexTypeRangeHigh' , 298)  # ...
    return CfgMgr.ISF__GenericTruthStrategy(name, **kwargs);

def getTruthStrategyGroupCaloDecay(name="ISF_MCTruthStrategyGroupCaloDecay", **kwargs):
    kwargs.setdefault('ParentMinPt'         , 1000.*MeV)
    kwargs.setdefault('ChildMinPt'          , 500.*MeV)
    kwargs.setdefault('VertexTypes'         , [ 5, 6, 7 ])
    kwargs.setdefault('VertexTypeRangeLow'  , 201)  # All kinds of decay processes
    kwargs.setdefault('VertexTypeRangeHigh' , 298)  # ...
    return CfgMgr.ISF__GenericTruthStrategy(name, **kwargs);

def getValidationTruthStrategy(name="ISF_ValidationTruthStrategy", **kwargs):
    kwargs.setdefault('ParentMinP'          , 50.*MeV)
    return CfgMgr.ISF__ValidationTruthStrategy(name, **kwargs);

def getLLPTruthStrategy(name="ISF_LLPTruthStrategy", **kwargs):
    kwargs.setdefault('PassProcessCodeRangeLow',  200 )
    kwargs.setdefault('PassProcessCodeRangeHigh', 299 )
    # ProcessCategory==9 corresponds to the 'fUserDefined' G4ProcessType:
    #   http://www-geant4.kek.jp/lxr/source//processes/management/include/G4ProcessType.hh
    kwargs.setdefault('PassProcessCategory',      9   ) # ==
    return CfgMgr.ISF__LLPTruthStrategy(name, **kwargs);
