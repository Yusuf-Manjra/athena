# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.DetFlags import DetFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

if ( DetFlags.detdescr.Muon_on() ):
    from AthenaCommon import CfgGetter
    from AthenaCommon.AppMgr import ToolSvc,ServiceMgr,theApp

    from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
    AGDD2Geo = AGDDtoGeoSvc()

    if not "MuonAGDDTool/MuonSpectrometer" in AGDD2Geo.Builders.__str__():
        AGDD2Geo.Builders += [CfgGetter.getPrivateTool("MuonSpectrometer", checkType=True)]

    if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()):
        if not "NSWAGDDTool/NewSmallWheel" in AGDD2Geo.Builders.__str__():
            AGDD2Geo.Builders += [CfgGetter.getPrivateTool("NewSmallWheel", checkType=True)]

    theApp.CreateSvc += ["AGDDtoGeoSvc"]
    ServiceMgr += AGDD2Geo
