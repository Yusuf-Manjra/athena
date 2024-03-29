#!/bin/bash
# art-description: Generation test Powheg+H7 WW->lvlv 
# art-include: master/AthGeneration
# art-include: master--HepMC2/Athena
# art-include: 22.0/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421363 --maxEvents=100 \
    --outputEVNTFile=test_phh7_wwlvlv.EVNT.pool.root \

echo "art-result: $? generate"
    


