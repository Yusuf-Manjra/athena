# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
# LS2_v1.py menu for the long shutdown development
#------------------------------------------------------------------------#

# This defines the input format of the chain and it's properties with the defaults set
# always required are: name, stream and groups
#['name', 'L1chainParts'=[], 'stream', 'groups', 'merging'=[], 'topoStartFrom'=False],
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp
from TriggerMenuMT.HLTMenuConfig.Menu.MenuPrescaleConfig import addSliceChainsToPrescales

def setupMenu():
    from TriggerJobOpts.TriggerFlags import TriggerFlags

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = []

    TriggerFlags.MuonSlice.signatures = [
        ChainProp(name='HLT_mu4_cosmic_L1MU4_EMPTY', l1SeedThresholds=['MU4'], stream=['CosmicMuons'], groups=['RATE:Cosmic_Muon','BW:Muon']),
        ChainProp(name='HLT_mu4_msonly_cosmic_L1MU4_EMPTY', l1SeedThresholds=['MU4'], stream=['CosmicMuons'], groups=['RATE:Cosmic_Muon','BW:Muon']),
     ]

    TriggerFlags.EgammaSlice.signatures = [
        ChainProp(name='HLT_g3_etcut_LArPEB_L1EM3',stream=['LArCells'], groups=['RATE:SinglePhoton', 'BW:Egamma'])
    ]

    TriggerFlags.METSlice.signatures = []
    TriggerFlags.JetSlice.signatures = []
    TriggerFlags.BjetSlice.signatures = [] 
    TriggerFlags.TauSlice.signatures = []
    TriggerFlags.BphysicsSlice.signatures = []
    TriggerFlags.CombinedSlice.signatures = []
    TriggerFlags.HeavyIonSlice.signatures  = []
    TriggerFlags.BeamspotSlice.signatures  = []
    TriggerFlags.MinBiasSlice.signatures   = []
    TriggerFlags.CalibSlice.signatures     = []
    TriggerFlags.CosmicSlice.signatures    = []
    TriggerFlags.StreamingSlice.signatures = []
    TriggerFlags.MonitorSlice.signatures   = []

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = [ ]

    addSliceChainsToPrescales(TriggerFlags, Prescales.HLTPrescales_cosmics)

class Prescales(object):
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = {}

    #   Signature name   | [ HLTprescale, HLTpass-through, rerun]
    #   - Prescale values should be a positive integer (default=1)
    #   - If the current pass_through value is non-zero,
    #     the value given here will be used as pass_through rate
    #     Assuming that pass through chains are configured so
    #     in the slice files and won't change. Also prescale
    #     and pass_through will not be used together.
    #   - If only the first value is specified,
    #     the default value of pass-through (=0) will be used
    #----------------------------------------------------------
    HLTPrescales = {
        }

    L1Prescales_cosmics  = {}
    HLTPrescales_cosmics = {}
    chain_list=[]

