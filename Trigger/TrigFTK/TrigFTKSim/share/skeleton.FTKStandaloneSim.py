# FTK Simulation Transform Skeleton Job Options
# $Id: skeleton.FTKStandaloneSim.py 689768 2015-08-17 12:23:46Z jahreda $

from AthenaCommon.AthenaCommonFlags import jobproperties as jp
from AthenaCommon.Logging import logging
ftkLog = logging.getLogger('FTKStandaloneSimulation')
#ftkLog.propagate = False
ftkLog.info( '********** STARTING FTKStandaloneSimulation **********' )
ftkLog.info("Setting number of events: %d" % runArgs.maxEvents)
    
if hasattr(runArgs,"maxEvents"):
    theApp.EvtMax = runArgs.maxEvents
else:
    ftkLog.info("Running on all the events")
    theApp.EvtMax = -1

if hasattr(runArgs,"skipEvents"):
    theApp.SkipEvents = runArgs.skipEvents
else:
    ftkLog.info("Skipping zero events")
    theApp.SkipEvents = 0

from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

from PerfMonComps.PerfMonFlags import jobproperties as pmjp
pmjp.PerfMonFlags.doSemiDetailedMonitoring = True # to enable monitoring
##pmjp.PerfMonFlags.doFullMon = True # to enable monitoring

# --------------------------------------------------------------
# FTK algorithm inclusions
# --------------------------------------------------------------
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

from TrigFTKSim.TrigFTKSimConf import *
import os
import sys
from TrigFTKSim.findInputs import findInputs
from TrigFTKSim.QueryFTKdb import QueryDB

# Helper function from transforms 
from PyJobTransforms.trfUtils import findFile

# control some general behavior
FTKRoadMarket = FTK_RoadMarketTool()
ToolSvc += FTKRoadMarket

# Adding the roadfinder and trackfitter algorithms
FTKRoadFinder = FTKRoadFinderAlgo( "FTKRoadFinderAlgo" , OutputLevel=INFO)
FTKTrackFitter = FTKTrackFitterAlgo( "FTKTrackFitterAlgo" , OutputLevel=INFO)

# List here the runArgs which need to be translated directly into FTKRoadFinder attributes
runArgsFromTrfMandatory = ['NBanks', 'NSubRegions', 'pmap_path', 'bankregion','banksubregion','rmap_path','ssmap_path','ssmapunused_path','pmapunused_path']

runArgsFromTrfMandatoryRF = ['ssmaptsp_path', 'UseTSPBank', 'DBBankLevel', 'TSPSimulationLevel', 'patternbankpath', 'bankpatterns']

runArgsFromTrfMandatoryTF = ['fitconstantspath','fit711constantspath','sectorpath','loadHWConf_path','pmapcomplete_path']


# These arguments are optional - give a dict with their default values 
runArgsFromTrfOptionalRF = {'TSPMinCoverage': 0,
                            'RoadWarrior': 0,
                            'badmap_path': 'empty.bmap', 
                            'badmap_path_for_hit': '',
                            'MakeCache': False,
                            'CachePath': 'bankcache_reg%d_sub%d.root'%(runArgs.bankregion[0], runArgs.banksubregion[0]),
                            'CachedBank': False,
                            'DoRoadFile': True,
                            'RoadFilesDir': '.',
                            'SaveRoads': True,
                            'SetAMSize': 0,
                            'IBLMode': 0,
                            'PixelClusteringMode': 0,
                            'GangedPatternReco' : 0,
                            'DuplicateGanged' : 1,
                            'SctClustering': 0,
                            'MaxMissingSCTPairs': 1,
                            'RestrictSctPairModule': True,
                            'RestrictSctPairLayer': True,
                            'SectorsAsPatterns' : 0,
                            'DCMatchMethod': 1,
                            'HWModeSS': 0,
                            'ModuleLUTPath': ""
                            }

runArgsFromTrfOptionalTF = {'IBLMode': 0,
                            'Chi2Cut': 17,
                            'Chi2Cut_Maj': 14,
                            'Chi2DofCut': 4, # if >0 the previous values are ignored
                            'Chi2Cut_VetoMaj': -1, # no majority veto by default
                            'HitWarrior': 2,
                            'KeepRejected': 0,
                            'FitRemoved': 0,
                            'HWNDiff': 6,
                            'MaxNcomb': 1000000, 
                            'MaxNhitsPerPlane': -1, 
                            'doTrackFile': True,
                            'addRoads': True,
                            'TRACKFITTER_MODE': 1,
                            'SSFMultiConnection': False,
                            'SSFNConnections': 999,
                            'SSFAllowExtraMiss': 0,
                            'SSFTRDefn': 1,
                            'SSFTRMinEta': 1.0,
                            'SSFTRMaxEta': 1.4,
                            'Save1stStageTrks': False
                            }

#JDC:Find files from ConstantsDir, use to set certain attributes

attList = ["fit711constantspath","fitconstantspath","patternbankpath","sectorpath"]
reg = runArgs.bankregion
sub = runArgs.banksubregion

#Set things to defaults
if not hasattr(runArgs, "versionTag"):
    runArgs.versionTag = ""

#Use the databse to find the correct directory to look in
if hasattr(runArgs, 'useDBPath') and runArgs.useDBPath:
    print runArgs.useDBPath
    runArgs.ConstantsDir = QueryDB(runArgs.runNum,runArgs.versionTag)
	
        
    
#For each region in the list reg and for each subregion in the list sub find each of the needed files (fit constants, sector path, pattern banks) using findInputs and save the paths to the write member or runArgs
if hasattr(runArgs,"ConstantsDir") and runArgs.ConstantsDir:
    #do we have specific version strings? then construct directory pattern
    fitconstantsdir = 'FitConstants/%s/' % runArgs.FitConstantsVersion if hasattr(runArgs, 'FitConstantsVersion') else ''
    patternsdir = 'Patterns/%s/' % runArgs.PatternsVersion if hasattr(runArgs, 'PatternsVersion') else ''
    #For each kind of needed file, empty the associated variable
    for i in range(0,len(attList)):
        setattr(runArgs,attList[i],[])
    #For each region in the region list
    for i in range(0,len(reg)):
        #For each subregion in the subregion list
        for j in range(0,len(sub)):
            print fitconstantsdir, patternsdir
            patternList = [[fitconstantsdir, "1?L_*reg%s.*gcon.bz2" % (reg[i])],[fitconstantsdir, "8L_*reg%s.*gcon.bz2" % (reg[i])],[patternsdir, "pattern*reg%s_sub%s.*root" % (reg[i], sub[j])],[fitconstantsdir, "reg%s.*conn" % (reg[i])]]
            try:
                if runArgs.separateSubRegFitConst:
                    patternList = [[fitconstantsdir, "11L_*reg%s.*sub%s.*bz2" % (reg[i], sub[j])],[fitconstantsdir, "8L_*reg%s.*sub%s.*bz2" % (reg[i], sub[j])],[patternsdir, "pattern*reg%s.*sub%s.*root" % (reg[i], sub[j])],[fitconstantsdir, "reg%s.*sub%s.%conn" % (reg[i], sub[j])]]
            except AttributeError, e:
                pass
            #For each kind of file needed, add what you found to the list for that kind of file
            for k in range(0,len(attList)) :
                setattr(runArgs,attList[k],getattr(runArgs,attList[k])+findInputs(runArgs.ConstantsDir,patternList[k]))

#Just to make sure everything is set correctly
print
print runArgs
print

#End JDC
		

# this dictionary describe the standard position for common files, this allow
# a shorter command line when standard files are used
ftkBaseConfigDir = findFile(os.environ['DATAPATH'], 'ftk_configuration')
if ftkBaseConfigDir is None:
    ftkLog.warning('Failed to find ftk_configuration directory searching along $DATAPATH - all paths must now be given explicitly')
    standardConfBaseDir = {}
else:
    ftkLog.info('Found ftk_configuration directory {0}'.format(ftkBaseConfigDir))
    standardConfBaseDir = {'ssmap_path': os.path.join(ftkBaseConfigDir, 'ss_files'),
                           'ssmaptsp_path': os.path.join(ftkBaseConfigDir, 'ss_files'),
                           'ssmapunused_path': os.path.join(ftkBaseConfigDir, 'ss_files'),
                           'loadHWConf_path': os.path.join(ftkBaseConfigDir, 'hwsdev_file'),
                           'pmap_path': os.path.join(ftkBaseConfigDir, 'map_files'),
                           'pmapunused_path': os.path.join(ftkBaseConfigDir, 'map_files'),
                           'pmapcomplete_path': os.path.join(ftkBaseConfigDir, 'map_files'),
                           'rmap_path': os.path.join(ftkBaseConfigDir, 'map_files'),
                           'badmap_path': os.path.join(ftkBaseConfigDir, 'bad_modules'),
                           'badmap_path_for_hit': os.path.join(ftkBaseConfigDir, 'bad_modules'),
                           'ModuleLUTPath': os.path.join(ftkBaseConfigDir, 'map_files'), 
                           }

# Preliminary check that the most important arguments are set
if not hasattr(runArgs,"patternbankpath") or not hasattr(runArgs,"bankregion") or not hasattr(runArgs,"banksubregion") or not hasattr(runArgs,"fitconstantspath") :
    missingargs = []
    for item in ("patternbankpath","bankregion","banksubregion","fitconstantspath") :
        if not hasattr(runArgs,item) :
            missingargs += [item]
    raise RuntimeError, 'Failed to find mandatory runtime arguments for transform: ' + str(missingargs)

# consistency check on the size of most relevent arguments
NumberOfSubregions = len(runArgs.patternbankpath)
if NumberOfSubregions != len(runArgs.bankregion) or NumberOfSubregions != len(runArgs.banksubregion) or NumberOfSubregions != len(runArgs.fitconstantspath) :
    raise RuntimeError, 'Not consistent number of region, sub-region, patterns, and fit constants'

# prestore some common configurations that can be called through the option FTKSetupTag
FTKTagOptions = {}
FTKTagOptions["TDRv0"] = \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_8Lc.pmap', 'rmap_path': 'raw_11L.tmap', 'ssmap_path': 'raw_30x32x36.ss', 'ssmapunused_path': 'raw_unused.ss', 'pmapunused_path': 'raw_8Lc_unused.pmap', 'bankpatterns': [30000000]*NumberOfSubregions,\
         'ssmaptsp_path': 'raw_15x16x36.ss', 'UseTSPBank': True, 'DBBankLevel': 1, 'TSPSimulationLevel': 2, \
         'loadHWConf_path': 'raw_11L.hw','pmapcomplete_path': 'raw_11L.pmap', 'SecondStageFit': True, 'TRACKFITTER_MODE': 1}
FTKTagOptions["TDRv1"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_8Lc.pmap', 'rmap_path': 'raw_11L.tmap', 'ssmap_path': 'raw_30x32x72.ss', 'ssmapunused_path': 'raw_unused.ss', 'pmapunused_path': 'raw_8Lc_unused.pmap', 'bankpatterns': [4194304]*NumberOfSubregions, \
         'ssmaptsp_path': 'raw_15x16x36.ss', 'UseTSPBank': True, 'DBBankLevel': 1, 'TSPSimulationLevel': 2, \
         'loadHWConf_path': 'raw_11L.hw','pmapcomplete_path': 'raw_11L.pmap','SetAMSize': 2, 'SecondStageFit': True, 'TRACKFITTER_MODE': 1}

FTKTagOptions["TDAQTDRv0"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_8LcIbl123.pmap', 'rmap_path': 'raw_12Libl.tmap', 'ssmap_path': 'raw_30x32x72Ibl.ss', 'ssmapunused_path': 'raw_8LcIBL123_unusedmedium.ss', 'pmapunused_path': 'raw_8LcIbl123_unused.pmap', 'bankpatterns': [4194304]*NumberOfSubregions, \
         'ssmaptsp_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': True, 'DBBankLevel': 1, 'TSPSimulationLevel': 2, \
         'loadHWConf_path': 'raw_12L.hw','pmapcomplete_path': 'raw_12Libl.pmap','SetAMSize': 2, 'SecondStageFit': True, 'TRACKFITTER_MODE': 1, 'SSFMultiConnection': True, 'SSFNConnections': 4, \
         'IBLMode': 1, 'PixelClusteringMode': 1}
FTKTagOptions["TDAQTDRv1"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_8LcIbl123.pmap', 'rmap_path': 'raw_12Libl.tmap', 'ssmap_path': 'raw_30x32x72Ibl.ss', 'ssmapunused_path': 'raw_8LcIBL123_unusedmedium.ss', 'pmapunused_path': 'raw_8LcIbl123_unused.pmap', 'bankpatterns': [4194304]*NumberOfSubregions, \
         'ssmaptsp_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': True, 'DBBankLevel': 1, 'TSPSimulationLevel': 2, \
         'loadHWConf_path': 'raw_12L.hw','pmapcomplete_path': 'raw_12Libl.pmap','SetAMSize': 2, 'SecondStageFit': True, 'TRACKFITTER_MODE': 3, 'SSFMultiConnection': True, 'SSFNConnections': 4, \
         'SSFAllowExtraMiss': 0, 'SSFTRDefn': 1, 'SSFTRMaxEta': 1.4, 'SSFTRMinEta': 1.0, \
         'IBLMode': 1, 'PixelClusteringMode': 1}
FTKTagOptions["TDAQTDRv2"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_8LcIbl123.pmap', 'rmap_path': 'raw_12Libl.tmap', 'ssmap_path': 'raw_30x32x72Ibl.ss', 'ssmapunused_path': 'raw_8LcIBL123_unusedmedium.ss', 'pmapunused_path': 'raw_8LcIbl123_unused.pmap', 'bankpatterns': [4194304]*NumberOfSubregions, \
         'ssmaptsp_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': True, 'DBBankLevel': 1, 'TSPSimulationLevel': 2, \
         'loadHWConf_path': 'raw_12L.hw','pmapcomplete_path': 'raw_12Libl.pmap','SetAMSize': 2, 'SecondStageFit': True, 'TRACKFITTER_MODE': 3, 'SSFMultiConnection': True, 'SSFNConnections': 4, \
         'SSFAllowExtraMiss': 1, 'SSFTRDefn': 1, 'SSFTRMaxEta': 1.4, 'SSFTRMinEta': 1.0, \
         'IBLMode': 1, 'PixelClusteringMode': 1, 'GangedPatternReco': 0, 'DuplicateGanged': 1}

FTKTagOptions["Run2v0"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_8LcIbl3D123.pmap', 'rmap_path': 'raw_12Libl3D.tmap', 'ssmap_path': 'raw_30x32x72Ibl.ss', 'ssmapunused_path': 'raw_8LcIBL123_unusedmedium.ss', 'pmapunused_path': 'raw_8LcIbl123_unused.pmap', 'bankpatterns': [4194304]*NumberOfSubregions, \
         'ssmaptsp_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': True, 'DBBankLevel': 1, 'TSPSimulationLevel': 2, \
         'loadHWConf_path': 'raw_12L.hw','pmapcomplete_path': 'raw_12Libl3D.pmap','SetAMSize': 2, 'SecondStageFit': True, 'TRACKFITTER_MODE': 3, 'SSFMultiConnection': True, 'SSFNConnections': 4, \
         'SSFAllowExtraMiss': 1, 'SSFTRDefn': 1, 'SSFTRMaxEta': 1.4, 'SSFTRMinEta': 1.0, \
         'IBLMode': 2, 'PixelClusteringMode': 1, 'GangedPatternReco': 0, 'DuplicateGanged': 1}

#### Run2TempMapv0 is for Run 2 IBL with 3d modules, but tower map (raw_12Libl3DTempv0.tmap) is not good for data flow since it has expanded phi towers in both directions. There may also still be some issues in the mapping in eta
FTKTagOptions["Run2TempMapv0"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_8LcIbl3D123.pmap', 'rmap_path': 'raw_12Libl3DTempv0.tmap', 'ssmap_path': 'raw_30x32x72Ibl.ss', 'ssmapunused_path': 'raw_8LcIBL123_unusedmedium.ss', 'pmapunused_path': 'raw_8LcIbl123_unused.pmap', 'bankpatterns': [4194304]*NumberOfSubregions, \
         'ssmaptsp_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': True, 'DBBankLevel': 1, 'TSPSimulationLevel': 2, \
         'loadHWConf_path': 'raw_12L.hw','pmapcomplete_path': 'raw_12Libl3D.pmap','SetAMSize': 2, 'SecondStageFit': True, 'TRACKFITTER_MODE': 3, 'SSFMultiConnection': True, 'SSFNConnections': 4, \
         'SSFAllowExtraMiss': 1, 'SSFTRDefn': 1, 'SSFTRMaxEta': 1.4, 'SSFTRMinEta': 1.0, \
         'IBLMode': 2, 'PixelClusteringMode': 1, 'GangedPatternReco': 0, 'DuplicateGanged': 1}


FTKTagOptions["TDAQTDRv2HWMode2"] =  \
    {'NBanks': 64, 'NSubRegions': 4, 'pmap_path': 'raw_8LcIbl123.pmap', 'rmap_path': 'raw_12Libl.tmap', 'ssmap_path': 'raw_30x32x72Ibl.ss', 'ssmapunused_path': 'raw_8LcIBL123_unusedmedium.ss', 'pmapunused_path': 'raw_8LcIbl123_unused.pmap', 'bankpatterns': [4194304]*NumberOfSubregions, \
         'ssmaptsp_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': True, 'DBBankLevel': 1, 'TSPSimulationLevel': 2, \
         'loadHWConf_path': 'raw_12L.hw','pmapcomplete_path': 'raw_12Libl.pmap','SetAMSize': 2, 'SecondStageFit': False, 'TRACKFITTER_MODE': 3, 'SSFMultiConnection': True, 'SSFNConnections': 4, \
         'SSFAllowExtraMiss': 1, 'SSFTRDefn': 1, 'SSFTRMaxEta': 1.4, 'SSFTRMinEta': 1.0, \
         'IBLMode': 1, 'PixelClusteringMode': 1, 'GangedPatternReco': 0, 'DuplicateGanged': 1}

FTKTagOptions['SectorsAsPatterns3D'] = {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_8LcIbl3D123.pmap', 'rmap_path': 'raw_12Libl3D.tmap', 'ssmap_path': 'raw_30x32x72Ibl.ss', 'ssmapunused_path': 'raw_8LcIBL123_unusedmedium.ss', 'pmapunused_path': 'raw_8LcIbl3D123_unused.pmap', 'bankpatterns': [-1]*NumberOfSubregions, 'ssmaptsp_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': False, 'loadHWConf_path': 'raw_8Lc.hw','SecondStageFit': False, 'TRACKFITTER_MODE': 1, 'IBLMode': 2, 'PixelClusteringMode': 1, 'SectorsAsPatterns': 1 , 'SaveRoads': True}
FTKTagOptions['SectorsAsPatterns12L3D'] = {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_12Libl3D.pmap', 'rmap_path': 'raw_12Libl3D.tmap', 'bankpatterns': [-1]*NumberOfSubregions, 'ssmap_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': False, 'loadHWConf_path': 'raw_12L.hw','SecondStageFit': False, 'TRACKFITTER_MODE': 1, 'IBLMode': 2, 'PixelClusteringMode': 1, 'SectorsAsPatterns': 1 , 'SaveRoads': True}

FTKTagOptions['SectorsAsPatterns3Dv2'] = {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_8LcIbl3D123.pmap', 'rmap_path': 'raw_12Libl3Dv2.tmap', 'ssmap_path': 'raw_30x32x72Ibl.ss', 'ssmapunused_path': 'raw_8LcIBL123_unusedmedium.ss', 'pmapunused_path': 'raw_8LcIbl3D123_unused.pmap', 'bankpatterns': [-1]*NumberOfSubregions, 'ssmaptsp_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': False, 'loadHWConf_path': 'raw_8Lc.hw','SecondStageFit': False, 'TRACKFITTER_MODE': 1, 'IBLMode': 2, 'PixelClusteringMode': 1, 'SectorsAsPatterns': 1 , 'SaveRoads': True}

FTKTagOptions['SectorsAsPatterns12L3Dv2'] = {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_12Libl3D.pmap', 'rmap_path': 'raw_12Libl3Dv2.tmap', 'bankpatterns': [-1]*NumberOfSubregions, 'ssmap_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': False, 'loadHWConf_path': 'raw_12L.hw','SecondStageFit': False, 'TRACKFITTER_MODE': 1, 'IBLMode': 2, 'PixelClusteringMode': 1, 'SectorsAsPatterns': 1 , 'SaveRoads': True}

FTKTagOptions['SectorsAsPatterns'] = \
    {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_8LcIbl123.pmap', 'rmap_path': 'raw_12Libl.tmap', 'ssmap_path': 'raw_30x32x72Ibl.ss', 'ssmapunused_path': 'raw_8LcIBL123_unusedmedium.ss', 'pmapunused_path': 'raw_8LcIbl123_unused.pmap', 'bankpatterns': [-1]*NumberOfSubregions, \
         'ssmaptsp_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': False, \
         'loadHWConf_path': 'raw_8Lc.hw','SecondStageFit': False, 'TRACKFITTER_MODE': 1,
         'IBLMode': 1, 'PixelClusteringMode': 1, 'SectorsAsPatterns': 1 , 'SaveRoads': True}
FTKTagOptions['SectorsAsPatterns12L'] = \
    {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_12Libl.pmap', 'rmap_path': 'raw_12Libl.tmap', 'bankpatterns': [-1]*NumberOfSubregions, \
     'ssmap_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': False, \
     'loadHWConf_path': 'raw_12L.hw','SecondStageFit': False, 'TRACKFITTER_MODE': 1,
     'IBLMode': 1, 'PixelClusteringMode': 1, 'SectorsAsPatterns': 1 , 'SaveRoads': True}
FTKTagOptions['SectorsAsPatterns12Lb'] = \
    {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_12LiblHW.pmap', 'rmap_path': 'raw_12Libl.tmap', 'bankpatterns': [-1]*NumberOfSubregions, \
     'ssmap_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': False, \
     'loadHWConf_path': 'raw_12L.hw','SecondStageFit': False, 'TRACKFITTER_MODE': 1,
     'IBLMode': 1, 'PixelClusteringMode': 1, 'SectorsAsPatterns': 1 , 'SaveRoads': True}


FTKTagOptions['SectorsAsPatternsHWMode2'] = \
    {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_8LcIbl123.pmap', 'rmap_path': 'raw_12Libl.tmap', 
     'bankpatterns': [-1]*NumberOfSubregions, \
     'ssmap_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': False, \
     'loadHWConf_path': 'raw_8Lc.hw','SecondStageFit': False, 
     'IBLMode': 1, 'PixelClusteringMode': 1, 'SectorsAsPatterns': 1,
     'MaxMissingSCTPairs': 0, 
     'HWModeSS': 2,'ModuleLUTPath': 'raw_8LcIbl123.moduleidmap', 'SaveRoads': True}
FTKTagOptions['SectorsAsPatterns12LHWMode2'] = \
    {'NBanks': 64, 'NSubRegions': 1, 'pmap_path': 'raw_12LiblHW.pmap', 'rmap_path': 'raw_12Libl64TmodB.tmap', 
     'bankpatterns': [-1]*NumberOfSubregions, \
     'ssmap_path': 'raw_15x16x36Ibl.ss', 'UseTSPBank': False, \
     'loadHWConf_path': 'raw_12L.hw','SecondStageFit': False, 
     'IBLMode': 1, 'PixelClusteringMode': 1, 'SectorsAsPatterns': 1,
     'MaxMissingSCTPairs': 0, 
     'HWModeSS': 2,'ModuleLUTPath': 'raw_12Libl.moduleidmap', 'SaveRoads': True}
    
# enable the "Scenario" runarg that sets other runarg values as consequence
if hasattr(runArgs, 'FTKSetupTag'):
    ftktag = getattr(runArgs,'FTKSetupTag')
    
    ftkLog.info("A FTKSetupTag=%s has been requested, default values will be set if no specific value is used" % ftktag)

    # loop over the keys to be set
    for k, v in FTKTagOptions[ftktag].iteritems() :
        if hasattr(runArgs,k) :
            info = "Running argument already set by the user: %s = %s" % (k, str(getattr(runArgs,k)))
        else :
            info = "Running argument set: %s = %s" %(k, str(v))
            setattr(runArgs,k,v)
        ftkLog.info(info)
        

skipArgs = []

if hasattr(runArgs,"SaveRoads") :
  ftkLog.info("Roads storing flag set to %s" % runArgs.SaveRoads)
  FTKRoadMarket.SaveRoads = runArgs.SaveRoads
else :
  ftkLog.info("Roads are not saved, default value")
  FTKRoadMarket.SaveRoads = False

# Normalize some mandatory options removing few of them according 
if hasattr(runArgs,'UseTSPBank') :
    if not runArgs.UseTSPBank :
        skipArgs += ['DBBankLevel','TSPSimulationLevel','ssmaptsp_path']
else :
    skipArgs += ['DBBankLevel','TSPSimulationLevel','ssmaptsp_path']

if not hasattr(runArgs,'SecondStageFit') :
   skipArgs += ['pmapunused_path','fit711constantspath','sectorpath','pmapcomplete_path','ssmapunused_path']
   FTKTrackFitter.SecondStageFit = False
elif not getattr(runArgs,'SecondStageFit') :
   skipArgs += ['pmapunused_path','fit711constantspath','sectorpath','pmapcomplete_path','ssmapunused_path']
   FTKTrackFitter.SecondStageFit = False
else :
   # Second stage case
   FTKTrackFitter.SecondStageFit = True
    
# set the FTKRoadFinder properties
for runArgName in runArgsFromTrfMandatory + runArgsFromTrfMandatoryRF + runArgsFromTrfOptionalRF.keys():
   ftkLog.info('Setting FTKRoadFinder argument %s' % runArgName)
   
   if runArgName in skipArgs:
      ftkLog.info("Skipping " + runArgName)
      continue
  
   if hasattr(runArgs, runArgName):
       if runArgName in standardConfBaseDir:
           ftkLog.info("Using preferential path \"%s\" for %s" % (standardConfBaseDir[runArgName],runArgName))
           setattr(FTKRoadFinder, runArgName, os.path.join(standardConfBaseDir[runArgName], getattr(runArgs, runArgName)))
       else:
           setattr(FTKRoadFinder, runArgName, getattr(runArgs, runArgName))
   elif runArgName in runArgsFromTrfOptionalRF:
       setattr(FTKRoadFinder, runArgName, runArgsFromTrfOptionalRF[runArgName])
   else:
       raise RuntimeError, 'Failed to find mandatory FTKRoadFinder runtime argument for transform %s' % runArgName


# Set the PerfMon output file name according the numbers of region and subregion
# of the first bank, additional are ingored
pmjp.PerfMonFlags.OutputFile = 'ntuple_FTKSim_Tower%dSub%d.pmon.gz' % (runArgs.bankregion[0], runArgs.banksubregion[0])

## Pre-exec
if hasattr(runArgs,"preExec"):
  ftkLog.info("transform pre-exec")
  for cmd in runArgs.preExec:
    ftkLog.info(cmd)
    exec(cmd)
 
## Pre-include
if hasattr(runArgs,"preInclude"): 
  for fragment in runArgs.preInclude:
      include(fragment)

# Look for the wrapper files
if hasattr(runArgs, 'inputTXT_FTKIPFile'):
  FTKRoadFinder.InputFromWrapper = True
  FTKRoadFinder.WrapperFiles = runArgs.inputTXT_FTKIPFile
elif hasattr(runArgs, 'inputNTUP_FTKIPFile'):
  FTKRoadFinder.InputFromWrapper = True
  FTKRoadFinder.RegionalWrapper = True
  FTKRoadFinder.WrapperFiles = runArgs.inputNTUP_FTKIPFile
  if hasattr(runArgs, "firstEventFTK"):
    FTKRoadFinder.FirstEventFTK = runArgs.firstEventFTK
elif hasattr(runArgs, 'inputRDOFile') :
  FTKRoadFinder.InputFromWrapper = False
  FTKSGInput = FTK_SGHitInput( maxEta= 3.2, minPt= 0.8*GeV)
  #FTKSGInput.OutputLevel = DEBUG
  #FTKSGInput.ReadTruthTracks = True
  ToolSvc += FTKSGInput

  include("PATJobTransforms/CommonSkeletonJobOptions.py")
  from RecExConfig.RecFlags import rec
  rec.readRDO.set_Value_and_Lock( True )
  globalflags.InputFormat.set_Value_and_Lock('pool')
  athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDOFile )

  #include("RecExCommon/RecExCommon_topOptions.py")
  if True :
    ServiceMgr.EventSelector.InputCollections = runArgs.inputRDOFile
    # ConditionsTag
    from IOVDbSvc.CondDB import conddb
    if len(globalflags.ConditionsTag())!=0:
      conddb.setGlobalTag(globalflags.ConditionsTag())
    include("RecExCond/AllDet_detDescr.py")
    # Particle Property
    protectedInclude( "PartPropSvc/PartPropSvc.py" )
    include.block( "PartPropSvc/PartPropSvc.py" )

    # Detector Status                                             
    if rec.doDetStatus() and not athenaCommonFlags.isOnline():
      try:
        include("DetectorStatus/DetStatusSvc_CondDB.py")                  
      except Exception:
        treatException("Could not load DetStatusSvc_CondDb !")
      rec.doFileMetaData=False
else :
    raise RuntimeError('No FTK inputs given. A TXT_FTKIP, NTUP_FTKIP or RDO file is mandatory.')

# Set output file
if hasattr(runArgs,'outputNTUP_FTKTMPFile'):
   FTKTrackFitter.TrackFileName = runArgs.outputNTUP_FTKTMPFile
else:
   ftkLog.warning('FTK Sim output filename was unset')

#check if we want to save 1st stage tracks
if hasattr(runArgs,'Save1stStageTrks'):
   FTKTrackFitter.Save1stStageTrks = runArgs.Save1stStageTrks

# check if the second stage is going to be used
if not hasattr(runArgs,'doAuxFW'):
  FTKTrackFitter.doAuxFW = False
else: FTKTrackFitter.doAuxFW = getattr(runArgs,'doAuxFW')


# set the FTKTrackFitter properties
for runArgName in runArgsFromTrfMandatory + runArgsFromTrfMandatoryTF + runArgsFromTrfOptionalTF.keys():
   ftkLog.info('Setting FTKTrackFitter argument %s' % runArgName)

   if runArgName in skipArgs:
      ftkLog.info("Skipping " + runArgName)
      continue

   if hasattr(runArgs, runArgName):
       if runArgName in standardConfBaseDir:
           ftkLog.info("Using preferintial path \"%s\" for %s" % (standardConfBaseDir[runArgName],runArgName))
           setattr(FTKTrackFitter,runArgName,  os.path.join(standardConfBaseDir[runArgName], getattr(runArgs, runArgName)))
       else:
           setattr(FTKTrackFitter, runArgName, getattr(runArgs, runArgName))
   elif runArgName in runArgsFromTrfOptionalTF:
      setattr(FTKTrackFitter, runArgName, runArgsFromTrfOptionalTF[runArgName])
   else:
      raise RuntimeError, 'Failed to find mandatory FTKTrackFitter runtime argument for transform %s' % runArgName


alg += FTKRoadFinder   
alg += FTKTrackFitter   

if not FTKTrackFitter.doTrackFile :
   from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
   Stream1 = AthenaPoolOutputStream( "Stream1" )
   Stream1.OutputFile =   "FTKSimulationOutput.pool.root"
   Stream1.ItemList += ["FTKAthTrackContainer#*","FTKTrackFitterStats#*","TrigInDetTrackCollection#*"]

## Post-include
if hasattr(runArgs,"postInclude"): 
  for fragment in runArgs.postInclude:
    include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
  ftkLog.info("transform post-exec")
  for cmd in runArgs.postExec:
    ftkLog.info(cmd)
    exec(cmd)
