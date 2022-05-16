#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building HION9 data18
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data18_hi.00365602.physics_HardProbes.merge.AOD.f1021_m2037._lb0203._0001.1 --outputDAODFile art.pool.root --reductionConf HION9 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True); rec.doHeavyIon.set_Value_and_Lock(True); from BTagging.BTaggingFlags import BTaggingFlags; BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-49"; from HIRecExample.HIRecExampleFlags import jobproperties; jobproperties.HIRecExampleFlags.doHIAODFix.set_Value_and_Lock(True); from HIJetRec.HIJetRecFlags import HIJetFlags; HIJetFlags.TWConfigFile="cluster.geo.DATA_PbPb_2018.root"; HIJetFlags.AntiKtRValues.set_Value_and_Lock([0.2,0.4,0.6,0.8,1.0]); from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "BTaggingFixer", Containers=["BTagging_AntiKt4HIAux."] )' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_HION9File DAOD_HION9.art.pool.root --outputDAOD_HION9_MRGFile art_merged.pool.root --preExec 'rec.doHeavyIon.set_Value_and_Lock(False)' 

echo "art-result: $? merge"

checkFile.py DAOD_HION9.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_HION9.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
