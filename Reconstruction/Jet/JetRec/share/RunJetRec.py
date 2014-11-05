# RunJetRec.py
#
# David Adams
# February 2014
#
# Job options for running jet reconstruction.
#
# Run with
# > athena.py test_RunJetRec.py
#

runJetGrooming = True

# Import the jet reconstruction control flags.
from JetRec.JetRecFlags import jetFlags

# Import the standard jet tool manager.
from JetRec.JetRecStandard import jtm

# Set this true to test LAr bad HV fractions with data.
useLArHVCorr = False

#--------------------------------------------------------------
# Define the finders and groomers.
# Each line configures a finder or groomer and its associated jetrec tool.
# The first argument is the name of the output collection and the jetrec tool.
# A fifth argument may be added to change the list of modifiers.
#--------------------------------------------------------------

# Update the modier lists.
jtm.modifiersMap["calib"] += [jtm.isolation]
if useLArHVCorr:
  jtm.modifiersMap["calib"] += [jtm.larhvcorr]
jtm.modifiersMap["mycalib"] = jtm.modifiersMap["calib"] + [jtm.pull, jtm.charge, jtm.showerdec]
print "%%%%%%%"
print jtm.modifiersMap["mycalib"]
print "%%%%%%%"

# Finders.
# Calibration for topo jets: calibOpt =
#   a - area corrn
#   r - residual pileup corrn
#   j - GES
#   g - GSC
#   i - Insitu
gatopo  = 0.01
gatruth = 0.00
gatrack = 0.00
if jetFlags.useTruth:
  jtm.addJetFinder("Run2AntiKt4TruthJets",    "AntiKt", 0.4,    "truth", ghostArea=gatruth, ptmin= 5000)
  jtm.addJetFinder("Run2AntiKt4TruthWZJets",  "AntiKt", 0.4,  "truthwz", ghostArea=gatruth, ptmin= 5000)
  jtm.addJetFinder("Run2AntiKt10TruthJets",   "AntiKt", 0.4,    "truth", ghostArea=gatruth, ptmin=40000)
  jtm.addJetFinder("Run2AntiKt10TruthWZJets", "AntiKt", 0.4,    "truth", ghostArea=gatruth, ptmin=40000)
  jtm.addJetFinder("Run2CamKt12TruthJets",     "CamKt", 0.4,    "truth", ghostArea=gatruth, ptmin=40000)
  jtm.addJetFinder("Run2CamKt12TruthWZJets",   "CamKt", 0.4,    "truth", ghostArea=gatruth, ptmin=40000)
if jetFlags.useTracks:
  jtm.addJetFinder("Run2AntiKt3PV0TrackJets", "AntiKt", 0.3, "pv0track", ghostArea=gatrack, ptmin= 2000)
  jtm.addJetFinder("Run2AntiKt4TrackJets",    "AntiKt", 0.4,    "track", ghostArea=gatrack, ptmin= 2000)
  jtm.addJetFinder("Run2AntiKt4ZTrackJets",   "AntiKt", 0.4,   "ztrack", ghostArea=gatrack, ptmin= 2000)
  jtm.addJetFinder("Run2AntiKt4PV0TrackJets", "AntiKt", 0.4, "pv0track", ghostArea=gatrack, ptmin= 2000)
jtm.addJetFinder(  "Run2AntiKt4EMTopoJets",   "AntiKt", 0.4,   "emtopo", "mycalib", ptmin=2000, ptminFilter= 7000, ghostArea=gatopo, calibOpt="arj")
jtm.addJetFinder(  "Run2AntiKt4LCTopoJets",   "AntiKt", 0.4,   "lctopo", "mycalib", ptmin=2000, ptminFilter= 7000, ghostArea=gatopo, calibOpt="ar")
jtm.addJetFinder(  "Run2AntiKt10LCTopoJets",  "AntiKt", 1.0,   "lctopo", "mycalib", ptmin=2000, ptminFilter=50000, ghostArea=gatopo, calibOpt="a")
jtm.addJetFinder(  "Run2CamKt12LCTopoJets",    "CamKt", 1.2,   "lctopo", "mycalib", ptmin=2000, ptminFilter=50000, ghostArea=gatopo, calibOpt="a")

# Groomers.
if runJetGrooming:
  jtm.addJetSplitter(   "Run2Split040CamKt12LCTopoJets",  1.0,  0.04, "Run2CamKt12LCTopoJets")
  jtm.addJetTrimmer(     "Run2TrimR3AntiKt10LCTopoJets",  0.3,  0.05, "Run2AntiKt10LCTopoJets")
  jtm.addJetPruner( "Run2PruneR30Z10AntiKt10LCTopoJets",  0.30, 0.10, "Run2AntiKt10LCTopoJets")

#--------------------------------------------------------------
# Configure the jet algorithm.
#--------------------------------------------------------------

from JetRec.JetAlgorithm import jetalg
