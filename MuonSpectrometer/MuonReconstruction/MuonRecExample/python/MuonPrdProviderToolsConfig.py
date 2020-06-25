# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import include

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

source = globalflags.DataSource()

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.CfgGetter import getService,getPrivateTool
from RecExConfig.RecAlgsFlags import recAlgs


def RpcPrepDataProviderTool(name="RpcPrepDataProviderTool",**kwargs):
  global source

  # setup dependencies which are not yet in C++
  import MuonCnvExample.MuonCablingConfig

  if source == 'data':
    kwargs["reduceCablingOverlap"] = True
    kwargs["produceRpcCoinDatafromTriggerWords"] = True
    kwargs["overlap_timeTolerance"] = 1000
    kwargs["solvePhiAmbiguities"] = True
    kwargs["etaphi_coincidenceTime"] = 1000
  elif source == 'geant4':
    pass
  #kwargs["etaphi_coincidenceTime"] = 100
  else:
    raise ValueError( "RpcPrepDataProviderTool: unsupported dataSource %s" % source )

  from MuonRPC_CnvTools.MuonRPC_CnvToolsConf import Muon__RpcRdoToPrepDataToolMT
  if athenaCommonFlags.isOnline: 
      kwargs["ReadKey"] = ""
  return Muon__RpcRdoToPrepDataToolMT(name,**kwargs)


def MdtPrepDataProviderTool(name="MdtPrepDataProviderTool", **kwargs):
  global source,include,getService,getPrivateTool

  # setup dependencies which are not yet in C++
  import MuonCnvExample.MuonCablingConfig
  from MuonCnvExample import MuonCalibConfig
  MuonCalibConfig.setupMdtCondDB()
  include("AmdcAth/AmdcAth_jobOptions.py")
  
  if source == 'data':
    kwargs.setdefault("UseTwin", True)

  from MuonMDT_CnvTools.MuonMDT_CnvToolsConf import Muon__MdtRdoToPrepDataToolMT
  return Muon__MdtRdoToPrepDataToolMT(name,**kwargs)


def TgcPrepDataProviderTool(name="TgcPrepDataProviderTool", **kwargs):
  # setup dependencies which are not yet in C++  
  import MuonCnvExample.MuonCablingConfig

  from MuonTGC_CnvTools.MuonTGC_CnvToolsConf import Muon__TgcRdoToPrepDataToolMT
  return Muon__TgcRdoToPrepDataToolMT(name, **kwargs)
  
  
def CscPrepDataProviderTool(name="CscPrepDataProviderTool", **kwargs):
  # setup dependencies which are not yet in C++
  import MuonCnvExample.MuonCablingConfig

  from MuonCSC_CnvTools.MuonCSC_CnvToolsConf import Muon__CscRdoToCscPrepDataToolMT
  return Muon__CscRdoToCscPrepDataToolMT(name, **kwargs)


################################################################################
# Also define the algorithms factory functions here
# Only CSC one is needed, because the C++ default is wrong
################################################################################

def CscRdoToCscPrepData(name="CscRdoToCscPrepData", **kwargs):
  # TODO: in C++ change default tool to the one set below. Then this whole function can be removed
  # kwargs.setdefault("CscRdoToCscPrepDataTool", "CscPrepDataProviderTool")

  from MuonRdoToPrepData.MuonRdoToPrepDataConf import CscRdoToCscPrepData as Muon__CscRdoToCscPrepData
  return Muon__CscRdoToCscPrepData(name, **kwargs)

#############################################################################
def MM_PrepDataProviderTool(name="MM_PrepDataProviderTool", **kwargs): 
  kwargs.setdefault("ClusterBuilderTool","SimpleMMClusterBuilderTool")
  kwargs.setdefault("NSWCalibTool","NSWCalibTool")
  from MuonMM_CnvTools.MuonMM_CnvToolsConf import Muon__MmRdoToPrepDataTool
  return Muon__MmRdoToPrepDataTool(name,**kwargs)

def STGC_PrepDataProviderTool(name="STGC_PrepDataProviderTool", **kwargs): 
  kwargs.setdefault("ClusterBuilderTool","SimpleSTgcClusterBuilderTool")
  from MuonSTGC_CnvTools.MuonSTGC_CnvToolsConf import Muon__sTgcRdoToPrepDataTool
  return Muon__sTgcRdoToPrepDataTool(name,**kwargs)

### algorithms for other technologies can use C++ defaults


### TODO: remove following backwards compat as soon as all clients have migrated to using CfgGetter
from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,getService,getServiceClone

#if DetFlags.haveRDO.CSC_on() or DetFlags.digitize.CSC_on():
#  CscRdoToPrepDataTool = getPublicTool("CscPrepDataProviderTool")

#if DetFlags.haveRDO.MDT_on() or DetFlags.digitize.MDT_on():
#  MdtRdoToPrepDataTool = getPublicTool("MdtPrepDataProviderTool")

#if DetFlags.haveRDO.RPC_on() or DetFlags.digitize.RPC_on():
#  RpcRdoToPrepDataTool = getPublicTool("RpcPrepDataProviderTool")

#if DetFlags.haveRDO.TGC_on() or DetFlags.digitize.TGC_on():
#  TgcRdoToPrepDataTool = getPublicTool("TgcPrepDataProviderTool")
