# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from ROOT import TMath

class LArDQGlobals(object):
      __slots__ = ('HVeta_EMB','HVphi_EMB','HVeta_EMEC','HVphi_EMEC','HVeta_HECFcal','HVphi_HECFcal',
                   'LB_Bins','LB_Min','LB_Max','BCID_Bins','BCID_Min','BCID_Max',
                   'colTime_Bins','colTime_Min','colTime_Max','avgColTime_Bins','avgColTime_Min','avgColTime_Max',
                   'FEB_Feedthrough','FEB_Slot','FEB_Crates',
                   'FEB_N_channels','FEB_channels_Min','FEB_channels_Max', 'N_FEBErrors','FEBErrors',
                   'N_FEB_Parttions_Max','N_Cells', 'N_FEB', 'N_Partitions','Partitions','N_Gains','Gains',
                   'N_DigitsSummary','DigitsSummary',
                   'N_SubDet','SubDet','Samples_Bins','Samples_Min','Samples_Max',
                   'Energy_Bins','Energy_Min','Energy_Max', 'DSPEnergy_Bins', 'DSPEnergy_Min', 'DSPEnergy_Max',
                   'DSPTime_Bins', 'DSPTime_Min', 'DSPTime_Max', 'DSPQuality_Bins', 'DSPQuality_Min', 'DSPQuality_Max',
                   'DSP1Energy_Bins', 'DSP1Energy_Min', 'DSP1Energy_Max','DSPRanges_Bins', 'DSPRanges_Min', 'DSPRanges_Max', 'DSPRanges',
                   'DSPEonEoff_Bins','DSPEonEoff_Max', 'DSPTonToff_Bins','DSPTonToff_Max', 'DSPQonQoff_Bins','DSPQonQoff_Max', 'DSPThr_Bins',
                   'Energy_Bins','Energy_Min','Energy_Max','L1Trig_Bins','L1Trig_Min','L1Trig_Max',
                   'Evt_Bins','Evt_Min','Evt_Max','Evt_labels',
                   'EvtRej_Bins','EvtRej_Min','EvtRej_Max','EvtRej_labels','EvtRejYield_labels',
                   'L1Trig_Bins','L1Trig_Min','L1Trig_Max','rejBits_Bins',
                   'defaultStreamNames')

lArDQGlobals = LArDQGlobals()

#HV eta-phi bins (used by LArAffectedRegions, bin values copied from there). NOTE: EMEC bins are provided as a list of bin boundaries (variable bin size), all other partitions are in format [Nbins,x_low,x_high]
#barrel
lArDQGlobals.HVeta_EMB = {"EMBAPS":[8, 0.,1.6],"EMBA":[7,0.,1.4]}
lArDQGlobals.HVphi_EMB = {"EMBAPS":[32,-TMath.Pi(),TMath.Pi()],"EMBA":[128,-TMath.Pi(),TMath.Pi()]}
for end in ["PS",""]:
    bins=lArDQGlobals.HVeta_EMB["EMBA"+end]
    lArDQGlobals.HVeta_EMB["EMBC"+end]=[bins[0],-1*(bins[2]+0.01),-1*(bins[1]+0.01)]
    lArDQGlobals.HVphi_EMB["EMBC"+end]=lArDQGlobals.HVphi_EMB["EMBA"+end]
#endcap
emecbinsA=[1.375,1.50,1.6,1.8,2.0,2.1,2.3,2.5,2.8,3.2]
emecbinsC=[-1*(bi+0.01) for bi in reversed(emecbinsA)]
phibinsPS=64
phibins=256
lArDQGlobals.HVeta_EMEC = {"EMECAPS":emecbinsA,"EMECA":emecbinsA,"EMECCPS":emecbinsC,"EMECC":emecbinsC} 
lArDQGlobals.HVphi_EMEC={}
lArDQGlobals.HVphi_EMEC["EMECAPS"]=[-TMath.Pi()+ x*2*TMath.Pi()/phibinsPS for x in xrange(phibinsPS+1)]
lArDQGlobals.HVphi_EMEC["EMECA"]=[-TMath.Pi()+ x*2*TMath.Pi()/phibins for x in xrange(phibins+1)]
lArDQGlobals.HVphi_EMEC["EMECCPS"]=lArDQGlobals.HVphi_EMEC["EMECAPS"]
lArDQGlobals.HVphi_EMEC["EMECC"]=lArDQGlobals.HVphi_EMEC["EMECA"]
#hec-fcal
lArDQGlobals.HVeta_HECFcal = {"HECA":[1,1.5, 3.2],"FCalA":[1, 2.9, 4.9]} #same for all layers
lArDQGlobals.HVphi_HECFcal = {"HECA":[32,-TMath.Pi(),TMath.Pi()],"FCalA":[16, -3.2, 3.2]} #same for all layers
for par in ["HEC","FCal"]:
    bins=lArDQGlobals.HVeta_HECFcal[par+"A"]
    lArDQGlobals.HVeta_HECFcal[par+"C"]=[bins[0],-1*(bins[2]+0.01),-1*(bins[1]+0.01)]
    lArDQGlobals.HVphi_HECFcal[par+"C"]=lArDQGlobals.HVphi_HECFcal[par+"A"]
#Lumi block ranges
lArDQGlobals.LB_Bins=3000
lArDQGlobals.LB_Min=0
lArDQGlobals.LB_Max=lArDQGlobals.LB_Bins+lArDQGlobals.LB_Min
#bunch crossing id ranges
lArDQGlobals.BCID_Bins=3564
lArDQGlobals.BCID_Min=0.5
lArDQGlobals.BCID_Max=lArDQGlobals.BCID_Min+lArDQGlobals.BCID_Bins
#LArCollisionTime (A-C) ranges
lArDQGlobals.colTime_Bins=101
lArDQGlobals.colTime_Min=-50.5
lArDQGlobals.colTime_Max=50.5
lArDQGlobals.avgColTime_Bins=320
lArDQGlobals.avgColTime_Min=-40
lArDQGlobals.avgColTime_Max=40
#FEB ranges
lArDQGlobals.FEB_Feedthrough={"EMBA":[0,31],"EMBC":[0,31],"EMECA":[0,24],"EMECC":[0,24],"HECA":[0,24],"HECC":[0,24],"FCalA":[0,24],"FCalC":[0,24]}
lArDQGlobals.FEB_Slot={"EMBA":[1,14],"EMBC":[1,14],"EMECA":[1,15],"EMECC":[1,15],"HECA":[1,15],"HECC":[1,15],"FCalA":[1,15],"FCalC":[1,15]}
lArDQGlobals.FEB_Crates={"EMBA":[1,448],"EMBC":[1,448],"EMECA":[1,375],"EMECC":[1,375],"HECA":[1,375],"HECC":[1,375],"FCalA":[1,375],"FCalC":[1,375]}
#Feb N channels
lArDQGlobals.FEB_N_channels=128
lArDQGlobals.FEB_channels_Min=-0.5
lArDQGlobals.FEB_channels_Max=lArDQGlobals.FEB_N_channels-0.5

#total number of FEB
lArDQGlobals.N_FEB=1524
lArDQGlobals.N_FEB_Parttions_Max=500
lArDQGlobals.N_Cells=200000

#FEB errors
lArDQGlobals.N_FEBErrors=13
lArDQGlobals.FEBErrors=["Parity","BCID","Sample Header","EVTID","SCAC status","Sca out of range","Gain mismatch","Type mismatch","# of samples","Empty data block","Checksum / block size","Missing header","Bad gain"]
#partitions
lArDQGlobals.N_Partitions=8
lArDQGlobals.Partitions=["EMBC","EMBA","EMECC","EMECA","HECC","HECA","FCalC","FCalA"]
lArDQGlobals.N_SubDet=4
lArDQGlobals.SubDet=["EMB","EMEC","HEC","FCal"]
lArDQGlobals.N_Gains=3
lArDQGlobals.Gains=["HIGH","MEDIUM","LOW"]

#digits summary
lArDQGlobals.N_DigitsSummary=4
lArDQGlobals.DigitsSummary=["OutOfRange","Saturation","Null Digits","Mean Time"]

#samples range
lArDQGlobals.Samples_Bins=32
lArDQGlobals.Samples_Min=-0.5
lArDQGlobals.Samples_Max=lArDQGlobals.Samples_Bins-0.5

#ADC energy range
lArDQGlobals.Energy_Bins=300
lArDQGlobals.Energy_Min=0.
lArDQGlobals.Energy_Max=3000.

#DSP  diff
lArDQGlobals.DSPEnergy_Bins=400
lArDQGlobals.DSPEnergy_Min=-40.
lArDQGlobals.DSPEnergy_Max=40.
lArDQGlobals.DSPTime_Bins=400
lArDQGlobals.DSPTime_Min=-800.
lArDQGlobals.DSPTime_Max=800.
lArDQGlobals.DSPQuality_Bins=400
lArDQGlobals.DSPQuality_Min=-3000.
lArDQGlobals.DSPQuality_Max=3000.
lArDQGlobals.DSP1Energy_Bins=5000
lArDQGlobals.DSP1Energy_Min=-515.
lArDQGlobals.DSP1Energy_Max=515.
lArDQGlobals.DSPRanges_Bins=4
lArDQGlobals.DSPRanges_Min=0
lArDQGlobals.DSPRanges_Max=4
lArDQGlobals.DSPRanges=["E < 2^{13} MeV","E < 2^{16} MeV","E < 2^{19} MeV","E < 2^{22} MeV"]
lArDQGlobals.DSPEonEoff_Bins=300
lArDQGlobals.DSPEonEoff_Max=20000.
lArDQGlobals.DSPTonToff_Bins=300
lArDQGlobals.DSPTonToff_Max=80000.
lArDQGlobals.DSPQonQoff_Bins=300
lArDQGlobals.DSPQonQoff_Max=66000.
lArDQGlobals.DSPThr_Bins=2000

#trigger word range
lArDQGlobals.L1Trig_Bins=256
lArDQGlobals.L1Trig_Min=-0.5
lArDQGlobals.L1Trig_Max=lArDQGlobals.L1Trig_Bins-0.5

#Event types histo
lArDQGlobals.Evt_Bins = 15
lArDQGlobals.Evt_Min = -0.5
lArDQGlobals.Evt_Max = 14.5
lArDQGlobals.Evt_labels = ["","Raw data - Transparent","","Result - Physic","","","Calibration","","","Pedestals","","","","Raw data+Result"]
lArDQGlobals.EvtRej_Bins = 7 
lArDQGlobals.EvtRej_Min = 0.5
lArDQGlobals.EvtRej_Max = 7.5
lArDQGlobals.EvtRej_labels = [">=1 FEB in error",">=4 FEBs in error","LArError_DATACORRUPTED","LArError_DATACORRUPTEDVETO","LArError_NOISEBURSTVETO","Accepted","Total"]
lArDQGlobals.EvtRejYield_labels = [">=1 FEB in error",">=4 FEBs in error","LArError_DATACORRUPTED","LArError_DATACORRUPTEDVETO","LArError_NOISEBURSTVETO","Accepted"]

lArDQGlobals.rejBits_Bins=8192

lArDQGlobals.defaultStreamNames = ["express","Main","CosmicCalo","L1Calo","L1Topo","ZeroBias","Standby","LArCells","LArCellsEmpty","Background","others"]
