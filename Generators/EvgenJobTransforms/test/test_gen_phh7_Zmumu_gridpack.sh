#!/bin/bash
# art-description: Generation test Powheg+H7_EG Z->mumu with gridpack 
# art-include: master/AthGeneration
# art-include: master--HepMC2/Athena
# art-include: 22.0/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=600433 --maxEvents=100 \
    --outputEVNTFile=test_powheg_zmumu.EVNT.pool.root \

echo "art-result: $? generate"
    


