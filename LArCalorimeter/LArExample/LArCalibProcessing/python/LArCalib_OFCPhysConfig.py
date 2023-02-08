# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory 
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from LArCalibProcessing.utils import FolderTagResolver
from IOVDbSvc.IOVDbSvcConfig import addFolders

def _ofcAlg(flags,postfix,folderSuffix,nPhases,dPhases,nDelays,nColl):
    result=ComponentAccumulator()
    LArPhysOFCAlg = CompFactory.LArOFCAlg("LArOFCPhysAlg_"+postfix)
    LArPhysOFCAlg.isSC = flags.LArCalib.isSC
    LArPhysOFCAlg.ReadCaliWave = False
    LArPhysOFCAlg.KeyList      = [ "LArPhysWave" ]
    LArPhysOFCAlg.Normalize    = flags.LArCalib.OFC.Normalize
    LArPhysOFCAlg.Verify       = True
    LArPhysOFCAlg.GroupingType = flags.LArCalib.GroupingType
    LArPhysOFCAlg.Nphase    = nPhases
    LArPhysOFCAlg.Dphase    = dPhases
    LArPhysOFCAlg.Ndelay    = nDelays
    LArPhysOFCAlg.Nsample   = flags.LArCalib.OFC.Nsamples
    LArPhysOFCAlg.FillShape = True
    LArPhysOFCAlg.TimeShift = False
    LArPhysOFCAlg.AddTimeOffset = -1.0*12.0#TimeShiftGuardRegion
    LArPhysOFCAlg.LArPhysWaveBinKey = "LArPhysWaveShift"
    LArPhysOFCAlg.UseDelta = flags.LArCalib.OFC.useDelta
    LArPhysOFCAlg.KeyOFC   = "LArOFC_"+postfix
    LArPhysOFCAlg.KeyShape = "LArShape_"+postfix
    if (nColl==0):
        LArPhysOFCAlg.DecoderTool = CompFactory.LArAutoCorrDecoderTool(UseAlwaysHighGain=flags.LArCalib.PhysACuseHG,
                                                                       isSC = flags.LArCalib.isSC)
    else:
        LArPhysOFCAlg.DecoderTool = CompFactory.LArAutoCorrDecoderTool(DecodeMode=1,
                                                                       UseAlwaysHighGain=flags.LArCalib.PhysACuseHG,
                                                                       isSC = flags.LArCalib.isSC,
                                                                       KeyAutoCorr="LArPhysAutoCorr")
                                                            
    if flags.LArCalib.OFC.Nsamples==4 and not flags.LArCalib.isSC:
        LArPhysOFCAlg.ReadDSPConfig   = True
        LArPhysOFCAlg.DSPConfigFolder = "/LAR/Configuration/DSPConfiguration"
        


    result.addEventAlgo(LArPhysOFCAlg)

    rootfile=flags.LArCalib.Output.ROOTFile
    if rootfile != "":
        OFC2Ntup=CompFactory.LArOFC2Ntuple("LArOFC2Ntuple_"+postfix)
        OFC2Ntup.ContainerKey = "LArOFC_"+postfix
        OFC2Ntup.NtupleName   = "OFC_"+postfix
        OFC2Ntup.AddFEBTempInfo   = False   
        OFC2Ntup.isSC = flags.LArCalib.isSC
        result.addEventAlgo(OFC2Ntup)

        Shape2Ntup=CompFactory.LArShape2Ntuple("LArShape2Ntuple_"+postfix)
        Shape2Ntup.ContainerKey="LArShape_"+postfix
        Shape2Ntup.NtupleName="SHAPE_"+postfix
        Shape2Ntup.AddFEBTempInfo   = False
        Shape2Ntup.isSC = flags.LArCalib.isSC
        result.addEventAlgo(Shape2Ntup)


    rs=FolderTagResolver()
    if nColl > 0:
       tagstr=rs.getFolderTag(flags.LArCalib.OFCPhys.Folder+folderSuffix)
       tagpref=tagstr[0:tagstr.find(folderSuffix)+len(folderSuffix)]
       tagpost=tagstr[tagstr.find(folderSuffix)+len(folderSuffix):]
       nc=int(nColl)
       OFCTag=f'{tagpref}-mu-{nc}{tagpost}'   
    else:
       OFCTag=rs.getFolderTag(flags.LArCalib.OFCPhys.Folder+folderSuffix)
    ShapeTag=rs.getFolderTag(flags.LArCalib.Shape.Folder+folderSuffix)
    del rs #Close database

    from RegistrationServices.OutputConditionsAlgConfig import OutputConditionsAlgCfg
    result.merge(OutputConditionsAlgCfg(flags,name="OutCondAlg"+postfix,
                                        outputFile=flags.LArCalib.Output.POOLFile,
                                        ObjectList=["LArOFCComplete#LArOFC_"+postfix+"#"+flags.LArCalib.OFCPhys.Folder+folderSuffix,
                                                    "LArShapeComplete#LArShape_"+postfix+"#"+flags.LArCalib.Shape.Folder+folderSuffix],
                                        IOVTagList=[OFCTag,ShapeTag]))

    return result


def LArOFCPhysCfg(flags,loadPhysAC=True):

    #Get basic services and cond-algos
    from LArCalibProcessing.LArCalibBaseConfig import LArCalibBaseCfg,chanSelStr
    result=LArCalibBaseCfg(flags)

    nColl=flags.LArCalib.OFC.Ncoll
    from LArCalibProcessing.utils import FolderTagResolver
    FolderTagResolver._globalTag=flags.IOVDb.GlobalTag
    rs=FolderTagResolver()
    PhysWaveTag=rs.getFolderTag(flags.LArCalib.PhysWave.Folder)
    AutoCorrTag=rs.getFolderTag(flags.LArCalib.AutoCorr.Folder)
    PhysAutoCorrTag= rs.getFolderTag(flags.LArCalib.PhysAutoCorr.Folder)
    if (nColl>0):
        #Insert mu in tag-name:
        elems=PhysAutoCorrTag.split("-")
        PhysAutoCorrTag="-".join([elems[0]+"_mu_%i"%nColl,]+elems[1:])
        del elems

    PhysCaliTdiffTag=rs.getFolderTag(flags.LArCalib.PhysCaliTdiff.Folder)
    del rs #Close database

    result.merge(addFolders(flags,flags.LArCalib.PhysWave.Folder,detDb=flags.LArCalib.Input.Database, tag=PhysWaveTag, modifiers=chanSelStr(flags)))
    result.merge(addFolders(flags,flags.LArCalib.AutoCorr.Folder,detDb=flags.LArCalib.Input.Database, tag=AutoCorrTag, modifiers=chanSelStr(flags)))
    if loadPhysAC:
        result.merge(addFolders(flags,flags.LArCalib.PhysAutoCorr.Folder,detDb=flags.LArCalib.Input.Database, tag=PhysAutoCorrTag,modifiers=chanSelStr(flags)))

    
    
    result.merge(addFolders(flags,"/LAR/ElecCalibOfl/OFCBin/PhysWaveShifts","LAR_OFL",tag="LARElecCalibOflOFCBinPhysWaveShifts-UPD3-00"))
    result.merge(addFolders(flags,"/LAR/Configuration/DSPConfiguration","LAR_ONL"))

    if (flags.LArCalib.OFC.UsePhysCalibTDiff):
        result.merge(addFolders(flags,flags.LArCalib.PhysCaliTdiff.Folder,detDb="LAR_OFL", db="COMP200", tag=PhysCaliTdiffTag))


    #def _ofcAlg(flags,postfix,folderSuffix,nPhases,dPhases,nDelays,nColl):
    result.merge(_ofcAlg(flags,"3ns","%isamples3bins17phases"%flags.LArCalib.OFC.Nsamples,nPhases=8,dPhases=3,nDelays=24,nColl=0))

    result.merge(_ofcAlg(flags,"3ns_mu","%isamples3bins17phases"%flags.LArCalib.OFC.Nsamples,nPhases=8,dPhases=3,nDelays=24,nColl=nColl))

    result.merge(_ofcAlg(flags,"1ns","%isamples"%flags.LArCalib.OFC.Nsamples,nPhases=24,dPhases=1,nDelays=24,nColl=0))

    result.merge(_ofcAlg(flags,"1ns_mu","%isamples"%flags.LArCalib.OFC.Nsamples,nPhases=24,dPhases=1,nDelays=24,nColl=nColl))


    #RegistrationSvc    
    result.addService(CompFactory.IOVRegistrationSvc(RecreateFolders = False))
    result.getService("IOVDbSvc").DBInstance=""

    #Ntuple writing
    rootfile=flags.LArCalib.Output.ROOTFile
    if rootfile != "":
        import os
        if os.path.exists(rootfile):
            os.remove(rootfile)
        result.addService(CompFactory.NTupleSvc(Output = [ "FILE1 DATAFILE='"+rootfile+"' OPT='NEW'" ]))
        result.setAppProperty("HistogramPersistency","ROOT")
        pass # end if ROOT ntuple writing


    #MC Event selector since we have no input data file
    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    result.merge(McEventSelectorCfg(flags,
                                    RunNumber         = flags.LArCalib.Input.RunNumbers[0],
                                    EventsPerRun      = 1,
                                    FirstEvent	      = 1,
                                    InitialTimeStamp  = 0,
                                    TimeStampInterval = 1))

    from PerfMonComps.PerfMonCompsConfig import PerfMonMTSvcCfg
    result.merge(PerfMonMTSvcCfg(flags))
    
    return result



if __name__ == "__main__":


    from AthenaConfiguration.AllConfigFlags import initConfigFlags
    ConfigFlags=initConfigFlags()
    from LArCalibProcessing.LArCalibConfigFlags import addLArCalibFlags
    addLArCalibFlags(ConfigFlags)

    ConfigFlags.Input.Files=[]
    ConfigFlags.LArCalib.Input.RunNumbers=[500000]
    ConfigFlags.LArCalib.Input.Database="/home/wlampl/calibTest/00403758_00403761_00403762_EndCap-EMB-EMEC_HIGH_40_21.0.20_1/poolFiles/myDB200_00403758_00403761_00403762_EB-EMECC_one.db"
    ConfigFlags.LArCalib.Input.SubDet="EM"
    ConfigFlags.LArCalib.BadChannelDB="/home/wlampl/calibTest/00403758_00403761_00403762_EndCap-EMB-EMEC_HIGH_40_21.0.20_1/poolFiles/SnapshotBadChannel_00403758_00403761_00403762_EB-EMECC.db"
    ConfigFlags.LArCalib.OFC.Ncoll=20
    ConfigFlags.LArCalib.BadChannelTag="-RUN2-UPD3-00"
    ConfigFlags.LArCalib.Output.ROOTFile="larofc.root"
    ConfigFlags.IOVDb.DatabaseInstance="CONDBR2"
    ConfigFlags.IOVDb.DBConnection="sqlite://;schema=output.sqlite;dbname=CONDBR2"
    ConfigFlags.IOVDb.GlobalTag="LARCALIB-RUN2-02"
    ConfigFlags.GeoModel.AtlasVersion="ATLAS-R3S-2021-03-00-00"
    ConfigFlags.IOVDb.DatabaseInstance="CONDBR2"
    ConfigFlags.LAr.doAlign=False
    ConfigFlags.Input.RunNumber=ConfigFlags.LArCalib.Input.RunNumbers[0]
    #ConfigFlags.Exec.OutputLevel=1
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()
    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(LArOFCPhysCfg(ConfigFlags))

    print(cfg.getService("IOVDbSvc").Folders)
    print("Start running...")
    cfg.run(1)
