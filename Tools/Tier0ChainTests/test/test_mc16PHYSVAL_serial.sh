#!/bin/sh

# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-description: DAOD building PHYSVAL mc16
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.AOD.e3601_s3126_r12885/* --outputDAODFile art.pool.root --reductionConf PHYSVAL --maxEvents -1 --preExec 'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff();'

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_PHYSVALFile DAOD_PHYSVAL.art.pool.root --outputDAOD_PHYSVAL_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_PHYSVAL.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_PHYSVAL.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
