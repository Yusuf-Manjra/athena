# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @package PyJobTransforms.trfFileUtils
# @brief Transform utilities to deal with files.
# @details Mainly used by argFile class.
# @author atlas-comp-transforms-dev@cern.ch
# @version $Id: trfFileUtils.py 602496 2014-06-18 19:31:32Z graemes $
# @todo make functions timelimited

import unittest

from subprocess import Popen,PIPE

import logging
msg = logging.getLogger(__name__)

#  @note Use the PyCmt forking decorator to ensure that ROOT is run completely within 
#  a child process and will not 'pollute' the parent python process with unthread-safe
#  bits of code (otherwise strange hangs are observed on subsequent uses of ROOT)
import PyUtils.Decorators as _decos

from PyUtils.RootUtils import import_root
from PyJobTransforms.trfDecorators import timelimited

## @note The 'AODFixVersion' is can appear for AOD or ESD files 
athFileInterestingKeys = ['beam_energy', 'beam_type', 'conditions_tag', 'file_size',
                          'file_guid', 'file_type', 'geometry', 'lumi_block', 'nentries', 'run_number', 
                          'AODFixVersion']



## @brief Determines metadata of BS, POOL or TAG file.
#  @details Trivial wrapper around PyUtils.AthFile.
#  @param fileName Path(s) to the file for which the metadata are determined
#  @param retrieveKeys Keys to extract from the @c AthFile.infos dictionary
#  @return 
#  - Dictionary containing metadata of the file(s)
#  - @c None if the determination failed.
@timelimited()
def AthenaFileInfo(fileNames, retrieveKeys = athFileInterestingKeys):
    msg.debug('Calling AthenaFileInfo for {0}'.format(fileNames))

    from PyUtils import AthFile
    AthFile.server.flush_cache()
    AthFile.server.disable_pers_cache()

    if isinstance(fileNames, str):
        fileNames = [fileNames,]

    metaDict = {}
    try:
        ## @note This code is transitional, until all the versions of AthFile we
        #  use support pfopen(). It should then be removed. Graeme, 2013-11-05.
        #  Note to Future: Give it 6 months, then get rid of it!
        if len(fileNames) > 1:
            try:    
                athFile = AthFile.pfopen(fileNames)
            except AttributeError:  
                msg.warning('This version of AthFile does not support "pfopen". Falling back to serial interface.')
                athFile = AthFile.fopen(fileNames)
        else:
            athFile = AthFile.fopen(fileNames)
        msg.debug('AthFile came back')
        for fname, meta in zip(fileNames, athFile):
            metaDict[fname] = {}
            for key in retrieveKeys:
                msg.debug('Looking for key {0}'.format(key))
                try:
                    # AODFix is tricky... it is absent in many files, but this is not an error
                    if key is 'AODFixVersion':
                        if 'tag_info' in meta.infos and isinstance('tag_info', dict) and 'AODFixVersion' in meta.infos['tag_info']:
                            metaDict[fname][key] = meta.infos['tag_info'][key]
                        else:
                            metaDict[fname][key] = ''
                    # beam_type seems odd for RAW - typical values seem to be [1] instead of 'collisions' or 'cosmics'.
                    # So we use the same scheme as AutoConfiguration does, mapping project names to known values
                    # It would be nice to import this all from AutoConfiguration, but there is no suitable method at the moment.
                    # N.B. This is under discussion so this code is temporary fix (Captain's Log, Stardate 2012-11-28) 
                    elif key is 'beam_type':
                        try:
                            if isinstance(meta.infos[key], list) and len(meta.infos[key]) > 0 and meta.infos[key][0] in ('cosmics' ,'singlebeam','collisions'):
                                metaDict[fname][key] = meta.infos[key]
                            else:
                                from RecExConfig.AutoConfiguration import KnownCosmicsProjects, Known1BeamProjects, KnownCollisionsProjects, KnownHeavyIonProjects
                                if 'bs_metadata' in meta.infos.keys() and isinstance(meta.infos['bs_metadata'], dict) and 'Project' in meta.infos['bs_metadata'].keys():
                                    project = meta.infos['bs_metadata']['Project']
                                elif 'tag_info' in meta.infos.keys() and isinstance(meta.infos['tag_info'], dict) and 'project_name' in meta.infos['tag_info'].keys():
                                    project = meta.infos['tag_info']['project_name']
                                else:
                                    msg.info('AthFile beam_type was not a known value ({0}) and no project could be found for this file'.format(meta.infos[key]))
                                    metaDict[fname][key] = meta.infos[key]
                                    continue
                                if project in KnownCollisionsProjects or project in KnownHeavyIonProjects:
                                    metaDict[fname][key] = ['collisions']
                                    continue
                                if project in KnownCosmicsProjects:
                                    metaDict[fname][key] = ['cosmics']
                                    continue
                                if project in Known1BeamProjects:
                                    metaDict[fname][key] = ['singlebeam']
                                    continue
                                # Erm, so we don't know
                                msg.info('AthFile beam_type was not a known value ({0}) and the file\'s project ({1}) did not map to a known beam type using AutoConfiguration'.format(meta.infos[key], project))
                                metaDict[fname][key] = meta.infos[key]
                        except Exception, e:
                            msg.error('Got an exception while trying to determine beam_type: {0}'.format(e))
                            metaDict[fname][key] = meta.infos[key]
                    else:
                        metaDict[fname][key] = meta.infos[key]
                except KeyError:
                    msg.warning('Missing key in athFile info: {0}'.format(key))
            msg.debug('Found these metadata for {0}: {1}'.format(fname, metaDict[fname].keys()))
        return metaDict
    except ValueError, e:
        msg.error('Problem in getting AthFile metadata for {0}'.format(fileNames))
        return None
        

## @brief Determines number of events in a HIST file.
#  @details Basically taken from PyJobTransformsCore.trfutil.MonitorHistFile
#  @param fileName Path to the HIST file.  
#  @return 
#  - Number of events.
#  - @c None if the determination failed.
#  @note Use the PyCmt forking decorator to ensure that ROOT is run completely within 
#  a child process and will not 'pollute' the parent python process with unthread-safe
#  bits of code (otherwise strange hangs are observed on subsequent uses of ROOT)
@_decos.forking
def HISTEntries(fileName):

    root = import_root()

    file = root.TFile.Open(fileName, 'READ')
    
    if not (isinstance(file, root.TFile) and file.IsOpen()):
        return None

    rundir = None
    keys = file.GetListOfKeys()
    
    for key in keys:
        
        name=key.GetName()
        
        if name.startswith('run_') and name is not 'run_multiple':
            
            if rundir is not None:
                msg.warning('Found two run_ directories in HIST file %s: %s and %s' % ( fileName, rundir, name) )
                return None
            else:
                rundir = name
                
        del name
       
    if rundir is None:
        msg.warning( 'Unable to find run directory in HIST file %s' % fileName )
        file.Close()
        return None
    
    msg.info( 'Using run directory %s for event counting of HIST file %s. ' % ( rundir, fileName ) )
    
    hpath = '%s/GLOBAL/DQTDataFlow/events_lb' % rundir
    possibleLBs = []
    if 'tmp.HIST_' in fileName:
        msg.info( 'Special case for temporary HIST file {0}. '.format( fileName ) )
        h = file.Get('{0}'.format(rundir))
        for directories in h.GetListOfKeys() :
            if 'lb' in directories.GetName():
                msg.info( 'Using {0} in tmp HIST file {1}. '.format(directories.GetName(),  fileName ) )
                hpath = rundir+'/'+str(directories.GetName())+'/GLOBAL/DQTDataFlow/events_lb' 
                possibleLBs.append(hpath)
    else:
        msg.info( 'Classical case for HIST file {0}. '.format( fileName ) )
        possibleLBs.append(hpath)
    nev = 0
    if len(possibleLBs) == 0:
        msg.warning( 'Unable to find events_lb histogram in HIST file %s' % fileName )
        file.Close()
        return None
    for hpath in possibleLBs:
        h = file.Get(hpath)
        
        if not isinstance( h, root.TH1 ):
            msg.warning( 'Unable to retrieve %s in HIST file %s.' % ( hpath, fileName ) )
            file.Close()
            return None
        
        nBinsX = h.GetNbinsX()
        nevLoc = 0
        
        for i in xrange(1, nBinsX):
            
            if h[i] < 0:
                msg.warning( 'Negative number of events for step %s in HIST file %s.' %( h.GetXaxis().GetBinLabel(i), fileName ) )
                file.Close()
                return None
            
            elif h[i] == 0:
                continue
            
            if nevLoc == 0:
                nevLoc = h[i]
                
            else:
                if nevLoc != h[i]:
                    msg.warning( 'Mismatch in events per step in HIST file %s; most recent step seen is %s.' % ( fileName, h.GetXaxis().GetBinLabel(i) ) )
                    file.Close()
                    return None
        nev += nevLoc        
    file.Close()
    return nev



## @brief Determines number of entries in NTUP file with given tree names.
#  @details Basically taken from PyJobTransformsCore.trfutil.ntup_entries.
#  @param fileName Path to the NTUP file.
#  @param treeNames Tree name or list of tree names.
#  In the latter case it is checked if all trees contain the same number of events  
#  @return 
#  - Number of entries.
#  - @c None if the determination failed.
#  @note Use the PyCmt forking decorator to ensure that ROOT is run completely within 
#  a child process and will not 'pollute' the parent python process with unthread-safe
#  bits of code (otherwise strange hangs are observed on subsequent uses of ROOT)
@_decos.forking
def NTUPEntries(fileName, treeNames):
    
    if not isinstance( treeNames, list ):
        treeNames=[treeNames]
        
    root = import_root()
    
    file = root.TFile.Open(fileName, 'READ')
    
    if not (isinstance(file, root.TFile) and file.IsOpen()):
        return None
    
    prevNum=None
    prevTree=None
               
    for treeName in treeNames:
            
        tree = file.Get(treeName)
            
        if not isinstance(tree, root.TTree):
            return None
        
        num = tree.GetEntriesFast()

        if not num>=0:
            msg.warning('GetEntriesFast returned non positive value for tree %s in NTUP file %s.' % ( treeName, fileName ))
            return None
                
        if prevNum is not None and prevNum != num:
            msg.warning( "Found diffferent number of entries in tree %s and tree %s of file %s." % ( treeName, prevTree, fileName  ))
            return None
        
        numberOfEntries=num
        prevTree=treeName
        del num
        del tree

    file.Close()

    return numberOfEntries


## @brief Get the size of a file via ROOT's TFile
#  @details Use TFile.Open to retrieve a ROOT filehandle, which will
#  deal with all non-posix filesystems. Return the GetSize() value.
#  The option filetype=raw is added to ensure this works for non-ROOT files too (e.g. BS)
#  @note Use the PyCmt forking decorator to ensure that ROOT is run completely within 
#  a child process and will not 'pollute' the parent python process with unthread-safe
#  bits of code (otherwise strange hangs are observed on subsequent uses of ROOT)
#  @param filename Filename to get size of
#  @return fileSize or None of there was a problem
@_decos.forking
def ROOTGetSize(filename):
    root = import_root()
    
    try:
        msg.debug('Calling TFile.Open for {0}'.format(filename))
        file = root.TFile.Open(filename + '?filetype=raw', 'READ')
        fsize = file.GetSize()
        msg.debug('Got size {0} from TFile.GetSize'.format(fsize))
    except ReferenceError:
        msg.error('Failed to get size of {0}'.format(filename))
        return None
    
    file.Close()
    del root
    return fsize
    

## @brief Return the LAN access type for a file URL
#  @param filename Name of file to examine
#  @return
#  - String with LAN protocol
def urlType(filename):
    if filename.startswith('dcap:'):
        return 'dcap'
    if filename.startswith('root:'):
        return 'root'
    if filename.startswith('rfio:'):
        return 'rfio'
    if filename.startswith('file:'):
        return 'posix'
    return 'posix'

