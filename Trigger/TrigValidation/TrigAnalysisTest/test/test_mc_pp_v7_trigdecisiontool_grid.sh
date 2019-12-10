#!/bin/bash

# art-description: Trigger Decision Tool test on AOD
# art-type: grid
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.3/Athena
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

export NAME="mc_pp_v7_trigdecisiontool_grid"
export JOB_LOG="athena.log"
export TEST="TrigAnalysisTest"
export DS='["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigAnalysisTest/AthenaTrigAOD_TrigEDMandTDTCheck_MC_pp_v7_chain/AOD.pool.root"]'

athena.py -c "RunningRTT=TRUE;jp.AthenaCommonFlags.PoolAODInput=${DS};DetDescrVersion='ATLAS-R2-2015-03-01-00'" -b TrigAnalysisTest/testAthenaTrigAOD_TrigDecTool.py | tee ${JOB_LOG}
echo "art-result: ${PIPESTATUS[0]} ${JOB_LOG%%.*}"

export SKIP_CHAIN_DUMP=1
echo "trigedm SKIP_CHAIN_DUMP" $[SKIP_CHAIN_DUMP]

source exec_art_triggertest_post.sh
