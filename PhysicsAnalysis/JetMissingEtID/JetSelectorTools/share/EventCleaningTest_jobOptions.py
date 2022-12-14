#
# This job options file defines an Athena testing job for the EventCleaningTool
#

import os

# Specify input file
input_file = os.getenv('ASG_TEST_FILE_MC', '/afs/cern.ch/user/a/asgbase/patspace/xAODs/r6630/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.AOD.e3698_s2608_s2183_r6630_tid05352803_00/AOD.05352803._000242.pool.root.1')

# Setup input
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [input_file]
theApp.EvtMax = 100

# Access the algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Configure the tools using the helper module
from JetSelectorTools.ConfiguredJetCleaningTools import recEventCleaningTool
ecTool = recEventCleaningTool()

# Override properties as desired
ecTool.JvtDecorator = "passJvt"
ecTool.OrDecorator = "passOR"
ecTool.CleaningLevel = "LooseBad"
ecTool.JetContainer = "AntiKt4EMTopoJets"

# Configure the test algorithm
from JetSelectorTools.JetSelectorToolsConf import EventCleaningTestAlg
alg = EventCleaningTestAlg('EventCleaningTestAlg',
                            EventCleaningTool=ecTool)

# Add to the alg sequence
theJob += alg
