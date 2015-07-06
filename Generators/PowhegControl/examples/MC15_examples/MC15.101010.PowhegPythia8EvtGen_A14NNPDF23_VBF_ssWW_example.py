# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Powheg VBF_ssWW setup starting from ATLAS defaults
#--------------------------------------------------------------
include('PowhegControl/PowhegControl_VBF_ssWW_Common.py')
PowhegConfig.generate()

#--------------------------------------------------------------
# Pythia8 showering with the A14 NNPDF2.3 tune
#--------------------------------------------------------------
include('MC15JobOptions/Pythia8_A14_NNPDF23LO_EvtGen_Common.py')
include('MC15JobOptions/Pythia8_Powheg.py')

#--------------------------------------------------------------
# EVGEN configuration
#--------------------------------------------------------------
evgenConfig.description = 'POWHEG+Pythia8 VBF same-sign WW production with A14 NNPDF2.3 tune'
evgenConfig.keywords    = [ 'SM', 'diboson', 'WW', 'VBF' ]
evgenConfig.contact     = [ 'james.robinson@cern.ch' ]
