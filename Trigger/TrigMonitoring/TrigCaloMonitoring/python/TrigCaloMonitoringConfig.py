# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


# Pay attention this will return a list of tools
# in RAW->ESD and another for ESD->AOD
def HLTCaloMonitoringTool():

  if 'DQMonFlags' not in dir():
    from AthenaMonitoring.DQMonFlags import DQMonFlags
  if DQMonFlags.monManEnvironment == 'tier0Raw':

    list = []
    return list

  #elif DQMonFlags.monManEnvironment == 'tier0ESD':
  elif ( ( DQMonFlags.monManEnvironment == 'tier0ESD' ) or ( DQMonFlags.monManEnvironment == 'tier0' ) ) :
    from TrigCaloMonitoring.TrigCaloMonitoringConf import HLTCaloESD_xAODTrigEMClusters
    from TrigCaloMonitoring.TrigCaloMonitoringConf import HLTCaloESD_xAODCaloClusters

    xAODTrigEMClusters = HLTCaloESD_xAODTrigEMClusters("xAODTrigEMClusters")
    xAODCaloClustersFS   = HLTCaloESD_xAODCaloClusters  ("xAODCaloClustersFS",
                                        HLTContainerRun2 = "HLT_xAOD__CaloClusterContainer_TrigCaloClusterMaker",
                                        HLTContainerRun3 = "HLT_TopoCaloClustersFS")
    xAODCaloClustersRoI  = HLTCaloESD_xAODCaloClusters  ("xAODCaloClustersRoI",
                                        HLTContainerRun2 = "",
                                        HLTContainerRun3 = "HLT_TopoCaloClustersRoI")
    xAODCaloClustersLC   = HLTCaloESD_xAODCaloClusters  ("xAODCaloClustersLC",
                                        HLTContainerRun2 = "",
                                        HLTContainerRun3 = "HLT_TopoCaloClustersLC")
        
    list = [ xAODTrigEMClusters ]
    list += [ xAODCaloClustersFS ]
    list += [ xAODCaloClustersRoI ]
    list += [ xAODCaloClustersLC ]
    
    return list
  else:
    list=[]
    return list

