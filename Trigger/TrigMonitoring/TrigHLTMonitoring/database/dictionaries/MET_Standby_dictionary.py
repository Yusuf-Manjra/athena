# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Slice='MET'
RunType='Standby'
Recostep='ESD'
MonitoringTriggers= [ " ",]                        
Config=[]
Config+= [("HLTMETMon", {}),]
PkgName = "TrigMETMonitoring.TrigMETMonitoringConfig"
CreatorName = "HLTMETMonitoringTool"
Script = "print 'Test script'"
