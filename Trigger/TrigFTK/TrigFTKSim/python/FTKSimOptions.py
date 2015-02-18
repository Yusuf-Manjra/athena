# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses

def addTrigFTKSimOptions(parser,nsubregions=4):
    parser.defineArgGroup('TrigFTKSim', 'Fast tracker simulation generic options')
    parser.add_argument('--NBanks', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Number of pattern banks', group='TrigFTKSim')
    # Here we set a default value as the merger wants this explicitly
    parser.add_argument('--NSubRegions', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Number of sub-regions', group='TrigFTKSim', default=trfArgClasses.argInt(nsubregions, runarg=True))
    parser.add_argument('--pmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of pmap file', group='TrigFTKSim')
    parser.add_argument('--pmapunused_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of pmapunused file', group='TrigFTKSim')
    parser.add_argument('--pmapcomplete_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of 11L pmap file', group='TrigFTKSim')
    parser.add_argument('--rmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of rmap file', group='TrigFTKSim')

    parser.add_argument('--IBLMode',type=trfArgClasses.argFactory(trfArgClasses.argInt,runarg=True),
                        help='Enalbe the IBL geometry',group='TrigFTKSim')
                        
    # Cannot take maxEvents as an argument from addAthenaArguments() as it will have the wrong
    # default ('first', but we need 'all')    
    parser.add_argument('--maxEvents', group='TrigFTKSim', type=trfArgClasses.argFactory(trfArgClasses.argSubstepInt, runarg=True, defaultSubstep='all'), 
                        nargs='+', metavar='substep:maxEvents',
                        help='Set maximum events for each processing step (default for this transform is to set for all substeps)')

    parser.add_argument("--firstEvent", "--FirstEvent", group="TrigFTKSim",
                        default=trfArgClasses.argInt(0, runarg=True),
                        help="the number of the first event for processing",
                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True))

    parser.add_argument('--postExec', type=trfArgClasses.argFactory(trfArgClasses.argSubstepList), nargs='+',
                        metavar='substep:POSTEXEC', group='TrigFTKSim',
                        help='Python code to execute after main job options are included (can be optionally limited to a single substep)')

    parser.add_argument('--postInclude', group = 'TrigFTKSim', type=trfArgClasses.argFactory(trfArgClasses.argSubstepList, splitter=','), nargs='+',
                        metavar='substep:POSTINCLUDE',
                        help='Python configuration fragment to include after main job options (can be optionally limited to a single substep). Will split on commas: frag1.py,frag2.py is understood.')

    parser.add_argument('--preExec', group = 'TrigFTKSim', type=trfArgClasses.argFactory(trfArgClasses.argSubstepList), nargs='+',
                        metavar='substep:PREEXEC',
                        help='Python code to execute before main job options are included (can be optionally limited to a single substep)')

    parser.add_argument('--preInclude', group = 'TrigFTKSim', type=trfArgClasses.argFactory(trfArgClasses.argSubstepList, splitter=','), nargs='+',
                        metavar='substep:PREINCLUDE',
                        help='Python configuration fragment to include before main job options (can be optionally limited to a single substep). Will split on commas: frag1.py,frag2.py is understood.')




    #JDC:    
    parser.add_argument('--ConstantsDir', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Directory where input files are kept', group='TrigFTKSim')
    parser.add_argument('--FitConstantsVersion', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Version of fit constants', group='TrigFTKSim')
    parser.add_argument('--PatternsVersion', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Version of patterns', group='TrigFTKSim')
    parser.add_argument('--separateSubRegFitConst', default=0, type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Use separate fit constant for each subregion', group='TrigFTKSim')
    parser.add_argument('--useDBPath', default=0, type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Query the Data Base to get File Paths', group='TrigFTKSim')
    parser.add_argument('--runNum', default=0, type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Run Number', group='TrigFTKSim')
    parser.add_argument('--versionTag', default=0, type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='COOL tag for a different version', group='TrigFTKSim')
    #end JDC
    
    parser.add_argument('--FTKSetupTag', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Setup the FTK architecture tag, if specific values are also used they have priority', group='TrigFTKSim')

    return None

def addTrigFTKSimRFOptions(parser):
    parser.defineArgGroup('TrigFTKRoadFinder', 'Add FTK road finder options')
    parser.add_argument('--ssmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of ssmap file', group='TrigFTKRoadFinder')
    parser.add_argument('--ssmaptsp_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of ssmaptsp file', group='TrigFTKRoadFinder')
    parser.add_argument('--ssmapunused_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of ssmapunused file', group='TrigFTKRoadFinder')
    parser.add_argument('--bankpatterns', type=trfArgClasses.argFactory(trfArgClasses.argIntList, runarg=True), 
                        help='Number of bank patterns', group='TrigFTKRoadFinder', nargs='+')
    parser.add_argument("--patternbankspath",type=trfArgClasses.argFactory(trfArgClasses.argList,runarg=True),
                        help="String format describing the generic path for the bank like: patterns_reg{0}_sub{1}.root.",
                        group="TrigFTKRoadFinder",nargs='+')

    parser.add_argument('--badmap_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of badmap file', group='TrigFTKRoadFinder')
    parser.add_argument('--badmap_path_for_hit', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of badmap file for hits', group='TrigFTKRoadFinder')
    parser.add_argument('--UseTSPBank', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='TSP bank utilisation', group='TrigFTKRoadFinder')
    parser.add_argument('--DBBankLevel', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='DBBankLevel', group='TrigFTKRoadFinder')
    parser.add_argument('--TSPSimulationLevel', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='TSPSimulationLevel', group='TrigFTKRoadFinder')
    parser.add_argument('--TSPMinCoverage', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='TSPMinCoverage', group='TrigFTKRoadFinder')

    parser.add_argument('--DCMatchMethod',type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                         help="Set the DC matching method", group="TrigFTKRoadFinder") 
    
    parser.add_argument("--PixelClusteringMode",type=trfArgClasses.argFactory(trfArgClasses.argInt,runarg=True),
                        help="Set the pixel clustering mode: 0 default, 1 ToT+pixel center",group="TrigFTKRoadFinder")
    parser.add_argument('--SctClustering',type=trfArgClasses.argFactory(trfArgClasses.argBool,runarg=False),
                        help="Set the SCT clustering [def: False]", group="TrigFTKRoadFinder")

    parser.add_argument('--MakeCache', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Enable MakeCache', group='TrigFTKRoadFinder')
    parser.add_argument('--CachePath', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='CachePath', group='TrigFTKRoadFinder')
    parser.add_argument("--CachedBank", type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Interpret the pattern bank has a cache", group="TrigFTKRoadFinder")
    parser.add_argument("--SectorsAsPatterns", type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="If 1 allows to use a list of sectors as pattern bank, default 0", group="TrigFTKRoadFinder")

    parser.add_argument('--DoRoadFile', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Enable roads file', group='TrigFTKRoadFinder')
    parser.add_argument('--RoadFilesDir', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='RoadFilesDir', group='TrigFTKRoadFinder')
    parser.add_argument('--SaveRoads', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Save roads file', group='TrigFTKRoadFinder')
    parser.add_argument('--SetAMSize',type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help='This variable decides how to set the limit on the number of patterns: 0 TSP, 1 or 2 AM (as soon as limit reached, before exceeded)',
                        group='TrigFTKRoadFinder')

    return None


def addTrigFTKSimTFOptions(parser):
    parser.defineArgGroup('TrigFTKTrackFitter', 'Add FTK track fitter options')

    parser.add_argument("--fitconstantspath",type=trfArgClasses.argFactory(trfArgClasses.argList,runarg=True),
                        help="String format describing the generic path for the fitconstants like: patterns_reg{0}.root.",
                        group="TrigFTKTrackFitter",nargs='+')
    parser.add_argument("--sectorspath",type=trfArgClasses.argFactory(trfArgClasses.argList,runarg=True),
                        help="String format describing the generic path for the sectors like: patterns_reg{0}.root.",
                        group="TrigFTKTrackFitter",nargs='+')
    parser.add_argument("--fit711constantspath",type=trfArgClasses.argFactory(trfArgClasses.argList,runarg=True),
                        help="String format describing the generic path for the 2nd stage fit constants like: patterns_reg{0}.root.",
                        group="TrigFTKTrackFitter",nargs='+')

    parser.add_argument('--Save1stStageTrks', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Save 1st stage tracks', group="TrigFTKTrackFitter")

    parser.add_argument('--loadHWConf_path', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), 
                        help='Location of HW configuration file', group='TrigFTKTrackFitter')
    parser.add_argument('--HWNDiff', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="Hit Warrior threshold", group='TrigFTKTrackFitter')
    parser.add_argument('--HitWarrior', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
                        help="Hit Warrior type: 0 none, 1 local, 2 global (def)", group='TrigFTKTrackFitter')

    parser.add_argument('--SecondStageFit', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Enables the second stage fitter", group='TrigFTKTrackFitter')

    parser.add_argument("--Save1stStageTrks", type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help="Save the tracks after the first stage", group='TrigFTKTrackFitter')
    
    parser.add_argument('--TRACKFITTER_MODE', type=trfArgClasses.argFactory(trfArgClasses.argIntList, runarg=True), 
                        help='track fitter mode', group='TrigFTKTrackFitter', nargs='+')
    parser.add_argument('--SSFMultiConnection', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True), 
                        help='Flag to enable the multi-connections mode in the SSF', group='TrigFTKTrackFitter')
    parser.add_argument('--SSFNConnections', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Maximum number of connections in the SSF', group='TrigFTKTrackFitter')
    parser.add_argument('--SSFAllowExtraMiss', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='Allowing extra missed layer in the transition region', group='TrigFTKTrackFitter')
    parser.add_argument('--SSFTRDefn', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), 
                        help='How the transition region is defined: 0=moduleID of last SCT layer, 1=by eta of 1st stage track', group='TrigFTKSim')
    parser.add_argument('--SSFTRMinEta', type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='when SSFTRDefn=1 (by eta), the min eta', group='TrigFTKTrackFitter')
    parser.add_argument('--SSFTRMaxEta', type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True), 
                        help='when SSFTRDefn=1 (by eta), the max eta', group='TrigFTKTrackFitter')

    return None
    
        
def addTrigFTKSimMergeOptions(parser):
    parser.defineArgGroup('TrigFTKMerge', 'Fast tracker simulation merge options')
    parser.add_argument('--FTKUnmergedInputPath', 
                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='Unmerged Input file path', group='TrigFTKMerge')

    parser.add_argument('--SaveTruthTree',
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Save truth tree in merged output', group='TrigFTKMerge')

    parser.add_argument('--FTKForceAllInput', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Force all Input files to be present', group='TrigFTKMerge')

    parser.add_argument('--FTKDoGrid', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Use the naming for the grid input', group='TrigFTKMerge')
    return None

if __name__ == '__main__':
    print "TrigFTKSim transform parser manipulator module"
