#!/bin/sh
#
# art-description: Athena runs topoclustering from an ESD file
# art-type: grid
# art-include: master/Athena

art.py createpoolfile

athena --threads=1 tauRec/run_tau_standalone.py | tee temp.log
echo "art-result: ${PIPESTATUS[0]}"

test_postProcessing_Errors.sh temp.log

