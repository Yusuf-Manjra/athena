# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl FixedScaleDecorator
#  Powheg runcard decorator for fixed scale
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class FixedScaleDecorator(object) :

  ## Define decorator name string
  name = 'fixed scale'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'fixedscale', -1, desc='(default -1, disabled) use reference renormalisation and factorisation scales' )
