# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from enum import Enum

class ProductionStep(Enum):
    # steps should be added when needed
    Default = 'Default'
    Simulation = 'Simulation'
    PileUpPresampling = 'PileUpPresampling'
    Overlay = 'Overlay'
    FastChain = 'FastChain'
    Digitization = 'Digitization'
