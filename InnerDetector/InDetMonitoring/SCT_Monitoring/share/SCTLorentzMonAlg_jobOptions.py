#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file SCTLorentzMonAlg_jobOptions.py
@author Susumu Oda
@date 2019-10-03
@brief Old style configuration of SCTLorentzMonAlg
'''

from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags, "SCTLorentzMonitor")

from SCT_Monitoring.SCT_MonitoringConf import SCTLorentzMonAlg
myMonAlg = helper.addAlgorithm(SCTLorentzMonAlg, "SCTLorentzMonAlg")

myMonAlg.TriggerChain = ''

# Set InDetTrackSummaryTool to TrackSummaryTool of SCTLorentzMonAlg
myMonAlg.TrackSummaryTool = InDetTrackSummaryTool

# Add a generic monitoring tool (a "group" in old language). The returned 
# object here is the standard GenericMonitoringTool.
myMonGroup = helper.addGroup(
    myMonAlg,
    "SCTLorentzMonitor",
    "SCT/GENERAL/"
)

### STEP 5 ###
# Configure histograms
nProfileBins = 360
N_BARRELS = 4
nSidesInclBoth = 3 # 0: Side 0, 1: Side 1, 2: both sides
bothSides = 2
nSurfaces = 3 # 0: 100, 1: 111, 2: all surfaces
surfaceNames = ["_100",   "_111",   ""]
surfaceNames2 = ["_100_",   "_111_",   ""]
surfaceTitles = ["100 - ", "111 - ", ""]
sideNames = ["_0", "_1", ""]
for l in range(N_BARRELS):
    for iSurface in range(nSurfaces):
        for side in range(nSidesInclBoth):
            xVar = "phiToWafer_"+str(l)+surfaceNames[iSurface]+sideNames[side]
            yVar = "nStrip_"+str(l)+surfaceNames[iSurface]+sideNames[side]
            if side==bothSides:
                histTitle = surfaceTitles[iSurface]+"Inc. Angle vs nStrips for Layer"+str(l)
                histName = "h_phiVsNstrips"+surfaceNames2[iSurface]+str(l)
            else:
                histTitle = surfaceTitles[iSurface]+"Inc. Angle vs nStrips for Layer Side"+str(l)+str(side)
                histName = "h_phiVsNstrips"+surfaceNames2[iSurface]+str(l)+"Side"+str(side)
            myMonGroup.defineHistogram(varname=xVar+","+yVar+";"+histName, # ; means alias
                                       type="TProfile",
                                       title=histTitle+";#phi to Wafer;Num of Strips",
                                       path="lorentz", # path cannot be "".
                                       xbins=nProfileBins, xmin=-90., xmax=90.)

topSequence += helper.result()
