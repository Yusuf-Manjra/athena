#!/bin/bash
#
# art-description: Run 4 pile-up presampling
# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-include: 23.0/Athena
# art-include: master/Athena
# art-output: RUN4_presampling.mu1.RDO.pool.root
# art-output: RDOAnalysis.root

if [ -z ${ATLAS_REFERENCE_DATA+x} ]; then
  ATLAS_REFERENCE_DATA="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art"
fi

Events=25
HSHitsFile="${ATLAS_REFERENCE_DATA}/PhaseIIUpgrade/HITS/ATLAS-P2-RUN4-01-01-00/mc21_14TeV.900149.PG_single_nu_Pt50.simul.HITS.e8481_s4038/HITS.32496427._001002.pool.root.1"
HighPtMinbiasHitsFiles="${ATLAS_REFERENCE_DATA}/PhaseIIUpgrade/HITS/ATLAS-P2-RUN4-01-01-00/mc21_14TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.merge.HITS.e8481_s4038_s4045/*"
LowPtMinbiasHitsFiles="${ATLAS_REFERENCE_DATA}/PhaseIIUpgrade/HITS/ATLAS-P2-RUN4-01-01-00/mc21_14TeV.900311.Epos_minbias_inelastic_lowjetphoton.merge.HITS.e8481_s4038_s4045/*"
DigiOutFileName="RUN4_presampling.mu1.RDO.pool.root"

Digi_tf.py \
--CA \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC15c-SDR-14-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-P2-RUN4-01-01-00 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileName} \
--preExec "flags.HGTD.Geometry.useGeoModelXml=True" \
--preInclude 'HITtoRDO:Campaigns.PhaseIIPileUp1' \
--postInclude 'PyJobTransforms.UseFrontier' \
--skipEvents 0

rc=$?
status=$rc
echo "art-result: $rc digiCA"

rc2=-9999
if [ $rc -eq 0 ]; then
  RunRDOAnalysis.py -i "$DigiOutFileName" -p
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
