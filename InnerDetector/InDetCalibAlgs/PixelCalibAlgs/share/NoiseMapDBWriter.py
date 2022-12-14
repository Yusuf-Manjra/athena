import AthenaCommon.AtlasUnixGeneratorJob



### configure the special pixel map service

from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc

SpecialPixelMapSvc = SpecialPixelMapSvc()
ServiceMgr += SpecialPixelMapSvc

if not 'DifferentialUpdates' in dir() :
  #DifferentialUpdates = True
  DifferentialUpdates = False

SpecialPixelMapSvc.DifferentialUpdates = DifferentialUpdates

SpecialPixelMapSvc.OutputFolder = "/PIXEL/NoiseMapShort"
SpecialPixelMapSvc.OutputLongFolder = "/PIXEL/NoiseMapLong"
SpecialPixelMapSvc.UseDualFolderStructure = True

## globalflags

from AthenaCommon.GlobalFlags import globalflags

globalflags.DatabaseInstance = 'CONDBR2'  ####
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'
globalflags.DetDescrVersion = 'ATLAS-R2-2015-03-01-00' # 09-Nov-2014
globalflags.ConditionsTag = 'CONDBR2-BLKPA-2014-03'
globalflags.print_JobProperties()


### set up conddb

from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag('CONDBR2-ES1PA-2014-00') ###########
conddb.setGlobalTag('CONDBR2-BLKPA-2014-03')
#conddb.blockFolder("/PIXEL/PixCalib");conddb.addFolder("PIXEL_OFL","/PIXEL/PixCalib <tag>PixCalib-IBL3D25DBM-04-01</tag>",force=True,forceMC=True);
#conddb.blockFolder("/Indet/Align");conddb.addFolder("INDET_OFL","/Indet/Align<tag>InDetAlign-RUN2-BLK-UPD4-09</tag>",force=True,forceMC=True);
#conddb.blockFolder("/TRT/Align");conddb.addFolder("TRT_OFL","/TRT/Align<tag>InDetAlign-RUN2-BLK-UPD4-09</tag>",force=True,forceMC=True);
#conddb.blockFolder("/TRT/Calib/DX");conddb.addFolder("TRT_OFL","/TRT/Calib/DX<tag>TRTCalibDX-RUN2-BLK-UPD4-03</tag>",force=True,forceMC=True);

#conddb.setGlobalTag('COMCOND-000-00')
#conddb.setGlobalTag('OFLCOND-RUN12-SDR-22')

## sqlite file db connection for writing
#yosuke conddb.iovdbsvc.dbConnection = "sqlite://;schema=noisemap.db;dbname=COMP200"
conddb.iovdbsvc.dbConnection = "sqlite://;schema=noisemap.db;dbname=CONDBR2"

if not 'OutputTag' in dir() :
  OutputTag = 'PixNoiseMapShort-DATA-RUN2-000-00'
  OutputLongTag = 'PixNoiseMapLong-DATA-RUN2-000-00'
if not 'ReferenceTag' in dir() :
  ReferenceTag = OutputTag
  ReferenceLongTag = OutputLongTag
  #ReferenceTag = 'PixNoiseMapShort-RUN2-DATA-RUN1-UPD4-01'
  #ReferenceLongTag = 'PixNoiseMapLong-RUN2-DATA-RUN1-UPD4-01'


if SpecialPixelMapSvc.DifferentialUpdates == True :
  SpecialPixelMapSvc.DBFolders = [ '/PIXEL/NoiseMapShortRef_oracle', '/PIXEL/NoiseMapLongRef_oracle' ]
  SpecialPixelMapSvc.SpecialPixelMapKeys = [ 'SpecialPixelMap', 'NoiseMapLong' ]
  conddb.iovdbsvc.Folders += [ "<dbConnection> oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_PIXEL;dbname=CONDBR2 </dbConnection> /PIXEL/NoiseMapShort <tag>" + ReferenceTag + "</tag>" + "<key> /PIXEL/NoiseMapShortRef_oracle </key>"]
  conddb.iovdbsvc.Folders += [ "<dbConnection> oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_PIXEL;dbname=CONDBR2 </dbConnection> /PIXEL/NoiseMapLong <tag>" + ReferenceLongTag + "</tag>" + "<key> /PIXEL/NoiseMapLongRef_oracle </key>" ]

  if os.path.isfile("noisemap.db") :
    SpecialPixelMapSvc.OverlayFolder = '/PIXEL/NoiseMapShortRef_sqlite'
    SpecialPixelMapSvc.OverlayLongFolder = '/PIXEL/NoiseMapLongRef_sqlite'
    SpecialPixelMapSvc.OverlayKey = 'NoiseMapShort_sqlite'
    SpecialPixelMapSvc.OverlayLongKey = 'NoiseMapLong_sqlite'
    SpecialPixelMapSvc.ModuleLevelOverlay = True
    conddb.iovdbsvc.Folders += [ "<dbConnection> sqlite://;schema=noisemap.db;dbname=CONDBR2 </dbConnection> /PIXEL/NoiseMapShort <tag>" + ReferenceTag + "</tag>" + "<key> /PIXEL/NoiseMapShortRef_sqlite </key>"]
    conddb.iovdbsvc.Folders += [ "<dbConnection> sqlite://;schema=noisemap.db;dbname=CONDBR2 </dbConnection> /PIXEL/NoiseMapLong <tag>" + ReferenceLongTag + "</tag>" + "<key> /PIXEL/NoiseMapLongRef_sqlite </key>" ]



## configure the PixMapDBWriter algorithm:

from PixelCalibAlgs.PixelCalibAlgsConf import PixMapDBWriter

PixMapDBWriter = PixMapDBWriter()

if not 'NoiseMapFilename' in dir() :
  NoiseMapFilename = "NoiseMap.root"

PixMapDBWriter.PixMapFileName = NoiseMapFilename   # name of ROOT input file with maps of noisy pixels
#PixMapDBWriter.CalculateOccupancy = True

doMerge = False

if doMerge == True :
  PixMapDBWriter.SpecialPixelMapKey = SpecialPixelMapSvc.SpecialPixelMapKeys[0] # key of special pixel map to merge with the new noise map


## DetFlags

from AthenaCommon.DetFlags import DetFlags

DetFlags.all_setOff()
DetFlags.pixel_setOn()
DetFlags.Print()

#DetDescrVersion = "ATLAS-GEO-08-00-02" #####
#DetDescrVersion = "ATLAS-R2-2015-02-01-00" # 09-Nov-2014

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


### define the job

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

job += PixMapDBWriter

theApp.CreateSvc += [ ServiceMgr.SpecialPixelMapSvc.getFullName() ]

### configure OutputConditionsAlg

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

OutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg","dummy.root")

OutputConditionsAlg.WriteIOV=True

OutputConditionsAlg.Run1 = OutputRun
OutputConditionsAlg.LB1 = OutputLB

OutputConditionsAlg.ObjectList=[ "CondAttrListCollection#" + SpecialPixelMapSvc.OutputFolder ]
OutputConditionsAlg.ObjectList+=[ "CondAttrListCollection#" + SpecialPixelMapSvc.OutputLongFolder ]

OutputConditionsAlg.IOVTagList=[ OutputTag ]
OutputConditionsAlg.IOVTagList+=[ OutputLongTag ]

from AthenaServices.AthenaServicesConf import AthenaOutputStream

job += AthenaOutputStream( "Stream1" )

ServiceMgr += SpecialPixelMapSvc


### configure IOVRegistrationSvc

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc

ServiceMgr += IOVRegistrationSvc()
regSvc = ServiceMgr.IOVRegistrationSvc

regSvc.OverrideNames = [ "ModuleSpecialPixelMap_Clob" ]
regSvc.OverrideTypes = [ "String4k" ]
regSvc.OverrideNames += [ "SpecialPixelMap" ]
regSvc.OverrideTypes += [ "Blob16M" ]

regSvc.PayloadTable = True


### configure the event selector

ServiceMgr.EventSelector.RunNumber         = OutputRun
ServiceMgr.EventSelector.EventsPerRun      = 5
ServiceMgr.EventSelector.FirstEvent        = 1
ServiceMgr.EventSelector.InitialTimeStamp  = 0
ServiceMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                   = 1


### configure the message service
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )

MessageSvc.OutputLevel      = 3
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000
MessageSvc.errorLimit       = 1000
