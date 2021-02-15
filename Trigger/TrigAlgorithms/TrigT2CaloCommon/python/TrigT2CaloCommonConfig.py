# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigDataAccess as _TrigDataAccess
from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc as _TrigCaloDataAccessSvc

class TrigDataAccess(_TrigDataAccess):
    __slots__ = ()

    def __init__(self, name='TrigDataAccess'):
        super(TrigDataAccess, self).__init__(name)

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        from AthenaCommon.GlobalFlags import globalflags
        from AthenaCommon.Logging import logging
        log = logging.getLogger(name)
     
        self.loadFullCollections = TriggerFlags.doHLT()
        self.loadAllSamplings    = TriggerFlags.doHLT()
        log.info('Not possible anymore to enable HLT calo offset correction, tools deprecated')

        from RecExConfig.RecFlags import rec
        transientBS = (rec.readRDO() and not globalflags.InputFormat()=='bytestream')
        if ( transientBS or TriggerFlags.doTransientByteStream() ):
            if ( not hasattr(svcMgr.ToolSvc,'LArRawDataContByteStreamTool') ):
                from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolConfig
                svcMgr.ToolSvc += LArRawDataContByteStreamToolConfig()
            svcMgr.ToolSvc.LArRawDataContByteStreamTool.DSPRunMode=4
            svcMgr.ToolSvc.LArRawDataContByteStreamTool.RodBlockVersion=10

        return

class TrigCaloDataAccessSvc(_TrigCaloDataAccessSvc):
    __slots__ = ()

    def __init__(self, name='TrigCaloDataAccessSvc'):
        super(TrigCaloDataAccessSvc, self).__init__(name)

        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        from AthenaCommon.GlobalFlags import globalflags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        from AthenaCommon.Logging import logging
        log = logging.getLogger(name)

        from RegionSelector.RegSelToolConfig import makeRegSelTool_TTEM, makeRegSelTool_TTHEC, makeRegSelTool_FCALEM, makeRegSelTool_FCALHAD, makeRegSelTool_TILE
        from AthenaCommon.AlgSequence import AthSequencer
        from LArCabling.LArCablingAccess import LArOnOffIdMapping
        LArOnOffIdMapping()
        self.RegSelToolEM = makeRegSelTool_TTEM()
        self.RegSelToolHEC = makeRegSelTool_TTHEC()
        self.RegSelToolFCALEM = makeRegSelTool_FCALEM()
        self.RegSelToolFCALHAD = makeRegSelTool_FCALHAD()
        self.RegSelToolTILE = makeRegSelTool_TILE()
        condseq = AthSequencer('AthCondSeq')
        condseq.RegSelCondAlg_TTEM.RegSelLUT="ConditionStore+RegSelLUTCondData_TTEM"

        if ( globalflags.DatabaseInstance == "COMP200" and TriggerFlags.doCaloOffsetCorrection() ) :
            log.warning("Not possible to run BCID offset correction with COMP200")
        else:
            if TriggerFlags.doCaloOffsetCorrection():
                if globalflags.DataSource()=='data' and athenaCommonFlags.isOnline():
                    log.info('Enable HLT calo offset correction for data')
                    from IOVDbSvc.CondDB import conddb
                    conddb.addFolder("LAR_ONL","/LAR/ElecCalibFlat/OFC")
                    from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc
                    svcMgr += LArFlatConditionSvc()
                    svcMgr.LArFlatConditionSvc.OFCInput="/LAR/ElecCalibFlat/OFC"
                    svcMgr.ProxyProviderSvc.ProviderNames += [ "LArFlatConditionSvc" ]

                    from IOVDbSvc.CondDB import conddb
                    conddb.addFolder("LAR_ONL","/LAR/ElecCalibFlat/OFC",className = 'CondAttrListCollection')

                    from AthenaCommon.AlgSequence import AthSequencer
                    condSequence = AthSequencer("AthCondSeq")
                    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArOFCFlat_ as LArOFCCondAlg
                    condSequence += LArOFCCondAlg (ReadKey="/LAR/ElecCalibFlat/OFC", WriteKey='LArOFC')
                    from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgOnlineDefault
                    LuminosityCondAlgOnlineDefault()
                else:
                    log.info('Enable HLT calo offset correction for MC')

                # Place the CaloBCIDAvgAlg in HltBeginSeq
                from AthenaCommon.AlgSequence import AlgSequence
                topSequence = AlgSequence()
                from AthenaCommon.CFElements import findSubSequence
                hltBeginSeq = findSubSequence(topSequence, 'HltBeginSeq')
                from CaloRec.CaloBCIDAvgAlgDefault import CaloBCIDAvgAlgDefault
                bcidAvgAlg = CaloBCIDAvgAlgDefault(sequence=hltBeginSeq)

                if not bcidAvgAlg:
                    log.info('Cannot use timer for CaloBCIDAvgAlg')
                else:
                    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
                    monTool = GenericMonitoringTool('MonTool')
                    monTool.defineHistogram('TIME_exec', path='EXPERT', type='TH1F', title="CaloBCIDAvgAlg execution time; time [ us ] ; Nruns", xbins=80, xmin=0.0, xmax=4000)
                    bcidAvgAlg.MonTool = monTool
                    log.info('using timer for CaloBCIDAvgAlg')


            else:
                log.info('Disable HLT calo offset correction')



        return
