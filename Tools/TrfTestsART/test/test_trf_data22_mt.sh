#!/bin/bash
#
# art-description: Reco_tf.py data22 RAWtoALL in MT mode
# art-type: grid
# art-include: master/Athena
# art-include: 23.0/Athena
# art-athena-mt: 8

timeout 64800 Reco_tf.py --CA \
  --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/TCT_Run3/data22_13p6TeV.00429782.physics_Main.daq.RAW._lb0562._SFO-11._0001.data \
  --outputAODFile=myAOD.pool.root \
  --outputHISTFile=myHIST.root \
  --outputDESDM_MCPFile=myDESDM_MCP.pool.root \
  --outputDRAW_ZMUMUFile=myDRAW_ZMUMU.data \
  --outputDRAW_EGZFile=myDRAW_EGZ.data \
  --outputDAOD_IDTIDEFile=myIDTIDE.pool.root \
  --multithreaded='True' \
  --preExec "all:flags.DQ.Steering.HLT.doMET=True; flags.DQ.Steering.HLT.doJet=True; flags.Exec.FPE=10;" \
  --autoConfiguration="everything" \
  --conditionsTag "all:CONDBR2-BLKPA-2022-07" \
  --geometryVersion="all:ATLAS-R3S-2021-03-00-00" \
  --runNumber="429782" \
  --maxEvents='-1'

rc1=$?
echo "art-result: ${rc1} Reco_tf_data22_mt"

# Check for FPEs in the logiles
test_trf_check_fpe.sh
fpeStat=$?

echo "art-result: ${fpeStat} FPEs in logfiles"
