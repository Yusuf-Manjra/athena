#!/bin/sh
#
# art-description: Athena (two thread mode only) runs muon segment finding, using the new job configuration system for Run 3, from an ESD file
# art-type: grid
# art-athena-mt: 8
# art-include: master/Athena

export ATHENA_CORE_NUMBER=8

python $Athena_DIR/python/MuonConfig/MuonSegmentFindingConfig.py | tee temp1.log
echo "art-result: ${PIPESTATUS[0]}"
athena --threads=2 MuonSegmentFinding.pkl | tee temp2.log
echo "art-result: ${PIPESTATUS[0]}"

test_postProcessing_Errors.sh temp1.log
test_postProcessing_Errors.sh temp2.log
