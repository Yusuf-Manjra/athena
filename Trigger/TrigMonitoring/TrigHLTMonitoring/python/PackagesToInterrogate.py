# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Authors: Ben Smart (ben.smart@cern.ch)
# See https://twiki.cern.ch/twiki/bin/view/Atlas/MaDQM for more information

# put all storage of information on tool package names etc. in a single class
class PackagesToInterrogate:

    PackagesToInterrogate = {
        "General"     : { "PackageName" : "TrigHLTMonitoring.TrigHLTMonitoringConfig",         "ToolName" : "HLTGeneralTool",              "MonitCategoryName" : "",                                               "Dll" : "TrigHLTMonitoring"},
        "Egamma"      : { "PackageName" : "TrigEgammaMonitoring.TrigEgammaMonitoringConfig",   "ToolName" : "TrigEgammaMonitoringTool",    "MonitCategoryName" : "TrigEgammaMonitoring.TrigEgammaMonitCategory",   "Dll" : "TrigEgammaAnalysisTools"},
        "IDtrk"       : { "PackageName" : "TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig",     "ToolName" : "TrigIDtrkMonitoringTool",     "MonitCategoryName" : "",                                               "Dll" : "TrigInDetAnalysisExample"},
        "MinBias"     : { "PackageName" : "TrigMinBiasMonitoring.TrigMinBiasMonitoringConfig", "ToolName" : "TrigMinBiasMonitoringTool",   "MonitCategoryName" : "TrigMinBiasMonitoring.TrigMinBiasMonitCategory", "Dll" : "TrigMinBiasMonitoring"},
    }
    #    "Egamma"      : { "PackageName" : "TrigEgammaMonitoring.TrigEgammaMonitoringConfig",   "ToolName" : "HLTEgammaMonitoringTool",     "MonitCategoryName" : "TrigEgammaMonitoring.TrigEgammaMonitCategory",   "Dll" : "TrigEgammaMonitoring"},
    #    "Egamma_Dump" : { "PackageName" : "TrigEgammaMonitoring.TrigEgammaMonitoringConfig",   "ToolName" : "HLTEgammaMonitoringDumpTool", "MonitCategoryName" : "TrigEgammaMonitoring.TrigEgammaMonitCategory",   "Dll" : "TrigEgammaMonitoring"}
    #}



