#########################################################
#
# SimulationJobOptions/postInitOptions.PixelCuts20.py
# Zach Marshall
#
# For a special production to evaluate the effect of low
# energy secondaries on Pixel Clustering.
#
#########################################################

atlasG4log.info("G4 PIX Config: Setting PIX cut")

from AthenaCommon.CfgGetter import getPublicTool
pixelRegionTool = getPublicTool('PixelPhysicsRegionTool')
pixelRegionTool.ElectronCut = 0.02
pixelRegionTool.PositronCut = 0.02
pixelRegionTool.GammaCut = 0.02
