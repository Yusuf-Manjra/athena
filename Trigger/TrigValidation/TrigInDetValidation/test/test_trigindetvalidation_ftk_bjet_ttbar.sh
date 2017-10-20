#!/bin/bash
# art-description: art job for FTK_bjet_ttbar
# art-type: grid
# art-output: HLTL2-plots
# art-output: HLTEF-plots
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.digit.RDO_FTK.e4993_s2887_r8937_r9119
# art-output: *.dat 
# art-output: *.root
# art-output: *.log

export RTTJOBNAME=TrigInDetValidation_FTK_bjet_ttbar

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_BjetSlice.py
athena.py  -c 'ARTConfig=["/eos/atlas/atlascerngroupdisk/proj-sit/trigindet/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.digit.RDO_FTK.e4993_s2887_r8937_r9119/RDO_FTK.10733709._000015.pool.root.1"];	    EventMax=500;runMergedChain=True;doFTK=True'             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_BjetSlice.py
JOBSTATUS_0=$?

get_files -data TIDAdata11-rtt.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -f data-bjet-FTK.root -b Test_bin.dat
JOBSTATUS_1=$?

TIDArun-art.sh data-bjet-FTK.root data-FTK_bjet_ttbar-reference.root HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_Bjet_FTF_forID HLT_j55_boffperf_split_FTKVtx_InDetTrigTrackingxAODCnv_Bjet_FTF HLT_j55_boffperf_split_FTK_InDetTrigTrackingxAODCnv_Bjet_FTK  HLT_j55_boffperf_split_FTKRefit_InDetTrigTrackingxAODCnv_Bjet_FTKRefit -d HLTL2-plots
JOBSTATUS_2=$?

TIDArun-art.sh data-bjet-FTK.root data-FTK_bjet_ttbar-reference.root HLT_j55_boffperf_split_InDetTrigTrackingxAODCnv_Bjet_IDTrig_forID HLT_j55_boffperf_split_FTKVtx_InDetTrigTrackingxAODCnv_Bjet_IDTrig HLT_j55_boffperf_split_FTK_InDetTrigTrackingxAODCnv_Bjet_FTK_IDTrig  HLT_j55_boffperf_split_FTKRefit_InDetTrigTrackingxAODCnv_Bjet_FTKRefit_IDTrig -d HLTEF-plots
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

