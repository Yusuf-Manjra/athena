# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigHLTMonitoring.HLTMonTriggerList import HLTMonTriggerList
hltmonList = HLTMonTriggerList()

from AthenaConfiguration.AllConfigFlags import ConfigFlags

# default configuration for pp
#if hltmonList.pp_mode :
containers = [
              "HLT_xAOD__TrigBphysContainer_L2BMuMuFex",  "HLT_xAOD__TrigBphysContainer_EFBMuMuFex",
              #"HLT_xAOD__TrigBphysContainer_L2BMuMuXFex", 
              "HLT_xAOD__TrigBphysContainer_EFBMuMuXFex",
              #"HLT_xAOD__TrigBphysContainer_L2MultiMuFex",
              "HLT_xAOD__TrigBphysContainer_EFMultiMuFex",
              #"HLT_xAOD__TrigBphysContainer_L2TrackMass",
              "HLT_xAOD__TrigBphysContainer_MultiTrkFex",
              "HLT_xAOD__TrigBphysContainer_EFTrackMass"
              ]
if ConfigFlags.Trigger.EDMVersion == 3 :
  # will add the MT version of TrigBphys containers once they are available in EDM
  containers = [ "HLT_DimuEF" ]
  
DetailedChains = {
                  "BMuMu"  : 'HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(bDimu|bJpsimumu)(_L1[0-9]?MU[0-9]+)?',
                  "BMuMuX" : 'HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_bBmumuxv[23](_L1[0-9]?MU[0-9]+)?',
                  "MultiMu": 'HLT_((mu[0-9]+_[2-9]mu[0-9]+)|(3mu[0-9]+))_(b.*)(_L1[0-9]?MU[0-9]+)?',
                  "MuTrack": 'HLT_(mu[0-9]+)_(b.*)_Trk(.*)(_L1[0-9]?MU[0-9]+)?',
                  "Tau3mu" : 'HLT_([2-3]?mu[0-9]+).*_bTau.*(_L1[0-9]?MU[0-9]+)?',
                 }
DetailedL1TopoChains = {
                      "L1BPH-M-DR-low"    : "HLT_2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6",
                      "L1BPH-M-DR"        : "HLT_2mu6_bUpsimumu_L1BPH-8M15-2MU6_BPH-0DR22-2MU6",
                      #"L1BPH-M"           : "HLT_2mu6_bDimu_L1BPH-8M15-2MU6"
                       }
DetailedIndividualChains = { 
                  # filled not from primary_bphys but from monitored_bphys
                  "HLT_2mu4_bJpsimumu" : "HLT_2mu4_bJpsimumu(_L1(?!BPH).*)?$",
                  "HLT_2mu4_bUpsimumu" : "HLT_2mu4_bUpsimumu(_L1(?!BPH).*)?$",
                  "HLT_2mu4_bDimu"     : "HLT_2mu4_bDimu(_L1(?!BPH).*)?$",
                  "HLT_mu6_mu4_bJpsimumu" : "HLT_mu6_mu4_bJpsimumu(_L1(?!BPH).*)?$",
                  "HLT_mu6_mu4_bUpsimumu" : "HLT_mu6_mu4_bUpsimumu(_L1(?!BPH).*)?$",
                  "HLT_mu6_mu4_bDimu"     : "HLT_mu6_mu4_bDimu(_L1(?!BPH).*)?$",
                  "HLT_2mu6_bJpsimumu" : "HLT_2mu6_bJpsimumu(_L1(?!BPH).*)?$",
                  "HLT_2mu6_bUpsimumu" : "HLT_2mu6_bUpsimumu(_L1(?!BPH).*)?$",
                  "HLT_2mu6_bDimu"     : "HLT_2mu6_bDimu(_L1(?!BPH).*)?$",
                  "HLT_mu11_mu6_bJpsimumu" : "HLT_mu11_mu6_bJpsimumu(_L1(?!BPH).*)?$",
                  "HLT_mu11_mu6_bUpsimumu" : "HLT_mu11_mu6_bUpsimumu(_L1(?!BPH).*)?$",
                  "HLT_mu11_mu6_bDimu"     : "HLT_mu11_mu6_bDimu(_L1(?!BPH).*)?$",
                  "HLT_mu11_mu6_bPhi"     : "HLT_mu11_mu6_bPhi(_L1(?!BPH).*)?$",
                  "HLT_mu11_mu6_bTau"     : "HLT_mu11_mu6_bTau(_L1(?!BPH).*)?$",
                  "HLT_mu11_mu6_bBmumu"     : "HLT_mu11_mu6_bBmumu(_L1(?!BPH).*)?$",
                  "HLT_mu11_mu6_bDimu2700"     : "HLT_mu11_mu6_bDimu2700(_L1(?!BPH).*)?$",
                  "HLT_2mu4_bBmumux_BsmumuPhi"     : "HLT_2mu4_bBmumux_BsmumuPhi(_delayed)?(_L1(?!BPH).*)?$",
                  "HLT_mu6_mu4_bBmumux_BsmumuPhi"     : "HLT_mu6_mu4_bBmumux_BsmumuPhi(_delayed)?(_L1(?!BPH).*)?$",
                  "HLT_2mu4_bBmumux_BpmumuKp"     : "HLT_2mu4_bBmumux_BpmumuKp(_delayed)?(_L1(?!BPH).*)?$",
                  "HLT_mu6_mu4_bBmumux_BpmumuKp"     : "HLT_mu6_mu4_bBmumux_BpmumuKp(_delayed)?(_L1(?!BPH).*)?$",
                 }
EfficiencyChains = {
                    "BMuMu"  : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(bDimu|bJpsimumu)(_L1[0-9]?MU[0-9]+)?",
                    "BMuMuX" : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_bBmumuxv[23](_L1[0-9]?MU[0-9]+)?",
                    #"MultiMu": 'HLT_((mu[0-9]+_[2-9]mu[0-9]+)|(3mu[0-9]+))_(b.*)',
                    #"MuTrack": "HLT_(mu[0-9]+)_(b.*)_Trk(.*)"
                   }
EffTrigDenom_noVtxOS = "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_bDimu_novtx_noos(_L1[0-9]?MU[0-9]+)?"

# configuration for HI and pPb
if hltmonList.HI_mode :
    containers = ["HLT_xAOD__TrigBphysContainer_L2BMuMuFex",  "HLT_xAOD__TrigBphysContainer_EFBMuMuFex"]
    DetailedChains = {"BMuMu"  : 'HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(bDimu|bJpsimumu)'}
    DetailedL1TopoChains = {}
    EfficiencyChains = {"BMuMu"  : "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_(bDimu|bJpsimumu)"}
    EffTrigDenom_noVtxOS = "HLT_(2mu[0-9]+|mu[0-9]+_?mu[0-9]+)_bDimu_novtx_noos"

# change nothing for cosmic
if hltmonList.cosmic_mode :
    pass
  

def TrigBphysMonitoringTool():
    from TrigBphysMonitoring.TrigBphysMonitoringConf import HLTXAODBphysMonTool
    from TrigHLTMonitoring.HLTMonTriggerList import hltmonList

    from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags
    from RecExConfig.RecFlags import rec
    theJpsiCollectionToRead = 'JpsiCandidates' if (HLTMonFlags.doMonTier0 and HLTMonFlags.doBphys and rec.doInDet) else ''

    import logging
    log = logging.getLogger('TrigBphysMonitoringTool')
    log.info('HLTXAODBphysMonTool JpsiCandidates input collection is set to:\'' + theJpsiCollectionToRead + '\'')

    HLTBphysMon = HLTXAODBphysMonTool(name       = 'HLTBphysMon',
                                  #OutputLevel = 1,
                                  JpsiCandidates = theJpsiCollectionToRead,
                                  BphysShifterPath='HLT/BphysMon/shifter',
                                  BphysExpertPath ='HLT/BphysMon/expert',
                                  BphysHistPrefix ='TrigBphys',
                                  monitoring_bphys = hltmonList.monitoring_bphys,
                                  primary_bphys = hltmonList.primary_bphys,
                                  GenerateChainDictsFromDB = True, #if True, the following dictionaries are overwritten
                                  
                                  ContainerList   =containers,
                                  DetailedChains_patterns = DetailedChains,
                                  DetailedL1TopoChains_patterns = DetailedL1TopoChains,
                                  DetailedIndividualChains_patterns = DetailedIndividualChains,
                                  EfficiencyChains_patterns = EfficiencyChains,
                                  EffTrigDenom_noVtxOS_pattern = EffTrigDenom_noVtxOS,
                                  
                                  MW_jpsi_forward_min =  2800,
                                  MW_jpsi_forward_max =  3400,
                                  MW_upsi_forward_min =  8000,
                                  MW_upsi_forward_max =  12000,
                                  MW_jpsi_central_min =  2950,
                                  MW_jpsi_central_max =  3250,
                                  MW_upsi_central_min =  9000,
                                  MW_upsi_central_max =  11500,
                                  Eta_min             =  -3.,
                                  Eta_max             =  3.,
                                  Mu_pt_min           =  0.,
                                  Mu_pt_max           =  50.,
                                  Trk_pt_min          =  0.,
                                  Trk_pt_max          =  25.,
                                  Z0_min              =  -150.,
                                  Z0_max              =  150.,
                                  D0_min              =  -11.,
                                  D0_max              =  11.,
                                  DeltaPhi_min        =  0.,
                                  DeltaPhi_max        =  3.2,
                                  DeltaEta_min        =  0.,
                                  DeltaEta_max        =  3.,
                                  DeltaR_min          =  0.,
                                  DeltaR_max          =  3.,
                                  PtSum_min           =  0.,
                                  PtSum_max           =  100.,
                                  OniaMass_min        =  0,
                                  OniaMass_max        =  14000,
                                  TauMass_min         =  0,
                                  TauMass_max         =  3000,
                                  OniaPt_min          =  8,
                                  OniaPt_max          =  100,
                                  MassErr_min         =  0.,
                                  MassErr_max         =  200.,
                                  Lxy_min             =  -2.,
                                  Lxy_max             =  20.,
                                  LxyErr_min          =  0.,
                                  LxyErr_max          =  2.,
                                  Tau_min             =  -2.,
                                  Tau_max             =  20.,
                                  TauErr_min          =  0.,
                                  TauErr_max          =  2.,
                                  pT_min              =  0.,
                                  pT_max              =  100.,
                                  pTErr_min           =  0.,
                                  pTErr_max           =  10.
                                  )
    
    items = [ HLTBphysMon ]
    return items

