# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HeavyQuarkDecorator
#  Powheg runcard decorator for heavy quarks
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python

class HeavyQuarkDecorator :

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.quark_mass = -1


  def append_to_run_card( self ) :
    ## Write decorations to runcard
    with open( self.decorated.runcard_path(), 'a' ) as f :
      f.write( 'qmass '+str(self.decorated.quark_mass)+' ! mass of heavy quark in GeV\n' )
