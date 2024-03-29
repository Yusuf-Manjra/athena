#!/bin/sh

# art-description: MC+MC Overlay with MT support, running with 4 threads
# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-athena-mt: 8
# art-include: 22.0/Athena
# art-include: 23.0/Athena
# art-include: master/Athena

# art-output: MC_plus_MC.RDO.pool.root
# art-output: log.*
# art-output: mem.summary.*
# art-output: mem.full.*
# art-output: runargs.*

export ATHENA_CORE_NUMBER=4

HITS_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CampaignInputs/mc21/HITS/mc21_13p6TeV.601229.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep.simul.HITS.e8453_s3873/50events.HITS.pool.root"
RDO_BKG_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CampaignInputs/mc21/RDO_BKG/mc21_13p6TeV.900149.PG_single_nu_Pt50.digit.RDO.e8453_e8455_s3864_d1761/50events.RDO.pool.root"


Overlay_tf.py \
--multithreaded \
--runNumber 601229 \
--inputHITSFile ${HITS_File} \
--inputRDO_BKGFile ${RDO_BKG_File} \
--outputRDOFile MC_plus_MC.RDO.pool.root \
--maxEvents 50 --skipEvents 10 --digiSeedOffset1 511 --digiSeedOffset2 727 \
--conditionsTag OFLCOND-MC21-SDR-RUN3-07  \
--geometryVersion ATLAS-R3S-2021-03-00-00 \
--preInclude 'all:Campaigns/MC21a.py' \
--imf False

rc=$?
status=$rc
echo "art-result: $rc overlay"
rc2=-9999
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 "${ArtPackage}" "${ArtJobName}" --mode=semi-detailed --order-trees --diff-root
    rc2=$?
    status=$rc2
fi
echo  "art-result: $rc2 regression"

exit $status
