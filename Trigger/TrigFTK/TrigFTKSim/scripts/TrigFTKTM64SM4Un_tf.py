#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## FTK Simulation Transform
#  Specialist version to do sim x 4 subregions and merge in one job
# @version $Id: TrigFTKSM4Un_tf.py 579246 2014-01-20 17:48:57Z gvolpi $ 

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses

ntowers = 64
subregions = 4

ListOfDefaultPositionalKeys=['--AMI', '--CachePath', '--CachedBank', '--DBBankLevel', '--DoRoadFile', '--FTKDoGrid', '--FTKForceAllInput', '--FTKSetupTag', '--FTKUnmergedInputPath', '--HWNDiff', '--HitWarrior', '--IBLMode', '--MakeCache', '--NBanks', '--NSubRegions', '--PixelClusteringMode', '--RoadFilesDir', '--SSFAllowExtraMiss', '--SSFMultiConnection', '--SSFNConnections', '--SSFTRDefn', '--SSFTRMaxEta', '--SSFTRMinEta', '--SaveRoads', '--SctClustering', '--SecondStageFit', '--SetAMSize', '--TRACKFITTER_MODE', '--TSPMinCoverage', '--TSPSimulationLevel', '--UseTSPBank', '--badmap_path', '--badmap_path_for_hit', '--bankpatterns', '--bankregion', '--execOnly', '--fit711constantspath', '--fitconstantspath', '--ignoreErrors', '--inputNTUP_FTKIPFile', '--inputNTUP_FTKTMP_0File', '--inputNTUP_FTKTMP_1File', '--inputNTUP_FTKTMP_2File', '--inputNTUP_FTKTMP_3File', '--inputTXT_FTKIPFile', '--loadHWConf_path', '--maxEvents', '--omitFileValidation', '--outputNTUP_FTKTMPFile', '--outputNTUP_FTKTMP_0File', '--outputNTUP_FTKTMP_1File', '--outputNTUP_FTKTMP_2File', '--outputNTUP_FTKTMP_3File', '--patternbank0path', '--patternbank1path', '--patternbank2path', '--patternbank3path', '--pmap_path', '--pmapcomplete_path', '--pmapunused_path', '--reportName', '--rmap_path', '--sectorpath', '--showGraph', '--showPath', '--showSteps', '--ssmap_path', '--ssmaptsp_path', '--ssmapunused_path', '--uploadtoami', '--validation']


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
    for tower in range(ntowers) :
      for subregion in range(subregions):
        executorSet.add(athenaExecutor(name = 'FTKFullSimulationBank{0}Sub{1}'.format(tower,subregion), 
              skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneSim.py',
              inData = ['NTUP_FTKIP','TXT_FTKIP'], outData = ['NTUP_FTKTMP_{0}_{1}'.format(tower,subregion)],
              extraRunargs = {'bankregion': [tower],'banksubregion': [subregion]},
              # Need to ensure that the correct subregion is used
              runtimeRunargs = {'patternbankpath': '[runArgs.patternbanksfmtpath.format({0},{1})]'.format(tower,subregion),
               'fitconstantspath': '[runArgs.fitconstantsfmtpath.format({0})]'.format(tower),
               'fit711constantspath': '[runArgs.fit711constantsfmtpath.format({0})]'.format(tower),
               'sectorpath': '[runArgs.sectorsfmtpath.format({0})]'.format(tower),
               'bankregion': [tower], 'subregions': [subregion],
              'outputNTUP_FTKTMPFile': 'runArgs.outputNTUP_FTKTMP_{0}_{1}File'.format(tower,subregion)}))
      executorSet.add(athenaExecutor(name = 'FTKSimulationMerge{0}'.format(tower), 
            skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneMerge.py',
            inData = [tuple([ 'NTUP_FTKTMP_{0}_{1}'.format(tower,subregion) for subregion in range(subregions) ])],
            outData = ['NTUP_FTKTMP_{0}'.format(tower)],
            extraRunargs = {'inputNTUP_FTKTMPFile': [ 'tmp.NTUP_FTKTMP_{0}_{1}'.format(tower,subregion) for subregion in range(subregions)]},
            runtimeRunargs = {'MergeRegion': tower,
            'FirstRegion': tower,
            'outputNTUP_FTKTMPFile': 'runArgs.outputNTUP_FTKTMP_{0}File'.format(tower)},
            ))
    # add final merge for all the tower, generating a NTUP_FTK file
    executorSet.add(athenaExecutor(name = "FTKSimulationMergeFinal",
          skeletonFile = 'TrigFTKSim/skeleton.FTKStandaloneMerge.py',
          inData = [tuple([ 'NTUP_FTKTMP_{0}'.format(tower) for tower in range(ntowers) ])],
          outData = ['NTUP_FTK'],
          extraRunargs = {'inputNTUP_FTKTMPFile': [ 'tmp.NTUP_FTKTMP_{0}'.format(tower) for tower in range(ntowers)]},
          runtimeRunargs = {'MergeFromTowers': True,
          'FirstRegion': 0},
          ))

    trf = transform(executor = executorSet, description = 'FTK simulation for {0} towers, with {1} subregions merge and final merge.'.format(ntowers,subregions))

#    addAthenaArguments(trf.parser)
    addFTKSimulationArgs(trf.parser)
    return trf


def addFTKSimulationArgs(parser):
    # Add a specific FTK argument group
    parser.defineArgGroup('TrigFTKSim', 'Fast tracker simulation options')
    parser.defineArgGroup('TrigFTKMerge', 'Fast tracker simulation merge options')
    
    parser.add_argument('--NBanks', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Number of pattern banks', group='TrigFTKSim')
    # Here we set a default value as the merger wants this explicitly
    parser.add_argument('--NSubRegions', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Number of sub-regions', group='TrigFTKSim', default=trfArgClasses.argInt(subregions, runarg=True))

    # Cannot take maxEvents as an argument from addAthenaArguments() as it will have the wrong
    # default ('first', but we need 'all')
    parser.add_argument('--maxEvents', group='TrigFTKSim', type=trfArgClasses.argFactory(trfArgClasses.argSubstepInt, runarg=True, defaultSubstep='all'), 
                        nargs='+', metavar='substep:maxEvents',
                        help='Set maximum events for each processing step (default for this transform is to set for all substeps)')


    parser.add_argument('--pmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of pmap file', group='TrigFTKSim')
    parser.add_argument('--pmapunused_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of pmapunused file', group='TrigFTKSim')
    parser.add_argument('--pmapcomplete_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of 11L pmap file', group='TrigFTKSim')
    parser.add_argument('--rmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of rmap file', group='TrigFTKSim')
    parser.add_argument('--loadHWConf_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of HW configuration file', group='TrigFTKSim')
    parser.add_argument('--HWNDiff', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
        help="Hit Warrior threshold", group='TrigFTKSim')
    parser.add_argument('--HitWarrior', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
        help="Hit Warrior type: 0 none, 1 local, 2 global (def)", group='TrigFTKSim')

    parser.add_argument('--ssmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of ssmap file', group='TrigFTKSim')
    parser.add_argument('--ssmaptsp_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of ssmaptsp file', group='TrigFTKSim')
    parser.add_argument('--ssmapunused_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of ssmapunused file', group='TrigFTKSim')
    
    parser.add_argument('--badmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of badmap file', group='TrigFTKSim')
    parser.add_argument('--badmap_path_for_hit', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of badmap file for hits', group='TrigFTKSim')
    parser.add_argument('--UseTSPBank', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='TSP bank utilisation', group='TrigFTKSim')
    parser.add_argument('--DBBankLevel', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='DBBankLevel', group='TrigFTKSim')
    parser.add_argument('--TSPSimulationLevel', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='TSPSimulationLevel', group='TrigFTKSim')
    parser.add_argument('--TSPMinCoverage', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='TSPMinCoverage', group='TrigFTKSim')

    parser.add_argument('--IBLMode',type=trfArgClasses.argFactory(trfArgClasses.argInt,runarg=True),
                        help='Enalbe the IBL geometry',group='TrigFTKSim')
    
    parser.add_argument("--PixelClusteringMode",type=trfArgClasses.argFactory(trfArgClasses.argInt,runarg=True),
                        help="Set the pixel clustering mode: 0 default, 1 ToT+pixel center",group="TrigFTKSim")
    parser.add_argument('--SctClustering',type=trfArgClasses.argFactory(trfArgClasses.argBool,runarg=False),
                        help="Set the SCT clustering [def: False]", group="TrigFTKSim")

    parser.add_argument('--MakeCache', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Enable MakeCache', group='TrigFTKSim')
    parser.add_argument('--CachePath', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='CachePath', group='TrigFTKSim')
    parser.add_argument("--CachedBank", type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Interpret the pattern bank has a cache", group="TrigFTKSim")
    
    parser.add_argument('--DoRoadFile', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Enable roads file', group='TrigFTKSim')
    parser.add_argument('--RoadFilesDir', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='RoadFilesDir', group='TrigFTKSim')
    parser.add_argument('--SaveRoads', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Save roads file', group='TrigFTKSim')
    parser.add_argument('--bankpatterns', type=trfArgClasses.argFactory(trfArgClasses.argIntList, runarg=True), 
                        help='Number of bank patterns', group='TrigFTKSim', nargs='+')
    parser.add_argument('--SetAMSize',type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='This variable decides how to set the limit on the number of patterns: 0 TSP, 1 or 2 AM (as soon as limit reached, before exceeded)',
                        group='TrigFTKSim')
    parser.add_argument('--SecondStageFit', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Enables the second stage fitter", group='TrigFTKSim')

    parser.add_argument('--TRACKFITTER_MODE', type=trfArgClasses.argFactory(trfArgClasses.argIntList, runarg=True), 
                        help='track fitter mode', group='TrigFTKSim', nargs='+')
    parser.add_argument('--SSFMultiConnection', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Flag to enable the multi-connections mode in the SSF', group='TrigFTKSim')
    parser.add_argument('--SSFNConnections', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Maximum number of connections in the SSF', group='TrigFTKSim')
    parser.add_argument('--SSFAllowExtraMiss', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Allowing extra missed layer in the transition region', group='TrigFTKSim')
    parser.add_argument('--SSFTRDefn', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='How the transition region is defined: 0=moduleID of last SCT layer, 1=by eta of 1st stage track', group='TrigFTKSim')
    parser.add_argument('--SSFTRMinEta', type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='when SSFTRDefn=1 (by eta), the min eta', group='TrigFTKSim')
    parser.add_argument('--SSFTRMaxEta', type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='when SSFTRDefn=1 (by eta), the max eta', group='TrigFTKSim')

    
    parser.add_argument("--patternbanksfmtpath",type=trfArgClasses.argFactory(trfArgClasses.argString,runarg=True),
                        help="String format describing the generic path for the bank like: patterns_reg{0}_sub{1}.root.",
                        group="TrigFTKSim")
    parser.add_argument("--fitconstantsfmtpath",type=trfArgClasses.argFactory(trfArgClasses.argString,runarg=True),
                        help="String format describing the generic path for the fitconstants like: patterns_reg{0}.root.")
    parser.add_argument("--sectorsfmtpath",type=trfArgClasses.argFactory(trfArgClasses.argString,runarg=True),
                        help="String format describing the generic path for the sectors like: patterns_reg{0}.root.",
                        group="TrigFTKSim")
    parser.add_argument("--fit711constantsfmtpath",type=trfArgClasses.argFactory(trfArgClasses.argString,runarg=True),
                        help="String format describing the generic path for the 2nd stage fit constants like: patterns_reg{0}.root.",
                        group="TrigFTKSim")

    
    parser.add_argument('--FTKSetupTag', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Setup the FTK architecture tag, if specific values are also used they have priority', group='TrigFTKSim')
    

    # File handling
    parser.add_argument('--inputNTUP_FTKIPFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftkiptmp', treeNames='ftkhits'),
                        help='FTK RDO file in ROOT format', group='TrigFTKSim', nargs='+')
    parser.add_argument('--inputTXT_FTKIPFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argFTKIPFile, runarg=True, io='input', type='txt_ftkip'), 
                        help='Wrapper files (in .dat.bz2 format)', group='TrigFTKSim', nargs='+')
    
    # The following for testing only
    for tower in range(ntowers) :
      parser.add_argument('--outputNTUP_FTKTMP_{0}File'.format(tower), 
                    type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftk', treeNames='ftkdata'),
                    help='Subregion {0} merged FTK file (for test only)'.format(tower), group='TrigFTKMerge')

      for subregion in range(subregions):
          parser.add_argument('--inputNTUP_FTKTMP_{0}_{1}File'.format(tower,subregion), 
                          type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='input', type='ntup_ftkiptmp', treeNames='ftkdata'),
                          help='FTK NTUP file from tower {0} subregion {1} (for testing only)'.format(tower,subregion), group='TrigFTKSim')
          parser.add_argument('--outputNTUP_FTKTMP_{0}_{1}File'.format(tower,subregion), 
                          type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftkiptmp', treeNames='ftkdata'),
                          help='FTK NTUP file from tower {0} subregion {1} (for testing only)'.format(tower,subregion), group='TrigFTKSim')
        
    parser.add_argument('--outputNTUP_FTKFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argNTUPFile, runarg=True, io='output', type='ntup_ftk', treeNames='ftkdata'),
                        help='FTK merged tracks file', group='TrigFTKReco')        
    parser.add_argument('--FTKUnmergedInputPath', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Unmerged Input file path', group='TrigFTKMerge')

    parser.add_argument('--FTKForceAllInput', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Force all Input files to be present', group='TrigFTKMerge')

    parser.add_argument('--FTKDoGrid', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Use the naming for the grid input', group='TrigFTKMerge')
    

if __name__ == '__main__':
    main()
