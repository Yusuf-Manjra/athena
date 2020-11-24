# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# ParticleJetToolsConfig: A helper module for configuring tools for    #
# truth jet reconstruction and classification                          #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################

from AthenaCommon import Logging
jrtlog = Logging.logging.getLogger('ParticleJetToolsConfig')

from AthenaConfiguration.ComponentFactory import CompFactory

# Putting MCTruthClassifier here as we needn't stick jet configs in really foreign packages
def getMCTruthClassifier():
    # Assume mc15 value
    firstSimCreatedBarcode = 200000
    truthclassif = CompFactory.MCTruthClassifier(
        "JetMCTruthClassifier",
        barcodeG4Shift=firstSimCreatedBarcode,
        xAODTruthLinkVector=""
        )
    # Config neessary only for Athena releases
    import os
    if "AtlasProject" in os.environ.keys():
        if os.environ["AtlasProject"] in ["Athena","AthDerivation"]:
            truthclassif.ParticleCaloExtensionTool=""
    return truthclassif

# Generates truth particle containers for truth labeling
truthpartoptions = {
    "Partons":{"ToolType":CompFactory.CopyTruthPartons,"ptmin":5000},
    "BosonTop":{"ToolType":CompFactory.CopyBosonTopLabelTruthParticles,"ptmin":100000},
    "FlavourLabel":{"ToolType":CompFactory.CopyFlavorLabelTruthParticles,"ptmin":5000},
}
def getCopyTruthLabelParticles(truthtype):
    toolProperties = {}
    if truthtype == "Partons":
        truthcategory = "Partons"
    elif truthtype in ["WBosons", "ZBosons", "HBosons", "TQuarksFinal"]:
        truthcategory = "BosonTop"
        toolProperties['ParticleType'] = truthtype
    else:
        truthcategory = "FlavourLabel"
        toolProperties['ParticleType'] = truthtype
        
    tooltype = truthpartoptions[truthcategory]["ToolType"]
    toolProperties.update( PtMin = truthpartoptions[truthcategory]["ptmin"],
                           OutputName = "TruthLabel"+truthtype)
    ctp = tooltype("truthpartcopy_"+truthtype,
                   **toolProperties
                   )
    return ctp

# Generates input truth particle containers for truth jets
def getCopyTruthJetParticles(modspec):
    truthclassif = getMCTruthClassifier()

    # Commented for now -- we ideally need a dual-use input file peeker
    # # Input file is EVNT
    # if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
    #    barCodeFromMetadata=0

    truthpartcopy = CompFactory.CopyTruthJetParticles(
        "truthpartcopy"+modspec,
        OutputName="JetInputTruthParticles"+modspec,
        MCTruthClassifier=truthclassif,
        BarCodeFromMetadata=2)
    if modspec=="NoWZ":
        truthpartcopy.IncludePromptLeptons=False
        # truthpartcopy.IncludePromptPhotons=False # Needs cherry-pick from 21.2
        truthpartcopy.IncludeMuons=True
        truthpartcopy.IncludeNeutrinos=True
    return truthpartcopy

def getJetQuarkLabel():
    jetquarklabel = CompFactory.Analysis.JetQuarkLabel(
        "jetquarklabel",
        McEventCollection = "TruthEvents"
        )
    return jetquarklabel

def getJetConeLabeling():
    jetquarklabel = getJetQuarkLabel()
    truthpartonlabel = CompFactory.Analysis.JetConeLabeling(
        "truthpartondr",
        JetTruthMatchTool = jetquarklabel
        )
    return truthpartonlabel

def getJetDeltaRLabelTool(jetdef, modspec):
    """returns a ParticleJetDeltaRLabelTool 
    Cone matching for B, C and tau truth for all but track jets.

    This function is meant to be used as callback from JetRecConfig where 
    it is called as func(jetdef, modspec). Hence the jetdef argument even if not used in this case.
    """
    jetptmin = float(modspec)
    jetdrlabeler = CompFactory.ParticleJetDeltaRLabelTool(
        "jetdrlabeler_jetpt{0}GeV".format(int(jetptmin/1000)),
        LabelName = "HadronConeExclTruthLabelID",
        DoubleLabelName = "HadronConeExclExtendedTruthLabelID",
        BLabelName = "ConeExclBHadronsFinal",
        CLabelName = "ConeExclCHadronsFinal",
        TauLabelName = "ConeExclTausFinal",
        BParticleCollection = "TruthLabelBHadronsFinal",
        CParticleCollection = "TruthLabelCHadronsFinal",
        TauParticleCollection = "TruthLabelTausFinal",
        PartPtMin = 5000.,
        DRMax = 0.3,
        MatchMode = "MinDR"
        )
    jetdrlabeler.JetPtMin = jetptmin
    return jetdrlabeler

