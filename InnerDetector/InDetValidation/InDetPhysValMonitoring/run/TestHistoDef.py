# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#--------------------------------------------------------------
# use auditors
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr

from GaudiSvc.GaudiSvcConf import AuditorSvc
from AthenaCommon.AppMgr import theApp

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True


#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = "ATLAS-GEO-20-00-01"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"
print globalflags



import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit


#--------------------------------------------------------------
# Load ReadCalibData Alg and Service
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/test.hdef"

#IOVDbSvc = Service("IOVDbSvc")
#from IOVDbSvc.CondDB import conddb


from InDetPhysValMonitoring.InDetPhysValMonitoringConf import AlgTestHistoDefSvc
topSequence+= AlgTestHistoDefSvc()

#--------------------------------------------------------------
# Event selector settings. Use McEventSelector
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

theApp.EvtMax                    = 1


#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = WARNING
ServiceMgr.HistogramDefinitionSvc.OutputLevel = INFO
topSequence.AlgTestHistoDefSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.infoLimit        = 100000

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
#IOVDbSvc.GlobalTag='OFLCOND-RUN12-SDR-25'
#IOVDbSvc.OutputLevel = DEBUG


