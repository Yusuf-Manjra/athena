# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from JetRecConfig.JetDefinition import JetModifier

########################################################################
# Special modifier setups used by jet trigger, but not in offline
# We just extend the jetmoddict from the standard config.

from JetRecConfig.StandardJetMods import jetmoddict
jetmoddict.update(

    # No need for the special momentum scales, just copy the basic four-vec
    # to "DetectorEtaPhi", because we're not doing origin correction
    # and certainly not with the offline collection names    
    ConstitFourMom_copy  = JetModifier("JetConstitFourMomTool", "constitfourmom_copy",
                                       JetScaleNames = ["DetectorEtaPhi"],
                                       AltConstitColls = [""],
                                       AltConstitScales = [0],
                                       AltJetScales = ["JetConstitScaleMomentum"]                                       
                                       )
)
