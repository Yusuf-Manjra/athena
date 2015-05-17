#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## FTK Merge + Reco Transform
#  Merge all FTK regions and reconstruct using the original RDOs
# @version $Id: TrigFTKMergeRecoNew_tf.py 615859 2014-09-10 09:05:30Z baines $ 

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments, addD3PDArguments, addDetectorArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

from SimuJobTransforms.simTrfArgs import addBasicDigiArgs

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses

from RecJobTransforms.recTransformUtils import addCommonRecTrfArgs

ListOfDefaultPositionalKeys=['--AMI', '--AMITag', '--AddCaloDigi', '--DBRelease', '--DataRunNumber', '--FTKDoGrid', '--FTKForceAllInput', '--FTKOldFormat', '--FTKRefitMode', '--FTKSetupTag', '--FTKUnmergedInputPath', '--FirstRegion', '--FirstSubreg', '--HWNDiff', '--HitWarrior', '--MergeFromTowers', '--NBanks', '--NSubRegions', '--asetup', '--athena', '--athenaopts', '--autoConfiguration', '--beamType', '--checkEventCount', '--command', '--conditionsTag', '--digiRndmSvc', '--digiSeedOffset1', '--digiSeedOffset2', '--doAllNoise', '--eventAcceptanceEfficiency', '--execOnly', '--geometryVersion', '--ignoreErrors', '--ignoreFilters', '--ignorePatterns', '--inputDESD_FTKFile', '--inputHitsFile', '--inputNTUP_FTKFile', '--inputNTUP_FTKTMPFile', '--inputRDOFile', '--jobNumber', '--loadHWConf_path', '--maxEvents', '--omitFileValidation', '--outputDESD_FTKFile', '--outputNTUP_FTKFile', '--outputNTUP_TRIGFile', '--outputRDOFile', '--pmap_path', '--postExec', '--postInclude', '--preExec', '--preInclude', '--reportName', '--samplingFractionDbTag', '--showGraph', '--showPath', '--showSteps', '--skipEvents', '--tmpRDO', '--triggerConfig', '--uploadtoami', '--validation']


@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    
    msg.info('This is %s' % sys.argv[0])
        
    trf = getTransform() 
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)


## Get the base transform with all arguments added
def getTransform():
    
    executorSet = set()
    executorSet.add(athenaExecutor(name = 'FTKSimulationMerge', 
                                   skeletonFile = 'TrigFTK_RawDataAlgs/skeleton.FTKStandaloneMergeToRDO.py',
                                   inData = ['NTUP_FTKTMP'],
                                   outData = ['NTUP_FTK'],))
    executorSet.add(athenaExecutor(name = 'FTKRecoRDOtoESD', skeletonFile = 'RecJobTransforms/skeleton.RAWtoESD.py',
                                   substep = 'r2e', inData = [('RDO', 'NTUP_FTK')], outData = ['DESD_FTK'], 
                                   perfMonFile = 'ntuple_RAWtoESD.pmon.gz',))
#                                   extraRunargs = {'preInclude': ['TrigFTK_RawDataAlgs/FTK_RDO_creator_jobOptions.py']}))
    executorSet.add(athenaExecutor(name = 'FTKRecoESDtoNTUP', skeletonFile = 'PyJobTransforms/skeleton.ESDtoDPD.py',
                                   substep = 'e2d', inData = ['DESD_FTK'], outData = ['NTUP_TRIG'], 
                                   perfMonFile = 'ntuple_ESDtoDPD.pmon.gz',
                                   ))
#                                   extraRunargs = {'preInclude': ['TrigFTKSim/FTKReco_jobOptions.py']}))

    trf = transform(executor = executorSet, description = 'FTK full region merge and reco.')

    addAthenaArguments(trf.parser, maxEventsDefaultSubstep='all')
    addDetectorArguments(trf.parser)
    addCommonRecTrfArgs(trf.parser)
    addFTKSimulationArgs(trf.parser)
    addD3PDArguments(trf.parser, pick=['NTUP_TRIG'], transform=trf)
    addBasicDigiArgs(trf.parser)
    return trf


def addFTKSimulationArgs(parser):
    # Add a specific FTK argument group
    parser.defineArgGroup('TrigFTKSim', 'Fast tracker simulation options')
    parser.defineArgGroup('TrigFTKMerge', 'Fast tracker simulation merge options')
    parser.defineArgGroup('TrigFTKReco', 'Fast tracker reco options')

    parser.add_argument('--NBanks', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Number of pattern banks', group='TrigFTKSim')
    parser.add_argument('--NSubRegions', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Number of sub-regions', group='TrigFTKSim')

    parser.add_argument('--pmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of pmap file', group='TrigFTKSim')
    parser.add_argument('--loadHWConf_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of HW configuration file', group='TrigFTKSim')
    parser.add_argument('--HWNDiff', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="Hit Warrior threshold", group='TrigFTKSim')
    parser.add_argument('--HitWarrior', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="Hit Warrior type: 0 no HW, 1 in sector, 2 global", group='TrigFTKSim')

    parser.add_argument('--FTKSetupTag', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Setup the FTK architecture tag, if specific values are also used they have priority', group='TrigFTKSim')
    
    parser.add_argument('--FirstRegion', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='First ID of the region (def: 0)')
    parser.add_argument('--FirstSubreg', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='First ID of the subregions (def: 0)')
    
    # File handling
    parser.add_argument('--inputNTUP_FTKTMPFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftk', treeNames='ftkdata'),
                        help='Set of FTK tracks merged to region level', group='TrigFTKMerge',nargs='+')
    parser.add_argument('--inputRDOFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input', runarg=True, type='rdo'),
                        help='Input RDO file', group='TrigFTKReco', nargs='+')
    parser.add_argument('--inputNTUP_FTKFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftk', treeNames='ftkdata'),
                        help='FTK merged tracks file', group='TrigFTKReco',nargs='+')
    parser.add_argument('--FTKOldFormat', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Interprets the NTUP_FTK file as coming from the old standalone simulation', group='TrigFTKReco')
    parser.add_argument('--inputDESD_FTKFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input', runarg=True, type='ftk'),
                        help='Input FTK ESD file', group='TrigFTKReco', nargs='+')
    parser.add_argument('--outputNTUP_FTKFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftk', treeNames='ftkdata'),
                        help='FTK merged tracks file', group='TrigFTKReco')
    parser.add_argument('--outputDESD_FTKFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output', runarg=True, type='ftk'),
                        help='Output FTK ESD file', group='TrigFTKReco')    

        
    parser.add_argument('--FTKUnmergedInputPath', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Unmerged Input file path', group='TrigFTKMerge')
    parser.add_argument('--FTKForceAllInput', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Force all Input files to be present', group='TrigFTKMerge')
    parser.add_argument('--FTKDoGrid', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Use the naming for the grid input', group='TrigFTKMerge')
    parser.add_argument('--MergeFromTowers',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="This option can be used to merge NTUP_FTK files produced from a previous partial merging job", group="TrigFTKMerge")

    
    parser.add_argument('--FTKRefitMode',
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help="TK refit scheme during the reconstruction", group="TrigFTKReco")

if __name__ == '__main__':
    main()
