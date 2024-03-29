# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from future import standard_library
standard_library.install_aliases()
import subprocess

###########################################################################
#
# <Marco.Delmastro@cern.ch>
#
# Example jobOptions to reconstruction one or more ramp run from data
#
# Last update: 09/12/2008 <Fabien.Tarrade@cern.ch>
#
###########################################################################

if not "SuperCells" in dir():
   SuperCells=False
if not 'SC_SampleShift' in dir():
   SC_SampleShift=0   

if not 'SCIgnoreBarrelChannels' in dir():
   SCIgnoreBarrelChannels=False

if not 'SCIgnoreEndcapChannels' in dir():
  SCIgnoreEndcapChannels=False

if not SuperCells: include("LArCalibProcessing/LArCalib_Flags.py")
if SuperCells:     include("LArCalibProcessing/LArCalib_FlagsSC.py")
include("LArCalibProcessing/GetInputFiles.py")

######################################################
       Run properties
######################################################

if not 'SubDet' in dir():
  SubDet = "Barrel"

if not 'RunNumberList' in dir():
  RunNumberList = [ '0018660' ]
  
if not 'FilePrefix' in dir():
  if (int(RunNumberList[0]))<99800 :
      FilePrefix = "daq.Ramp"
   else :
      FilePrefix = "data*"
     
if not 'InputDir' in dir():
   InputDir = "/castor/cern.ch/grid/atlas/DAQ/lar/ElecCalib/2008/"+RunNumberList[0]
   
if not 'Partition' in dir():
   if (SubDet=='EndCap'):
      Partition      = "EB-EC*"
   else:
      Partition      = "EB-EMB*"     

if not 'FullFileName' in dir():
   if not 'Trigger' in dir():
      if (int(RunNumberList[0]))<99800 :
         Trigger = "*"+Partition
      else :
         Trigger = "calibration_LArElec-Ramp"+".*"
   
   FullFileName = []
   for RunNumber in RunNumberList :
       FullFileName+=GetInputFilesFromTokens(InputDir,int(RunNumber),FilePrefix,Trigger)

if not 'GainList' in dir():
   GainList = [ "HIGH", "MEDIUM", "LOW" ]
   
if not 'GroupingType' in dir():
   if not SuperCells: GroupingType = "ExtendedSubDetector"
   if SuperCells:     GroupingType = "SuperCells"

        
if not 'ChannelSelection' in dir():
   ## to read all
   ChannelSelection = " "
   ## example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,3:34</channelSelection>" 
   #ChannelSelection = "<channelSelection>0,0:1000</channelSelection>"
   
if not 'runAccumulator' in dir(): 
   runAccumulator = False # averaged mode

from string import *
def DBConnectionFile(sqlitefile):
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"

#######################################################
#                Monitoring properties
#######################################################

if not 'doMonitoring' in dir():
   doMonitoring = False

if not 'doLArCalibDataQuality' in dir():
   doLArCalibDataQuality = False     

if not 'online' in dir():
   online = False

if not 'CheckBadEvents' in dir():
   CheckBadEvents = True

#######################################################
#                Ramp properties                      #
#######################################################

if not 'isHEC' in dir():
   isHEC= False

if not 'HECKey' in dir():
   HECKey="LArHEC_PAmap"

if not 'PeakOF' in dir():
   PeakOF = True   # OF reconstruction
   
if not 'CorrectBias' in dir():
   CorrectBias = False   # Set whether to correct for bias - if True, will look for peds from COOL
	
if not 'StripsXtalkCorr' in dir():
   StripsXtalkCorr = True # Xtalk correction for strips

if not "ADCSaturation" in dir():
   ADCsaturation = 4090 # Set to 0 if you want to keep saturating pulses

if not "RampRangeValue" in dir():
   RampRangeValue = 3600 # Check on the raw data ADC sample before ped subtraction and pulse reconstruction to include point in fit
   
if not 'KeyOutput' in dir():  
   KeyOutput = "LArRamp" # Key of LArRampComplete object in DetStore

if not 'SaveRawRamp' in dir():
   SaveRawRamp = True
  
if not 'SaveAverages' in dir():
   SaveAverages = False # Add averages to ROOT file

if not 'ApplyCorr' in dir():
   ApplyCorr = False # Apply corr. before dumping to ROOT file

if not 'SaveAllSamples' in dir():
   SaveAllSamples = False # Add Samples and RMS to RAMPS TTree

if not 'CorrectBadChannels' in dir():
   CorrectBadChannels=True

if not 'ApplyAdHocCorrection' in dir():
   ApplyAdHocCorrection=False
   
#######################################################
#       Ramp output name
#######################################################

if not 'WriteNtuple' in dir():
   WriteNtuple = LArCalib_Flags.WriteNtuple

if not 'WritePoolFile' in dir():
   WritePoolFile = LArCalib_Flags.WritePoolFile

if not 'WriteIOV' in dir():
   WriteIOV = LArCalib_Flags.WriteIOV

if not 'IOVBegin' in dir():
   IOVBegin = int(RunNumberList[0])
   
if not 'IOVEnd' in dir():
   IOVEnd = LArCalib_Flags.IOVEnd   

if not 'DBConnectionCOOL' in dir():
   DBConnectionCOOL = "COOLOFL_LAR/CONDBR2"

## HEC map
if not 'ReadHECMapFromCOOL' in dir():
   ReadHECMapFromCOOL = True
	
if not 'InputHECMapPoolFileDir' in dir():
   InputHECMapPoolFileDir  = subprocess.getoutput("pwd")
   
if not 'InputHECMapPoolFileName' in dir():
   InputHECMapPoolFileName = "LArHECMap.pool.root"   
   
## Pedestal
if not 'ReadPedFromCOOL' in dir():
   ReadPedFromCOOL = True
	
if not 'PedRunNumber' in dir():
   PedRunNumber = "1005020_1005021_1005022"

if not 'InputPedPoolFileDir' in dir():
   InputPedPoolFileDir  = subprocess.getoutput("pwd")
   
if not 'InputPedPoolFileName' in dir():
   InputPedPoolFileName = "LArPedestal_" + str(PedRunNumber) +"_"+Partition.replace("*","")+ ".pool.root"

## OFC Cali
if not 'ReadOFCFromCOOL' in dir():
   ReadOFCFromCOOL = True
	
if not 'OFCRunNumber' in dir():
   OFCRunNumber = "30950"

if not 'InputOFCPoolFileDir' in dir():
   InputOFCPoolFileDir  = subprocess.getoutput("pwd")

if not 'InputOFCPoolFileName' in dir():
   if PeakOF :
      InputOFCPoolFileName = "LArOFCCali_" + str(PedRunNumber) +"_"+Partition.replace("*","")+ ".pool.root"
   if ( PeakOF and StripsXtalkCorr ) :
      InputOFCPoolFileName = "LArOFCCali_StripsXtalkCorr_" + str(OFCRunNumber) +"_"+Partition.replace("*","")+ ".pool.root"

if not 'CaliOFCFolder' in dir():
   if  not StripsXtalkCorr:
      CaliOFCFolder  = LArCalib_Flags.LArOFCCaliFolder
   else:
      CaliOFCFolder  = LArCalib_Flags.LArOFCCaliFolderXtlk

if 'dbname' in dir():
   rs=FolderTagResover(dbname=dbname)
else:
   rs=FolderTagResover()
if not 'LArRampFolderOutputTag' in dir():
   LArRampFolderOutputTag = rs.getFolderTagSuffix(LArCalib_Flags.LArRampFolder)
if not 'PedLArCalibFolderTag' in dir(): 
   PedLArCalibFolderTag = rs.getFolderTagSuffix(LArCalib_Flags.LArPedestalFolder)                                                
if not 'LArCaliOFCFolderTag' in dir():
   LArCaliOFCFolderTag = rs.getFolderTagSuffix(CaliOFCFolder)
del rs
   
if not 'OutputRampRootFileDir' in dir():
   OutputRampRootFileDir = subprocess.getoutput("pwd")
   
if not 'OutputRampPoolFileDir' in dir():
   OutputRampPoolFileDir = subprocess.getoutput("pwd")

if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)
	
if not 'BaseFileName' in dir():
   BaseFileName = "LArRamp"
   
   for RunNumber in RunNumberList :
      BaseFileName = BaseFileName+"_"+str(RunNumber)+"_"+Partition.replace("*","")
   
   if StripsXtalkCorr:
      BaseFileName += "_StripsXtalkCorr"
   if PeakOF:
      BaseFileName += "_OFC"	   

if not 'OutputRampRootFileName' in dir():
   OutputRampRootFileName = BaseFileName+".root"
   
if not 'OutputRampPoolFileName' in dir():
   OutputRampPoolFileName = BaseFileName+".pool.root"

if not 'OutputObjectSpecRamp' in dir():
   OutputObjectSpecRamp = "LArRampComplete#"+KeyOutput+"#"+LArCalib_Flags.LArRampFolder
   OutputObjectSpecTagRamp = LArCalibFolderTag(LArCalib_Flags.LArRampFolder,LArRampFolderOutputTag)
   
if ( ReadPedFromCOOL ):
   if 'InputPedSQLiteFile' in dir():
      InputDBConnectionPed = DBConnectionFile(InputPedSQLiteFile)
   else:
      InputDBConnectionPed = DBConnectionCOOL

if ( PeakOF and ReadOFCFromCOOL ):
   if 'InputDBConnectionOFC' not in dir():
      if 'InputOFCSQLiteFile' in dir():
         InputDBConnectionOFC = DBConnectionFile(InputOFCSQLiteFile)
      else:
         InputDBConnectionOFC = DBConnectionCOOL

if ( ReadHECMapFromCOOL ):
   if 'InputHECMapSQLiteFile' in dir():
      InputDBConnectionHECMap = DBConnectionFile(InputHECMapSQLiteFile)
   else:
      InputDBConnectionHECMap = DBConnectionCOOL

## Bad Channel   
   
if not 'ReadBadChannelFromCOOL' in dir():
   ReadBadChannelFromCOOL = True   
if ( ReadBadChannelFromCOOL ):      
   if 'InputBadChannelSQLiteFile' in dir():
      InputDBConnectionBadChannel = DBConnectionFile(InputBadChannelSQLiteFile)
   else:
      if 'InputDBConnectionBadChannel' not in dir():
         InputDBConnectionBadChannel = "COOLOFL_LAR/CONDBR2"      

#######################################################################################
#                                print summary                                        #
#######################################################################################

RampLog = logging.getLogger( "RampLog" )
RampLog.info( " ======================================================== " )
RampLog.info( " ***                 LAr Ramp summary                  *** " )
RampLog.info( " ======================================================== " )
RampLog.info( " RunNumber                          = "+str(RunNumberList) )
RampLog.info( " SubDetector                        = "+SubDet )
RampLog.info( " Partition                          = "+Partition )
RampLog.info( " Type                               = Ramp " )
for i in range(len(FullFileName)):
   RampLog.info( " FullFileName                       = "+FullFileName[i] )
if ( ReadPedFromCOOL ):
   RampLog.info( " InputDBConnectionPed               = "+InputDBConnectionPed )
else :
   RampLog.info( " InputPedPoolFileName               = "+InputPedPoolFileName )
RampLog.info( " PedLArCalibFolderTag               = "+PedLArCalibFolderTag )
if ( PeakOF and ReadOFCFromCOOL ):
   RampLog.info( " InputDBConnectionOFC               = "+InputDBConnectionOFC )
elif ( PeakOF) :
   RampLog.info( " InputOFCPoolFileName               = "+InputOFCPoolFileName )
if 'LArCaliOFCFolderTag' in dir() :
   RampLog.info( " LArCaliOFCFolderTag               = "+LArCaliOFCFolderTag )
RampLog.info( " OutputRampRootFullFileName         = "+OutputRampRootFileDir+"/"+OutputRampRootFileName )
RampLog.info( " OutputRampPoolFullFileName         = "+OutputRampPoolFileDir+"/"+OutputRampPoolFileName )
RampLog.info( " OutputObjectSpecRamp               = "+OutputObjectSpecRamp )
RampLog.info( " OutputObjectSpecTagRamp            = "+OutputObjectSpecTagRamp )
RampLog.info( " IOVBegin                           = "+str(IOVBegin) )
RampLog.info( " IOVEnd                             = "+str(IOVEnd) )
RampLog.info( " LArCalibOutputDB                   = "+OutputDB )
for i in range(len(GainList)):
   RampLog.info( " GainList                       = "+GainList[i] )
RampLog.info( " ======================================================== " )
#######################################################################################

include ("LArConditionsCommon/LArMinimalSetup.py")
from LArCabling.LArCablingAccess import LArOnOffIdMapping
LArOnOffIdMapping()
if SuperCells:
  from LArCabling.LArCablingAccess import LArOnOffIdMappingSC,LArCalibIdMappingSC,LArLATOMEMappingSC
  LArOnOffIdMappingSC()
  LArCalibIdMappingSC()
  LArLATOMEMappingSC()

#
# Provides ByteStreamInputSvc name of the data file to process in the offline context
#

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import theApp, ServiceMgr, ToolSvc

theByteStreamInputSvc=ServiceMgr.ByteStreamInputSvc
if not 'FullFileName' in dir():
   RampLog.info( "No FullFileName! Please give a FullFileName list." )
   theApp.exit(-1)

else :   
   ServiceMgr.EventSelector.Input=FullFileName
   
ServiceMgr.EventSelector.MaxBadEvents = 0

##############################################################################################
#                                                                                            #
# FullFileName is already the list of files compute by the fonction GetInputFilesFromTokens  #
#                                                                                            #
# maybe useful one day                                                                       #
#                                                                                            #
#else                                                                                        #
#   ServiceMgr.EventSelector.Input=OneFileName                                           #
#   for i in range(len(ServiceMgr.EventSelector.Input)):                                 #
#      theByteStreamInputSvc.NumFile+=[10000]                                                #
##############################################################################################

###############################################################
#                                                             #
#                To read BS for one or more FT                #
#                                                             #
###############################################################

## For splitting the delay runs, there are three new jobOptions.
## All three are vectors of integers
#################################################################

if ( runAccumulator ):
   if SuperCells:
      from LArByteStream.LArByteStreamConf import LArLATOMEDecoder

      theLArLATOMEDecoder = LArLATOMEDecoder("LArLATOMEDecoder")
      theLArLATOMEDecoder.DumpFile = SC_DumpFile
      theLArLATOMEDecoder.RawDataFile = SC_RawDataFile
      from LArByteStream.LArByteStreamConf import LArRawSCDataReadingAlg
      larRawSCDataReadingAlg = LArRawSCDataReadingAlg() 
      larRawSCDataReadingAlg.adcCollKey = Gain
      larRawSCDataReadingAlg.adcBasCollKey = ""
      larRawSCDataReadingAlg.etCollKey = ""
      larRawSCDataReadingAlg.etIdCollKey = ""
      larRawSCDataReadingAlg.LATOMEDecoder = theLArLATOMEDecoder
      larRawSCDataReadingAlg.OutputLevel = WARNING
      larRawSCDataReadingAlg.LATOMEDecoder.IgnoreBarrelChannels = SCIgnoreBarrelChannels
      larRawSCDataReadingAlg.LATOMEDecoder.IgnoreEndcapChannels = SCIgnoreEndcapChannels
      topSequence += larRawSCDataReadingAlg
   else:
      from LArByteStream.LArByteStreamConf import LArRawCalibDataReadingAlg
 
      theLArRawCalibDataReadingAlg=LArRawCalibDataReadingAlg()
      theLArRawCalibDataReadingAlg.LArDigitKey=Gain
      theLArRawCalibDataReadingAlg.LArFebHeaderKey="LArFebHeader"
      topSequence+=theLArRawCalibDataReadingAlg
   include("./LArCalib_CalibrationPatterns_"+str(IOVBegin)+".py")

else:

   if SuperCells:
      from LArByteStream.LArByteStreamConf import LArLATOMEDecoder 
      from LArByteStream.LArByteStreamConf import LArRawSCCalibDataReadingAlg
      LArRawSCCalibDataReadingAlg = LArRawSCCalibDataReadingAlg()
      LArRawSCCalibDataReadingAlg.LArSCAccCalibDigitKey = Gain
      LArRawSCCalibDataReadingAlg.LATOMEDecoder = LArLATOMEDecoder("LArLATOMEDecoder")
      LArRawSCCalibDataReadingAlg.LATOMEDecoder.DumpFile = SC_DumpFile
      LArRawSCCalibDataReadingAlg.LATOMEDecoder.RawDataFile = SC_RawDataFile
      LArRawSCCalibDataReadingAlg.LATOMEDecoder.ProtectSourceId = SCProtectSourceId
      LArRawSCCalibDataReadingAlg.LATOMEDecoder.IgnoreBarrelChannels = SCIgnoreBarrelChannels
      LArRawSCCalibDataReadingAlg.LATOMEDecoder.IgnoreEndcapChannels = SCIgnoreEndcapChannels
      LArRawSCCalibDataReadingAlg.LATOMEDecoder.OutputLevel = WARNING
      topSequence+=LArRawSCCalibDataReadingAlg

   else:   
      from LArByteStream.LArByteStreamConf import LArRawCalibDataReadingAlg
 
      theLArRawCalibDataReadingAlg=LArRawCalibDataReadingAlg()
      theLArRawCalibDataReadingAlg.LArAccCalibDigitKey=Gain
      theLArRawCalibDataReadingAlg.LArFebHeaderKey="LArFebHeader"
      topSequence+=theLArRawCalibDataReadingAlg

##########################################################################
#                                                                        #
#                         Ramp run reconstruction                        #
#                                                                        #
##########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

if 'BadChannelsFolder' not in dir():
   BadChannelsFolder="/LAR/BadChannelsOfl/BadChannels"
if 'MissingFEBsFolder' not in dir():
   MissingFEBsFolder="/LAR/BadChannelsOfl/MissingFEBs"

if not 'InputBadChannelSQLiteFile' in dir():
   RampLog.info( "Read Bad Channels from Oracle DB")
else :   
   RampLog.info( "Read Bad Channels from SQLite file") 

#if ( ReadBadChannelFromCOOL ):      
#   if 'InputBadChannelSQLiteFile' in dir():
#      InputDBConnectionBadChannel = DBConnectionFile(InputBadChannelSQLiteFile)
#   else:
#      #InputDBConnectionBadChannel = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=CONDBR2;"
#      if 'InputDBConnectionBadChannel' not in dir():
#         InputDBConnectionBadChannel = "COOLONL_LAR/" + conddb.dbname
      
if 'BadChannelsLArCalibFolderTag' in dir() :
   BadChannelsTagSpec = LArCalibFolderTag (BadChannelsFolder,BadChannelsLArCalibFolderTag) 
   conddb.addFolder("",BadChannelsFolder+"<tag>"+BadChannelsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",
                    className="CondAttrListCollection")
else :
   conddb.addFolder("",BadChannelsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className="CondAttrListCollection")

if 'MissingFEBsLArCalibFolderTag' in dir() :
   MissingFEBsTagSpec = LArCalibFolderTag (MissingFEBsFolder,MissingFEBsLArCalibFolderTag)   
   conddb.addFolder("",MissingFEBsFolder+"<tag>"+MissingFEBsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",
                    className='AthenaAttributeList')
else :
   conddb.addFolder("",MissingFEBsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className='AthenaAttributeList')

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg, LArBadFebCondAlg
theLArBadChannelCondAlg=LArBadChannelCondAlg(ReadKey=BadChannelsFolder)
if SuperCells:
   theLArBadChannelCondAlg.isSC=True
condSeq+=theLArBadChannelCondAlg

theLArBadFebCondAlg=LArBadFebCondAlg(ReadKey=MissingFEBsFolder)
condSeq+=theLArBadFebCondAlg

## This algorithm verifies that no FEBs are dropping out of the run
## If it finds corrupt events, it breaks the event loop and terminates the job rapidly
if not SuperCells:
   include ("LArROD/LArFebErrorSummaryMaker_jobOptions.py")       
   topSequence.LArFebErrorSummaryMaker.CheckAllFEB=False
if CheckBadEvents:
   from LArCalibDataQuality.LArCalibDataQualityConf import LArBadEventCatcher
   theLArBadEventCatcher=LArBadEventCatcher()
   theLArBadEventCatcher.CheckAccCalibDigitCont=True
   theLArBadEventCatcher.CheckBSErrors=True
   theLArBadEventCatcher.KeyList=GainList
   theLArBadEventCatcher.StopOnError=False
   topSequence+=theLArBadEventCatcher 

   
if SuperCells:
   conddb.addFolder("","/LAR/IdentifierOfl/OnOffIdMap_SC<db>COOLOFL_LAR/OFLP200</db><tag>LARIdentifierOflOnOffIdMap_SC-000</tag>",className="AthenaAttributeList") 

## define the DB Gobal Tag :
ServiceMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB   
try:
   ServiceMgr.IOVDbSvc.DBInstance=""
except: 
   pass

# Temperature folder
#conddb.addFolder("DCS_OFL","/LAR/DCS/FEBTEMP")
#ServiceMgr.EventSelector.InitialTimeStamp = 1284030331
#import cx_Oracle
#import time
#import datetime
#try:
#   connection=cx_Oracle.connect("ATLAS_SFO_T0_R/readmesfotz2008@atlr")
#   cursor=connection.cursor()
#   sRequest=("SELECT RUNNR,CREATION_TIME FROM SFO_TZ_RUN WHERE RUNNR='%s'")%(RunNumberList[0])
#   cursor.execute(sRequest)
#   times= cursor.fetchall()
#   d=times[0][1]
#   iovtemp=int(time.mktime(d.timetuple()))
#except:
#   iovtemp=1283145454

#print "Setting timestamp for run ",RunNumberList[0]," to ",iovtemp
#ServiceMgr.IOVDbSvc.forceTimestamp = 1283145454
#ServiceMgr.IOVDbSvc.forceTimestamp = iovtemp


from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
ServiceMgr.PoolSvc.ReadCatalog += larCalibCatalogs

if ( doLArCalibDataQuality  ) :
   ## The reference is the Oracle DB
   if not SuperCells:
      conddb.addFolder("LAR_ONL","/LAR/ElecCalibOnl/Ramp<key>LArRampRef</key>"+ChannelSelection)
   else:   
      #FIXME once the SC calib data are in oracle
      conddb.addFolder("","/LAR/ElecCalibOflSC/Ramps/RampLinea<key>LArRampRef</key><dbConnection>sqlite://;schema=/eos/atlas/atlascerngroupdisk/det-larg/SuperCells/CalibData/poolFiles/LATOMERun_EndcapWeekly_220411-182904/db_00417648_00417649_00417650.db;dbname=CONDBR2</dbConnection><tag>LARElecCalibOflSCRampsRampLinea-UPD3-00</tag>"+ChannelSelection)

#if (CorrectBadChannels or StripsXtalkCorr):
#   conddb.addFolder("LAR","/LAR/BadChannelsOfl/BadChannels")
#   conddb.addFolder("LAR","/LAR/BadChannelsOfl/MissingFEBs")
   
if (isHEC):
   if ( ReadHECMapFromCOOL ):
      if not 'InputHECMapSQLiteFile' in dir():
         RampLog.info( "Read HECMap from Oracle DB" )
      else :
         RampLog.info( "Read HEC map from SQLite file" )

   if ( ReadHECMapFromCOOL ):
      HECMapFolder  = "/LAR/ElecCalibOfl/HecPAMap"
      if "HECMapTagSpec" in dir():
         conddb.addFolder("",HECMapFolder+"<tag>"+HECMapTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionHECMap+"</dbConnection>",className="LArRinjComplete")
      else :
         conddb.addFolder("",HECMapFolder+"<dbConnection>"+InputDBConnectionHECMap+"</dbConnection>",className="LArRinjComplete")

   else:
      if 'InputHECMapPoolFileName' in dir():
         RampLog.info( "Read HECMap form POOL file" )
         PoolFileList += [ InputHECMapPoolFileDir +"/"+ InputHECMapPoolFileName ]   
      else:
         RampLog.info( "No PoolFileList found! Please list the POOL files containing HECMap or read from COOL." )
         theApp.exit(-1)    

if (CorrectBias or PeakOF or StripsXtalkCorr):
   if ( ReadPedFromCOOL or ReadOFCFromCOOL ):
      if not 'InputPedSQLiteFile' in dir():
         RampLog.info( "Read Pedestal from Oracle DB" )
      else :  
         RampLog.info( "Read Pedestal from SQLite file" )
          
      if not 'InputOFCSQLiteFile' in dir():
         RampLog.info( "Read OFC Cali from Oracle DB" )
      else :
         RampLog.info( "Read OFC Cali from SQLite file" )

if ( ReadPedFromCOOL ):
   if (CorrectBias or StripsXtalkCorr or PeakOF):
      PedestalFolder  = LArCalib_Flags.LArPedestalFolder
      PedestalTagSpec = LArCalibFolderTag(PedestalFolder,PedLArCalibFolderTag)
      #RampLog.info("Ped. folder:",PedestalFolder," ",PedestalTagSpec," ",InputDBConnectionPed)
      conddb.addFolder("",PedestalFolder+"<tag>"+PedestalTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionPed+"</dbConnection>"+ChannelSelection,className="LArPedestalComplete")
      
else:
   if 'InputPedPoolFileName' in dir():
      RampLog.info( "Read Pedestal from POOL file" )
      PoolFileList += [ InputPedPoolFileDir +"/"+ InputPedPoolFileName ]   
   else:
      RampLog.info( "No PoolFileList found! Please list the POOL files containing Pedestal or read from COOL." )
      theApp.exit(-1)

if ( ReadOFCFromCOOL ):
   if PeakOF:
      if not 'CaliOFCTagSpec' in dir():
         CaliOFCTagSpec = LArCalibFolderTag(CaliOFCFolder,LArCaliOFCFolderTag)
      conddb.addFolder("",CaliOFCFolder+"<tag>"+CaliOFCTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionOFC+"</dbConnection>"+ChannelSelection, className="LArOFCComplete")

else:
   if 'InputOFCPoolFileName' in dir():
         RampLog.info( "Read OFC Cali form POOL file" )
         PoolFileList += [ InputOFCPoolFileDir +"/"+ InputOFCPoolFileName ]   
   else:
         RampLog.info( "No PoolFileList found! Please list the POOL files containing OFC or read from COOL." )
         theApp.exit(-1)

if ( len(PoolFileList)>0 ):
   from AthenaCommon.ConfigurableDb import getConfigurable
   ServiceMgr += getConfigurable( "ProxyProviderSvc" )()
   ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
   
   ServiceMgr += getConfigurable( "CondProxyProvider" )()
   ServiceMgr.CondProxyProvider.InputCollections += PoolFileList

if ( StripsXtalkCorr ) :
   from LArCalibUtils.LArCalibUtilsConf import LArStripsCrossTalkCorrector
   theLArStripsCrossTalkCorrector = LArStripsCrossTalkCorrector()
   theLArStripsCrossTalkCorrector.KeyList = GainList
   theLArStripsCrossTalkCorrector.ADCsaturation = ADCsaturation
   theLArStripsCrossTalkCorrector.NoXtalkCorr=["deadReadout","deadPhys","deadCalib","almostDead"]
   theLArStripsCrossTalkCorrector.DontUseForXtalkCorr=["short","peculiarCalibrationLine","deadReadout"]
   theLArStripsCrossTalkCorrector.AcceptableDifference=25.0 #in per-cent
   topSequence +=theLArStripsCrossTalkCorrector



from LArCalibUtils.LArCalibUtilsConf import LArRampBuilder
theLArRampBuilder = LArRampBuilder()
theLArRampBuilder.KeyList      = GainList
theLArRampBuilder.SubtractDac0 = False
theLArRampBuilder.ProblemsToMask=["deadCalib","deadReadout","deadPhys","almostDead","short"]

if ( PeakOF ):
    theLArRampBuilder.RecoType = "OF"
    from LArRecUtils.LArRecUtilsConf import LArOFPeakRecoTool
    theLArOFPeakRecoTool=LArOFPeakRecoTool()
    theLArOFPeakRecoTool.UseShape = False
    theLArRampBuilder.PeakOFTool=theLArOFPeakRecoTool
else :
    theLArRampBuilder.RecoType = "Parabola"
    from LArRecUtils.LArRecUtilsConf import LArParabolaPeakRecoTool
    theLArParabolaPeakRecoTool=LArParabolaPeakRecoTool()
    theLArParabolaPeakRecoTool.correctBias = CorrectBias
    ToolSvc+=theLArParabolaPeakRecoTool

# As the first ramp point no longer is taken at DAC=0
# this point should not have a special place.
theLArRampBuilder.DAC0         = -1
theLArRampBuilder.StoreRawRamp = SaveRawRamp
theLArRampBuilder.StoreRecRamp = True
theLArRampBuilder.Polynom      = 1    
theLArRampBuilder.RampRange    = RampRangeValue # Check on the raw data ADC sample before ped subtraction
                                                # and pulse reconstruction to include point in fit
theLArRampBuilder.correctBias  = CorrectBias
theLArRampBuilder.minDAC = 10      # minimum DAC value to use in fit
theLArRampBuilder.KeyOutput = KeyOutput
theLArRampBuilder.DeadChannelCut = -9999
theLArRampBuilder.GroupingType = GroupingType

theLArRampBuilder.isSC = SuperCells


if ( isHEC ) :
   theLArRampBuilder.isHEC = isHEC
   theLArRampBuilder.HECKey = HECKey
   
topSequence+=theLArRampBuilder


######################################################################
#                                                                    #
#                          Correction                                #
#                                                                    #
######################################################################

if CorrectBadChannels:
   from LArCalibUtils.LArCalibUtilsConf import LArCalibPatchingAlg_LArRampComplete_
   theLArRampPatcher=LArCalibPatchingAlg_LArRampComplete_("LArRampPatcher")
   theLArRampPatcher.ContainerKey=KeyOutput
   theLArRampPatcher.PatchMethod="PhiAverage"
   
   theLArRampPatcher.ProblemsToPatch=["deadCalib","deadReadout","deadPhys","almostDead","short"]
   theLArRampPatcher.UseCorrChannels=False      

   if SuperCells:
      theLArRampPatcher.OnOffMap="LArOnOffIdMapSC"
      theLArRampPatcher.CalibLineKey="LArCalibIdMapSC"
      theLArRampPatcher.SuperCell=True
   
   ## block standard patching for this CB
   theLArRampPatcher.DoNotPatchCBs=[0x3e198000]
   topSequence+=theLArRampPatcher

if ( ApplyAdHocCorrection ):
   from LArCalibUtils.LArCalibUtilsConf import LArRampAdHocPatchingAlg
   LArRampAdHocPatchingAlg = LArRampAdHocPatchingAlg("LArRampAdHocPatchingAlg")
   LArRampAdHocPatchingAlg.ContainerKey = KeyOutput

   if ( 'ChannelsToBePatchedHG' in dir() and 'PatchesToBeAppliedHG' in dir() ):
      if ( len(ChannelsToBePatchedHG) and len(ChannelsToBePatchedHG)==len(PatchesToBeAppliedHG) ):
         LArRampAdHocPatchingAlg.ChannelsToBePatchedHG = ChannelsToBePatchedHG
         LArRampAdHocPatchingAlg.PatchesToBeAppliedHG  = PatchesToBeAppliedHG
         if 'ValuesToBePatchedHG' in dir():
               LArRampAdHocPatchingAlg.ValuesToBeAppliedHG = ValuesToBePatchedHG

   if ( 'ChannelsToBePatchedMG' in dir() and 'PatchesToBeAppliedMG' in dir() ):
      if ( len(ChannelsToBePatchedMG) and len(ChannelsToBePatchedMG)==len(PatchesToBeAppliedMG) ):
         LArRampAdHocPatchingAlg.ChannelsToBePatchedMG = ChannelsToBePatchedMG
         LArRampAdHocPatchingAlg.PatchesToBeAppliedMG  = PatchesToBeAppliedMG
         if 'ValuesToBePatchedMG' in dir():
               LArRampAdHocPatchingAlg.ValuesToBeAppliedMG = ValuesToBePatchedMG

   if ( 'ChannelsToBePatchedLG' in dir() and 'PatchesToBeAppliedLG' in dir() ):
      if ( len(ChannelsToBePatchedLG) and len(ChannelsToBePatchedLG)==len(PatchesToBeAppliedLG) ):
         LArRampAdHocPatchingAlg.ChannelsToBePatchedLG = ChannelsToBePatchedLG
         LArRampAdHocPatchingAlg.PatchesToBeAppliedLG  = PatchesToBeAppliedLG
         if 'ValuesToBePatchedLG' in dir():
               LArRampAdHocPatchingAlg.ValuesToBeAppliedLG = ValuesToBePatchedLG
 
   topSequence+=LArRampAdHocPatchingAlg

######################################################################
#                                                                    #
#                          Output                                    #
#                                                                    #
######################################################################

if ( doLArCalibDataQuality  ) :
   from LArCalibDataQuality.LArCalibDataQualityConf import LArRampValidationAlg
   from LArCalibDataQuality.Thresholds import rampThr, rampThrFEB
   theRampValidationAlg=LArRampValidationAlg("RampVal")
   theRampValidationAlg.RampTolerance=rampThr
   theRampValidationAlg.RampToleranceFEB=rampThrFEB
   theRampValidationAlg.ProblemsToMask=["deadReadout","deadCalib","deadPhys","almostDead",
                                        "highNoiseHG","highNoiseMG","highNoiseLG"]
   theRampValidationAlg.KeyList=GainList
   if not SuperCells:
      theRampValidationAlg.PatchMissingFEBs=True
   else:
      theRampValidationAlg.PatchMissingFEBs=False
      theRampValidationAlg.CablingKey="LArOnOffIdMapSC"
      theRampValidationAlg.CalibLineKey="LArCalibIdMapSC"
   theRampValidationAlg.UseCorrChannels=False 
   theRampValidationAlg.ValidationKey="LArRamp"
   theRampValidationAlg.ReferenceKey="LArRampRef"
   
   theRampValidationAlg.OutputLevel=INFO
   theRampValidationAlg.MsgLevelForDeviations=WARNING
   theRampValidationAlg.ListOfDevFEBs="rampFebs.txt"
   theRampValidationAlg.ThinnedValContainer="ThinRampsVal"
   theRampValidationAlg.ThinnedRefContainer="ThinRampsRef"

   ##in case of CalibBoard patching, please uncomment:
   ## adding new patching
   theRampValidationAlg.PatchCBs=[0x3e198000]

   topSequence+=theRampValidationAlg

   ## second instance of the validation tool to detect "bad" channel 
   theBadRamp=LArRampValidationAlg("RampFail")
   theBadRamp.ProblemsToMask=["deadReadout","deadCalib","deadPhys","almostDead",
                              "highNoiseHG","highNoiseMG","highNoiseLG"]
   theBadRamp.RampTolerance=["0.05,0.05,0.05"]
   theBadRamp.RampToleranceFEB=["0.05,0.05,0.05"]
   theBadRamp.KeyList=GainList
   theBadRamp.ValidationKey="LArRamp"
   theBadRamp.ReferenceKey="LArRampRef"
   theBadRamp.MsgLevelForDeviations=ERROR
   theBadRamp.ListOfDevFEBs="Bad_rampFebs.txt"
   theRampValidationAlg.PatchMissingFEBs=False
   topSequence+=theBadRamp


if ( doMonitoring ) :
 
   from AthenaMonitoring.DQMonFlags import DQMonFlags
   DQMonFlags.enableLumiAccess.set_Value_and_Lock(False)

   from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
   topSequence += AthenaMonManager( "LArMon" )
   LArMon = topSequence.LArMon

   ## tier0 style
   LArMon.FileKey = "GLOBAL"
   LArMon.ManualDataTypeSetup = True
   LArMon.Environment         = "tier0"
   LArMon.ManualRunLBSetup    = True
   LArMon.Run                 = int(RunNumberList[0])
   LArMon.LumiBlock           = 1
   LArMon.CheckEveryNoEvents  = 999999 #to do the check only at the end of the run
   
   include("LArMonTools/LArFEBMon_jobOptions.py")
   
   RootHistOutputFileName='RootHistos_'+BaseFileName+".root"

   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import THistSvc
   if os.path.exists(OutputRampRootFileDir+ "/" +RootHistOutputFileName): 
      os.remove(OutputRampRootFileDir+ "/" +RootHistOutputFileName)
   ServiceMgr += THistSvc()

   #ServiceMgr.THistSvc.Output = ["GLOBAL DATAFILE='"+OutputRampRootFileDir+ "/" +RootHistOutputFileName+"' OPT='NEW'"]
   
if WriteNtuple or doMonitoring:

   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputRampRootFileDir+"/"+OutputRampRootFileName): 
      os.remove(OutputRampRootFileDir+"/"+OutputRampRootFileName)  
   ServiceMgr += NTupleSvc()
   ServiceMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputRampRootFileDir+"/"+OutputRampRootFileName+"' OPT='NEW'" ]

if ( WritePoolFile ):

   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   if os.path.exists(OutputRampPoolFileDir+"/"+OutputRampPoolFileName): 
      os.remove(OutputRampPoolFileDir+"/"+OutputRampPoolFileName)  
   theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg",OutputRampPoolFileDir+"/"+OutputRampPoolFileName,
                             [OutputObjectSpecRamp],[OutputObjectSpecTagRamp],WriteIOV)
   theOutputConditionsAlg.Run1 = IOVBegin
   if IOVEnd>0:
      theOutputConditionsAlg.Run2=IOVEnd

   ServiceMgr.IOVDbSvc.dbConnection  = OutputDB

   from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
   ServiceMgr += IOVRegistrationSvc()
   ServiceMgr.IOVRegistrationSvc.OutputLevel = INFO
   ServiceMgr.IOVRegistrationSvc.RecreateFolders = False

if (WriteNtuple):
   
   # Ramp fit ntuple
   from LArCalibTools.LArCalibToolsConf import LArRamps2Ntuple
   LArRamps2Ntuple=LArRamps2Ntuple("LArRamps2Ntuple")
   klist=[]
   for i in GainList:
      klist+=["LArRamp"+i]
   if not SuperCells: LArRamps2Ntuple.ContainerKey = klist #Only for raw ramp
   if SuperCells: LArRamps2Ntuple.ContainerKey = ["LArRampHIGH"]     # Modification to avoid problems in LArRampBuilder
   LArRamps2Ntuple.RealGeometry = True
   LArRamps2Ntuple.OffId = True
   LArRamps2Ntuple.NtupleName = "RAMPS"
   LArRamps2Ntuple.RawRamp = SaveRawRamp
   LArRamps2Ntuple.SaveAllSamples = SaveAllSamples
   LArRamps2Ntuple.ApplyCorr = ApplyCorr
   LArRamps2Ntuple.AddFEBTempInfo = False
   LArRamps2Ntuple.isSC = SuperCells
   if SuperCells:
      LArRamps2Ntuple.CalibMapKey = "LArCalibIdMapSC"
      from CaloAlignmentAlgs.CaloAlignmentAlgsConf import CaloSuperCellAlignCondAlg
      condSeq += CaloSuperCellAlignCondAlg("CaloSuperCellAlignCondAlg")
      LArRamps2Ntuple.ExtraInputs += (('CaloSuperCellDetDescrManager', 'ConditionStore+CaloSuperCellDetDescrManager'))
   
   topSequence+= LArRamps2Ntuple
      
   if ( SaveAverages ):
      # Ramp points ntuple(s)
      from LArCalibTools.LArCalibToolsConf import LArAverages2Ntuple

      if not SuperCells:
         for g in GainList:
            LArAverages2NtupleMR=LArAverages2Ntuple("LArAverages2Ntuple"+g)
            LArAverages2NtupleMR.ContainerKey = g
            topSequence+= LArAverages2NtupleMR
               
      if SuperCells:
         LArAverages2NtupleSC=LArAverages2Ntuple("LArAverages2NtupleSC")
         LArAverages2NtupleSC.ContainerKey = "SC"
         LArAverages2Ntuple.OffId = True
         LArAverages2NtupleSC.RealGeometry = True
         #LArAverages2NtupleSC.NSamples = 50
         LArAverages2NtupleSC.isSC = SuperCells
         topSequence+= LArAverages2NtupleSC
  
###########################################################################	
	
ServiceMgr.MessageSvc.OutputLevel  = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
ServiceMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

ServiceMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = WARNING)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################
