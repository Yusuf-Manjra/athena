# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg ZZ subprocess
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#            Daniel Hayden   <danhayden0@googlemail.com>
#            Stephen Bieniek <stephen.paul.bieniek@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
from PowhegConfig_base import PowhegConfig_base
import PowhegDecorators

###############################################################################
#
#  ZZ
#
###############################################################################
class PowhegConfig_ZZ(PowhegConfig_base) :
  # These are process specific - put generic properties in PowhegConfig_base
  cutallpairs      = -1
  mllmin           = 4

  # Set process-dependent paths in the constructor
  def __init__( self, runArgs=None ) :
    super(PowhegConfig_ZZ, self).__init__(runArgs)
    self._powheg_executable += '/ZZ/pwhg_main'

    # Add decorators
    PowhegDecorators.decorate( self, 'diboson' )
    PowhegDecorators.decorate( self, 'fixed scale' )
    PowhegDecorators.decorate( self, 'v2' )

    # Set optimised integration parameters
    self.ncall1   = 50000
    self.ncall2   = 50000 #100000
    self.nubound  = 100000 #100000
    self.xupbound = 10
    self.itmx1    = 5
    self.itmx2    = 14

    # Override defaults
    self.allowed_decay_modes = [ 'semileptonic', 'e-mu', 'e-tau', 'mu-tau', 'leptonic', 'leptons-nu', 'hadrons-nu', 'only-e', 'only-mu', 'only-tau' ]
    self.decay_mode = 'only-e'
    self._use_delta_lambda = False

  # Implement base-class function
  def generateRunCard(self) :
    self.initialiseRunCard()

    with open( str(self.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'cutallpairs '+str(self.cutallpairs)+'   ! \n' )
      f.write( 'mllmin '+str(self.mllmin)+'             ! Minimum invariant mass of lepton pairs from Z decay\n' )
