# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
#   jtm.addJetFinder("AntiKt4EMTopoJets", "AntiKt", 0.4, "em", "calib_topo_ungroomed")
#

import copy

myname = "JetRecStandardToolManager.py: "

print myname + "Defining standard tools"

#########################################################
# Set and lock flags.
#########################################################

from JetRec.JetRecFlags import jetFlags

# Set status on for flags that are initialized with status off.
jetFlags.useTruth.set_On()
jetFlags.useTopo.set_On()
jetFlags.useTracks.set_On()
jetFlags.useMuonSegments.set_On()
jetFlags.useBTagging.set_On()

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
print myname + "jetFlags.useTruth: " + str(jetFlags.useTruth())
print myname + "jetFlags.useTopo: " + str(jetFlags.useTopo())
print myname + "jetFlags.useTracks: " + str(jetFlags.useTracks())
print myname + "jetFlags.useMuonSegments: " + str(jetFlags.useMuonSegments())
print myname + "jetFlags.useBTagging: " + str(jetFlags.useBTagging())
print myname + "jetFlags.useCaloQualityTool: " + str(jetFlags.useCaloQualityTool())
print myname + "jetFlags.additionalTopoGetters: " + str(jetFlags.additionalTopoGetters())
print myname + "jetFlags.truthFlavorTags: " + str(jetFlags.truthFlavorTags())
print myname + "jetFlags.skipTools: " + str(jetFlags.skipTools())

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
import JetRec.JetRecStandardTools

if 0:
  print "First call to JetRecStandardToolManager.py:"
  from traceback import format_exc
  print format_exc()
  print "Exception"
  raise Exception
  from traceback import print_stack
  print_stack()

#########################################################
# Getters
#########################################################

# Pseudojet getters
empfgetters =  [jtm.empflowget]
emcpfgetters = [jtm.emcpflowget]
lcpfgetters =  [jtm.lcpflowget]
emgetters = [jtm.emget]
lcgetters = [jtm.lcget]
trackgetters = [jtm.trackget]
# Add track ghosts
if jetFlags.useTracks():
  emgetters += [jtm.gtrackget]
  lcgetters += [jtm.gtrackget]
  empfgetters += [jtm.gtrackget]
  emcpfgetters += [jtm.gtrackget]
  lcpfgetters += [jtm.gtrackget]
if jetFlags.useMuonSegments():
  emgetters += [jtm.gmusegget]
  lcgetters += [jtm.gmusegget]
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
  emgetters += [jtm.gtruthget]
  lcgetters += [jtm.gtruthget]
  empfgetters += [jtm.gtruthget]
  emcpfgetters += [jtm.gtruthget]
  lcpfgetters += [jtm.gtruthget]
  # Add truth cone matching and truth flavor ghosts.
  flavorgetters = []
  for ptype in jetFlags.truthFlavorTags():
    flavorgetters += [getattr(jtm, "gtruthget_" + ptype)]
  emgetters      += flavorgetters
  lcgetters      += flavorgetters
  truthgetters   += flavorgetters
  truthwzgetters += flavorgetters
  trackgetters   += flavorgetters
  empfgetters    += flavorgetters
  emcpfgetters   += flavorgetters
  lcpfgetters    += flavorgetters
# Add track jet ghosts.
if jetFlags.useTracks():
  trackjetgetters = []
  trackjetgetters += [jtm.gakt3trackget]
  trackjetgetters += [jtm.gakt4trackget]
  emgetters += trackjetgetters
  lcgetters += trackjetgetters
  empfgetters += trackjetgetters
  emcpfgetters += trackjetgetters
  lcpfgetters += trackjetgetters

# Add getter lists to jtm indexed by input type name.
jtm.gettersMap["emtopo"]    = list(emgetters)
jtm.gettersMap["lctopo"]    = list(lcgetters)
jtm.gettersMap["empflow"]   = list(empfgetters)
jtm.gettersMap["emcpflow"]  = list(emcpfgetters)
jtm.gettersMap["lcpflow"]   = list(lcpfgetters)
jtm.gettersMap["track"]     = list(trackgetters)
jtm.gettersMap["ztrack"]    = list(trackgetters)
jtm.gettersMap["pv0track"]  = list(trackgetters)
if jetFlags.useTruth():
  jtm.gettersMap["truth"]   = list(truthgetters)
  jtm.gettersMap["truthwz"] = list(truthwzgetters)

#########################################################
# Modifiers
#########################################################

# Modifiers for ungroomed jets from all input types.
common_ungroomed_modifiers = [
  jtm.width,
  jtm.jetisol,
  jtm.ktdr,
  jtm.nsubjettiness,
  jtm.ktsplitter,
  jtm.angularity,
  jtm.dipolarity,
  jtm.planarflow,
  jtm.ktmassdrop,
  jtm.encorr,
  jtm.comshapes
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

# Modifiers for track jets.
track_ungroomed_modifiers = list(common_ungroomed_modifiers)
if jetFlags.useTruth() and jtm.haveParticleJetTools:
  track_ungroomed_modifiers += [jtm.trackjetdrlabeler]

# Modifiers for topo (and pflow) jets.
topo_ungroomed_modifiers = ["jetfilter"]
topo_ungroomed_modifiers += common_ungroomed_modifiers
topo_ungroomed_modifiers += [jtm.jetens]
# Tool initialization raises errors.
# ATLJETMET-15
topo_ungroomed_modifiers += [jtm.larhvcorr]
if jetFlags.useCaloQualityTool():
  topo_ungroomed_modifiers += [jtm.caloqual_cluster]
if jetFlags.useTracks():
  topo_ungroomed_modifiers += [jtm.jvf, jtm.jvt, jtm.trkmoms]
  topo_ungroomed_modifiers += ["trackassoc"]
if jetFlags.useTruth():
  topo_ungroomed_modifiers += ["truthassoc"]
  if jtm.haveParticleJetTools:
    topo_ungroomed_modifiers += [jtm.jetdrlabeler]

# Modifiers for groomed jets.
groomed_modifiers = [ ]

# Modifiers for groomed topo jets.
topo_groomed_modifiers = list(groomed_modifiers)
if jetFlags.useTracks():
  topo_groomed_modifiers += [jtm.jetorigincorr]
groomed_modifiers += [jtm.jetsorter]
topo_groomed_modifiers += [jtm.jetsorter]

# Function to filter out skipped tools.
def filterout(skiptoolnames, tools):
  outtools = []
  remtoolnames = []
  nrem = 0
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
  print myname + "Removed tools: " + str(remtoolnames)
  return outtools

# Modifiers for pflow jets.
# Same as topo jets.
pflow_ungroomed_modifiers = []
pflow_ungroomed_modifiers += ["calib"]
pflow_ungroomed_modifiers += topo_ungroomed_modifiers

# Here add tools to be run for topo jets and NOT for pflow.

# Cluster moments.
topo_ungroomed_modifiers += [jtm.clsmoms]

# Add origin corrn for uncalibrated, ungroomed topo jets.
if jetFlags.useTracks():
  topo_ungroomed_modifiers += [jtm.jetorigincorr]

# Modifiers for calibrated topo jets.
calib_topo_ungroomed_modifiers = []
if jetFlags.useTracks():
  calib_topo_ungroomed_modifiers += [jtm.jetorigincorr, "calib", jtm.jetsorter]
calib_topo_ungroomed_modifiers += topo_ungroomed_modifiers

# Add Btagging.
btags = ["btag"]
if jetFlags.useBTagging():
  topo_ungroomed_modifiers += btags
  calib_topo_ungroomed_modifiers += btags

# Filter out skipped tools.
if len(jetFlags.skipTools()):
  print myname + "Tools to be skipped: " + str(jetFlags.skipTools())
  topo_ungroomed_modifiers        = filterout(jetFlags.skipTools(), topo_ungroomed_modifiers)
  calib_topo_ungroomed_modifiers  = filterout(jetFlags.skipTools(), calib_topo_ungroomed_modifiers)
  if jetFlags.useTruth():
    truth_ungroomed_modifiers     = filterout(jetFlags.skipTools(), truth_ungroomed_modifiers)
  track_ungroomed_modifiers       = filterout(jetFlags.skipTools(), track_ungroomed_modifiers)
  topo_groomed_modifiers          = filterout(jetFlags.skipTools(), topo_groomed_modifiers)
  groomed_modifiers               = filterout(jetFlags.skipTools(), groomed_modifiers)
  pflow_ungroomed_modifiers       = filterout(jetFlags.skipTools(), pflow_ungroomed_modifiers)

# Add modifier lists to jtm indexed by modifier type name.
jtm.modifiersMap["none"]                  = []
jtm.modifiersMap["topo_ungroomed"]        =       list(topo_ungroomed_modifiers)
jtm.modifiersMap["calib_topo_ungroomed"]  = list(calib_topo_ungroomed_modifiers)
jtm.modifiersMap["calib"]                 = list(calib_topo_ungroomed_modifiers)
jtm.modifiersMap["pflow"]                 =      list(pflow_ungroomed_modifiers)
if jetFlags.useTruth():
  jtm.modifiersMap["truth_ungroomed"]     =      list(truth_ungroomed_modifiers)
jtm.modifiersMap["track_ungroomed"]       =      list(track_ungroomed_modifiers)
jtm.modifiersMap["topo_groomed"]          =         list(topo_groomed_modifiers)
jtm.modifiersMap["groomed"]               =              list(groomed_modifiers)

# Also index modifier type names by input type name.
# These are used when the modifier list is omitted.
jtm.modifiersMap["emtopo"]    =  list(topo_ungroomed_modifiers)
jtm.modifiersMap["lctopo"]    =  list(topo_ungroomed_modifiers)
jtm.modifiersMap["track"]     = list(track_ungroomed_modifiers)
jtm.modifiersMap["ztrack"]    = list(track_ungroomed_modifiers)
jtm.modifiersMap["pv0track"]  = list(track_ungroomed_modifiers)
if jetFlags.useTruth():
  jtm.modifiersMap["truth"]   = list(truth_ungroomed_modifiers)
  jtm.modifiersMap["truthwz"] = list(truth_ungroomed_modifiers)

print myname + "End."
