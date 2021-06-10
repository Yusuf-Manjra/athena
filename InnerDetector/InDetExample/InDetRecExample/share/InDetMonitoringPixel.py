#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
@file InDetMonitoringPixel.py
@brief Top configuration of Pixel Monitoring in Run 3 style but in Run 2 environment
'''
forceOnline = False # for testing of online monitoring and 100LB histograms

doHitMonAlg       = True
doClusterMonAlg   = True
doErrorMonAlg     = True

from PixelMonitoring.PixelMonitoringConf import PixelAthHitMonAlg
from PixelMonitoring.PixelAthHitMonAlgCfg import PixelAthHitMonAlgCfg

from PixelMonitoring.PixelMonitoringConf import PixelAthClusterMonAlg
from PixelMonitoring.PixelAthClusterMonAlgCfg import PixelAthClusterMonAlgCfg

from PixelMonitoring.PixelMonitoringConf import PixelAthErrorMonAlg
from PixelMonitoring.PixelAthErrorMonAlgCfg import PixelAthErrorMonAlgCfg

from InDetRecExample.InDetKeys import InDetKeys
from InDetRecExample import TrackingCommon

if forceOnline : athenaCommonFlags.isOnline = True
kwargsHitMonAlg = { 'doOnline'        : True if athenaCommonFlags.isOnline() else False,      #Histograms for online (athenaPT) running
                     'doLumiBlock'     : False if athenaCommonFlags.isOnline() else True,       #Turn on/off histograms stored for each lumi block
                     'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam                    
                     'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                     'doHeavyIonMon'   : InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring
                     'doFEPlots'       : True,       #Turn on/off histograms with FE Status information
                     'RDOName'         : InDetKeys.PixelRDOs()
}

kwargsClusMonAlg = { 'doOnline'        : True if athenaCommonFlags.isOnline() else False,      #Histograms for online (athenaPT) running
                      'doLumiBlock'     : False if athenaCommonFlags.isOnline() else True,       #Turn on/off histograms stored for each lumi block
                      'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam
                      'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                      'doHeavyIonMon'   : InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring
                      'doFEPlots'       : True,       #Turn on/off histograms with FE Status information
                      'ClusterName'     : InDetKeys.PixelClusters(),
                      'TrackName'       : InDetKeys.Tracks()
}

kwargsErrMonAlg = { 'doOnline'        : True if athenaCommonFlags.isOnline() else False,      #Histograms for online (athenaPT) running
                     'doLumiBlock'     : False if athenaCommonFlags.isOnline() else True,       #Turn on/off histograms stored for each lumi block
                     'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam                    
                     'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                     'doHeavyIonMon'   : InDetFlags.doHeavyIon()
}
if forceOnline : athenaCommonFlags.isOnline = False

                                                                           
from AthenaMonitoring.DQMonFlags import DQMonFlags                                                                                                                                      
from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags, "NewPixelMonitoring")

if doHitMonAlg:
  pixelAthMonAlgHitMonAlg = helper.addAlgorithm(PixelAthHitMonAlg, 'PixelAthHitMonAlg')
  for k, v in kwargsHitMonAlg.items():
    setattr(pixelAthMonAlgHitMonAlg, k, v)
  PixelAthHitMonAlgCfg(helper, pixelAthMonAlgHitMonAlg, **kwargsHitMonAlg)

if doClusterMonAlg:
  pixelAthClusterMonAlg = helper.addAlgorithm(PixelAthClusterMonAlg, 'PixelAthClusterMonAlg')
  for k, v in kwargsClusMonAlg.items():
    setattr(pixelAthClusterMonAlg, k, v)
  pixelAthClusterMonAlg.HoleSearchTool   = TrackingCommon.getInDetHoleSearchTool()
  pixelAthClusterMonAlg.TrackSelectionTool.UseTrkTrackTools = True
  pixelAthClusterMonAlg.TrackSelectionTool.CutLevel         = "TightPrimary"
  pixelAthClusterMonAlg.TrackSelectionTool.maxNPixelHoles   = 1
  pixelAthClusterMonAlg.TrackSelectionTool.maxD0            = 2
  pixelAthClusterMonAlg.TrackSelectionTool.maxZ0            = 150
  pixelAthClusterMonAlg.TrackSelectionTool.TrackSummaryTool = TrackingCommon.getInDetTrackSummaryTool()
  pixelAthClusterMonAlg.TrackSelectionTool.Extrapolator     = TrackingCommon.getInDetExtrapolator()
  
  #print getattr(pixelAthClusterMonAlg, 'onTrack') 
  PixelAthClusterMonAlgCfg(helper, pixelAthClusterMonAlg, **kwargsClusMonAlg)

if doErrorMonAlg:
  pixelAthMonAlgErrorMonAlg = helper.addAlgorithm(PixelAthErrorMonAlg, 'PixelAthErrorMonAlg')
  for k, v in kwargsErrMonAlg.items():
    setattr(pixelAthMonAlgErrorMonAlg, k, v)
  PixelAthErrorMonAlgCfg(helper, pixelAthMonAlgErrorMonAlg, **kwargsErrMonAlg)

topSequence += helper.result()



