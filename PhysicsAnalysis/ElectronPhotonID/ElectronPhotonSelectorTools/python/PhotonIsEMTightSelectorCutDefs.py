# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# default configuration of the PhotonIsEMSelectorCutDefs
# This one is used for stadard Tight photons cuts menus

# Import a needed helper
from PATCore.HelperUtils import *

# Define GeV
GeV = 1000.0

def PhotonIsEMTightSelectorConfigDC14(theTool) :
    '''
    These are the photon isEM definitions for Tight menu
    '''
    
    theTool = GetTool(theTool)

    #
    # PHOTON tight cuts, with updated using *DC14*
    #
    theTool.ConfigFile = "ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMTightSelectorCutDefs.conf"

