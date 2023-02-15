#
# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format


if __name__ == '__main__':
    
    
    from AthenaConfiguration.AllConfigFlags import initConfigFlags
    from AthenaCommon.Logging import logging
    import glob
    import sys

    import argparse
    parser = argparse.ArgumentParser(prog='python -m L1CaloFEXTools.L1CaloFEXToolsConfig',
                                   description="""Decorator tool for FEX towers athena script.\n\n
                                   Example: python -m L1CaloFEXTools.L1CaloFEXToolsConfig --filesInput "data22*" --evtMax 10 --outputs eTOBs """)
    parser.add_argument('--evtMax',type=int,default=-1,help="number of events")
    parser.add_argument('--filesInput',nargs='+',help="input files",required=True)
    parser.add_argument('--skipEvents',type=int,default=0,help="number of events to skip")
    parser.add_argument('--outputLevel',default="WARNING",choices={ 'INFO','WARNING','DEBUG','VERBOSE'})
    parser.add_argument('--outputs',nargs='+',choices={"jFex","eFex","gFex"},required=True, help="What data to decode and output.")
    args = parser.parse_args()


    log = logging.getLogger('L1CaloFEXToolsConfig')
    log.setLevel(logging.DEBUG)

    from AthenaCommon import Constants
    algLogLevel = getattr(Constants,args.outputLevel)

    flags = initConfigFlags()
    if any(["data" in f for f in args.filesInput]):
        flags.Trigger.triggerConfig='DB'

    flags.Exec.OutputLevel = algLogLevel
    flags.Exec.MaxEvents = args.evtMax
    flags.Exec.SkipEvents = args.skipEvents
    flags.Input.Files = [file for x in args.filesInput for file in glob.glob(x)]
    flags.Concurrency.NumThreads = 1
    flags.Concurrency.NumConcurrentEvents = 1
  
    if any(["data" in f for f in args.filesInput]):
        s=args.filesInput[0].replace('*','').replace('.data','')
        flags.Output.AODFileName = "AOD."+(s.split("/")[-1]).split('_SFO')[0]+"pool.root"
    else:
        flags.Output.AODFileName = 'AOD.pool.root'  
    
    flags.Trigger.EDMVersion = 3
    flags.Trigger.doLVL1 = True
    flags.Trigger.enableL1CaloPhase1 = True
    if flags.Common.isOnline:
        flags.IOVDb.GlobalTag = flags.Trigger.OnlineCondTag

    if not flags.Input.isMC and flags.Input.RunNumber[0] > 400000:
        flags.GeoModel.AtlasVersion = 'ATLAS-R3S-2021-03-01-00'

    # Enable only calo for this test
    from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(flags,['LAr','Tile','MBTS'],True)

    flags.lock()
    
    # Set up the main service "acc"
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(flags)
    
    # The decoderAlg needs to load ByteStreamMetadata for the detector mask
    from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
    acc.merge(ByteStreamReadCfg(flags))    
    
    # Generate run3 L1 menu
    from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
    acc.merge(L1ConfigSvcCfg(flags))
    
    decoderTools = []
    outputEDM = []
    maybeMissingRobs = []

    
    def addEDM(edmType, edmName):
        auxType = edmType.replace('Container','AuxContainer')
        return [f'{edmType}#{edmName}', f'{auxType}#{edmName}Aux.']
        
    def getSimHandle(key):
        """
        Add 'Sim' to the standard handle path
        """
        keyPath = key.path()
        keyPath += "Sim"
        key.Path = keyPath
        return key

    ##################################################
    # LATOME and Tile
    ##################################################
    from L1CaloFEXSim.L1CaloFEXSimCfg import ReadSCellFromByteStreamCfg,TriggerTowersInputCfg
    
    #Creates SCells
    acc.merge(ReadSCellFromByteStreamCfg(flags,key="SCell",keyIn="SC_ET"))
    
    # Creates the TriggerTower container
    acc.merge(TriggerTowersInputCfg(flags))
    
    if "eFex" in args.outputs:
        
        ##################################################
        # eFEX simulation
        ##################################################         
        eFEXInputs = CompFactory.LVL1.eTowerMakerFromSuperCells('eTowerMakerFromSuperCells')
        eFEXInputs.eSuperCellTowerMapperTool = CompFactory.LVL1.eSuperCellTowerMapper('eSuperCellTowerMapper')
        eFEX = CompFactory.LVL1.eFEXDriver('eFEXDriver')
        eFEX.eFEXSysSimTool = CompFactory.LVL1.eFEXSysSim('eFEXSysSimTool')
        
        #TOBs
        eFEX.eFEXSysSimTool.Key_eFexEMOutputContainer  = getSimHandle( eFEX.eFEXSysSimTool.Key_eFexEMOutputContainer )
        eFEX.eFEXSysSimTool.Key_eFexTauOutputContainer = getSimHandle( eFEX.eFEXSysSimTool.Key_eFexTauOutputContainer)

        outputEDM += addEDM('xAOD::eFexEMRoIContainer' , 'L1_eEMRoISim' )
        outputEDM += addEDM('xAOD::eFexTauRoIContainer', 'L1_eTauRoISim')
        
        #xTOBs
        eFEX.eFEXSysSimTool.Key_eFexEMxTOBOutputContainer  = getSimHandle( eFEX.eFEXSysSimTool.Key_eFexEMxTOBOutputContainer )
        eFEX.eFEXSysSimTool.Key_eFexTauxTOBOutputContainer = getSimHandle( eFEX.eFEXSysSimTool.Key_eFexTauxTOBOutputContainer)

        outputEDM += addEDM('xAOD::eFexEMRoIContainer' , 'L1_eEMxRoISim' )
        outputEDM += addEDM('xAOD::eFexTauRoIContainer', 'L1_eTauxRoISim')        
        
        acc.addEventAlgo(eFEXInputs, sequenceName='AthAlgSeq')
        acc.addEventAlgo(eFEX, sequenceName='AthAlgSeq')    
        
        ##################################################
        # eFEX decoded TOBs
        ##################################################       
        from L1CaloFEXByteStream.L1CaloFEXByteStreamConfig import eFexByteStreamToolCfg
        
        eFexTool = eFexByteStreamToolCfg('eFexBSDecoder', flags,TOBs='eTOBs',xTOBs='exTOBs')
        
        decoderTools += [eFexTool]
        maybeMissingRobs += eFexTool.ROBIDs        
        
        outputEDM += addEDM('xAOD::eFexEMRoIContainer' , 'L1_eEMRoI'  )
        outputEDM += addEDM('xAOD::eFexTauRoIContainer', 'L1_eTauRoI' )
        outputEDM += addEDM('xAOD::eFexEMRoIContainer' , 'L1_eEMxRoI' )
        outputEDM += addEDM('xAOD::eFexTauRoIContainer', 'L1_eTauxRoI')
        
        
    if "jFex" in args.outputs:
        ##################################################
        # jFEX simulation
        ##################################################    
        jFEX = CompFactory.LVL1.jFEXDriver('jFEXDriver')
        jFEX.jSuperCellTowerMapperTool = CompFactory.LVL1.jSuperCellTowerMapper('jSuperCellTowerMapper')
        jFEX.jSuperCellTowerMapperTool.SCellMasking = True
        jFEX.jFEXSysSimTool = CompFactory.LVL1.jFEXSysSim('jFEXSysSimTool')
        
        #TOBs
        jFEX.jFEXSysSimTool.Key_jFexSRJetOutputContainer = getSimHandle( jFEX.jFEXSysSimTool.Key_jFexSRJetOutputContainer)
        jFEX.jFEXSysSimTool.Key_jFexLRJetOutputContainer = getSimHandle( jFEX.jFEXSysSimTool.Key_jFexLRJetOutputContainer)
        jFEX.jFEXSysSimTool.Key_jFexTauOutputContainer   = getSimHandle( jFEX.jFEXSysSimTool.Key_jFexTauOutputContainer  )
        jFEX.jFEXSysSimTool.Key_jFexSumETOutputContainer = getSimHandle( jFEX.jFEXSysSimTool.Key_jFexSumETOutputContainer)
        jFEX.jFEXSysSimTool.Key_jFexMETOutputContainer   = getSimHandle( jFEX.jFEXSysSimTool.Key_jFexMETOutputContainer  )
        jFEX.jFEXSysSimTool.Key_jFexFwdElOutputContainer = getSimHandle( jFEX.jFEXSysSimTool.Key_jFexFwdElOutputContainer)
        
        outputEDM += addEDM('xAOD::jFexSRJetRoIContainer', 'L1_jFexSRJetRoISim')
        outputEDM += addEDM('xAOD::jFexLRJetRoIContainer', 'L1_jFexLRJetRoISim')
        outputEDM += addEDM('xAOD::jFexTauRoIContainer'  , 'L1_jFexTauRoISim'  )
        outputEDM += addEDM('xAOD::jFexFwdElRoIContainer', 'L1_jFexFwdElRoISim')
        outputEDM += addEDM('xAOD::jFexSumETRoIContainer', 'L1_jFexSumETRoISim')
        outputEDM += addEDM('xAOD::jFexMETRoIContainer'  , 'L1_jFexMETRoISim'  ) 
        

        #xTOBs
        jFEX.jFEXSysSimTool.Key_xTobOutKey_jJ   = getSimHandle( jFEX.jFEXSysSimTool.Key_xTobOutKey_jJ  )
        jFEX.jFEXSysSimTool.Key_xTobOutKey_jLJ  = getSimHandle( jFEX.jFEXSysSimTool.Key_xTobOutKey_jLJ )
        jFEX.jFEXSysSimTool.Key_xTobOutKey_jTau = getSimHandle( jFEX.jFEXSysSimTool.Key_xTobOutKey_jTau)
        jFEX.jFEXSysSimTool.Key_xTobOutKey_jEM  = getSimHandle( jFEX.jFEXSysSimTool.Key_xTobOutKey_jEM )    
        
        outputEDM += addEDM('xAOD::jFexSRJetRoIContainer', 'L1_jFexSRJetxRoISim' )
        outputEDM += addEDM('xAOD::jFexLRJetRoIContainer', 'L1_jFexLRJetxRoISim' )
        outputEDM += addEDM('xAOD::jFexTauRoIContainer'  , 'L1_jFexTauxRoISim'   )
        outputEDM += addEDM('xAOD::jFexFwdElRoIContainer', 'L1_jFexFwdElxRoISim' )
        
        acc.addEventAlgo(jFEX, sequenceName='AthAlgSeq')   
        
        
        ##################################################
        # jFEX decoded TOBs
        ##################################################       
        from L1CaloFEXByteStream.L1CaloFEXByteStreamConfig import jFexRoiByteStreamToolCfg
        jFexRoiByteStreamTool = jFexRoiByteStreamToolCfg('jFexBSDecoderTool', flags=flags, writeBS=False)
        
        decoderTools += [jFexRoiByteStreamTool]
        maybeMissingRobs += jFexRoiByteStreamTool.ROBIDs
                
        outputEDM += addEDM('xAOD::jFexSRJetRoIContainer', 'L1_jFexSRJetRoI')
        outputEDM += addEDM('xAOD::jFexLRJetRoIContainer', 'L1_jFexLRJetRoI')
        outputEDM += addEDM('xAOD::jFexTauRoIContainer'  , 'L1_jFexTauRoI'  )
        outputEDM += addEDM('xAOD::jFexFwdElRoIContainer', 'L1_jFexFwdElRoI')
        outputEDM += addEDM('xAOD::jFexSumETRoIContainer', 'L1_jFexSumETRoI')
        outputEDM += addEDM('xAOD::jFexMETRoIContainer'  , 'L1_jFexMETRoI'  ) 

        ##################################################
        # jFEX decoded Towers
        ##################################################  
        from L1CaloFEXByteStream.L1CaloFEXByteStreamConfig import jFexInputByteStreamToolCfg
        inputjFexTool = jFexInputByteStreamToolCfg('jFexInputBSDecoder', flags)
        for module_id in inputjFexTool.ROBIDs:
            maybeMissingRobs.append(module_id)

        decoderTools += [inputjFexTool]
        # saving/adding the jTower xAOD container
        outputEDM += addEDM('xAOD::jFexTowerContainer', inputjFexTool.jTowersWriteKey.Path)        

    if "gFex" in args.outputs:    

        ##################################################
        # gFEX simulation
        ##################################################  
        gFEX = CompFactory.LVL1.gFEXDriver('gFEXDriver')
        gFEX.gSuperCellTowerMapperTool = CompFactory.LVL1.gSuperCellTowerMapper('gSuperCellTowerMapper')
        gFEX.gFEXSysSimTool = CompFactory.LVL1.gFEXSysSim('gFEXSysSimTool')
        
        #TOBs
        gFEX.gFEXSysSimTool.Key_gFexSRJetOutputContainer              = getSimHandle( gFEX.gFEXSysSimTool.Key_gFexSRJetOutputContainer             )
        gFEX.gFEXSysSimTool.Key_gFexLRJetOutputContainer              = getSimHandle( gFEX.gFEXSysSimTool.Key_gFexLRJetOutputContainer             )
        gFEX.gFEXSysSimTool.Key_gFexRhoOutputContainer                = getSimHandle( gFEX.gFEXSysSimTool.Key_gFexRhoOutputContainer               )
        gFEX.gFEXSysSimTool.Key_gScalarEJwojOutputContainer           = getSimHandle( gFEX.gFEXSysSimTool.Key_gScalarEJwojOutputContainer          )
        gFEX.gFEXSysSimTool.Key_gMETComponentsJwojOutputContainer     = getSimHandle( gFEX.gFEXSysSimTool.Key_gMETComponentsJwojOutputContainer    )
        gFEX.gFEXSysSimTool.Key_gMHTComponentsJwojOutputContainer     = getSimHandle( gFEX.gFEXSysSimTool.Key_gMHTComponentsJwojOutputContainer    )
        gFEX.gFEXSysSimTool.Key_gMSTComponentsJwojOutputContainer     = getSimHandle( gFEX.gFEXSysSimTool.Key_gMSTComponentsJwojOutputContainer    )
        gFEX.gFEXSysSimTool.Key_gMETComponentsNoiseCutOutputContainer = getSimHandle( gFEX.gFEXSysSimTool.Key_gMETComponentsNoiseCutOutputContainer)
        gFEX.gFEXSysSimTool.Key_gMETComponentsRmsOutputContainer      = getSimHandle( gFEX.gFEXSysSimTool.Key_gMETComponentsRmsOutputContainer     )
        gFEX.gFEXSysSimTool.Key_gScalarENoiseCutOutputContainer       = getSimHandle( gFEX.gFEXSysSimTool.Key_gScalarENoiseCutOutputContainer      )
        gFEX.gFEXSysSimTool.Key_gScalarERmsOutputContainer            = getSimHandle( gFEX.gFEXSysSimTool.Key_gScalarERmsOutputContainer           )

        outputEDM += addEDM('xAOD::gFexJetRoIContainer'   , 'L1_gFexRhoRoISim'            )
        outputEDM += addEDM('xAOD::gFexJetRoIContainer'   , 'L1_gFexSRJetRoISim'          )
        outputEDM += addEDM('xAOD::gFexJetRoIContainer'   , 'L1_gFexLRJetRoISim'          )
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gScalarEJwojSim'          )
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gMETComponentsJwojSim'    )
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gMHTComponentsJwojSim'    )
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gMSTComponentsJwojSim'    )
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gMETComponentsNoiseCutSim')
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gScalarENoiseCutSim'      )
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gScalarERmsSim'           )                
        outputEDM += addEDM('xAOD::gFexTowerContainer',     'L1_gFexTriggerTowers'        )                
        
        acc.addEventAlgo(gFEX, sequenceName='AthAlgSeq')
        
        
        ##################################################       
        # gFEX decoded TOBs
        ##################################################       
        from L1CaloFEXByteStream.L1CaloFEXByteStreamConfig import gFexByteStreamToolCfg
        gFexTool = gFexByteStreamToolCfg('gFexBSDecoder', flags)
        decoderTools += [gFexTool]
                
        outputEDM += addEDM('xAOD::gFexJetRoIContainer'   , 'L1_gFexRhoRoI'            )
        outputEDM += addEDM('xAOD::gFexJetRoIContainer'   , 'L1_gFexSRJetRoI'          )
        outputEDM += addEDM('xAOD::gFexJetRoIContainer'   , 'L1_gFexLRJetRoI'          )
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gScalarEJwoj'          )
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gMETComponentsJwoj'    )
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gMHTComponentsJwoj'    )
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gMSTComponentsJwoj'    )
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gMETComponentsNoiseCut')
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gScalarENoiseCut'      )
        outputEDM += addEDM('xAOD::gFexGlobalRoIContainer', 'L1_gScalarERms'           )        


        from L1CaloFEXByteStream.L1CaloFEXByteStreamConfig import gFexInputByteStreamToolCfg
        gFexInputTool = gFexInputByteStreamToolCfg('gFexInputBSDecoder', flags)
        decoderTools += [gFexInputTool]
        outputEDM += addEDM('xAOD::gFexTowerContainer'    , 'L1_gFexDataTowers'        )                

    
    decoderAlg = CompFactory.L1TriggerByteStreamDecoderAlg(name="L1TriggerByteStreamDecoder", DecoderTools=decoderTools, MaybeMissingROBs=maybeMissingRobs)
    acc.addEventAlgo(decoderAlg, sequenceName='AthAlgSeq')
    
    # Uses SCell to decorate the jTowers
    from L1CaloFEXAlgos.L1CaloFEXAlgosConfig import L1CaloFEXDecoratorCfg
    DecoratorAlgo = L1CaloFEXDecoratorCfg(flags, 'jFexTower2SCellDecorator', True)
    acc.merge(DecoratorAlgo)
   
    
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags, 'AOD', ItemList=outputEDM))
    
    acc.getEventAlgo("EventInfoTagBuilder").PropagateInput = (flags.Input.Format != Format.BS)

    if acc.run().isFailure():
        sys.exit(1)

