#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - LO with reweighting module
# art-type: grid
# art-output: test_lhe_events.events

set -e

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputTXTFile=test_lhe_events --jobConfig=950113

echo "art-result: $?"
