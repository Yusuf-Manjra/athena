#!/bin/sh
#
# art-description: Reco_tf runs on 13TeV collision data 2018
# art-athena-mt: 4
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


Reco_tf.py --athenaopts="--threads=1" --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data18_13TeV/data18_13TeV.00348885.physics_Main.daq.RAW._lb0827._SFO-8._0002.data --maxEvents 300 --AMI=f1089 --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputHISTFile myHist.root --preExec 'rec.doFloatingPointException=True;rec.doTrigger=False;' --postExec 'FPEAuditor.NStacktracesOnFPE=5'

#Remember retval of transform as art result
RES=$?
echo "art-result: $RES Reco"
return $RES

