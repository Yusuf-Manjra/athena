#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - Cluster grid pack
# art-type: grid

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=950111 --outputFileValidation=False

echo "art-result: $? Gen_tf"
