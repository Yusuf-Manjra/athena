#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def LArFEBMonConfig(inputFlags, cellDebug=False, dspDebug=False):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArFEBMonCfg')

    from LArMonitoring.GlobalVariables import lArDQGlobals

    from AthenaConfiguration.ComponentFactory import CompFactory
    larFEBMonAlg = helper.addAlgorithm(CompFactory.LArFEBMonAlg,'larFEBMonAlg')

    GroupName="FEBMon"
    nslots=[]
    for i in range(0,len(lArDQGlobals.FEB_Slot)): 
       nslots.append(lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[i]][1])

    larFEBMonAlg.MonGroup=GroupName
    larFEBMonAlg.PartitionNames=lArDQGlobals.Partitions
    larFEBMonAlg.SubDetNames=lArDQGlobals.SubDet
    larFEBMonAlg.Streams=lArDQGlobals.defaultStreamNames

    if "COMP200" not in inputFlags.IOVDb.DatabaseInstance:
       iovDbSvc=helper.resobj.getService("IOVDbSvc")
       condLoader=helper.resobj.getCondAlgo("CondInputLoader")
       dbString="<db>COOLONL_LAR/CONDBR2</db>"
       persClass="AthenaAttributeList"
       fld="/LAR/Configuration/DSPThresholdFlat/Thresholds"
       iovDbSvc.Folders.append(fld+dbString)
       condLoader.Load.append((persClass,fld))
       larFEBMonAlg.keyDSPThresholds=fld
    else:
       fld='/LAR/Configuration/DSPThreshold/Thresholds'
       db='LAR_ONL'
       obj='LArDSPThresholdsComplete'
       helper.resobj.addFolderList(inputFlags,[(fld,db,obj)])
       larFEBMonAlg.keyDSPThresholds="LArDSPThresholds"

    #from AthenaCommon.Constants import VERBOSE
    #larFEBMonAlg.OutputLevel=VERBOSE

    # adding LArFebErrorSummary algo
    from LArROD.LArFebErrorSummaryMakerConfig import LArFebErrorSummaryMakerCfg
    acc = LArFebErrorSummaryMakerCfg(inputFlags)
    helper.resobj.merge(acc)

    Group = helper.addGroup(
        larFEBMonAlg,
        GroupName,
        '/LAr/'+GroupName+'/'
    )


    #Summary histos
    summary_hist_path='Summary/'
    

    Group.defineHistogram('nbFEB;NbOfReadoutFEBGlobal', 
                                  title='# of readout FEB/DSP header',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.N_FEB+11, xmin=-0.5, xmax=lArDQGlobals.N_FEB+10+0.5)
    Group.defineHistogram('nbFEBpart,part;NbOfEvts2d', 
                                  title='# of readout FEB/DSP header:Num. FEBs:Partition',
                                  type='TH2I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.N_FEB_Parttions_Max, xmin=-0.5, xmax=lArDQGlobals.N_FEB_Parttions_Max-0.5,
                                  ybins=lArDQGlobals.N_Partitions, ymin=-0.5, ymax=lArDQGlobals.N_Partitions-0.5,
                                  #? how to put labels to Y axis only ?
                                  )
    Group.defineHistogram('febError,part;NbOfLArFEBMonAlgErrors_dE', 
                                  title='# of data corruption errors',
                                  type='TH2I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.N_FEBErrors, xmin=0.5, xmax=lArDQGlobals.N_FEBErrors+0.5,
                                  ybins=lArDQGlobals.N_Partitions, ymin=-0.5, ymax=lArDQGlobals.N_Partitions-0.5,
                                  labels=lArDQGlobals.FEBErrors+lArDQGlobals.Partitions)
    Group.defineHistogram('dspThrADC;dspThresholdsADC', 
                                  title='DSP thresholds to readout samples:Number of cells:Cell threshold in ADC counts',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.DSPThr_Bins+1, xmin=-0.5, xmax=lArDQGlobals.DSPThr_Bins+0.5)
    Group.defineHistogram('dspThrQT;dspThresholds_qfactortime', 
                                  title='DSP thresholds to readout (qfactor+time):Number of cells:Cell threshold in ADC counts',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.DSPThr_Bins+1, xmin=-0.5, xmax=lArDQGlobals.DSPThr_Bins+0.5)
    Group.defineHistogram('EvtType;Eventtype', 
                                  title='Event type (1st readout FEB)',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.Evt_Bins, xmin=lArDQGlobals.Evt_Min, xmax=lArDQGlobals.Evt_Max,
                                  labels=lArDQGlobals.Evt_labels)
    Group.defineHistogram('LVL1Trig;TriggerWord', 
                                  title='Number of Events per L1 trigger word (8 bits):L1 trigger word',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.L1Trig_Bins, xmin=lArDQGlobals.L1Trig_Min, xmax=lArDQGlobals.L1Trig_Max)
    Group.defineHistogram('LVL1TrigAllDSP;TriggerWordAllDSP', 
                                  title='Number of L1 trigger word per DSP (8 bits):L1 trigger word',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.L1Trig_Bins, xmin=lArDQGlobals.L1Trig_Min, xmax=lArDQGlobals.L1Trig_Max)
    Group.defineHistogram('EvtRej;EventsRejected', 
                                  title='Nb of events rejected (at least one error)',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.EvtRej_Bins, xmin=lArDQGlobals.EvtRej_Min, xmax=lArDQGlobals.EvtRej_Max,
                                  labels=lArDQGlobals.EvtRej_labels)
    Group.defineHistogram('EvtRej,EvtRejYield;EventsRejectedYield', 
                                  title='Data corruption yield:Corruption type:Yield(%)',
                                  type='TProfile',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.EvtRej_Bins-1, xmin=lArDQGlobals.EvtRej_Min, xmax=lArDQGlobals.EvtRej_Max-1,
                                  labels=lArDQGlobals.EvtRejYield_labels)
    Group.defineHistogram('LB,EvtRejYield;YieldOfRejectedEventsVsLB', 
                                  title='Yield of corrupted events (DATACORRUPTED):Luminosity Block:Yield(%)',
                                  type='TProfile',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max)
    Group.defineHistogram('LB,EvtRejYieldOut;YieldOfRejectedEventsVsLBout', 
                                  title='Yield of corrupted events (DATACORRUPTED) not vetoed by time window:Luminosity Block:Yield(%)',
                                  type='TProfile',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max)
    Group.defineHistogram('rejBits;rejectionBits', 
                                  title='Errors at the origin of event rejection:Bits:Number of (rejected) events',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.rejBits_Bins, xmin=-0.5, xmax=lArDQGlobals.rejBits_Bins-0.5)
    Group.defineHistogram('LB;NbOfEventsVsLB', 
                                  title='Nb of events per LB:Luminosity Block',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max)
    Group.defineHistogram('LB;NbOfEventsVsLB', 
                                  title='Nb of events per LB:Luminosity Block',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max)
    Group.defineHistogram('NbOfSweet2;NbOfSw2', 
                                  title='# of cells with samples readout:Number of cells:Number of events',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.N_Cells/10, xmin=-1000, xmax=lArDQGlobals.N_Cells-1000)
    Group.defineHistogram('LB,LArEvSize;eventSizeVsLB', 
                                  title='LAr event size (w/o ROS headers):Luminosity Block:Megabytes',
                                  type='TProfile',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max)
    Group.defineHistogram('NbOfSamp;NbOfSamples', 
                                  title='# of samples (1st readout FEB):Samples:Number of events',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.Samples_Bins, xmin=lArDQGlobals.Samples_Min, xmax=lArDQGlobals.Samples_Max)

    if inputFlags.DQ.Environment == 'online':
       Group.defineHistogram('LBf,EvtRejYield;EventsRejectedLB',
                                titile='% of events rejected in current LB (online only)',
                                type='TProfile',
                                path=summary_hist_path,
                                xbins=1, xmin=0, xmax=1, labels=['% of events'])
       Group.defineHistogram('LB,streamBin,LArEvSizePart;eventSizeStreamVsLB',
                                titile='LAr event size per stream per LB (w/o ROS headers)',
                                type='TProfile2D',
                                path=summary_hist_path,
                                xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max,
                                ybins=len(larFEBMonAlg.Streams),ymin=-0.5, ymax= len(larFEBMonAlg.Streams)-0.5,
                                #labels=[''] needs only labels on Yaxis
                                )

    # Now per partition histograms
    for subdet in range(0,lArDQGlobals.N_SubDet):
       hist_path='/LAr/'+GroupName+'/'+lArDQGlobals.SubDet[subdet]+'/'
       slot_low = lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[subdet*2]][0] - 0.5
       slot_up  = lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[subdet*2]][1] + 0.5
       slot_n = int(slot_up - slot_low)
       ft_low = lArDQGlobals.FEB_Feedthrough[lArDQGlobals.Partitions[subdet*2]][0] - 0.5
       ft_up  = lArDQGlobals.FEB_Feedthrough[lArDQGlobals.Partitions[subdet*2]][1] + 0.5
       ft_n = int(ft_up - ft_low)

       darray = helper.addArray([lArDQGlobals.Partitions[2*subdet:2*subdet+2]],larFEBMonAlg,lArDQGlobals.SubDet[subdet])

       darray.defineHistogram('slotPar,FTPar;Parity',
                              title='Parity error:Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotBcid,FTBcid;BCID',
                              title='BCID mismatch betw. 2 halves of FEB:Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotRadd,FTRadd;RADD',
                              title='Sample header mismatch betw. 2 halves of FEB:Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotEvtid,FTEvtid;EVTID',
                              title='EVTID mismatch betw. 2 halves of FEB:Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotScac,FTScac;SCACStatus',
                              title='Wrong SCAC status in one half of a FEB:Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotscout,FTscout;scaOutOfRange',
                              title='Sca out of range:Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotgain,FTgain;gainMismatch',
                              title='Gain mismatch within time samples:Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slottype,FTtype;typeMismatch',
                              title='Event type mismatch:Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotsmp,FTsmp;badNbOfSamp',
                              title='Non uniform number of samples:Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotzero,FTzero;zeroSamp',
                              title='Empty FEB data blocks:Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotsum,FTsum;checkSum',
                              title='Checksum / DSP block size:Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotmis,FTmis;missingHeader',
                              title='Missing header :Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotgain,FTgain;badGain',
                              title='Bad gain :Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotabs,FTabs;LArFEBMonAlgErrorsAbsolute',
                              title='Nb of events with at least one error :Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)
        
       darray.defineHistogram('slotabs,FTabs,erryield;LArFEBMonAlgErrors',
                              title='% of events with at least one error :Slot:FT',
                              type='TProfile2D',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotmist,FTmist;missingTriggerType',
                              title='LVL1 trigger type missing or different from event type :Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotnb,FTnb;nbOfEvts',
                              title='Nb of events (DSP header check only) :Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotnb,FTnb;NbOfSweet1PerFEB',
                              title='Average # of cells with (qfactor+time) readout :Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              weight='weightsweet1',
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotnb,FTnb;NbOfSweet2PerFEB',
                              title='Average # of cells with samples readout :Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              weight='weightsweet2',
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('nbFEBpart;nbOfFebBlocks',
                              title='# of readout FEBs (DSP header check only) :Slot:FT',
                              type='TH1I',
                              path=hist_path,
                              xbins=lArDQGlobals.N_FEB_Parttions_Max, xmin=-0.5, xmax=lArDQGlobals.N_FEB_Parttions_Max-0.5)

       darray.defineHistogram('slotMasked,FTMasked;knownFaultyFEB',
                              title='FEB with known errors (1:err. ignored 2:FEB masked):Slot:FT',
                              type='TH2I',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('LB,LArEvSizePart;eventSizeVsLB',
                              title='LAr event size per LB (w/o ROS headers):Luminosity Block',
                              type='TProfile',
                              path=hist_path,
                              xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max)

       if inputFlags.DQ.Environment == 'online':
          darray.defineHistogram('LBf,erronl;EventsRejectedLB',
                                titile='% of events rejected in current LB (online only)',
                                type='TProfile',
                                path=hist_path,
                                xbins=1, xmin=0, xmax=1, labels=['% of events'])
          darray.defineHistogram('LB,streamBin,LArEvSizePart;eventSizeStreamVsLB',
                                titile='LAr event size per stream per LB (w/o ROS headers)',
                                type='TProfile2D',
                                path=hist_path,
                                xbins=lArDQGlobals.LB_Bins, xmin=lArDQGlobals.LB_Min, xmax=lArDQGlobals.LB_Max,
                                ybins=len(larFEBMonAlg.Streams),ymin=-0.5, ymax= len(larFEBMonAlg.Streams)-0.5,
                                #labels=[''] needs only labels on Y-axis
                                )
       pass

    return helper.result()

    

if __name__=='__main__':

   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from AthenaCommon.Logging import log
   from AthenaCommon.Constants import DEBUG
   from AthenaCommon.Configurable import Configurable
   Configurable.configurableRun3Behavior=1
   log.setLevel(DEBUG)


   from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
   createLArMonConfigFlags()

   from AthenaConfiguration.TestDefaults import defaultTestFiles
   ConfigFlags.Input.Files = defaultTestFiles.RAW

   ConfigFlags.Output.HISTFileName = 'LArFEBMonOutput.root'
   ConfigFlags.DQ.enableLumiAccess = True
   ConfigFlags.DQ.useTrigger = True
   ConfigFlags.Beam.Type = 'collisions'
   ConfigFlags.lock()


   from CaloRec.CaloRecoConfig import CaloRecoCfg
   cfg=CaloRecoCfg(ConfigFlags)

   #from CaloD3PDMaker.CaloD3PDConfig import CaloD3PDCfg,CaloD3PDAlg
   #cfg.merge(CaloD3PDCfg(ConfigFlags, filename=ConfigFlags.Output.HISTFileName, streamname='CombinedMonitoring'))

   aff_acc = LArFEBMonConfig(ConfigFlags)
   cfg.merge(aff_acc)

   cfg.printConfig()

   ConfigFlags.dump()
   f=open("LArFEBMon.pkl","w")
   cfg.store(f)
   f.close()

   #cfg.run(100,OutputLevel=WARNING)
