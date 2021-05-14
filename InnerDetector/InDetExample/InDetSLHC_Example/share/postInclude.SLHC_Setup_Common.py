##
## postInclude common to all ITk layouts
##

include("PyJobTransforms/UseFrontier.py")

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.DetFlags import DetFlags

from IOVDbSvc.CondDB import conddb
conddb.blockFolder("/Indet/IBLDist")

import MagFieldServices.SetupField

print "Forcing the SiliconConditionsSvc to use GeoModel"
from SiLorentzAngleSvc.LorentzAngleSvcSetup import lorentzAngleSvc
lorentzAngleSvc.forceUseGeoModel()

# fixing the EtaPtFilterTool settings for HL-LHC
from McParticleTools.McParticleToolsConf import EtaPtFilterTool
EtaPtFilterTool.InnerEtaRegionCuts = [0.0,4.2,300]
EtaPtFilterTool.OuterEtaRegionCuts = [4.2,5.5,1000]

if rec.OutputFileNameForRecoStep():
    if rec.doTruth():
      xAODMaker__xAODTruthCnvAlg("GEN_AOD2xAOD",WriteInTimePileUpTruth=True)
    if not hasattr(ServiceMgr, 'PixelConditionsSummarySvc'):
        print "PixelConditionsSummarySvc not found, adding to ServiceMgr"
        from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
        ServiceMgr += PixelConditionsSummarySvc()
    print "Disabling UseSpecialPixelMap"
    ServiceMgr.PixelConditionsSummarySvc.UseSpecialPixelMap = False
    if not hasattr(ServiceMgr, 'PixelOfflineCalibSvc'):
        print "PixelOfflineCalibSvc not found, adding to ServiceMgr"
        from PixelConditionsServices.PixelConditionsServicesConf import PixelOfflineCalibSvc
        ServiceMgr+=PixelOfflineCalibSvc()
    print "Disabling HDCFromCOOL"
    ServiceMgr.PixelOfflineCalibSvc.HDCFromCOOL = False

from InDetRecExample.InDetJobProperties import InDetFlags
if InDetFlags.doStagingStudies:
    print("Staging studies: Allowing missing modules to be ignored in Conditions")
    from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTool
    ToolSvc += PixelCalibDbTool()
    ToolSvc.PixelCalibDbTool.IgnoreMissingElements = True
    
