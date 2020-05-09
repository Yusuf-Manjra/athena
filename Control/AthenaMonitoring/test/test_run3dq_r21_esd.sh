#!/bin/bash
# art-description: ESD->HIST, R21 data ESD
# art-type: grid
# art-include: master/Athena
# art-output: ExampleMonitorOutput.root
# art-output: log*

Run3DQTestingDriver.py 'Input.Files=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q431/21.0/myESD.pool.root"]' DQ.Steering.doHLTMon=False --postExec 'cfg.getEventAlgo("LArCollisionTimeAlg").cutIteration=False'  > log.HIST_Creation 2>&1

echo "art-result: $? HIST_Creation"

ArtPackage=$1
ArtJobName=$2
art.py download ${ArtPackage} ${ArtJobName}
hist_diff.sh ExampleMonitorOutput.root ./ref-*/ExampleMonitorOutput.root -x TIME_execute -i > log.HIST_Diff 2>&1
echo "art-result: $? HIST_Diff"
