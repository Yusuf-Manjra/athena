#!/bin/bash
# art-description: athenaMT trigger test using the full LS2_v1 menu from TriggerMenuMT
# art-type: grid
# art-include: master/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*

export INPUT="run2dataFTK" # FTK doesn't matter here - using this dataset because of larger stats
export EVENTS=1000
export THREADS=1
export SLOTS=1
export JOBOPTION="TrigUpgradeTest/full_menu.py"
export REGTESTEXP="TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*"
export EXTRA="doWriteBS=False;doWriteRDOTrigger=True;"

source exec_TrigUpgradeTest_art_athenaMT.sh
source exec_TrigUpgradeTest_art_post.sh
