#!/bin/bash
#
# art-description: Run 4 digitization of a ttbar sample with pile-up (saving in-time truth)
# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-include: 23.0/Athena
# art-include: master/Athena
# art-output: RUN4_ttbar.puTruth.RDO.pool.root
# art-output: RDOAnalysis.root

if [ -z ${ATLAS_REFERENCE_DATA+x} ]; then
  ATLAS_REFERENCE_DATA="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art"
fi

Events=100
HSHitsFile="${ATLAS_REFERENCE_DATA}/PhaseIIUpgrade/HITS/ATLAS-P2-RUN4-01-01-00/mc21_14TeV.601229.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep.simul.HITS.e8481_s4038/HITS.32253544._000008.pool.root.1"
HighPtMinbiasHitsFiles="${ATLAS_REFERENCE_DATA}/PhaseIIUpgrade/HITS/ATLAS-P2-RUN4-01-01-00/mc21_14TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.merge.HITS.e8481_s4038_s4051/*"
LowPtMinbiasHitsFiles="${ATLAS_REFERENCE_DATA}/PhaseIIUpgrade/HITS/ATLAS-P2-RUN4-01-01-00/mc21_14TeV.900311.Epos_minbias_inelastic_lowjetphoton.merge.HITS.e8481_s4038_s4051/*"
DigiOutFileName="RUN4_ttbar.puTruth.RDO.pool.root"

Digi_tf.py \
--CA \
--conditionsTag default:OFLCOND-MC15c-SDR-14-05 \
--preInclude 'HITtoRDO:Campaigns.PhaseIIPileUp1' \
--postInclude 'PyJobTransforms.UseFrontier' \
--runNumber="601229" \
--digiSeedOffset1="74" \
--digiSeedOffset2="74" \
--digiSteeringConf="StandardInTimeOnlyTruth" \
--geometryVersion="all:ATLAS-P2-RUN4-01-01-00" \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber="74" \
--maxEvents ${Events} \
--skipEvents="300" \
--outputRDOFile ${DigiOutFileName} \
--imf False

rc=$?
status=$rc
echo "art-result: $rc digiCA"

rc2=-9999
if [ $rc -eq 0 ]; then
  RunRDOAnalysis.py -i "$DigiOutFileName"
  rc2=$?
  status=$rc2
fi
echo "art-result: $rc2 analysis"

if command -v art.py >/dev/null 2>&1; then
  rc3=-9999
  if [ $rc -eq 0 ]; then
    art.py compare grid --entries 10 "${1}" "${2}" --mode=semi-detailed --file="$DigiOutFileName"
    rc3=$?
    status=$rc3
  fi
  echo "art-result: $rc3 regression"
fi

exit $status
