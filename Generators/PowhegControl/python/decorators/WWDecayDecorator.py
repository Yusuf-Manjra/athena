# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl WWDecayDecorator
#  Powheg runcard decorator for WW decays
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from AthenaCommon import Logging

logger = Logging.logging.getLogger("PowhegControl")


class WWDecayDecorator(object):

    # Define decorator name string
    name = "WW decay"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated, boson):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.allowed_decay_modes = []
        self.decorated.add_phantom("decay_mode", None, desc="WW decay mode")
        self.boson = boson

    # Convert decay mode into appropriate Powheg parameters
    def finalise(self):
        # Calculate appropriate decay mode numbers
        __decay_code_lookup = {"e": -11, "mu": -13, "tau": -15, "emu": -113, "l": -135}
        if self.decorated.decay_mode not in self.decorated.allowed_decay_modes:
            logger.warning("Decay mode {0} not recognised!".format(self.decorated.decay_mode))
        __W_sign = [-1, +1]["WpWp" in self.decorated.decay_mode]
        __vdecaymodeW1, __vdecaymodeW2 = self.decorated.decay_mode.replace("WpWp", "").replace("WmWm", "").split("v")[:-1]
        __vdecaymodeW1 = __decay_code_lookup[__vdecaymodeW1] * __W_sign
        __vdecaymodeW2 = __decay_code_lookup[__vdecaymodeW2] * __W_sign

        self.decorated.fix_parameter("vdecaymode{}1".format(self.boson), __vdecaymodeW1, "(default process-dependent) First W decay mode")
        self.decorated.fix_parameter("vdecaymode{}2".format(self.boson), __vdecaymodeW2, "(default process-dependent) Second W decay mode")
