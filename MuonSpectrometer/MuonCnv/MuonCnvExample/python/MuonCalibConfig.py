# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# JobOption fragments for Condition Database access
# of muon calibration constants

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam
from AthenaCommon.BFieldFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon import CfgMgr
#--------------------------------------------------------------

from MuonCnvExample.MuonCnvUtils import specialAddFolderSplitOnline,mdtCalibWindowNumber
from MuonCnvExample.MuonCalibFlags import mdtCalibFlags,cscCalibFlags
mdtCalibFlags.setDefaults()
cscCalibFlags.setDefaults()

################################################################################
# CSC calibration
################################################################################

def setupCscCondDB():
    # Access to the CSC calibration database
    # Copied and modified from:
    # include( "CscCalibTools/CscCalibTool_jobOptions.py" )
    from MuonCondSvc.CscCondDB import cscCondDB

    ## Load Ped File
    if cscCalibFlags.CscPedFromLocalFile():
        cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
        cscCondDB.addPedFolder()    #<--- Adds pedestal and noise folders
        cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
    else:
        cscCondDB.addPedFolder()    #<--- Adds pedestal and noise folders
        
    ## Load Noise File
    if cscCalibFlags.CscNoiseFromLocalFile():
        cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
        cscCondDB.addNoiseFolder()    #<--- Adds pedestal and noise folders
        cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
    else:
        cscCondDB.addNoiseFolder()    #<--- Adds pedestal and noise folders

    ## Load PSlope File
    if cscCalibFlags.CscPSlopeFromLocalFile():
        cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
        cscCondDB.addPSlopeFolder()    #<--- Adds pedestal and noise folders
        cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
    else:
        cscCondDB.addPSlopeFolder()    #<--- Adds pedestal and noise folders

    ## Load Status File
    if cscCalibFlags.CscStatusFromLocalFile():
        cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
        cscCondDB.addStatusFolder()    #<--- Adds pedestal and noise folders
        cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
    else:
        cscCondDB.addStatusFolder()    #<--- Adds pedestal and noise folders

    ## Load Rms File
    if cscCalibFlags.CscRmsFromLocalFile():
        cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
        cscCondDB.addRmsFolder()    #<--- Adds pedestal and noise folders
        cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
    else:
        cscCondDB.addRmsFolder()    #<--- Adds pedestal and noise folders

    ## Load F001 File
    if cscCalibFlags.CscF001FromLocalFile():
        cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
        cscCondDB.addF001Folder()   
        cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
    else:
        cscCondDB.addF001Folder()   


    if not athenaCommonFlags.isOnline():
        log = logging.getLogger(__name__+".setupCscCondDB()")
        log.info("This is for OffLine so T0Base and T0Phase folders are added!!")

        ## Load T0Base File
        if cscCalibFlags.CscT0BaseFromLocalFile():
            cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
            cscCondDB.addT0BaseFolder()   
            cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
        else:
            cscCondDB.addT0BaseFolder()   


        ## Load T0Phase File
        if cscCalibFlags.CscT0PhaseFromLocalFile():
            cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
            cscCondDB.addT0PhaseFolder()   
            cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
        else:
            cscCondDB.addT0PhaseFolder()   

    #cscCondDB.addPedFolders()    #<--- Adds pedestal and noise folders
    #cscCondDB.addAllFolders()

# end of function setupCscCondDB()


def CscCalibTool(name,**kwargs):
    # setup condDB folders
    setupCscCondDB()
    # make tool
    return CfgMgr.CscCalibTool(
        Slope=0.19,
        Noise=3.5,
        Pedestal=2048.0,
        ReadFromDatabase=True,
        SlopeFromDatabase=False,
        integrationNumber=12.0,
        integrationNumber2=11.66,
        samplingTime=50.0,
        signalWidth=14.40922,
        timeOffset=71.825,
        IsOnline = athenaCommonFlags.isOnline(),
        Latency = 119
        )
    #               timeOffset=46.825) + 25 SimHIT digit time


################################################################################
# MDT calibration
################################################################################


if mdtCalibFlags.readMDTCalibFromBlob():
    mdt_folder_name_appendix = "BLOB"
else:
    mdt_folder_name_appendix=""


def setupMdtCondDB():

    global mdt_folder_name_appendix
    from IOVDbSvc.CondDB import conddb

    if mdtCalibFlags.mdtCalibrationSource()=="MDT":
        conddb.addFolderSplitOnline("MDT", '/MDT/Onl/RT' + mdt_folder_name_appendix,'/MDT/RT' + mdt_folder_name_appendix, className='CondAttrListCollection')
        conddb.addFolderSplitOnline("MDT", '/MDT/Onl/T0' + mdt_folder_name_appendix,'/MDT/T0' + mdt_folder_name_appendix, className='CondAttrListCollection')
    else:
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr.TagInfoMgr.ExtraTagValuePairs += ["MDTCalibrationSource", mdtCalibFlags.mdtCalibrationSource()]
        specialAddFolderSplitOnline(mdtCalibFlags.mdtCalibrationSource(), '/MDT/Onl/RT' + mdt_folder_name_appendix,'/MDT/RT' + mdt_folder_name_appendix)
        specialAddFolderSplitOnline(mdtCalibFlags.mdtCalibrationSource(), '/MDT/Onl/T0' + mdt_folder_name_appendix,'/MDT/T0' + mdt_folder_name_appendix)

    from AthenaCommon.AlgSequence import AthSequencer
    from MdtCalibDbCoolStrTool.MdtCalibDbCoolStrToolConf import MdtCalibDbAlg
    condSequence = AthSequencer("AthCondSeq")
    if not hasattr(condSequence,"MdtCalibDbAlg"):
        condSequence += MdtCalibDbAlg("MdtCalibDbAlg")

    if conddb.isOnline and not conddb.isMC:
        MdtCalibDbAlg.TubeFolder = "/MDT/T0"
        MdtCalibDbAlg.RtFolder = "/MDT/RT"
        MdtCalibDbAlg.ReadKeyTube = "/MDT/T0"
        MdtCalibDbAlg.ReadKeyRt = "/MDT/RT"
    else:
        MdtCalibDbAlg.TubeFolder = "/MDT/T0" + mdt_folder_name_appendix
        MdtCalibDbAlg.RtFolder = "/MDT/RT" + mdt_folder_name_appendix
        MdtCalibDbAlg.ReadKeyTube = "/MDT/T0" + mdt_folder_name_appendix
        MdtCalibDbAlg.ReadKeyRt = "/MDT/RT" + mdt_folder_name_appendix
    MdtCalibDbAlg.RT_InputFiles = ["Muon_RT_default.data"]
    if globalflags.DataSource == 'data':
        MdtCalibDbAlg.defaultT0 = 40
    elif globalflags.DataSource == 'geant4':
        MdtCalibDbAlg.defaultT0 = 799
    MdtCalibDbAlg.UseMLRt = mdtCalibFlags.useMLRt()
    MdtCalibDbAlg.TimeSlewingCorrection = mdtCalibFlags.correctMdtRtForTimeSlewing()
    MdtCalibDbAlg.MeanCorrectionVsR = [ -5.45973, -4.57559, -3.71995, -3.45051, -3.4505, -3.4834, -3.59509, -3.74869, -3.92066, -4.10799, -4.35237, -4.61329, -4.84111, -5.14524 ]
    MdtCalibDbAlg.PropagationSpeedBeta = mdtCalibFlags.mdtPropagationSpeedBeta()
    # the same as MdtCalibrationDbTool
    MdtCalibDbAlg.CreateBFieldFunctions = mdtCalibFlags.correctMdtRtForBField()
    MdtCalibDbAlg.CreateWireSagFunctions = mdtCalibFlags.correctMdtRtWireSag()
    MdtCalibDbAlg.CreateSlewingFunctions = mdtCalibFlags.correctMdtRtForTimeSlewing()

    from MdtCalibSvc.MdtCalibSvcConf import MdtCalibrationTool
    MdtCalibrationTool.DoSlewingCorrection = mdtCalibFlags.correctMdtRtForTimeSlewing()
    MdtCalibrationTool.DoTemperatureCorrection = mdtCalibFlags.applyRtScaling()
    MdtCalibrationTool.DoWireSagCorrection = mdtCalibFlags.correctMdtRtWireSag()
    if beamFlags.beamType() == 'collisions':
        MdtCalibrationTool.DoTofCorrection = True
        if globalflags.DataSource() == 'geant4':
            MdtCalibrationTool.TimeWindowSetting = mdtCalibWindowNumber('Collision_G4')
        elif globalflags.DataSource() == 'data':
            MdtCalibrationTool.TimeWindowSetting = mdtCalibWindowNumber('Collision_G4')
    else: # cosmics or single beam
        MdtCalibrationTool.DoTofCorrection = False

    from MdtCalibSvc.MdtCalibSvcConf import MdtCalibrationDbTool
    MdtCalibrationDbTool.CreateBFieldFunctions = mdtCalibFlags.correctMdtRtForBField()
    MdtCalibrationDbTool.CreateWireSagFunctions = mdtCalibFlags.correctMdtRtWireSag()
    MdtCalibrationDbTool.CreateSlewingFunctions = mdtCalibFlags.correctMdtRtForTimeSlewing()

# end of function setupMdtCondDB()


def MdtCalibDbTool(name="MdtCalibDbTool",**kwargs):
    # setup COOL folders
    global mdt_folder_name_appendix
    setupMdtCondDB()
    # set some default properties
    from IOVDbSvc.CondDB import conddb
    if conddb.isOnline and not conddb.isMC:
       kwargs.setdefault("TubeFolder", "/MDT/T0")
       kwargs.setdefault("RtFolder",  "/MDT/RT")
    else:
       kwargs.setdefault("TubeFolder", "/MDT/T0"+ mdt_folder_name_appendix)
       kwargs.setdefault("RtFolder",  "/MDT/RT"+ mdt_folder_name_appendix)
    kwargs.setdefault("RT_InputFiles" , ["Muon_RT_default.data"])
    if globalflags.DataSource == 'data':
        kwargs.setdefault("defaultT0", 40)
    elif globalflags.DataSource == 'geant4':
        kwargs.setdefault("defaultT0", 799)
    kwargs.setdefault("UseMLRt",  mdtCalibFlags.useMLRt() )
    kwargs.setdefault("TimeSlewingCorrection", mdtCalibFlags.correctMdtRtForTimeSlewing())
    kwargs.setdefault("MeanCorrectionVsR", [ -5.45973, -4.57559, -3.71995, -3.45051, -3.4505, -3.4834, -3.59509, -3.74869, -3.92066, -4.10799, -4.35237, -4.61329, -4.84111, -5.14524 ])
    kwargs.setdefault("PropagationSpeedBeta", mdtCalibFlags.mdtPropagationSpeedBeta())
    return CfgMgr.MuonCalib__MdtCalibDbCoolStrTool(name,**kwargs)

def MdtCalibrationDbSvc(name="MdtCalibrationDbSvc",**kwargs):
    kwargs.setdefault( "CreateBFieldFunctions", mdtCalibFlags.correctMdtRtForBField() )
    kwargs.setdefault( "CreateWireSagFunctions", mdtCalibFlags.correctMdtRtWireSag() )
    kwargs.setdefault( "CreateSlewingFunctions", mdtCalibFlags.correctMdtRtForTimeSlewing())
    kwargs.setdefault( "DBTool", "MdtCalibDbTool" )
    return CfgMgr.MdtCalibrationDbSvc(name,**kwargs)

def MdtCalibrationSvc(name="MdtCalibrationSvc",**kwargs):
    # call dependent tools. TODO: fix in C++ (move to ServiceHandle + declareProperty)
    from AthenaCommon.CfgGetter import getService
    getService("MdtCalibrationDbSvc")
    kwargs.setdefault( "DoSlewingCorrection",  mdtCalibFlags.correctMdtRtForTimeSlewing() )
    # Hack to use DoTemperatureCorrection for applyRtScaling; but applyRtScaling should not be used anyway, since MLRT can be used
    kwargs.setdefault( "DoTemperatureCorrection", mdtCalibFlags.applyRtScaling() )
    kwargs.setdefault( "DoWireSagCorrection",  mdtCalibFlags.correctMdtRtWireSag() )
    if beamFlags.beamType() == 'collisions':
        kwargs.setdefault("DoTofCorrection",True)
        if globalflags.DataSource() == 'geant4':
            # for collisions cut away hits that are far outside of the MDT time window
            kwargs.setdefault( "TimeWindowSetting", mdtCalibWindowNumber('Collision_G4') )
        elif globalflags.DataSource() == 'data':
            # for collisions cut away hits that are far outside of the MDT time window
            kwargs.setdefault( "TimeWindowSetting", mdtCalibWindowNumber('Collision_G4') )
    else: # cosmics or single beam
        kwargs.setdefault("DoTofCorrection",False)
    return CfgMgr.MdtCalibrationSvc(name,**kwargs)

