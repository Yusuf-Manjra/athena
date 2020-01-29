from __future__ import print_function

from LArROD.LArRODFlags import larRODFlags
from AthenaCommon.GlobalFlags import globalflags
from LArByteStream.LArByteStreamConf import LArRawDataReadingAlg

def LArRawChannelBuilderDefault():
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr

    if larRODFlags.readDigits() and globalflags.InputFormat() == 'bytestream':
        if LArRawDataReadingAlg() not in topSequence:
            print ("Adding LArRawDataReaderAlg")
            topSequence+=LArRawDataReadingAlg()

        from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault

        LArADC2MeVCondAlgDefault()

        from LArROD.LArRODConf import LArRawChannelBuilderAlg
        theLArRawChannelBuilder=LArRawChannelBuilderAlg()
        if larRODFlags.keepDSPRaw():
            theLArRawChannelBuilder.LArRawChannelKey=larRODFlags.RawChannelFromDigitsContainerName()
        topSequence += theLArRawChannelBuilder

        #Useless here but for backward compatiblity
        #from AthenaCommon.AppMgr import ToolSvc
        #from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
        #theADC2MeVTool = LArADC2MeVToolDefault()
        #ToolSvc += theADC2MeVTool
