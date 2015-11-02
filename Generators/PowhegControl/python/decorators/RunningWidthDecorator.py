# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl RunningWidthDecorator
#  Powheg runcard decorator for running width
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
class RunningWidthDecorator(object) :

  ## Define decorator name string
  name = 'running width'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'runningwidth', 0, desc='(default 0, disabled) use running width' )
