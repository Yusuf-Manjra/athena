# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Logging import logging
from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.AppMgr import ServiceMgr
from RecExConfig.Configured import Configured

from RecExConfig.RecFlags import rec

from TrigRoiConversion.TrigRoiConversionConf import RoiWriter


class xAODConversionGetter(Configured):
    def configure(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        #schedule xAOD conversions here
        from TrigBSExtraction.TrigBSExtractionConf import TrigHLTtoxAODConversion
        xaodconverter = TrigHLTtoxAODConversion()
        
        from TrigNavigation.TrigNavigationConfig import HLTNavigationOffline
        xaodconverter.Navigation = HLTNavigationOffline()

        from TrigEDMConfig.TriggerEDM import getPreregistrationList
        from TrigEDMConfig.TriggerEDM import getEFRun1BSList,getEFRun2EquivalentList,getL2Run1BSList,getL2Run2EquivalentList
        xaodconverter.Navigation.ClassesToPreregister = getPreregistrationList(ConfigFlags.Trigger.EDMVersion)
        ## if ConfigFlags.Trigger.EDMVersion == 2:
        ##     #        if TriggerFlags.doMergedHLTResult():
        ##     #if ConfigFlags.Trigger.EDMVersion == 2: #FPP
        ##     xaodconverter.Navigation.ClassesToPreregister = getHLTPreregistrationList()
        ## else:
        ##     xaodconverter.Navigation.ClassesToPreregister = list(set(getL2PreregistrationList()+getEFPreregistrationList()+getHLTPreregistrationList()))

        #we attempt to convert the entire old navigation (L2+EF)
        #xaodconverter.BStoxAOD.ContainersToConvert = list(set(getL2PreregistrationList()+getEFPreregistrationList()))
        # we want only containers from Run 1 with the BS tag
        xaodconverter.BStoxAOD.ContainersToConvert = getL2Run1BSList() + getEFRun1BSList()
        xaodconverter.BStoxAOD.NewContainers = getL2Run2EquivalentList() + getEFRun2EquivalentList()

        xaodconverter.HLTResultKey="HLTResult_EF"
        topSequence += xaodconverter

        # define list of HLT xAOD containers to be written to the output root file
        # (previously this was defined in HLTTriggerResultGetter def configure)
        from TrigEDMConfig.TriggerEDM import getTriggerEDMList
        self.xaodlist = {}
        self.xaodlist.update( getTriggerEDMList(TriggerFlags.ESDEDMSet(), 2 ))

        return True
    
        

class ByteStreamUnpackGetter(Configured):
    def configure(self):
        log = logging.getLogger("ByteStreamUnpackGetter")

        log.info( "TriggerFlags.dataTakingConditions: %s", TriggerFlags.dataTakingConditions() )
        hasHLT = TriggerFlags.dataTakingConditions()=='HltOnly' or TriggerFlags.dataTakingConditions()=='FullTrigger'
        if not hasHLT:
            log.info("Will not configure HLT BS unpacking because dataTakingConditions flag indicates HLT was disabled")
            return True

        # Define the decoding sequence
        from TrigHLTResultByteStream.TrigHLTResultByteStreamConf import HLTResultMTByteStreamDecoderAlg
        from TrigOutputHandling.TrigOutputHandlingConf import TriggerEDMDeserialiserAlg
        from AthenaCommon.CFElements import seqAND
        decoder = HLTResultMTByteStreamDecoderAlg()
        deserialiser = TriggerEDMDeserialiserAlg("TrigDeserialiser")
        deserialiser.ExtraOutputs += [('xAOD::TrigCompositeContainer' , 'StoreGateSvc+HLTNav_Summary')]
        decodingSeq = seqAND("HLTDecodingSeq")
        decodingSeq += decoder  # BS -> HLTResultMT
        decodingSeq += deserialiser  # HLTResultMT -> xAOD

        # Append the decoding sequence to topSequence
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += decodingSeq

        log.debug("Configured HLT result BS decoding sequence")
        return True

class ByteStreamUnpackGetterRun1or2(Configured):
    def configure(self):

        log = logging.getLogger("ByteStreamUnpackGetterRun1or2")
        from AthenaCommon.AlgSequence import AlgSequence 
        topSequence = AlgSequence()
        
        #if TriggerFlags.readBS():
        log.info( "TriggerFlags.dataTakingConditions: %s", TriggerFlags.dataTakingConditions() )
        # in MC this is always FullTrigger
        hasHLT = TriggerFlags.dataTakingConditions()=='HltOnly' or TriggerFlags.dataTakingConditions()=='FullTrigger'
        
        if hasHLT:
            # Decide based on the run number whether to assume a merged, or a
            # split HLT:
            if not TriggerFlags.doMergedHLTResult():
                ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [
                    "HLT::HLTResult/HLTResult_L2",
                    "HLT::HLTResult/HLTResult_EF" ]
            else:
                ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [
                    "HLT::HLTResult/HLTResult_HLT" ]
                pass
            pass

        # BS unpacking
        from TrigBSExtraction.TrigBSExtractionConf import TrigBSExtraction
        extr = TrigBSExtraction()

        # Add fictional output to ensure data dependency in AthenaMT
        extr.ExtraOutputs += [("TrigBSExtractionOutput", "StoreGateSvc+TrigBSExtractionOutput")]
        
        if hasHLT:
            from TrigNavigation.TrigNavigationConfig import HLTNavigationOffline
            extr.Navigation = HLTNavigationOffline()

            from TrigEDMConfig.TriggerEDM import getEDMLibraries
            extr.Navigation.Dlls = getEDMLibraries()            

            from TrigEDMConfig.TriggerEDM import getPreregistrationList
            extr.Navigation.ClassesToPreregister = getPreregistrationList(ConfigFlags.Trigger.EDMVersion)
            
            if TriggerFlags.doMergedHLTResult():
                extr.L2ResultKey=""
                extr.EFResultKey=""
            else:
                extr.HLTResultKey=""

            #
            # Configure DataScouting
            #
            from PyUtils.MetaReaderPeeker import metadata
            if 'stream' in metadata:
                stream_local = metadata['stream']
                if stream_local.startswith('calibration_DataScouting_') or TriggerFlags.doAlwaysUnpackDSResult():
                    if 'calibration' in stream_local and 'DataScouting_' in stream_local:
                        ds_tag = stream_local[12:27]
                        ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/"+ds_tag ]
                        extr.DSResultKeys += [ ds_tag ]

        else:
            #if data doesn't have HLT info set HLTResult keys as empty strings to avoid warnings
            # but the extr alg must run
            extr.L2ResultKey=""
            extr.EFResultKey=""
            extr.HLTResultKey=""
            extr.DSResultKeys=[]

        topSequence += extr
        
        from TrigSerializeTP.TrigSerializeTPConf import TrigSerTPTool
        TrigSerToolTP = TrigSerTPTool('TrigSerTPTool')

        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += TrigSerToolTP
        from TrigEDMConfig.TriggerEDM import getTPList
        TrigSerToolTP.TPMap = getTPList((ConfigFlags.Trigger.EDMVersion))
        
        from TrigSerializeCnvSvc.TrigSerializeCnvSvcConf import TrigSerializeConvHelper
        TrigSerializeConvHelper = TrigSerializeConvHelper(doTP = True)
        ToolSvc += TrigSerializeConvHelper

        return True


class TrigDecisionGetter(Configured):
    def configure(self):
        log = logging.getLogger("TrigDecisionGetter")

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        from TrigDecisionMaker.TrigDecisionMakerConfig import TrigDecisionMakerMT
        tdm = TrigDecisionMakerMT('TrigDecMakerMT')

        if not TriggerFlags.readBS():
            # Construct trigger bits from HLTNav_summary instead of reading from BS
            from TrigOutputHandling.TrigOutputHandlingConf import TriggerBitsMakerTool
            tdm.BitsMakerTool = TriggerBitsMakerTool()

        topSequence += tdm
        log.info('xTrigDecision writing enabled')

        return True

class TrigDecisionGetterRun1or2(Configured):
    #class to setup the writing or just making of TrigDecisionObject
    def configure(self):
        
        log = logging.getLogger("TrigDecisionGetterRun1or2")

        from AthenaCommon.AlgSequence import AlgSequence 
        topSequence = AlgSequence()
        
        #if hasOnlyLVL1:
        #from RecExConfig.ObjKeyStore import objKeyStore
        #objKeyStore.addStreamESD('TrigDec::TrigDecision','TrigDecision')
        #objKeyStore.addStreamAOD('TrigDec::TrigDecision','TrigDecision')
        
        from RecExConfig.RecFlags import rec
        if ( rec.doWriteESD() or rec.doWriteAOD() or rec.doESD() or rec.doAOD() ) and \
               ( not ( rec.readAOD() or rec.readESD() or rec.doWriteBS()) ):
            log.info("Will write TrigDecision object to storegate")
            
            from TrigDecisionMaker.TrigDecisionMakerConfig import WriteTrigDecision
            trigDecWriter = WriteTrigDecision()  # noqa: F841

#           WritexAODTrigDecision() is called within WriteTrigDecision()

            # inform TD maker that some parts may be missing
            if TriggerFlags.dataTakingConditions()=='Lvl1Only':
                topSequence.TrigDecMaker.doL2=False
                topSequence.TrigDecMaker.doEF=False
                topSequence.TrigDecMaker.doHLT=False
                topSequence.TrigNavigationCnvAlg.doL2 = False
                topSequence.TrigNavigationCnvAlg.doEF = False
                topSequence.TrigNavigationCnvAlg.doHLT = False
            elif TriggerFlags.dataTakingConditions()=='HltOnly':
                from AthenaCommon.AlgSequence import AlgSequence
                topSequence.TrigDecMaker.doL1=False
            # Decide based on the doMergedHLTResult to assume a merged, or a
            # split HLT:
            if not TriggerFlags.doMergedHLTResult():
                topSequence.TrigDecMaker.doHLT = False
                topSequence.TrigNavigationCnvAlg.doL2 = False
                topSequence.TrigNavigationCnvAlg.doHLT = False
            else:
                topSequence.TrigDecMaker.doL2 = False
                topSequence.TrigDecMaker.doEF = False
                topSequence.TrigNavigationCnvAlg.doL2 = False
                topSequence.TrigNavigationCnvAlg.doEF = False
                pass
                
        else:
            log.info("Will not write TrigDecision object to storegate")
    
        return True
    
    
class HLTTriggerResultGetter(Configured):

    log = logging.getLogger("HLTTriggerResultGetter.py")

    def _AddOPIToESD(self):

        log = logging.getLogger("HLTTriggerResultGetter.py")        
        
        if rec.doESD():
            from PyUtils.MetaReaderPeeker import metadata
            if 'stream' in metadata:
                stream = metadata['stream']
                log.debug("the stream found in 'metadata' is "+stream)
                if "express" in stream:
                    from TrigEDMConfig.TriggerEDM import getTypeAndKey,EDMDetails
                    type,key=getTypeAndKey("TrigOperationalInfo#HLT_EXPRESS_OPI_HLT")
                    if 'collection'in EDMDetails[type]:
                        colltype = EDMDetails[type]['collection']
                        log.info("Adding HLT_EXPRESS_OPI_HLT to ESD for stream "+stream)
                        from RecExConfig.ObjKeyStore import objKeyStore
                        objKeyStore.addStreamESD(colltype, key)
                    return True
            else:
                log.warning("Could not determine stream of bytestream file, not adding HLT_EXPRESS_OPI_HLT to ESD.")
        return False

    def configure(self):

        log = logging.getLogger("HLTTriggerResultGetter.py")
        from RecExConfig.ObjKeyStore import objKeyStore

        # Set AODFULL for data unless it was set explicitly already
        if TriggerFlags.AODEDMSet.isDefault() and globalflags.DataSource()=='data':
            TriggerFlags.AODEDMSet = 'AODFULL'
            
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        log.info("BS unpacking (TF.readBS): %d", TriggerFlags.readBS() )
        if TriggerFlags.readBS():
            if ConfigFlags.Trigger.EDMVersion == 1 or \
               ConfigFlags.Trigger.EDMVersion == 2:
                bs = ByteStreamUnpackGetterRun1or2()  # noqa: F841
            elif ConfigFlags.Trigger.EDMVersion >=3:
                bs = ByteStreamUnpackGetter()  # noqa: F841
            else:
                raise RuntimeError("Invalid EDMVersion=%s " % ConfigFlags.Trigger.EDMVersion)

        xAODContainers = {}

        if ConfigFlags.Trigger.EDMVersion == 1:
            xaodcnvrt = xAODConversionGetter()
            xAODContainers = xaodcnvrt.xaodlist

        if ConfigFlags.Trigger.EDMVersion == 1 or \
           ConfigFlags.Trigger.EDMVersion == 2:
            if rec.doTrigger() or TriggerFlags.doTriggerConfigOnly():
                tdt = TrigDecisionGetterRun1or2()  # noqa: F841
        elif ConfigFlags.Trigger.EDMVersion >= 3:
            if TriggerFlags.readBS():
                tdt = TrigDecisionGetter()  # noqa: F841
        else:
            raise RuntimeError("Invalid EDMVersion=%s " % ConfigFlags.Trigger.EDMVersion)

        # Temporary hack to add Run-3 navigation to ESD and AOD
        if (rec.doESD() or rec.doAOD()) and ConfigFlags.Trigger.EDMVersion == 3:
            # The hack with wildcards is needed for BS->ESD because we don't know the exact keys
            # of HLT navigation containers before unpacking them from the BS event.
            objKeyStore._store['streamESD'].allowWildCard(True)
            objKeyStore._store['streamAOD'].allowWildCard(True)
            objKeyStore.addManyTypesStreamESD(['xAOD::TrigCompositeContainer#HLTNav*',
                                               'xAOD::TrigCompositeAuxContainer#HLTNav*'])
            objKeyStore.addManyTypesStreamAOD(['xAOD::TrigCompositeContainer#HLTNav*',
                                               'xAOD::TrigCompositeAuxContainer#HLTNav*'])

        # TrigJetRec additions
        if rec.doWriteESD():
            objKeyStore.addStreamESD("JetKeyDescriptor","JetKeyMap")
            objKeyStore.addStreamESD("JetMomentMap","TrigJetRecMomentMap")

        if rec.doWriteAOD():
            objKeyStore.addStreamAOD("JetKeyDescriptor","JetKeyMap")
            objKeyStore.addStreamAOD("JetMomentMap","TrigJetRecMomentMap")
                    
        # ID truth
        if not rec.readESD() and (not rec.readAOD()) and TriggerFlags.doID() \
                and rec.doTruth():
            try:
                from TrigInDetTruthAlgs.TrigInDetTruthAlgsConfig import \
                    TrigIDTruthMaker
                topSequence += TrigIDTruthMaker()
            except Exception:
                log.warning( "Couldn't set up the trigger ID truth maker" )
                pass

        if rec.doESD() or rec.doAOD():
            from TrigEDMConfig.TriggerEDM import getTrigIDTruthList
            objKeyStore.addManyTypesStreamESD(getTrigIDTruthList(TriggerFlags.ESDEDMSet()))
            objKeyStore.addManyTypesStreamAOD(getTrigIDTruthList(TriggerFlags.AODEDMSet()))

        if rec.doAOD() or rec.doWriteAOD():
            # schedule the RoiDescriptorStore conversion
            # log.warning( "HLTTriggerResultGetter - setting up RoiWriter" )
            roiWriter = RoiWriter()
            # Add fictional input to ensure data dependency in AthenaMT
            roiWriter.ExtraInputs += [("TrigBSExtractionOutput", "StoreGateSvc+TrigBSExtractionOutput")]
            topSequence += roiWriter
            # write out the RoiDescriptorStores
            from TrigEDMConfig.TriggerEDMRun2 import TriggerRoiList
            objKeyStore.addManyTypesStreamAOD( TriggerRoiList )

        #Are we adding operational info objects in ESD?
        added=self._AddOPIToESD()
        if added:
            log.debug("Operational Info object HLT_EXPRESS_OPI_HLT with extra information about express stream prescaling added to the data.")
        


        # ESD objects definitions
        _TriggerESDList = {}

        from TrigEDMConfig.TriggerEDM import getTriggerEDMList 
        # we have to store xAOD containers in the root file, NOT AOD,
        # if the xAOD container list is not empty
        if(xAODContainers):
            _TriggerESDList.update( xAODContainers )
        else:
            _TriggerESDList.update( getTriggerEDMList(TriggerFlags.ESDEDMSet(),  ConfigFlags.Trigger.EDMVersion) ) 
        
        log.info("ESD content set according to the ESDEDMSet flag: %s and EDM version %d", TriggerFlags.ESDEDMSet(), ConfigFlags.Trigger.EDMVersion)

        # AOD objects choice
        _TriggerAODList = {}
        
        #from TrigEDMConfig.TriggerEDM import getAODList    
        _TriggerAODList.update( getTriggerEDMList(TriggerFlags.AODEDMSet(),  ConfigFlags.Trigger.EDMVersion) ) 

        log.info("AOD content set according to the AODEDMSet flag: %s and EDM version %d", TriggerFlags.AODEDMSet(),ConfigFlags.Trigger.EDMVersion)

        log.debug("ESD EDM list: %s", _TriggerESDList)
        log.debug("AOD EDM list: %s", _TriggerAODList)
        
        # Highlight what is in AOD list but not in ESD list, as this can cause
        # the "different number of entries in branch" problem, when it is in the
        # AOD list but the empty container per event is not created
        # Just compares keys of dicts, which are the class names, not their string keys in StoreGate
        not_in = [ element for element in  _TriggerAODList if element not in _TriggerESDList ]
        if (len(not_in)>0):
            log.warning("In AOD list but not in ESD list: ")
            log.warning(not_in)
        else:
            log.info("AOD list is subset of ESD list - good.")


        def _addSlimming(stream, edm):
            from TrigNavTools.TrigNavToolsConfig import navigationThinningSvc

            edmlist = list(y.split('-')[0] for x in edm.values() for y in x) #flatten names
          
            svc = navigationThinningSvc ({'name':'HLTNav_%s'%stream, 'mode':'cleanup', 
                                          'result':'HLTResult_HLT',
                                          'features':edmlist})

            from OutputStreamAthenaPool.CreateOutputStreams import registerTrigNavThinningSvc
            registerTrigNavThinningSvc (stream, svc)

            log.info("Configured slimming of HLT for %s", stream)
            print(svc)  # noqa: ATL901
            del edmlist


        if TriggerFlags.doNavigationSlimming() and rec.readRDO() and rec.doWriteAOD():
            _addSlimming('StreamAOD', _TriggerESDList ) #Use ESD item list also for AOD!
            log.info("configured navigation slimming for AOD output")
            
        if TriggerFlags.doNavigationSlimming() and rec.readRDO() and rec.doWriteESD():
            _addSlimming('StreamESD', _TriggerESDList )                
            log.info("configured navigation slimming for ESD output")




        objKeyStore.addManyTypesStreamESD( _TriggerESDList )
        objKeyStore.addManyTypesStreamAOD( _TriggerAODList )        
            
        return True





