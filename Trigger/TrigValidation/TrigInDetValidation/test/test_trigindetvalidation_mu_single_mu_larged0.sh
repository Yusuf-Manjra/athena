#!/bin/bash
# art-description: art job for mu_single_mu_larged0
# art-type: grid
# art-output: HLTEF-plots
# art-output: HLTL2-plots
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  mc15_13TeV.107237.ParticleGenerator_mu_Pt4to100_vertxy20.recon.RDO.e3603_s2726_r7728
# art-output: *.dat 
# art-output: *.root
# art-output: *.log
# art-input-nfiles: 10



export RTTJOBNAME=TrigInDetValidation_mu_single_mu_larged0


fileList="['${ArtInFile//,/', '}']"
echo "List of files = $fileList"

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MuonSlice.py
athena.py  -c "ARTConfig=$fileList;EventMax=30000"             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MuonSlice.py
echo "art-result: $? athena_0"

get_files -data TIDAdata11-rtt-larged0.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin_larged0.dat
TIDArdict.exe TIDAdata11-rtt-larged0.dat -f data-muon.root -p 13  -b Test_bin_larged0.dat
echo "art-result: $? TIDArdict_1"

TIDArun-art.sh data-muon.root data-mu_single_mu_larged0-reference.root HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_FTF  HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig -d HLTEF-plots
echo "art-result: $? TIDArun_2"

TIDArun-art.sh data-muon.root data-mu_single_mu_larged0-reference.root HLT_mu6_idperf_InDetTrigTrackingxAODCnv_Muon_FTF   -d HLTL2-plots
echo "art-result: $? TIDArun_3"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times
echo "art-result: $? TIDArun_4"

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF
echo "art-result: $? TIDArun_5"

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir
echo "art-result: $? RunTrigCostD3PD_6"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_7"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_8"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"
echo "art-result: $? TIDAcpucost_9"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"
echo "art-result: $? TIDAcpucost_10"

