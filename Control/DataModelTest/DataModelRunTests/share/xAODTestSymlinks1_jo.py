#
# $Id$
#
# File: DataModelRunTests/share/xAODTestSymlinks1_jo.py
# Author: snyder@bnl.gov
# Date: Apr 2017
# Purpose: Test syminks and hive.
#

## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool



#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 20


#--------------------------------------------------------------
# Set up the algorithm.
#--------------------------------------------------------------

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()
if nThreads >= 1:
  svcMgr.ForwardSchedulerSvc.CheckDependencies = True
  svcMgr.ForwardSchedulerSvc.DataLoaderAlg = 'SGInputLoader'

  from SGComps.SGCompsConf import SGInputLoader
  topSequence += SGInputLoader( OutputLevel=INFO, ShowEventDump=False )


from DataModelTestDataCommon.DataModelTestDataCommonConf import \
     DMTest__xAODTestReadSymlink
from DataModelTestDataWrite.DataModelTestDataWriteConf import \
     DMTest__xAODTestWriteCVec, \
     DMTest__xAODTestWriteCInfo
topSequence += DMTest__xAODTestWriteCVec ("xAODTestWriteCVec")
topSequence += DMTest__xAODTestWriteCInfo ("xAODTestWriteCInfo")
topSequence += DMTest__xAODTestReadSymlink ("xAODTestReadSymlink", Key='cinfo')


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.debugLimit  = 100000
svcMgr.ClassIDSvc.OutputLevel = 3

# No stats printout
ChronoStatSvc = Service( "ChronoStatSvc" )
ChronoStatSvc.ChronoPrintOutTable = FALSE
ChronoStatSvc.PrintUserTime       = FALSE
ChronoStatSvc.StatPrintOutTable   = FALSE
