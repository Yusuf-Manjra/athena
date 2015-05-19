# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_ttH
#  Powheg configuration for ttH subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from .. import ATLASCommonParameters

## Default Powheg configuration for ttH generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_ttH(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_ttH, self).__init__( runArgs, opts )
    self._powheg_executable += '/ttH/pwhg_main'

    ## Add process specific options
    self.add_parameter( 'delta_mttmin', 0, desc='(default 0d0) if not zero, use generation cut on m_tt' )

    ## Decorate with generic option sets
    self.add_parameter_set( 'fake virtual' )
    self.add_parameter_set( 'Higgs decay mode' )
    self.add_parameter_set( 'Higgs properties' )
    self.add_parameter_set( 'LHEv3' )
    self.add_parameter_set( 'running scales' )
    self.add_parameter_set( 'semileptonic' )
    self.add_parameter_set( 'top decay branching', tdec_prefix=False )
    self.add_parameter_set( 'top decay mode' )
    self.add_parameter_set( 'v2' )
    self.add_parameter_set( 'zero width' )

    ## Set optimised integration parameters
    self.ncall1   = 10000
    self.ncall2   = 10000
    self.nubound  = 10000

    ## Override defaults
    self.minlo        = -1
    self.topdecaymode = 00000 #22222 - decaying tops not supported at the moment
