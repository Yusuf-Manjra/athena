# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl WDecayModeDecorator
#  Powheg runcard decorator for W decay mode
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class WDecayModeDecorator(object) :

  ## Define decorator name string
  name = 'W decay mode'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'wdecaymode', 11111, desc='(default 11111) 5 digits indicating the maximum number of (e,mu,tau,u,c) allowed in decays' )

