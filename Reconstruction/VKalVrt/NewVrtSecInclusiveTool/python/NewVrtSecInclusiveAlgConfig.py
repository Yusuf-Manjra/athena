# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Author: Vadim Kostyukhin vadim.kostyukhin@cern.ch

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Constants import INFO

def NewVrtSecInclusiveAlgCfg(flags, algname="NVSI_Alg"):
  
   acc = ComponentAccumulator()
   myargs = {}
   from NewVrtSecInclusiveTool.NewVrtSecInclusiveConfig import SoftBFinderToolCfg
   myargs["BVertexTool"] = acc.popToolsAndMerge(SoftBFinderToolCfg(flags,FillHist=True))
   myargs["OutputLevel"] = INFO
   myargs.setdefault("BVertexContainerName","AllBVertices")

   NVSI_Alg = CompFactory.Rec.NewVrtSecInclusiveAlg(algname, **myargs)
   acc.addEventAlgo(NVSI_Alg)
   return acc

def NewVrtSecInclusiveAlgTightCfg(flags, algname="NVSI_Alg_Tight"):
  
   acc = ComponentAccumulator()
   myargs = {}
   from NewVrtSecInclusiveTool.NewVrtSecInclusiveConfig import SoftBFinderToolCfg
   myargs["BVertexTool"] = acc.popToolsAndMerge(SoftBFinderToolCfg(flags,FillHist=False,v2tIniBDTCut=-0.3,v2tFinBDTCut=0.8,cosSVPVCut=0.4))
   myargs["OutputLevel"] = INFO
   myargs.setdefault("BVertexContainerName","NSVI_SecVrt_Tight")

   NVSI_Alg = CompFactory.Rec.NewVrtSecInclusiveAlg(algname, **myargs)
   acc.addEventAlgo(NVSI_Alg)
   return acc

def NewVrtSecInclusiveAlgMediumCfg(flags, algname="NVSI_Alg_Medium"):
  
   acc = ComponentAccumulator()
   myargs = {}
   from NewVrtSecInclusiveTool.NewVrtSecInclusiveConfig import SoftBFinderToolCfg
   myargs["BVertexTool"] = acc.popToolsAndMerge(SoftBFinderToolCfg(flags,FillHist=False,v2tIniBDTCut=-0.6,v2tFinBDTCut=0.2,cosSVPVCut=0.5))
   myargs["OutputLevel"] = INFO
   myargs.setdefault("BVertexContainerName","NSVI_SecVrt_Medium")

   NVSI_Alg = CompFactory.Rec.NewVrtSecInclusiveAlg(algname, **myargs)
   acc.addEventAlgo(NVSI_Alg)
   return acc

def NewVrtSecInclusiveAlgLooseCfg(flags, algname="NVSI_Alg_Loose"):
  
   acc = ComponentAccumulator()
   myargs = {}
   from NewVrtSecInclusiveTool.NewVrtSecInclusiveConfig import SoftBFinderToolCfg
   myargs["BVertexTool"] = acc.popToolsAndMerge(SoftBFinderToolCfg(flags,FillHist=False,v2tIniBDTCut=-0.4,v2tFinBDTCut=-0.3,cosSVPVCut=0.4))
   myargs["OutputLevel"] = INFO
   myargs.setdefault("BVertexContainerName","NSVI_SecVrt_Loose")

   NVSI_Alg = CompFactory.Rec.NewVrtSecInclusiveAlg(algname, **myargs)
   acc.addEventAlgo(NVSI_Alg)
   return acc

