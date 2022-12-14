# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import logging
msg = logging.getLogger(__name__)

from PyJobTransforms.trfArgClasses import argFactory, argPOOLFile, argBool, argString, argInt, argFile

## Add FCS Ntuple transform arguments
def addFCS_NtupArgs(parser):
    parser.defineArgGroup('FCS_Ntup_tf', 'FCS_Ntup_tf specific options')
    parser.add_argument('--inputESDFile', nargs = '+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input ESD files', group='FCS_Ntup_tf')
    parser.add_argument('--outputNTUP_FCSFile', nargs = '+',
                        type=argFactory(argFile, io='output'),
                        help='Output NTUP_FCS files', group='FCS_Ntup_tf')
    parser.add_argument('--outputGeoFileName',
                        type=argFactory(argString),
                        help='Optional geometry file', group='FCS_Ntup_tf')
    parser.add_argument('--doG4Hits',
                        type=argFactory(argBool),
                        help='Flag to turn on the writing of G4 hits', group='FCS_Ntup_tf')
    parser.add_argument('--saveAllBranches',
                        type=argFactory(argBool),
                        help='Flag to turn on extra branches for Hits and G4hits', group='FCS_Ntup_tf')
    parser.add_argument('--NTruthParticles',
                        type=argFactory(argInt),
                        help='Option to select number of truth particles copied to ntuples', group='FCS_Ntup_tf')
    parser.add_argument('--doClusterInfo',
                        type=argFactory(argBool),
                        help='Flag to turn on extra branches for clusters', group='FCS_Ntup_tf')
