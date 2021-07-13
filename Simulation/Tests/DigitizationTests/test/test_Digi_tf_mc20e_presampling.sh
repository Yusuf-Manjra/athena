#!/bin/sh
#
# art-description: Run MC20e pile-up presamling with 2018 geometry and conditions, 25ns pile-up, MT output containers
# art-type: grid
# art-include: master/Athena
# art-output: mc20e_presampling.RDO.pool.root
# art-output: log.*

Events=25
DigiOutFileName="mc20e_presampling.RDO.pool.root"
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/mc16_13TeV.900149.PG_single_nu_Pt50.simul.HITS.e8307_s3482/HITS.24078104._234467.pool.root.1"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.simul.HITS_FILT.e8341_s3687_s3704/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.900311.Epos_minbias_inelastic_lowjetphoton.simul.HITS_FILT.e8341_s3687_s3704/*"


Digi_tf.py \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC16-SDR-RUN2-04 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf "StandardSignalOnlyTruth" \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileName} \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC20e.py' \
--skipEvents 0

rc=$?
status=$rc
echo  "art-result: $rc Digi_tf.py"
rc1=-9999
rc2=-9999
rc3=-9999
rc4=-9999

# get reference directory
source DigitizationCheckReferenceLocation.sh
echo "Reference set being used: " ${DigitizationTestsVersion}

if [ $rc -eq 0 ]
then
    # Do reference comparisons
    art-diff.py ./$DigiOutFileName   /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName
    rc1=$?
    status=$rc1
fi
echo  "art-result: $rc1 diff-pool"

if [ $rc -eq 0 ]
then
    art-diff.py ./$DigiOutFileName /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileName --diff-type=diff-root --mode=semi-detailed
    rc2=$?
    status=$rc2
fi
echo  "art-result: $rc2 diff-root"

if [ $rc -eq 0 ]
then
    checkFile ./$DigiOutFileName
    rc3=$?
    status=$rc3
fi
echo "art-result: $rc3 checkFile"

if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed
    rc4=$?
    status=$rc4
fi
echo  "art-result: $rc4 art-compare"

exit $status
