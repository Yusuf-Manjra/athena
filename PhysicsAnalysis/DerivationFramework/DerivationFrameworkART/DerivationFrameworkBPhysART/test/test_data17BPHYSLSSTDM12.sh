#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building STDM12 data17BPHYSLS
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00337491.physics_BphysLS.merge.AOD.f873_m1885._lb0100._0001.1 --outputDAODFile art.pool.root --reductionConf STDM12 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True); from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-49"; from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "InDetTrackParticlesFixer", Containers = [ "InDetTrackParticlesAux." ] )' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_STDM12File DAOD_STDM12.art.pool.root --outputDAOD_STDM12_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_STDM12.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_STDM12.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
