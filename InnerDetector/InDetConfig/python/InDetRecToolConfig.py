# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

  if not flags.Detector.RecoTRT:
    kwargs.setdefault("TRTStrawSummarySvc", "")

  kwargs.setdefault("PixelToTPIDTool", None)
  kwargs.setdefault("TestBLayerTool", None)
  kwargs.setdefault("RunningTIDE_Ambi", flags.InDet.doTIDE_Ambi)
  kwargs.setdefault("DoSharedHits", False)
  kwargs.setdefault("usePixel", flags.Detector.RecoPixel)
  kwargs.setdefault("useSCT", flags.Detector.RecoSCT)
  kwargs.setdefault("useTRT", flags.Detector.RecoTRT)

  result.addPublicTool(CompFactory.InDet.InDetTrackSummaryHelperTool(the_name, **kwargs), primary=True)
  return result

def InDetBoundaryCheckToolCfg(flags, name='InDetBoundaryCheckTool', **kwargs):
  result = ComponentAccumulator()

  if 'SctSummaryTool' not in kwargs:
    if flags.Detector.RecoSCT:
      tmpAcc = InDetSCT_ConditionsSummaryToolCfg(flags)
      kwargs.setdefault("SctSummaryTool", tmpAcc.popPrivateTools())
      result.merge(tmpAcc)
    else:
      kwargs.setdefault("SctSummaryTool", None)

  if 'PixelLayerTool' not in kwargs :
    tmpAcc = InDetTestPixelLayerToolCfg(flags)
    kwargs.setdefault("PixelLayerTool", tmpAcc.getPrimary())
    result.merge(tmpAcc)

  kwargs.setdefault("UsePixel", flags.Detector.RecoPixel)
  kwargs.setdefault("UseSCT", flags.Detector.RecoSCT)

  indet_boundary_check_tool = CompFactory.InDet.InDetBoundaryCheckTool(name, **kwargs)
  result.setPrivateTools(indet_boundary_check_tool)
  return result


def InDetTrackHoleSearchToolCfg(flags, name = 'InDetHoleSearchTool', **kwargs):
  result = ComponentAccumulator()
  if 'Extrapolator' not in kwargs:
    tmpAcc =  InDetExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", tmpAcc.getPrimary())
    result.merge(tmpAcc)

  if 'BoundaryCheckTool' not in kwargs:
    tmpAcc = InDetBoundaryCheckToolCfg(flags)
    kwargs.setdefault('BoundaryCheckTool', tmpAcc.popPrivateTools())
    result.merge(tmpAcc)

  if flags.Beam.Type == "cosmics" :
    kwargs.setdefault("Cosmics", True)

  kwargs.setdefault( "CountDeadModulesAfterLastHit" , True)

  indet_hole_search_tool = CompFactory.InDet.InDetTrackHoleSearchTool(name, **kwargs)
  result.addPublicTool(indet_hole_search_tool, primary=True)
  return result


#FIXME: Andi says this and all subtools may become private without needing additional changes
def InDetExtrapolatorCfg(flags, name='InDetExtrapolator', **kwargs) :
    result = ComponentAccumulator()
    # FIXME copied from the old config, also needs fixing on the c++ side.
    if 'Propagators' not in kwargs :
        tmpAcc = InDetPropagatorCfg(flags)
        kwargs.setdefault( "Propagators", [tmpAcc.getPrimary()  ] ) # [ InDetPropagator, InDetStepPropagator ],
        result.merge(tmpAcc)
    propagator= kwargs.get('Propagators')[0].name if kwargs.get('Propagators',None) is not None and len(kwargs.get('Propagators',None))>0 else None

    if 'MaterialEffectsUpdators' not in kwargs :
        tmpAcc = InDetMaterialEffectsUpdatorCfg(flags)
        kwargs.setdefault( "MaterialEffectsUpdators", [tmpAcc.getPrimary() ] )
        result.merge(tmpAcc)
    material_updator= kwargs.get('MaterialEffectsUpdators')[0].name if  kwargs.get('MaterialEffectsUpdators',None) is not None and len(kwargs.get('MaterialEffectsUpdators',None))>0  else None

    if 'Navigator' not in kwargs :
        tmpAcc = InDetNavigatorCfg(flags)
        kwargs.setdefault( "Navigator"               , tmpAcc.getPrimary())
        result.merge(tmpAcc)

    sub_propagators = []
    sub_updators    = []

    # -------------------- set it depending on the geometry ----------------------------------------------------
    # default for ID is (Rk,Mat)
    sub_propagators += [ propagator ]
    sub_updators    += [ material_updator ]

    # default for Calo is (Rk,MatLandau)
    sub_propagators += [ propagator ]
    sub_updators    += [ material_updator ]

    # default for MS is (STEP,Mat)
    #  sub_propagators += [ InDetStepPropagator.name() ]
    sub_updators    += [ material_updator ]
    # @TODO should check that all sub_propagators and sub_updators are actually defined.

    kwargs.setdefault("SubPropagators"          , sub_propagators)
    kwargs.setdefault("SubMEUpdators"           , sub_updators)

    extrapolator = CompFactory.Trk.Extrapolator(name, **kwargs)
    result.addPublicTool(extrapolator, primary=True)
    return result

def PixelConditionsSummaryToolCfg(flags, name = "InDetPixelConditionsSummaryTool", **kwargs):
    #FIXME - fix the duplication in TrigInDetConfig.py and PixelConditionsSummaryConfig.py
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelConfigCondAlgCfg, PixelDCSCondStateAlgCfg, PixelDCSCondStatusAlgCfg

    kwargs.setdefault( "UseByteStream", not flags.Input.isMC)

    if flags.InDet.usePixelDCS:
        kwargs.setdefault( "IsActiveStates", [ 'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED' ] )
        kwargs.setdefault( "IsActiveStatus", [ 'OK', 'WARNING', 'ERROR', 'FATAL' ] )
    
    result = ComponentAccumulator()
    result.merge(PixelConfigCondAlgCfg(flags))
    result.merge(PixelDCSCondStateAlgCfg(flags))
    result.merge(PixelDCSCondStatusAlgCfg(flags))

    result.setPrivateTools(CompFactory.PixelConditionsSummaryTool(name, **kwargs))
    return result


def InDetSCT_ConditionsSummaryToolCfg(flags, name = "InDetSCT_ConditionsSummaryTool", **kwargs) :
  result = ComponentAccumulator()
  
  cfgCondToolAcc = SCT_ConfigurationConditionsToolCfg(flags, name)
  SCT_ConfigurationConditionsTool = cfgCondToolAcc.popPrivateTools()
  result.merge(cfgCondToolAcc)
  if (flags.InDet.doPrintConfigurables):
      print (SCT_ConfigurationConditionsTool)

  # Load calibration conditions tool
  calDataAcc = SCT_ReadCalibDataToolCfg(flags)
  SCT_ReadCalibDataTool = calDataAcc.popPrivateTools()
  result.merge(calDataAcc)
  if (flags.InDet.doPrintConfigurables):
      print (SCT_ReadCalibDataTool)
  
  # Load flagged condition tool
  withFlaggedCondTool=kwargs.pop("withFlaggedCondTool",True)

  if withFlaggedCondTool:
    flCondToolAcc = SCT_FlaggedConditionToolCfg(flags)
    SCT_FlaggedConditionTool = flCondToolAcc.popPrivateTools()
    result.merge(flCondToolAcc)
    if (flags.InDet.doPrintConfigurables):
      print (SCT_FlaggedConditionTool)
  
  # Load conditions Monitoring tool
  if not flags.Common.isOnline:
      monCondAcc = SCT_MonitorConditionsToolCfg(flags)
      SCT_MonitorConditionsTool = monCondAcc.popPrivateTools()
      result.merge(monCondAcc)
      if (flags.InDet.doPrintConfigurables):
          print (SCT_MonitorConditionsTool)

  # Load bytestream errors tool (use default instance without "InDet")
  SCT_BSToolAcc = SCT_ByteStreamErrorsToolCfg(flags, **{"ConfigTool" : SCT_ConfigurationConditionsTool})
  SCT_ByteStreamErrorsTool = SCT_BSToolAcc.popPrivateTools()
  result.merge(SCT_BSToolAcc)
  if (flags.InDet.doPrintConfigurables):
      print (SCT_ByteStreamErrorsTool)
  
  ConditionsTools = []
  if flags.InDet.useSctDCS:
      from SCT_ConditionsTools.SCT_DCSConditionsConfig import SCT_DCSConditionsCfg # FIXME this doesn't seem to have the UseDefaultHV hack from the old config?
      SCT_DCSCondAcc = SCT_DCSConditionsCfg(flags)
      SCT_DCSConditionsTool = SCT_DCSCondAcc.popPrivateTools()
      ConditionsTools += [ SCT_DCSConditionsTool ]
      result.merge(SCT_DCSCondAcc)
      if (flags.InDet.doPrintConfigurables):
          print (SCT_DCSConditionsTool)
  if withFlaggedCondTool:
    ConditionsTools.append(SCT_FlaggedConditionTool)
  if not flags.Input.isMC :
      print ("Conditions db instance is ", flags.IOVDb.DatabaseInstance)
      
      # Configure summary tool
      ConditionsTools +=  [SCT_ConfigurationConditionsTool,]

      ConditionsTools+= [SCT_ByteStreamErrorsTool,
                         SCT_ReadCalibDataTool]

      if kwargs.pop("withTdaqTool", True):
        SCT_TdaqEnabledTool = result.popToolsAndMerge(SCT_TdaqEnabledToolCfg(flags))
        ConditionsTools += [ SCT_TdaqEnabledTool ]
        if (flags.InDet.doPrintConfigurables):
          print (SCT_TdaqEnabledTool)

      if not flags.Common.isOnline:
          ConditionsTools += [ SCT_MonitorConditionsTool ]

  # switch conditions off for SLHC usage
  elif flags.InDet.doSLHC:
      ConditionsTools= []
    
  else :
      # Not SLHC and is MC
      ConditionsTools= [ SCT_ConfigurationConditionsTool,
                         SCT_MonitorConditionsTool,
                         SCT_ReadCalibDataTool]

  if flags.InDet.doSCTModuleVeto:
      ConditionsTools += [ SCT_MonitorConditionsTool ]
  
  kwargs.setdefault("ConditionsTools", ConditionsTools)
  InDetSCT_ConditionsSummaryTool = CompFactory.SCT_ConditionsSummaryTool(name, **kwargs)
  if (flags.InDet.doPrintConfigurables):
      print (InDetSCT_ConditionsSummaryTool)
  result.setPrivateTools(InDetSCT_ConditionsSummaryTool)
  return result

def SCT_ConfigurationConditionsToolCfg(flags, name="SCT_ConfigurationConditionsTool", **kwargs):
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

  result = ComponentAccumulator()

  # For SCT_ID and SCT_DetectorElementCollection used in SCT_ConfigurationCondAlg and SCT_ConfigurationConditionsTool
  from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
  result.merge(SCT_GeometryCfg(flags))

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
  acc = SCT_CablingToolCfg(flags)
  kwargs.setdefault("SCT_CablingTool", acc.popPrivateTools())
  result.merge(acc)

  result.addCondAlgo(CompFactory.SCT_ConfigurationCondAlg(name, **kwargs))
  return result


def SCT_ReadCalibDataToolCfg(flags, name="SCT_ReadCalibDataTool", cond_kwargs={}, **kwargs):
  result = ComponentAccumulator()

  # For SCT_ID and SCT_DetectorElementCollection used in SCT_ReadCalibDataCondAlg and SCT_ReadCalibDataTool
  from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
  result.merge(SCT_GeometryCfg(flags))

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
  acc = SCT_CablingToolCfg(flags)
  kwargs.setdefault("SCT_CablingTool", acc.popPrivateTools())
  result.merge(acc)

  result.setPrivateTools(CompFactory.SCT_ReadCalibDataTool(name,**kwargs))
  return result


def SCT_FlaggedConditionToolCfg(flags, name="SCT_FlaggedConditionTool", **kwargs):
  result = ComponentAccumulator()

  # For SCT_ID and SCT_DetectorElementCollection used in SCT_FlaggedConditionTool
  from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
  result.merge(SCT_GeometryCfg(flags))

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
    from SCT_Cabling.SCT_CablingConfig import SCT_CablingCondAlgCfg
    result = SCT_CablingCondAlgCfg(flags)

    tool = CompFactory.SCT_CablingTool()
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
      tmpAcc = PixelConditionsSummaryToolCfg(flags)
      kwargs.setdefault( "PixelSummaryTool", tmpAcc.getPrimary())
      result.merge(tmpAcc)

  kwargs.setdefault("CheckActiveAreas", flags.InDet.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDeadRegions", flags.InDet.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDisabledFEs", flags.InDet.checkDeadElementsOnTrack)

  tool = CompFactory.InDet.InDetTestPixelLayerTool( name = the_name, **kwargs)
  result.addPublicTool( tool )
  result.setPrivateTools( tool )
  return result

def InDetPropagatorCfg(flags, name='InDetPropagator',**kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  tool = None
  if flags.InDet.propagatorType == "STEP":
    tool = CompFactory.Trk.STEP_Propagator( name = the_name, **kwargs)
  else:
    if flags.InDet.propagatorType == "RungeKutta":
        kwargs.setdefault("AccuracyParameter", 0.0001)
        kwargs.setdefault("MaxStraightLineStep", .004) # Fixes a failed fit
    tool = CompFactory.Trk.RungeKuttaPropagator( name = the_name, **kwargs)

  result.addPublicTool( tool )
  result.setPrivateTools( tool )
  return result

def InDetMaterialEffectsUpdatorCfg(flags, name = "InDetMaterialEffectsUpdator", **kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  if not flags.BField.solenoidOn:
      import AthenaCommon.SystemOfUnits as Units
      kwargs.setdefault(EnergyLoss          = False)
      kwargs.setdefault(ForceMomentum       = True)
      kwargs.setdefault(ForcedMomentumValue = 1000*Units.MeV)

  tool = CompFactory.Trk.MaterialEffectsUpdator( name = the_name, **kwargs)
  result.addPublicTool( tool )
  result.setPrivateTools( tool )
  return result

def InDetNavigatorCfg(flags, name='InDetNavigator', **kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  if 'TrackingGeometrySvc' not in kwargs :
      from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
      tmpAcc = TrackingGeometrySvcCfg(flags)
      kwargs.setdefault("TrackingGeometrySvc", tmpAcc.getPrimary())
      result.merge(tmpAcc)

  tool = CompFactory.Trk.Navigator( name = the_name, **kwargs)
  result.addPublicTool( tool )
  result.setPrivateTools( tool )
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

