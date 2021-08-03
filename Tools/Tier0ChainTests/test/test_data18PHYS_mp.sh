#!/bin/sh

# art-include: master/Athena
# art-description: DAOD building PHYS data18 MP
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt
# art-athena-mt: 8

set -e

Reco_tf.py --athenaopts='--nprocs=2' --athenaMPMergeTargetSize 'DAOD_*:0' --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data18_13TeV.00357750.physics_Main.merge.AOD.r12879_p4685/AOD.26343283._000019.pool.root.1 --outputDAODFile art.pool.root --reductionConf PHYS --maxEvents -1 --preExec 'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff();'

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_PHYSFile DAOD_PHYS.art.pool.root --outputDAOD_PHYS_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_PHYS.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_PHYS.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
