# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# JetRecStandardToolManager.py
#
# David Adams
# March 2014
#
# Constructs an instance jtm of the JetToolManager with
# standard definitions for jet tools.
#
# Usage:
#   from JetRec.JetRecStandardToolManager import jtm
#   jtm.addJetFinder("AntiKt4EMTopoJets", "AntiKt", 0.4, "em", "calib_emtopo_ungroomed")
#
from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetRec_jobOptions')

myname = "JetRecStandardToolManager.py: "

jetlog.info( myname + "Defining standard tools" )

#########################################################
# Set and lock flags.
#########################################################

from JetRec.JetRecFlags import jetFlags, JetContentDetail

# Set status on for flags that are initialized with status off.
jetFlags.useTruth.set_On()
jetFlags.useTopo.set_On()
jetFlags.useTracks.set_On()
jetFlags.usePFlow.set_On()
jetFlags.useMuonSegments.set_On()
jetFlags.useBTagging.set_On()

from InDetRecExample.InDetJobProperties import InDetFlags
if not InDetFlags.doR3LargeD0():
  jetFlags.useLargeD0Tracks.set_Off()


# Lock all the flags used here so that later attempts to change
# the value will fail with an error message.
jetFlags.useTruth.lock()
jetFlags.useTopo.lock()
jetFlags.useTracks.lock()
jetFlags.useMuonSegments.lock()
jetFlags.useBTagging.lock()
jetFlags.useCaloQualityTool.lock()
jetFlags.additionalTopoGetters.lock()
jetFlags.truthFlavorTags.lock()
jetFlags.skipTools.lock()

# Display all flags used here.
jetlog.info( myname + "jetFlags.useTruth: " + str(jetFlags.useTruth()) )
jetlog.info( myname + "jetFlags.useTopo: " + str(jetFlags.useTopo()) )
jetlog.info( myname + "jetFlags.useTracks: " + str(jetFlags.useTracks()) )
jetlog.info( myname + "jetFlags.useMuonSegments: " + str(jetFlags.useMuonSegments()) )
jetlog.info( myname + "jetFlags.useBTagging: " + str(jetFlags.useBTagging()) )
jetlog.info( myname + "jetFlags.useCaloQualityTool: " + str(jetFlags.useCaloQualityTool()) )
jetlog.info( myname + "jetFlags.additionalTopoGetters: " + str(jetFlags.additionalTopoGetters()) )
jetlog.info( myname + "jetFlags.truthFlavorTags: " + str(jetFlags.truthFlavorTags()) )
jetlog.info( myname + "jetFlags.skipTools: " + str(jetFlags.skipTools()) )
jetlog.info( myname + "jetFlags.useTrackVertexTool: " + str(jetFlags.useTrackVertexTool()) )

#########################################################
# Create standard tool manager.
#########################################################

# Import the jet tool manager.
from JetRec.JetToolSupport import JetToolManager

# Global jet tool manager with standard definitions
jtm = JetToolManager()

# Pt thresholds in MeV
jtm.ptminFinder = 2000
jtm.ptminFilter =    0

# Add standard tool definitions to the tool manager.
import JetRec.JetRecStandardTools as JetRecStandardTools
jetlog.verbose("Use trigger store? %s", JetRecStandardTools.UseTriggerStore)

# Function to filter out skipped tools.
def filterout(skiptoolnames, tools):
  outtools = []
  remtoolnames = []
  for tool in tools:
    keep = True
    for toolname in skiptoolnames:
      skiptool = jtm[toolname]
      same = tool == skiptool
      if same:
        keep = False
        remtoolnames += [toolname]
    if keep:
      outtools += [tool]
  jetlog.info( myname + "Removed tools: " + str(remtoolnames) )
  return outtools

#########################################################
# Getters
#########################################################

# Pseudojet getters
empfgetters =  [jtm.empflowget]

trackgetters = [jtm.trackget]
pv0trackgetters = [jtm.pv0trackget]
# Add track ghosts
emgetters = [jtm.emget]
lcgetters = [jtm.lcget]
if jetFlags.useTracks():
  if jetFlags.useVertices():
    emgetters = [jtm.emoriginget]
    lcgetters = [jtm.lcoriginget]
  else:
    emgetters = [jtm.emget]
    lcgetters = [jtm.lcget]
  emgetters += [jtm.gtrackget]
  lcgetters += [jtm.gtrackget]
  empfgetters += [jtm.gtrackget]

if jetFlags.useLargeD0Tracks():
  emgetters += [jtm.gtracklrtget]

if jetFlags.useMuonSegments():
  emgetters += [jtm.gmusegget]
  lcgetters += [jtm.gmusegget]
  empfgetters  += [jtm.gmusegget]
# Add jet ghosts.
if 1:
  for gettername in jetFlags.additionalTopoGetters():
    getter = jtm[gettername]
    emgetters += [getter]
    lcgetters += [getter]
# Add truth getter and truth ghosts.
if jetFlags.useTruth():
  truthgetters = [jtm.truthget]
  truthwzgetters = [jtm.truthwzget]
  trackgetters += [jtm.gtruthget]
  pv0trackgetters += [jtm.gtruthget]
  emgetters += [jtm.gtruthget]
  lcgetters += [jtm.gtruthget]
  empfgetters += [jtm.gtruthget]
  # Add truth cone matching and truth flavor ghosts.
  flavorgetters = []
  for ptype in jetFlags.truthFlavorTags():
    flavorgetters += [getattr(jtm, "gtruthget_" + ptype)]
  emgetters      += flavorgetters
  lcgetters      += flavorgetters
  truthgetters   += flavorgetters
  truthwzgetters += flavorgetters
  trackgetters   += flavorgetters
  pv0trackgetters += flavorgetters
  empfgetters    += flavorgetters
# Add track jet ghosts.
if jetFlags.useTracks():
  trackjetgetters = []
  trackjetgetters += [jtm.gakt2trackget]
#  trackjetgetters += [jtm.gakt3trackget]
  trackjetgetters += [jtm.gakt4trackget]
#  trackjetgetters += [jtm.gvrtrackget]
  emgetters += trackjetgetters
  lcgetters += trackjetgetters
  empfgetters += trackjetgetters

empfgetters += [jtm.gtowerget]

# Add getter lists to jtm indexed by input type name.
jtm.gettersMap["emtopo"]    = list(emgetters)
jtm.gettersMap["lctopo"]    = list(lcgetters)
jtm.gettersMap["empflow"]   = list(empfgetters)
jtm.gettersMap["track"]     = list(trackgetters)
jtm.gettersMap["pv0track"]  = list(pv0trackgetters)
if jetFlags.useTruth():
  jtm.gettersMap["truth"]   = list(truthgetters)
  jtm.gettersMap["truthwz"] = list(truthwzgetters)

jtm.gettersMap["emtopo_reduced"]  = filterout(["gakt2trackget","gakt4trackget"],emgetters)
jtm.gettersMap["lctopo_reduced"]  = filterout(["gakt2trackget","gakt4trackget"],lcgetters)
jtm.gettersMap["empflow_reduced"] = filterout(["gakt2trackget","gakt4trackget"],empfgetters)


#########################################################
# Modifiers
#########################################################

# Modifiers for ungroomed jets from all input types.
common_ungroomed_modifiers = [
  jtm.width,
]

# Add parton truth labels and truth jet modifiers.
if jetFlags.useTruth():
  if jtm.haveParticleJetTools:
    common_ungroomed_modifiers += [jtm.partontruthlabel]
    common_ungroomed_modifiers += [jtm.truthpartondr]

  # Modifiers for truth jets.
  truth_ungroomed_modifiers = list(common_ungroomed_modifiers)
  if jtm.haveParticleJetTools:
    truth_ungroomed_modifiers += [jtm.jetdrlabeler]

  # Modifiers for ungroomed truth large-R jets
  truth_ungroomed_larger_modifiers = truth_ungroomed_modifiers
  # Add splitting scale modifiers for truth labeling
  truth_ungroomed_larger_modifiers += [jtm.ktsplitter]

# Modifiers for track jets.
track_ungroomed_modifiers = list(common_ungroomed_modifiers)
if jetFlags.useTruth() and jtm.haveParticleJetTools:
  track_ungroomed_modifiers += [jtm.trackjetdrlabeler]

# Modifiers for calibrated topo jets.
# Modifiers for topo (and pflow) jets.
ungroomed_modifiers = [jtm.jetens, "calib", jtm.jetsorter] # calibration first. turn off with calibopt
ungroomed_modifiers += ["jetfilter"]
ungroomed_modifiers += common_ungroomed_modifiers
ungroomed_modifiers += [jtm.larhvcorr]
ungroomed_modifiers += [jtm.ecpsfrac]
if jetFlags.useCaloQualityTool():
  ungroomed_modifiers += [jtm.caloqual_cluster]
if jetFlags.useTracks():
  if jetFlags.useVertices():
    ungroomed_modifiers += [jtm.trkmoms]
    ungroomed_modifiers += [jtm.trksummoms]
    ungroomed_modifiers += [jtm.jvf]
    ungroomed_modifiers += [jtm.jvt]
    ungroomed_modifiers += [jtm.jetorigin_setpv]
  ungroomed_modifiers += [jtm.charge]
  ungroomed_modifiers += ["trackassoc"]
  
if jetFlags.useTruth():
  if jetFlags.detailLevel()>=JetContentDetail.Full:
    # only at this detail level are the truth jets build. We can then schedule the TruthAssociation calculation :
    ungroomed_modifiers += ["truthassoc"]
  if jtm.haveParticleJetTools:
    ungroomed_modifiers += [jtm.jetdrlabeler]

#if jetFlags.detailLevel>=JetContentDetail.Validation:
#  ungroomed_modifiers += [jtm.jvt]

# Modifiers for groomed jets.
groomed_modifiers = [
    jtm.jetsorter,
    jtm.nsubjettiness,
    jtm.ktdr,
    jtm.ktsplitter,
    jtm.encorr,
    jtm.energycorrelatorgeneralized,
    jtm.energycorrelatorgeneralizedratios,
    jtm.charge,
    jtm.angularity,
    jtm.comshapes,
    jtm.ktmassdrop,
    jtm.dipolarity,
    jtm.pull,
    jtm.planarflow,
    jtm.width,
    jtm.qw,
    jtm.trksummoms
    ]

# Modifiers for pflow jets.
# Same as topo jets.
# 28may2015 - ecpsfrac is not yet working for pflow in xAOD.
pflow_ungroomed_modifiers = []
pflow_ungroomed_modifiers += [jtm.constitfourmom_pflow]
pflow_ungroomed_modifiers += filterout(["ecpsfrac"], ungroomed_modifiers)

pflow_groomed_modifiers = []
pflow_groomed_modifiers += [jtm.constitfourmom_pflow]
pflow_groomed_modifiers += groomed_modifiers

# For truth jets, don't add track moments
truth_groomed_modifiers = filterout(["trksummoms"], groomed_modifiers)

# Here add tools to be run for topo jets and NOT for pflow.

# Cluster moments.
ungroomed_modifiers += [jtm.clsmoms]

# Voronoi moments.
#ungroomed_modifiers += [jtm.voromoms]

# Add Btagging.
btags = ["btag"]
if jetFlags.useBTagging():
  ungroomed_modifiers += btags

# EM-only modifiers here
emtopo_ungroomed_modifiers = []
emtopo_ungroomed_modifiers += [jtm.constitfourmom_emtopo]
emtopo_ungroomed_modifiers += ungroomed_modifiers

emtopo_groomed_modifiers = []
emtopo_groomed_modifiers += [jtm.constitfourmom_emtopo]
emtopo_groomed_modifiers += groomed_modifiers

# LC-only modifiers here
lctopo_ungroomed_modifiers = []
lctopo_ungroomed_modifiers += [jtm.constitfourmom_lctopo]
lctopo_ungroomed_modifiers += ungroomed_modifiers

lctopo_groomed_modifiers = []
lctopo_groomed_modifiers += [jtm.constitfourmom_lctopo]
lctopo_groomed_modifiers += groomed_modifiers



### XAOD SIZE REDUCTION
# define a reduced set for AOD size reduction
pflow_reduced_modifiers  = [jtm.constitfourmom_pflow,  jtm.jetens, "calib", jtm.jetsorter]
emtopo_reduced_modifiers = [jtm.constitfourmom_emtopo, jtm.jetens, "calib", jtm.jetsorter]
lctopo_reduced_modifiers = [jtm.constitfourmom_lctopo, jtm.jetens, "calib", jtm.jetsorter]



# Filter out skipped tools.
if len(jetFlags.skipTools()):
  jetlog.info( myname + "Tools to be skipped: " + str(jetFlags.skipTools()) )
  ungroomed_modifiers               = filterout(jetFlags.skipTools(), ungroomed_modifiers)
  if jetFlags.useTruth():
    truth_ungroomed_modifiers       = filterout(jetFlags.skipTools(), truth_ungroomed_modifiers)
  track_ungroomed_modifiers         = filterout(jetFlags.skipTools(), track_ungroomed_modifiers)
  groomed_modifiers                 = filterout(jetFlags.skipTools(), groomed_modifiers)
  pflow_ungroomed_modifiers         = filterout(jetFlags.skipTools(), pflow_ungroomed_modifiers)
  emtopo_ungroomed_modifiers        = filterout(jetFlags.skipTools(), emtopo_ungroomed_modifiers)
  lctopo_ungroomed_modifiers        = filterout(jetFlags.skipTools(), lctopo_ungroomed_modifiers)
  pflow_groomed_modifiers           = filterout(jetFlags.skipTools(), pflow_groomed_modifiers)
  emtopo_groomed_modifiers          = filterout(jetFlags.skipTools(), emtopo_groomed_modifiers)
  lctopo_groomed_modifiers          = filterout(jetFlags.skipTools(), lctopo_groomed_modifiers)

# Add modifier lists to jtm indexed by modifier type name.
jtm.modifiersMap["none"]                  = []
jtm.modifiersMap["ungroomed"]             =            list(ungroomed_modifiers)
jtm.modifiersMap["groomed"]               =              list(groomed_modifiers)

jtm.modifiersMap["emtopo_ungroomed"]      =     list(emtopo_ungroomed_modifiers)
jtm.modifiersMap["lctopo_ungroomed"]      =     list(lctopo_ungroomed_modifiers)
jtm.modifiersMap["pflow_ungroomed"]       =      list(pflow_ungroomed_modifiers)

jtm.modifiersMap["emtopo_groomed"]        =       list(emtopo_groomed_modifiers)
jtm.modifiersMap["lctopo_groomed"]        =       list(lctopo_groomed_modifiers)
jtm.modifiersMap["pflow_groomed"]         =        list(pflow_groomed_modifiers)

jtm.modifiersMap["emtopo_reduced"]        =       list(emtopo_reduced_modifiers)
jtm.modifiersMap["lctopo_reduced"]        =       list(lctopo_reduced_modifiers)
jtm.modifiersMap["pflow_reduced"]         =        list(pflow_reduced_modifiers)

if jetFlags.useTruth():
  jtm.modifiersMap["truth_ungroomed"]        =      list(truth_ungroomed_modifiers)
  jtm.modifiersMap["truth_ungroomed_larger"] =      list(truth_ungroomed_larger_modifiers)
  jtm.modifiersMap["truth_groomed"]          =      list(truth_groomed_modifiers)
jtm.modifiersMap["track_ungroomed"]          =      list(track_ungroomed_modifiers)

# Also index modifier type names by input type name.
# These are used when the modifier list is omitted.
jtm.modifiersMap["track"]                 = list(track_ungroomed_modifiers)
jtm.modifiersMap["pv0track"]              = list(track_ungroomed_modifiers)
if jetFlags.useTruth():
  jtm.modifiersMap["truth"]               = list(truth_ungroomed_modifiers)
  jtm.modifiersMap["truthwz"]             = list(truth_ungroomed_modifiers)

jetlog.info( myname + "End." )
