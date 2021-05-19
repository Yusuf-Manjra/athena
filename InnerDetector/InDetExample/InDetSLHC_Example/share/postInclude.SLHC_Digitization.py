if hasattr(topSequence, 'StreamRDO'):
    outStream = topSequence.StreamRDO
else:
    ## temporary back-compatibility
    outStreams = AlgSequence( "Streams" )
    if hasattr(outStreams, 'StreamRDO'):
        outStream = outStreams.StreamRDO
    else:
        raise AttributeError("AthenaPoolOutputStream not found")

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.CfgGetter import getService, getPublicTool
if DetFlags.digitize.pixel_on():
    outStream.ItemList+=["SiHitCollection#PixelHits"]
    #disable some Pixel stuff
    pixelTool = ServiceMgr.GeoModelSvc.DetectorTools['PixelDetectorTool']
    pixelTool.Alignable = False
    from Digitization.DigitizationFlags import digitizationFlags
    pixeldigi = None
    from PyJobTransforms.trfUtils import releaseIsOlderThan
    if releaseIsOlderThan(19,2):
        if not digitizationFlags.doXingByXingPileUp() and hasattr(job, 'PixelDigitization'):
            if hasattr( job.PixelDigitization, 'DigitizationTool'):
                pixeldigi = job.PixelDigitization.DigitizationTool
            else:
                pixeldigi = job.PixelDigitization
        elif hasattr( ToolSvc, 'PixelDigitizationTool'):
            pixeldigi = ToolSvc.PixelDigitizationTool
        else:
            for alg in job:
                if hasattr(alg, 'PileUpTools'):
                    pixeldigi = alg.PileUpTools[ "PixelDigitizationTool" ]
                    break
                pass
    else:
        pixeldigi = getPublicTool("PixelDigitizationTool")
    if None == pixeldigi:
        raise AttributeError("PixelDigitization(Tool) not found.")
    pixeldigi.PixelTools = []
    pixeldigi.PixelTools += ['PixelDiodeCrossTalkGenerator','PixelChargeSmearer','PixelGangedMerger','PixelRandomDisabledCellGenerator','PixelCellDiscriminator']
    pixeldigi.EnableSpecialPixels = False
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.doStagingStudies():
        print("Staging studies: Allowing missing modules to be ignored in Digi")
        pixeldigi.IgnoreMissingElements = True
    if hasattr(pixeldigi,'UseCalibCondDB'):
        pixeldigi.UseCalibCondDB = False
        pixeldigi.UsePixMapCondDB = False
        pixeldigi.UsePixCondSum = False
        pixeldigi.DisableDistortions = True
    else:
        ## From PixelDigitization-01-00-05 onwards configure tools directly
        from AthenaCommon.CfgGetter import getService, getPublicTool
        getPublicTool("SpecialPixelGenerator").UsePixCondSum = False
        getPublicTool("PixelBarrelChargeTool").DisableDistortions = True
        getPublicTool("PixelECChargeTool").DisableDistortions = True
        getPublicTool("DBMChargeTool").DisableDistortions = True
        getPublicTool("IblPlanarChargeTool").DisableDistortions = True
        getPublicTool("Ibl3DChargeTool").DisableDistortions = True

    ServiceMgr.PixelCalibSvc.DisableDB     = False

    if hasattr(pixeldigi,'OfflineCalibSvc') :
       pixeldigi.OfflineCalibSvc=""

if DetFlags.digitize.SCT_on():

    outStream.ItemList+=["SiHitCollection#SCT_Hits"]

    if 'doStripDigi' in digitizationFlags.experimentalDigi():
        if not hasattr( ToolSvc, 'StripFrontEnd'):
            from StripDigitization.StripDigitizationConf import StripFrontEnd
            ToolSvc += StripFrontEnd("StripFrontEnd")
        getPublicTool("StripDigitizationTool").FrontEnd.UseCalibData = False
        getPublicTool("StripDigitizationTool").FrontEnd.MaxStripsPerSide = 1280
        getPublicTool("StripDigitizationTool").FrontEnd.DataReadOutMode = 0
        getPublicTool("StripDigitizationTool").FrontEnd.DataCompressionMode = 2

    else:
        if not hasattr( ToolSvc, 'SCT_FrontEnd'):
            from SCT_Digitization.SCT_DigitizationConf import SCT_FrontEnd
            ToolSvc += SCT_FrontEnd("SCT_FrontEnd")
        getPublicTool("SCT_DigitizationTool").FrontEnd.UseCalibData = False
        getPublicTool("SCT_DigitizationTool").FrontEnd.MaxStripsPerSide = 1280
        getPublicTool("SCT_DigitizationTool").FrontEnd.DataReadOutMode = 0
        getPublicTool("SCT_DigitizationTool").FrontEnd.DataCompressionMode = 2
 

    if not digitizationFlags.doXingByXingPileUp():
        if DetFlags.pileup.SCT_on():
            #changing the range of the SCT
            from AthenaCommon.AppMgr import ServiceMgr
            from PileUpComps.PileUpCompsConf import PileUpXingFolder
            sctxingFolder = ServiceMgr.PileUpMergeSvc.Intervals['SiliconRange']
            sctxingFolder.FirstXing = -50
            sctxingFolder.LastXing = 25
