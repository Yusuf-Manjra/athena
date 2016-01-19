# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#################################################
# EF Electron Hypothesis Algorithm Configuration:
# Ryan Mackenzie White <ryan.white@cern.ch>
#################################################
from AthenaCommon.Logging import logging #AT
import traceback #AT
from TrigEgammaHypo.TrigEgammaHypoConf import TrigEFElectronHypo
from AthenaCommon.SystemOfUnits import GeV

# Include EGammaPIDdefs for loose,medium,tight definitions
from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefElectron
class TrigEFElectronHypoBase (TrigEFElectronHypo):
    __slots__ = []
    def __init__(self, name):
        super( TrigEFElectronHypoBase, self ).__init__( name )

        from TrigEgammaHypo.TrigEFElectronHypoMonitoring import TrigEFElectronHypoValidationMonitoring, TrigEFElectronHypoOnlineMonitoring, TrigEFElectronHypoCosmicMonitoring
        validation = TrigEFElectronHypoValidationMonitoring()
        online     = TrigEFElectronHypoOnlineMonitoring()
        cosmic     = TrigEFElectronHypoCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")

        self.AthenaMonTools = [ time, validation, online, cosmic ]
        
        #Load Tool Service
        from AthenaCommon.AppMgr import ToolSvc
        from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
        ToolSvc += LuminosityToolOnline()
#-----------------------------------------------------------------------
# --- Following classes to be used with TriggerMenu
# --- Rely on TM to configure 
# --- RMWhite 2014
#
# No Cut for EF
class TrigEFElectronHypo_e_NoCut (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( TrigEFElectronHypo_e_NoCut, self ).__init__( name ) 
        self.AcceptAll = True
        self.CaloCutsOnly = False
        self.ApplyIsEM = False
        self.ApplyEtIsEM = False
        self.IsEMrequiredBits = 0X0
        self.emEt = float(threshold)*GeV
#
#-----------------------------------------------------------------------
# --- Et Cut only -- threshold passed from TM chain name
# --- No cut applied at L2
#-----------------------------------------------------------------------
class TrigEFElectronHypo_e_EtCut (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( TrigEFElectronHypo_e_EtCut, self ).__init__( name ) 
        self.AcceptAll = False
        self.CaloCutsOnly = False
        self.ApplyIsEM = False
        self.ApplyEtIsEM = False
        self.IsEMrequiredBits = 0X0
        self.emEt = float(threshold)*GeV

#-----------------------------------------------------------------------
# --- eXX Particle ID selection
# --- loose, medium, tight isEM
# --- mvtloose, mvtmedium, mvttight LH
# --- W T&P supporting trigger
#-----------------------------------------------------------------------

class TrigEFElectronHypo_e_ID (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_ID, self ).__init__( name ) 
# Set the properties        
        self.AcceptAll = False
        self.CaloCutsOnly = False
        self.ApplyIsEM = True
        self.ApplyEtIsEM = False
        self.emEt = float(threshold)*GeV
        self.IsEMrequiredBits = 0X0
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronToolName
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronHypoToolName
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronIsEMBits
        
        # Add the PID tools
        if( 'lh' in IDinfo):
            self.egammaElectronCutIDToolName = "AsgElectronIsEMSelector/AsgElectronIsEMVLooseSelector"
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronToolName[IDinfo]
            self.UseAthenaElectronLHIDSelectorTool = True
        else:
            self.AthenaElectronLHIDSelectorToolName="AsgElectronLikelihoodTool/AsgElectronLHLooseSelector"
            self.IsEMrequiredBits =  ElectronIsEMBits[IDinfo]
            if('1' in IDinfo):
                if( float(threshold) < 20 ):
                    self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronToolName[IDinfo]
                else:
                    self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronHypoToolName[IDinfo]
            else:
                self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronToolName[IDinfo]


class TrigEFElectronHypo_e_ID_HI (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_ID_HI, self ).__init__( name ) 
        # Set the properties        
        self.AcceptAll = False
        self.CaloCutsOnly = False
        self.ApplyIsEM = True
        self.ApplyEtIsEM = False
        self.emEt = float(threshold)*GeV
        self.IsEMrequiredBits = 0X0
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronToolName
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronHypoToolName
        from TrigEgammaHypo.TrigEGammaPIDdefsHI import ElectronIsEMBits
        
        # Add the PID tools
        if( 'lh' in IDinfo):
            self.egammaElectronCutIDToolName = "AsgElectronIsEMSelector/AsgElectronIsEMVLooseSelector"
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronToolName[IDinfo]
            self.UseAthenaElectronLHIDSelectorTool = True
        else:
            self.AthenaElectronLHIDSelectorToolName="AsgElectronLikelihoodTool/AsgElectronLHLooseSelector"
            self.IsEMrequiredBits =  ElectronIsEMBits[IDinfo]
            if('1' in IDinfo):
                if( float(threshold) < 20 ):
                    self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronToolName[IDinfo]
                else:
                    self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronHypoToolName[IDinfo]
            else:
                self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronToolName[IDinfo]

 
# Likelihood only chains for alignment studies 
class TrigEFElectronHypo_e_LH (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo, lhInfo):
        super( TrigEFElectronHypo_e_LH, self ).__init__( name ) 
        # Set the properties        
        self.AcceptAll = False
        self.CaloCutsOnly = False
        self.ApplyIsEM = True
        self.ApplyEtIsEM = False
        self.emEt = float(threshold)*GeV
        self.IsEMrequiredBits = 0X0
       
        
        # Add the PID tools
        self.egammaElectronCutIDToolName = "AsgElectronIsEMSelector/AsgElectronIsEMVLooseSelector"
     
        self.UseAthenaElectronLHIDSelectorTool = True
        if(IDinfo == 'lhvloose'):
            from TrigEgammaHypo.TrigEgammaPidTools import ElectronLHVLooseToolName
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronLHVLooseToolName[lhInfo]
        elif(IDinfo == 'lhloose'):
            from TrigEgammaHypo.TrigEgammaPidTools import ElectronLHLooseToolName
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronLHLooseToolName[lhInfo]
        elif(IDinfo == 'lhmedium'):
            from TrigEgammaHypo.TrigEgammaPidTools import ElectronLHMediumToolName
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronLHMediumToolName[lhInfo]
        elif(IDinfo == 'lhtight'):
            from TrigEgammaHypo.TrigEgammaPidTools import ElectronLHTightToolName
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronLHTightToolName[lhInfo]

# --- eXX LH chains for alignment with isolation 
class TrigEFElectronHypo_e_LH_Iso (TrigEFElectronHypo_e_LH):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo, lhInfo, isoInfo):
        super( TrigEFElectronHypo_e_LH_Iso, self ).__init__( name, threshold, IDinfo, lhInfo ) 
# Set the properties        
        self.CaloCutsOnly = False
        self.ApplyEtIsEM = True
        #Isolation
        self.ApplyIsolation = True
        self.useClusETforCaloIso = True
        self.useClusETforTrackIso = True
        #EtCone Size              =  20, 30, 40
        self.EtConeSizes = 3
        self.RelEtConeCut       = [-1, -1, -1]
        self.EtConeCut          = [-1, -1, -1]
        #PtCone Size              =  20, 30, 40
        self.PtConeSizes = 6
        if 'ivarloose' in isoInfo:
            self.RelPtConeCut       = [-1, -1, -1,0.100,-1,-1]
        else:
            self.RelPtConeCut       = [0.100, -1, -1,-1,-1,-1]
        self.PtConeCut          = [-1, -1, -1,-1,-1,-1]

# --- W T&P supporting trigger
#-----------------------------------------------------------------------
class TrigEFElectronHypo_e_WTP (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold):
        super( TrigEFElectronHypo_e_WTP, self ).__init__( name ) 
        self.AcceptAll = False
        self.CaloCutsOnly = False
        self.ApplyIsEM = True
        self.ApplyEtIsEM = False
        self.emEt = float(threshold)*GeV
        self.IsEMrequiredBits = 0X0
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronToolName
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronHypoToolName
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronIsEMBits
        from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefElectron
        self.AthenaElectronLHIDSelectorToolName="AsgElectronLikelihoodTool/AsgElectronLHLooseSelector"
        self.IsEMrequiredBits =  SelectionDefElectron.Electron_trk
        self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronToolName["loose"]

#-----------------------------------------------------------------------
# --- eXX Particle ID selection CaloCuts only
# --- loose, medium, tight isEM
# --- Currently Not for likelihood, but derives from e_ID class 
#-----------------------------------------------------------------------
# Dictionary for mapping PID to toolname
class TrigEFElectronHypo_e_ID_CaloOnly (TrigEFElectronHypo_e_ID):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_ID_CaloOnly, self ).__init__( name, threshold, IDinfo ) 
        
        # Set the properties        
        self.CaloCutsOnly = True
        self.IsEMrequiredBits = 0X0
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronCaloToolName
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronCaloHypoToolName
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronIsEMBits
        
        if( 'lh' in IDinfo):
            self.egammaElectronCutIDToolName = "AsgElectronIsEMSelector/AsgElectronIsEMVLooseCaloSelector"
            self.UseAthenaElectronLHIDSelectorTool = True
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronCaloToolName[IDinfo]
        else:
            self.AthenaElectronLHIDSelectorToolName="AsgElectronLikelihoodTool/AsgElectronLHLooseSelector"
            self.IsEMrequiredBits =  ElectronIsEMBits[IDinfo]
            if('1' in IDinfo):
                if( float(threshold) < 20 ):
                    self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronCaloToolName[IDinfo]
                else:
                    self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronCaloHypoToolName[IDinfo]
            else:
                self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronCaloToolName[IDinfo]

#-----------------------------------------------------------------------
# --- eXX IsEM Selection uses the Et of the object
class TrigEFElectronHypo_e_ID_EtIsEM (TrigEFElectronHypoBase):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_ID_EtIsEM, self ).__init__( name ) 
# Set the properties        
        self.AcceptAll = False
        self.CaloCutsOnly = False
        self.ApplyIsEM = True
        self.ApplyEtIsEM = True
        self.emEt = float(threshold)*GeV
        self.IsEMrequiredBits = 0X0
        
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronToolName
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronIsEMBits
        
        # Add the PID tools
        if( 'lh' in IDinfo):
            self.egammaElectronCutIDToolName = "AsgElectronIsEMSelector/AsgElectronIsEMVLooseSelector"
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronToolName[IDinfo]
            self.UseAthenaElectronLHIDSelectorTool = True
        else:
            self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronToolName[IDinfo]
            self.IsEMrequiredBits = ElectronIsEMBits[IDinfo]
    

#-----------------------------------------------------------------------
# --- eXX IsEM Selection uses the Et of the object and PID for HeavyIon
class TrigEFElectronHypo_e_ID_EtIsEM_HI (TrigEFElectronHypoBase):
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_ID_EtIsEM_HI, self ).__init__( name ) 

        self.AcceptAll = False
        self.CaloCutsOnly = False
        self.ApplyIsEM = True
        self.ApplyEtIsEM = True
        self.emEt = float(threshold)*GeV
        self.IsEMrequiredBits = 0X0
        
        from TrigEgammaHypo.TrigEgammaPidTools import ElectronToolName
        from TrigEgammaHypo.TrigEgammaPIDDefsHI import ElectronISEMBits # imported from different place
        
        # Add the PID tools
        if( 'lh' in IDinfo):
            self.egammaElectronCutIDToolName = "AsgElectronIsEMSelector/AsgElectronIsEMVLooseSelector"
            self.AthenaElectronLHIDSelectorToolName='AsgElectronLikelihoodTool/'+ElectronToolName[IDinfo]
            self.UseAthenaElectronLHIDSelectorTool = True
        else:
            self.egammaElectronCutIDToolName = 'AsgElectronIsEMSelector/'+ElectronToolName[IDinfo]
            self.IsEMrequiredBits = ElectronIsEMBits[IDinfo]
        

# --- eXX IsEM Selection uses the Et of the object
#-----------------------------------------------------------------------
# --- eXX Particle ID and Isolation
# --- derives from e_ID
# --- iloose, imedium
#-----------------------------------------------------------------------
class TrigEFElectronHypo_e_Iso (TrigEFElectronHypo_e_ID):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo, isoInfo):
        super( TrigEFElectronHypo_e_Iso, self ).__init__( name, threshold, IDinfo )
        #Isolation
        self.ApplyIsolation = True
        self.useClusETforCaloIso = True
        self.useClusETforTrackIso = True
        #EtCone Size              =  20, 30, 40
        self.EtConeSizes = 3
        self.RelEtConeCut       = [-1, -1, -1]
        self.EtConeCut          = [-1, -1, -1]
        #PtCone Size              =  20, 30, 40
        self.PtConeSizes = 6
        if 'ivarloose' in isoInfo:
            self.RelPtConeCut       = [-1, -1, -1,0.100,-1,-1]
        else:
            self.RelPtConeCut       = [0.100, -1, -1,-1,-1,-1]
        self.PtConeCut          = [-1, -1, -1,-1,-1,-1]

class TrigEFElectronHypo_e_ID_EtIsEM_Iso (TrigEFElectronHypo_e_ID_EtIsEM):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_ID_EtIsEM_Iso, self ).__init__( name, threshold, IDinfo )
        #Isolation
        self.ApplyIsolation = True
        self.useClusETforCaloIso = True
        self.useClusETforTrackIso = True
        #EtCone Size              =  15, 20, 25, 30, 35, 40
        self.EtConeSizes = 3
        self.RelEtConeCut       = [-1, -1, -1]
        self.EtConeCut          = [-1, -1, -1]
        #PtCone Size              =  20, 30, 40
        self.PtConeSizes = 6
        self.RelPtConeCut       = [0.100, -1, -1,-1,-1,-1]
        self.PtConeCut          = [-1, -1, -1,-1,-1,-1]
#-----------------------------------------------------------------------
# --- eXX Particle ID and Isolation performance chains
# --- derives from e_ID
# --- iloose, imedium
# --- run selection but accept all 
#-----------------------------------------------------------------------
class TrigEFElectronHypo_e_ID_perf (TrigEFElectronHypo_e_ID):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo):
        super( TrigEFElectronHypo_e_IsD_perf, self ).__init__( name, threshold, IDinfo )
        self.AcceptAll = True

class TrigEFElectronHypo_e_Iso_perf (TrigEFElectronHypo_e_ID):
    __slots__ = []
    def __init__(self, name, threshold, IDinfo, isoInfo):
        super( TrigEFElectronHypo_e_Iso_perf, self ).__init__( name, threshold, IDinfo )
        #Isolation
        self.AcceptAll = True
        self.ApplyIsolation = True
        self.useClusETforCaloIso = True
        self.useClusETforTrackIso = True
        #EtCone Size              =  15, 20, 25, 30, 35, 40
        self.EtConeSizes = 6
        self.RelEtConeCut       = [-1, -1, -1, -1, -1, -1]
        self.EtConeCut          = [-1, -1, -1, -1, -1, -1]
        #PtCone Size              =  20, 30, 40
        self.PtConeSizes = 3
        self.RelPtConeCut       = [0.100, -1, -1]
        self.PtConeCut          = [-1, -1, -1]

