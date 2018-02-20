#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py  --outputHISTFile=myMergedMonitoring_Muons_0.root --AMI=q205 --maxEvents=300 --outputESDFile=myESD_Muons_0.pool.root --geometryVersion=ATLAS-R1-2012-03-00-00 --outputAODFile=myAOD_Muons_0.AOD.pool.root --outputTAGFile=myTAG_Muons_0.root --ignoreErrors=False --conditionsTag=COMCOND-BLKPA-RUN1-07 --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data12_8TeV.00204073.physics_Muons.merge.RAW._lb0144._SFO-3._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data12_8TeV.00204073.physics_Muons.merge.RAW._lb0145._SFO-9._0001.1 --autoConfiguration=everything --preExec='rec.doTrigger=False;' --imf False
echo "art-result: $?"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $?"

art.py compare grid --days=3 --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $?"

