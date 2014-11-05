# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigValAlgs.TrigValAlgsConf import TrigCountDumper
from TrigValAlgs.TrigValAlgsConf import TrigDecisionChecker
from TrigValAlgs.TrigValAlgsConf import TrigEDMChecker
from TrigValAlgs.TrigValAlgsConf import TrigSlimValAlg

# TrigCountDumper configurable
# Run with TrigCountDumper configured from AOD header
class TrigCountDumper ( TrigCountDumper ):
  __slots__ = []
  def __init__(self, name="TrigCountDumper"):
    super( TrigCountDumper, self ).__init__( name )
    from AthenaCommon.Logging import logging  # loads logger
    log = logging.getLogger( name )
  
  def setDefaults(self, handle):
    WriteEventDecision=False
    MonitoredChains = [ ]
    MonitoringBlock = 100

# TrigDecisionChecker configurable
# Run with TrigDecisionTool configured from AOD header
class TrigDecisionChecker ( TrigDecisionChecker ):
    __slots__ = []
    def __init__(self, name="TrigDecisionChecker"):
        super( TrigDecisionChecker, self ).__init__( name )

        from AthenaCommon.Logging import logging  # loads logger
        log = logging.getLogger( name )

        #print "TrigDecisionChecker.py : adding TrigDecisionTool"
        #from TrigDecision.TrigDecisionConfig import TrigDecisionTool_AOD
        #from AthenaCommon.AppMgr import ToolSvc
        #ToolSvc += TrigDecisionTool_AOD('TrigDecisionTool')


    def setDefaults(self, handle):

        #if not hasattr( handle, 'TrigDecisionTool' ) :
        #    print "TrigDecisionChecker.py : adding TrigDecisionTool"
        #    from TrigDecision.TrigDecisionConfig import TrigDecisionTool
        #    handle.TrigDecisionTool = TrigDecisionTool('TrigDecisionTool')
        WriteEventDecision=False
        MonitoredChains = [ ]
        MonitoringBlock = 100

        self.MuonItems = ['HLT_mu26_imedium','HLT_mu6_idperf']
        self.ElectronItems = ['e0_perf_L1EM15VH','e28_loose1_iloose','e28_lhloose_iloose']
        self.PhotonItems = ['g0_perf_L1EM15VH','g25_loose1','g35_loose1']
        self.MinBiasItems = ['HLT_mb_perf_L1MBTS_1', 'HLT_mb_sptrk', 'HLT_mb_sp2000_pusup600_trk70_hmt']
        self.JetItems = ['HLT_j400','HLT_j460_a10_L1J100','HLT_j200_320eta490','HLT_j60','HLT_j110','HLT_j150','HLT_j200','HLT_j260','HLT_j330','HLT_3j175','HLT_4j100','HLT_5j85','HLT_ht1000_L1J100','HLT_j30_muvtx',] #DC14 jet menu

# Run with TrigDecisionTool configured from XML files
class TrigDecisionChecker_XMLConfig( TrigDecisionChecker ):
    __slots__ = []
    def __init__(self, name="TrigDecChecker"):
        super( TrigDecisionChecker_XMLConfig, self ).__init__( name )

    def setDefaults(self, handle):

        #if not hasattr( handle, 'TrigDecisionTool' ) :
        #    from TrigDecision.TrigDecisionConfig import TrigDecisionTool_XMLConfig
        #    handle.TrigDecisionTool = TrigDecisionTool_XMLConfig('TrigDecisionTool_xml')
        WriteEventDecision=False
        MonitoredChains = [ ]
        MonitoringBlock = 100


# TrigEDMChecker configurable
class TrigEDMChecker ( TrigEDMChecker ):
    __slots__ = []
    def __init__(self, name="TrigEDMChecker"):
        super( TrigEDMChecker, self ).__init__( name )

    def setDefaults(self, handle) :

        doDumpAll = True
        doDumpTrigMissingET = False
        doDumpMuonFeature = False
        doDumpCombinedMuonFeature = False
        doDumpTrigPhotonContainer = False
        doDumpTrigEMCluster = False
        doDumpTrigTauCluster = False
        doDumpTrigMuonEFContainer = False
        doDumpTrigElectronContainer = False
        doDumpTrigTau = False
        doDumpTrigInDetTrackCollection = False
        doDumpTrigVertexCollection = False
        doDumpTrigEFBphysContainer = False
        doDumpTrigL2BphysContainer = False
        doDumpTrigT2Jet = False
        doDumpTrigEFBjetContainer = False
        doDumpTrigL2BjetContainer = False

# TrigSlimValAlg configurable
class TrigSlimValAlg ( TrigSlimValAlg ):
  __slots__ = []
  def __init__(self, name="TrigSlimValAlg"):
    super( TrigSlimValAlg, self ).__init__( name )

  def setDefaults(self, handle):

    self.TrigDecisionTool = "Trig::TrigDecisionTool/TrigDecisionTool"
    self.Navigation = "HLT::Navigation/Navigation"
    self.SlimmingTool = "HLT::TrigNavigationSlimmingTool/TrigNavigationSlimmingTool"


