# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# import Hypo Algs/Tools
from TrigMuonHypoMT.TrigMuonHypoMTConf import (  # noqa: F401 (algs not used here)
    TrigMufastHypoAlg, TrigMufastHypoTool,
    TrigmuCombHypoAlg, TrigmuCombHypoTool,
    TrigMuonEFMSonlyHypoAlg, TrigMuonEFMSonlyHypoTool,
    TrigMuisoHypoAlg, TrigMuisoHypoTool,
    TrigMuonEFCombinerHypoAlg, TrigMuonEFCombinerHypoTool,
    TrigMuonEFTrackIsolationHypoAlg, TrigMuonEFTrackIsolationHypoTool,
    TrigL2MuonOverlapRemoverMufastAlg, TrigL2MuonOverlapRemoverMucombAlg, TrigL2MuonOverlapRemoverTool,
    TrigMuonEFInvMassHypoAlg, TrigMuonEFInvMassHypoTool,
    TrigMuonLateMuRoIHypoAlg, TrigMuonLateMuRoIHypoTool
)

# import monitoring
from TrigMuonHypoMT.TrigMuonHypoMonitoringMT import (
    TrigMufastHypoMonitoring,
    TrigmuCombHypoMonitoring,
    TrigMuonEFMSonlyHypoMonitoring,
    TrigMuisoHypoMonitoring,
    TrigMuonEFCombinerHypoMonitoring,
    TrigL2MuonOverlapRemoverMonitoringMufast,
    TrigL2MuonOverlapRemoverMonitoringMucomb,
    TrigMuonEFInvMassHypoMonitoring
)

# other imports
from AthenaCommon.SystemOfUnits import GeV

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigMuonHypoMTConfig')

trigMuonEFSAThresholds = {
    '0GeV'             : [ [0,9.9],              [ 0.100 ] ],
    '2GeV'             : [ [0,9.9],              [ 2.000 ] ],
    '3GeV'             : [ [0,9.9],              [ 3.000 ] ],
    '4GeV'             : [ [0,1.05,1.5,2.0,9.9], [  3.0,  2.5,  2.5,  2.5] ],
    '4GeV_barrelOnly'  : [ [0,1.05,1.5,2.0,9.9], [  3.0,1000.0,1000.0,1000.0]], 
    '5GeV'             : [ [0,1.05,1.5,2.0,9.9], [  4.6,  3.3,  4.0,  4.5] ],
    '6GeV'             : [ [0,1.05,1.5,2.0,9.9], [  5.4,  4.5,  4.9,  5.3] ],
    '7GeV'             : [ [0,1.05,1.5,2.0,9.9], [  6.3,  5.6,  5.6,  6.3] ],
    '8GeV'             : [ [0,1.05,1.5,2.0,9.9], [  7.2,  6.7,  6.4,  7.3] ],
    '10GeV'            : [ [0,1.05,1.5,2.0,9.9], [  8.9,  9.0,  8.4,  9.2] ],
    '11GeV'            : [ [0,1.05,1.5,2.0,9.9], [  9.8, 10.1,  9.3, 10.1] ],
    '12GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 10.6, 11.0, 10.2, 11.0] ], 
    '13GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 11.4, 12.0, 11.1, 12.0] ],
    '14GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 12.2, 13.0, 12.1, 13.0] ],
    '15GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 13.0, 14.0, 13.0, 14.0] ],
    '18GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 15.7, 16.6, 15.4, 16.3] ],
    '20GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 17.5, 18.5, 17.0, 18.0] ],
    '22GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 19.1, 20.0, 18.4, 19.6] ],
    '24GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 20.4, 20.8, 19.3, 21.0] ],
    '26GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 21.6, 22.0, 21.2, 23.7] ], # not optimized
    '30GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 25.0, 24.5, 23.0, 26.0] ],
    '40GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 31.5, 30.0, 28.5, 32.5] ], 
    '40GeV_barrelOnly' : [ [0,1.05,1.5,2.0,9.9], [ 31.5,1000.0,1000.0,1000.0]], 
    '40GeV_uptoEC2'    : [ [0,1.05,1.5,2.0,9.9], [ 31.5, 30.0, 28.5,  1000.0]], 
    '50GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 45.0, 45.0, 45.0, 45.0] ], 
    '50GeV_barrelOnly' : [ [0,1.05,1.5,2.0,9.9], [ 45.0,1000.0,1000.0,1000.0]], 
    '60GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 54.0, 54.0, 54.0, 54.0] ], 
    '60GeV_barrelOnly' : [ [0,1.05,1.5,2.0,9.9], [ 54.0,1000.0,1000.0,1000.0]], 
    '70GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 63.0, 63.0, 63.0, 63.0] ], 
    '80GeV'            : [ [0,1.05,1.5,2.0,9.9], [ 72.0, 72.0, 72.0, 72.0] ],
    '100GeV'           : [ [0,1.05,1.5,2.0,9.9], [ 90.0, 90.0, 90.0, 90.0] ],
   }


efCombinerThresholds = {
    # original + 2015 tuning
    '2GeV_v15a'             : [ [0,9.9], [2.000] ],
    '3GeV_v15a'             : [ [0,9.9], [3.000] ],
    '4GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  3.94,  3.91,  3.77,  3.72] ],
    '5GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  4.91,  4.86,  4.84,  4.83] ],
    '6GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  5.92,  5.86,  5.70,  5.64] ],
    '7GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  6.85,  6.77,  6.74,  6.74] ],
    '8GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  7.89,  7.81,  7.60,  7.53] ],
    '10GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [  9.84,  9.77,  9.54,  9.47] ],
    '11GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 10.74, 10.64, 10.58, 10.53] ],
    '12GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 11.70, 11.59, 11.53, 11.49] ],
    '13GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 12.80, 12.67, 12.43, 12.38] ],
    '14GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 13.75, 13.62, 13.38, 13.36] ],
    '15GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 14.63, 14.49, 14.42, 14.38] ],
    '16GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 15.65, 15.50, 15.39, 15.37] ], # Lidia - extrapolated not optimized 			
    '18GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 17.68, 17.51, 17.34, 17.34] ],
    '20GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 19.65, 19.42, 19.16, 19.19] ],
    '22GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 21.57, 21.32, 21.07, 21.11] ],
    '24GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 23.53, 23.21, 22.99, 23.03] ],
    '26GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 25.49, 25.15, 24.90, 24.95] ],
    '27GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 26.26, 26.12, 26.11, 26.02] ],
    '28GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 27.23, 27.09, 27.07, 26.99] ],
    '30GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 29.17, 29.03, 29.00, 28.92] ],
    '32GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 31.10, 30.96, 30.91, 30.84] ],
    '34GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 33.04, 32.88, 32.81, 32.74] ],
    '35GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 34.14, 33.82, 33.65, 33.65] ], # Lidia - extrapolated not optimized 			
    '36GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 35.23, 34.75, 34.48, 34.55] ],
    '38GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 36.87, 36.67, 36.55, 36.48] ],
    '40GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 38.76, 38.54, 38.38, 38.31] ],
    '50GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 45.00, 45.00, 45.00, 45.00] ],
    '60GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 54.00, 54.00, 54.00, 54.00] ],
    '70GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 63.00, 63.00, 63.00, 63.00] ],
    '80GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 72.00, 72.00, 72.00, 72.00] ],
    '100GeV_v15a'           : [ [0,1.05,1.5,2.0,9.9], [ 90.00, 90.00, 90.00, 90.00] ],
    }


muCombThresholds = {
    # original + 2015 tuning
    '2GeV_v15a'             : [ [0,9.9],              [ 2.000] ],
    '3GeV_v15a'             : [ [0,9.9],              [ 3.000] ],
    '4GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  3.86,  3.77,  3.69,  3.70] ],
    '5GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  4.9,  4.8,  4.8,  4.8] ],
    '6GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  5.87,  5.79,  5.70,  5.62] ],
    '7GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  6.8,  6.7,  6.7,  6.6] ],
    '8GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  7.80,  7.72,  7.59,  7.46] ],
    '10GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [  9.73,  9.63,  9.45,  9.24] ],
    '11GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 10.8, 10.4, 10.6, 10.6] ],
    '12GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 11.7, 11.3, 11.4, 11.5] ],
    '13GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 12.62, 12.48, 12.24, 11.88] ],
    '14GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 13.57, 13.44, 13.21, 12.77] ],
    '15GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 14.5, 14.0, 14.0, 14.5] ],
    '16GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 15.47, 15.09, 14.98, 15.08] ], # Lidia - extrapolated not optimized 			
    '18GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 17.41, 17.27, 16.95, 16.25] ],
    '20GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 19.31, 19.19, 18.80, 17.95] ],
    '22GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 21.19, 21.07, 20.68, 19.71] ],
    '24GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 23.08, 22.99, 22.56, 21.39] ],
    '25GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 24.2, 23.2, 23.2, 22.6] ], 
    '26GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 24.95, 24.86, 24.39, 23.13] ],
    '27GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 26.2, 25.1, 25.1, 24.4] ],
    '28GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 27.1, 26.0, 26.0, 25.2] ],
    '30GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 29.0, 28.0, 28.0, 27.0] ],
    '32GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 30.7, 29.9, 29.9, 28.7] ],
    '34GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 32.5, 31.8, 31.8, 30.4] ],
    '36GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 34.03, 34.29, 33.58, 31.36] ],
    '38GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 35.8, 35.4, 35.4, 33.6] ],
    '40GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 37.5, 37.0, 37.0, 35.0] ],
    '40GeV_slow_v15a'      : [ [0,1.05,1.5,2.0,9.9], [ 40.0, 40.0, 40.0, 40.0] ],
    '50GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 40.0, 40.0, 40.0, 40.0] ],
    '60GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 45.0, 45.0, 45.0, 45.0] ],
    '60GeV_slow_v15a'      : [ [0,1.05,1.5,2.0,9.9], [ 47.0, 47.0, 47.0, 47.0] ],
    '70GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 49.0, 49.0, 49.0, 49.0] ],
    '80GeV_v15a'            : [ [0,1.05,1.5,2.0,9.9], [ 56.0, 56.0, 56.0, 56.0] ],
    '100GeV_v15a'           : [ [0,1.05,1.5,2.0,9.9], [ 70.0, 70.0, 70.0, 70.0] ],
    }


muFastThresholds = {
    # 2011a tuning + 2015 tuning
    '4GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  3.38,  1.25,  3.17,  3.41] ],
    '4GeV_barrelOnly_v15a'   : [ [0,1.05,1.5,2.0,9.9], [  3.38, 1000., 1000., 1000.] ],
    '6GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  5.17,  3.25,  4.69,  5.14] ],
    '8GeV_v15a'              : [ [0,1.05,1.5,2.0,9.9], [  6.63,  5.17,  6.39,  6.81] ],
    '10GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  8.28,  6.35,  7.19,  8.58] ],
    #not optimized: ATR-20049
    '11GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [  8.99,  6.62,  7.40,  9.32] ],
    '13GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 10.42,  7.16,  7.81, 10.80] ],
    '14GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 11.15,  7.58,  8.43, 11.61] ],
    '15GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 11.31, 10.52, 12.00, 13.24] ],
    '18GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 14.33,  9.45, 10.96, 14.35] ],
    '20GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 15.87, 10.73, 12.21, 15.87] ],
    '22GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 17.00, 10.77, 13.38, 17.05] ],
    '24GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 18.24, 11.35, 14.49, 17.91] ],
    '26GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 19.52, 11.61, 15.42, 19.35] ],
    '30GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 17.83, 18.32, 20.46, 23.73] ],
    '36GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 23.94, 12.25, 19.80, 23.17] ],
    '40GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 21.20, 25.38, 29.54] ],
    '50GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 21.20, 25.38, 29.54] ], # lixia, not optimized
    '60GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 21.20, 25.38, 29.54] ],
    '80GeV_v15a'             : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 21.20, 25.38, 29.54] ],
    '40GeV_uptoEC2_v15a'     : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 21.20, 25.38, 1000.] ],
    '40GeV_barrelOnly_v15a'  : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 1000., 1000., 1000.] ],
    '50GeV_barrelOnly_v15a'  : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 1000., 1000., 1000.] ],
    '60GeV_barrelOnly_v15a'  : [ [0,1.05,1.5,2.0,9.9], [ 21.13, 1000., 1000., 1000.] ],
    }


muFastThresholdsForECWeakBRegion = {
    #
    # 2011a tuning + 2015 tuning
    '4GeV_v15a'             : [  2.72,  1.58],
    '4GeV_barrelOnly_v15a'  : [ 1000., 1000. ],
    '6GeV_v15a'             : [  3.91,  2.22],
    '8GeV_v15a'             : [  4.65,  3.26],
    '10GeV_v15a'            : [  5.96,  4.24],
    #not optimized: ATR-20049
    '11GeV_v15a'            : [  6.19,  4.37],
    '13GeV_v15a'            : [  6.65,  4.64],
    '14GeV_v15a'            : [  6.78,  5.03],
    '15GeV_v15a'            : [  7.61,  7.81 ],
    '18GeV_v15a'            : [  8.48,  7.26],
    '20GeV_v15a'            : [  8.63,  7.26],
    '22GeV_v15a'            : [  9.53,  7.77],
    '24GeV_v15a'            : [  9.02,  8.31],
    '26GeV_v15a'            : [  9.89,  8.77],
    '30GeV_v15a'            : [ 14.41, 17.43 ],
    '36GeV_v15a'            : [ 10.78, 10.66],
    '40GeV_v15a'            : [ 15.07, 18.02 ],
    '50GeV_v15a'            : [ 15.07, 18.02 ], # lixia, not optimized
    '60GeV_v15a'            : [ 15.07, 18.02 ],
    '80GeV_v15a'            : [ 15.07, 18.02 ],
    '40GeV_uptoEC2_v15a'    : [ 15.07, 18.02 ],
    '40GeV_barrelOnly_v15a' : [ 1000., 1000. ],
    '50GeV_barrelOnly_v15a' : [ 1000., 1000. ],
    '60GeV_barrelOnly_v15a' : [ 1000., 1000. ],
    }


# Working points for EF track isolation algorithm
# syntax is:
# 'WPname' : cut on 0.3 cone
# put < 0 for no cut
trigMuonEFTrkIsoThresholds = {
    'ivarmedium'      : 0.07, #ivarmedium
    'ivartight'       : 0.06, #ivartight
    'ivarverytight'  : 0.04   #ivarverytight
    }


#Possible dimuon mass window cuts
#Fomat is [lower bound, upper bound] in GeV
# <0 for no cut
trigMuonEFInvMassThresholds = {
    '10invm70' : [10., 70.]
}

def addMonitoring(tool, monClass, name, thresholdHLT ):
    try:
        tool.MonTool = monClass( "MonTool" )
        tool.MonTool.HistPath = name + "/" + thresholdHLT
    except AttributeError:
        log.error('%s Monitoring Tool failed', name)


def getThresholdsFromDict( chainDict ):    
    cparts = [i for i in chainDict['chainParts'] if i['signature']=='Muon' or i['signature']=='Bphysics']
    return sum( [ [part['threshold']]*int(part['multiplicity']) for part in cparts ], [])


def TrigMufastHypoToolFromDict( chainDict ):	

    if 'lateMu' in chainDict['chainParts'][0]['chainPartName']:
       thresholds = ['passthrough']
    else:
        thresholds = getThresholdsFromDict( chainDict )
    config = TrigMufastHypoConfig()
    tool=config.ConfigurationHypoTool( chainDict['chainName'], thresholds )
    # Setup MonTool for monitored variables in AthenaMonitoring package
    addMonitoring( tool, TrigMufastHypoMonitoring, 'TrigMufastHypoTool', chainDict['chainName'] )
    
    return tool


class TrigMufastHypoConfig(object):

    log = logging.getLogger('TrigMufastHypoConfig')

    def ConfigurationHypoTool( self, toolName, thresholds ):

        tool = TrigMufastHypoTool( toolName )  

        nt = len(thresholds)
        log.debug('Set %d thresholds', nt)
        tool.PtBins = [ [ 0, 2.5 ] ] * nt
        tool.PtThresholds = [ [ 5.49 * GeV ] ] * nt
        tool.PtThresholdForECWeakBRegionA = [ 3. * GeV ] * nt
        tool.PtThresholdForECWeakBRegionB = [ 3. * GeV ] * nt

        for th, thvalue in enumerate(thresholds):
            if (thvalue == 'passthrough'):
                tool.PtBins[th] = [-10000.,10000.]
                tool.PtThresholds[th] = [ -1. * GeV ]
                tool.AcceptAll = True
            else:
                if "idperf" in toolName or int(thvalue) < 5:
                    thvaluename =  thvalue + 'GeV_v15a'
                elif "0eta105" in toolName:
                    thvaluename = thvalue+ "GeV_barrelOnly_v15a"
                else:
                    thvaluename = '6GeV_v15a'



                log.debug('Number of threshold = %d, Value of threshold = %s', th, thvaluename)

                try:
                    tool.AcceptAll = False
                    values = muFastThresholds[thvaluename]
                    tool.PtBins[th] = values[0]
                    tool.PtThresholds[th] = [ x * GeV for x in values[1] ]
                    log.debug('Configration of threshold[%d] %s', th, tool.PtThresholds[th])
                    log.debug('Configration of PtBins[%d] %s', th, tool.PtBins[th])
                    if thvaluename in muFastThresholdsForECWeakBRegion:
                        spThres = muFastThresholdsForECWeakBRegion[thvaluename]
                        tool.PtThresholdForECWeakBRegionA[th] = spThres[0] * GeV
                        tool.PtThresholdForECWeakBRegionB[th] = spThres[1] * GeV
                    else:
                        log.debug('No special thresholds for EC weak Bfield regions for %s. Copy EC1 for region A, EC2 for region B.', thvaluename)
                        spThres = values[0][1]
                        if thvaluename == '2GeV' or thvaluename == '3GeV':
                            tool.PtThresholdForECWeakBRegionA[th] = spThres[0] * GeV
                            tool.PtThresholdForECWeakBRegionB[th] = spThres[0] * GeV
                        else:
                            tool.PtThresholdForECWeakBRegionA[th] = spThres[1] * GeV
                            tool.PtThresholdForECWeakBRegionB[th] = spThres[2] * GeV

                        log.debug('Thresholds for A[%d]/B[%d] = %d/%d', th, th, tool.PtThresholdForECWeakBRegionA[th], tool.PtThresholdForECWeakBRegionB[th])

                except LookupError:
                    raise Exception('MuFast Hypo Misconfigured: threshold %r not supported' % thvaluename)

        return tool

### for TrigL2MuonOverlapRemoverMufast
def TrigL2MuonOverlapRemoverMufastToolFromDict( chainDict ):	

    thresholds = getThresholdsFromDict( chainDict )
    config = TrigL2MuonOverlapRemoverMufastConfig()
    tool=config.ConfigurationHypoTool( chainDict['chainName'], thresholds)
    # # Setup MonTool for monitored variables in AthenaMonitoring package
    addMonitoring( tool, TrigL2MuonOverlapRemoverMonitoringMufast, 'TrigL2MuonOverlapRemoverMufastTool', chainDict['chainName'] )
    
    return tool


class TrigL2MuonOverlapRemoverMufastConfig(object):
    
    def ConfigurationHypoTool( self, toolName, thresholds): 
        
        tool = TrigL2MuonOverlapRemoverTool( toolName )  
        tool.Multiplicity = len(thresholds) 
        tool.DoMufastBasedRemoval = True

        # cut defintion
        tool.MufastRequireDR       = True
        tool.MufastRequireMass     = True
        tool.MufastRequireSameSign = True
        # BB
        tool.MufastDRThresBB       = 0.05
        tool.MufastMassThresBB     = 0.20
        # BE
        tool.MufastDRThresBE       = 0.05
        tool.MufastMassThresBE     = 0.20
        # EE
        tool.MufastEtaBinsEC       = [0, 1.9, 2.1, 9.9]
        tool.MufastDRThresEC       = [0.06, 0.05, 0.05]
        tool.MufastMassThresEC     = [0.20, 0.15, 0.10]

        return tool


### for TrigL2MuonOverlapRemoverMucomb
def TrigL2MuonOverlapRemoverMucombToolFromDict( chainDict ):	

    thresholds = getThresholdsFromDict( chainDict )
    config = TrigL2MuonOverlapRemoverMucombConfig()
    tool=config.ConfigurationHypoTool( chainDict['chainName'], thresholds)
    # # Setup MonTool for monitored variables in AthenaMonitoring package
    addMonitoring( tool, TrigL2MuonOverlapRemoverMonitoringMucomb, 'TrigL2MuonOverlapRemoverMucombTool', chainDict['chainName'] )
    
    return tool


class TrigL2MuonOverlapRemoverMucombConfig(object):
    
    def ConfigurationHypoTool( self, toolName, thresholds): 
        
        tool = TrigL2MuonOverlapRemoverTool( toolName )  
        tool.Multiplicity = len(thresholds) 
        tool.DoMucombBasedRemoval = True
        # cut defintion
        tool.MucombRequireDR       = True
        tool.MucombRequireMufastDR = True
        tool.MucombRequireMass     = True
        tool.MucombRequireSameSign = True
        tool.MucombEtaBins         = [0, 0.9, 1.1, 1.9, 2.1, 9.9]
        tool.MucombDRThres         = [0.002, 0.001, 0.002, 0.002, 0.002]
        tool.MucombMufastDRThres   = [0.4,   0.4,   0.4,   0.4,   0.4]
        tool.MucombMassThres       = [0.004, 0.002, 0.006, 0.006, 0.006]

        return tool


def TrigmuCombHypoToolFromDict( chainDict ):

    if 'idperf' in chainDict['chainParts'][0]['chainPartName'] or 'lateMu' in chainDict['chainParts'][0]['chainPartName']:
       thresholds = ['passthrough']
    else:
       thresholds = getThresholdsFromDict( chainDict )
    config = TrigmuCombHypoConfig()
    
    tight = False # can be probably decoded from some of the proprties of the chain, expert work
    tool=config.ConfigurationHypoTool( chainDict['chainName'], thresholds, tight )

    addMonitoring( tool, TrigmuCombHypoMonitoring, "TrigmuCombHypoTool", chainDict['chainName'] )

    return tool

class TrigmuCombHypoConfig(object):

    log = logging.getLogger('TrigmuCombHypoConfig')

    def ConfigurationHypoTool( self, thresholdHLT, thresholds, tight ):

        tool = TrigmuCombHypoTool( thresholdHLT )

        nt = len(thresholds)
        log.debug('Set %d thresholds', nt)
        tool.PtBins = [ [ 0, 2.5 ] ] * nt
        tool.PtThresholds = [ [ 5.83 * GeV ] ] * nt

        for th, thvalue in enumerate(thresholds):
            if thvalue == 'passthrough':
                tool.AcceptAll = True
                tool.PtBins[th] = [-10000.,10000.]
                tool.PtThresholds[th] = [ -1. * GeV ]
            else:
                if int(thvalue) >= 24:
                    thvaluename = '22GeV_v15a'
                else:
                    thvaluename = thvalue + 'GeV_v15a'
                log.debug('Number of threshold = %d, Value of threshold = %s', th, thvaluename)

                try:
                    values = muCombThresholds[thvaluename]
                    tool.PtBins[th] = values[0]
                    tool.PtThresholds[th] = [ x * GeV for x in values[1] ]
                    if (tight is True):
                        tool.ApplyPikCuts        = True
                        tool.MaxPtToApplyPik      = 25.
                        tool.MaxChi2IDPik         = 3.5
                except LookupError:
                    raise Exception('MuComb Hypo Misconfigured: threshold %r not supported' % thvaluename)
        

        return tool 



### for TrigMuisoHypo
def TrigMuisoHypoToolFromDict( chainDict ):

    config = TrigMuisoHypoConfig()
    tool = config.ConfigurationHypoTool( chainDict['chainName'] )
    addMonitoring( tool, TrigMuisoHypoMonitoring,  "TrigMuisoHypoTool", chainDict['chainName'])
    return tool
    

class TrigMuisoHypoConfig(object):

    log = logging.getLogger('TrigMuisoHypoConfig')

    def ConfigurationHypoTool( self, toolName ):	

        tool = TrigMuisoHypoTool( toolName )  
        
        # If configured with passthrough, set AcceptAll flag on, not quite there in the menu
        tool.AcceptAll = False
        if 'passthrough' in toolName:                    
            tool.AcceptAll = True
            log.debug('MuisoHypoConfig configured in pasthrough mode')

        if "FTK" in toolName: # allows us to use different working points in FTK mode
            tool.IDConeSize   = 2
            tool.MaxIDIso_1   = 0.12
            tool.MaxIDIso_2   = 0.12
            tool.MaxIDIso_3   = 0.12  
        else:
            tool.IDConeSize   = 2
            tool.MaxIDIso_1   = 0.1
            tool.MaxIDIso_2   = 0.1
            tool.MaxIDIso_3   = 0.1

        return tool


def TrigMuonEFMSonlyHypoToolFromDict( chainDict ) :
    thresholds = getThresholdsFromDict( chainDict ) 
    config = TrigMuonEFMSonlyHypoConfig()
    tool = config.ConfigurationHypoTool( chainDict['chainName'], thresholds )
    addMonitoring( tool, TrigMuonEFMSonlyHypoMonitoring, "TrigMuonEFMSonlyHypoTool", chainDict['chainName'] )
    return tool

def TrigMuonEFMSonlyHypoToolFromName(chainDict):
    #For full scan chains, we need to configure the thresholds based on all muons
    #in the chain to get the counting correct. Currently a bit convoluted as 
    #the chain dict is (improperly) overwritten when merging for FS chains.
    #Can probably improve this once serial merging is officially implemented
    thresholds=[]
    chainName = chainDict["chainName"]
    hltChainName = chainName[:chainName.index("_L1")]
    cparts = hltChainName.split("_")
    if 'HLT' in hltChainName:
        cparts.remove('HLT')
    for part in cparts:
        if 'mu' in part:
            thr=part.replace('mu','')
            if 'noL1' in part:
                thr =thr.replace('noL1','')
            thresholds.append(thr)
    config = TrigMuonEFMSonlyHypoConfig()
    tool = config.ConfigurationHypoTool(chainDict['chainName'], thresholds)
    addMonitoring( tool, TrigMuonEFMSonlyHypoMonitoring, "TrigMuonEFMSonlyHypoTool", chainDict['chainName'] )
    return tool
    
class TrigMuonEFMSonlyHypoConfig(object):

    log = logging.getLogger('TrigMuonEFMSonlyHypoConfig')

    def ConfigurationHypoTool( self, toolName, thresholds ):

        log = logging.getLogger(self.__class__.__name__)
        tool = TrigMuonEFMSonlyHypoTool( toolName )  

        nt = len(thresholds)
        log.debug('Set %d thresholds', nt)
        tool.PtBins = [ [ 0, 2.5 ] ] * nt
        tool.PtThresholds = [ [ 5.49 * GeV ] ] * nt

        if '3layersEC' in toolName:
            tool.RequireThreeStations=True
        for th, thvalue in enumerate(thresholds):
            if "0eta105" in toolName:
                thvaluename = thvalue+ "GeV_barrelOnly"
            else:
                thvaluename = thvalue + 'GeV'
            log.debug('Number of threshold = %d, Value of threshold = %s', th, thvaluename)

            try:
                tool.AcceptAll = False
                values = trigMuonEFSAThresholds[thvaluename]
                tool.PtBins[th] = values[0]
                tool.PtThresholds[th] = [ x * GeV for x in values[1] ]

            except LookupError:
                if (thvalue=='passthrough'):
                    tool.AcceptAll = True
                    tool.PtBins[th] = [-10000.,10000.]
                    tool.PtThresholds[th] = [ -1. * GeV ]
                else:
                    raise Exception('MuonEFMSonly Hypo Misconfigured: threshold %r not supported' % thvaluename)

        return tool

    
def TrigMuonEFCombinerHypoToolFromDict( chainDict ) :
    if 'idperf' in chainDict['chainParts'][0]['chainPartName']:
       thresholds = ['passthrough']
    else:
       thresholds = getThresholdsFromDict( chainDict )
    config = TrigMuonEFCombinerHypoConfig()
    tool = config.ConfigurationHypoTool( chainDict['chainName'], thresholds )
    addMonitoring( tool, TrigMuonEFCombinerHypoMonitoring, "TrigMuonEFCombinerHypoTool", chainDict['chainName'] )
    return tool

def TrigMuonEFCombinerHypoToolFromName(chainDict):
    #For full scan chains, we need to configure the thresholds based on all muons
    #in the chain to get the counting correct. Currently a bit convoluted as 
    #the chain dict is (improperly) overwritten when merging for FS chains.
    #Can probably improve this once serial merging is officially implemented
    thresholds=[]
    chainName = chainDict["chainName"]
    hltChainName = chainName[:chainName.index("_L1")]
    cparts = hltChainName.split("_")
    if 'HLT' in hltChainName:
        cparts.remove('HLT')
    for part in cparts:
        if 'mu' in part:
            thr=part.replace('mu','')
            if 'noL1' in part:
                thr =thr.replace('noL1','')
            thresholds.append(thr)
    config = TrigMuonEFCombinerHypoConfig()
    tool = config.ConfigurationHypoTool(chainDict['chainName'], thresholds)
    addMonitoring( tool, TrigMuonEFCombinerHypoMonitoring, "TrigMuonEFCombinerHypoTool", chainDict['chainName'] )
    return tool
    
class TrigMuonEFCombinerHypoConfig(object):
        
    log = logging.getLogger('TrigMuonEFCombinerHypoConfig')

    def ConfigurationHypoTool( self, thresholdHLT, thresholds ):

        tool = TrigMuonEFCombinerHypoTool( thresholdHLT )  

        nt = len(thresholds)
        log.debug('Set %d thresholds', nt)
        tool.PtBins = [ [ 0, 2.5 ] ] * nt
        tool.PtThresholds = [ [ 5.49 * GeV ] ] * nt

 
        for th, thvalue in enumerate(thresholds):
            thvaluename = thvalue + 'GeV_v15a'
            log.debug('Number of threshold = %d, Value of threshold = %s', th, thvaluename)

            try:
                tool.AcceptAll = False
                values = efCombinerThresholds[thvaluename]
                tool.PtBins[th] = values[0]
                tool.PtThresholds[th] = [ x * GeV for x in values[1] ]

            except LookupError:
                if (thvalue=='passthrough'):
                    tool.AcceptAll = True
                    tool.PtBins[th] = [-10000.,10000.]
                    tool.PtThresholds[th] = [ -1. * GeV ]
                else:
                    raise Exception('MuonEFCB Hypo Misconfigured: threshold %r not supported' % thvaluename)

        return tool



def TrigMuonEFTrackIsolationHypoToolFromDict( chainDict ) :
    cparts = [i for i in chainDict['chainParts'] if i['signature']=='Muon']
    thresholds = cparts[0]['isoInfo']
    config = TrigMuonEFTrackIsolationHypoConfig()
    tool = config.ConfigurationHypoTool( chainDict['chainName'], thresholds )
    return tool

class TrigMuonEFTrackIsolationHypoConfig(object) :

    log = logging.getLogger('TrigMuonEFTrackIsolationHypoConfig')

    def ConfigurationHypoTool(self, toolName, isoCut):

        tool=TrigMuonEFTrackIsolationHypoTool(toolName)

        try:
            ptcone03 = trigMuonEFTrkIsoThresholds[ isoCut ]

            tool.PtCone02Cut = 0.0
            tool.PtCone03Cut = ptcone03
            tool.AcceptAll = False

            if 'MS' in isoCut:
                tool.RequireCombinedMuon = False
            else:
                tool.RequireCombinedMuon = True

            tool.DoAbsCut = False
            if 'var' in isoCut :
                tool.useVarIso = True
            else :
                tool.useVarIso = False                                
        except LookupError:
            if(isoCut=='passthrough') :
                log.debug('Setting passthrough')
                tool.AcceptAll = True
            else:
                log.error('isoCut = ', isoCut)
                raise Exception('TrigMuonEFTrackIsolation Hypo Misconfigured')
        return tool

def TrigMuonEFInvMassHypoToolFromDict( chainDict ) :
    cparts = [i for i in chainDict['chainParts'] if i['signature']=='Muon']
    thresholds = cparts[0]['invMassInfo']
    config = TrigMuonEFInvMassHypoConfig()
    tool = config.ConfigurationHypoTool( chainDict['chainName'], thresholds )
    addMonitoring( tool, TrigMuonEFInvMassHypoMonitoring, "TrigMuonEFInvMassHypoTool", chainDict['chainName'] )
    return tool

class TrigMuonEFInvMassHypoConfig(object) :

    log = logging.getLogger('TrigMuonEFInvMassHypoConfig')

    def ConfigurationHypoTool(self, toolName, thresholds):

        tool=TrigMuonEFInvMassHypoTool(toolName)

        try:
            massWindow = trigMuonEFInvMassThresholds[thresholds] 

            tool.InvMassLow = massWindow[0]
            tool.InvMassHigh = massWindow[1]
            tool.AcceptAll = False

        except LookupError:
            if(thresholds=='passthrough') :
                log.debug('Setting passthrough')
                tool.AcceptAll = True
            else:
                log.error('threshokds = ', thresholds)
                raise Exception('TrigMuonEFTrackIsolation Hypo Misconfigured')
        return tool

def TrigMuonLateMuRoIHypoToolFromDict( chainDict ) :
    config = TrigMuonLateMuRoIHypoConfig()
    tool = config.ConfigurationHypoTool( chainDict['chainName'] )
    return tool

class TrigMuonLateMuRoIHypoConfig(object) :

    log = logging.getLogger('TrigMuonLateMuRoIHypoConfig')

    def ConfigurationHypoTool(self, toolName):

        tool=TrigMuonLateMuRoIHypoTool(toolName)

        try:
            tool.AcceptAll = False

        except LookupError:
            raise Exception('TrigMuonLateMuRoI Hypo Misconfigured')
        return tool



if __name__ == '__main__':
    # normaly this tools are private and have no clash in naming, for the test we create them and never assign so they are like public,
    # in Run3 config this is checked in a different way so having Run 3 JO behaviour solves test issue
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1 

    configToTest = [ 'HLT_mu6fast_L1MU6',
                     'HLT_mu6Comb_L1MU6',
                     'HLT_mu6_L1MU6',                    
                     'HLT_mu20_ivar_L1MU20',
                     'HLT_2mu6Comb_L12MU6',
                     'HLT_2mu6_L12MU6']

    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
                    
    for c in configToTest:
        log.info("testing config %s", c)
        chainDict = dictFromChainName(c)
        toolMufast = TrigMufastHypoToolFromDict(chainDict)
        assert toolMufast
        toolmuComb = TrigmuCombHypoToolFromDict(chainDict)
        assert toolmuComb
        toolMuiso = TrigMuisoHypoToolFromDict(chainDict)
        assert toolMuiso
        toolEFMSonly = TrigMuonEFMSonlyHypoToolFromDict(chainDict)
        assert toolEFMSonly
        toolEFCombiner = TrigMuonEFCombinerHypoToolFromDict(chainDict)
        assert toolEFCombiner
        
    log.info("All OK")
