#!/bin/sh
#
# art-description: Reco_tf runs on 8TeV collision data with all streams 2012
# art-athena-mt: 4
# art-type: grid
# art-include: master/Athena
# art-include: 22.0/Athena

# We set --preExec="DQMonFlags.doLVL1CaloMon=False" to resolve ATR-25910. Lv1CaloMon monitors hardware and so anyway is not relevant for reprocessing old data.
Reco_tf.py --athenaopts="--threads=8" --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1 --maxEvents 300 --autoConfiguration everything --conditionsTag="COMCOND-BLKPA-RUN1-07" --preExec="DQMonFlags.doLVL1CaloMon=False" --postExec 'all:from IOVDbSvc.CondDB import conddb;conddb.addOverride("/TRT/Calib/PID_NN", "TRTCalibPID_NN_v1");conddb.addOverride("/TRT/Onl/Calib/PID_NN", "TRTCalibPID_NN_v1");' --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputHISTFile myHist.root 

RES=$?
echo "art-result: $RES Reco"
