###############################################################
#
# Job options file
#
# Based on AthExStoreGateExamples
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
svcMgr += ForwardSchedulerSvc()
#svcMgr.ForwardSchedulerSvc.CheckDependencies = True # fails when views are dynamic

# Use McEventSelector so we can run with AthenaMP
import AthenaCommon.AtlasUnixGeneratorJob

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

viewList = []
for viewNumber in range( 2 ):
	viewList += [ "view" + str( viewNumber ) ]

#Make views
job += CfgMgr.AthViews__ViewSubgraphAlg("make_alg")
job.make_alg.ViewNames = viewList
for viewName in viewList:
	job.make_alg.ExtraOutputs += [ ( 'int', viewName + '_view_start' ) ]

#Make one view
job += CfgMgr.AthViews__DFlowAlg1("dflow_alg1")
job.dflow_alg1.RequireView = True
#
job += CfgMgr.AthViews__DFlowAlg2("dflow_alg2")
job.dflow_alg2.RequireView = True
#
job += CfgMgr.AthViews__DFlowAlg3("dflow_alg3")
job.dflow_alg3.RequireView = True

#Merge views
job += CfgMgr.AthViews__ViewMergeAlg("merge_alg")
for viewName in viewList:
	job.merge_alg.ExtraInputs += [ ( 'int', viewName + '_dflow_dummy' ) ]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 10

#==============================================================
#
# End of job options file
#
###############################################################

