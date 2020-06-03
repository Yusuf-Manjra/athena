#!/bin/sh
#
# art-description: RecoTrf with beamType=collisions
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena

Reco_tf.py  --outputHISTFile=myMergedMonitoring_Egamma_0.root --maxEvents=500 --outputESDFile=myESD_Egamma_0.pool.root --outputAODFile=myAOD_Egamma_0.AOD.pool.root --outputTAGFile=myTAG_Egamma_0.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data11_7TeV.00184169.physics_Egamma.merge.RAW._lb0900._SFO-10._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data11_7TeV.00184169.physics_Egamma.merge.RAW._lb0900._SFO-11._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data11_7TeV.00184169.physics_Egamma.merge.RAW._lb0900._SFO-12._0001.1 --preExec='rec.doTrigger=True;DQMonFlags.doCTPMon=False;' --imf False
echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"

