# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_Wt_DR
#  Powheg configuration for Wt_DR subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base
from .. import ATLASCommonParameters

## Default Powheg configuration for Wt_DR generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_Wt_DR(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_Wt_DR, self).__init__( runArgs, opts )
    self._powheg_executable += '/ST_wtch_DR/pwhg_main'

    ## Decorate with generic option sets
    self.add_parameter_set( 'CKM' )
    self.add_parameter_set( 'lepton mass' )
    self.add_parameter_set( 'sin**2 theta W' )
    self.add_parameter_set( 'single top' )
    self.add_parameter_set( 'top decay branching' )
    self.add_parameter_set( 'top decay mode' )
    self.add_parameter_set( 'top decay second generation quark' )
    self.add_parameter_set( 'top mass' )
    self.add_parameter_set( 'W decay mode' )

    ## Set optimised integration parameters
    self.ncall1  = 50000
    self.ncall2  = 50000
    self.nubound = 50000

    ## Override defaults
    self.topdecaymode = 11111
