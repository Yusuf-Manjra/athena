#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

##########################################################################################
# Helper functions to digest the reconstruction options dictionary
# and translate it into the python configuration objects used by
# jet reco code.

from JetRecConfig.JetDefinition import JetConstit, xAODType, JetDefinition
from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Jet.JetRecoConfiguration")

# Extract the jet reco dict from the chainDict
def extractRecoDict(chainParts):
    # interpret the reco configuration only
    # eventually should just be a subdict in the chainDict
    recoKeys = ['recoAlg','dataType','calib','jetCalib','trkopt','cleaning']
    recoDict = {}
    for p in chainParts:
        for k in recoKeys:
            # Look for our key in the chain part
            if k in p.keys():
                # found the key, check for consistency with other chain parts of this chain
                if k in recoDict.keys():
                    if p[k] != recoDict[k]:
                        log.error('Inconsistent reco setting for' + k)
                        exit(1)
                # copy this entry to the reco dictionary
                recoDict[k] = p[k]

    return recoDict

# Define the jet constituents to be interpreted by JetRecConfig
# When actually specifying the reco, clustersKey should be
# set, but default to None to allow certain checks, in particular
# grooming configuration
def defineJetConstit(jetRecoDict,clustersKey=None,pfoPrefix=None):
    constitMods = []
    if "sk" in jetRecoDict["dataType"]:
        constitMods.append("SK")

    # Get the details of the constituent definition:
    # type, mods and the input container name
    if jetRecoDict["dataType"]=="pf":
        jetConstit = JetConstit( xAODType.ParticleFlow, constitMods)
        if pfoPrefix is None:
            raise RuntimeError("JetRecoConfiguration: Cannot define PF jets without pfo prefix!")
        jetConstit.rawname = pfoPrefix+"ParticleFlowObjects"
        jetConstit.inputname = pfoPrefix+"CHSParticleFlowObjects"
        
    if "tc" in jetRecoDict["dataType"]:
        # apply this scale
        if jetRecoDict["calib"] == "em":
            constitMods = ["EM"] + constitMods
        elif jetRecoDict["calib"] == "lcw":
            constitMods = ["LC"] + constitMods
        # read from this cluster collection,
        # overriding the standard offline collection
        jetConstit = JetConstit( xAODType.CaloCluster, constitMods)
        if clustersKey is not None:
            jetConstit.rawname = clustersKey
            if jetRecoDict["dataType"]=="tc":
                jetConstit.inputname = clustersKey
        # apply constituent pileup suppression
        if "cs" in jetRecoDict["dataType"]:
            constitMods.append("CS")
        if "sk" in jetRecoDict["dataType"]:
            constitMods.append("SK")
    return jetConstit

# Arbitrary min pt for fastjet, set to be low enough for MHT(?)
# Could/should adjust higher for large-R
def defineJets(jetRecoDict,clustersKey=None,pfoPrefix=None):
    radius = float(jetRecoDict["recoAlg"].lstrip("a").rstrip("tr"))/10
    jetConstit = defineJetConstit(jetRecoDict,clustersKey,pfoPrefix)
    jetDef = JetDefinition( "AntiKt", radius, jetConstit, ptmin=5000.)
    return jetDef

def defineReclusteredJets(jetRecoDict):
    rcJetConstit = JetConstit( xAODType.Jet, [])
    rcJetDef = JetDefinition( "AntiKt", 1.0, rcJetConstit)
    return rcJetDef

def defineGroomedJets(jetRecoDict,ungroomedDef,ungroomedJetsName):
    from JetRecConfig.JetGrooming import JetTrimming
    # Only actually one type now, but leave open possibility of others
    groomDef = {
        "t":JetTrimming(ungroomedDef,ungroomedJetsName,smallR=0.2,ptfrac=0.05)
        }[jetRecoDict["recoAlg"][-1]]
    return groomDef

##########################################################################################
# Generation of modifier lists. So far only calib, but can add track, substructure mods

from JetRecConfig.StandardJetMods import jetmoddict

# Make generating the list a bit more comprehensible
def getModSpec(modname,modspec=''):
    return (jetmoddict[modname],str(modspec))

def defineTrackMods(trkopt):
    trkmods = [
        (jetmoddict["TrackMoments"],trkopt),
        (jetmoddict["JVF"],trkopt),
        (jetmoddict["JVT"],trkopt)
    ]
    return trkmods

# Translate calib specification into something understood by
# the calibration config helper
def defineCalibFilterMods(jetRecoDict,dataSource,rhoKey="auto"):
    # Minimum modifier set for calibration w/o track GSC
    # Should eventually build in more mods, depend on track info etc
    jetalg = jetRecoDict["recoAlg"]
    if jetRecoDict["jetCalib"] == "nojcalib" or jetalg=="a10r":
        calibMods = []
    else:
        if jetRecoDict["trkopt"]=="notrk" and "gsc" in jetRecoDict["jetCalib"]:
            raise ValueError("Track GSC requested but no track source provided!")

        if jetRecoDict["trkopt"]=="notrk" and "subres" in jetRecoDict["jetCalib"]:
            raise ValueError("Pileup residual calibration requested but no track source provided!")

        if jetRecoDict["dataType"]=="tc":
            calibContext,calibSeq = {
                ("a4","subjes"):   ("TrigRun2","JetArea_EtaJES_GSC"),        # Calo GSC only
                ("a4","subjesIS"): ("TrigRun2","JetArea_EtaJES_GSC_Insitu"), # Calo GSC only
                ("a4","subjesgscIS"): ("TrigRun2GSC","JetArea_EtaJES_GSC_Insitu"), # Calo+Trk GSC
                ("a4","subresjesgscIS"): ("TrigRun2GSC","JetArea_Residual_EtaJES_GSC_Insitu"), # pu residual + calo+trk GSC
                ("a10","subjes"):  ("TrigUngroomed","JetArea_EtaJES"),
                ("a10t","jes"):    ("TrigTrimmed","EtaJES_JMS"),
                }[(jetRecoDict["recoAlg"],jetRecoDict["jetCalib"])]

            pvname = ""
            gscDepth = "EM3"
            if "gsc" in jetRecoDict["jetCalib"]:
                gscDepth = "trackWIDTH"
                pvname = "HLT_EFHistoPrmVtx"

        elif jetRecoDict["dataType"]=="pf":
            gscDepth = "auto"
            calibContext = "TrigLS2"
            calibSeq = "JetArea_Residual_EtaJES_GSC"
            if jetRecoDict["jetCalib"].endswith("IS"):
                calibSeq += "_Insitu"
            pvname = "HLT_EFHistoPrmVtx"

        calibSpec = ":".join( [calibContext, dataSource, calibSeq, rhoKey, pvname, gscDepth] )
        from .TriggerJetMods import ConstitFourMom_copy
        if jetalg=="a4":
            calibMods = [(ConstitFourMom_copy,""),
                         getModSpec("CaloEnergies"), # Needed for GSC
                         getModSpec("Calib",calibSpec),
                         getModSpec("Sort")]
        else:
            calibMods = [(ConstitFourMom_copy,""),
                         getModSpec("Calib",calibSpec),
                         getModSpec("Sort")]

    filtercut = {"a4":5000, "a10":50000, "a10r": 50000, "a10t":100000}[jetalg]
    return calibMods + [getModSpec("Filter",filtercut)]
