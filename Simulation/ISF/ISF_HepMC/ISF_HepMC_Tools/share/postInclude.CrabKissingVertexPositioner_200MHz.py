from AthenaCommon.CfgGetter import getPublicTool
ckvptool = getPublicTool("ISF_CrabKissingVertexPositioner")
# all lengths are in mm
ckvptool.BunchShape = "GAUSS" # GAUSS or FLAT
ckvptool.BunchLength = 150.
ckvptool.Epsilon = 2.5e-3 # normalized emittance
ckvptool.BetaStar = 394. #
ckvptool.AlfaParallel = 6e-3 # kissing angle
ckvptool.AlfaX = 9.6e-3 #rad, crabbing angle
ckvptool.ThetaX = 0. #rad, half crossing angle
ToolSvc.ISF_GenEventVertexPositioner.VertexShifters=[ckvptool]
