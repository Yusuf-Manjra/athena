#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, CAtoGlobalWrapper
from AthenaConfiguration.AllConfigFlags import ConfigFlags

#Muon RecRoiTools
from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool
from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3TGCRecRoiTool

#Muon trigger threshold tool
from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import getTrigThresholdDecisionTool

def RoIBResultByteStreamToolCfg(name, flags, writeBS=False):
  tool = CompFactory.RoIBResultByteStreamTool(name)
  if flags.Trigger.enableL1MuonPhase1:
    # disable legacy MUCTPI ByteStream decoding/encoding as part of RoIBResult
    tool.MUCTPIModuleId = 0xFF
  if not flags.Trigger.enableL1CaloLegacy:
    # disable legacy L1Calo ByteStream decoding/encoding as part of RoIBResult
    tool.JetModuleIds = []
    tool.EMModuleIds = []

  # TODO: add switches for CTP and L1Topo parts once they can be dropped in some configs

  if writeBS:
    # write BS == read RDO
    tool.RoIBResultReadKey="RoIBResult"
    tool.RoIBResultWriteKey=""
  else:
    # read BS == write RDO
    tool.RoIBResultReadKey=""
    tool.RoIBResultWriteKey="RoIBResult"
  return tool

def ExampleL1TriggerByteStreamToolCfg(name, writeBS=False):
  tool = CompFactory.ExampleL1TriggerByteStreamTool(name)
  from libpyeformat_helper import SourceIdentifier,SubDetector
  muctpi_moduleid = 1
  muctpi_robid = int(SourceIdentifier(SubDetector.TDAQ_MUON_CTP_INTERFACE, muctpi_moduleid))
  tool.MUCTPIModuleId = muctpi_moduleid
  tool.ROBIDs = [muctpi_robid]
  if writeBS:
    # write BS == read xAOD
    tool.MuonRoIContainerReadKey="LVL1MuonRoIs"
    tool.MuonRoIContainerWriteKey=""
  else:
    # read BS == write xAOD
    tool.MuonRoIContainerReadKey=""
    tool.MuonRoIContainerWriteKey="LVL1MuonRoIs"
  return tool

def MuonRoIByteStreamToolCfg(name, flags, writeBS=False):
  tool = CompFactory.MuonRoIByteStreamTool(name)
  from libpyeformat_helper import SourceIdentifier,SubDetector
  muctpi_moduleid = 1
  muctpi_robid = int(SourceIdentifier(SubDetector.TDAQ_MUON_CTP_INTERFACE, muctpi_moduleid))
  tool.MUCTPIModuleId = muctpi_moduleid
  tool.ROBIDs = [muctpi_robid]
  if writeBS:
    # write BS == read xAOD
    tool.MuonRoIContainerReadKey="LVL1MuonRoIs"
    tool.MuonRoIContainerWriteKey=""
  else:
    # read BS == write xAOD
    tool.MuonRoIContainerReadKey=""
    tool.MuonRoIContainerWriteKey="LVL1MuonRoIs"

  tool.UseRun3Config = flags.Trigger.enableL1MuonPhase1
  tool.RPCRecRoiTool = getRun3RPCRecRoiTool(name="RPCRecRoiTool",useRun3Config=flags.Trigger.enableL1MuonPhase1)
  tool.TGCRecRoiTool = getRun3TGCRecRoiTool(name="TGCRecRoiTool",useRun3Config=flags.Trigger.enableL1MuonPhase1)
  tool.TrigThresholdDecisionTool = getTrigThresholdDecisionTool(name="TrigThresholdDecisionTool")

  return tool

def L1TriggerByteStreamDecoderCfg(flags):
  from AthenaCommon.Configurable import Configurable
  cb = Configurable.configurableRun3Behavior
  Configurable.configurableRun3Behavior += 1

  decoderTools = []
  if not flags.Trigger.doLVL1: #if we rerun L1, don't decode the original RoIBResult
    if flags.Trigger.enableL1CaloLegacy or not flags.Trigger.enableL1MuonPhase1:
      roibResultTool = RoIBResultByteStreamToolCfg(name="RoIBResultBSDecoderTool", flags=flags, writeBS=False)
      decoderTools += [roibResultTool]

  if flags.Trigger.enableL1MuonPhase1:
    muonRoiTool = MuonRoIByteStreamToolCfg(name="L1MuonBSDecoderTool", flags=flags, writeBS=False)
    decoderTools += [muonRoiTool]

  decoderAlg = CompFactory.L1TriggerByteStreamDecoderAlg(name="L1TriggerByteStreamDecoder",
                                                         DecoderTools=decoderTools)

  acc = ComponentAccumulator()
  acc.addEventAlgo(decoderAlg, primary=True)

  Configurable.configurableRun3Behavior = cb
  return acc

def L1TriggerByteStreamEncoderCfg(flags):
  acc = ComponentAccumulator()

  if flags.Trigger.enableL1CaloLegacy or not flags.Trigger.enableL1MuonPhase1:
    roibResultTool = RoIBResultByteStreamToolCfg(name="RoIBResultBSEncoderTool", flags=flags, writeBS=True)
    acc.addPublicTool(roibResultTool)
    # Special - in BS->BS job without L1Sim, need to decode extra data from input
    # for encoding the CTP information back to BS
    if flags.Input.Format == 'BS' and not flags.Trigger.doLVL1 and roibResultTool.CTPModuleId != 0xFF:
      if flags.Trigger.Online.isPartition:
          from TrigByteStreamCnvSvc.TrigByteStreamConfig import TrigByteStreamCfg
          bsReadCfg = TrigByteStreamCfg
      else:
          from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
          bsReadCfg = ByteStreamReadCfg
      acc.merge(bsReadCfg(flags, type_names=['CTP_RDO/CTP_RDO']))

  if flags.Trigger.enableL1MuonPhase1:
    muonRoiTool = MuonRoIByteStreamToolCfg(name="L1MuonBSEncoderTool", flags=flags, writeBS=True)
    acc.addPublicTool(muonRoiTool)

  return acc

def L1ByteStreamDecodersRecExSetup():
  # Use new-style config from the above functions and import into old-style JO
  CAtoGlobalWrapper(L1TriggerByteStreamDecoderCfg,ConfigFlags)

def L1ByteStreamEncodersRecExSetup():
  # Use new-style config from the above functions and import into old-style JO
  CAtoGlobalWrapper(L1TriggerByteStreamEncoderCfg,ConfigFlags)
