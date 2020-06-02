# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileRawChannel creation from TileDigits 
# TileRawChannelMaker algorithm using

from AthenaCommon.Logging import logging
import traceback

from RecExConfig.Configured import Configured
#from TileRecUtils.TileRawChannelMakerBase import TileRawChannelMakerBase 

class TileRawChannelGetter_DigiHSTruth ( Configured)  :
    """ This getter module creates an instance of the TileRawChannelMaker
    algorithm to obtain TileRawChannel objects from the TileDigits stored
    in a TileDigitsContainer.
    According to the values of the TileRecFlags jobProperties, the
    corresponding AlgTools are added to TileRawChannelMaker algorithm.
    """
    
    _outputType = "TileRawChannelContainer"
    # _output = { _outputType : "TileRawChannelFit" }

    def configure(self):

        mlog = logging.getLogger( 'TileRawChannelGetter::configure:' )
        mlog.info ("entering")        
        
        # Instantiation of the C++ algorithm
        try:        
            from TileRecUtils.TileRecUtilsConf import TileRawChannelMaker               
            theTileRawChannelMaker = TileRawChannelMaker("TileRChMaker_DigiHSTruth")
        except Exception:
            mlog.error("could not import TileRecUtils.TileRawChannelMaker")
            traceback.print_exc()
            return False
    
        self._TileRChMaker_DigiHSTruth = theTileRawChannelMaker

        # Configure TileInfoLoader
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr( ServiceMgr, "TileInfoLoader" ):
            from TileConditions.TileInfoConfigurator import TileInfoConfigurator
            tileInfoConfigurator = TileInfoConfigurator()

        from TileRecUtils.TileRecFlags import jobproperties

        # set time window for amplitude correction if it was not set correctly before
        if jobproperties.TileRecFlags.TimeMaxForAmpCorrection() <= jobproperties.TileRecFlags.TimeMinForAmpCorrection() :
            from AthenaCommon.BeamFlags import jobproperties
            mlog.info("adjusting min/max time of parabolic correction for %s", jobproperties.Beam.bunchSpacing)
            halfBS = jobproperties.Beam.bunchSpacing.get_Value()/2.
            if halfBS > 25.1:
                mlog.info("Bunch spacing is too big, keeping default limits for parabolic correction")
            else:
                jobproperties.TileRecFlags.TimeMinForAmpCorrection = -halfBS
                jobproperties.TileRecFlags.TimeMaxForAmpCorrection = halfBS

        NoiseFilterTools = []
        if jobproperties.TileRecFlags.noiseFilter() == 1:
            from TileRecUtils.TileRecUtilsConf import TileRawChannelNoiseFilter
            theTileRawChannelNoiseFilter = TileRawChannelNoiseFilter()
            NoiseFilterTools += [theTileRawChannelNoiseFilter]

        TileFrameLength = ServiceMgr.TileInfoLoader.NSamples
        if TileFrameLength!=7:
            mlog.info("disabling reading of OFC from COOL because Nsamples!=7")
            jobproperties.TileRecFlags.OfcFromCOOL = False

        jobproperties.TileRecFlags.print_JobProperties('tree&value')
        
        # run optimal filter only if readDigits is set
        if jobproperties.TileRecFlags.readDigits():
            if jobproperties.TileRecFlags.doTileOptATLAS():
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderOpt2Filter
                    theTileRawChannelBuilderOptATLAS= TileRawChannelBuilderOpt2Filter("TileRawChannelBuilderOptATLAS_DigiHSTruth")
                except Exception:
                    mlog.error("could not get handle to TileRawChannelBuilderOpt2Filter Quit")
                    traceback.print_exc()
                    return False
                
                # setup COOL to get OFCs
                toolOfcCool = None
                if jobproperties.TileRecFlags.OfcFromCOOL():
                    from TileConditions.TileInfoConfigurator import TileInfoConfigurator
                    tileInfoConfigurator = TileInfoConfigurator()
                    tileInfoConfigurator.setupCOOLOFC()
                    from TileConditions.TileCondToolConf import getTileCondToolOfcCool
                    toolOfcCool = getTileCondToolOfcCool('COOL')
                else:
                    from TileConditions.TileInfoConfigurator import TileInfoConfigurator
                    tileInfoConfigurator = TileInfoConfigurator()
                    tileInfoConfigurator.setupCOOLPHYPULSE()
                    tileInfoConfigurator.setupCOOLAutoCr()
                    
                #TileRawChannelBuilderOptATLAS Options:
                theTileRawChannelBuilderOptATLAS.TileRawChannelContainer = "TileRawChannelCnt_DigiHSTruth"
                theTileRawChannelBuilderOptATLAS.RunType         = jobproperties.TileRecFlags.TileRunType()
                theTileRawChannelBuilderOptATLAS.calibrateEnergy = jobproperties.TileRecFlags.calibrateEnergy()
                if jobproperties.TileRecFlags.BestPhaseFromCOOL(): # can't correct time and use best phase at the same time
                    theTileRawChannelBuilderOptATLAS.correctTime = False
                else:
                    theTileRawChannelBuilderOptATLAS.correctTime = jobproperties.TileRecFlags.correctTime()
                theTileRawChannelBuilderOptATLAS.BestPhase       = jobproperties.TileRecFlags.BestPhaseFromCOOL()
                theTileRawChannelBuilderOptATLAS.NoiseFilterTools= NoiseFilterTools
                theTileRawChannelBuilderOptATLAS.OF2 = True
                #theTileRawChannelBuilderOptATLAS.PedestalMode = 1 # not sure if we need this option here
                theTileRawChannelBuilderOptATLAS.MaxIterations = 1 # just one iteration
                theTileRawChannelBuilderOptATLAS.Minus1Iteration = False # assume that max sample is at t=0
                theTileRawChannelBuilderOptATLAS.AmplitudeCorrection = jobproperties.TileRecFlags.correctAmplitude()
                if jobproperties.TileRecFlags.OfcFromCOOL():
                    theTileRawChannelBuilderOptATLAS.TileCondToolOfc = toolOfcCool

                theTileRawChannelBuilderOptATLAS.AmpMinForAmpCorrection = jobproperties.TileRecFlags.AmpMinForAmpCorrection()
                if jobproperties.TileRecFlags.TimeMaxForAmpCorrection() > jobproperties.TileRecFlags.TimeMinForAmpCorrection():
                    theTileRawChannelBuilderOptATLAS.TimeMinForAmpCorrection = jobproperties.TileRecFlags.TimeMinForAmpCorrection()
                    theTileRawChannelBuilderOptATLAS.TimeMaxForAmpCorrection = jobproperties.TileRecFlags.TimeMaxForAmpCorrection()
                
                mlog.info(" adding now TileRawChannelBuilderOpt2Filter with name TileRawChannelBuilderOptATLAS to the aglorithm: %s",
                          theTileRawChannelMaker.name())

                theTileRawChannelMaker.TileRawChannelBuilder += [theTileRawChannelBuilderOptATLAS]
            

            #jobproperties.TileRecFlags.print_JobProperties('tree&value')

            # now add algorithm to topSequence
            # this should always come at the end

            mlog.info(" now adding to topSequence")        
            from AthenaCommon.AlgSequence import AlgSequence
            topSequence = AlgSequence()
            theTileRawChannelMaker.TileDigitsContainer = "TileDigitsCnt_DigiHSTruth"

            if jobproperties.TileRecFlags.noiseFilter() == 2:
                # Instantiation of the C++ algorithm
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawCorrelatedNoise
                    theTileRawCorrelatedNoise=TileRawCorrelatedNoise("TileRCorreNoise")
                except Exception:
                    mlog.error("could not import TileRecUtils.TileRawCorrelatedNoise")
                    traceback.print_exc()
                    return False
                #theTileRawCorrelatedNoise.UseMeanFiles = False
                #theTileRawCorrelatedNoise.PMTOrder = True
                theTileRawChannelMaker.TileDigitsContainer = "NewDigitsContainer"
                topSequence += theTileRawCorrelatedNoise

            topSequence += theTileRawChannelMaker

        else:
            mlog.info(" Disable all OF methods because readDigits flag set to False ")
            jobproperties.TileRecFlags.doTileFlat = False
            jobproperties.TileRecFlags.doTileFit = False
            jobproperties.TileRecFlags.doTileFitCool = False
            jobproperties.TileRecFlags.doTileOpt2 = False
            jobproperties.TileRecFlags.doTileOptATLAS = False
            jobproperties.TileRecFlags.doTileManyAmps = False
            jobproperties.TileRecFlags.doTileMF = False
            jobproperties.TileRecFlags.doTileOF1 = False
            jobproperties.TileRecFlags.doTileWiener = False
            jobproperties.TileRecFlags.OfcFromCOOL = False
            jobproperties.TileRecFlags.print_JobProperties('tree&value')

        return True

    def TileRChMaker_DigiHSTruth(self):
        return self._TileRChMaker_DigiHSTruth
   
##    # would work only if one output object type
##    def outputKey(self):
##        return self._output[self._outputType]

##    def outputType(self):
##        return self._outputType


