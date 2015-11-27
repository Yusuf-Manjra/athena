# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
logSignatureDict = logging.getLogger("TriggerMenu.menu.SignatureDicts")
from copy import deepcopy

#==========================================================
##This is stored in chainDict['Signature']
#==========================================================
SliceIDDict = {
    'Electron': 'e',
    'Jet'     : 'j',
    'HT'      : 'ht',
    'Photon'  : 'g',
    'Muon'    : 'mu',
    'Tau'     : 'tau',
    'MET'     : 'xe',
    'XS'      : 'xs',
    'TE'      : 'te',
    'MinBias' : 'mb',
    'HeavyIon' : 'hi',
    'Cosmic'  : 'cosmic',
    'Calibration'   : 'calib',
    'Streaming'     : 'streamer',
    'Monitoring'    : 'mon',
    'Beamspot'      : 'beamspot',
    'EnhancedBias'  : 'eb',
    'Test'          : 'TestChain',
}

#==========================================================
# ---- Generic Template for all chains          ----
# ---- chainParts specific information given in ----
# ---- signature specific dictionaries below    ----
#==========================================================
ChainDictTemplate = {
    'chainName'    : '',
    'L1item'        : '',
    'topo'          : '',
    'signatures'    : '',
    'stream'        : '',
    'groups'        : [],
    'EBstep'        : '',
    'chainParts'   : [],
    'topoStartFrom' : False,
}



#==========================================================
# ----- Allowed HLT Topo Keywords (in addition to generic topos like DR, DETA, DPHI...)
#==========================================================
AllowedTopos_e = ["Jpsiee","Zeg","Zee"]
AllowedTopos_mu = ['Jpsimumu']
AllowedTopos_xe = ['1dphi10', '2dphi05', '6dphi05', '6dphi15', 'mt25', 'mt35', 'razor140', 'razor170', 'razor200','razor100','razor185','razor195']
AllowedTopos_bphys = ['bJpsi', 'bTau', 'bDimu', 
                      'bJpsimumu', 'bUpsimumu', 
                      'bBmumu', 'bBmumux', 
                      'bBmumuxv2', 'bBmumuxv3',
                      '02dr-2mu6', '2invm', 
                      'BcmumuDs' ,   'BcmumuDsloose' ,
                      '7invm9', 'noos', 'noid', 'novtx',
                      '11invm60','18invm60', 'bUpsi',
                      'Trkloose', 'Zmumu', 'noL2', 'noEFbph',
                      'noinvm', 'ss', 'BpmumuKp',
                      '11invm24', '24invm60',
                      'tightChi2', 'Lxy0']
AllowedTopos_jet = ['muvtx',
                    'llp',
		    'noiso',
                    'deta2',
                    'deta25',
                    'invm250',
                    'invm400',
                    'invm600',
                    'invm700',
                    'invm800',
                    'invm1000']
AllowedTopos_comb = ['taumass', 'dr05', 'dz02']

#AllowedTopos = AllowedTopos_e+AllowedTopos_mu+AllowedTopos_bphys+AllowedTopos_jet+AllowedTopos_xe+AllowedTopos_comb

#NOTE: removed jets from list, special case for VBF triggers
AllowedTopos = AllowedTopos_e + AllowedTopos_mu + AllowedTopos_bphys + AllowedTopos_xe + AllowedTopos_comb


#==========================================================
# Jet
#==========================================================
# ---- Jet Dictinary of all allowed Values ----
JetChainParts = {
    'signature'    : ['Jet'],
    'L1item'       : '',
    'chainPartName': '',
    'threshold'    : '',
    'multiplicity' : '',
    'etaRange'     : ['0eta490','0eta320','0eta240', '240eta490','280eta320', '320eta490'],
    'trigType'     : ['j'],
    'extra'        : ['noL1','test1','test2','test3'],
    'cleaning'     : ['cleanL','cleanT','cleanL','cleanLLP',
                      'cleanLA','cleanTA','cleanLA','cleanLLPA', 'noCleaning'
                      ],
    'recoAlg'      : ["a3","a4", "a10", "a10r"],
    'dataType'     : ['TT', 'tc', 'cc', 'ion'],
    'calib'        : ["had","lcw","em"],
    'jetCalib'     : ["jes","sub","subjes", "nojcalib"],
    'scan'         : ['FS','PS'],
    'addInfo'      : ['perf'],    
    'topo'         : AllowedTopos_jet,
    'bTag'         : ['bloose', 'bmedium', 'btight', 'bperf','boffperf', 'bmv2c2060', 'bmv2c2070', 'bmv2c2077', 'bmv2c2085'],
    'bTracking'    : ['EFID'],
    'bConfig'      : ['split', 'hybrid', 'singlepass'],
#    'bMatching'    : ['mu4antidr05', 'mu6antidr05'],
    'bMatching'    : ['antimatchdr05mu'],
    'dataScouting' : ['ds1', 'ds2'],
}
# ---- Jet Dictinary of default Values ----
JetChainParts_Default = {
    'signature'    : ['Jet'],
    'L1item'       : '',
    'threshold'    : '',
    'multiplicity' : '',
    'etaRange'     : '0eta320',
    'trigType'     :'j',
    'extra'        : '',
    'cleaning'     : 'noCleaning',
    'recoAlg'      :'a4',
    'dataType'     :'tc',
    'calib'        :'em',
    'jetCalib'     :'subjes',
    'scan'         :'FS',
    'addInfo'      : [],    
    'topo'         : [],  
    'bTag'         : '',
    'bTracking'    : '',
    'bConfig'      : [],
    'bMatching'    : [],
    'dataScouting' : '',
    }

#==========================================================
# HT chains
#==========================================================
# ---- HT Dictinary of all allowed Values ----
HTChainParts = deepcopy(JetChainParts)
HTChainParts['signature']    = ['HT']
HTChainParts['trigType']     = ['ht']
HTChainParts['extra']     = ['j20', 'j25', 'j30',]
# ---- HTDictinary of default Values ----
HTChainParts_Default = deepcopy(JetChainParts_Default)
HTChainParts_Default['signature']    = ['HT']
HTChainParts_Default['trigType']     = 'ht'
HTChainParts_Default['extra']     = ''

#==========================================================
# Muon 
#==========================================================
# ---- Muon Dictinary of all allowed Values ----
MuonChainParts = {
    'signature'      : ['Muon'],
    'L1item'         : '',
    'chainPartName'  : '',
    'multiplicity'   : '',    
    'trigType'       : ['mu'],
    'etaRange'       : ['0eta105', '0eta250'],
    'threshold'      : '',
    'extra'          : ['noL1'],
    'IDinfo'         : [],
    'isoInfo'        : ['iloose', 'imedium', 'itight',],
    'reccalibInfo'   : ['msonly', 'l2msonly', 'idperf','muoncalib', 'mucombTag','muL2'],
    'trkInfo'        : ['fasttr', 'hlttr', 'ftk', 'IDT'],
    'hypoInfo'       : [],
    'FSinfo'         : ['ftkFS', 'nscan03', 'l2nscan03', 'nscan05', 'l2nscan05', 'JpsimumuFS', 'JpsimumuL2'],
    'L2IDAlg'        : ['L2Star','IdScan','FTK'],
    'L2SAAlg'        : ['muFast', 'l2muonSA',],
    'L2CBAlg'        : ['muComb',],
    'EFAlg'          : ['SuperEF'],
    'addInfo'        : ['cosmicEF', 'cosmic','IdTest','fsperf', 'ds1', 'ds2','ds3', 'r1extr', 'perf', 'noEF','10invm30','pt2','z10','llns'],
    'overlapRemoval' : ['wOvlpRm', 'noOvlpRm', 'noMuCombOvlpRm'],
    'topo'           : AllowedTopos_mu,
    }
# ---- MuonDictinary of default Values ----
MuonChainParts_Default = {
    'signature'      : ['Muon'],
    'L1item'         : '',
    'multiplicity'   : '',    
    'trigType'       : ['mu'],
    'etaRange'       : '0eta250',    
    'threshold'      : '',
    'extra'          : '',    
    'IDinfo'         : '',    
    'isoInfo'        : '',    
    'reccalibInfo'   : '',    
    'trkInfo'        : '',    
    'hypoInfo'       : '',    
    'FSinfo'         : '',
    'L2IDAlg'        : ['L2StarA'],
    'L2SAAlg'        : ['l2muonSA'],
    'L2CBAlg'        : ['muComb'],
    'EFAlg'          : ['SuperEF'],
    'overlapRemoval' : ['wOvlpRm'],
    'addInfo'        : [],
    'topo'           : [],
    }

#==========================================================
# Bphysics
#==========================================================
# ---- Allowed Keywords ----
#AllowedMatchingKeywords = ['antidr']
# ---- Bphysics Dictinary of all allowed Values ----
BphysicsChainParts = deepcopy(MuonChainParts)
BphysicsChainParts['signature'] = ['Bphysics']
BphysicsChainParts['topo'] = AllowedTopos_bphys
# ---- Bphysics Dictinary of default Values ----
BphysicsChainParts_Default = deepcopy(MuonChainParts_Default)
BphysicsChainParts_Default['signature'] = ['Bphysics']
BphysicsChainParts_Default['topo'] = []
BphysicsChainParts_Default['overlapRemoval'] = ['noOvlpRm']

#==========================================================
# Taus
#==========================================================
# ---- Tau Dictionary of all allowed Values ----
TauChainParts = {
    'signature'    : ['Tau'],
    'L1item'       : '',
    'chainPartName': '',
    'threshold'    : '',
    'preselection' : ['r1', 'FTK', 'FTKRefit', 'calo', 'track', 'mvonly', 'ptonly', 'caloonly', 'trackonly', 'tracktwo', 'trackcalo', 'tracktwocalo','tracktwo2015'],
    'selection'    : ['loose1', 'medium1', 'tight1', 'perf', 'r1medium1', 'r1perf', 'cosmic','dikaon', 'dikaontight'],
    'multiplicity' : '',
    'trigType'     : ['tau'],   
    'trkInfo'      : ['idperf'],
    'extra'        : '',
    'recoAlg'      : '',
    'calib'        : '',
    'addInfo'      : ['IdTest'],
}
TauChainParts_Default = {
    'signature'    : ['Tau'],
    'L1item'       : '',
    'chainPartName': '',
    'threshold'    : '20',
    'preselection' : 'tracktwo',
    'selection'    : 'medium1',
    'multiplicity' : '',
    'trigType'     : ['tau'],   
    'trkInfo'      : [],
    'extra'        : '',
    'recoAlg'      : '',
    'calib'        : '',
    'addInfo'      : '',
}

#==========================================================
# MET
#==========================================================
# ---- Met Dictinary of all allowed Values ----
METChainParts = {
    'signature'    : ['MET'],
    'L1item'       : '',
    'chainPartName': '',
    'threshold'    : '',
    'multiplicity' : '',
    'topo'         : AllowedTopos_xe,
    'trigType'     : ['xe'],   
    'extra'        : ['noL1'],
    'calib'        : ['lcw','had','em'],    
    'L2recoAlg'    : ['','l2fsperf','L2FS'],
    'EFrecoAlg'    : ['tc','cell','pueta','mht','pufit'],
    'L2muonCorr'   : ['','wL2MuFEB','wEFMuFEB'],
    'EFmuonCorr'   : ['','wEFMu'],
    'addInfo'      : [],
    }
# ---- MetDictinary of default Values ----
METChainParts_Default = {
    'signature'      : ['MET'],
    'L1item'         : '',
    'trigType'       : ['xe'],
    'threshold'      : '',
    'extra'          : '',
    'calib'          : 'lcw',
    'L2recoAlg'      : '',
    'EFrecoAlg'      : 'cell',
    'L2muonCorr'     : '',
    'EFmuonCorr'     : '',
    'addInfo'        : '',
    }


#==========================================================
# XS
#==========================================================
# ---- xs Dictinary of all allowed Values ----
XSChainParts = METChainParts 
XSChainParts['signature'] = ['XS']
XSChainParts['trigType']  = ['xs'],   

# ---- xs Dictinary of default Values ----
XSChainParts_Default = METChainParts_Default
XSChainParts_Default['signature'] = ['XS']
XSChainParts_Default['trigType']  = ['xs']

#==========================================================
# TE
#==========================================================
# ---- te Dictinary of all allowed Values ----
TEChainParts = METChainParts 
TEChainParts['signature'] = ['TE']
TEChainParts['trigType']  = ['te'],   

# ---- te Dictinary of default Values ----
TEChainParts_Default = METChainParts_Default
TEChainParts_Default['signature'] = ['TE']
TEChainParts_Default['trigType']  = ['te']


#==========================================================
# Electron Chains
#==========================================================
# ---- Electron Dictinary of all allowed Values ----
ElectronChainParts = {
    'signature'      : ['Electron'],
    'chainPartName'  : '',
    'L1item'         : '',
    'extra'          : 'ion',
    'multiplicity'   : '',    
    'trigType'       : ['e'],
    'threshold'      : '',
    'etaRange'       : ['0eta250', '250eta490'],
    'IDinfo'         : ['loose', 'medium', 'tight', 'lhloose', 'lhmedium', 'lhtight', 'loose1', 'medium1', 'tight1', 'vloose', 'lhvloose'],
    'isoInfo'        : [ 'iloose','ivarloose'],
    'trkInfo'        : ['fasttr', 'hlttr', 'IDTrkNoCut','FwdBackTrk','idperf'],
    'caloInfo'       : ['L2EFCalo','HLTCalo'],
    'lhInfo'         : ['cutd0dphideta','nod0','nodphires','nodeta','smooth'],
    'L2IDAlg'        : ['L2StarA','L2StarB','L2StarC','FTK','TRT','SiTrack','IdScan'],
    'addInfo'        : ['etcut','ringer','conv','etisem','gsf','trkcut',
                        'L2Star','perf','IdTest'],
    }
# ---- Egamma Dictinary of default Values ----
ElectronChainParts_Default = {
    'signature'      : ['Electron'],
    'multiplicity'   : '',    
    'L1item'         : '',
    'trigType'       : '',
    'threshold'      : '',
    'etaRange'       : '0eta250',
    'extra'          : '',
    'IDinfoType'     : '',
    'IDinfo'         : '',
    'isoInfo'        : '',
    'reccalibInfo'   : '',
    'trkInfo'        : '',
    'caloInfo'       : '',   
    'lhInfo'         : '',
    'L2IDAlg'        : '',
    'hypoInfo'       : '',
    'recoAlg'        : '',
    'FSinfo'         : '',
    'addInfo'        : [],
    }


#==========================================================
# Photon chains
#==========================================================
# ---- Photon Dictinary of all allowed Values ----
PhotonChainParts = {
    'L1item'         : '',
    'signature'      : ['Photon'],
    'chainPartName'  : '',
    'multiplicity'   : '',    
    'trigType'       : ['g'],
    'threshold'      : '',
    'extra'          : ['i', 'i5', 'i6', 'ns', 'Tvh', 'ion'],
    'IDinfo'         : [ 'loose', 'medium', 'tight', 'NoCut', 'nocut','loose1', 'medium1', 'tight1'],
    'isoInfo'        : '',
    'reccalibInfo'   : 'MSonly',
    'trkInfo'        : ['fasttr', 'hlttr', 'ftk'],
    'caloInfo'       : ['HLTCalo'],
    'hypoInfo'       : '',
    'recoAlg'        : [],
    'FSinfo'         : ['ftkFS',],
    'addInfo'        : ['etcut', 'jetcalibdelayed', 'cosmic', 'perf', 'hiptrt','ringer','conv','larpeb',],
    }

# ---- Photon Dictinary of default Values ----
PhotonChainParts_Default = {
    'signature'      : ['Photon'],
    'L1item'         : '',
    'multiplicity'   : '',    
    'trigType'       : '',
    'threshold'      : '',
    'extra'          : '',
    'IDinfo'         : '',
    'isoInfo'        : '',
    'reccalibInfo'   : '',
    'trkInfo'        : '',
    'caloInfo'       : '',    
    'hypoInfo'       : '',
    'recoAlg'        : '',
    'FSinfo'         : '',
    'addInfo'        : [],
    }

#==========================================================
# Test chains
#==========================================================
# ---- Test Dictinary of all allowed Values ----
TestChainParts = {
    'L1item'         : '',
    'signature'      : ['Test'],
    'chainPartName'  : '',
    'multiplicity'   : '',    
    'trigType'       : ['TestChain'],
    'threshold'      : '',
    'addInfo'        : [''],
    }

# ---- Test Dictionary of default Values ----
TestChainParts_Default = {
    'signature'      : ['Test'],
    'L1item'         : '',
    'multiplicity'   : '',    
    'trigType'       : '',
    'threshold'      : '',
    'addInfo'        : [],
    }




#==========================================================
# Combined Chains
#==========================================================
# ---- Combined Dictinary of all allowed Values ----
CombinedChainParts = deepcopy(PhotonChainParts)
CombinedChainParts['signature'] = ['Photon','Muon']
CombinedChainParts['chainParts'] = ['g','mu'],
CombinedChainParts['topo'] = AllowedTopos_comb
# ---- Combined Dictinary of default Values ----
CombinedChainParts_Default = deepcopy(PhotonChainParts_Default)
CombinedChainParts_Default['signature'] = ['Photon','Muon']
CombinedChainParts_Default['chainParts'] = ['g','mu'],
CombinedChainParts_Default['trigType'] =['g','mu']
CombinedChainParts_Default['topo'] = []

#==========================================================
# MinBias chains
#==========================================================
# ---- MinBias Dictionary of all allowed Values ----
MinBiasChainParts = {
    'signature'      : ['MinBias'],
    'L1item'         : '',
    'chainPartName'  : '',
    'multiplicity'   : '',    
    'trigType'       : ['mb'],
    'threshold'      : '',
    'extra'          : ['noisesup', 'vetombts2in', 'ion',],
    'IDinfo'         : [],
    'ZDCinfo'        : ['lg', 'hg'],
    'trkInfo'        : ['hlttr', 'ftk', 'costr'],
    'hypoL2Info'     : ['sp2', 'sp300', 'sp400', 'sp500', 'sp600', 'sp700', 'sp800', 'sp900',
                        'sp1000', 'sp1200', 'sp1300', 'sp1400', 'sp1500', 'sp1600', 'sp1700', 'sp1800', 
                        'sp2000', 'sp2100', 'sp2200', 'sp2400', 'sp2500', 'sp2700', 'sp3000', ],
    'pileupInfo'     : ['pusup350', 'pusup400', 'pusup500', 'pusup550', 'pusup600', 'pusup700', 'pusup750', 'pusup800',
                        'pusup1000', 'pusup1100', 'pusup1200', 'pusup1300', 'pusup1400', 'pusup1500',],
    'hypoEFInfo'     : ['trk10', 'trk40', 'trk45', 'trk50', 'trk55', 'trk60', 'trk65', 'trk70', 'trk75', 'trk80', 'trk90',
                        'trk100', 'trk110', 'trk120', 'trk130', 'trk140', 'trk150', 'trk160', 
                        'pt4', 'pt6', 'pt8', ],
    'hypoEFsumEtInfo': ['sumet40', 'sumet50', 'sumet60', 'sumet70', 'sumet80', 'sumet90', 'sumet110', 'sumet150',],
    'recoAlg'        : ['mbts', 'sptrk', 'sp', 'noalg', 'perf', 'hmt', 'hmtperf', 'idperf', 'zdcperf'],
    'addInfo'        : ['peb'],
    }
# ---- MinBiasDictinary of default Values ----
MinBiasChainParts_Default = {
    'signature'      : ['MinBias'],
    'L1item'         : '',
    'chainPartName'  : '',
    'multiplicity'   : '',    
    'trigType'       : ['mb'],
    'threshold'      : '',
    'extra'          : '',    
    'IDinfo'         : '',
    'ZDCinfo'        : '',    
    'trkInfo'        : '',    
    'hypoL2Info'       : '',
    'pileupInfo'       : '',
    'hypoEFInfo'       : '',
    'hypoEFsumEtInfo': '',    
    'recoAlg'        : [],
    'addInfo'        : [],
    }

#==========================================================
# HeavyIon chains
#==========================================================
# ---- HeavyIon Dictionary of all allowed Values ----
HeavyIonChainParts = {
    'signature'      : ['HeavyIon'],
    'L1item'         : '',
    'chainPartName'  : '',
    'multiplicity'   : '',
    'trigType'       : ['hi'],
    'threshold'      : '',
    'extra'          : ['th1', 'th2', 'th3', 'th4', 'th5', 'th6', 'th7', 'th8', 'th9', 'th10', 'th11', 'th12', 'th13', 'th14', 'th15', 'th16', 'perf', 'perfzdc'],
    'IDinfo'         : [],
    'trkInfo'        : [],
    'eventShape'     : ['v2', 'v3', 'v23'],
    'eventShapeVeto' : ['veto2', 'veto3'],
    'hypoL2Info'     : ['loose', 'medium', 'tight', 'gg',],
    'pileupInfo'     : [],
    'hypoEFInfo'     : [],
    'hypoEFsumEtInfo': ['fcalet3000', 'fcalet3306', 'fcalet3391', 'fcalet3516',],
    'recoAlg'        : ['ucc', 'upc'],
    'addInfo'        : [ ],
    }

# ---- HeavyIonDictinary of default Values ----
HeavyIonChainParts_Default = { 
    'signature'      : ['HeavyIon'],
    'L1item'         : '',
    'chainPartName'  : '',
    'multiplicity'   : '',
    'trigType'       : ['hi'],
    'threshold'      : '',
    'extra'          : '',
    'IDinfo'         : '',
    'trkInfo'        : '',
    'eventShape'     : '',
    'eventShapeVeto' : '',
    'hypoL2Info'       : '',
    'pileupInfo'       : '',
    'hypoEFInfo'       : '',
    'hypoEFsumEtInfo': '',    
    'recoAlg'        : [],
    'addInfo'        : [],
    }


#==========================================================
# ---- CosmicDef chains -----
#==========================================================
AllowedCosmicChainIdentifiers = ['larps',
                                 'larhec',
                                 'tilecalib', 
                                 #'pixel', 
                                 'sct', 
                                 'id',]

# ---- Cosmic Chain Dictinary of all allowed Values ----
CosmicChainParts = {
    'signature'      : ['Cosmic'],
    'chainPartName'  : '',
    'L1item'       : '',
    'purpose'        : AllowedCosmicChainIdentifiers,
    'addInfo'        : ['cosmicid','noise', 'beam', 'laser', 'AllTE', 'central', 'ds'], #'trtd0cut'
    'trackingAlg'    : ['idscan', 'sitrack', 'trtxk'],
    'hits'           : ['4hits'],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : 'cosmic', 
    'extra'          : '',
    }

# ---- Cosmic Chain Default Dictinary of all allowed Values ----
CosmicChainParts_Default = {
    'signature'      : ['Cosmic'],
    'chainPartName'  : '',
    'L1item'       : '',
    'purpose'        : [],
    'addInfo'        : [],
    'trackingAlg'    : [],
    'hits'           : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : '', 
    'extra'          : '',

    }

#==========================================================
# ---- StreamingDef chains -----
#==========================================================
AllowedStreamingChainIdentifiers = ['noalg']

# ---- Streaming Chain Dictinary of all allowed Values ----
StreamingChainParts = {
    'signature'      : ['Streaming'],
    'chainPartName'  : '',
    'L1item'       : '',
    'threshold'      : '',
    'multiplicity'   : '',
    'streamingInfo'  : ['bkg', 'idmon', 'mb', 'eb', 'zb','to','standby',
                        'hltpassthrough', 'jettauetmiss', 'larcells', 
                        'cosmiccalo', 'cosmicmuons','idcosmic', 'dcmmon','zb', 'l1calo'],
    'trigType'       : 'streamer', 
    'extra'          : '',
    'streamType'        : AllowedStreamingChainIdentifiers,
    'algo' : ['NoAlg']
    }

# ---- Cosmic Chain Default Dictinary of all allowed Values ----
StreamingChainParts_Default = {
    'signature'      : ['Streaming'],
    'chainPartName'  : '',
    'L1item'       : '',
    'threshold'      : '',
    'multiplicity'   : '',
    'streamingInfo'  : 'hltpassthrough',
    'trigType'       : '', 
    'extra'          : '',
    'streamType'     : '',
    'algo' : [],
    }
#==========================================================
# ---- CalibDef chains -----
#==========================================================
AllowedCalibChainIdentifiers = ['csccalib',
                                'larcalib', 
                                'idcalib', 
                                'l1calocalib', 
                                'tilelarcalib',
                                'alfacalib',
                                'larnoiseburst',
                                'ibllumi', 
                                'lumipeb',
                                #'vdm',
                                'lhcfpeb',
                                'alfaidpeb',
                                'larpebj',
                                'l1satmon',
                                'zdcpeb',
                                ]

# ---- Calib Chain Dictinary of all allowed Values ----
##stramingInfo not use in ChainConfiguration, only to distinguish streaming

CalibChainParts = {
    'signature'      : ['Calibration'],
    'chainPartName'  : '',
    'L1item'         : '',
    'purpose'        : AllowedCalibChainIdentifiers,
    'location'       : ['central', 'fwd'],
    'calibType'      : [],
    'addInfo'        : ['loose','noise','beam'],
    'hypo'           : ['trk9', 'trk16', 'trk29', 'conej40', 'conej165', 'conej75_320eta490', 'conej140_320eta490','satu20em'],
    'hits'           : [],
    'streamingInfo'  : ['vdm',],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : ['trk'], 
    'extra'          : ['rerun',''],
    }

# ---- Calib Chain Default Dictinary of all allowed Values ----
CalibChainParts_Default = {
    'signature'      : ['Calibration'],
    'chainPartName'  : '',
    'L1item'       : '',
    'purpose'        : [],
    'calibType'      : [],
    'addInfo'        : [],
    'hypo'           : '',
    'hits'           : [],
    'streamingInfo'  : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'location'   : '',
    'trigType'       : '', 
    'extra'          : '',

    }
#==========================================================
# ---- MonitorDef chains -----
#==========================================================
AllowedMonitorChainIdentifiers = ['robrequest', 'timeburner', 'idmon', 'costmonitor','cscmon','l1calooverflow']

# ---- Monitor Chain Dictinary of all allowed Values ----
MonitorChainParts = {
    'signature'      : ['Monitoring'],
    'chainPartName'  : '',
    'L1item'       : '',
    'monType'        : AllowedMonitorChainIdentifiers,
    'location'       : [],
    'calibType'      : [],
    'addInfo'        : [],
    'hypo'           : ['trkFS',],
    'hits'           : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : 'mon',
    'extra'          : '',
    }

# ---- Monitor Chain Default Dictinary of all allowed Values ----
MonitorChainParts_Default = {
    'signature'      : ['Monitoring'],
    'chainPartName'  : '',
    'L1item'       : '',
    'monType'        : [],
    'calibType'      : [],
    'addInfo'        : [],
    'hypo'           : '',
    'hits'           : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'location'   : '',
    'trigType'       : '', 
    'extra'          : '',

    }

#==========================================================
# ---- EB chains -----
#==========================================================
AllowedEBChainIdentifiers = ['eb']

# ---- Enhanced Bias Chain Dictinary of all allowed Values ----
EnhancedBiasChainParts = {
    'signature'      : ['EnhancedBias'],
    'chainPartName'  : '',
    'L1item'       : '',
    'algType'        : ['high','firstempty','empty','unpairediso','unpairednoniso', 'low'],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : '',
    'extra'          : '',
    }

# ---- EnhancedBias Chain Default Dictinary of all allowed Values ----
EnhancedBiasChainParts_Default = {
    'signature'      : ['EnhancedBias'],
    'chainPartName'  : '',
    'L1item'         : '',
    'algType'        : 'physics',
    'threshold'      : '',
    'multiplicity'   : '',
    'location'   : '',
    'trigType'       : '', 
    'extra'          : '',

    }

#==========================================================
# ---- BeamspotDef chains -----
#==========================================================
AllowedBeamspotChainIdentifiers = ['beamspot',]
BeamspotChainParts = {
    'signature'      : ['Beamspot'],
    'chainPartName'  : '',
    'L1item'       : '',
    'monType'        : AllowedBeamspotChainIdentifiers,
    'location'       : ['vtx'],
    'addInfo'        : ['trkFS', 'allTE', 'activeTE'],
    'hypo'           : [],
    'l2IDAlg'        : ['L2StarB','trkfast'],
    'threshold'      : '',
    'multiplicity'   : '',
    'trigType'       : 'beamspot',
    'extra'          : '',
    'eventBuildType' : ['peb','pebTRT'],
    }

# ---- Beamspot Chain Default Dictinary of all allowed Values ----
BeamspotChainParts_Default = {
    'signature'      : ['Beamspot'],
    'chainPartName'  : '',
    'L1item'       : '',
    'monType'        : [],
    'addInfo'        : [],
    'hypo'           : [],
    'l2IDAlg'        : [],
    'threshold'      : '',
    'multiplicity'   : '',
    'location'   : 'vtx',
    'trigType'       : 'beamspot', 
    'extra'          : '',
    'eventBuildType' : '',
    }

#==========================================================
#==========================================================
def getSignatureNameFromToken(chainpart):
    theMatchingTokens = []
    reverseSliceIDDict = dict([(value, key) for key, value in SliceIDDict.iteritems()]) #reversed SliceIDDict
    for sig,token in SliceIDDict.items():
            if (token in chainpart):
                theMatchingTokens += [token]
    theToken = max(theMatchingTokens, key=len) # gets the longest string in t
    if len(theMatchingTokens)>0:
        if len(theMatchingTokens)>1:
            logSignatureDict.info('There are several signatures tokens, %s, matching this chain part %s. Picked %s.' % (theMatchingTokens,chainpart,theToken))            
        return reverseSliceIDDict[theToken]
    logSignatureDict.error('No signature matching chain part %s was found.' % (chainpart))
    return False


            
def getSignatureInformation(signature):
    if signature == 'Electron':
        return [ElectronChainParts_Default, ElectronChainParts]
    if signature == 'Photon':
        return [PhotonChainParts_Default, PhotonChainParts]
    if signature == "Jet":
        return [JetChainParts_Default, JetChainParts]
#    if signature == "Bjet":
#        return [BjetChainParts_Default, BjetChainParts]
    if signature == "HT":
        return [HTChainParts_Default, HTChainParts]
    if signature == "Tau":
        return [TauChainParts_Default, TauChainParts] 
    if (signature == "Muon"):
        return [MuonChainParts_Default, MuonChainParts]
    if  (signature == "Bphysics"):
        return [BphysicsChainParts_Default, BphysicsChainParts]
    if  (signature == "Combined"):
        return [CombinedChainParts_Default, CombinedChainParts]
    if signature == "MET":
        return [METChainParts_Default, METChainParts]
    if signature == "XS":
        return [XSChainParts_Default, XSChainParts]
    if signature == "TE":
        return [TEChainParts_Default, TEChainParts]
    if signature == "MinBias":
        return [MinBiasChainParts_Default, MinBiasChainParts]
    if signature == "HeavyIon":
        return [HeavyIonChainParts_Default, HeavyIonChainParts]
    if signature == "Cosmic":
        return [CosmicChainParts_Default, CosmicChainParts]
    if signature == "Calibration":
        return [CalibChainParts_Default, CalibChainParts]
    if signature == "Streaming":
        return [StreamingChainParts_Default, StreamingChainParts]
    if signature == "Monitoring":
        return [MonitorChainParts_Default, MonitorChainParts]
    if signature == "Beamspot":
        return [BeamspotChainParts_Default, BeamspotChainParts]
    if signature == "EnhancedBias":
        return [EnhancedBiasChainParts_Default, EnhancedBiasChainParts]
    if signature == "Test":
        return [TestChainParts_Default, TestChainParts]
    else:
        raise RuntimeError("ERROR Cannot find corresponding dictionary")


def getBasePattern():
    import re
    import itertools
    # possibleTT = '|'.join(allowedSignaturePropertiesAndValues['trigType'])
    #print 'SignatureDicts.py: Allowed values for triType in base pattern', SliceIDDict.values()
    allTrigTypes = SliceIDDict.values()
    possibleTT = '|'.join(allTrigTypes)
    pattern = re.compile("(?P<multiplicity>\d*)(?P<trigType>(%s))(?P<threshold>\d+)(?P<extra>\w*)" % (possibleTT))
    return pattern



