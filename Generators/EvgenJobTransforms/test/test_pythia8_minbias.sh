#!/bin/bash
# art-description: Generation test Pythia8 min_bias 
# art-include: 21.6/AthGeneration
# art-output *.root
# art-output log.generate

#get_files -jo MC15.361033.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic.py

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --runNumber=361033 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID361xxx/MC15.361033.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic.py \
    --outputEVNTFile=test_minbias_inelastic.EVNT.pool.root \
    $@
$@ echo "art-result: $? generate"
#rm -fr MC15.361033.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic.py
