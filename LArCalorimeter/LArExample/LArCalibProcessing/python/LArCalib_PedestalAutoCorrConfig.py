# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory 

def LArPedestalAutoCorrCfg(flags):

    #Get basic services and cond-algos
    from LArCalibProcessing.LArCalibBaseConfig import LArCalibBaseCfg
    result=LArCalibBaseCfg(flags)

    #Calibration runs are taken in fixed gain. 
    #The SG key of the digit-container is name of the gain
    gainStrMap={0:"HIGH",1:"MEDIUM",2:"LOW"}
    digKey=gainStrMap[flags.LArCalib.Gain]

    result.addEventAlgo(CompFactory.LArRawCalibDataReadingAlg(LArAccDigitKey=digKey,
                                                              LArFebHeaderKey="LArFebHeader",
                                                              FailOnCorruption=False))

    from LArROD.LArFebErrorSummaryMakerConfig import LArFebErrorSummaryMakerCfg
    result.merge(LArFebErrorSummaryMakerCfg(flags))
    result.getEventAlgo("LArFebErrorSummaryMaker").CheckAllFEB=False
    result.addEventAlgo(CompFactory.LArBadEventCatcher(CheckAccCalibDigitCont=True,
                                                       CheckBSErrors=True,
                                                       KeyList=[digKey,],
                                                       StopOnError=False))

    LArPedACBuilder=CompFactory.LArPedestalAutoCorrBuilder()
    LArPedACBuilder.KeyList         = [digKey,]
    LArPedACBuilder.PedestalKey     = "Pedestal" 
    LArPedACBuilder.AutoCorrKey     = "LArAutoCorr"
    LArPedACBuilder.GroupingType    = flags.LArCalib.GroupingType

    result.addEventAlgo(LArPedACBuilder)

    #ROOT ntuple writing:
    rootfile=flags.LArCalib.Output.ROOTFile
    if rootfile != "":
        result.addEventAlgo(CompFactory.LArPedestals2Ntuple(ContainerKey = "Pedestal",
                                                            AddFEBTempInfo = False
                                                        )
                        )

        result.addEventAlgo(CompFactory.LArAutoCorr2Ntuple(ContainerKey = "LArAutoCorr",
                                                           AddFEBTempInfo  = False
                                                       )
                        )

        import os
        if os.path.exists(rootfile):
            os.remove(rootfile)
        result.addService(CompFactory.NTupleSvc(Output = [ "FILE1 DATAFILE='"+rootfile+"' OPT='NEW'" ]))
        result.setAppProperty("HistogramPersistency","ROOT")
        pass # end if ROOT ntuple writing



    #Get the current folder tag by interrogating the database:
    from LArCalibProcessing.utils import FolderTagResolver
    tagResolver=FolderTagResolver()
    pedestalTag=tagResolver.getFolderTag(flags.LArCalib.Pedestal.Folder)
    autocorrTag=tagResolver.getFolderTag(flags.LArCalib.AutoCorr.Folder)
    del tagResolver


    #Output (POOL + sqlite) file writing:
    from RegistrationServices.OutputConditionsAlgConfig import OutputConditionsAlgCfg
    result.merge(OutputConditionsAlgCfg(flags,
                                        outputFile=flags.LArCalib.Output.POOLFile,
                                        ObjectList=["LArPedestalComplete#Pedestal#"+flags.LArCalib.Pedestal.Folder,
                                                    "LArAutoCorrComplete#LArAutoCorr#"+flags.LArCalib.AutoCorr.Folder],
                                        IOVTagList=[pedestalTag,autocorrTag]
                                    ))

    #RegistrationSvc    
    result.addService(CompFactory.IOVRegistrationSvc(RecreateFolders = False))


    #Validation (comparision with reference):
    if flags.LArCalib.doValidation:
        from IOVDbSvc.IOVDbSvcConfig import addFolders 

        result.merge(addFolders(flags,"/LAR/ElecCalibOnl/Pedestal<key>PedestalRef</key>","LAR_ONL"))


        from LArCalibDataQuality.Thresholds import pedThr,rmsThr, pedThrFEB,rmsThrFEB        
        from AthenaCommon.Constants import ERROR,WARNING

        thePedestalValidationAlg=CompFactory.LArPedestalValidationAlg("PedestalVal")
        #thePedestalValidationAlg.BadChannelMaskingTool=theLArPedValBCMask
        thePedestalValidationAlg.UseBadChannelInfo=False
        thePedestalValidationAlg.ValidationKey="Pedestal"
        thePedestalValidationAlg.ReferenceKey="PedestalRef"
        thePedestalValidationAlg.PedestalTolerance=pedThr
        thePedestalValidationAlg.PedestalRMSTolerance=rmsThr
        thePedestalValidationAlg.PedestalToleranceFEB=pedThrFEB
        thePedestalValidationAlg.PedestalRMSToleranceFEB=rmsThrFEB
        thePedestalValidationAlg.MsgLevelForDeviations=WARNING
        thePedestalValidationAlg.ListOfDevFEBs="pedFebs.txt"
        thePedestalValidationAlg.CheckCompletness=True
        thePedestalValidationAlg.PatchMissingFEBs=True
        thePedestalValidationAlg.CheckNumberOfCoolChannels=False
        thePedestalValidationAlg.UseCorrChannels=False #Corrections go into the regular data channels
        result.addEventAlgo(thePedestalValidationAlg)

        ## second instance of the validation tool to detect "bad" channel
        theBadPedestal=CompFactory.LArPedestalValidationAlg("PedestalFail")
        #theBadPedestal.BadChannelMaskingTool=theLArPedValBCMask
        theBadPedestal.UseBadChannelInfo=False
        theBadPedestal.ValidationKey="Pedestal"
        theBadPedestal.ReferenceKey="PedestalRef"
        theBadPedestal.PedestalTolerance       = ["10,10,10"]
        theBadPedestal.PedestalRMSTolerance    = ["0.5, 0.5, 0.5"]
        theBadPedestal.PedestalToleranceFEB    = ["8, 8, 8"]
        theBadPedestal.PedestalRMSToleranceFEB = ["1.2, 1.2, 1.2"]

        theBadPedestal.MsgLevelForDeviations=ERROR      
        theBadPedestal.CheckCompletness=False
        theBadPedestal.CheckNumberOfCoolChannels=False
        theBadPedestal.ListOfDevFEBs="Bad_pedFebs.txt"
        result.addEventAlgo(theBadPedestal)


        result.merge(addFolders(flags,"/LAR/ElecCalibOfl/AutoCorrs/AutoCorr<key>LArAutoCorrRef</key>","LAR_OFL"))
        from LArCalibDataQuality.Thresholds import acThr, acThrFEB
        theAutoCorrValidationAlg=CompFactory.LArAutoCorrValidationAlg("AutoCorrVal")
        theAutoCorrValidationAlg.UseBadChannelInfo=False
        theAutoCorrValidationAlg.ValidationKey="LArAutoCorr"
        theAutoCorrValidationAlg.ReferenceKey="LArAutoCorrRef"
        theAutoCorrValidationAlg.AutoCorrTolerance=acThr
        theAutoCorrValidationAlg.AutoCorrToleranceFEB=acThrFEB
        theAutoCorrValidationAlg.MsgLevelForDeviations=WARNING
        theAutoCorrValidationAlg.ListOfDevFEBs="ACFebs.txt"
        theAutoCorrValidationAlg.CheckCompletness=True
        theAutoCorrValidationAlg.PatchMissingFEBs=True
        theAutoCorrValidationAlg.CheckNumberOfCoolChannels=False
        theAutoCorrValidationAlg.UseCorrChannels=False #Corrections go into the regular data channels
        result.addEventAlgo(theAutoCorrValidationAlg)
      
        ## second instance of the validation tool to detect "bad" channel     
        theBadAutoCorr=CompFactory.LArAutoCorrValidationAlg("AutoCorrFail")
        theBadAutoCorr.UseBadChannelInfo=False
        theBadAutoCorr.ValidationKey="LArAutoCorr"
        theBadAutoCorr.ReferenceKey="LArAutoCorrRef"
        theBadAutoCorr.AutoCorrTolerance    = ["0.15, 0.15, 0.15"]
        theBadAutoCorr.AutoCorrToleranceFEB = ["0.15, 0.15, 0.15"]
        theBadAutoCorr.MsgLevelForDeviations=ERROR
        theBadAutoCorr.CheckFifthSample=True
        theBadAutoCorr.ListOfDevFEBs="Bad_ACFebs.txt"
        theBadAutoCorr.CheckCompletness=False
        theBadAutoCorr.CheckNumberOfCoolChannels=False
        result.addEventAlgo(theBadAutoCorr)

        result.getService("IOVDbSvc").DBInstance=""

    return result


if __name__ == "__main__":

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from LArCalibProcessing.LArCalibConfigFlags import addLArCalibFlags
    addLArCalibFlags(ConfigFlags)


    ConfigFlags.LArCalib.Input.Dir = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/LArCalibProcessing"
    ConfigFlags.LArCalib.Input.Type="calibration_LArElec-Pedestal"
    ConfigFlags.LArCalib.Input.RunNumbers=[174585,]

    ConfigFlags.Input.Files=ConfigFlags.LArCalib.Input.Files
    
    ConfigFlags.LArCalib.Output.ROOTFile="ped.root"

    ConfigFlags.IOVDb.DBConnection="sqlite://;schema=output.sqlite;dbname=CONDBR2"
    ConfigFlags.IOVDb.GlobalTag="LARCALIB-000-02"

    print ("Input files to be processed:")
    for f in ConfigFlags.Input.Files:
        print (f)

    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(LArPedestalAutoCorrCfg(ConfigFlags))

    print("Start running...")

    cfg.run()
