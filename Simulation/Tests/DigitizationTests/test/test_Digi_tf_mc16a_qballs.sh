#!/bin/sh
#
# art-description: Run digitization of an MC16a qball sample with 2016a geometry and conditions, 25ns pile-up
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-output: mc16a_qball.RDO.pool.root

DigiOutFileName="mc16a_qball.RDO.pool.root"

HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"


Digi_tf.py \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/QBALLS.HITS.17587614._000019.pool.root.1 \
--conditionsTag default:OFLCOND-MC16-SDR-16 \
--digiSeedOffset1 170 \
--digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 1 \
--maxEvents 10 \
--numberOfCavernBkg 0 \
--numberOfHighPtMinBias 0.116075313 \
--numberOfLowPtMinBias 44.3839246425 \
--outputRDOFile ${DigiOutFileName} \
--digiSteeringConf "StandardSignalOnlyTruth" \
--postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' 'HITtoRDO:condSeq.LArAutoCorrTotalCondAlg.deltaBunch=1' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--pileupFinalBunch 6 \
--preExec 'all:from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(True);' \
--preInclude 'all:LArConfiguration/LArConfigRun2Old.py' 'HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrainsMC15_2015_25ns_Config1.py,RunDependentSimData/configLumi_run284500_mc16a.py' \
--skipEvents 0

rc=$?
status=$rc
echo  "art-result: $rc Digi_tf.py"

rc1=-9999
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2

    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed

    rc1=$?
    status=$rc1
fi
echo  "art-result: $rc1 regression"

exit $status
