# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory 
from AthenaConfiguration.MainServicesConfig import MainServicesCfg

def LArDelay_OFCCaliCfg(flags):

    #Get basic services and cond-algos
    from LArCalibProcessing.LArCalibBaseConfig import LArCalibBaseCfg
    result=LArCalibBaseCfg(flags)

    #Calibration runs are taken in fixed gain. 
    #The SG key of the digit-container is name of the gain
    gainStrMap={0:"HIGH",1:"MEDIUM",2:"LOW"}
    digKey=gainStrMap[flags.LArCalib.Gain]

    from LArCalibProcessing.utils import FolderTagResolver
    FolderTagResolver._globalTag=flags.LArCalib.GlobalTag
    tagResolver=FolderTagResolver()
    pedestalTag=tagResolver.getFolderTag(flags.LArCalib.Pedestal.Folder)
    caliWaveTag=tagResolver.getFolderTag(flags.LArCalib.CaliWave.Folder)
    caliOFCTag=tagResolver.getFolderTag(flags.LArCalib.OFCCali.Folder)
    acTag=tagResolver.getFolderTag(flags.LArCalib.AutoCorr.Folder)
    del tagResolver
    
    print("pedestalTag",pedestalTag)
    print("acTag",acTag)


    from IOVDbSvc.IOVDbSvcConfig import addFolders
    result.merge(addFolders(flags,flags.LArCalib.Pedestal.Folder,detDb=flags.LArCalib.Input.Database, tag=pedestalTag, className="LArPedestalComplete"))
    result.merge(addFolders(flags,flags.LArCalib.AutoCorr.Folder,detDb=flags.LArCalib.Input.Database, tag=acTag))
    

    result.addEventAlgo(CompFactory.LArRawCalibDataReadingAlg(LArAccCalibDigitKey=digKey,
                                                              LArFebHeaderKey="LArFebHeader",
                                                              PosNegPreselection=flags.LArCalib.Preselection.Side,
                                                              BEPreselection=flags.LArCalib.Preselection.BEC,
                                                              FTNumPreselection=flags.LArCalib.Preselection.FT))
    
    from LArROD.LArFebErrorSummaryMakerConfig import LArFebErrorSummaryMakerCfg
    result.merge(LArFebErrorSummaryMakerCfg(flags))
    result.getEventAlgo("LArFebErrorSummaryMaker").CheckAllFEB=False

    
    if flags.LArCalib.Input.SubDet == "EM":
        from LArCalibProcessing.LArStripsXtalkCorrConfig import LArStripsXtalkCorrCfg
        result.merge(LArStripsXtalkCorrCfg(flags,[digKey,]))
    
    
        theLArCalibShortCorrector = CompFactory.LArCalibShortCorrector(KeyList = [digKey,])
        result.addEventAlgo(theLArCalibShortCorrector)


    theLArCaliWaveBuilder = CompFactory.LArCaliWaveBuilder()
    theLArCaliWaveBuilder.KeyList= [digKey,]
    theLArCaliWaveBuilder.KeyOutput="LArCaliWave"
    theLArCaliWaveBuilder.GroupingType     = flags.LArCalib.GroupingType
    theLArCaliWaveBuilder.SubtractPed      = True
    theLArCaliWaveBuilder.CheckEmptyPhases = True
    theLArCaliWaveBuilder.NBaseline        = 0 # to avoid the use of the baseline when Pedestal are missing
    theLArCaliWaveBuilder.UseDacAndIsPulsedIndex = False # should have an impact only for HEC
    theLArCaliWaveBuilder.RecAllCells      = False
    theLArCaliWaveBuilder.isSC       = flags.LArCalib.isSC
    result.addEventAlgo(theLArCaliWaveBuilder)
    
    

    LArCaliOFCAlg = CompFactory.LArOFCAlg("LArCaliOFCAlg")
    LArCaliOFCAlg.ReadCaliWave = True
    LArCaliOFCAlg.KeyList   = [ "LArCaliWave" ]
    LArCaliOFCAlg.Nphase    = 50
    LArCaliOFCAlg.Dphase    = 1
    LArCaliOFCAlg.Ndelay    = 24
    LArCaliOFCAlg.Nsample   = 4
    LArCaliOFCAlg.Normalize = True
    LArCaliOFCAlg.TimeShift = False
    LArCaliOFCAlg.TimeShiftByIndex = -1
    LArCaliOFCAlg.Verify    = True
    LArCaliOFCAlg.FillShape = False
    #LArCaliOFCAlg.DumpOFCfile = "LArOFCCali.dat"
    LArCaliOFCAlg.GroupingType = flags.LArCalib.GroupingType
    LArCaliOFCAlg.isSC = flags.LArCalib.isSC
    LArCaliOFCAlg.DecoderTool=CompFactory.LArAutoCorrDecoderTool(isSC=flags.LArCalib.isSC)
    result.addEventAlgo(LArCaliOFCAlg)


    #ROOT ntuple writing:
    rootfile=flags.LArCalib.Output.ROOTFile
    if rootfile != "":
        result.addEventAlgo(CompFactory.LArCaliWaves2Ntuple(KeyList = ["LArCaliWave",],
                                                            AddFEBTempInfo = False
                                                        ))

        result.addEventAlgo(CompFactory.LArOFC2Ntuple(ContainerKey = "LArOFC",
                                                      AddFEBTempInfo  = False
                                                  ))

        import os
        if os.path.exists(rootfile):
            os.remove(rootfile)
        result.addService(CompFactory.NTupleSvc(Output = [ "FILE1 DATAFILE='"+rootfile+"' OPT='NEW'" ]))
        result.setAppProperty("HistogramPersistency","ROOT")
        pass # end if ROOT ntuple writing



    #Get the current folder tag by interrogating the database:
    from LArCalibProcessing.utils import FolderTagResolver
    tagResolver=FolderTagResolver()
    caliWaveTag=tagResolver.getFolderTag(flags.LArCalib.CaliWave.Folder)
    caliOFCTag=tagResolver.getFolderTag(flags.LArCalib.OFCCali.Folder)
    del tagResolver


    #Output (POOL + sqlite) file writing:
    from RegistrationServices.OutputConditionsAlgConfig import OutputConditionsAlgCfg
    result.merge(OutputConditionsAlgCfg(flags,
                                        outputFile=flags.LArCalib.Output.POOLFile,
                                        ObjectList=["LArCaliWaveContainer#LArCaliWave#"+flags.LArCalib.CaliWave.Folder,
                                                    "LArOFCComplete#LArOFC#"+flags.LArCalib.OFCCali.Folder],
                                        IOVTagList=[caliWaveTag,caliOFCTag]
                                    ))

    #RegistrationSvc    
    result.addService(CompFactory.IOVRegistrationSvc(RecreateFolders = False))


    result.getService("IOVDbSvc").DBInstance=""

    return result


if __name__ == "__main__":


    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from LArCalibProcessing.LArCalibConfigFlags import addLArCalibFlags
    addLArCalibFlags(ConfigFlags)


    ConfigFlags.LArCalib.Input.Dir = "/scratch/wlampl/calib21/Aug27"
    ConfigFlags.LArCalib.Input.Type="calibration_LArElec-Delay"
    ConfigFlags.LArCalib.Input.RunNumbers=[400268,]
    ConfigFlags.LArCalib.Input.Database="db.sqlite"
    ConfigFlags.Input.Files=ConfigFlags.LArCalib.Input.Files
    ConfigFlags.LArCalib.Preselection.BEC=[1]
    ConfigFlags.LArCalib.Preselection.Side=[0]


    ConfigFlags.LArCalib.Output.ROOTFile="ofccali.root"

    ConfigFlags.IOVDb.DBConnection="sqlite://;schema=output.sqlite;dbname=CONDDBR2"
    ConfigFlags.IOVDb.GlobalTag="LARCALIB-RUN2-02"
    #ConfigFlags.Exec.OutputLevel=1
    print ("Input files to be processed:")
    for f in ConfigFlags.Input.Files:
        print (f)

    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(LArDelay_OFCCaliCfg(ConfigFlags))
    print("Start running...")
    cfg.run()

