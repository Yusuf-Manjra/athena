#!/bin/sh

get_files -jo TrigInDetValidation_RTT_topOptions_ElectronSlice.py
athena.py  -c 'XMLDataSet="TrigInDetValidation_el_Zee_pu46_run1";EventMax=4000;' TrigInDetValidation_RTT_topOptions_ElectronSlice.py 


get_files -data TIDAdata11-rtt.dat
get_files -data Test_bin.dat
TIDArdict.exe TIDAdata11-rtt.dat -f data-electron-pileup.root -p 11 -b Test_bin.dat

get_files -data data-el_Zee_pu46_run1-reference.root
TIDAcomparitor.exe data-electron-pileup.root data-el_Zee_pu46_run1-reference.root HLT_e24_medium_L2Star_idperf_TrigL2SiTrackFinder_eGamma_0 HLT_e24_medium_L2Star_idperf_TrigL2SiTrackFinder_eGamma_1 HLT_e24_medium_L2Star_idperf_TrigL2SiTrackFinder_eGamma_2 -d HLTL2-plots

get_files -data data-el_Zee_pu46_run1-reference.root
TIDAcomparitor.exe data-electron-pileup.root data-el_Zee_pu46_run1-reference.root HLT_e5_loose_L2Star_idperf_InDetTrigTrackingxAODCnv_Electron_EFID HLT_e24_medium_L2Star_idperf_InDetTrigTrackingxAODCnv_Electron_EFID -d HLTEF-plots-lowpt

get_files -data data-el_Zee_pu46_run1-reference.root
TIDAcomparitor.exe data-electron-pileup.root data-el_Zee_pu46_run1-reference.root HLT_e5_loose_L2Star_idperf_TrigL2SiTrackFinder_eGamma_0 HLT_e5_loose_L2Star_idperf_TrigL2SiTrackFinder_eGamma_1 HLT_e5_loose_L2Star_idperf_TrigL2SiTrackFinder_eGamma_2 -d HLTL2-plots-lowpt

get_files -data expert-monitoring-el_Zee_pu46_run1-ref.root
TIDAcpucost.exe expert-monitoring.root expert-monitoring-el_Zee_pu46_run1-ref.root --auto -o times

RunTrigCostD3PD.exe -f trig_cost.root --outputTagFromAthena --monitorAllChainSeqAlg --monitorROI --linkOutputDir

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"

TIDAcpucost.exe costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent" check_log.pl --conf checklogTriggerTest.conf %JOBLOG


