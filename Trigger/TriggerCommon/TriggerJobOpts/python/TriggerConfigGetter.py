# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigConfigSvc.TrigConfigSvcUtils import interpretConnection

from TriggerJobOpts.TriggerFlags import TriggerFlags
from RecExConfig.RecFlags  import rec
from RecExConfig.RecAlgsFlags import recAlgs

from AthenaCommon.GlobalFlags  import globalflags
from RecExConfig.Configured import Configured 

from AthenaCommon.Logging import logging
from AthenaCommon.Resilience import protectedInclude
from AthenaCommon.AppMgr import ServiceMgr as svcMgr


class TriggerConfigGetter(Configured):
    """ This class brings to the job Trigger Configuration.
    """

    _environment=""

    def __init__(self, environment=""):
        log = logging.getLogger( "TriggerConfigGetter.py" )
        if environment:
            log.info('Initialize (environment "%s")', environment)
        else:
            log.info('Initialize (interpreting rec and trigger flags)')
        self._environment = environment
        super(TriggerConfigGetter,self).__init__() # calls configure


    def checkFileMetaData(self):
        log = logging.getLogger( "TriggerConfigGetter.py" )
        from PyUtils.MetaReaderPeekerFull import metadata
        self.hasLBwiseHLTPrescalesAndL1ItemDef = True

        # protection against early runs
        if 'runNumbers' in metadata and self._environment == "" and globalflags.DataSource() == 'data' and rec.readRDO() and any([run < 134230 for run in metadata['runNumbers']]):
            self.hasLBwiseHLTPrescalesAndL1ItemDef = False
        if self.hasLBwiseHLTPrescalesAndL1ItemDef:
            log.info("Using LB-wise HLT prescales")
        else:
            log.info("Using run-wise HLT prescales")


    def checkInput(self):
        self.checkFileMetaData()

        log = logging.getLogger( "TriggerConfigGetter.py" )
        if "ReadPool" in self._environment or "WritePool" in self._environment:
            log.info("Flags are ignored, since 'ReadPool' or  'WritePool' is specified")
            return True

        if rec.readESD() or rec.readAOD(): # and globalflags.DataSource()=='data':  # need this for MC as well
            protectedInclude("TriggerJobOpts/TriggerConfigCheckMetadata.py")

        if rec.readRDO() and globalflags.InputFormat()=='bytestream' and globalflags.DataSource()=='data' and TriggerFlags.configForStartup()!='HLToffline':
            protectedInclude("TriggerJobOpts/TriggerConfigCheckHLTpsk.py")

        log.info("The following flags are set:")
        log.info("globalflags.InputFormat             : %s", globalflags.InputFormat())
        log.info("rec.read.*                          : RDO: %s, ESD: %s, AOD: %s, TAG: %s", rec.readRDO(), rec.readESD(), rec.readAOD(), rec.readTAG())
        log.info("rec.doWrite.*                       : ESD: %s, AOD: %s, TAG: %s", rec.doWriteESD(), rec.doWriteAOD(), rec.doWriteTAG())
        log.info("globalflags.DataSource              : %s", globalflags.DataSource())
        log.info("TriggerFlags.configForStartup       : %s", TriggerFlags.configForStartup())
        log.info("TriggerFlags.dataTakingConditions   : %s", TriggerFlags.dataTakingConditions())
        log.info("TriggerFlags.configurationSourceList: %s", TriggerFlags.configurationSourceList())

        count = len([1 for x in [rec.readRDO(),rec.readESD(),rec.readAOD()] if x ]) #readTAG is only set with another input
        if count == 0:
            log.warning("Don't know what the input format is.")
            return False
        if count >= 2:
            log.warning("More than one input format specified, please set only the appropriate one.")
            return False
        return True


    def setConfigSvcConnParams(self,connectionParameters):
        sl = []
        if hasattr(svcMgr,'LVL1ConfigSvc'):
            sl += [svcMgr.LVL1ConfigSvc]
        if hasattr(svcMgr,'HLTConfigSvc'):
            sl += [svcMgr.HLTConfigSvc]

        if "alias" in connectionParameters:
            for svc in sl:
                svc.ConfigSource = 'DBLookUp'
                svc.DBServer  = connectionParameters["alias"]
                svc.DBUser = ""
        else:
            technology = connectionParameters["techno"]
            for svc in sl:
                svc.ConfigSource = technology

            if technology == 'sqlite':
                for svc in sl:
                    svc.DBServer = connectionParameters["filename"]
                    svc.DBUser = "dummy"

            if technology == 'oracle':
                for svc in sl:
                    svc.DBServer  = connectionParameters["server"]
                    svc.DBAccount = connectionParameters["schema"]
                    svc.DBUser    = connectionParameters["user"  ]
                    svc.DBPass    = connectionParameters["passwd"]

        if hasattr(svcMgr,'LVL1ConfigSvc'):
            svcMgr.LVL1ConfigSvc.DBSMKey     = TriggerFlags.triggerDbKeys()[0]
            svcMgr.LVL1ConfigSvc.DBLVL1PSKey = TriggerFlags.triggerDbKeys()[1]
            svcMgr.LVL1ConfigSvc.DBBGSKey    = TriggerFlags.triggerDbKeys()[3]
            svcMgr.LVL1ConfigSvc.UseFrontier = TriggerFlags.triggerUseFrontier()
        if hasattr(svcMgr,'HLTConfigSvc'):
            svcMgr.HLTConfigSvc.DBSMKey      = TriggerFlags.triggerDbKeys()[0]
            svcMgr.HLTConfigSvc.DBHLTPSKey   = TriggerFlags.triggerDbKeys()[2]
            svcMgr.HLTConfigSvc.UseFrontier  = TriggerFlags.triggerUseFrontier()


    def configure(self):
        log = logging.getLogger( "TriggerConfigGetter.py" )
        from PyUtils.MetaReaderPeekerFull import metadata

        # first check the input
        if "HIT2RDO" in self._environment:
            TriggerFlags.doHLT = False
            log.info("For simulation jobs the following flags are set:")
            log.info("globalflags.InputFormat             : %s", globalflags.InputFormat())
            log.info("globalflags.DataSource              : %s", globalflags.DataSource())
            log.info("TriggerFlags.configForStartup       : %s", TriggerFlags.configForStartup())
            log.info("TriggerFlags.dataTakingConditions   : %s", TriggerFlags.dataTakingConditions())
            log.info("TriggerFlags.doHLT                  : %s", TriggerFlags.doHLT())
        else:
            if not self.checkInput():
                log.error("Could not determine job input. Can't setup trigger configuration and will return!")
                return
            # self.checkInput() may call TriggerConfigCheckMetadata, this can in turn set "rec.doTrigger.set_Value_and_Lock(False)"
            # but TriggerConfigGetter might have only been called in the first place due to this flag having been true, 
            # so re-check that we're still OK to be executing here
            if not (recAlgs.doTrigger() or rec.doTrigger() or TriggerFlags.doTriggerConfigOnly()):
                log.info("Aborting TriggerConfigGetter as the trigger flags were switched to false in checkInput()")
                return True

        self.readPool       = globalflags.InputFormat() == 'pool'
        self.readRDO        = rec.readRDO()
        self.writeESDAOD    = rec.doWriteESD() or rec.doWriteAOD() or rec.doWriteDPD()
        self.writeAOD       = rec.doWriteAOD() or rec.doWriteDPD()
        self.ConfigSrcList  = TriggerFlags.configurationSourceList()
        self.readMC         = globalflags.DataSource()=='geant3' or globalflags.DataSource()=='geant4'
        self.readTriggerDB  = TriggerFlags.readMenuFromTriggerDb() and self.readRDO
        self.isCommisioning = globalflags.DataSource()=='data' and globalflags.DetGeo()=='commis' 
        self.l1Folders      = TriggerFlags.dataTakingConditions()=='FullTrigger' or TriggerFlags.dataTakingConditions()=='Lvl1Only'
        self.hltFolders     = TriggerFlags.dataTakingConditions()=='FullTrigger' or TriggerFlags.dataTakingConditions()=='HltOnly'
        self.isRun1Data     = False 
        self.hasxAODMeta    = ( 
            ("metadata_items" in metadata)
            and 
            any((('TriggerMenu' or 'MenuJSON') in key) for key in metadata["metadata_items"].keys())
        )

        if globalflags.DataSource()=='data':
            from RecExConfig.AutoConfiguration  import GetRunNumber
            runNumber = GetRunNumber()
            if runNumber is not None and runNumber > 0 and runNumber < 230000 :
                self.isRun1Data = True
        self.isTriggerReprocessing = False

        if self._environment: # I don't think anyone calls TriggerConfigGetter with an argument
            self.readPool  = False
            self.writeESDAOD = False
            self.readHits = False
            if "ReadPoolRDO" in self._environment:
                self.readPool = True
                self.readRDO = True
            elif "ReadPool" in self._environment:
                self.readPool = True
                self.readRDO = False
            if "WritePool" in self._environment:
                self.writeESDAOD = True
            if "HIT2RDO" in self._environment:
                self.readRDO = False
                self.readHits = True


        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        # define ConfigSvc (for Run 1 + 2)
        if ConfigFlags.Trigger.EDMVersion == 1 or ConfigFlags.Trigger.EDMVersion == 2:
            if not self.ConfigSrcList:
                if (self.readPool and not self.readRDO) or (self.readRDO and not self.readPool): # (ESD, AOD, DPD) or (RDO-BS)
                    self.ConfigSrcList = ['ds']
                elif (self.readRDO and self.readPool) or rec.readTAG() or self.readHits:           # (RDO-MC) or TAG
                    self.ConfigSrcList = ['xml']
                else: # should not get here: should be found by checkInput
                    log.fatal('no reading of BS, RDO, AOD, ESD, or TAG specified')

        log.info('Creating the Trigger Configuration Services')

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr

        ########################################################################
        # START OF TEMPORARY SOLUTION FOR RUN-3 TRIGGER DEVELOPMENT
        ########################################################################
        log.info("ConfigFlags.Trigger.EDMVersion: %i", ConfigFlags.Trigger.EDMVersion)
        if ConfigFlags.Trigger.EDMVersion >= 3:
            if self.hasxAODMeta:
                if not hasattr(svcMgr, 'xAODConfigSvc'):
                    from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigSvc
                    svcMgr += TrigConf__xAODConfigSvc('xAODConfigSvc')
            else: # Does not have xAODMeta
                # Run-3 Trigger Configuration Services (just producing menu data)
                from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
                from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg,HLTConfigSvcCfg
                CAtoGlobalWrapper(L1ConfigSvcCfg,ConfigFlags)
                CAtoGlobalWrapper(HLTConfigSvcCfg,ConfigFlags)

        else:
            # non-MT (Run-2) Trigger Configuration
            if 'ds' in self.ConfigSrcList:
                log.info("setup DSConfigSvc and add instance to ServiceMgr")
                from TrigConfigSvc.TrigConfigSvcConf import TrigConf__DSConfigSvc
                svcMgr += TrigConf__DSConfigSvc("DSConfigSvc")

        ########################################################################
        # END OF TEMPORARY SOLUTION FOR RUN-3 TRIGGER DEVELOPMENT
        ########################################################################

        if self.readTriggerDB:
            log.info( "Using TriggerDB connection '%s'", TriggerFlags.triggerDbConnection() )
            self.trigDbConnectionParameters = interpretConnection(TriggerFlags.triggerDbConnection(), resolveAlias=False)
            self.setConfigSvcConnParams(self.trigDbConnectionParameters)

        log.info("TriggerFlags.triggerCoolDbConnection is '%s' [default: '']", TriggerFlags.triggerCoolDbConnection())
        if ('ds' in self.ConfigSrcList) and not self.hasxAODMeta:
            self.setupCOOLReading(TriggerFlags.triggerCoolDbConnection())

        if hasattr(svcMgr, 'DSConfigSvc'):
            db = 'TRIGGERDB'
            if self.isRun1Data:
                db = 'TRIGGERDB_RUN1'
            elif self.readMC:
                db = 'TRIGGERDBMC'
            elif self.isTriggerReprocessing:
                db = 'TRIGGERDBREPR'
            svcMgr.DSConfigSvc.ConfigSource = 'dblookup'
            svcMgr.DSConfigSvc.DBServer = db
            log.info("DSConfigSvc trigger database is '%s'", db)
        
        if not self.hasxAODMeta:
            self.setupxAODWriting()
        else:
            log.info("Input file already has xAOD trigger metadata. Will not re-create it.")

        # all went fine we are configured
        return True


    def setupCOOLReading(self,TrigCoolDbConnection):
        log = logging.getLogger( "TriggerConfigGetter.py" )
        log.info( 'Will setup IOVDbSvc to access configuration meta data because "ds" specified and no xAODMetadata available')
        log.info( '  local ConfigSrcList: %r', self.ConfigSrcList)
        log.info( '  hasxAODMeta        : %r', self.hasxAODMeta)

        ## if we process MC from an XML file the dbConnection needs to
        ## be set to a local SQlite file

        # when reading from the COOL database (either form
        # ORACLE/SQlite) we need an algorithm that triggers the
        # copying
        #
        # In addition for MC the IOV has to be adjusted since in COOL the config data is written with IOV=infinity

        if TrigCoolDbConnection=='':
            log.info("COOL DBConnection: not set, will use default conditions database" )
            TrigCoolDbConnection = 'TRIGGER'
            addNewFolders = TriggerFlags.configForStartup()=="HLTonline" and self.readRDO
        else: # for sqlite COOL: temp (usually /tmp/hltMenu.xxx.db) or predefined (e.g. trigconf.db)
            log.info("COOL DBConnection: %s", TrigCoolDbConnection)
            addNewFolders = globalflags.DataSource()!='data' and self.readRDO # bytestream or MC RDO

        # add folders for reading
        from IOVDbSvc.CondDB import conddb
        folders = []
        if self.hltFolders:
            folders += [ "HLT/Menu", "HLT/HltConfigKeys" ]
        if self.l1Folders:
            folders += [ "LVL1/Lvl1ConfigKey", "LVL1/Menu", "LVL1/Prescales" ]
        if globalflags.DataSource() == 'data':
            if self.l1Folders:
                folders += [ "LVL1/BunchGroupKey" ]
                #if TriggerFlags.EDMDecodingVersion() < 3:
                #   folders += [ "LVL1/BunchGroupDescription", "LVL1/BunchGroupContent" ]

        if self.hasLBwiseHLTPrescalesAndL1ItemDef:
            if self.hltFolders:
                folders += [ "HLT/Prescales", "HLT/PrescaleKey" ]
            if self.l1Folders:
                folders += [ "LVL1/ItemDef" ]

        log.info("Adding folders to IOVDbSvc")

        if addNewFolders:
            # Need thresholds folders but only for Tier0 BS->ESD
            log.info("Also adding new folders to IOVDbSvc")
            if self.hltFolders:
                folders += [ "HLT/Groups" ]
            if self.l1Folders:
                folders += [ "LVL1/Thresholds" ]

        for f in folders:
            log.info("     /TRIGGER/%s", f)
            conddb.addFolderWithTag(TrigCoolDbConnection, "/TRIGGER/%s" % f, "HEAD")

    def setupxAODWriting( self ):
        """
        Method setting up the writing of the ROOT-readable trigger configuration
        metadata.
        """

        # Get a logger:
        log = logging.getLogger( "TriggerConfigGetter.py" )
        log.info( "ESD/AOD writing enabled, will set up xAOD trigger "
                  "configuration metadata writing" )

        # Get the algorithm sequence:
        from AthenaCommon.AlgSequence import AlgSequence
        topAlgs = AlgSequence()

        # Add the algorithm creating the trigger configuration metadata for
        # the output:
        writeTriggerMenu = False # Run2 offline xAOD metadata summary format. Writing of this now is deprecated. Reading supported still.
        writeMenuJSON = False # Run3 offline xAOD metadata summary format
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        if ConfigFlags.Trigger.EDMVersion == 1 or ConfigFlags.Trigger.EDMVersion == 2:
            if ConfigFlags.Trigger.doConfigVersionConversion:
                log.info("Configuring Run-1&2 to Run-3 configuration metadata conversion")

                # Save the menu in JSON format
                from RecExConfig.AutoConfiguration import GetRunNumber, GetLBNumber
                dbKeys = fetchRun3ConfigFiles(isMC=self.readMC, run=GetRunNumber(), lb=GetLBNumber())

                from TrigConfigSvc.TrigConfigSvcConf import (TrigConf__LVL1ConfigSvc,
                                                             TrigConf__HLTConfigSvc,
                                                             TrigConf__HLTPrescaleCondAlg,
                                                             TrigConf__L1PrescaleCondAlg)
                from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigSvc

                from AthenaCommon.AlgSequence import AthSequencer
                condSeq = AthSequencer ('AthCondSeq')
                from AthenaCommon.AppMgr import ServiceMgr as svcMgr
                from AthenaCommon.AppMgr import theApp

                # L1 service and CondAlg
                l1ConfigSvc = TrigConf__LVL1ConfigSvc("LVL1ConfigSvcRun3",
                                                      InputType="file",
                                                      JsonFileName="L1Menu.json",
                                                      SMK=dbKeys["SMK"],
                                                      JsonFileNameBGS="BunchGroups.json",
                                                      BGSK=dbKeys["BGSK"])
                svcMgr += l1ConfigSvc
                theApp.CreateSvc += [ l1ConfigSvc.getFullName() ]

                condSeq += TrigConf__L1PrescaleCondAlg("L1PrescaleCondAlgRun3",
                                                       Source="FILE",
                                                       L1Psk=dbKeys["L1PSK"],
                                                       Filename="L1PrescalesSet.json")

                # HLT service and CondAlg
                hltConfigSvc = TrigConf__HLTConfigSvc("HLTConfigSvcRun3",
                                                      InputType="file",
                                                      JsonFileName="HLTMenu.json",
                                                      SMK=dbKeys["SMK"])
                svcMgr += hltConfigSvc
                theApp.CreateSvc += [ hltConfigSvc.getFullName() ]

                condSeq += TrigConf__HLTPrescaleCondAlg("HLTPrescaleCondAlgRun3",
                                                        Source="FILE",
                                                        HLTPsk=dbKeys["HLTPSK"],
                                                        Filename="HLTPrescalesSet.json")

                # xAODConfigSvc for accessing the Run-3 converted menu
                svcMgr += TrigConf__xAODConfigSvc('xAODConfigSvc',
                                                  UseInFileMetadata = False)

                from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODMenuWriterMT, TrigConf__KeyWriterTool
                menuwriter = TrigConf__xAODMenuWriterMT()
                menuwriter.KeyWriterTool = TrigConf__KeyWriterTool('KeyWriterToolOffline')
                writeMenuJSON = True
                topAlgs += menuwriter

            else:
                from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODMenuWriter
                topAlgs += TrigConf__xAODMenuWriter( OverwriteEventObj = True )
                writeTriggerMenu = True

        elif ConfigFlags.Trigger.EDMVersion >= 3:
            from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODMenuWriterMT, TrigConf__KeyWriterTool
            menuwriter = TrigConf__xAODMenuWriterMT()
            menuwriter.KeyWriterTool = TrigConf__KeyWriterTool('KeyWriterToolOffline')
            writeMenuJSON = True
            topAlgs += menuwriter
            # Schedule also the prescale conditions algs
            from AthenaCommon.Configurable import Configurable
            Configurable.configurableRun3Behavior += 1
            from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator, appendCAtoAthena
            from TrigConfigSvc.TrigConfigSvcCfg import  L1PrescaleCondAlgCfg, HLTPrescaleCondAlgCfg
            acc = ComponentAccumulator()
            acc.merge( L1PrescaleCondAlgCfg( ConfigFlags ) )
            acc.merge( HLTPrescaleCondAlgCfg( ConfigFlags ) )
            appendCAtoAthena( acc )
            Configurable.configurableRun3Behavior -= 1

        else:
            raise RuntimeError("Invalid EDMVersion=%s " % ConfigFlags.Trigger.EDMVersion)

          # Set up the metadata for the output ESD and AOD:
        from RecExConfig.ObjKeyStore import objKeyStore

         # The metadata objects to add to the output:
        if writeTriggerMenu:
            metadataItems = [ "xAOD::TriggerMenuContainer#TriggerMenu",
                              "xAOD::TriggerMenuAuxContainer#TriggerMenuAux." ]
            objKeyStore.addManyTypesMetaData( metadataItems )

        if writeMenuJSON:
            metadataItems = [ "xAOD::TriggerMenuJsonContainer#MenuJSON_HLT",
                              "xAOD::TriggerMenuJsonAuxContainer#MenuJSON_HLTAux.",
                              "xAOD::TriggerMenuJsonContainer#MenuJSON_L1",
                              "xAOD::TriggerMenuJsonAuxContainer#MenuJSON_L1Aux.",
                              "xAOD::TriggerMenuJsonContainer#MenuJSON_HLTPS",
                              "xAOD::TriggerMenuJsonAuxContainer#MenuJSON_HLTPSAux.",
                              "xAOD::TriggerMenuJsonContainer#MenuJSON_L1PS",
                              "xAOD::TriggerMenuJsonAuxContainer#MenuJSON_L1PSAux.",
                              # "xAOD::TriggerMenuJsonContainer#MenuJSON_BG", // TODO
                              # "xAOD::TriggerMenuJsonAuxContainer#MenuJSON_BGAux.", // TODO
                            ]
            objKeyStore.addManyTypesMetaData( metadataItems )

        if ConfigFlags.Trigger.EDMVersion >= 3:
            from TrigEDMConfig.TriggerEDMRun3 import recordable
            from AthenaConfiguration.ComponentFactory import CompFactory
            from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable

            enhancedBiasWeightCompAlg = CompFactory.EnhancedBiasWeightCompAlg()
            enhancedBiasWeightCompAlg.EBWeight = recordable("HLT_EBWeight")
            from TrigDecisionTool.TrigDecisionToolConfig import getRun3NavigationContainerFromInput
            enhancedBiasWeightCompAlg.FinalDecisionKey = getRun3NavigationContainerFromInput(ConfigFlags)
            topAlgs += conf2toConfigurable( enhancedBiasWeightCompAlg )


""" Retrieve Run2 trigger configuration from the database and save as Run3 .JSON files with known name.
    The Run3 offline trigger infrastructure should then be configured from these .JSON files.
"""
def fetchRun3ConfigFiles(isMC, run, lb):
    import subprocess
    triggerDBKeys = {}
    if isMC:
        triggerDBKeys['DB'] = TriggerFlags.triggerDbConnection()
        triggerDBKeys['SMK'] = TriggerFlags.triggerDbKeys()[0]
        triggerDBKeys['L1PSK'] = TriggerFlags.triggerDbKeys()[1]
        triggerDBKeys['HLTPSK'] = TriggerFlags.triggerDbKeys()[2]
        triggerDBKeys['BGSK'] = TriggerFlags.triggerDbKeys()[3]
    else:
        from TrigConfigSvc.TrigConfigSvcCfg import getTrigConfFromCool
        triggerDBKeys = getTrigConfFromCool(run, lb)
        triggerDBKeys['DB'] = 'TRIGGERDB' if run > 230000 else 'TRIGGERDB_RUN1'

    cmd = "TrigConfReadWrite -i {DB} {SMK},{L1PSK},{HLTPSK},{BGSK} -o r3json > Run3ConfigFetchJSONFiles.log".format(**triggerDBKeys)
    log = logging.getLogger( "TriggerConfigGetter.py" )
    log.info("Running command '%s'", cmd)
    filesFetchStatus = subprocess.run(cmd, shell=True)
    assert filesFetchStatus.returncode == 0, "TrigConfReadWrite failed to fetch JSON files"
    return triggerDBKeys
    


if __name__ == "__main__":
    keys = fetchRun3ConfigFiles(isMC=False, run=360026, lb=151)
    for k,v in {"SMK" : 2749, "L1PSK" : 23557, "HLTPSK" : 17824, "BGSK" : 2181}.items():
        assert  k in keys, "Missing key {}".format(k)
        assert v == keys[k], "Wrong value {}".format(v)
