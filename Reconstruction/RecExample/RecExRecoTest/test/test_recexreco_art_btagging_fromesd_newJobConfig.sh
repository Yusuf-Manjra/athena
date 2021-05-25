#!/bin/sh
#
# art-description: Athena (serial mode only) runs flavor taggingreconstruction, using the new Run 3 job configuration, from an ESD file
# art-type: grid
# art-athena-mt
# art-include: master/Athena
python $Athena_DIR/python/BTagging/BTagRun3Config.py | tee temp.log
echo "art-result: ${PIPESTATUS[0]}"

test_postProcessing_Errors.sh temp.log
