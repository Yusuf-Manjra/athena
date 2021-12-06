# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1TriggerByteStreamDecoderCfg
from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg, HLTConfigSvcCfg, L1PrescaleCondAlgCfg, HLTPrescaleCondAlgCfg
from TriggerJobOpts.TriggerConfig import triggerPOOLOutputCfg
from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg

from AthenaCommon.Logging import logging
log = logging.getLogger('TriggerRecoConfig')



def TriggerRecoCfg(flags):
    """ Configures trigger data decoding
    Run 3 data:
    HLTResultMTByteStreamDecoderAlg -> TriggerEDMDeserialiserAlg

    Run 2 data:
    TrigBSExtraction -> TrigDecisionMaker -> DecisionConv to xAOD -> NavigationConv to xAOD

    Run 1 data:
    as for Run 2 + Run 1 EDM to xAOD conversion
    """
    acc = ComponentAccumulator()
    acc.merge( ByteStreamReadCfg(flags) )


    acc.merge( L1TriggerByteStreamDecoderCfg(flags) )
    acc.merge( L1ConfigSvcCfg(flags) )
    acc.merge( HLTConfigSvcCfg(flags) )
    acc.merge( L1PrescaleCondAlgCfg(flags) )
    acc.merge( HLTPrescaleCondAlgCfg(flags) )

    if flags.Trigger.EDMVersion == 3:
        acc.merge(Run3TriggerBSUnpackingCfg(flags))
        acc.merge(triggerPOOLOutputCfg(flags))
        tdm = CompFactory.TrigDec.TrigDecisionMakerMT()
        acc.addEventAlgo( tdm )

    elif flags.Trigger.EDMVersion == 2 or flags.Trigger.EDMVersion == 1:

        acc.merge( Run1Run2BSExtractionCfg(flags) )
        from AnalysisTriggerAlgs.AnalysisTriggerAlgsCAConfig import RoIBResultToxAODCfg
        xRoIBResultAcc, _ = RoIBResultToxAODCfg(flags)
        acc.merge( xRoIBResultAcc )
        acc.merge (Run1Run2DecisionMakerCfg(flags) )
        menuwriter = CompFactory.TrigConf.xAODMenuWriterMT()
        menuwriter.KeyWriterTool = CompFactory.TrigConf.KeyWriterTool("KeyWriterToolOffline")
        acc.addEventAlgo( menuwriter )

    else:
        raise RuntimeError("Invalid EDMVersion=%s " % flags.Trigger.EDMVersion)
    if flags.Trigger.EDMVersion == 1:
        pass # TODO add R1 to xAOD conversion



    return acc



def Run1Run2BSExtractionCfg( flags ):
    """Configures Trigger data from BS extraction """
    acc = ComponentAccumulator()
    extr = CompFactory.TrigBSExtraction()

    # Add fictional output to ensure data dependency in AthenaMT
    # Keeping the run 2 workflow, we run this after we have put the full serialised navigation into xAOD
    extr.ExtraInputs += [("xAOD::TrigNavigation", "StoreGateSvc+TrigNavigation")]
    extr.ExtraOutputs += [("TrigBSExtractionOutput", "StoreGateSvc+TrigBSExtractionOutput")]
    if 'HLT' in flags.Trigger.availableRecoMetadata:
        serialiserTool = CompFactory.TrigTSerializer()
        acc.addPublicTool(serialiserTool)
        extr.NavigationForL2 = CompFactory.HLT.Navigation("NavigationForL2", 
                                                          ClassesFromPayloadIgnore = ["TrigPassBits#passbits"]) # Ignore the L2 TrigPassBits to avoid clash with EF (ATR-23411)

        extr.Navigation = CompFactory.HLT.Navigation("Navigation")
        from TrigEDMConfig.TriggerEDM import getEDMLibraries
        extr.Navigation.Dlls = getEDMLibraries()            
        from TrigEDMConfig.TriggerEDM import getPreregistrationList
        extr.Navigation.ClassesToPreregister = getPreregistrationList(flags.Trigger.EDMVersion)
        from eformat import helper as efh
        robIDMap = {}   # map of result keys and their ROB ID
        if flags.Trigger.EDMVersion == 1:  # Run-1 has L2 and EF result
            robIDMap["HLTResult_L2"] = efh.SourceIdentifier(efh.SubDetector.TDAQ_LVL2, 0).code()
            robIDMap["HLTResult_EF"] = efh.SourceIdentifier(efh.SubDetector.TDAQ_EVENT_FILTER, 0).code()
            extr.L2ResultKey = "HLTResult_L2"
            extr.HLTResultKey = "HLTResult_EF"
        else:
            robIDMap["HLTResult_HLT"] = efh.SourceIdentifier(efh.SubDetector.TDAQ_HLT, 0).code()
            extr.L2ResultKey = ""
            extr.HLTResultKey = "HLTResult_HLT"
        # Configure DataScouting
        from PyUtils.MetaReaderPeeker import metadata
        if 'stream' in metadata:
            stream_local = metadata['stream']   # e.g. calibration_DataScouting_05_Jets
            if stream_local.startswith('calibration_DataScouting_'):
                ds_tag = '_'.join(stream_local.split('_')[1:3])   # e.g. DataScouting_05
                ds_id = int(stream_local.split('_')[2])           # e.g. 05
                robIDMap[ds_tag] = efh.SourceIdentifier(efh.SubDetector.TDAQ_HLT, ds_id).code()
                extr.DSResultKeys += [ds_tag]
    else:
        log.info("Will not schedule real HLT bytestream extraction, instead EDM gap filling is running")
        # if data doesn't have HLT info set HLTResult keys as empty strings to avoid warnings
        # but the extraction algorithm must run
        extr.L2ResultKey = ""
        extr.HLTResultKey = ""
        extr.DSResultKeys = []

    HLTResults = [ f"HLT::HLTResult/{k}" for k in robIDMap.keys() ]
    acc.addService( CompFactory.ByteStreamAddressProviderSvc( TypeNames = HLTResults) )


    from TrigEDMConfig.TriggerEDM import getTPList
    acc.addPublicTool( CompFactory.TrigSerTPTool(TPMap = getTPList((flags.Trigger.EDMVersion))) )
    
    acc.addPublicTool( CompFactory.TrigSerializeConvHelper(doTP = True) )

    acc.addPublicTool( CompFactory.HLT.HLTResultByteStreamTool(HLTResultRobIdMap = robIDMap))

    acc.addEventAlgo(extr)

    return acc

def Run3TriggerBSUnpackingCfg(flags):
    """Configures conversions BS -> HLTResultMT -> Collections """
    acc = ComponentAccumulator()
    from AthenaCommon.CFElements import seqAND
    decoder = CompFactory.HLTResultMTByteStreamDecoderAlg()
    deserialiser = CompFactory.TriggerEDMDeserialiserAlg("TrigDeserialiser")
    from TrigDecisionTool.TrigDecisionToolConfig import getRun3NavigationContainerFromInput
    deserialiser.ExtraOutputs += [('xAOD::TrigCompositeContainer' , 'StoreGateSvc+'+getRun3NavigationContainerFromInput(flags))]
    acc.addSequence(seqAND("HLTDecodingSeq"))
    acc.addEventAlgo( decoder, "HLTDecodingSeq")
    acc.addEventAlgo( deserialiser, "HLTDecodingSeq")
    log.debug("Configured HLT result BS decoding sequence")
    return acc

def Run1Run2DecisionMakerCfg(flags):
    """Configures HLTNavigation(tool) -> xAODNavigation and TrigDec::TrigDecision -> xAOD::TrigDecision """
    acc = ComponentAccumulator()
    doL1=True
    doL2=True
    doEF=True
    doHLT=True

    if 'HLT' not in flags.Trigger.availableRecoMetadata:
        doL2=False
        doEF=False
        doHLT=False
    if 'L1' not in flags.Trigger.availableRecoMetadata:
        doL1=False

    if flags.Trigger.EDMVersion == 1:  # Run-1 has L2 and EF result
        doHLT = False
        doL2 = False
    else:
        doL2 = False
        doEF = False

    decMaker = CompFactory.TrigDec.TrigDecisionMaker( "TrigDecMaker", 
                                                      doL1 = doL1,
                                                      doL2 = doL2,
                                                      doEF = doEF,
                                                      doHLT = doHLT)
    acc.addEventAlgo(decMaker)


    from TrigDecisionTool.TrigDecisionToolConfig import TrigDecisionToolCfg
    acc.merge(TrigDecisionToolCfg(flags))

    from TrigConfxAOD.TrigConfxAODConfig import getxAODConfigSvc
    cnvTool = CompFactory.xAODMaker.TrigDecisionCnvTool(TrigConfigSvc = acc.getPrimaryAndMerge( getxAODConfigSvc( flags )) )

    decCnv = CompFactory.xAODMaker.TrigDecisionCnvAlg( xAODKey = "xAODTrigDecision",
                                                         CnvTool = cnvTool)    

    acc.addEventAlgo(decCnv)

    acc.addEventAlgo( CompFactory.xAODMaker.TrigNavigationCnvAlg('TrigNavigationCnvAlg', 
                                                                 doL2 = doL2, 
                                                                 doEF = doEF,
                                                                 doHLT = doHLT))
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags

    flags.fillFromArgs()

    log.info("Checking setup for EDMVersion %d", flags.Trigger.EDMVersion)
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    flags.Input.Files = defaultTestFiles.RAW # need to update this depending on EDMversion
    flags.Exec.MaxEvents=5

    flags.lock()

    acc = MainServicesCfg(flags)
    acc.merge( TriggerRecoCfg(flags) )
    acc.printConfig(withDetails=True)
    with open("TriggerReco.pkl", "wb") as file:
        acc.store(file)
    # TODO decide if we want to run actually
    # sc = acc.run()
    # if sc.isFailure():
    #     import sys
    #     sys.exit(-1)

