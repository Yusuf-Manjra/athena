#!/bin/sh

# art-include: master/Athena
# art-description: DAOD building PHYS and PHYSLITE mc20 MP w/ SharedWriter w/o parallel compression
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile*.txt
# art-output: checkxAOD*.txt
# art-output: checkIndexRefs*.txt
# art-athena-mt: 8

ATHENA_CORE_NUMBER=8 Derivation_tf.py \
  --CA True \
  --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc20_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.AOD.e3601_s3681_r13167/AOD.27312826._000061.pool.root.1 \
  --outputDAODFile art.pool.root \
  --formats PHYS PHYSLITE \
  --maxEvents -1 \
  --sharedWriter True \
  --parallelCompression False \
  --multiprocess True \

echo "art-result: $? reco"

checkFile.py DAOD_PHYS.art.pool.root > checkFile_PHYS.txt

echo "art-result: $?  checkfile PHYS"

checkFile.py DAOD_PHYSLITE.art.pool.root > checkFile_PHYSLITE.txt

echo "art-result: $?  checkfile PHYSLITE"

checkxAOD.py DAOD_PHYS.art.pool.root > checkxAOD_PHYS.txt

echo "art-result: $?  checkxAOD PHYS"

checkxAOD.py DAOD_PHYSLITE.art.pool.root > checkxAOD_PHYSLITE.txt

echo "art-result: $?  checkxAOD PHYSLITE"

checkIndexRefs.py DAOD_PHYS.art.pool.root > checkIndexRefs_PHYS.txt 2>&1

echo "art-result: $?  checkIndexRefs PHYS"

checkIndexRefs.py DAOD_PHYSLITE.art.pool.root > checkIndexRefs_PHYSLITE.txt 2>&1

echo "art-result: $?  checkIndexRefs PHYSLITE"
