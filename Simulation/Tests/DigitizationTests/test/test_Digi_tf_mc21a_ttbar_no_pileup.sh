#!/bin/bash
#
# art-description: Run a digitization example to compare configuration between ConfGetter and the new ComponentAccumulator approach.
# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-include: 22.0/Athena
# art-include: 23.0/Athena
# art-include: master/Athena
# art-output: mc21a.ttbar.nopileup.CG.RDO.pool.root
# art-output: mc21a.ttbar.nopileup.CA.RDO.pool.root
# art-output: log.*
# art-output: legacy.*
# art-output: DigiConfig*

if [ -z ${ATLAS_REFERENCE_DATA+x} ]; then
  ATLAS_REFERENCE_DATA="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art"
fi

Events=25
DigiOutFileNameCG="mc21a.ttbar.nopileup.CG.RDO.pool.root"
DigiOutFileNameCA="mc21a.ttbar.nopileup.CA.RDO.pool.root"
HSHitsFile="${ATLAS_REFERENCE_DATA}/CampaignInputs/mc21/HITS/mc21_13p6TeV.601229.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep.simul.HITS.e8453_s3873/1000events_singleBS.HITS.pool.root"


# config only
Digi_tf.py \
    --DataRunNumber 410000 \
    --conditionsTag default:OFLCOND-MC21-SDR-RUN3-07 \
    --digiSeedOffset1 170 --digiSeedOffset2 170 \
    --geometryVersion default:ATLAS-R3S-2021-03-00-00 \
    --inputHITSFile ${HSHitsFile} \
    --jobNumber 568 \
    --maxEvents ${Events} \
    --outputRDOFile ${DigiOutFileNameCA} \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --preInclude 'all:Campaigns/MC21NoPileUp.py' \
    --skipEvents 0 \
    --runNumber 601229 \
    --athenaopts '"--config-only=DigiConfigCG.pkl"'

# full run
Digi_tf.py \
    --DataRunNumber 410000 \
    --conditionsTag default:OFLCOND-MC21-SDR-RUN3-07 \
    --digiSeedOffset1 170 --digiSeedOffset2 170 \
    --geometryVersion default:ATLAS-R3S-2021-03-00-00 \
    --inputHITSFile ${HSHitsFile} \
    --jobNumber 568 \
    --maxEvents ${Events} \
    --outputRDOFile ${DigiOutFileNameCG} \
    --postExec 'HITtoRDO:job+=CfgMgr.JobOptsDumperAlg(FileName="DigiConfigCG.txt")' \
    --postInclude 'default:PyJobTransforms/UseFrontier.py' \
    --preInclude 'all:Campaigns/MC21NoPileUp.py' \
    --skipEvents 0 \
    --runNumber 601229

rc=$?
status=$rc
echo "art-result: $rc digiOLD"
mv runargs.HITtoRDO.py runargs.legacy.HITtoRDO.py
mv log.HITtoRDO legacy.HITtoRDO

rc2=-9999
Digi_tf.py \
    --CA \
    --DataRunNumber 410000 \
    --conditionsTag default:OFLCOND-MC21-SDR-RUN3-07 \
    --digiSeedOffset1 170 --digiSeedOffset2 170 \
    --geometryVersion default:ATLAS-R3S-2021-03-00-00 \
    --inputHITSFile ${HSHitsFile} \
    --jobNumber 568 \
    --maxEvents ${Events} \
    --outputRDOFile ${DigiOutFileNameCA} \
    --postInclude 'PyJobTransforms.UseFrontier' 'HITtoRDO:Digitization.DigitizationSteering.DigitizationTestingPostInclude' \
    --preInclude 'HITtoRDO:Campaigns.MC21NoPileUp' \
    --skipEvents 0 \
    --runNumber 601229

rc2=$?
if [[ $status -eq 0 ]]
then
    status=$rc2
fi
echo "art-result: $rc2 digiCA"

rc3=-9999
if [[ $status -eq 0 ]]
then
    acmd.py diff-root ${DigiOutFileNameCG} ${DigiOutFileNameCA} \
        --mode=semi-detailed --error-mode resilient --order-trees \
        --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings index_ref
    rc3=$?
    status=$rc3
fi
echo "art-result: $rc3 OLDvsCA"

# get reference directory
source DigitizationCheckReferenceLocation.sh
echo "Reference set being used: ${DigitizationTestsVersion}"

rc4=-9999
if [[ $rc -eq 0 ]]
then
    # Do reference comparisons
    art.py compare ref --mode=semi-detailed --no-diff-meta "$DigiOutFileNameCG" "${ATLAS_REFERENCE_DATA}/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileNameCG"
    rc4=$?
    status=$rc4
fi
echo "art-result: $rc4 OLDvsFixedRef"

rc5=-9999
if [[ $rc -eq 0 ]]
then
    checkFile "$DigiOutFileNameCG"
    rc5=$?
    status=$rc5
fi
echo "art-result: $rc5 checkFile"

rc6=-9999
if [[ $rc -eq 0 ]]
then
    art.py compare grid --entries 25 "$1" "$2" --mode=semi-detailed --file="$DigiOutFileNameCG"
    rc6=$?
    status=$rc6
fi
echo "art-result: $rc6 regression"

exit $status
