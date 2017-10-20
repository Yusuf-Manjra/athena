#!/bin/bash
# art-description: art job for mu_Zmumu_IBL_pu40_offline
# art-type: grid
# art-output: HLTL2-plots
# art-output: HLTEF-plots
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.RDO.e3601_s2576_s2132_r7143
# art-output: *.dat 
# art-output: *.root
# art-output: *.log

export RTTJOBNAME=TrigInDetValidation_mu_Zmumu_IBL_pu40_offline

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MuonSlice.py
athena.py  -c 'ARTConfig=["/eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.RDO.e3601_s2576_s2132_r7143/RDO.06718162._000013.pool.root.1"];	    EventMax=2000;runMergedChain=True;doIDNewTracking=True;PdgId=13'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_MuonSlice.py
JOBSTATUS_0=$?

get_files -data TIDAdata11-rtt-offline.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt-offline.dat -f data-muon-pileup-merge.root  -b Test_bin.dat
JOBSTATUS_1=$?

TIDArun-art.sh data-muon-pileup-merge.root data-mu_Zmumu_IBL_pu40_offline-reference.root HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_FTF   -d HLTL2-plots
JOBSTATUS_2=$?

TIDArun-art.sh data-muon-pileup-merge.root data-mu_Zmumu_IBL_pu40_offline-reference.root HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_FTF HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig  -d HLTEF-plots
JOBSTATUS_3=$?

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times
JOBSTATUS_4=$?

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF
JOBSTATUS_5=$?

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir
JOBSTATUS_6=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"
JOBSTATUS_7=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"
JOBSTATUS_8=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"
JOBSTATUS_9=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"
JOBSTATUS_10=$?

