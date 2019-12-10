#!/bin/bash

# art-description: ID TriggerTest over e28_lhtight_idperf on MC
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="slice_id_build"
export EVENTS="10"
export JOBOPTION="TriggerTest/testIDAthenaTrigRDO.py"
export COST_MONITORING="False"

source exec_athena_art_trigger_validation.sh
source exec_art_triggertest_post.sh
