
include.block ( "EventOverlayJobTransforms/TruthOverlay_jobOptions.py" )

# McEventCollection copying.  Alghough logically it belongs to
# overlaying that is done later, it has to be scheduled before
# digitization algorithms as a workaround for bug #35465

if OverlayFlags.doBkg():
    from OverlayCommonAlgs.OverlayCommonAlgsConf import DeepCopyObjects   
    job += DeepCopyObjects("BkgRdo")
    job.BkgRdo.EvtInfoObjects = True
    job += DeepCopyObjects("BkgRdo0")
    job.BkgRdo0.TruthObjects = True

from OverlayCommonAlgs.OverlayCommonAlgsConf import CopyMcEventCollection
job += CopyMcEventCollection()
if readBS and isRealData:
    job.CopyMcEventCollection.RealData = True

