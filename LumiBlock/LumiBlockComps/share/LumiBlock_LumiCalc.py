# add LumiBlockMetaDataTool to ToolSvc and configure
from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
ToolSvc += LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )
LumiBlockMetaDataTool.OutputLevel = INFO

# add ToolSvc.LumiBlockMetaDataTool to MetaDataSvc
from AthenaServices.AthenaServicesConf import MetaDataSvc
svcMgr += MetaDataSvc( "MetaDataSvc" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.LumiBlockMetaDataTool ]

# add LumiCalcSvc to ServiceMgr and configure
from LumiBlockComps.LumiBlockCompsConf import LumiCalcSvc
LumiCalcSvc = LumiCalcSvc()
LumiCalcSvc.Triggers = ["EF_mu20"]
LumiCalcSvc.UseMC = True
LumiCalcSvc.LBCollNames = ["LumiBlocks", "IncompleteLumiBlocks"]
LumiCalcSvc.Verbose = False
svcMgr += LumiCalcSvc
