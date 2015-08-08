if DQMonFlags.monManEnvironment() == 'tier0ESD' or DQMonFlags.monManEnvironment() == 'online' or  DQMonFlags.monManEnvironment() == 'AOD' :

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
 
    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    from AthenaMonitoring.DQMonFlags import DQMonFlags

    import TrigEgammaMonitoring.TrigEgammaMonitCategory as egammaConf
 
    try:

        monManEgamma = AthenaMonManager(name="EgammaMonManager",
                                        FileKey             = DQMonFlags.monManFileKey(),
                                        Environment         = DQMonFlags.monManEnvironment(),
                                        ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                        DataType            = DQMonFlags.monManDataType())
        topSequence += monManEgamma

        from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
        monbadlb = GetLArBadLBFilterTool()
        
        egammaMonOutputLevel = INFO
        #egammaMonOutputLevel = VERBOSE
        #egammaMonOutputLevel = DEBUG


### Setup which objects will be monitored
        if not ('egammaMonitorPhotons' in dir()):
            egammaMonitorPhotons  = True
            
        if not ('egammaMonitorElectrons' in dir()):
            egammaMonitorElectrons  = True

        if not ('egammaMonitorFwdEg' in dir()):
            egammaMonitorFwdEg  = True

        if not ('egammaMonitorZee' in dir()):
            egammaMonitorZee  = True

        if not ('egammaMonitorJPsi' in dir()):
            egammaMonitorJPsi  = True

        if not ('egammaMonitorUpsilon1S' in dir()):
            egammaMonitorUpsilon1S  = True

        if not ('egammaMonitorUpsilon2S' in dir()):
            egammaMonitorUpsilon2S  = True

        if not ('egammaMonitorWenu' in dir()):
            egammaMonitorWenu  = True
            
        if not ('egammaMonitorTop' in dir()):
            egammaMonitorTop  = True

## Commenting in/out Mon tools for now
        egammaMonitorWenu  = False
        egammaMonitorTop = False
        egammaMonitorJPsi  = False
        egammaMonitorUpsilon1S  = False
        egammaMonitorUpsilon2S  = False
#        egammaMonitorZee  = False
#        egammaMonitorFwdEg  = False
#        egammaMonitorPhotons  = False
#        egammaMonitorElectrons  = False

## Commenting in/out Trigger for now

        #MyDoTrigger = True
        MyDoTrigger = False
        #MyDoTrigger = DQMonFlags.useTrigger()
        
#        print "EGamma Monitoring : use trigger ?", MyDoTrigger

###

        photonTrigItems = []
        electronTrigItems = []
        mySingleElectronTrigger = []
        myDiElectronTrigger = [] 
        ZeeTrigItems = []
        JPsiTrigItems = []
        UpsilonTrigItems = []
        FrwdETrigItems = []
        MyTrigDecisionTool = ""

        if (MyDoTrigger):

#            photonTrigItems = ["HLT_g35_loose_g25_loose"]
#            electronTrigItems = ["HLT_e24_medium_L1EM18VH"]
#            mySingleElectronTrigger = ["HLT_e24_medium_L1EM18VH"] 
#            myDiElectronTrigger = ["HLT_2e12_loose","HLT_e24_lhtight_L1EM20VH_e15_etcut_Zee"] 
#            JpsiTrigItems = ["HLT_e5_lhtight_e4_etcut_Jpsiee"] 
#            FrwdETrigItems = ["HLT_e24_medium_L1EM18VH"]
#            ZeeTrigItems = ["HLT_e24_lhtight_L1EM20VH_e15_etcut_Zee", "HLT_2e12_loose"]

            photonTrigItems += egammaConf.primary_double_pho
            mySingleElectronTrigger += egammaConf.primary_single_ele
            myDiElectronTrigger += egammaConf.primary_double_ele
            JPsiTrigItems += egammaConf.monitoring_Jpsiee
            FrwdETrigItems += egammaConf.primary_single_ele
            ZeeTrigItems += egammaConf.monitoring_Zee 

            MyTrigDecisionTool = "Trig::TrigDecisionTool/TrigDecisionTool"
            
#            print "---------------"
#            print "--------------- Config Trigger EGamma Monitoring"
#            print "---------------"
#            print photonTrigItems
#            print mySingleElectronTrigger
#            print myDiElectronTrigger
#            print JPsiTrigItems
#            print FrwdETrigItems
#            print ZeeTrigItems
#            print "---------------"

        if(egammaMonitorPhotons):
            from egammaPerformance.egammaPerformanceConf import photonMonTool
            phMonTool = photonMonTool(name= "phMonTool",
                                      EgTrigDecisionTool = MyTrigDecisionTool,
                                      EgUseTrigger = MyDoTrigger,
                                      EgTrigger = photonTrigItems,
                                      PhotonContainer = "Photons",
                                      OutputLevel = egammaMonOutputLevel,
                                      )
            
            phMonTool.FilterTools += [ monbadlb ]

            if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
                phMonTool.FilterTools.append(monFilledBunchFilterTool)
            ToolSvc+=phMonTool
            monManEgamma.AthenaMonTools += [ "photonMonTool/phMonTool" ]

        if(egammaMonitorElectrons):
            from egammaPerformance.egammaPerformanceConf import electronMonTool
            elMonTool = electronMonTool(name= "elMonTool",
                                        EgTrigDecisionTool = MyTrigDecisionTool,
                                        EgUseTrigger = MyDoTrigger,
                                        EgTrigger = mySingleElectronTrigger,
                                        ElectronContainer = "Electrons",
                                        OutputLevel = egammaMonOutputLevel,
                                        )
                        
            elMonTool.FilterTools += [ monbadlb ]
            
            if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
                elMonTool.FilterTools.append(monFilledBunchFilterTool)
            ToolSvc+=elMonTool
            monManEgamma.AthenaMonTools += [ "electronMonTool/elMonTool" ]
            print elMonTool

        if(egammaMonitorFwdEg):
            from egammaPerformance.egammaPerformanceConf import forwardElectronMonTool
            fwdMonTool = forwardElectronMonTool(name= "fwdMonTool",
                                                EgTrigDecisionTool = MyTrigDecisionTool,
                                                EgUseTrigger = MyDoTrigger,
                                                EgTrigger = FrwdETrigItems,
                                                ForwardElectronContainer ="ForwardElectrons",
                                                OutputLevel = egammaMonOutputLevel,
                                              )
            
            fwdMonTool.FilterTools += [ monbadlb ]
            
            if jobproperties.Beam.beamType()=='collisions' and hasattr(ToolSvc, 'DQFilledBunchFilterTool'):
                fwdMonTool.FilterTools.append(monFilledBunchFilterTool)
            ToolSvc+=fwdMonTool
            monManEgamma.AthenaMonTools += [ "forwardElectronMonTool/fwdMonTool" ]    

        if(egammaMonitorZee):
            from egammaPerformance.egammaPerformanceConf import ZeeTaPMonTool
            ZeeMonTool = ZeeTaPMonTool(name= "ZeeMonTool",
                                       EgTrigDecisionTool = MyTrigDecisionTool,
                                       EgUseTrigger = MyDoTrigger,
                                       EgTrigger = ZeeTrigItems,
                                       ElectronContainer  ="Electrons",
                                       massPeak = 91188,
                                       electronEtCut = 20000,
                                       massLowerCut = 60000,
                                       massUpperCut = 120000,
                                       OutputLevel = egammaMonOutputLevel,
                                       )
            
            ToolSvc+=ZeeMonTool
            monManEgamma.AthenaMonTools += [ "ZeeTaPMonTool/ZeeMonTool" ]

        if(egammaMonitorJPsi):
            from egammaPerformance.egammaPerformanceConf import ZeeTaPMonTool
            JPsiMonTool = PhysMonTool(name= "JPsiMonTool",
                                         EgTrigDecisionTool = MyTrigDecisionTool,
                                         EgUseTrigger = MyDoTrigger,
                                         EgTrigger = JPsiTrigItems,
                                         ElectronContainer="Electrons",
                                         electronEtCut = 5000,
                                         massLowerCut = 2500,
                                         massUpperCut = 3500,
                                         OutputLevel = egammaMonOutputLevel,
#                                         massPeak = 3097,
#                                         massElectronClusterEtCut = 3000 ,
#                                         massLowerCut = 2500,
#                                         massUpperCut = 3500,
                                         )
        
            ToolSvc+=JPsiMonTool
            monManEgamma.AthenaMonTools += [ "ZeeTaPMonTool/JPsiMonTool" ]


        if(egammaMonitorUpsilon1S):
            from egammaPerformance.egammaPerformanceConf import physicsMonTool
            Upsilon1SMonTool = physicsMonTool(name= "Upsilon1SMonTool",
                                              ElectronContainer="Electrons",
                                              Trigger_Items = UpsilonTrigItems,
                                              ProcessName = "Upsilon1See",
                                              Selection_Items = ["all"],
                                              massShift = 9460,
                                              massElectronClusterEtCut = 1000 ,
                                              massLowerCut = 5000,
                                              massUpperCut = 15000,
                                              OutputLevel = egammaMonOutputLevel,
                                              TrigDecisionTool = MyTrigDecisionTool,
                                              UseTrigger = MyDoTrigger)
            
            ToolSvc+=Upsilon1SMonTool
            monManEgamma.AthenaMonTools += [ "physicsMonTool/Upsilon1SMonTool" ]


        if(egammaMonitorUpsilon2S):
            from egammaPerformance.egammaPerformanceConf import physicsMonTool
            Upsilon2SMonTool = physicsMonTool(name= "Upsilon2SMonTool",
                                              ElectronContainer="Electrons",
                                              Trigger_Items = UpsilonTrigItems,
                                              ProcessName = "Upsilon2See",
                                              Selection_Items = ["all"],
                                              massShift = 10023,
                                              massElectronClusterEtCut = 1000 ,
                                              massLowerCut = 5000,
                                              massUpperCut = 15000,
                                              OutputLevel = egammaMonOutputLevel,
                                              TrigDecisionTool = MyTrigDecisionTool,
                                              UseTrigger = MyDoTrigger)
            
            ToolSvc+=Upsilon2SMonTool
            monManEgamma.AthenaMonTools += [ "physicsMonTool/Upsilon2SMonTool" ]

        if(egammaMonitorWenu):
            from egammaPerformance.egammaPerformanceConf import ephysicsMonTool
            WenuMonTool = ephysicsMonTool(name= "WenuMonTool",
                                          ElectronContainer="Electrons",
                                          #JetContainer="Cone7TowerJets",
                                          JetContainer="AntiKt4TopoEMJets",
                                          metName = "MET_RefFinal",
                                          ProcessName = "Wenu",
                                          triggerXselection = WenutriggerXselection,
                                          massShift = 9460,
                                          massElectronClusterEtCut = 1000 ,
                                          massLowerCut = 0,
                                          massUpperCut = 200,
                                          LeadingElectronClusterPtCut = 20000,
                                          MissingParticleEtCut = 25000,
                                          JetEnergyCut = 1000000,
                                          DeltaRCut = 10,
                                          OutputLevel = egammaMonOutputLevel,
                                          TrigDecisionTool = MyTrigDecisionTool,
                                          UseTrigger = MyDoTrigger)
            
            ToolSvc+=WenuMonTool
            monManEgamma.AthenaMonTools += [ "ephysicsMonTool/WenuMonTool" ]

        if(egammaMonitorTop):
            from egammaPerformance.egammaPerformanceConf import TopphysicsMonTool
            TopMonTool = TopphysicsMonTool(name= "TopMonTool",
                                          ElectronContainer="Electrons",
                                          JetContainer="AntiKt4TopoEMJets",
                                          metName = "MET_RefFinal",
                                          ProcessName = "Topww",
                                          triggerXselection = WenutriggerXselection,
                                          Selection_Items = ["all","loose","tight","medium"],
                                          massShift = 9460,
                                          massElectronClusterEtCut = 1000 ,
                                          massLowerCut = 0,
                                          massUpperCut = 200,
                                          LeadingElectronClusterPtCut = 25000,
                                          MissingParticleEtCut = 40000,
                                          JetEnergyCut = 1000000,
                                          DeltaRCut = 10,
                                          OutputLevel = egammaMonOutputLevel,
                                          TrigDecisionTool = MyTrigDecisionTool,
                                          UseTrigger = MyDoTrigger)

            ToolSvc+=TopMonTool
            monManEgamma.AthenaMonTools += [ "TopphysicsMonTool/TopMonTool" ]
        
            

    except Exception:
        from AthenaCommon.Resilience import treatException
        treatException("egammaMonitoring_jobOptions.py: exception when setting up Egamma monitoring")
 
