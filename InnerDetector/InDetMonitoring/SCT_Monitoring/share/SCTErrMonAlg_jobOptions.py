#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file SCTErrMonAlg_jobOptions.py
@brief Old style configuration of SCTErrMonAlg
'''

from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags, "SCTErrMonitor")

from SCT_Monitoring.SCT_MonitoringConf import SCTErrMonAlg
myMonAlg = helper.addAlgorithm(SCTErrMonAlg, "SCTErrMonAlg")
myMonAlg.TriggerChain = ""

# Use SCT conditions tools defined in InDetRecConditionsAccess.py
# SCT_ByteStreamErrorsTool
myMonAlg.SCT_ByteStreamErrorsTool = sct_ByteStreamErrorsToolSetup.getTool()
# InDetSCT_ConfigurationConditionsTool
myMonAlg.conditionsTool = InDetSCT_ConfigurationConditionsTool
# SCT_DCSConditionsTool
if InDetFlags.useDCS():
    myMonAlg.SCT_DCSConditionsTool = InDetSCT_DCSConditionsTool
# InDetSCT_ConditionsSummaryTool
myMonAlg.SCT_ConditionsSummaryTool = InDetSCT_ConditionsSummaryTool

# FilterTools
if jobproperties.Beam.beamType()=='collisions':
    from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
    myMonAlg.FilterTools += [GetFilledBunchFilterTool()]

myMonGroup = helper.addGroup(myMonAlg, "SCTErrMonitor", "SCT/")

# Configure histograms

from ROOT import SCT_Monitoring as sctMon

# Filled in fillHistograms
myMonGroup.defineHistogram(varname = "moduleOutBin, moduleOut;SCTConfOutM",
                           type = "TProfile",
                           title = "Num of Out Modules in All Region",
                           path = "GENERAL/Conf",
                           xbins = 1,
                           xmin = -0.5,
                           xmax = 0.5,
                           xlabels = ["Mod Out"])

# Fiiled in fillByteStreamErrors
from ROOT import SCT_ByteStreamErrors
for i in range(SCT_ByteStreamErrors.NUM_ERROR_TYPES):
    myMonGroup.defineHistogram(varname = "lumiBlock, n_"+SCT_ByteStreamErrors.errorTypesDescription[i]+";SCT_"+SCT_ByteStreamErrors.errorTypesDescription[i]+"VsLbs",
                               type = "TProfile",
                               title = "Ave. "+SCT_ByteStreamErrors.errorTypesDescription[i]+" per LB in All Region;LumiBlock;Num of "+SCT_ByteStreamErrors.errorTypesDescription[i],
                               path = "GENERAL/Conf",
                               xbins = sctMon.NBINS_LBs,
                               xmin = 0.5,
                               xmax = sctMon.NBINS_LBs+0.5)

# Filled in fillByteStreamErrorsHelper
myMonGroup.defineHistogram(varname = "maskedLinksBin;Masked Links",
                           weight = "maskedLinks",
                           type = "TH1I",
                           title = "Number of Masked Links for SCT,ECA,B,ECC",
                           path = "GENERAL/errors",
                           xbins = sctMon.N_REGIONS_INC_GENERAL,
                           xmin = -0.5,
                           xmax = sctMon.N_REGIONS_INC_GENERAL-0.5,
                           xlabels = ["EndCapC", "Barrel", "EndCapA", "All"])

# Filled in fillByteStreamErrors
coverageTitles = [
    "", # All (not used)
    "Ave. Coverage of Enabled Links per LB", # All - Disabled
    "Ave. Coverage of Links with No Bad LinkLevelError per LB", # All - BadLinkLevelError
    "Ave. Coverage of Links with No Bad RODLevelError per LB", # All - BadRODLevelError
    "Ave. Coverage of Links with No Bad Error per LB", # All - BadError
    "Ave. Coverage of links Not Affected by PS Trip", # All - PSTrip (DCS)
    "Ave. Coverage of Links With No Bad Problem per LB" # All - Summary
]
for iProblem in range(1, sctMon.numberOfProblemForCoverage):
    myMonGroup.defineHistogram(varname = "lumiBlock, detectorCoverage"+sctMon.coverageVarNames[iProblem]+";SCT_Coverage"+sctMon.coverageVarNames[iProblem]+"VsLbs",
                               type = "TProfile",
                               title = coverageTitles[iProblem]+";LumiBlock;Detector Coverage [%]",
                               path = "DetectorCoverage",
                               xbins = sctMon.NBINS_LBs,
                               xmin = 0.5,
                               xmax = sctMon.NBINS_LBs+0.5)    

# Fiiled in fillByteStreamErrors
myMonGroup.defineHistogram(varname = "lumiBlock, psTripModules;SCT_ModulesWithPSTripVsLbs",
                           type = "TProfile",
                           title = "Ave. Num of Modules Affected by PS Trip per LB in All Region;LumiBlock;Num. of Modules Affected by PS Trip",
                           path = "DetectorCoverage",
                           xbins = sctMon.NBINS_LBs,
                           xmin = 0.5,
                           xmax = sctMon.NBINS_LBs+0.5)

# Filled in fillByteStreamErrorsHelper
xlabels = []
for i in range(SCT_ByteStreamErrors.NUM_ERROR_TYPES):
    xlabels.append(SCT_ByteStreamErrors.errorTypesDescription[i])
for reg in range(sctMon.N_REGIONS):
    nLayers = sctMon.n_layers[reg]*2
    ylabels = []
    for i in range(nLayers):
        ylabels.append(str(i/2)+"_"+str(i%2))
    myMonGroup.defineHistogram(varname = "errorType, layerSide, errorFraction;RateErrorsPerLumi",
                               cutmask = "is"+sctMon.subDetNameShort[reg].Data(),
                               type = "TProfile2D",
                               title = "Rate of Error Types for "+sctMon.layerName[reg].Data()+" per Lumi-Block",
                               path = "SCT"+sctMon.subDetNameShort[reg].Data()+"/errors",
                               xbins = SCT_ByteStreamErrors.NUM_ERROR_TYPES,
                               xmin = -0.5,
                               xmax = SCT_ByteStreamErrors.NUM_ERROR_TYPES-0.5,
                               xlabels = xlabels,
                               ybins = nLayers,
                               ymin = -0.5,
                               ymax = nLayers-0.5,
                               ylabels = ylabels,
                               duration = "lb")

topSequence += helper.result()
