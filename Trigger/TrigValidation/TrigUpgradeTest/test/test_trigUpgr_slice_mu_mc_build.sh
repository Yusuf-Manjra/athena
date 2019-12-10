#!/bin/bash
# art-description: athenaMT trigger test on MC running L1 simulation and the LS2_v1 muon menu from TriggerMenuMT
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export EVENTS=10
export THREADS=1
export SLOTS=1
export INPUT="run2mc_ttbar"
export JOBOPTION="TrigUpgradeTest/full_menu.py"
export REGTESTEXP="TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*"
export EXTRA="doL1Sim=True;doEmptyMenu=True;doMuonSlice=True;doWriteBS=False;doWriteRDOTrigger=True;"

# Find the regtest reference installed with the release
export REGTESTREF=`find_data.py TrigUpgradeTest/slice_mu_mc.ref`


source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
