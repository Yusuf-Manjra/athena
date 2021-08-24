# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, ConfigurationError
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from AthenaCommon.Logging import logging


def OutputStreamCfg(configFlags, streamName, ItemList=[], MetadataItemList=[],
                    disableEventTag=False, trigNavThinningSvc=None):
   MakeEventStreamInfo=CompFactory.MakeEventStreamInfo
   AthenaOutputStream=CompFactory.AthenaOutputStream
   AthenaOutputStreamTool=CompFactory.AthenaOutputStreamTool
   StoreGateSvc=CompFactory.StoreGateSvc

   eventInfoKey = "EventInfo"
   if configFlags.Common.ProductionStep == ProductionStep.PileUpPresampling:
      eventInfoKey = configFlags.Overlay.BkgPrefix + "EventInfo"

   msg = logging.getLogger("OutputStreamCfg")
   flagName="Output.%sFileName" % streamName
   if configFlags.hasFlag(flagName):
      fileName=configFlags._get(flagName)
   else:
      fileName="my%s.pool.root" % streamName
      msg.info("No file name predefined for stream %s. Using %s", streamName, fileName)

   if fileName in configFlags.Input.Files:
      raise ConfigurationError("Same name for input and output file %s" % fileName)


   outputAlgName=f"OutputStream{streamName}"

   result=ComponentAccumulator(sequence=CompFactory.AthSequencer("AthOutSeq",StopOverride=True))
   # define athena output stream
   writingTool = AthenaOutputStreamTool( f"Stream{streamName}Tool" )

   outputStream = AthenaOutputStream(
      outputAlgName,
      WritingTool = writingTool,
      ItemList    = [ f"xAOD::EventInfo#{eventInfoKey}", f"xAOD::EventAuxInfo#{eventInfoKey}Aux."  ] + ItemList, 
      MetadataItemList = MetadataItemList,
      OutputFile = fileName,
      )
   outputStream.ExtraOutputs += [("DataHeader", "StoreGateSvc+" + f"Stream{streamName}")]
   result.addService(StoreGateSvc("MetaDataStore"))
   outputStream.MetadataStore = result.getService("MetaDataStore")
   outputStream.MetadataItemList += [
        f"EventStreamInfo#Stream{streamName}",
        "IOVMetaDataContainer#*",
   ]

   streamInfoTool = MakeEventStreamInfo( f"Stream{streamName}_MakeEventStreamInfo" )
   streamInfoTool.Key = f"Stream{streamName}"
   streamInfoTool.EventInfoKey = eventInfoKey
   outputStream.HelperTools.append(streamInfoTool)

   # Make EventFormat object
   eventFormatKey = f"EventFormatStream{streamName}"
   eventFormatTool = CompFactory.xAODMaker.EventFormatStreamHelperTool(
      f"Stream{streamName}_MakeEventFormat",
      Key=eventFormatKey,
   )
   outputStream.HelperTools.append(eventFormatTool)
   msg.debug("Creating event format for this stream")

   # Simplifies naming 
   outputStream.MetadataItemList.append(
      f"xAOD::EventFormat#{eventFormatKey}"
   )

   # setup FileMetaData
   fileMetadataKey = "FileMetaData"
   outputStream.HelperTools.append(
        CompFactory.xAODMaker.FileMetaDataCreatorTool(
            name="FileMetaDataCreatorTool",
            OutputKey=fileMetadataKey,
#            StreamName=f"Stream{streamName}",
            StreamName=streamName,

        )
   )

   # Support for MT thinning.
   thinningCacheTool = CompFactory.getComp ("Athena::ThinningCacheTool") # AthenaServices
   thinningCacheTool = thinningCacheTool (f"ThinningCacheTool_Stream{streamName}",
#                            StreamName = f"Stream{streamName}")
                            StreamName = streamName)

   if trigNavThinningSvc is not None:
      thinningCacheTool.TrigNavigationThinningSvc = trigNavThinningSvc
   outputStream.HelperTools.append(thinningCacheTool)


   outputStream.MetadataItemList += [
        f"xAOD::FileMetaData#{fileMetadataKey}",
        f"xAOD::FileMetaDataAuxInfo#{fileMetadataKey}Aux.",
   ]

   # Event Tag
   if not disableEventTag:
      key = "SimpleTag"
      outputStream.WritingTool.AttributeListKey=key
      # build eventinfo attribute list
      EventInfoAttListTool, EventInfoTagBuilder=CompFactory.getComps("EventInfoAttListTool","EventInfoTagBuilder",)
      tagBuilder = EventInfoTagBuilder(AttributeList=key,
                                       Tool=EventInfoAttListTool(),
                                       EventInfoKey=eventInfoKey)
      result.addEventAlgo(tagBuilder)

   # For xAOD output
   if "AOD" in streamName:
      outputStream.WritingTool.SubLevelBranchName = "<key>"

      AthenaPoolCnvSvc=CompFactory.AthenaPoolCnvSvc
      poolcnvsvc = AthenaPoolCnvSvc()
      result.addService(poolcnvsvc)
      poolcnvsvc.PoolAttributes += [ "DatabaseName = '" + fileName + "'; COMPRESSION_LEVEL = '5'" ]
      poolcnvsvc.PoolAttributes += [ "DatabaseName = '" + fileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '-10000000'" ]
      poolcnvsvc.PoolAttributes += [ "DatabaseName = '" + fileName + "'; ContainerName = 'TTree=CollectionTree'; CONTAINER_SPLITLEVEL = '1'" ]
      poolcnvsvc.PoolAttributes += [ "DatabaseName = '" + fileName + "'; ContainerName = 'TTree=Aux.'; CONTAINER_SPLITLEVEL = '1'"]

   result.addEventAlgo(outputStream)
   return result

def addToESD(configFlags, itemOrList):
   """
   Adds items to ESD stream

   The argument can be either list of items or just one item

   returns CA to be merged i.e.: result.merge(addToESD(flags, "xAOD::CoolObject"))
   """
   if not configFlags.Output.doWriteESD:
      return ComponentAccumulator()
   items = [itemOrList] if isinstance(itemOrList, str) else itemOrList   
   return OutputStreamCfg(configFlags, "ESD", ItemList=items)

def addToAOD(configFlags, itemOrList):
   """
   Adds items to AOD stream

   @see add addToESD
   """
   if not configFlags.Output.doWriteAOD:
      return ComponentAccumulator()
   items = [itemOrList] if isinstance(itemOrList, str) else itemOrList   
   return OutputStreamCfg(configFlags, "AOD", ItemList=items)
