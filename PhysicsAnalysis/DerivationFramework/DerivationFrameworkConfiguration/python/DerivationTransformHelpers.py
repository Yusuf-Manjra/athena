"""Main derivation transform configuration helpers

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from PyJobTransforms.trfArgClasses import argFactory, argList, argNTUPFile, argPOOLFile, argSubstepBool
from PyJobTransforms.trfExe import athenaExecutor, reductionFrameworkExecutor

def addDerivationArguments(parser):
    """Add common derivation command-line parser arguments."""
    parser.defineArgGroup('Derivation', 'Derivation Options')
    parser.add_argument('--formats', nargs='*',
                        type=argFactory(argList),
                        help='List of required D(2)AOD formats',
                        group='Derivation')
    parser.add_argument('--inputAODFile', nargs='+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input AOD for DAOD building',
                        group='Derivation')
    parser.add_argument('--inputDAOD_PHYSFile', nargs='+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input DAOD_PHYS for D2AOD building',
                        group='Derivation')
    parser.add_argument('--inputEVNTFile', nargs='+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input EVNT for DAOD_TRUTHX building',
                        group='Derivation')
    parser.add_argument('--outputDAODFile', nargs='+',
                        type=argFactory(argPOOLFile, io='output'),
                        help='Output DAOD filename stub, DAOD_X will be prepended to it',
                        group='Derivation')
    parser.add_argument('--outputD2AODFile', nargs='+',
                        type=argFactory(argPOOLFile, io='output'),
                        help='Output D2AOD filename stub, D2AOD_X will be prepended to it',
                        group='Derivation')
    parser.add_argument('--passThrough',
                        type=argFactory(argSubstepBool, defaultSubstep = 'Derivation', runarg=True),
                        metavar='BOOL',
                        help='Disable all skimming and write every event',
                        group='Derivation')


def addPhysicsValidationArguments(parser):
    """Add validation command-line parser arguments."""
    parser.defineArgGroup('Physics Validation', 'Physics validation options')
    parser.add_argument('--validationFlags', nargs='+',
                        type=argFactory(argList),
                        help='Physics validation histogram switches',
                        group='Physics Validation')
    parser.add_argument('--inputDAOD_PHYSVALFile', nargs='+',
                        type=argFactory(argPOOLFile, io='input'),
                        help='Input DAOD_PHYSVAL for validation ntuples building',
                        group='Physics Validation')
    parser.add_argument('--outputNTUP_PHYSVALFile',
                        type=argFactory(argNTUPFile, io='output'),
                        help='Output physics validation file',
                        group='Physics Validation')


def addDerivationSubstep(executor_set):
    # We use the existing DF executor which inherits from the athenaExecutor.
    # It handles the composite output filenames and inserts them into the transform's dataDictionary.
    # If this isn't done the file validation will run over the wrong file name.
    executor = reductionFrameworkExecutor(name='Derivation',
                                          skeletonFile=None,
                                          skeletonCA='DerivationFrameworkConfiguration.DerivationSkeleton',
                                          substep='DerivationFramework',
                                          tryDropAndReload=False,
                                          perfMonFile='ntuple.pmon.gz',
                                          inData=['EVNT', 'AOD', 'DAOD_PHYS'],
                                          outData=['DAOD', 'D2AOD'])
    executor_set.add(executor)


def addPhysicsValidationSubstep(executor_set):
    executor = athenaExecutor(name='PhysicsValidation',
                              skeletonFile=None,
                              skeletonCA='DerivationFrameworkConfiguration.PhysicsValidationSkeleton',
                              substep='PhysicsValidation',
                              tryDropAndReload=False,
                              perfMonFile='ntuple.pmon.gz',
                              inData=['DAOD_PHYSVAL'],
                              outData=['NTUP_PHYSVAL'])
    executor_set.add(executor)
