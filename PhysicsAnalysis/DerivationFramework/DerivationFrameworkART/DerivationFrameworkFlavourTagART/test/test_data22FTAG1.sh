#!/bin/sh

# art-include: master/Athena
# art-include: 23.0/Athena
# art-description: DAOD building FTAG1 data22
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile*.txt
# art-output: checkxAOD*.txt
# art-output: checkIndexRefs*.txt

set -e

Derivation_tf.py \
--CA True \
--inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CampaignInputs/data22/AOD/data22_13p6TeV.00431906.physics_Main.merge.AOD.r13928_p5279/1000events.AOD.30220215._001367.pool.root.1 \
--outputDAODFile art.pool.root \
--formats FTAG1 \
--maxEvents -1 \

echo "art-result: $? reco"

checkFile.py DAOD_FTAG1.art.pool.root > checkFile_FTAG1.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_FTAG1.art.pool.root > checkxAOD_FTAG1.txt

echo "art-result: $?  checkxAOD"

checkIndexRefs.py DAOD_FTAG1.art.pool.root > checkIndexRefs_FTAG1.txt 2>&1

echo "art-result: $?  checkIndexRefs"
