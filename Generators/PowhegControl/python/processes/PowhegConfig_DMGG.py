# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_DMGG
#  Powheg configuration for DMGG subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

## Default Powheg configuration for DMGG generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_DMGG(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_DMGG, self).__init__( runArgs, opts )
    self._powheg_executable += '/DMGG/pwhg_main'

    ## Add process specific options
    self.add_parameter( 'Lambda', 100,    desc='(default 100) UV scale', parameter='DMLambda' )
    self.add_parameter( 'nloformfact', 1, desc='(default 1, enabled) include Higgs-specific corrections into GGS and GGP form factor' )

    ## Decorate with generic option sets
    self.add_parameter_set( 'CKKW' )
    self.add_parameter_set( 'dark matter' )
    self.add_parameter_set( 'dark matter phi' )
    self.add_parameter_set( 'LHEv3' )
    self.add_parameter_set( 'mass window' )
    self.add_parameter_set( 'running scale' )
    self.add_parameter_set( 'running width' )
    self.add_parameter_set( 'v2' )

    ## Set optimised integration parameters
    self.ncall1   = 2000000
    self.itmx1    = 10
    self.ncall2   = 100000
    self.nubound  = 100000
    self.foldx    = 2
    self.foldy    = 2

    ## Override defaults
    self.bornktmin    = 100.0
    self.doublefsr    = 1
    self.minlo        = -1
    self.runningscale = 3
