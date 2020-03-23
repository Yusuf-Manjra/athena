#!/bin/bash
# art-description: Generation test MadGraph LHE-only for W+bW-b
# art-type: build
# art-include: master/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421125 \
    --outputTXTFile=test_mg_WbWb.TXT \

echo "art-result: $? generate"


