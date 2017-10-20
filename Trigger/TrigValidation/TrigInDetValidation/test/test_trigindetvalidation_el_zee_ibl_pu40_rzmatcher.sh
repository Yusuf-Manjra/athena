#!/bin/bash
# art-description: art job for el_Zee_IBL_pu40_rzMatcher
# art-type: grid
# art-output: HLTEF-plots
# art-output: HLTL2-plots
# art-output: HLTEF-plots-lowpt
# art-output: HLTL2-plots-lowpt
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  mc15_13TeV.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.recon.RDO.e3601_s2665_s2183_r7191
# art-output: *.dat 
# art-output: *.root
# art-output: *.log

export RTTJOBNAME=TrigInDetValidation_el_Zee_IBL_pu40_rzMatcher

get_files -jo            TrigInDetValidation/TrigInDetValidation_RTT_topOptions_ElectronSlice.py
athena.py  -c 'ARTConfig=["/eos/atlas/atlascerngroupdisk/proj-sit/trigindet/mc15_13TeV.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.recon.RDO.e3601_s2665_s2183_r7191/RDO.06753544._000001.pool.root.1"];           EventMax=5000;runMergedChain=True;PdgId=11'            TrigInDetValidation/TrigInDetValidation_RTT_topOptions_ElectronSlice.py
JOBSTATUS_0=$?

get_files -data TIDAdata11-rtt-rzMatcher.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt-rzMatcher.dat -f data-electron-merge.root -b Test_bin.dat
JOBSTATUS_1=$?

TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40_rzMatcher-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig   -d HLTEF-plots
JOBSTATUS_2=$?

TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40_rzMatcher-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF -d HLTL2-plots
JOBSTATUS_3=$?

TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40_rzMatcher-reference.root  HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig  -d HLTEF-plots-lowpt
JOBSTATUS_4=$?

TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40_rzMatcher-reference.root HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF  -d HLTL2-plots-lowpt
JOBSTATUS_5=$?

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -o times
JOBSTATUS_6=$?

TIDArun-art.sh expert-monitoring.root  expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF
JOBSTATUS_7=$?

RunTrigCostD3PD.exe -f trig_cost.root  --outputTagFromAthena --costMode --linkOutputDir
JOBSTATUS_8=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"
JOBSTATUS_9=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"
JOBSTATUS_10=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"
JOBSTATUS_11=$?

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"
JOBSTATUS_12=$?

