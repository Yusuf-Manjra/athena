# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def PhaseIIPileUp(flags):
    """Phase-II Upgrade / Run 4 flags for MC with pile-up"""
    flags.Beam.NumberOfCollisions = 200.

    flags.GeoModel.Align.Dynamic = False  # no dynamic alignment for now

    from LArConfiguration.LArConfigRun3 import LArConfigRun3PileUp
    LArConfigRun3PileUp(flags)
    flags.LAr.ROD.NumberOfCollisions = 200  # Run 4 default

    flags.Digitization.DoInnerDetectorNoise = False  # disable noise for now

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False

    # pile-up
    flags.Digitization.PU.NumberOfLowPtMinBias = 209.2692
    flags.Digitization.PU.NumberOfHighPtMinBias = 0.725172
    flags.Digitization.PU.BunchStructureConfig = 'RunDependentSimData.BunchStructure_2017'
    flags.Digitization.PU.CustomProfile={
        'run': 242020,
        'startmu': 190.0,
        'endmu': 210.0,
        'stepmu': 1.0,
        'startlb': 1,
        'timestamp': 1412020000
    }


def PhaseIINoPileUp(flags):
    """Phase-II Upgrade / Run 4 flags for MC without pile-up"""
    flags.Beam.NumberOfCollisions = 0.

    flags.GeoModel.Align.Dynamic = False  # no dynamic alignment for now

    from LArConfiguration.LArConfigRun3 import LArConfigRun3NoPileUp
    LArConfigRun3NoPileUp(flags)

    flags.Digitization.DoInnerDetectorNoise = False  # disable noise for now

    flags.Tile.BestPhaseFromCOOL = False
    flags.Tile.correctTime = False
