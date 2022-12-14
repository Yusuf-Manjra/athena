#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    infile = 'aod/AOD-16.6.3/AOD-16.6.3-full.pool.root'

    keys = [
        #Rec::TrackParticleContainer_tlp1
        'HLT_InDetTrigParticleCreationIOTRT_CosmicsN_EFID',
        'HLT_InDetTrigParticleCreation_CosmicsN_EFID',
        'HLT_InDetTrigParticleCreation_Photon_EFID',
        'MuTagTrackParticles',
        'HLT_InDetTrigParticleCreationTRTOnly_Muon_EFID',
        'MuidExtrTrackParticles',
        'MuidCombTrackParticles',
        'MuGirlRefittedTrackParticles',
        'MuonboyTrackParticles',
        'MuonboyMuonSpectroOnlyTrackParticles',
        'HLT_InDetTrigParticleCreation_Muon_EFID',
        'MuTagIMOTrackParticles',
        'StacoTrackParticles',
        'HLT_InDetTrigParticleCreation_Bphysics_EFID',
        'MooreTrackParticles',
        'HLT_InDetTrigParticleCreationCombined_Electron_EFID',
        'HLT_InDetTrigParticleCreationTRTOnly_Electron_EFID',
        'HLT_InDetTrigParticleCreation_Bjet_EFID',
        'AtlfastTrackParticles',
        'HLT_InDetTrigParticleCreationTRTOnly_Tau_EFID',
        'HLT_InDetTrigParticleCreation_Tau_EFID',
        'HLT_InDetTrigParticleCreationTRTOnly_FullScan_EFID',
        'HLT_InDetTrigParticleCreation_FullScan_EFID',
        'HLT_InDetTrigParticleCreation_Electron_EFID',
        'TrackParticleCandidate',
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True)

