# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import re

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerJobOpts.TriggerFlags' )

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties # noqa: F401
from TriggerJobOpts.CommonSignatureHelper import AllowedList
from TrigConfigSvc.TrigConfigSvcUtils import getKeysFromNameRelease, getMenuNameFromDB


_flags = []

# Define simple boolean flags
def bool_flag_with_default(name, val):
    return type(name,
                (JobProperty, ),
                {
                   "statusOn": True,
                   "allowedType": ['bool'],
                   "StoredValue": val,
                })

default_true_flags = [
    "doLVL1", # run the LVL1 simulation (set to FALSE to read the LVL1 result from BS file)
    "doL1Topo", # Run the L1 Topo simulation (set to FALSE to read the L1 Topo result from BS file)
    "doMergedHLTResult", # if False disable decoding of the merged HLT Result (so decoding L2/EF Result) """
    "doAlwaysUnpackDSResult",  # if False disable decoding of DS results for all files but for real DS files
    "doID",  # if False, disable ID algos at LVL2 and EF """
    "doCalo",  # if False, disable Calo algorithms at LVL2 & EF """
    "doCaloOffsetCorrection",  # enable Calo pileup offset BCID correction """
    "doMuon", # if FAlse, disable Muons, note: muons need input file containing digits"""
    "doNavigationSlimming",  # Enable the trigger navigation slimming"""
]

default_false_flags = [
    "fakeLVL1", # create fake RoI from KINE info  """
    "useRun1CaloEnergyScale",
    "doTruth",
    "doTriggerConfigOnly",  # if True only the configuration services should be set, no algorithm """
    "doTransientByteStream",  # Write transient ByteStream before executing HLT algorithms.
                              # To be used for running on MC RDO with clients which require BS inputs.
    "writeBS",  # Write ByteStream output file """
    "readBS",
    "readMenuFromTriggerDb", # define the TriggerDb to be the source of the LVL1 and HLT trigger menu
    "generateMenuDiagnostics",  # Generate additional files heling in menu diagnostics """
]

for name in default_true_flags:
    newFlag = bool_flag_with_default(name, True)
    globals()[newFlag.__name__] = newFlag
    _flags.append(newFlag)

for name in default_false_flags:
    newFlag = bool_flag_with_default(name, False)
    globals()[newFlag.__name__] = newFlag
    _flags.append(newFlag)

class doHLT(JobProperty):
    """ if False, disable HLT selection """
    statusOn=True
    allowedType=['bool']
    StoredValue=True
    
_flags.append(doHLT)

class doValidationMonitoring(JobProperty):
    """Enables extra validation monitoring"""
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doValidationMonitoring)

# trigger configuration source list
class configurationSourceList(JobProperty):
    """ define where to read trigger configuration from. Allowed values: ['xml','aod','ds']"""
    statusOn=True
    allowedType=['list']
    StoredValue=[]
    allowedValues = AllowedList( ['aod','xml','ds'] )

_flags.append(configurationSourceList)

class AODEDMSet(JobProperty):
    """ Define which sets of object go to AOD """
    statusOn=True
    allowedType=['list']
    StoredValue='AODSLIM'

_flags.append(AODEDMSet)

class ESDEDMSet(JobProperty):
    """ Define which sets of object go to ESD (or derived ESD) """
    statusOn=True
    allowedType=['list']
    StoredValue='ESD'

_flags.append(ESDEDMSet)

class OnlineCondTag(JobProperty):
    """ Default (online) HLT conditions tag """
    statusOn=True
    allowedType=['str']
    StoredValue='CONDBR2-HLTP-2018-01'

_flags.append(OnlineCondTag)

class OnlineGeoTag(JobProperty):
    """ Default (online) HLT geometry tag """
    statusOn=True
    allowedType=['str']
    StoredValue='ATLAS-R2-2016-01-00-01'
    
_flags.append(OnlineGeoTag)

# =========
#
# trigger flags used by trigger configuration
#

class configForStartup(JobProperty):
    """ A temporary flag to determine the actions to be taken for the different cases of HLT running in the startup phase"""
    statusOn=True
    allowedType=['string']
    StoredValue = 'HLTonline'
    
    allowedValues = [
        'HLTonline',
        'HLToffline'
        ]

_flags.append(configForStartup)


class dataTakingConditions(JobProperty):
    """ A flag that describes the conditions of the Trigger at data taking, and determines which part of it will be processed in reconstruction."""
    statusOn=True
    allowedType=['string']
    StoredValue = 'FullTrigger'
    
    allowedValues = [
        'HltOnly',
        'Lvl1Only',
        'FullTrigger',
        'NoTrigger'
        ]

_flags.append(dataTakingConditions)

class triggerUseFrontier(JobProperty):
    """Flag determines if frontier should be used to connect to the oracle database, current default is False"""
    statusOn=True
    allowedType=['bool']
    StoredValue = False
    def _do_action(self):
        log = logging.getLogger( 'TriggerFlags.triggerUseFrontier' )
        log.info("Setting TriggerFlags.triggerUseFrontier to %r", self.get_Value())
        
_flags.append(triggerUseFrontier)



class triggerConfig(JobProperty):
    """ Flag to set various menus and options (read from XML or DB)
    Allowed values:

    Note that we use LVL1 prefix here in order not to touch the
    HLT if we're only running a LVL1 digitization job. The
    prefix is automatically added in the Digi job transform.
    
    NONE or OFF                             -trigger off 

    For digitization (L1) only use LVL1 prefix:
    LVL1:DEFAULT                            -default L1 menu
    LVL1:MenuName                           -takes the L1 xml representation of this menu
    LVL1:DB:connectionstring:SMKey,L1PSKey  -takes these db keys
    LVL1:DB:connectionstring:MenuName,Rel   -takes this menu from the db - not yet supported

    For MC reconstruction use MCRECO prefix:
    MCRECO:DEFAULT                                       -default L1 and HLT menu
    MCRECO:MenuName                                      -takes the L1 and HLT xml respresentations of the menu
    MCRECO:DB:connectionstring:SMKey,L1PSK,HLTPSK[,BGK]  -takes these db keys
    MCRECO:DB:connectionstring:MenuName,Rel              -takes this menu from the db (looks up the SMK)
                                                         -NB for the above: move to alias tables?
                                                   
    For data reconstruction: use DATARECO prefix. TO BE IMPLEMENTED. 
    DATARECO:ONLINE
    DATARECO:OFFLINE
    DATARECO:DB:connectionstring:SMKey,L1PSK,HLTPSK
    
    InFile: TO BE IMPLEMENTED

    connectionstring can be one of the following
    1)  <ALIAS>                              -- usually TRIGGERDB or TRIGGERDBMC (generally any string without a colon ':')
    2)  <type>:<detail>                      -- <type> has to be oracle, mysql, or sqlite_file, <detail> is one of the following
    2a) sqlite_file:filename.db              -- an sqlite file, no authentication needed, will be opened in read-only mode
    2b) oracle://ATLAS_CONFIG/ATLAS_CONF_TRIGGER_V2  -- a service description type://server/schema without user and password
    2c) oracle://ATLAS_CONFIG/ATLAS_CONF_TRIGGER_V2;username=ATLAS_CONF_TRIGGER_V2_R;password=<...>  -- a service description with user and password

    Note: specifying :DBF: instead of :DB: will set the trigger flag triggerUseFrontier to true
    """
    
    statusOn=''
    allowedType=['string']
    StoredValue = 'MCRECO:DEFAULT'


    def _do_action(self):
        """ setup some consistency """
        from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
                
        log = logging.getLogger( 'TriggerFlags.triggerConfig' )
        log.info("triggerConfig: \""+self.get_Value()+"\"")
        # We split the string passed to the flag
        configs = self.get_Value().split(":")
        
        ## ------
        ##  OFF or NONE: we want to turn everything related to trigger to false (via rec flag)
        ##  Note that this is true for reconstruction only at the moment. For LVL1 Digitization jobs,
        ##  which don't use rec flags, this is still done in the skeleton. Might be changed in future.
        ## ------
        if (configs[0] == 'OFF' or configs[0] == 'NONE'):
            from RecExConfig.RecFlags  import rec
            rec.doTrigger=False
            log.info("triggerConfig: Setting rec.doTrigger to False")
            
            
        ## ------
        ## DATARECO : We deal with data (cosmics, single run, collisions)
        ## ------               
        elif configs[0] == 'DATARECO':
            if configs[1] == 'ONLINE': # We read config from COOL directly
                log.warning("triggerConfig: DATARECO:ONLINE (reco from cool) is not yet implemented. You should not use it.")
            elif configs[1] == 'OFFLINE': # We read config from XML
                log.warning("triggerConfig: DATARECO:OFFLINE (reco from xml) is not yet implemented. You should not use it.")
            elif configs[1] == 'REPR': # We read config from XML
                log.info("triggerConfig: DATARECO:REPR is designed to configure the offline reconstruction in a trigger reprocessing job")
                try:
                    f = open("MenuCoolDbLocation.txt",'r')
                    tf.triggerCoolDbConnection = f.read()
                    f.close()
                except IOError:
                    log.fatal("triggerConfig=DATARECO:REPR requires 'MenuCoolDbLocation.tx' to be present in the local directory (reco part of trigger reprocessing)")
                    
            elif configs[1] == 'DB' or configs[1] == 'DBF': # We read config from a private DB
                ### We read the menu from the TriggerDB
                tf.readMenuFromTriggerDb=True
                tf.triggerUseFrontier = (configs[1]=='DBF')
                tf.triggerDbConnection = ':'.join(configs[2:-1])  # the dbconnection goes from second to last ':', it can contain ':'
                DBkeys = configs[-1].split(",")
                if (len(DBkeys) == 3):                            # we got 3 keys (SM, L1PS, HLTPS)
                    tf.triggerDbKeys=[int(x) for x in DBkeys] + [1]
                    log.info("triggerConfig: DATARECO from DB with speficied keys SMK %i, L1 PSK %i, and HLT PSK %i.", *tuple(tf.triggerDbKeys()[0:3]))
                elif (len(DBkeys) == 2):                       # we got a menu name and a release which we need to look up 
                    log.info("triggerConfig: DATARECO from DB with specified menu name and release: finding keys...")
                    tf.triggerDbKeys=getKeysFromNameRelease(tf.triggerDbConnection(),DBkeys[0],DBkeys[1],False) + [1]
                    log.info("triggerConfig: DATARECO from DB with keys SMK %i, L1 PSK %i, and HLT PSK %i.", *tuple(tf.triggerDbKeys()[0:3]))
                else:
                    log.info("triggerConfig: DATARECO from DB configured with wrong number of keys/arguments" )

        ## ---------
        ##  InFile : We wish to read a file with config info already in it (ESD, AOD, ...)
        ## ---------            
        elif configs[0] == 'InFile': 
            log.warning("triggerConfig: Infile is not yet implemented. You should not use it.")

        ## ------
        ##  LVL1 : For LVL1 simulation only in Digitization job 
        ## ------
        elif configs[0] == 'LVL1':
            if configs[1] == 'DB' or configs[1]=='DBF':
                ### We read config from the TriggerDB
                tf.readMenuFromTriggerDb=True
                tf.triggerUseFrontier = (configs[1]=='DBF')
                tf.triggerDbConnection = ':'.join(configs[2:-1])  # the dbconnection goes from second to last ':', it can contain ':'
                DBkeys = configs[-1].split(",")
                if (len(DBkeys) == 2): #We got either 2 keys (SM, L1PS) or menu name plus release. If latter, second object will contain a .
                    if '.' not in str(DBkeys[1]):
                        tf.triggerDbKeys=[int(x) for x in DBkeys] +[-1,1] # SMkey, L1PSkey, HLTPSkey, BGkey
                        log.info("triggerConfig: LVL1 from DB with specified keys SMK %i and L1 PSK %i.", *tuple(tf.triggerDbKeys()[0:2]))
                    else:
                        log.info("triggerConfig: LVL1 from DB with speficied menu name and release: finding keys...")
                        tf.triggerDbKeys=getKeysFromNameRelease(tf.triggerDbConnection(),DBkeys[0],DBkeys[1],True) + [-1,1]
                        log.info("triggerConfig: LVl1 from DB with keys SMK %i and L1 PSK %i", *tuple(tf.triggerDbKeys()[0:2]))
                else:                  #We got a menu name which we need to look up - not implemented yet
                    log.info("triggerConfig: LVL1 from DB configured with wrong number of keys/arguments" )

            else:
                ### We read config from XML
                tf.readLVL1configFromXML=True
                if (configs[1] == 'DEFAULT' or configs[1] == 'default'):
                    tf.triggerMenuSetup = 'default'
                else:
                    tf.triggerMenuSetup = configs[1]
                log.info("triggerConfig: LVL1 menu from xml (%s)", tf.triggerMenuSetup())

                

        #------
        # MCRECO: Reconstruction of MC
        #------            
        elif configs[0] == 'MCRECO':
            from RecExConfig.RecFlags  import rec
            from RecJobTransforms.RecConfig import recConfig
            rec.doTrigger = True

            if configs[1] == 'DB' or configs[1]=='DBF':
                ### We read the menu from the TriggerDB
                tf.readMenuFromTriggerDb=True
                tf.triggerUseFrontier = (configs[1]=='DBF')
                tf.triggerDbConnection = ':'.join(configs[2:-1])  # the dbconnection goes from second to last ':', it can contain ':'
                DBkeys = configs[-1].split(",")
                if (len(DBkeys) == 4):                            # we got 4 keys (SM, L1PS, HLTPS,BGK)
                    tf.triggerDbKeys=[int(x) for x in DBkeys]
                    log.info("triggerConfig: MCRECO from DB with speficied keys SMK %i, L1 PSK %i, HLT PSK %i, and BGK %i.", *tuple(tf.triggerDbKeys()[0:4]))
                if (len(DBkeys) == 3):                            # we got 3 keys (SM, L1PS, HLTPS)
                    tf.triggerDbKeys=[int(x) for x in DBkeys] + [1]
                    log.info("triggerConfig: MCRECO from DB with speficied keys SMK %i, L1 PSK %i, and HLT PSK %i.", *tuple(tf.triggerDbKeys()[0:3]))
                elif (len(DBkeys) == 2):                       # we got a menu name and a release which we need to look up 
                    log.info("triggerConfig: MCRECO from DB with specified menu name and release: finding keys...")
                    tf.triggerDbKeys=getKeysFromNameRelease(tf.triggerDbConnection(),DBkeys[0],DBkeys[1],False) + [1]
                    log.info("triggerConfig: MCRECO from DB with keys SMK %i, L1 PSK %i, and HLT PSK %i.", *tuple(tf.triggerDbKeys()[0:3]))
                else:
                    log.info("triggerConfig: MCRECO from DB configured with wrong number of keys/arguments" )

                # we need to set triggerMenuSetup to the correct name
                # that we get from the triggerDB, otherwise
                # TriggerGetter->GenerateMenu.generateMenu() would be
                # run with the wrong menu and the configuration of the
                # algorithms would be incorrect (bug 72547)
                tf.triggerMenuSetup=getMenuNameFromDB(tf.triggerDbConnection(),tf.triggerDbKeys()[2])
                log.info("triggerConfig: Setting tf.triggerMenuSetup to " + tf.triggerMenuSetup())
            else:
                ### We read the menu from xml
                if (configs[-1] == 'DEFAULT' or configs[-1] == 'default'):
                    tf.triggerMenuSetup = 'default'
                else:
                    tf.triggerMenuSetup = configs[-1]

                tf.readLVL1configFromXML=True
                log.info("triggerConfig: MCRECO menu from xml (%s)", tf.triggerMenuSetup())

            # This part was there in the original (old) csc_reco_trigger.py snippet
            # Still wanted?
            if rec.doTrigger:
                # Switch off trigger of sub-detectors
                for detOff in recConfig.detectorsOff:
                    cmd = 'TriggerFlags.do%s = False' % detOff
                    # possibly not all DetFlags have a TriggerFlag
                    try:
                        exec(cmd)
                        log.info(cmd)
                    except AttributeError:
                        pass
        #------            
        # We passed a wrong argument to triggerConfig
        #------
        else:
            log.error("triggerConfig argument \""+self.get_Value()+"\" not understood. "
                      + "Please check in TriggerFlags.py to see the allowed values.")
            

_flags.append(triggerConfig)



class readL1TopoConfigFromXML(JobProperty):
    """Use to specify external l1topo xml configuration file
    (e.g. from the release or a local directory)
    
    If set to True:
    the L1Topo config will be taken from TriggerFlags.inputL1TopoConfigFile()
    
    If set to False:    
    the L1Topo config xml file is read from the python generated XML
    file, which is specified in TriggerFlags.outputL1TopoconfigFile()
    """
    statusOn=True
    allowedType=['bool']
    # note: if you change the following default value, you must also change the default value in class inputLVL1configFile
    # StoredValue=False
    StoredValue = False # once the python generation is implemented the default should be False

    def _do_action(self):
        """ setup some consistency """
        if self.get_Value():
            TriggerFlags.inputL1TopoConfigFile = "TriggerMenuXML/L1Topoconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            TriggerFlags.inputL1TopoConfigFile = TriggerFlags.outputL1TopoConfigFile()

_flags.append(readL1TopoConfigFromXML)

class readLVL1configFromXML(JobProperty):
    """ If set to True the LVL1 config file is read from earlier generated XML file """
    statusOn=True
    allowedType=['bool']
    # note: if you change the following default value, you must also change the default value in class inputLVL1configFile
    # StoredValue=False
    StoredValue = False

    def _do_action(self):
        """ setup some consistency """
        import os
        log = logging.getLogger( 'TriggerFlags.readLVL1configFromXML' )

        import TriggerMenuMT.LVL1MenuConfig.LVL1.Lvl1Flags  # noqa: F401
        
        if self.get_Value() is False:
            TriggerFlags.inputLVL1configFile = TriggerFlags.outputLVL1configFile()
            TriggerFlags.Lvl1.items.set_On()
        else:
            TriggerFlags.inputLVL1configFile = "TriggerMenuMT/LVL1config_"+_getMenuBaseName(TriggerFlags.triggerMenuSetup())+"_" + TriggerFlags.menuVersion() + ".xml"
            xmlFile=TriggerFlags.inputLVL1configFile()
            from TrigConfigSvc.TrigConfigSvcConfig import findFileInXMLPATH
            if xmlFile!='NONE' and not os.path.exists(findFileInXMLPATH(xmlFile)):
                log.error("Cannot find LVL1 xml file %s", xmlFile)

            TriggerFlags.Lvl1.items.set_Off()

_flags.append(readLVL1configFromXML)

class triggerDbKeys(JobProperty):
    """ define the keys [Configuration, LVL1Prescale, HLTPrescale, L1BunchGroupSet] in that order!"""
    statusOn=False
    allowedType=['list']
    StoredValue=[0,0,0,1]

_flags.append(triggerDbKeys)

class triggerDbConnection(JobProperty):
    """ define triggerDB connection parameters"""
    statusOn=False
    allowedType=['dict']
    StoredValue="TRIGGERDB"

_flags.append(triggerDbConnection)

class triggerCoolDbConnection(JobProperty):
    """ define connection parameters to cool if external sqlite file is to be used"""
    statusOn=True
    allowedType=['str']
    StoredValue=''

_flags.append(triggerCoolDbConnection)

class outputL1TopoConfigFile(JobProperty):
    """ File name for output L1Topo configuration XML file produced by the python menu generation """
    statusOn=True
    allowedType=['str']
    StoredValue=""

    def __call__(self):
        if self.get_Value() == "":
            # triggerMenuSetup contains the prescale mode in many
            # cases, e.g. MC_pp_v5_tight_mc_prescale. Prescaling is
            # not available for L1Topo, therefore that part is being
            # removed.
            return "L1Topoconfig_" + _getMenuBaseName(TriggerFlags.triggerMenuSetup()) + "_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(outputL1TopoConfigFile)

class outputLVL1configFile(JobProperty):
    """ File name for output LVL1 configuration XML file """
    statusOn=True
    StoredValue=""

    def __call__(self):
        if self.get_Value() == "":
            return "LVL1config_"+_getMenuBaseName(TriggerFlags.triggerMenuSetup())+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(outputLVL1configFile)

class outputHLTconfigFile(JobProperty):
    """ File name for output HLT configuration XML file """
    statusOn=True
#    allowedType=['str']
    StoredValue=""
    
    def __call__(self):
        if self.get_Value() == "":
            return "HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()

_flags.append(outputHLTconfigFile)

class outputHLTmenuJsonFile(JobProperty):
    """ File name for output HLT configuration XML file """
    statusOn=True
    StoredValue=""

    def __call__(self):
        if self.get_Value() == "":
            return "HLTMenu_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".json"
        else:
            return self.get_Value()

_flags.append(outputHLTmenuJsonFile)

class inputL1TopoConfigFile(JobProperty):
    """Used to define an external L1Topo configuration file. To be
    used together with trigger flag readL1TopoConfigFromXML.

    If TriggerFlags.readL1TopoConfigFromXML()==True, then this file is
    used for L1TopoConfiguration.
    
    Defaults to L1Topoconfig_<triggerMenuSetup>_<menuVersion>.xml
    """
    statusOn=True
    allowedType=['str']
    StoredValue=""

    def __call__(self):
        if self.get_Value() == "":
            return "L1Topoconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(inputL1TopoConfigFile)



class inputLVL1configFile(JobProperty):
    """ File name for input LVL1 configuration XML file """
    statusOn=True
#    allowedType=['str']
#   The following default is appropriate when XML cofig is the default
#    StoredValue="TriggerMenuXML/LVL1config_default_" + TriggerFlags.menuVersion() + ".xml"
#   The following default is appropriate when python config is the default
    StoredValue=""
#    StoredValue = "TriggerMenuXML/LVL1config_default_" + TriggerFlags.menuVersion() + ".xml"

    def __call__(self):
        if self.get_Value() == "":
            return "LVL1config_"+_getMenuBaseName(TriggerFlags.triggerMenuSetup())+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()

_flags.append(inputLVL1configFile)

# remove prescale suffixes
def _getMenuBaseName(menuName):
    m = re.match(r'(.*v\d(?:_primaries)?).*', menuName)
    if m:
        menuName = m.groups()[0]
    return menuName


class inputHLTconfigFile(JobProperty):
    """ File name for input HLT configuration XML file """
    statusOn=True
    allowedType=['str']
#   The following default is appropriate when XML cofig is the default
#    StoredValue="TriggerMenuXML/HLTconfig_default_" + TriggerFlags.menuVersion() + ".xml"
#   The following default is appropriate when python config is the default
    StoredValue=""
#    StoredValue = "TriggerMenuXML/HLTconfig_default_" + TriggerFlags.menuVersion() + ".xml"

    def __call__(self):
        if self.get_Value() == "":
            return "HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(inputHLTconfigFile)

# =================
#
# trigger menu flags - menu version, prescale sets
#
# =================
class menuVersion(JobProperty):
    """ Defines the menu version to use, usually the same as the release number. This is part of the XML file name. """
    statusOn=True
    allowedType=['str']
    
    from AthenaCommon.AppMgr import release_metadata
    StoredValue = release_metadata()['release']  # returns '?' if missing
    
_flags.append(menuVersion)


class triggerMenuSetup(JobProperty):
    """ Defines the luminosity dependent setup of trigger lumi01 == 10^33, switches on/off signatures """
    statusOn=True
    allowedType=['str']
    allowedValues = [
        'default', 'cosmic_default', 'InitialBeam_default',
        # menus for 10^31 with EMScale L1 calib
        'Physics_lumi1E31_simpleL1Calib','Physics_lumi1E31_simpleL1Calib_no_prescale',
        'MC_lumi1E31_simpleL1Calib','MC_lumi1E31_simpleL1Calib_no_prescale',
        'MC_lumi1E31_simpleL1Calib_physics_prescale',
        # menus for 10^32 with EMScale L1 calib
        'Physics_lumi1E32_simpleL1Calib','Physics_lumi1E32_simpleL1Calib_no_prescale',
        'MC_lumi1E32_simpleL1Calib','MC_lumi1E32_simpleL1Calib_no_prescale',
        'MC_lumi1E32_simpleL1Calib_physics_prescale',
        # menus for 10^33
        'Physics_lumi1E33','Physics_lumi1E33_no_prescale',
        'MC_lumi1E33','MC_lumi1E33_no_prescale',
        'Physics_lumi1E34','Physics_lumi1E34_no_prescale',
        'MC_lumi1E34','MC_lumi1E34_no_prescale',
        #
        'Cosmics','Cosmic_v1', 'Cosmic2009_v1', 'Cosmic2009_v2', 
        'InitialBeam_v1', 'MC_InitialBeam_v1', 'MC_InitialBeam_v1_no_prescale',
        'Cosmic2009_simpleL1Calib', 'Cosmic2009_inclMuons',
        'enhBias',
        # for 2010 running
        'Cosmic_v2','Cosmic_v3',
        'InitialBeam_v2', 'MC_InitialBeam_v2', 'MC_InitialBeam_v2_no_prescale',
        'InitialBeam_v3', 'MC_InitialBeam_v3', 'MC_InitialBeam_v3_no_prescale',
        #for 2010-2011 running
        'Physics_pp_v1', 'Physics_pp_v1_no_prescale', 'Physics_pp_v1_cosmics_prescale',
        'MC_pp_v1', 'MC_pp_v1_no_prescale',
        'MC_pp_v1_tight_mc_prescale', 'MC_pp_v1_loose_mc_prescale',
        #v2 
        'Physics_pp_v2', 'Physics_pp_v2_no_prescale', 'Physics_pp_v2_cosmics_prescale', 
        'MC_pp_v2', 'MC_pp_v2_primary', 'MC_pp_v2_no_prescale', 
        'MC_pp_v2_tight_mc_prescale', 'MC_pp_v2_loose_mc_prescale',
        #v3
        'Physics_pp_v3', 'Physics_pp_v3_no_prescale', 'Physics_pp_v3_cosmics_prescale', 
        'MC_pp_v3', 'MC_pp_v3_primary', 'MC_pp_v3_no_prescale', 
        'MC_pp_v3_tight_mc_prescale', 'MC_pp_v3_loose_mc_prescale',
        #v4
        'Physics_pp_v4', 'Physics_pp_v4_no_prescale', 'Physics_pp_v4_cosmics_prescale',
        'MC_pp_v4', 'MC_pp_v4_primary', 'MC_pp_v4_no_prescale',
        'MC_pp_v4_upgrade_mc_prescale','MC_pp_v4_tight_mc_prescale', 'MC_pp_v4_loose_mc_prescale',
        # L1 v2 for testing
        'L1_pp_v2',
        'L1_pp_v3',
        'L1_pp_v4',
        'L1_pp_test',
        'L1_alfa_v1',
        'L1_alfa_v2',
        # for HeavyIon
        'InitialBeam_HI_v1', 'InitialBeam_HI_v1_no_prescale',
        'MC_InitialBeam_HI_v1', 'MC_InitialBeam_HI_v1_no_prescale',
        'Physics_HI_v1', 'Physics_HI_v1_no_prescale',
        'MC_HI_v1',     'MC_HI_v1_no_prescale', 'MC_HI_v1_pPb_mc_prescale',
        #
        'Physics_HI_v2', 'Physics_HI_v2_no_prescale', 
        'MC_HI_v2',  'MC_HI_v2_no_prescale', 'MC_HI_v2_pPb_mc_prescale',
        #
        'Physics_default', 'MC_loose_default', 'MC_tight_default', 'default_loose', 'default_tight',
        # -----------------------------------------------------------------
        # Run 2
        'MC_pp_v5', 'MC_pp_v5_no_prescale', 'MC_pp_v5_tight_mc_prescale', 'MC_pp_v5_loose_mc_prescale','MC_pp_v5_special_mc_prescale', # for development and simulation
        'Physics_pp_v5', # for testing algorithms and software quality during LS1, later for data taking
        'Physics_pp_v5_cosmics_prescale',
        'Physics_pp_v5_tight_physics_prescale', 
        'LS1_v1', # for P1 detector commissioning (cosmics, streamers)
        'DC14', 'DC14_no_prescale', 'DC14_tight_mc_prescale', 'DC14_loose_mc_prescale', # for DC14
        'Physics_HI_v3', 'Physics_HI_v3_no_prescale', # for 2015 lead-lead menu 
        'MC_HI_v3', 'MC_HI_v3_tight_mc_prescale',
        'Physics_HI_v4', 'Physics_HI_v4_no_prescale', # for 2016 proton-lead menu 
        'MC_HI_v4', 'MC_HI_v4_tight_mc_prescale',

        'MC_pp_v6','Physics_pp_v6','MC_pp_v6_no_prescale', 'MC_pp_v6_tight_mc_prescale', 'MC_pp_v6_tightperf_mc_prescale', 'MC_pp_v6_loose_mc_prescale','Physics_pp_v6_tight_physics_prescale',
        'MC_pp_v7','Physics_pp_v7','Physics_pp_v7_primaries','MC_pp_v7_no_prescale', 'MC_pp_v7_tight_mc_prescale', 'MC_pp_v7_tightperf_mc_prescale', 'MC_pp_v7_loose_mc_prescale','Physics_pp_v7_tight_physics_prescale',
        # -----------------------------------------------------------------
        # Run 3 (and preparation for Run-3)
        'LS2_v1', # for development of AthenaMT
        'LS2_emu_v1', # emulation test menu for AthenaMT
        'MC_pp_run3_v1', # MC_pp_run3 for AthenaMT
        'PhysicsP1_pp_run3_v1', # PhysicsP1_pp_run3 for AthenaMT
        'Physics_pp_run3_v1', # Physics_pp_run3 for AthenaMT
        'PhysicsP1_HI_run3_v1',  # PhysicsP1_HI_run3 for AthenaMT
        'Dev_HI_run3_v1', # Dev_HI_run3 for AthenaMT
        'MC_pp_v7_TriggerValidation_mc_prescale', # MC trigger simulated in 21.0 but reconstructed in 22.0
        'MC_pp_v8', 'Physics_pp_v8', 'MC_pp_v8_no_prescale', 'MC_pp_v8_tight_mc_prescale', 'MC_pp_v8_tightperf_mc_prescale', 'MC_pp_v8_loose_mc_prescale','Physics_pp_v8_tight_physics_prescale',
        'Cosmic_run3_v1',
        'LS2_v1_TriggerValidation_prescale',
        'LS2_v1_BulkMCProd_prescale',
        'LS2_v1_CPSampleProd_prescale'
        ]

    _default_menu='Physics_pp_v7_primaries'
    _default_cosmic_menu='Physics_pp_v5_cosmics_prescale'
    _default_InitialBeam_menu='MC_InitialBeam_v3_no_prescale'
    
    StoredValue = _default_menu

    def _do_action(self):
        """ setup some consistency """

        # meaning full default menu
        if self.get_Value() == 'default':
            self.set_Value(self._default_menu)
            self._log.info("%s - trigger menu 'default' changed to '%s'", self.__class__.__name__, self.get_Value())
        elif self.get_Value() == 'cosmic_default':
            self.set_Value(self._default_cosmic_menu)
            self._log.info("%s - trigger menu 'cosmic_default' changed to '%s'", self.__class__.__name__, self.get_Value())
        elif self.get_Value() == 'InitialBeam_default':
            self.set_Value(self._default_InitialBeam_menu)
            self._log.info("%s - trigger menu 'InitialBeam_default' changed to '%s'", self.__class__.__name__, self.get_Value())

        # filenames for LVL1 and HLT
        if TriggerFlags.readLVL1configFromXML() is True:
            TriggerFlags.inputLVL1configFile = "LVL1config_"+_getMenuBaseName(TriggerFlags.triggerMenuSetup())+"_" + TriggerFlags.menuVersion() + ".xml"

_flags.append(triggerMenuSetup)

class L1PrescaleSet(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    allowedValues = ['', 'None']
    StoredValue = ''
_flags.append(L1PrescaleSet)

class HLTPrescaleSet(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    allowedValues = ['', 'None']
    StoredValue = ''
_flags.append(HLTPrescaleSet)


# the container of all trigger flags

class Trigger(JobPropertyContainer):
    """ Trigger top flags """
      

    def Slices_all_setOn(self):
        """ Runs setL2 and setEF in all slices. Effectivelly enable trigger. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.setAll()


    def Slices_all_setOff(self):
        """ Runs unsetAll in all slices. Effectivelly disable trigger. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.unsetAll()


## attach yourself to the RECO flags
from RecExConfig.RecFlags import rec
rec.add_Container(Trigger)


for flag in _flags:
    rec.Trigger.add_JobProperty(flag)
del _flags

## make an alias for trigger flags which looks like old TriggerFlags class
TriggerFlags = rec.Trigger


## add online specific flags
import TriggerJobOpts.TriggerOnlineFlags    # noqa: F401

## add slices generation flags
log.info("TriggerFlags importing SliceFlags"  )
from TriggerJobOpts.SliceFlags import *                             # noqa: F401, F403


def sync_Trigger2Reco():
    from AthenaCommon.Include import include
    from RecExConfig.RecAlgsFlags import recAlgs
    from AthenaCommon.GlobalFlags  import globalflags
    from RecExConfig.RecFlags import rec
    
    if  recAlgs.doTrigger() and rec.readRDO() and not globalflags.InputFormat()=='bytestream':
        include( "TriggerJobOpts/TransientBS_DetFlags.py" )

    if globalflags.InputFormat() == 'bytestream':
        TriggerFlags.readBS = True
        TriggerFlags.doLVL1 = False
        TriggerFlags.doHLT   = False

    if rec.doWriteBS():
        TriggerFlags.writeBS = True

del log
