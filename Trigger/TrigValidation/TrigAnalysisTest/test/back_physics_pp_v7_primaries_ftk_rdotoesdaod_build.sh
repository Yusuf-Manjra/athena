#!/bin/bash

# art-description: Trigger v7 primaries RDO to ESD and AOD test, plus extra tests using these files. With FTK
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="physics_pp_v7_primaries_ftk_rdotoesdaod_build"
export INPUT="ftk"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="Physics_pp_v7_primaries"
export EVENTS="3"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoESDAOD.py"
export EXTRA="from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.doFTK=True;from RecExConfig.RecFlags import rec;rec.doFTK.set_Value_and_Lock(True);"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
source exec_art_triganalysistest_post.sh
