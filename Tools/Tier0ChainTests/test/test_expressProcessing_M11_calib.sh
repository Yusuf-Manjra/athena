#!/bin/sh
#
# art-description: Express processing at Tier0
# art-type: grid
# art-include: master/Athena
# art-include: 22.0/Athena
# art-athena-mt: 8

# There was a database connection problem reported in ATR-24782. Rodney Walker's solution is to use the following export to fix the problem:
export TNS_ADMIN=/cvmfs/atlas.cern.ch/repo/sw/database/DBRelease/current/oracle-admin


Reco_tf.py  \
--AMI x654  \
--inputBSFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/TCT_Run3/data22_calib.00412340.physics_CosmicCalo.daq.RAW._lb0018._SFO-14._0001.data" \
--outputAODFile="AOD.pool.root" \
--outputESDFile="ESD.pool.root" \
--outputDAOD_L1CALO2File="L1CALO2.pool.root" \
--outputHISTFile="HIST.root" \
--imf False

rc1=$?
echo "art-result: $rc1 Reco"

rc2=-9999
if [ ${rc1} -eq 0 ]
then
  ArtPackage=$1
  ArtJobName=$2
  art.py compare grid --entries 30 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees --ignore-exit-code diff-pool \
  --ignore-leave '(.*)TrigCompositeAuxContainer_v2_HLTNav_Summary_ESDSlimmedAux(.*)' \
  --ignore-leave '(.*)HLTNav_Summary_AODSlimmedAux(.*)' # ATR-25521
  rc2=$?
fi
echo  "art-result: ${rc2} Diff"

