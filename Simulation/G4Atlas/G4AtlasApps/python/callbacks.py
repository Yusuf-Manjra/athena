# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Collection of standard callback functions to be used in the sim initialisation, e.g.

from G4AtlasApps import callbacks
SimFlags.InitFunctions.add_function("postInit", callbacks.use_classicalrk4_stepper)
"""


## Change the field stepper
def use_simplerunge_stepper():
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng._ctrl.fldMenu.SetDefaultStepper('SimpleRunge')

def use_classicalrk4_stepper():
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng._ctrl.fldMenu.SetDefaultStepper('ClassicalRK4')

def use_nystromrk4_stepper():
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng._ctrl.fldMenu.SetDefaultStepper('NystromRK4')

## Use verbose G4 tracking
def use_verbose_tracking():
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng.gbl.G4Commands().tracking.verbose(1)

# Add a truth catch for LLP decay processes
def add_LLP_truth_strategies():
    from G4AtlasApps import AtlasG4Eng,PyG4Atlas
    astrategy=PyG4Atlas.MCTruthStrg('G4TruthStrategies', 'LLP','IDET::IDET',1)
    astrategy.add_Volumes('CALO::CALO',1)
    astrategy.add_Volumes('BeamPipe::BeamPipe',1)
    mcTruthMenu=PyG4Atlas.G4AtlasEngine.menu_MCTruth()
    mcTruthMenu.add_McTruthStrategy(astrategy)

