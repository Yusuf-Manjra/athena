#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
@file Util.py
@brief Helper functions for CostAnalysisPostProcessing script
'''

import ROOT
from math import fabs
from AthenaCommon.Logging import logging
log = logging.getLogger('CostAnalysisPostProcessing')


def saveMetadata(inputFile):
    import json

    metatree = inputFile.Get("metadata")
    if metatree is None:
        return None

    metatree.GetEntry(0)
    metadata = []

    metadata.append({'runNumber' : metatree.runNumber})
    metadata.append({'AtlasProject' : str(metatree.AtlasProject)})
    metadata.append({'AtlasVersion' : str(metatree.AtlasVersion)})

    metadata.append({'ChainMonitor' : metatree.ChainMonitor})
    metadata.append({'AlgorithmMonitor' : metatree.AlgorithmMonitor})
    metadata.append({'AlgorithmClassMonitor' : metatree.AlgorithmClassMonitor})
    metadata.append({'ROSMonitor' : metatree.ROSMonitor})
    metadata.append({'GlobalsMonitor' : metatree.GlobalsMonitor})
    metadata.append({'ThreadMonitor' : metatree.ThreadMonitor})

    metadata.append({'AdditionalHashMap' : str(metatree.AdditionalHashMap)})
    metadata.append({'DoEBWeighting' : metatree.DoEBWeighting})
    metadata.append({'BaseEventWeight' : metatree.BaseEventWeight})

    metadata.append({'HLTMenu' : json.loads(str(metatree.HLTMenu))})

    with open('metadata.json', 'w') as outMetaFile:
        metafile = {}
        metafile['text'] = 'metadata'
        metafile['children'] = metadata
        json.dump(obj=metafile, fp=outMetaFile, indent=2, sort_keys=True)


def exploreTree(inputFile):
    ''' @brief Explore ROOT Tree to find tables with histograms to be saved in csv

    Per each found directory TableConstructor object is created.
    Expected directory tree:
        rootDir
            table1Dir
                entry1Dir
                    hist1
                    hist2
                    ...
                entry2Dir
                    hist1
                    ...
            table2Dir
            ...
            walltimeHist
    
    @param[in] inputFile ROOT.TFile object with histograms
    '''


    for key in inputFile.GetListOfKeys():
        walltime = getWalltime(inputFile, key.GetName())
        obj = key.ReadObj()
        if not obj.IsA().InheritsFrom(ROOT.TDirectory.Class()): continue

        for table in obj.GetListOfKeys():
            tableObj = table.ReadObj()
            if not tableObj.IsA().InheritsFrom(ROOT.TDirectory.Class()): continue

            log.info("Processing Table %s", table.GetName())
            # Find and create Table Constructor for specific Table
            try:
                className = table.GetName() + "_TableConstructor"
                exec("from TrigCostAnalysis." + className + " import " + className)
                t = eval(className + "(tableObj)")

                if table.GetName() == "Chain_HLT":
                    t.totalTime = getAlgorithmTotalTime(inputFile, obj.GetName())

                fileName = getFileName(table.GetName(), key.GetName())
                histPrefix = getHistogramPrefix(table.GetName(), key.GetName())

                t.fillTable(histPrefix)
                t.normalizeColumns(walltime)
                t.saveToFile(fileName)
 
            except (NameError, ImportError):
                log.warning("Class {0} not defined - directory {1} will not be processed"
                            .format(table.GetName()+"_TableConstructor", table.GetName()))


def getWalltime(inputFile, rootName):
    ''' @brief Extract walltime value from histogram
    
    @param[in] inputFile ROOT TFile to look for histogram
    @param[in] rootName Name of the root directory to search for tables

    @return walltime value if found else 0 and an error
    '''

    dirObj = inputFile.Get(rootName)
    if not dirObj.IsA().InheritsFrom(ROOT.TDirectory.Class()): return 0
    for hist in dirObj.GetListOfKeys():
        if '_walltime' in hist.GetName():
            obj = hist.ReadObj()
            return obj.GetBinContent(1)

    log.error("Walltime not found")
    return 0


def getAlgorithmTotalTime(inputFile, rootName):
    ''' @brief Extract total time [s] of algorithms from histogram
    
    @param[in] inputFile ROOT TFile to look for histogram
    @param[in] rootName Name of the root directory to search for tables

    @return total execution time [s] value if found else 0
    '''

    totalTime = 0
    alg = inputFile.Get(rootName).Get("Global_HLT").Get("All")
    hist = alg.Get(rootName + "_Global_HLT_All_AlgTime_perEvent")
    for i in range(1, hist.GetXaxis().GetNbins()):
        totalTime += hist.GetBinContent(i) * hist.GetXaxis().GetBinCenterLog(i)

    return totalTime * 1e-3

def convert(entry):
    ''' @brief Save entry number in scientific notation'''
    if type(entry) is float or type(entry) is int:
        # Avoid scientific notation for small numbers and 0
        if entry == 0:
            return 0
        elif fabs(entry) > 10000 or fabs(entry) < 0.0001:
            return "{:.4e}".format(entry)
        elif int(entry) == entry:
            # Get rid of unnecessary 0
            return int(entry)
        else:
            return "{:.4}".format(entry)

    return entry


def getFileName(tableName, rootName):
    '''@brief Get name of file to save the table

    @param[in] tableName Table name
    @param[in] rootName  Name of table's root directory

    @return Filename for given table
    '''
    return "Table_" + tableName + "_" + rootName + ".csv"


def getHistogramPrefix(tableName, rootName):
    '''@brief Construct full histogram name

    @param[in] tableName Table name
    @param[in] rootName  Name of table's root directory

    @return Histogram prefix for given table
    '''

    return rootName + '_' + tableName + '_'