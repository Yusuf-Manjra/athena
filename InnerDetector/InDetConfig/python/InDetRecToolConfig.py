# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline, addFolders

def InDetPrdAssociationToolCfg(name='InDetPrdAssociationTool',**kwargs) :
  acc = ComponentAccumulator()
  '''
  Provide an instance for all clients in which the tool is only set in c++
  '''
  the_name = makeName( name, kwargs)

  kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'PixelClusterAmbiguitiesMap') # InDetKeys.GangedPixelMap
  kwargs.setdefault("addTRToutliers", True)

  InDetPRD_AssociationToolGangedPixels = CompFactory.InDet.InDetPRD_AssociationToolGangedPixels(the_name, **kwargs)
  acc.setPrivateTools(InDetPRD_AssociationToolGangedPixels)
  return acc

def InDetPrdAssociationTool_setupCfg(name='InDetPrdAssociationTool_setup',**kwargs) :
  '''
  Provide an instance for all clients which set the tool explicitely
  '''
  kwargs.setdefault("SetupCorrect", True)
  return InDetPrdAssociationToolCfg(name, **kwargs)

def InDetTrigPrdAssociationToolCfg(name='InDetTrigPrdAssociationTool_setup',**kwargs) :
  kwargs.setdefault("PixelClusterAmbiguitiesMapName", "TrigPixelClusterAmbiguitiesMap")
  kwargs.setdefault("addTRToutliers", False)

  return InDetPrdAssociationToolCfg(name, **kwargs)

def InDetTrackSummaryHelperToolCfg(flags, name='InDetSummaryHelper', **kwargs):
  result = ComponentAccumulator()

  the_name = makeName( name, kwargs)
  isHLT=kwargs.pop("isHLT",False)

  if 'AssoTool' not in kwargs :
    if not isHLT:
      InDetPrdAssociationTool_setup = result.popToolsAndMerge(InDetPrdAssociationTool_setupCfg())
      result.addPublicTool(InDetPrdAssociationTool_setup)
      kwargs.setdefault("AssoTool", InDetPrdAssociationTool_setup)
    else:
      InDetTrigPrdAssociationTool = result.popToolsAndMerge(InDetTrigPrdAssociationToolCfg())
      result.addPublicTool(InDetTrigPrdAssociationTool)
      kwargs.setdefault("AssoTool", InDetTrigPrdAssociationTool)

  if "HoleSearch" not in kwargs:
    acc = InDetTrackHoleSearchToolCfg(flags)
    # FIXME: assuming we don't use DetailedPixelHoleSearch (since it seems to be off in standard workflows)
    kwargs.setdefault("HoleSearch", acc.getPrimary())
    result.merge(acc)

  if not flags.Detector.EnableTRT:
    kwargs.setdefault("TRTStrawSummarySvc", "")

  kwargs.setdefault("PixelToTPIDTool", None)
  kwargs.setdefault("TestBLayerTool", None)
  kwargs.setdefault("RunningTIDE_Ambi", flags.InDet.doTIDE_Ambi)
  kwargs.setdefault("DoSharedHits", False)
  kwargs.setdefault("usePixel", flags.Detector.EnablePixel)
  kwargs.setdefault("useSCT", flags.Detector.EnableSCT)
  kwargs.setdefault("useTRT", flags.Detector.EnableTRT)

  result.addPublicTool(CompFactory.InDet.InDetTrackSummaryHelperTool(the_name, **kwargs), primary=True)
  return result

def InDetBoundaryCheckToolCfg(flags, name='InDetBoundaryCheckTool', **kwargs):
  result = ComponentAccumulator()

  if 'SctSummaryTool' not in kwargs:
    if flags.Detector.EnableSCT:
      tmpAcc = InDetSCT_ConditionsSummaryToolCfg(flags)
      kwargs.setdefault("SctSummaryTool", tmpAcc.popPrivateTools())
      result.merge(tmpAcc)
    else:
      kwargs.setdefault("SctSummaryTool", None)

  if 'PixelLayerTool' not in kwargs :
    kwargs.setdefault("PixelLayerTool", result.getPrimaryAndMerge(InDetTestPixelLayerToolCfg(flags)))

  kwargs.setdefault("UsePixel", flags.Detector.EnablePixel)
  kwargs.setdefault("UseSCT", flags.Detector.EnableSCT)

  indet_boundary_check_tool = CompFactory.InDet.InDetBoundaryCheckTool(name, **kwargs)
  result.setPrivateTools(indet_boundary_check_tool)
  return result


def InDetTrackHoleSearchToolCfg(flags, name = 'InDetHoleSearchTool', **kwargs):
  result = ComponentAccumulator()
  if 'Extrapolator' not in kwargs:
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    kwargs.setdefault("Extrapolator", result.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

  if 'BoundaryCheckTool' not in kwargs:
    BoundaryCheckTool = result.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags))
    kwargs.setdefault('BoundaryCheckTool', BoundaryCheckTool)

  if flags.Beam.Type == "cosmics" :
    kwargs.setdefault("Cosmics", True)

  kwargs.setdefault( "CountDeadModulesAfterLastHit" , True)

  indet_hole_search_tool = CompFactory.InDet.InDetTrackHoleSearchTool(name, **kwargs)
  result.addPublicTool(indet_hole_search_tool, primary=True)
  return result

def InDetSCT_ConditionsSummaryToolCfg(flags, name = "InDetSCT_ConditionsSummaryTool", **kwargs) :
  result = ComponentAccumulator()


  # Load flagged condition tool
  withFlaggedCondTool=kwargs.pop("withFlaggedCondTool",True)
  withTdaqTool=kwargs.pop("withTdaqTool", True)

  ConditionsTools = []

  cfgCondToolAcc = SCT_ConfigurationConditionsToolCfg(flags)
  SCT_ConfigurationConditionsTool = cfgCondToolAcc.popPrivateTools()
  result.merge(cfgCondToolAcc)
  ConditionsTools += [ SCT_ConfigurationConditionsTool]
  if (flags.InDet.doPrintConfigurables):
    print (SCT_ConfigurationConditionsTool)

  if withFlaggedCondTool:
    flCondToolAcc = SCT_FlaggedConditionToolCfg(flags)
    SCT_FlaggedConditionTool = flCondToolAcc.popPrivateTools()
    result.merge(flCondToolAcc)
    ConditionsTools += [ SCT_FlaggedConditionTool ]
    if (flags.InDet.doPrintConfigurables):
      print (SCT_FlaggedConditionTool)

  # Load bytestream errors tool (use default instance without "InDet")
  if not flags.Input.isMC :
    SCT_BSToolAcc = SCT_ByteStreamErrorsToolCfg(flags, **{"ConfigTool" : SCT_ConfigurationConditionsTool})
    SCT_ByteStreamErrorsTool = SCT_BSToolAcc.popPrivateTools()
    result.merge(SCT_BSToolAcc)
    ConditionsTools+= [ SCT_ByteStreamErrorsTool ]
    if (flags.InDet.doPrintConfigurables):
      print (SCT_ByteStreamErrorsTool)

  if flags.InDet.useSctDCS:
    from SCT_ConditionsTools.SCT_DCSConditionsConfig import SCT_DCSConditionsCfg # FIXME this doesn't seem to have the UseDefaultHV hack from the old config?
    SCT_DCSCondAcc = SCT_DCSConditionsCfg(flags)
    SCT_DCSConditionsTool = SCT_DCSCondAcc.popPrivateTools()
    ConditionsTools += [ SCT_DCSConditionsTool ]
    result.merge(SCT_DCSCondAcc)
    if (flags.InDet.doPrintConfigurables):
      print (SCT_DCSConditionsTool)

  if withTdaqTool and not flags.Input.isMC :
    SCT_TdaqEnabledTool = result.popToolsAndMerge(SCT_TdaqEnabledToolCfg(flags))
    ConditionsTools += [ SCT_TdaqEnabledTool ]
    if (flags.InDet.doPrintConfigurables):
      print (SCT_TdaqEnabledTool)

  # Load calibration conditions tool
  # @TODO or just for data?
  calDataAcc = SCT_ReadCalibDataToolCfg(flags)
  SCT_ReadCalibDataTool = calDataAcc.popPrivateTools()
  result.merge(calDataAcc)
  ConditionsTools += [ SCT_ReadCalibDataTool ]
  if (flags.InDet.doPrintConfigurables):
    print (SCT_ReadCalibDataTool)

  # Load conditions Monitoring tool
  if not flags.Common.isOnline :
      # @TODO really also for MC ?
      SCT_MonitorConditionsTool = result.popToolsAndMerge( SCT_MonitorConditionsToolCfg(flags) )
      ConditionsTools += [ SCT_MonitorConditionsTool ]
      if (flags.InDet.doPrintConfigurables):
          print (SCT_MonitorConditionsTool)

  if flags.InDet.doSCTModuleVeto :
      from SCT_ConditionsTools import SCT_ModuleVetoConfig
      SCT_ModuleVetoTool = result.popToolsAndMerge( SCT_ModuleVetoConfig.SCT_ModuleVetoCfg(flags) )
      ConditionsTools += [ SCT_ModuleVetoTool ]
      if (flags.InDet.doPrintConfigurables):
          print ( SCT_ModuleVetoTool )

  kwargs.setdefault("ConditionsTools", ConditionsTools)
  InDetSCT_ConditionsSummaryTool = CompFactory.SCT_ConditionsSummaryTool(name, **kwargs)
  if (flags.InDet.doPrintConfigurables):
      print (InDetSCT_ConditionsSummaryTool)
  result.setPrivateTools(InDetSCT_ConditionsSummaryTool)
  return result

def SCT_ConfigurationConditionsToolCfg(flags, name="InDetSCT_ConfigurationConditionsTool", **kwargs):
  # Load conditions configuration service and load folders and algorithm for it
  # Load folders that have to exist for both MC and Data
  SCTConfigurationFolderPath='/SCT/DAQ/Config/'
  #if its COMP200, use old folders...
  if (flags.IOVDb.DatabaseInstance == "COMP200"):
      SCTConfigurationFolderPath='/SCT/DAQ/Configuration/'
  #...but now check if we want to override that decision with explicit flag (if there is one)
  if flags.InDet.ForceCoraCool:
      SCTConfigurationFolderPath='/SCT/DAQ/Configuration/'
  if flags.InDet.ForceCoolVectorPayload:
      SCTConfigurationFolderPath='/SCT/DAQ/Config/'
  
  if (flags.InDet.ForceCoolVectorPayload and flags.InDet.ForceCoraCool):
    print ('*** SCT DB CONFIGURATION FLAG CONFLICT: Both CVP and CoraCool selected****')
    SCTConfigurationFolderPath=''

  cond_kwargs = {}
  cond_kwargs["ChannelFolder"] = SCTConfigurationFolderPath+("Chip" if flags.IOVDb.DatabaseInstance=="COMP200" else "ChipSlim")
  cond_kwargs["ModuleFolder"] = SCTConfigurationFolderPath+"Module"
  cond_kwargs["MurFolder"] = SCTConfigurationFolderPath+"MUR"
  cond_kwargs["dbInstance"] = "SCT"
  cond_kwargs["SCT_ConfigurationCondAlgName"] = "SCT_ConfigurationCondAlg"

  # For SCT_ID and SCT_DetectorElementCollection used in SCT_ConfigurationCondAlg and SCT_ConfigurationConditionsTool
  from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
  result = SCT_ReadoutGeometryCfg(flags)

  if 'ChannelFolderDB' not in cond_kwargs:
    result.merge(addFoldersSplitOnline(flags,
                                       detDb=cond_kwargs['dbInstance'],
                                       online_folders=cond_kwargs["ChannelFolder"],
                                       offline_folders=cond_kwargs["ChannelFolder"],
                                       className='CondAttrListVec',
                                       splitMC=True))
  else:
    result.merge(addFolders(flags, [cond_kwargs["ChannelFolderDB"]], detDb = cond_kwargs['dbInstance'], className='CondAttrListVec'))
  if 'ModuleFolderDB' not in cond_kwargs:
    result.merge(addFoldersSplitOnline(flags,
                                       detDb=cond_kwargs['dbInstance'],
                                       online_folders=cond_kwargs["ModuleFolder"],
                                       offline_folders=cond_kwargs["ModuleFolder"],
                                       className='CondAttrListVec',
                                       splitMC=True))
  else:
    result.merge(addFolders(flags, [cond_kwargs["ModuleFolderDB"]], detDb = cond_kwargs['dbInstance'], className='CondAttrListVec'))
  if 'MurFolderDB' not in cond_kwargs:
    result.merge(addFoldersSplitOnline(flags,
                                       detDb=cond_kwargs['dbInstance'],
                                       online_folders=cond_kwargs["MurFolder"],
                                       offline_folders=cond_kwargs["MurFolder"],
                                       className='CondAttrListVec',
                                       splitMC=True))
  else:
    result.merge(addFolders(flags, [cond_kwargs["MurFolderDB"]], detDb = cond_kwargs['dbInstance'],  className='CondAttrListVec'))

  ConfigCondAlg_kwargs={}
  ConfigCondAlg_kwargs["ReadKeyChannel"]=cond_kwargs["ChannelFolder"]
  ConfigCondAlg_kwargs["ReadKeyModule"] =cond_kwargs["ModuleFolder"]
  ConfigCondAlg_kwargs["ReadKeyMur"]    =cond_kwargs["MurFolder"]
  result.merge(SCT_ConfigurationCondAlgCfg(flags,name="SCT_ConfigurationCondAlg", **ConfigCondAlg_kwargs))

  tool = CompFactory.SCT_ConfigurationConditionsTool(name, **kwargs)
  result.setPrivateTools(tool)
  return result

def getSCTDAQConfigFolder(flags) :
  if flags.InDet.ForceCoolVectorPayload and flags.InDet.ForceCoraCool :
        raise Exception('SCT DB CONFIGURATION FLAG CONFLICT: Both CVP and CoraCool selected')  
  return '/SCT/DAQ/Config/' if (    flags.InDet.ForceCoolVectorPayload
                                      or (flags.IOVDb.DatabaseInstance != "COMP200"
                                          and not flags.InDet.ForceCoraCool)) else '/SCT/DAQ/Configuration/'


def SCT_ConfigurationCondAlgCfg(flags, name="SCT_ConfigurationCondAlg", **kwargs):
  result = ComponentAccumulator()
  config_folder_prefix = getSCTDAQConfigFolder(flags)
  channelFolder = config_folder_prefix+("Chip" if flags.IOVDb.DatabaseInstance=="COMP200" else "ChipSlim")
  kwargs.setdefault("ReadKeyChannel", channelFolder)
  kwargs.setdefault("ReadKeyModule", config_folder_prefix+"Module")
  kwargs.setdefault("ReadKeyMur", config_folder_prefix+"MUR")

  result.merge(addFoldersSplitOnline(flags,
                                     detDb="SCT",
                                     online_folders=channelFolder,
                                     offline_folders=channelFolder,
                                     className='CondAttrListVec',
                                     splitMC=True))
  result.merge(addFoldersSplitOnline(flags,
                                     detDb="SCT",
                                     online_folders=config_folder_prefix+"Module",
                                     offline_folders=config_folder_prefix+"Module",
                                     className='CondAttrListVec',
                                     splitMC=True))
  result.merge(addFoldersSplitOnline(flags,
                                     detDb="SCT",
                                     online_folders=config_folder_prefix+"MUR",
                                     offline_folders=config_folder_prefix+"MUR",
                                     className='CondAttrListVec',
                                     splitMC=True))

  SCT_CablingTool = result.popToolsAndMerge(SCT_CablingToolCfg(flags))
  kwargs.setdefault("SCT_CablingTool", SCT_CablingTool)

  SCT_ReadoutTool = result.popToolsAndMerge(SCT_ReadoutToolCfg(flags))
  kwargs.setdefault("SCT_ReadoutTool", SCT_ReadoutTool)

  result.addCondAlgo(CompFactory.SCT_ConfigurationCondAlg(name, **kwargs))
  return result


def SCT_ReadCalibDataToolCfg(flags, name="InDetSCT_ReadCalibDataTool", cond_kwargs={}, **kwargs):
  # For SCT_ID and SCT_DetectorElementCollection used in SCT_ReadCalibDataCondAlg and SCT_ReadCalibDataTool
  from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
  result = SCT_ReadoutGeometryCfg(flags)

  cond_kwargs.setdefault("NoiseFolder","/SCT/DAQ/Calibration/NoiseOccupancyDefects")
  cond_kwargs.setdefault("GainFolder","/SCT/DAQ/Calibration/NPtGainDefects")
  cond_kwargs.setdefault("ReadCalibDataCondAlgName","SCT_ReadCalibDataCondAlg")

  result.merge(addFoldersSplitOnline(flags,
                                     detDb="SCT",
                                     online_folders=cond_kwargs["NoiseFolder"],
                                     offline_folders=cond_kwargs["NoiseFolder"],
                                     className='CondAttrListCollection',
                                     splitMC=True))
  result.merge(addFoldersSplitOnline(flags,
                                     detDb="SCT",
                                     online_folders=cond_kwargs["GainFolder"],
                                     offline_folders=cond_kwargs["GainFolder"],
                                     className='CondAttrListCollection',
                                     splitMC=True))

  result.addCondAlgo(CompFactory.SCT_ReadCalibDataCondAlg(name = cond_kwargs["ReadCalibDataCondAlgName"],
                                                          ReadKeyGain = cond_kwargs["GainFolder"],
                                                          ReadKeyNoise = cond_kwargs["NoiseFolder"]))
  SCT_CablingTool = result.popToolsAndMerge(SCT_CablingToolCfg(flags))
  kwargs.setdefault("SCT_CablingTool", SCT_CablingTool)

  result.setPrivateTools(CompFactory.SCT_ReadCalibDataTool(name,**kwargs))
  return result


def SCT_FlaggedConditionToolCfg(flags, name="InDetSCT_FlaggedConditionTool", **kwargs):
  # For SCT_ID and SCT_DetectorElementCollection used in SCT_FlaggedConditionTool
  from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
  result = SCT_ReadoutGeometryCfg(flags)

  tool = CompFactory.SCT_FlaggedConditionTool(name, **kwargs)
  result.setPrivateTools(tool)
  return result


def SCT_MonitorConditionsToolCfg(flags, name="InDetSCT_MonitorConditionsTool", cond_kwargs={}, **kwargs):
  cond_kwargs.setdefault("Folder", "/SCT/Derived/Monitoring")
  cond_kwargs.setdefault("dbInstance", "SCT_OFL")
  cond_kwargs.setdefault("MonitorCondAlgName", "SCT_MonitorCondAlg")

  result = ComponentAccumulator()

  # For SCT_ID used in SCT_MonitorConditionsTool
  from AtlasGeoModel.GeoModelConfig import GeoModelCfg
  result.merge(GeoModelCfg(flags))

  if  "FolderDb" not in cond_kwargs:
    cond_kwargs["FolderDb"] = cond_kwargs["Folder"]
  result.merge(addFolders(flags, cond_kwargs["FolderDb"], cond_kwargs["dbInstance"], className="CondAttrListCollection"))

  result.addCondAlgo( CompFactory.SCT_MonitorCondAlg(name    = cond_kwargs["MonitorCondAlgName"],
                                                     ReadKey = cond_kwargs["Folder"] ))

  tool = CompFactory.SCT_MonitorConditionsTool(name, **kwargs)
  result.setPrivateTools(tool)

  return result


def SCT_ByteStreamErrorsToolCfg(flags, name="SCT_ByteStreamErrorsTool", **kwargs):
  result = SCT_ConfigurationConditionsToolCfg(flags)
  kwargs.setdefault("ConfigTool", result.popPrivateTools())
  tool = CompFactory.SCT_ByteStreamErrorsTool(name, **kwargs)
  result.setPrivateTools(tool)
  return result

def SCT_CablingToolCfg(flags):
  result = ComponentAccumulator()

  # For SCT_ID used in SCT_CablingTool
  from AtlasGeoModel.GeoModelConfig import GeoModelCfg
  result.merge(GeoModelCfg(flags))

  from SCT_Cabling.SCT_CablingConfig import SCT_CablingCondAlgCfg
  result.merge(SCT_CablingCondAlgCfg(flags))

  tool = CompFactory.SCT_CablingTool()
  result.setPrivateTools(tool)
  return result

def SCT_ReadoutToolCfg(flags, name="SCT_ReadoutTool", **kwargs):
  result = SCT_CablingToolCfg(flags)
  kwargs.setdefault("SCT_CablingTool", result.popPrivateTools())

  tool = CompFactory.SCT_ReadoutTool(**kwargs)
  result.setPrivateTools(tool)
  return result

def SCT_TdaqEnabledToolCfg(flags, name="InDetSCT_TdaqEnabledTool", **kwargs):
  if flags.Input.isMC:
    print("Warning: should not setup SCT_TdaqEnabledCond for MC")
    return

  result = ComponentAccumulator()

  # For SCT_ID used in SCT_TdaqEnabledTool
  from AtlasGeoModel.GeoModelConfig import GeoModelCfg
  result.merge(GeoModelCfg(flags))

  # Folder
  #FIXME - is there a better way to do this? What about run3?
  folder = '/TDAQ/Resources/ATLAS/SCT/Robins' if (flags.IOVDb.DatabaseInstance == "CONDBR2") else '/TDAQ/EnabledResources/ATLAS/SCT/Robins'
  result.merge( addFolders(flags, [folder], detDb="TDAQ", className="CondAttrListCollection") )

  # Algorithm
  kwargs.setdefault( "SCT_CablingTool", result.popToolsAndMerge(SCT_CablingToolCfg(flags)) )
  result.addCondAlgo( CompFactory.SCT_TdaqEnabledCondAlg(**kwargs) )

  # Tool
  result.setPrivateTools(CompFactory.SCT_TdaqEnabledTool(name=name))

  return result

def InDetTestPixelLayerToolCfg(flags, name = "InDetTestPixelLayerTool", **kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  if 'PixelSummaryTool' not in kwargs :
    from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
    kwargs.setdefault("PixelSummaryTool", result.popToolsAndMerge(PixelConditionsSummaryCfg(flags)))

  if 'Extrapolator' not in kwargs :
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    kwargs.setdefault("Extrapolator", result.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

  kwargs.setdefault("CheckActiveAreas", flags.InDet.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDeadRegions", flags.InDet.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDisabledFEs", flags.InDet.checkDeadElementsOnTrack)

  tool = CompFactory.InDet.InDetTestPixelLayerTool( name = the_name, **kwargs)
  result.addPublicTool( tool, primary=True )
  return result

def splitDefaultPrefix(name) :
    default_prefix=''
    for prefix in ['InDet','InDetTrig'] :
        if name[0:len(prefix)] == prefix :
            name=name[len(prefix):]
            default_prefix=prefix
            break
    return default_prefix,name

def makeName( name, kwargs) :
    default_prefix,name=splitDefaultPrefix(name)
    namePrefix=kwargs.pop('namePrefix',default_prefix)
    nameSuffix=kwargs.pop('nameSuffix','')
    return namePrefix + name + nameSuffix

def makeNameGetPreAndSuffix( name, kwargs) :
    default_prefix,name=splitDefaultPrefix(name)
    namePrefix=kwargs.pop('namePrefix',default_prefix)
    nameSuffix=kwargs.pop('nameSuffix','')
    return namePrefix + name + nameSuffix,namePrefix,nameSuffix

