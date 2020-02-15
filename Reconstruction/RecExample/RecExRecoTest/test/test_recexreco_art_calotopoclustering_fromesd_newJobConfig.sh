#!/bin/sh
#
# art-description: Athena runs topoclustering reconstruction, using the new job configuration for Run 3, from an ESD file
# art-type: grid
# art-athena-mt
# art-include: master/Athena

python $Athena_DIR/python/CaloRec/CaloTopoClusterConfig.py | tee temp.log
echo "art-result: ${PIPESTATUS[0]}"

test_postProcessing_Errors.sh temp.log
