# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from RecExConfig.Configured import Configured
from AthenaCommon.GlobalFlags  import globalflags
from LArCellRec.LArNoisyROFlags import larNoisyROFlags

from LArCabling.LArCablingAccess import LArOnOffIdMapping

class LArNoisyROSummaryGetter ( Configured )  :
    _outputType = "LArNoisyROSummary"
    _output = { _outputType : "LArNoisyROSummary" }

        
    def configure(self):
        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( 'LArNoisyROSummaryGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream CaloCell object
        import traceback
        try:
            from CaloRec.CaloCellGetter import CaloCellGetter
            theCaloCellGetter = CaloCellGetter()
        except:
            mlog.error("could not get handle to CaloCell Quit")
            traceback.print_exc()
            return False
        if not theCaloCellGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("CaloCellGetter unusable. Quit.")
                return False
            else:
                mlog.error("CaloCellGetter unusable. Continue nevertheless")
         
        LArOnOffIdMapping() # Set up cabling cond algo
  
        # now configure the algorithm
        # cannot have same name
        try:        
            from LArCellRec.LArCellRecConf import LArNoisyROAlg,LArNoisyROTool
        except:
            mlog.error("could not import LArNoisyROAlg or LArNoisyROTool")
            traceback.print_exc()
            return False

        theLArNoisyROTool=LArNoisyROTool(CellQualityCut=larNoisyROFlags.CellQualityCut(),
                                         BadChanPerFEB=larNoisyROFlags.BadChanPerFEB(),
                                         BadFEBCut=larNoisyROFlags.BadFEBCut(),
                                         MNBLooseCut=larNoisyROFlags.MNBLooseCut(),
                                         MNBTightCut=larNoisyROFlags.MNBTightCut(),
                                         MNBTight_PsVetoCut=larNoisyROFlags.MNBTight_PsVetoCut()
                                         )


        theLArNoisyROAlg=LArNoisyROAlg()
        theLArNoisyROAlg.Tool=theLArNoisyROTool

        self._LArNoisyROMakerHandle = theLArNoisyROAlg
        theLArNoisyROAlg.OutputKey=self.outputKey()        
        
        if globalflags.DataSource()=='geant4':
           theLArNoisyROAlg.isMC = True

        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore
        objKeyStore.addStreamESD(self.outputType(),self.outputKey())
        objKeyStore.addTransient(self.outputType(),self.outputKey())
        
        # now add algorithm to topSequence
        # this should always come at the end

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        topSequence += theLArNoisyROAlg
        
        return True

    def LArNoisyROMakerHandle(self):
        return self._LArNoisyROMakerHandle

   
# would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType
