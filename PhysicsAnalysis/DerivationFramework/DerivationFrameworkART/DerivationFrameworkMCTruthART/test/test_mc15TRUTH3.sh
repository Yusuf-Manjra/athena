#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building TRUTH3 mc15
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/EVNT.17955321._002707.pool.root.1 --outputDAODFile art.pool.root --reductionConf TRUTH3 --maxEvents 1000

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_TRUTH3File DAOD_TRUTH3.art.pool.root --outputDAOD_TRUTH3_MRGFile art_merged.pool.root --athenaopts="-s" 

echo "art-result: $? merge"

checkFile.py DAOD_TRUTH3.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_TRUTH3.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
