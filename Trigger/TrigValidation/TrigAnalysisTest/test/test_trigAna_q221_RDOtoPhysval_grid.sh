#!/bin/bash

# art-description: Test of transform RDO->RDO_TRIG->ESD->AOD->NTUP_PHYSVAL with serial athena (legacy trigger)
# art-type: grid
# art-include: master/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.out
# art-output: *.err
# art-output: *.log.tar.gz
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: *.check*
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml

unset ATHENA_NPROC_NUM

export NAME="trigAna_q221_RDOtoPhysval_grid"
export TEST="TrigAnalysisTest"
export INPUT="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11315/RDO.17533168._000001.pool.root.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11315/RDO.17533168._000002.pool.root.1"
export EVENTS=1000
export JOB_LOG="athena.RDOtoAOD.log"
export VALIDATIONFLAGS="doTrigEgamma,doTrigBphys,doTrigMET,doTrigJet,doTrigMuon,doTrigHLTResult,doTrigCalo,doTrigMinBias,doTrigTau,doTrigIDtrk,doTrigBjet"

# run the first transform
echo "Running RDO->RDO_TRIG->ESD->AOD with Reco_tf command:"
(set -x
Reco_tf.py \
--AMI=q221 \
--imf=True \
--maxEvents=${EVENTS} \
--inputRDOFile=${INPUT} \
--outputAODFile=AOD.pool.root \
--steering="doRDO_TRIG" \
--valid=True \
--postInclude="TriggerTest/disableChronoStatSvcPrintout.py" \
>${JOB_LOG} 2>&1
) 2>&1

export ATH_RETURN=$?
echo "art-result: ${ATH_RETURN} ${JOB_LOG%.*}"

# add log for merging
echo "### ${JOB_LOG} ###" > athena.merged.log
cat ${JOB_LOG} >> athena.merged.log
export JOB_LOG="athena.PhysVal.log"

# run the second transform
echo "Running AOD->NTUP_PHYSVAL with Reco_tf command:"
(set -x
Reco_tf.py \
--AMI=q221 \
--imf=True \
--maxEvents=${EVENTS} \
--inputAODFile=AOD.pool.root \
--outputNTUP_PHYSVALFile=NTUP_PHYSVAL.pool.root \
--validationFlags="${VALIDATIONFLAGS}" \
--postInclude="TriggerTest/disableChronoStatSvcPrintout.py" \
>${JOB_LOG} 2>&1
) 2>&1

export ATH_RETURN=$?
echo "art-result: ${ATH_RETURN} ${JOB_LOG%.*}"

# merge transform logs for post-processing and prepare for RegTest comparison
source exec_art_triganalysistest_merge_trf_logs.sh "RDOtoRDOTrigger RAWtoESD ESDtoAOD PhysicsValidation"

# use TrigUpgradeTest post-processing script
source exec_TrigUpgradeTest_art_post.sh

# zip the merged log (can be large and duplicates information)
tar -czf athena.merged.log.tar.gz athena.merged.log
rm -f athena.merged.log
